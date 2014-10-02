#ifndef _LCDKEYPAD_H_
#define _LCDKEYPAD_H_
/* LCD and Keypad support for sjfw -
 * (c) 2011, Christopher "ScribbleJ" Jansen
 *
 * This class provides a system of LCD menus navigated by button presses on an attached keypad.
 *
 */

#include "config.h"

#ifdef HAS_LCD
#include "Temperature.h"
#include "GCode.h"
#include "GcodeQueue.h"
#include "Motion.h"
#include "Globals.h"

#include <Wire.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>

#ifdef HAS_SD
#include <SD.h>
#endif

// These bits of global data are used by the LCD and Keypad handler classes.
#ifdef HAS_KEYPAD
#include "Keypad.h"
extern const char *_kp_buttonmap[];
extern const int _kp_rpins[];
extern const int _kp_cpins[];
#endif
extern uint8_t _lcd_linestarts[];
extern float const RATES_OF_CHANGE[];

class LCDKeypad
{
public:
  enum MODE { TEMP, SDSELECT, MOTORS, MENU, MODS };


  LCDKeypad() 
  : LCD(
#ifdef LCD_I2C_ADDR
        LCD_I2C_ADDR,
        LCD_E_PIN,LCD_RW_PIN,LCD_RS_PIN,
        LCD_4_PIN,LCD_5_PIN,LCD_6_PIN,LCD_7_PIN
#else
        LCD_RS_PIN, LCD_RW_PIN, LCD_E_PIN,
        LCD_0_PIN, LCD_1_PIN, LCD_2_PIN, LCD_3_PIN,
        LCD_4_PIN, LCD_5_PIN, LCD_6_PIN, LCD_7_PIN,
        LCD_X, LCD_Y, _lcd_linestarts
#endif
        )
#ifdef HAS_KEYPAD        
    ,KEYPAD(_kp_cpins, _kp_rpins, _kp_buttonmap, KEYPAD_DEBOUNCE_MICROS)
#endif    
  {
    lastkey = 0;
    last_lcdrefresh = millis();
    motordistance_roc = &RATES_OF_CHANGE[0];
    ROC_START = motordistance_roc;
    MOD_AXIS=0;
    lcd_x = LCD_X;
    lcd_y = LCD_Y;
    switchmode_TEMP();
  }

  void reinit()
  {
    LCD.begin(LCD_X, LCD_Y);
    LCD.setBacklightPin(LCD_BL_PIN, NEGATIVE);
    LCD.on();
    LCD.clear();
    switchmode_TEMP();
    display_TEMP();
  }

  // This needs to be called regularly e.g. from the mainloop.
  void handleUpdates()
  {
#ifdef HAS_KEYPAD
    char pressedkey = KEYPAD.getPressedKey();
    if(pressedkey && pressedkey != lastkey)
    {
      inputswitch(pressedkey);
    }
    lastkey = pressedkey;
#endif

    unsigned long now = millis();
    if(now - last_lcdrefresh > LCD_REFRESH_MILLIS)
    {
      last_lcdrefresh = now;
      update_TEMPGRAPH();
      update_TEMP();

#ifdef HAS_SD
      update_SDSELECT();
      update_MODS();
#endif
      
#ifdef HAS_KEYPAD
      update_MOTORS();
#endif      
    }
  }

#ifdef HAS_KEYPAD  
  void setRowPin(int n, int pin) { KEYPAD.setRowPin(n, pin); }
  void setColPin(int n, int pin) { KEYPAD.setColPin(n, pin); }

  void changeMode(MODE which)
  {
    if(currentmode == which)
      return;

    switch(which)
    {
      case TEMP:
        switchmode_TEMP();
        break;
      case SDSELECT:
        switchmode_SDSELECT();
        break;
      case MOTORS:
        switchmode_MOTORS();
        break;
      case MENU:
        switchmode_MENU();
        break;
      case MODS:
        switchmode_MODS();
        break;
      default:
        break;
    }
  }
#else
  void setRowPin(int n, int pin) { ; }
  void setColPin(int n, int pin) { ; }
#endif  

private:
  unsigned long last_lcdrefresh;
  char lastkey;
  File entry;
#ifdef LCD_I2C_ADDR
  LiquidCrystal_I2C LCD;
#else
  LiquidCrystal LCD;
#endif
#ifdef HAS_KEYPAD
  Keypad KEYPAD;
#endif
  MODE currentmode;
  int  tempdistance;
  float motordistance;
  float const* motordistance_roc;
  float const* ROC_START;
  int MOD_AXIS;
  bool extrude;
  uint8_t tgraph[8*8];
  int lcd_x;
  int lcd_y;


  
#ifdef HAS_KEYPAD  
  void inputswitch(char key)
  {
    bool handled = false;
    switch(currentmode)
    {
      case TEMP:
        handled = keyhandler_TEMP(key);
        break;
      case SDSELECT:
        handled = keyhandler_SDSELECT(key);
        break;
      case MOTORS:
        handled = keyhandler_MOTORS(key);
        break;
      case MENU:
        handled = keyhandler_MENU(key);
        break;
      case MODS:
        handled = keyhandler_MODS(key);
        break;
    }
    if(!handled)
    {
      keyhandler_default(key);
    }
  }

  void keyhandler_default(char key) 
  {
    switch(key)
    {
      case 'A':
        changeMode(TEMP);
        break;
      case 'B':
        changeMode(SDSELECT);
        break;
      case 'C':
        changeMode(MOTORS);
        break;
      case 'D':
        changeMode(MODS);
        break;
      default:
        ;
    }
  }

  bool keyhandler_SDSELECT(char key) 
  { 
#ifdef HAS_SD    
    if(SDFILE)
      return false;

    switch(key)
    {
      case '6':
        entry = SDFILE.openNextFile();
        switchmode_SDSELECT();
        return true;
      case '#':
        SDFILE = entry;
        switchmode_SDSELECT();
        return true;
      default:
        return false;
    }
#endif
    return false;
  }


  bool keyhandler_MENU(char key) { return false; }

  
  bool keyhandler_MOTORS(char key) 
  { 
    bool handled = false;
    switch(key)
    {
      case '1':
        motordistance += *motordistance_roc;
        return true;
      case '7':
        motordistance -= *motordistance_roc;
        if(motordistance < 0)
          motordistance = 0;
        return true;
      case '*':
        extrude = !extrude;
        return true;
      case '#':
        motordistance_roc++;
        if(*motordistance_roc == 0.0f)
        {
          motordistance_roc = ROC_START;
        }
        return true;
      case '5':
      case '2':
      case '8':
      case '4':
      case '6':
      case '3':
      case '9':
        if(GCODES.isFull())
          return true;
        handled = true;
    }
    if(!handled)
      return false;

    GCode t;
    Point& p = GCode::getLastpos();
    switch(key)
    {
      case '5':
        t[M].setInt(84);
        GCODES.enqueue(t);
        return true;
      case '2':
        t[G].setInt(1);
#ifdef REVERSE_PRINTER
        t[Y].setFloat(p[Y] + motordistance);
#else
        t[Y].setFloat(p[Y] - motordistance);
#endif
        break;
      case '8':
        t[G].setInt(1);
#ifdef REVERSE_PRINTER
        t[Y].setFloat(p[Y] - motordistance);
#else
        t[Y].setFloat(p[Y] + motordistance);
#endif
        break;
      case '4':
        t[G].setInt(1);
#ifdef REVERSE_PRINTER
        t[X].setFloat(p[X] + motordistance);
#else
        t[X].setFloat(p[X] - motordistance);
#endif
        break;
      case '6':
        t[G].setInt(1);
#ifdef REVERSE_PRINTER
        t[X].setFloat(p[X] - motordistance);
#else
        t[X].setFloat(p[X] + motordistance);
#endif
        break;
      case '3':
        t[G].setInt(1);
        t[Z].setFloat(p[Z] + motordistance);
        break;
      case '9':
        t[G].setInt(1);
        t[Z].setFloat(p[Z] - motordistance);
        break;
    }
    GCODES.enqueue(t);
    return true;
  }

  bool keyhandler_TEMP(char key)
  {
    bool handled = false;
    switch(key)
    {
      case '1':
        adjustTempHotend(5);
        handled = true;
        break;
      case '3':
        adjustTempBed(5);
        handled = true;
        break;
      case '4':
        while(!TEMPERATURE.setHotend(0));
        handled = true;
        break;
      case '6':
        while(!TEMPERATURE.setPlatform(0));
        handled = true;
        break;
      case '7':
        adjustTempHotend(-5);
        handled = true;
        break;
      case '9':
        adjustTempBed(-5);
        handled = true;
        break;
      case 'A':
        reinit();
        handled = true;
        break;
    }
    if(handled)
    {
      LCD.clear();
      display_TEMP();
    }
    return handled;
  }

  void adjustTempHotend(int diff)
  {
    int16_t now = TEMPERATURE.getHotendST();
    now += diff;
    if(now < 0)
      now = 0;
    while(!TEMPERATURE.setHotend(now));
  }

  void adjustTempBed(int diff)
  {
    int16_t now = TEMPERATURE.getPlatformST();
    now += diff;
    if(now < 0)
      now = 0;
    while(!TEMPERATURE.setPlatform(now));
  }
#endif  


  void update_TEMPGRAPH()
  {
    int d=TEMPERATURE.getHotend()-TEMPERATURE.getHotendST();
    d/=2;
    if(d > 7)
      d=7;
    if(d < -8)
      d=-8;
    d+=8;
    d=15-d;

    for(int x=0;x<16;x++)
    {
      for(int y=0;y<3;y++)
      {
        tgraph[(y*16)+x] <<= 1;
        if(tgraph[16+(y*16)+x] & 0b00010000)
          tgraph[(y*16)+x] |= 1;
        tgraph[(y*16)+x] &= 0b00011111;
      }
      tgraph[x+48] <<= 1;
      if(x == d)
        tgraph[x+48] |= 1;
      tgraph[x+48] &= 0b00011111;
    }
    for(int x=0;x<8;x++)
    {
      LCD.createChar(x,tgraph+(x*8));
    }
  }

  void display_TEMP()
  {
    LCD.clear();
    LCD.home();
    int t=0;
    if(lcd_x>16)
    {
      LCD.write("Hotend:    /   ");
      LCD.setCursor(8,0);
    }
    else
    {
      LCD.write("H:   /   ");
      LCD.setCursor(2,0);
    }
    t = TEMPERATURE.getHotend();
    if(t == 1024)
      LCD.write("MISSING!");
    else
    {
      LCD.write(t);
      LCD.setCursor(lcd_x > 16 ? 11 : 6,0);
      LCD.write("/ ");
      LCD.write(TEMPERATURE.getHotendST());
    }
    LCD.setCursor(0,1);
    if(lcd_x>16)
    {
      LCD.write("Bed   :    /   ");
      LCD.setCursor(8,1);
    }
    else
    {
      LCD.write("B:   /   ");
      LCD.setCursor(2,1);
    }

    t = TEMPERATURE.getPlatform();
    if(t == 1024)
      LCD.write("MISSING!");
    else
    {
      LCD.write(t);
      LCD.setCursor(lcd_x > 16 ? 11 : 6,1);
      LCD.write("/ ");
      LCD.write(TEMPERATURE.getPlatformST());
    }
    LCD.setCursor(lcd_x > 16 ? 16 : 12,0);
    LCD.write((uint8_t)0);
    LCD.write((uint8_t)2);
    LCD.write((uint8_t)4);
    LCD.write((uint8_t)6);
    LCD.setCursor(lcd_x > 16 ? 16 : 12,1);
    LCD.write((uint8_t)1);
    LCD.write((uint8_t)3);
    LCD.write((uint8_t)5);
    LCD.write((uint8_t)7);

    tagline();
  }

  void switchmode_TEMP()
  {
    currentmode = TEMP;
    LCD.clear();
    display_TEMP();
  }

  void update_TEMP()
  {
    if(currentmode != TEMP)
      return;

    int t=0;
    if(lcd_x>16)
    {
      LCD.setCursor(8,0);
      LCD.write("        ");
      LCD.setCursor(8,0);
    }
    else
    {
      LCD.setCursor(2,0);
      LCD.write("        ");
      LCD.setCursor(2,0);
    }
    t = TEMPERATURE.getHotend();
    if(t == 1024)
      LCD.write("MISSING!");
    else
    {
      LCD.write(t);
      LCD.setCursor(lcd_x > 16 ? 11 : 6,0);
      LCD.write("/ ");
      LCD.write(TEMPERATURE.getHotendST());
    }
    LCD.setCursor(0,1);
    if(lcd_x>16)
    {
      LCD.setCursor(8,1);
      LCD.write("         ");
      LCD.setCursor(8,1);
    }
    else
    {
      LCD.setCursor(2,1);
      LCD.write("        ");
      LCD.setCursor(2,1);
    }

    t = TEMPERATURE.getPlatform();
    if(t == 1024)
      LCD.write("MISSING!");
    else
    {
      LCD.write(t);
      LCD.setCursor(lcd_x > 16 ? 11 : 6,1);
      LCD.write("/ ");
      LCD.write(TEMPERATURE.getPlatformST());
    }
    LCD.setCursor(lcd_x > 16 ? 16 : 12,0);
    LCD.write((uint8_t)0);
    LCD.write((uint8_t)2);
    LCD.write((uint8_t)4);
    LCD.write((uint8_t)6);
    LCD.setCursor(lcd_x > 16 ? 16 : 12,1);
    LCD.write((uint8_t)1);
    LCD.write((uint8_t)3);
    LCD.write((uint8_t)5);
    LCD.write((uint8_t)7);

    tagline();
  }

#ifdef HAS_KEYPAD
  void switchmode_MODS()
  {
    currentmode = MODS;
    display_MODS();
  }
  void label(char const* str, float n) { LCD.write(str); LCD.write(n); }
  void label(char const* str, char const* str2) { LCD.write(str); LCD.write(str2); }
  void label(char const* str, int32_t n) { LCD.write(str); LCD.write(n); }
  void label(char const* str, int n) { LCD.write(str); LCD.write((int32_t)n); }
  void display_MODS()
  {
    LCD.clear();
    label("AXIS:",MOD_AXIS);
    LCD.setCursor(10,0);
    label("MIN:",(int32_t)MOTION.getAxis(MOD_AXIS).getStartFeed());
    LCD.setCursor(0,1);
    label("ACC:",MOTION.getAxis(MOD_AXIS).getAccel()); 
    LCD.setCursor(10,1);
    label("MAX:",(int32_t)MOTION.getAxis(MOD_AXIS).getMaxFeed());
    LCD.setCursor(0,2);
    label("FD:",MOTION.getFeedModifier()); LCD.write("%");
    LCD.setCursor(9,2);
    label("STP:",MOTION.getAxis(MOD_AXIS).getStepsPerMM());

    tagline();
  }
#endif

  void update_MODS()
  {
    if(currentmode != MODS)
      return;
    tagline();
  }

#ifdef HAS_KEYPAD
  bool keyhandler_MODS(char key) 
  { 
    bool handled = false;
    float t;
    switch(key)
    {
      case 'A': // Switch axis, or default
      case 'B':
      case 'C':
      case 'D':
        if(key - 'A' == MOD_AXIS)
          break;
        MOD_AXIS = key - 'A';
        handled = true;
        break;
      case '1': // Lower Minimum Feedrate
        t = MOTION.getAxis(MOD_AXIS).getStartFeed();
        t-=100;
        if(t>=100)
        {
          MOTION.getAxis(MOD_AXIS).setMinimumFeedrate(t);
          handled = true;
        }
        break;
      case '3': // Raise Minimum Feed
        t = MOTION.getAxis(MOD_AXIS).getStartFeed();
        t+=100;
        if(t<=MOTION.getAxis(MOD_AXIS).getMaxFeed())
        {
          MOTION.getAxis(MOD_AXIS).setMinimumFeedrate(t);
          handled = true;
        }
        break;
      case '7': // Lower Max Feed
        t = MOTION.getAxis(MOD_AXIS).getMaxFeed();
        t-=100;
        if(t>=MOTION.getAxis(MOD_AXIS).getStartFeed())
        {
          MOTION.getAxis(MOD_AXIS).setMaximumFeedrate(t);
          handled = true;
        }
        break;
      case '9':  // Raise Max Feed
        t = MOTION.getAxis(MOD_AXIS).getMaxFeed();
        t+=100;
        MOTION.getAxis(MOD_AXIS).setMaximumFeedrate(t);
        handled = true;
        break;
      case '4':  // Lower Accel
        t = MOTION.getAxis(MOD_AXIS).getAccel();
        t-=100;
        if(t>=100)
        {
          MOTION.getAxis(MOD_AXIS).setAccel(t);
          handled = true;
        }
        break;
      case '6':  // Raise Accel
        t = MOTION.getAxis(MOD_AXIS).getAccel();
        t+=100;
        MOTION.getAxis(MOD_AXIS).setAccel(t);
        handled = true;
        break;
      case '2': // jog axis positive
        switch(MOD_AXIS)
        {
          case 0:
          case 1:
            MOTION.getAxis(MOD_AXIS).setCurrentPosition(
              MOTION.getAxis(MOD_AXIS).getCurrentPosition() - 1.0f);
            break;
          case 2:
            MOTION.getAxis(MOD_AXIS).setCurrentPosition(
              MOTION.getAxis(MOD_AXIS).getCurrentPosition() - 0.1f);
            break;
          case 3:
            MOTION.getAxis(3).setStepsPerUnit(
              MOTION.getAxis(3).getStepsPerMM() + 10);
            break;
        }
        handled = true;
        break;
      case '8': // jog axis negative
        switch(MOD_AXIS)
        {
          case 0:
          case 1:
            MOTION.getAxis(MOD_AXIS).setCurrentPosition(
              MOTION.getAxis(MOD_AXIS).getCurrentPosition() + 1.0f);
            break;
          case 2:
            MOTION.getAxis(MOD_AXIS).setCurrentPosition(
              MOTION.getAxis(MOD_AXIS).getCurrentPosition() + 0.1f);
            break;
          case 3:
            MOTION.getAxis(3).setStepsPerUnit(
              MOTION.getAxis(3).getStepsPerMM() - 10);
            break;
        }
        handled = true;
        break;
      case '*': // Lower feed percentage
        t = MOTION.getFeedModifier();
        t -= 5.0f;
        if(t<LOW_FEED_MODIFIER) t = LOW_FEED_MODIFIER;
        MOTION.setFeedModifier(t);
        handled = true;
        break;
      case '#': // Raise feed percentage
        MOTION.setFeedModifier(MOTION.getFeedModifier() + 5.0f);
        handled = true;
        break;
      case '0': // Toggle fan
        GCode::togglefan();
        handled = true;
        break;
      case '5': // Toggle pause
        GCODES.togglepause();
        handled = true;
        break;
    }
    if(handled)
      display_MODS();
    return(handled);
  }

  void switchmode_MOTORS()
  {
    currentmode = MOTORS;
    LCD.clear();
    display_MOTORS();
  }
  void update_MOTORS()
  {
    if(currentmode != MOTORS)
      return;
    LCD.home();
    display_MOTORS();
  }

  void display_MOTORS()
  {
    Point& lastpos = GCode::getLastpos();
    label("X:", lastpos[X]);
    LCD.setCursor(8,0);
    label("Y:", lastpos[Y]);
    LCD.setCursor(0,1);
    label("Z:", lastpos[Z]);
    if(lcd_y > 2)
    {
      LCD.setCursor(8,1);
      label("E:", lastpos[E]);
      LCD.setCursor(0,2);
      label("Move:", motordistance);
      if(lcd_x > 16)
        label(" ROC:",*motordistance_roc);
      else
        label("/",*motordistance_roc);
      LCD.setCursor(0,3);
      label("Extruder:", extrude ? "ON " : "OFF");
    }
    else
    {
      label("I:", motordistance);
      label("/",*motordistance_roc);
      label(" E:", extrude ? "-" : "*");
    }
    if(lcd_x>16)
    {
      LCD.setCursor(16,0);
      LCD.write((uint8_t)0);
      LCD.write((uint8_t)2);
      LCD.write((uint8_t)4);
      LCD.write((uint8_t)6);
      LCD.setCursor(16,1);
      LCD.write((uint8_t)1);
      LCD.write((uint8_t)3);
      LCD.write((uint8_t)5);
      LCD.write((uint8_t)7);
    }


  }

  void switchmode_MENU()
  {
    currentmode = MENU;
    LCD.clear();
    LCD.write("MAIN MENU");
  }

    
  void switchmode_SDSELECT()
  {
    currentmode = SDSELECT;
    LCD.clear();
    display_SDSELECT();
  }
#endif  

  void update_SDSELECT()
  {
    if(currentmode != SDSELECT)
      return;
    tagline();
  }

  void display_SDSELECT()
  {
#ifdef HAS_SD    
    if(!entry.isDirectory())
    {
      LCD.write("Printing: ");
      LCD.setCursor(0,1);
      LCD.write(SDFILE.name());
      tagline();
      return;
    } else {
      entry = SDFILE.openNextFile();
    }

    LCD.write("SD File Select");
    LCD.setCursor(0,1);
    LCD.write("> ");
    LCD.write(SDFILE.name());
    if(lcd_y > 2)
    {
      LCD.setCursor(0,2);
      LCD.write("6=NEXT #=PRINT");
    }
#else
    LCD.write("Get SD from");
    LCD.setCursor(0,1);
    LCD.write("Kliment on IRC");
#endif    
    tagline();
  }

  void tagline()
  {
    if(lcd_y < 4)
      return;
    LCD.setCursor(0,3);
#ifdef HAS_SD
    if(!SDFILE.isDirectory())
    {
      LCD.write("SD Print: ");
      LCD.write((uint16_t)(SDFILE.position() * 100 / SDFILE.size()));
      LCD.write("%");
		} else {
#endif
    	LCD.write(LCD_TAGLINE);
#ifdef HAS_SD
		}
#endif
  }

};


#endif
#endif

