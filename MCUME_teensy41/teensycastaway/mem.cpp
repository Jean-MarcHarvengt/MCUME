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

#include <Arduino.h>

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



PROGMEM char GetMemB(unsigned long address)
{
	address &= MEMADDRMASK;
	if (address<MEMSIZE)
		return ReadB(address + membase);
	else
	if (address<ROMBASE2)
		return -1;
	else
	if (address<IOBASE)
		return ReadB(address + rombase);
	else
	if (address<IOBASE+IOSIZE)
		return DoIORB(address);
	return -1;
}

/* Fetch word, address may not be word-aligned */
PROGMEM short GetMemW(unsigned long address)
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
		return ReadW(address + membase);
	else
	if (address<ROMBASE2)
		return -1;
	else
	if (address<IOBASE)
		return ReadW(address + rombase);
	else
	if (address<IOBASE+IOSIZE)
		return DoIORW(address);
	return -1;
}

/* Fetch dword, address may not be dword-aligned */
PROGMEM long GetMemL(unsigned long address)
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
		return ReadL(address + membase);
	else
	if (address<ROMBASE2)
		return -1;
	if (address<IOBASE)
		return ReadL(address + rombase);
	if (address<IOBASE+IOSIZE)
		return DoIORL(address);
	return -1;
}


/* Write byte to address */
PROGMEM void SetMemB (unsigned long address, unsigned char value) 
{
    address &= MEMADDRMASK;
	ON_WRITE(address, value);
	if (address<MEMSIZE) {
		//RAM
		if (address<SVADDR && !GetFC2()){
			ExceptionGroup0(BUSERR, address, 0);
			return;
		}
		WriteB(address + membase, value);
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
PROGMEM void SetMemW(unsigned long address, unsigned short value)
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
		WriteW(address + membase, value);
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
PROGMEM void SetMemL(unsigned long address, unsigned long value)
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
			WriteL(address + membase, value);
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




#ifdef UNUSED


//Simplifed versions of memory access commands used for instructions fetch
//Instruction fetch is likely only to be from ram or rom, otherwise the
//program will surely have crashed anyway!

char GetMemBpc(unsigned long address)
{
	address &= MEMADDRMASK;
	if (address<MEMSIZE) return ReadB(address + membase);
	else return ReadB(address + rombase);
}

short GetMemWpc(unsigned long address)
{
	address &= MEMADDRMASK;
	if (address<MEMSIZE) return ReadW(address + membase);
	else return ReadW(address + rombase);
}

/* Fetch dword, address may not be dword-aligned */
long GetMemLpc(unsigned long address)
{
	address &= MEMADDRMASK;
	if (address<MEMSIZE) return ReadL(address + membase);
	else return ReadL(address + rombase);
}

//Movep support
//-------------

void SetMemPW(unsigned long address, unsigned long value)
{
//	int8 psgctrl1,value1;

if (address&0xffff03==0xff8800) {
	uint32 psgctrl1=(value>>8)&15;
	uint32 value1=value&255;
	psg[psgctrl1]=value1;
	if (psgctrl1==lastpsg) sample[sampos++]=samvol[psg[8]]+samvol[psg[9]]+samvol[value1];
	else if (psgctrl1==13 || psgctrl1==12) psg_epos=0;
	return;
}
address &= MEMADDRMASK;
if (address>=IOBASE && address<IOBASE+IOSIZE) {
	//IO
	DoIOWB(address, (int8)(value>>8));
	DoIOWB(address+2, (int8)value);
	return;
}
if (address<MEMSIZE) {
	//RAM
	if (address<SVADDR && !GetFC2()) ExceptionGroup0(BUSERR, address, 0);
	address+=(uint32)membase;
	WriteB(address, (int8)(value>>8));
	WriteB(address+2, (int8)value);
	return;
}
}

void SetMemPL(unsigned long address, unsigned long value)
{//	int8 psgctrl1,psgctrl2,value1,value2;

if (address&0xffff03==0xff8800) {
	uint32 psgctrl1=(value>>24)&15;
	uint32 value1=(value>>16)&255;
	psg[psgctrl1]=value1;
	if (psgctrl1==lastpsg) sample[sampos++]=samvol[psg[8]]+samvol[psg[9]]+samvol[value1];
	else if (psgctrl1==13 || psgctrl1==12) psg_epos=0;
	psgctrl1=(value>>8)&15;
	value1=value&255;
	if (psgctrl1==lastpsg) sample[sampos++]=samvol[psg[8]]+samvol[psg[9]]+samvol[value1];
	else if (psgctrl1==13 || psgctrl1==12) psg_epos=0;
	return;
}
address &= MEMADDRMASK;
if (address>=IOBASE && address<IOBASE+IOSIZE) {
	//IO
	DoIOWB(address, (int8)(value>>24));
	DoIOWB(address+2, (int8)(value>>16));
	DoIOWB(address+4, (int8)(value>>8));
	DoIOWB(address+6, (int8)value);
	return;
}
if (address<MEMSIZE) {
	//RAM
	if (address<SVADDR && !GetFC2()) ExceptionGroup0(BUSERR, address, 0);
	address+=(uint32)membase;
	WriteB(address, (int8)(value>>24));
	WriteB(address+2, (int8)(value>>16));
	WriteB(address+4, (int8)(value>>8));
	WriteB(address+6, (int8)value);
	return;
}
}



static void *__rombase=NULL, *__membase=NULL;

void MemQuit(void)
{
	if (__membase)
		free(__membase);
	__membase=NULL;
	if (__rombase)
		free(__rombase);
	__rombase=NULL;
}

static void _set_dword(void *buf,unsigned dat)
{
	unsigned short *b=(unsigned short *)buf;
	*b++=dat&0xffff;
	*b=dat>>16;
}

void MemClean(void)
{
	membase=(int8*)__membase;
	memset(membase,0,MEMSIZE);
	//savestate_init();
	memcpy (membase, ((int8*)__rombase)+rombase_pos, 8);

	_set_dword(membase+0x420,0x752019f3);
	_set_dword(membase+0x43a,0x237698aa);
	_set_dword(membase+0x51a,0x5555aaaa);
#if MEMSIZE==0x00080000
	_set_dword(membase+0x436,0x80000-0x8000);
	_set_dword(membase+0x42e,0x80000);
	WriteB(membase+0x425,1);
	WriteB(rombase+0xff8000,1);
	memconf=1;
#else
#if MEMSIZE==0x00100000
	_set_dword(membase+0x436,0x100000-0x8000);
	_set_dword(membase+0x42e,0x100000);
	WriteB(membase+0x425,5);
	WriteB(rombase+0xff8000,5);
	memconf=5;
#else
#if MEMSIZE==0x00200000
	_set_dword(membase+0x436,0x200000-0x8000);
	_set_dword(membase+0x42e,0x200000);
	WriteB(membase+0x425,2);
	WriteB(rombase+0xff8000,2);
	memconf=2;
#else
#if MEMSIZE==0x00400000
	_set_dword(membase+0x436,0x400000-0x8000);
	_set_dword(membase+0x42e,0x400000);
	WriteB(membase+0x425,0xA);
	WriteB(rombase+0xff8000,0xA);
	memconf=0xA;
#else
#error DCaSTaway ERROR: MEMSIZE incorrect.
#endif
#endif
#endif
#endif
	_set_dword(membase+0x4c2,3);
	WriteW(membase+0x4a6,2);

	//if (TosCountry)
 	//	vid_syncmode=2;
	//else
	//	vid_syncmode=0;
}


static unsigned actual_rom_crc=0;

static unsigned rom_checksum(void)
{
	int n;
	unsigned crc=0;
	for(n=ROMBASE2;n<MEMADDRSIZE;n++)
		crc+=(n+1)*rombase[n];
	return crc;
}

void MemReInit(void)
{
	unsigned crc=rom_checksum();
	if (crc!=actual_rom_crc)
		MemInit();
	else
		MemClean();
}

PROGMEM int MemInit(void)
{
	int n;
	uint8 val1,val2;
	unsigned long len;
	FILE *roms;

    //Load ROM
	if (NULL == (roms = fopen (rom, "rb"))) {
#ifdef  DREAMCAST
		reinit_sdcard();
		if (NULL == (roms = fopen (rom_sd, "rb"))) 
#endif
		{
			if (__membase)
				memset(__membase,0,MEMSIZE);
			return 1;
		}
	}
	MemQuit();
	fseek(roms,0,SEEK_END);
	len=(unsigned long)ftell(roms);
	fseek(roms,0,SEEK_SET);
	if (len>(MEMADDRSIZE-ROMBASE2))
		len=(MEMADDRSIZE-ROMBASE2);
	if (len<=(192*1024))
		rombase_pos=ROMBASE-ROMBASE2;
	else
		rombase_pos=0;

	if (!__rombase)
		__rombase = calloc(1,MEMADDRSIZE-ROMBASE2);
	rombase=(int8*)__rombase;
	for(n=0;n<(MEMADDRSIZE-ROMBASE2);n+=2)
	{
		rombase[n]=0x4e;
		rombase[n+1]=0x71;
	}
	if (len != fread(rombase+rombase_pos,1,len,roms))
	{
		fclose(roms);
		return 2;
	}
	fclose (roms);
#ifdef BYTES_SWAP
	for (n=0; n<(MEMADDRSIZE-ROMBASE2); n+=2) {
		val1 = rombase[n];
		val2 = rombase[n+1];
		rombase[n] = val2;
		rombase[n+1] =val1;
	}
#endif
	/* precalculate rombase - now just address-adding happens later */
	if (rombase_pos)
	{
		rombase -= ROMBASE2+rombase_pos-(ROMBASE-ROMBASE2);
		memcpy(rombase+ROMBASE2,rombase+ROMBASE,(MEMADDRSIZE-ROMBASE));
	}
	else
	{
		rombase -= ROMBASE2;
		memcpy(rombase+ROMBASE,rombase+ROMBASE2,(MEMADDRSIZE-ROMBASE));
	}

	TOS_FixRom((uint8 *)(((int8*)__rombase)+rombase_pos));

	//Allocate and clear RAM
	if (!__membase)
		__membase = (int8*)calloc(1,MEMSIZE+0x10);
	MemClean();
	actual_rom_crc=rom_checksum();
	initialize_memmap();
	return 0;
}
#endif
