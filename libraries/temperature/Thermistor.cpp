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

#include <Arduino.h>

#include "Thermistor.h"
#include "ThermistorTable.h"
#include <util/atomic.h>

Thermistor::Thermistor(int8_t analog_pin_in, uint8_t table_index_in) :
analog_pin(analog_pin_in), 
raw_valid(false), next_sample(0), table_index(table_index_in)
{
	for (int i = 0; i < SAMPLE_COUNT; i++) { sample_buffer[i] = 0; }
}

void Thermistor::init() {
  if(analog_pin < 0) 
    return;
}

Thermistor::SensorState Thermistor::update() {
	int temp = analogRead(analog_pin);
	current_temp = thermistorToCelsius(temp,table_index);
	return SS_OK;
}

void Thermistor::changeTable(int16_t adc_val, int16_t temp_val, int8_t which)
{
  putThermistorEntry(adc_val, temp_val, which);
}

