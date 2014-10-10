/* YOU ARE PROBABLY LOOKING FOR THE BOARD CONFIG FILES UNDER ramps/ AND gen4/
   AND NOT THIS FILE, which is just defining some things peculiar to the sourcecode. */

#define LCD_TAGLINE "SJFW by ScribbleJ"
#define LOW_FEED_MODIFIER 25.0f

#define REPRAP_COMPAT 
//#define ADV_CHECKSUMS
#define REPG_COMPAT
// Maximum length of a single 'fragment' of Gcode; characters in-between spaces.
#define MAX_GCODE_FRAG_SIZE 32
// Gcode is a big structure here, 10 is a lot of ram.
#ifdef __AVR_ATmega644P__
#define GCODE_BUFSIZE 5
#define HOST_RECV_BUFSIZE 100
#define HOST_SEND_BUFSIZE 100
#else
#define GCODE_BUFSIZE 10
#define HOST_RECV_BUFSIZE 200
#define HOST_SEND_BUFSIZE 200
#endif

// Each source eats anough ram for 1 addtl gcode
#if (defined HAS_BT) || defined(HAS_KEYPAD)
#define GCODE_SOURCES 5
#define HOST_SOURCE 0
#define SD_SOURCE   1
#define BT_SOURCE   2
#define LCD_SOURCE  3
#define EEPROM_SOURCE 4
#else
#define GCODE_SOURCES 3
#define HOST_SOURCE 0
#define EEPROM_SOURCE 1
#define SD_SOURCE   2
#endif

// How often to update the temperature display on the LCD.
#define LCD_REFRESH_MILLIS 2000
// Size of LCD command queue.. should be minimum of LCD_X*LCD_Y + 100
#define LCD_BUFFER_SIZE 350



#define HOST_BAUD 115200
// if defined, INTERRUPT_STEPS allows the comm ISRs to interrupt the movement ISR.
#define INTERRUPT_STEPS
// How often to recompute speed for acceleration in sjfw
#define ACCELS_PER_SECOND 1000.0f
#define ACCEL_INC_TIME F_CPU/ACCELS_PER_SECOND

//#define COMMS_ERR2
//#define DEBUG_OPT
//#define DEBUG_MOVE
//#define DEBUG_ACCEL
#define DEBUG_JUMP

#define BT_BAUD 9600
//#define BT_DEBUG


