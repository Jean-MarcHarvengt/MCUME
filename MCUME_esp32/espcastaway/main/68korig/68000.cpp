/*
* Castaway
*  (C) 1994 - 2002 Martin Doering, Joachim Hoenig
*
* 68000.c - 68000 emulator jump table and misc subroutines
*
* This file is distributed under the GPL, version 2 or at your
* option any later version.  See doc/license.txt for details.
*
* revision history
*  23.05.2002  JH  FAST1.0.1 code import: KR -> ANSI, restructuring
*  09.06.2002  JH  Use of mmap for memory access discontinued
*  12.06.2002  JH  Correct bus error/address error exception stack frame
*  13.06.2002  JH  Merged in Martin's BCD implementation (untested),
*                  completed jump table. STOP is now the only
*                  unimplemented instruction.
*  14.06.2002  JH  Implemented STOP, shutdown CPU after multiple bus errors.
*                  Removed inst parameter from CPU opcode functions.
*  19.06.2002  JH  CPURun() returns # of executed instructions.
*  20.06.2002  JH  added yet another SR implementation variant.
*  02.07.2002  JH  Support different CPU types. Removed MOVE CCR,<EA> from
*                  68000 jump table.
*  20.08.2002  JH  Fixed CPU shutdown.
*  27.08.2002  JH  Bugfix: S and T flag incorrectly reported for group 0 exceptions
*                  Implemented additional 68010 registers and instructions.
*  31.08.2002  JH  Implemented M68010 exception stack frames.
*  08.10.2002  JH  Implemented Trace exception
*/
static char     sccsid[] = "$Id: 68000.c,v 1.18 2002/10/10 19:52:11 jhoenig Exp $";
#include "config.h"

#include <stdio.h>
#include <assert.h>
#include <setjmp.h>
#include "68000.h"
#include "mem.h"
#include "st.h"
#include "op68k.h"
#include "proto.h"
#include "operin.h"


#if (CPU_TYPE == 68000 || CPU_TYPE == 68008)
#include "op68000.h"
#elif (CPU_TYPE == 68010) // broken
#include "op68010.h"
#endif
extern int disass;
unsigned long   reg[23];
unsigned long   dfc, sfc, vbr;
#define biginst reg[18]
#define pc reg[19]
#define usp reg[20]
#define ssp reg[21]
#define recalc_int reg[22]


extern void            StartDisass();
unsigned short SaveWordTrc;
unsigned char GetTrc;
unsigned short int0=0;
unsigned long exaddress=0x12345678;
int             intmask, intpri;
char            cpu_state;
unsigned        cpu_type;
volatile unsigned cpu_count;

jmp_buf         trap_buf;


unsigned short GetSRB(void)
{	
	uint32 cvnz=reg[16];   //-------c-------v-------n-------z
	uint32 stx=reg[17];    //---------------s-------t-------x
	uint32 sr=(cvnz>>24)+(cvnz>>15)+(cvnz<<2)+(cvnz>>5)+(stx<<4);
	return sr&(unsigned short)0xff;
}

unsigned short           GetSRW(void)
{
	uint32 cvnz=reg[16];   //-------c-------v-------n-------z
	uint32 stx=reg[17];    //---------------s-------t-------x
	uint32 sr=(cvnz>>24)+(cvnz>>15)+(cvnz<<2)+(cvnz>>5)+(stx<<4);
	uint32 sr2=(stx<<7)+(stx>>3)+(intmask<<8);
	return (sr&(unsigned short)0xff)+(sr2&(unsigned short)0xff00);
}


void SetSRB(unsigned long sr)
{
	uint32 cvnz=((sr&1)<<24)+((sr&2)<<15)+((sr&4)>>2)+((sr&8)<<5);
	reg[16]=cvnz;
    SetX(sr&0x10);
}

void SetSRW(unsigned long sr)
{
    uint32 cvnz=((sr&1)<<24)+((sr&2)<<15)+((sr&4)>>2)+((sr&8)<<5);
	reg[16]=cvnz;
	SetX (sr&0x10);
    SetI ((sr&0x700)>>8);
    SetS (sr&0x2000);
    SetT (sr&0x8000);
}

void            ExceptionGroup0(
								int number,
								unsigned long address,
								int ReadWrite)
{
    short sr = GetSRW(), context = 0;
#ifdef DEBUG
    ON_TRAP(number);
    assert(cpu_state != -3);
#endif
	if ((exaddress+1!=address)&&(exaddress+2!=address)&&(exaddress+3!=address)){
		if (cpu_state == 0) {
			cpu_state = -3;
			longjmp(trap_buf, 1);
		}
#if CPU_TYPE == 68000
		else if (cpu_state > 0) {
			context |= 0x8;
		}
		if (ReadWrite) context |= 0x10;
#else
		if (ReadWrite) context |= 0x100;
#endif
		if (GetS()) context |= 0x4;
		if (ReadWrite && address == pc) context |= 0x2;
		else context |= 0x1;
		cpu_state = 0; /* begin group 0 exception processing */
		SetS (1);
		SetT (0);
#if CPU_TYPE == 68010
		reg[15] -= 44; /* Rerun info */
		reg[15] -= 4; SetMemL(reg[15], address); /* fault address */
		reg[15] -= 2; SetMemW(reg[15], context);
		reg[15] -= 2; SetMemW(reg[15], 0x8000 | (number * 4));
		reg[15] -= 4; SetMemL(reg[15], GetPC());
		reg[15] -= 2; SetMemW(reg[15], sr);
#else
		reg[15] = reg[15] - 14;
		SetMemW(reg[15], context);
		SetMemL(reg[15] + 2, address);
		SetMemW(reg[15] + 6, (uint16)biginst);
		SetMemW(reg[15] + 8, sr);
		SetMemL(reg[15] + 10, GetPC());
#endif
		
		SetPC (GetMemL ((long) number * 4) + vbr);
		/* end exception processing */
		cpu_state = -1;
		exaddress=address;
		
	}
	int0=1;
}

void ExceptionGroup1(int number)
{
	
	uint32 sp, newpc=pc;
    short sr = GetSRW();
	
    if (number!=TRAPV) newpc-=2;
    SetS(1); 
	SetT(0);
    sp=reg[15];
    sp-=4; SetMemL(sp, newpc);
    sp-=2; SetMemW(sp, sr);
	reg[15]=sp;
    SetPC (GetMemL(number*4+vbr));
	int0=1;
}

void Interrupt(int number, int level)
{
	uint32 sp;
    short sr = GetSRW();
    SetI(level); SetS(1); SetT(0);
	sp=reg[15];
    sp-=4; SetMemL(sp, GetPC());
    sp-=2; SetMemW(sp, sr);
	reg[15]=sp;
    SetPC (GetMemL(number*4+vbr));
}

void ExceptionGroup2(int number)
{
	uint32 sp;
    short sr = GetSRW();
    SetS(1); SetT(0);
	sp=reg[15];
    sp-=4; SetMemL(sp, GetPC());
    sp-=2; SetMemW(sp, sr);
	reg[15]=sp;
    SetPC (GetMemL(number*4+vbr));
}

unsigned long Trace()
{
	uint32 sp;
	short sr;
	register unsigned long cycleco=0;
	register unsigned long address,timeinst;
	register int8 *mymembase=membase;
	register int8 *myrombase=rombase;
	register uint32 myinst;
	address = pc&MEMADDRMASK;
	//	disass=1;
	int0=0;
	if (address<MEMSIZE) myinst=biginst=ReadSL(mymembase+address);
	else if (address>=ROMBASE) myinst=biginst=ReadSL(myrombase+address);
	else { ExceptionGroup1(ADDRESSERR); return cycleco; }
#ifdef DISASS
	if (disass==1) StoreTrace();
#endif
	pc+=2;
	cycleco+=(*jmp_table[(myinst<<16)>>19])(reg);
	
	if (!int0){
		sr = GetSRW();
		SetS(1); SetT(0);
		sp=reg[15];
		sp-=4; SetMemL(sp, GetPC());
		sp-=2; SetMemW(sp, sr);
		reg[15]=sp;
		SetPC (GetMemL(TRACE*4+vbr));
		return cycleco;
	}
	return 0;
	
}


unsigned long            IllIns(operin)
{
    ExceptionGroup1(ILLINSTR);
	return 0;
}

unsigned long            Line_A(operin)
{
    ExceptionGroup1(LINE_A);
	return 0;
}

unsigned long            Line_F(operin)
{
    ExceptionGroup1(LINE_F);
	return 0;
}

unsigned long            Stop(void)
{
    if (!GetS())
        ExceptionGroup1(PRIV);
    SetSRW(GetMPCW());
    pc += 2;
    cpu_state = -2; /* stopped */
    //longjmp(trap_buf, 1);
	return 0;
}

void            HWReset(void)
{
    reg[15] = GetMemL (0l);
    SetPC(GetMemL (4l));
    SetI(7);
    SetT(0);
    SetS(1);
    cpu_state = -1; /* running */
    cpu_type = CPU_TYPE;
}
