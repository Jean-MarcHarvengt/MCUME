#include "xram.h"
#include <Arduino.h>

EXTMEM static unsigned char MemPool[MEMSIZE]; 

unsigned char * xram_MemPool(void) {
  return (&MemPool[0]);
}

