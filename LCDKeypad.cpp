#include <stdint.h>

#include "config.h"
#include "LCDKeypad.h"

uint8_t _lcd_linestarts[] = LCD_LINESTARTS;
#ifdef HAS_KEYPAD
const char *_kp_buttonmap[] = { KEYPAD_BUTTONMAP };
const int _kp_rpins[] = { KEYPAD_ROWPINS };
const int _kp_cpins[] = { KEYPAD_COLPINS };
#endif
const float RATES_OF_CHANGE[] = { 5, 10, 100, 0.1, 1, 0 };

