/*
 * Castaway
 *  (C) 1994 - 2002 Martin Doering, Joachim Hoenig
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#ifndef _68000H
#define _68000H

#include "config.h"     // wg. define
#include "operin.h"

/*
 * Supported CPU types
 * CPU                  CPU_TYPE
 * MC68000              68000
 * MC68008              68000
 * MC68EC000            not supported (yet)
 * MC68010              68010
 * MC68020              68020
 * MC68EC020            not supported (yet)
 * MC68030              not supported (yet)
 * MC68EC030            not supported (yet)
 * MC68040              not supported (yet)
 * MC68EC040            not supported (yet)
 * CPU32                not supported (yet)
 */
#define CPU_TYPE        68000

/*
 * Exception vector numbers (trap numbers).
 */
#define RESET           0
#define BUSERR          2
#define ADDRESSERR      3
#define ILLINSTR        4
#define DIVZ            5
#define TRAPCHK         6
#define TRAPV           7
#define PRIV            8
#define TRACE           9
#define LINE_A          10
#define LINE_F          11
#if CPU_TYPE != 68000
#define FORMATERR       14
#endif
#define AUTOINT1        25
#define AUTOINT2        26
#define AUTOINT3        27
#define AUTOINT4        28
#define AUTOINT5        29
#define AUTOINT6        30
#define AUTOINT7        31
#define TRAP0           32 + 0
#define TRAP1           32 + 1
#define TRAP2           32 + 2
#define TRAP3           32 + 3
#define TRAP4           32 + 4
#define TRAP5           32 + 5
#define TRAP6           32 + 6
#define TRAP7           32 + 7
#define TRAP8           32 + 8
#define TRAP9           32 + 9
#define TRAP10          32 + 10
#define TRAP11          32 + 11
#define TRAP12          32 + 12
#define TRAP13          32 + 13
#define TRAP14          32 + 14
#define TRAP15          32 + 15

/* Group 0 exceptions (address error / bus error) */
extern void     ExceptionGroup0(
        int number,             /* trap number */
        unsigned long address,  /* fault address */
        int ReadWrite);         /* read = true, write = false */
/* Group 1 exceptions: illegal instruction, privilege violation, interrupts */
extern void     ExceptionGroup1(int number);
extern void     Interrupt(int number, int level);
/* Group 2 exceptions: traps, divide by zero */
extern void     ExceptionGroup2(int number);

extern void     HWReset(void);  /* Reset */
extern unsigned long     IllIns(operin);   /* Illegal instruction */
extern unsigned long     Line_A(operin);   /* Axxx instruction opcode  */
extern unsigned long     Line_F(operin);   /* Fxxx instruction opcode */
extern unsigned long     Stop(void);     /* Stop instruction */

extern unsigned char tracemode;	//For Trace Switching
extern unsigned short SaveWordTrc;
extern unsigned char GetTrc;

/*
 * CPU Registers
 *
 * WARNING:
 * Never access registers directly from outside the emulator.
 * The internal representation of registers may change.
 */
/* Data and address registers are stored in native byte order.
 * register numbering: 0=d0, 1=d1, ..., 8=a0, 9=a1, ... 15=a7 */
extern unsigned long reg[23];
#define biginst reg[18]
#define pc reg[19]
#define usp reg[20]
#define ssp reg[21]
#define recalc_int reg[22]


#define ins7 (biginst & 7)
#define ins15 (biginst & 15)

/* 68010 registers */
extern unsigned long    dfc, sfc, vbr;
/* 68020 registers */
extern unsigned long    caar, cacr, isp, msp;

extern int      intmask;
extern char     cpu_state;      /* 0, 1, 2 while processing exceptions,
                                 * -1 executing, -2 stopped, -3 shutdown */
extern unsigned cpu_type;       /* 68000, 68010 */

/*
 * Interrupts
 *
 * The state of interrupt lines IPL0-IPL2 is stored in the variable
 * intpri. The emulator acknowledges interrupts by calling QueryIRQ().
 * It expects the interrupt vector number to be returned.
 * WARNING:
 * Never update intpri asynchronously. Use the CPUEvent() function
 * to force CPURun() to return before updating intpri.
 */
extern int      intpri;
extern int      QueryIRQ(int level); /* get interrupt vector number */

#define GetS() *(((int8*)reg)+70)
#define GetFC2() GetS()
#define GetFC1() address == pc
#define GetFC0() address != pc
#define GetFC() (((GetFC2() << 1) | GetFC1()) << 1) | GetFC0()

/*
 * memory access jump tables
 */
#define MEMADDRMASK 0x00ffffffl
#define MEMADDRMASKS 0x00fffffel
#define MEMIDXSHIFT 11
#define MEMADDRSIZE (MEMADDRMASK + 1)
#define MEMTABLESIZE (MEMADDRSIZE >> MEMIDXSHIFT)
#define MEMBANKSIZE (MEMADDRSIZE / MEMTABLESIZE)
#define MEMBANKMASK (MEMBANKSIZE - 1)

/* Fetch byte from address */
char GetMemB(unsigned long address);
short GetMemW(unsigned long address);
long GetMemL(unsigned long address);
void SetMemB (unsigned long address, unsigned char value);
void SetMemW(unsigned long address, unsigned short value);
void SetMemL(unsigned long address, unsigned long value);
#ifdef DEBUG
extern int             stop_on;
extern int             verb_on;
/* go to sleep (gives debugger a chance to attach) */
void            DebugStop(void);
#ifdef TRACEBACK
/*
 * execution trace
 */
struct state {
    uint32          reg[16];
    uint32          stack[8];
    uint32          pc, usp, ssp;
    uint16          inst[5];
    uint16          sr;

};
extern int             hide_supervisor;
extern unsigned long   instcnt;
extern int             tbi;
extern struct state    traceback[TRACEBACK];
extern int             trace_on;
/* print execution trace of last TRACEBACK instructions and sleep */
void            TraceStop(void);
#endif
#endif
#endif

