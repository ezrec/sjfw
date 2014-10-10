#ifndef _CONFIG_H_
#define _CONFIG_H_

#define HAS_LCD         1
#define HAS_KEYPAD      1
#define HAS_SD          1

#define SD_AUTORUN      "sjfwauto.gc"

#include "config-common.h"

#define NUM_AXES 4
#define ENDSTOPS_INVERTING 1
#define ENDSTOPPULLUPS 1
#define SAFE_DEFAULT_FEED 1500

#define HOTEND_TEMP_PIN 13
#define PLATFORM_TEMP_PIN 14

#define HOTEND_HEAT_PIN -1
#define PLATFORM_HEAT_PIN -1

#define SD_SELECT_PIN   SS


#define X_DCMOTOR       1
#define X_ENCODER_A     18
#define X_ENCODER_B     14
#define X_MIN_PIN       -1
#define X_MAX_PIN       -1
#define X_INVERT_DIR    false
#define X_STEPS_PER_UNIT 62.745
#define X_MAX_FEED      12000
#define X_AVG_FEED      6000
#define X_START_FEED    2000
#define X_ACCEL_RATE    200
#define X_DISABLE       false

#define Y_DCMOTOR       2
#define Y_ENCODER_A     19
#define Y_ENCODER_B     15
#define Y_MIN_PIN       -1
#define Y_MAX_PIN       -1
#define Y_INVERT_DIR    false
#define Y_STEPS_PER_UNIT 62.745
#define Y_MAX_FEED      12000
#define Y_AVG_FEED      6000
#define Y_START_FEED    2000
#define Y_ACCEL_RATE    200
#define Y_DISABLE       false

#define Z_DCMOTOR       3
#define Z_ENCODER_A     20
#define Z_ENCODER_B     16
#define Z_MIN_PIN       -1
#define Z_MAX_PIN       -1
#define Z_INVERT_DIR    true
#define Z_STEPS_PER_UNIT 2267.718
#define Z_MAX_FEED      150
#define Z_AVG_FEED      100
#define Z_START_FEED    75
#define Z_ACCEL_RATE    50
#define Z_DISABLE       true

#define A_DCMOTOR       4
#define A_ENCODER_A     21
#define A_ENCODER_B     17
#define A_INVERT_DIR    false
#define A_STEPS_PER_UNIT 729.99
#define A_MAX_FEED      24000
#define A_AVG_FEED      12000
#define A_START_FEED    5000
#define A_ACCEL_RATE    2000
#define A_DISABLE       false


// The following config is for a parallel LCD connected to 
// AUX-2 in 4-bit mode.

#define USE4BITMODE
#define LCD_I2C_ADDR    0x20

/* NOTE: Since LCD_I2C_ADDR is defined, these 'pins' are off the
 * GPIO Expander at the I2C address
 */
#define LCD_BL_PIN      7
#define LCD_RS_PIN      6
#define LCD_RW_PIN      5
#define LCD_E_PIN       4
#define LCD_0_PIN       -1
#define LCD_1_PIN       -1
#define LCD_2_PIN       -1
#define LCD_3_PIN       -1
#define LCD_4_PIN       0
#define LCD_5_PIN       1
#define LCD_6_PIN       2
#define LCD_7_PIN       3

#define LCD_X 20
#define LCD_Y 4
#define LCD_LINESTARTS {0x0, 0x40, 0x14, 0x54}

#ifdef HAS_KEYPAD
#define KP_ROWS 4
#define KP_COLS 4
#define KEYPAD_BUTTONMAP "123A", "456B", "789C", "*0#D"
#define KEYPAD_ROWPINS 8,9,10,11
#define KEYPAD_COLPINS 12,13,14,15
#define KEYPAD_DEBOUNCE_MICROS 50
#endif


#endif // CONFIG_H
