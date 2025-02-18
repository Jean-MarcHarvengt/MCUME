/*
* Castaway
*  (C) 1994 - 2002 Martin Doering, Joachim Hoenig
*
* $File$ - memory read/write
*
* This file is distributed under the GPL, version 2 or at your
* option any later version.  See doc/license.txt for details.
*
* revision history
*  23.05.2002  JH  FAST1.0.1 code import: KR -> ANSI, restructuring
*  30.05.2002  JH  Discontinued using mmap and mprotect, now using
*                  Martin's memory access jump table.
*  12.06.2002  JH  Correct bus error/address error exception stack frame
*  14.06.2002  JH  LowRamSetX() functions improved.
*  09.07.2002  JH  Now loads any 192k ROM file
*  10.07.2002  MAD Now loads any ROM file
*  16.09.2002  JH  Bugfix: Word access on unmapped I/O address stacked
*                  two bus error stack frames. Fault address corrected.
*  08.10.2002  JH  Fixed integer types.
*  27.10.2002  AG  Trashed everything for more speed! mwuhahaha!
*/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#ifndef DREAMCAST
#else
#include <string.h>
#endif

#include "dcastaway.h"
#include "st.h"
#include "mem.h"
#include "m68k_intrf.h"

static unsigned rombase_pos=0;

char rom[80]; // = ROM;
#ifdef DREAMCAST
void reinit_sdcard(void);
char rom_sd[24] = ROM_SD;
#endif
static int samvol[16]={0,0,0,1,1,1,2,3,5,7,11,17,25,38,57,85};
extern uint32 psg[26];
#define lastpsg psg[25]
#define sampos psg[24]



char GetMemB(unsigned long address)
{
	address &= MEMADDRMASK;
	if (address<MEMSIZE)
		return ReadB(address/* + mem1base*/);
	else
	if (address<ROMBASE2)
		return -1;
	else
	if (address<IOBASE)
		return *(uint8*)((uint32)(address + rombase)^1);
	    //ReadB(address + rombase);
	else
	if (address<IOBASE+IOSIZE)
		return DoIORB(address);
	return -1;
}

/* Fetch word, address may not be word-aligned */
short GetMemW(unsigned long address)
{
#ifdef CHKADDRESSERR
    address &= MEMADDRMASK;
    if (address & 0x1){
		ExceptionGroup0(ADDRESSERR, address, 1);
		return -1;
	}
#else
    address &= MEMADDRMASKS;
#endif
	if (address<MEMSIZE)	
		return ReadW(address /*+ mem1base*/);
	else
	if (address<ROMBASE2)
		return -1;
	else
	if (address<IOBASE)
		return *(uint16*)(address + rombase);
		//return ReadW(address + rombase);
	else
	if (address<IOBASE+IOSIZE)
		return DoIORW(address);
	return -1;
}

/* Fetch dword, address may not be dword-aligned */
long GetMemL(unsigned long address)
{
#ifdef CHKADDRESSERR
    address &= MEMADDRMASK;
    if (address & 0x1){
		ExceptionGroup0(ADDRESSERR, address, 1);
		return -1;
	}
#else
    address &= MEMADDRMASKS;
#endif
	if (address<MEMSIZE)	
		return ReadL(address/* + mem1base*/);
	else
	if (address<ROMBASE2)
		return -1;
	if (address<IOBASE)
		//ReadL(address + rombase);
		return ((*(uint16*)(address + rombase))<<16)|(*(uint16*)(address + rombase+2));
	if (address<IOBASE+IOSIZE)
		return DoIORL(address);
	return -1;
}


/* Write byte to address */
void SetMemB (unsigned long address, unsigned char value) 
{
    address &= MEMADDRMASK;
	ON_WRITE(address, value);
	if (address<MEMSIZE) {
		//RAM
		if (address<SVADDR && !GetFC2()){
			ExceptionGroup0(BUSERR, address, 0);
			return;
		}
		WriteB(address/* + mem1base*/, value);
		return;
	}
	if (address>=IOBASE && address<IOBASE+IOSIZE) {
		//IO
		DoIOWB(address, value);
		return;
	}
	//Unmapped
	ON_UNMAPPED(address, value);
}

/* Write word, address may not be word-aligned */
void SetMemW(unsigned long address, unsigned short value)
{	
#ifdef CHKADDRESSERR
    address &= MEMADDRMASK;
    if (address & 0x1){
		ExceptionGroup0(ADDRESSERR, address, 1);
		return;
	}
#else
    address &= MEMADDRMASKS;
#endif
	ON_WRITE(address, value);
	if (address<MEMSIZE) {
		//RAM
		if (address<SVADDR ){
			if (!GetFC2()||address<8){
				ExceptionGroup0(BUSERR, address, 0);
				return;
			}
		}		
		WriteW(address/* + mem1base*/, value);
		return;
	}
	if (address>=IOBASE && address<IOBASE+IOSIZE) {
		//IO
		DoIOWW(address, value);
		return;
	}
	//Unmapped
	ON_UNMAPPED(address, value);
}

/* Write dword, address may not be dword-aligned */
void SetMemL(unsigned long address, unsigned long value)
{
#ifdef CHKADDRESSERR
    address &= MEMADDRMASK;
    if (address & 0x1){
		ExceptionGroup0(ADDRESSERR, address, 1);
		return;
	}
#else
    address &= MEMADDRMASKS;
#endif	
	
	ON_WRITE(address, value);
	if (address<MEMSIZE) {
		//RAM
		if (address<SVADDR){
			if (!GetFC2()||address<8){
				ExceptionGroup0(BUSERR, address, 0);
				return;
			}
		}
#ifdef OPTIMIZE_VMEM		
		if ( (address >= vid_mem) && (address < (vid_mem+32768) ) ) {
			printf("vwlmem\n");
			WriteL(&videobuf[address-vid_mem], value);
		}	
		else	
#endif		
		WriteL(address/* + mem1base*/, value);
		return;
	}
	if (address>=IOBASE && address<IOBASE+IOSIZE) {
		//IO
		DoIOWL(address, value);
		return;
	}
	//Unmapped
	ON_UNMAPPED(address, value);
}





