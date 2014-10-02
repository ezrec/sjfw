#ifndef _KEYPAD_H
#define _KEYPAD_H
/* Button-grid-scanning-style keypad support.  IDK what the name for these are, they are common.
 * (c) 2011, Christopher "ScribbleJ" Jansen
 * 
 */

#include "config.h"

class Keypad {
public:
 
  Keypad(const int *cps, const int *rps, const char **buttonmap_in, uint8_t debounce_in)
  {                    
    buttonmap = buttonmap_in;
    debounce  = debounce_in;
    good_time = 0;
    last_char = 0;

    for (int i = 0; i < KP_COLS; i++) 
    {
      colpins[i] = cps[i];
    }

    for (int i = 0; i < KP_ROWS; i++) 
    {
      rowpins[i] = rps[i];
    }

    reinit();
  }

  void reinit()
  {
    if ( colpins[0] < 0)
      return;

    for (int i = 0; i < KP_COLS; i++) 
    {
      pinMode(colpins[i], INPUT);
    }

    for (int i = 0; i < KP_ROWS; i++) 
    {
      pinMode(rowpins[i], INPUT_PULLUP);
    }
  }

   
  // Scans one column of keys for keypresses.
  uint8_t scanCol(uint8_t colnum)
  {
    pinMode(colpins[colnum], OUTPUT);
    digitalWrite(colpins[colnum], 0);

    uint8_t ret = 0;
    for(int x=0;x<KP_ROWS;x++)
    {
      if(!digitalRead(rowpins[x]))
        ret |= 1 << x;
    }

    pinMode(colpins[colnum], INPUT);

    return ret;
  }

  // returns the first pressed key that is found.
  // note - this is expected to be called in a loop like the sjfw mainloop so that it can perform debounce.
  // this function and the above are written this way to make it easy to add support for multiple buttons later.
  char getPressedKey()
  {
    // if config is invalid, maybe we get it later.
    if(colpins[0] < 0)
      return 0;

    unsigned long now = micros();

    uint8_t colscan = 0;
    for(int x=0;x<KP_COLS;x++)
    {
      colscan = scanCol(x);
      if(colscan)
      {
        for(int y=0;y<KP_ROWS;y++)
        {
          if(colscan & (1 << y))
          {
            char fc = buttonmap[y][x];
            if(fc == last_char && good_time <= now)
              return fc;

            if(fc != last_char)
            {
              last_char = fc;
              good_time = now + debounce;
            }
            return 0;
          }
        }
      }
    }
    if(good_time && good_time < now)
    {
      good_time = 0;
      last_char = 0;
    }
    return 0;
  }

  void setColPin(int n, int pin) { colpins[n] = pin; if(n ==0) reinit(); }
  void setRowPin(int n, int pin) { rowpins[n] = pin; }

private:
  int colpins[KP_COLS];
  int rowpins[KP_ROWS];

  const char **buttonmap;
  uint8_t debounce;
  unsigned long good_time;
  char last_char;

};

#endif 
