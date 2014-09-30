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

#ifndef SHARED_AVR_PORT_HH_
#define SHARED_AVR_PORT_HH_

#include <avr/io.h>

// The AVR port and pin mapping is based on a convention that has held true for all ATMega chips
// released so far: that the ports begin in sequence from register 0x00 from A onwards, and are
// arranged:
// 0 PINx
// 1 DDRx
// 2 PORTx
// This is verified true for the 168/644p/1280.

#define PINx _SFR_MEM8(port_base+0)
#define DDRx _SFR_MEM8(port_base+1)
#define PORTx _SFR_MEM8(port_base+2)

typedef uint16_t port_base_t;
#define NULL_PORT 0xffff

class Port;
extern Port PORTMAP[];

class Port {
private:
	port_base_t port_base;
public:
	Port() : port_base(NULL_PORT) {}

	Port(port_base_t port_base_in) : port_base(port_base_in) {}

  Port& operator=(const Port& p) { port_base = p.port_base; return *this; }

	bool isNull() { return port_base == NULL_PORT; }
	void setPinDirection(uint8_t pin_index, bool out) {
		DDRx = (DDRx & ~_BV(pin_index)) | (out?_BV(pin_index):0);
	}
	bool getPin(uint8_t pin_index) {
		return (PINx & _BV(pin_index)) != 0;
	}
	void setPin(uint8_t pin_index, bool on) {
		PORTx = (PORTx & ~_BV(pin_index)) | (on?_BV(pin_index):0);
	}
  port_base_t getpb() const { return port_base; }
  static Port& getPortFromLetter(char l) { return(PORTMAP[l-'A']); }
};

extern Port PortA, PortB, PortC, PortD;
extern Port PortE, PortF, PortG, PortH;
extern Port PortJ, PortK, PortL, PortNull;

class Pin {
private:
	Port port;
	uint8_t pin_index : 4;
public:
	Pin() : port(Port()), pin_index(0) {}

	Pin(Port port_in, uint8_t pin_index_in) : port(port_in), pin_index(pin_index_in) {}

  Pin& operator=(const Pin& p) { port = p.port; pin_index = p.pin_index; return *this; }

	bool isNull() { return port.isNull(); }
	void setDirection(bool out) { port.setPinDirection(pin_index,out); }
	bool getValue() { return port.getPin(pin_index); }
	void setValue(bool on) { port.setPin(pin_index,on); }
	uint8_t getPinIndex() { return pin_index; }
  uint16_t getPortIndex() const { return port.getpb(); }
};

#endif // SHARED_AVR_PORT_HH_

