#include "emuapi.h"

#ifdef HAS_T41 
#include <Arduino.h>
EXTMEM static unsigned char MemPool[8*1024*1024];
static int ind=0;
#endif

void * loc_Malloc(int size) {
  void * pt = emu_Malloc(size);
#ifdef HAS_T41 
  if (pt == NULL) {
    pt = MemPool+ind;
    ind += size;
  }
#endif  
  return(pt);
}
