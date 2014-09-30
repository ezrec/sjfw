/*
 * Copyright (C) 2014, Netronome, Inc.
 * All right reserved.
 * Author: Jason McMullan <jason.mcmullan@netronome.com>
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

#ifndef AVR_ARDUINO_H
#define AVR_ARDUINO_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Time.h"
#include "ArduinoMap.h"

#define OUTPUT  true
#define INPUT   false

#define HIGH    true
#define LOW     false

static inline void pinMode(uint8_t pin, bool is_output)
{
  ArduinoMap::getPort(pin).setPinDirection(ArduinoMap::getPinnum(pin), is_output);
}

static inline void digitalWrite(uint8_t pin, bool is_high)
{
  ArduinoMap::getPort(pin).setPin(ArduinoMap::getPinnum(pin), is_high);
}

static inline volatile uint8_t *digitalPinToPort(uint8_t pin)
{
  return &_SFR_MEM8(ArduinoMap::getPort(pin).getpb());
}

static inline uint8_t digitalPinToBitMask(uint8_t pin)
{
  return 1 << ArduinoMap::getPinnum(pin);
}

static inline volatile uint8_t *portInputRegister(volatile uint8_t *port)
{
  return port + 0;
}

static inline void delayMicroseconds(unsigned int us)
{
  unsigned long ms = (us + 999) / 1000;
  wait(ms);
}

static inline void delay(unsigned int ms)
{
  wait(ms);
}

#define CHANGE 1
#define FALLING 2
#define RISING 3

static inline void noInterrupts(void)
{
  cli();
}

static inline void interrupts(void)
{
  sei();
}

void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode);
void detachInterrupt(uint8_t interruptNum);

#endif /* AVR_ARDUINO_H */
/* vim: set shiftwidth=2 expandtab:  */
