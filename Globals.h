#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "config.h"
// QND function to estimate free ram.
inline int getFreeRam () {
  // These externs are defined and used in libavr
  extern int __heap_start, *__brkval; 
  // Really I want SP, but without writing any asm, this is an easy way to get it!
  int v; 

  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval) - sizeof(int); 
}

// Whether I should define such common letters as globals is a bit questionable, but...
// at any rate, it's CRITICAL the order here matches the array in GCode.  We also
// assume multiple places that T is the last item, and the major axis are in order at the start.
enum { X=0, Y, Z, E, M, G, F, P, S, T };

#ifdef HAS_SD
#include <SD.h>
extern File SDFILE;
#endif

#endif
