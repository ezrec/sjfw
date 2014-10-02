/*
 * Copyright (C) 2014, Jason S. McMullan
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "config.h"

#include <SD.h>

#include "Eeprom.h"
#include "Host.h"
#include "GcodeQueue.h"
#include "Temperature.h"
#include "Globals.h"
#include "LCDKeypad.h"

#ifdef HAS_LCD
extern LCDKeypad LCDKEYPAD;
#endif

extern "C" void atexit(void (*function)(void))
{
}

#include <Wire.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>

void setup () {
    LCDKEYPAD.reinit();

#ifdef HAS_SD
    // Init SD
    SD.begin(SD_SELECT_PIN);
#endif

    eeprom::beginRead();

    // Print banner
    HOST.write_P(PSTR("start\n"));
}

void loop () {
    // Checks to see if gcodes are waiting to run and runs them if so.
    GCODES.handlenext();

    // Rather than interleaving, we'll just run it twice in succession, to catch instances where it
    // wants an immediate rerun, otherwise assume it's good 'till we get back.
    GCODES.handlenext();
    GCODES.checkaxes();

    // Checks to see if recieve buffer has enough data to parse, and sends it to
    // GcodeQueue.h for processing if so.
    HOST.scan_input();

    // Updates temperature information; scans temperature sources
    TEMPERATURE.update();

    // Manage Eeprom operations
    eeprom::update();

#ifdef HAS_LCD
    // Update LCD, read keypresses, etc.
    LCDKEYPAD.handleUpdates();
#endif

}

/* vim: set shiftwidth=4 expandtab:  */
