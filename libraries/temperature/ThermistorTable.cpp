/*
 * Copyright 2010 by Adam Mayer	 <adam@makerbot.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "ThermistorTable.h"
#include <stdint.h>
#include <avr/pgmspace.h>
#include <string.h>

const static int NUMTEMPS = 20;

// Thermistor lookup table for RepRap Temperature Sensor Boards (http://make.rrrf.org/ts)
// Made with createTemperatureLookup.py (http://svn.reprap.org/trunk/reprap/firmware/Arduino/utilities/createTemperatureLookup.py)
// ./createTemperatureLookup.py --r0=100000 --t0=25 --r1=0 --r2=4700 --beta=4066 --max-adc=1023
// r0: 100000
// t0: 25
// r1: 0
// r2: 4700
// beta: 4066
// max adc: 1023
typedef int16_t TempTable[NUMTEMPS][2];

TempTable default_table = {
  {1, 841},
  {54, 255},
  {107, 209},
  {160, 184},
  {213, 166},
  {266, 153},
  {319, 142},
  {372, 132},
  {425, 124},
  {478, 116},
  {531, 108},
  {584, 101},
  {637, 93},
  {690, 86},
  {743, 78},
  {796, 70},
  {849, 61},
  {902, 50},
  {955, 34},
  {1008, 3}
};


typedef struct {
	int16_t adc;
	int16_t value;
} Entry;

inline Entry getEntry(int8_t entryIdx, int8_t which) {
	Entry rv;
		memcpy(&rv, &(default_table[entryIdx][0]), sizeof(Entry));
	return rv;
}

void putThermistorEntry(int16_t adc_val, int16_t temp_val, int8_t which) {
  if(which < 0)
    return;
  if(which >= NUMTEMPS)
    return;

  default_table[which][0] = adc_val;
  default_table[which][1] = temp_val;
}

int16_t thermistorToCelsius(int16_t reading, int8_t table_idx) {
  int8_t bottom = 0;
  int8_t top = NUMTEMPS-1;
  int8_t mid = (bottom+top)/2;
  Entry e;
  while (mid > bottom) {
	  e = getEntry(mid,table_idx);
	  if (reading < e.adc) {
		  top = mid;
		  mid = (bottom+top)/2;
	  } else {
		  bottom = mid;
		  mid = (bottom+top)/2;
	  }
  }
  Entry eb = getEntry(bottom,table_idx);
  Entry et = getEntry(top,table_idx);
  if (bottom == 0 && reading < eb.adc) {
	  // out of scale; safety mode
	  return 1024;
  }
  if (top == NUMTEMPS-1 && reading > et.adc) {
	  // out of scale; safety mode
	  return 1024;
  }

  int16_t celsius  = eb.value +
		  ((reading - eb.adc) * (et.value - eb.value)) / (et.adc - eb.adc);
  if (celsius > 300)
	  celsius = 1024;
  return celsius;
}



