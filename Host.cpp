/*
		(c) 2011 Christopher "ScribbleJ" Jansen
*/

#include "Host.h"
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "GcodeQueue.h"

#include "config.h"

Host::Host(int port_in, unsigned long BAUD)
{
	port = port_in;
	baud = BAUD;
}

void Host::begin()
{
	Serial.begin(baud);
}

void Host::scan_input()
{
	char buf[MAX_GCODE_FRAG_SIZE];
	uint8_t len;

	for(len=0;len<MAX_GCODE_FRAG_SIZE;len++)
	{
		int c = Serial.read();
		if (c < 0)
			break;

		buf[len] = c;
	}

	// HOST.labelnum("Fragment length:", len);

	if(len == MAX_GCODE_FRAG_SIZE)
	{
		rxerror("Frag Over");
		return;
	}

	if (len > 0)
		GCODES.parsebytes(buf, len, port);

}
