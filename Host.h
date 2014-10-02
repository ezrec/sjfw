#ifndef _HOST_H_
#define _HOST_H_

/* Serial comms - port0 or 2
	Chris "ScribbleJ" Jansen
	(c)2011

	Loosely based on Teacup serial library
*/
#include <Arduino.h>

#include <avr/interrupt.h>
#include <stdlib.h>
#include "config.h"
#include <avr/pgmspace.h>

#define MASK(PIN) (1 << PIN)
#if ((defined __AVR_ATmega2560__) || (defined __AVR_ATmega1280__))
#define HIGHPORTS
#else
#undef HIGHPORTS
#endif

class Host
{
	public:
		// singleton
		static Host& Instance(int port)
		{
			static Host instance(HOST_BAUD, port);
			return instance;
		}
		static Host& Instance() { return Instance(0); }
	private:
		explicit Host(unsigned long baud, int port);
		Host(Host&);
		Host& operator=(Host&);
		int port;
		char convbuf[32];

	public:
		uint8_t rxchars() { return Serial.available(); }
		uint8_t popchar() { return Serial.read(); }
		uint8_t peekchar() { return Serial.peek(); }

		void write(uint8_t data)
		{
			Serial.write(data);
		}

		void write(const char *data) { uint8_t i = 0, r; while ((r = data[i++])) write(r); }
		void write(uint32_t n, int radix)
		{
			ultoa(n,convbuf,radix);
			write(convbuf);
		}
		void write(int32_t n, int radix)
		{
			ltoa(n,convbuf,radix);
			write(convbuf);
		}
		void write(int16_t n, int radix) { write((int32_t)n, radix); }
		void write(uint16_t n, int radix) { write((int32_t)n, radix); }

		void write(float n, signed char width, unsigned char prec)
		{
			dtostrf(n,width,prec,convbuf);
			write(convbuf);
		}
		void write_P(const char* data)
		{
			char c;
			while ((c = pgm_read_byte(data++)))
				write(c);
		}

		void endl()
		{
			write((uint8_t)'\n');
		}



		void labelnum(const char *label, uint16_t num, bool end=true)
		{
			write(label);
			write((uint32_t)num,10);
			if(end)
				endl();
		}
		void labelnum(const char *label, int32_t num, bool end=true)
		{
			write(label);
			write(num,10);
			if(end)
				endl();
		}
		void labelnum(const char *label, uint32_t num, bool end=true)
		{
			write(label);
			write(num,10);
			if(end)
				endl();
		}
		void labelnum(const char *label, int num, bool end=true) { labelnum(label, (uint16_t)num, end); };

		void labelnum(const char *label, float num, bool end=true)
		{
			write(label);
			write(num,0,4);
			if(end)
				endl();
		}

		void rxerror(const char*errmsg, int32_t linenum)
		{
			labelnum("rs ", linenum, false);
#ifdef REPRAP_COMPAT
			(void)errmsg; // unused
#else
			write(' ');
			write(errmsg);
#endif
			endl();
			//rxring.reset();
			//input_ready=0;
		}

		void rxerror(const char* errmsg)
		{
			write("!! ");
			write(errmsg);
			endl();
			//rxring.reset();
			//input_ready=0;
		}

		void scan_input();
};

extern Host& HOST;

#ifdef HAS_BT
extern Host& BT;
#endif


#endif
