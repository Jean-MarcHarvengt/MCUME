#ifndef _H_MOS6502MEMORY
#define _H_MOS6502MEMORY

#include <stdint.h>

extern uint8_t vicmemory[];


#define readWord(location) (vicmemory[location])
#define writeWord(location,value) {vicmemory[location]=value;}
//extern  uint8_t readWord( uint16_t location);
//extern  void writeWord( uint16_t location, uint8_t value);

#define silentReadWord(location) (vicmemory[location])
#define silentWriteWord(location,value) {vicmemory[location]=value;}
#define silentReadDWord(location) (vicmemory[location] | vicmemory[location + 1] << 8)
#define silentWriteDWord(location,value) {vicmemory[location]=uint8_t(value & 0xFF); vicmemory[location + 1]=(uint8_t)((value >> 8) & 0xFF);}


#endif