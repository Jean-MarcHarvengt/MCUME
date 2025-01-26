/*****************************************************************************
 *
 *	 z80.c
 *	 Portable Z80 emulator V2.7
 *
 *	 Copyright (C) 1998,1999,2000 Juergen Buchmueller, all rights reserved.
 *
 *	 - This source code is released as cpu_readmapfreeware for non-commercial purposes.
 *	 - You are free to use and redistribute this code in modified or
 *	   unmodified form, provided you list me in the credits.
 *	 - If you modify this source code, you must add a notice to each modified
 *	   source file that it has been changed.  If you're a nice person, you
 *	   will clearly mark each change too.  :)
 *	 - If you wish to use this for commercial purposes, please contact me at
 *	   pullmoll@t-online.de
 *	 - The author of this copywritten work reserves the right to change the
 *     terms of its usage and license at any time, including retroactively
 *   - This entire notice must remain in the source code.
 *
 *	 Changes in 2.7:
 *	  - removed z80_vm specific code, it's not needed (and never was).
 *   Changes in 2.6:
 *	  - BUSY_LOOP_HACKS needed to call change_pc16() earlier, before
 *		checking the opcodes at the new address, because otherwise they
 *		might access the old (wrong or even NULL) banked memory region.
 *		Thanks to Sean Young for finding this nasty bug.
 *   Changes in 2.5:
 *	  - Burning cycles always adjusts the ICount by a multiple of 4.
 *	  - In REPEAT_AT_ONCE cases the R register wasn't incremented twice
 *		per repetition as it should have been. Those repeated opcodes
 *		could also underflow the ICount.
 *	  - Simplified TIME_LOOP_HACKS for BC and added two more for DE + HL
 *		timing loops. I think those hacks weren't endian safe before too.
 *   Changes in 2.4:
 *	  - z80_reset zaps the entire context, sets IX and IY to 0xffff(!) and
 *		sets the Z flag. With these changes the Tehkan World Cup driver
 *		_seems_ to work again.
 *   Changes in 2.3:
 *	  - External termination of the execution loop calls z80_burn() and
 *		z80_vm_burn() to burn an amount of cycles (R adjustment)
 *	  - Shortcuts which burn CPU cycles (BUSY_LOOP_HACKS and TIME_LOOP_HACKS)
 *		now also adjust the R register depending on the skipped opcodes.
 *   Changes in 2.2:
 *	  - Fixed bugs in CPL, SCF and CCF instructions flag handling.
 *	  - Changed variable EA and ARG16() function to UINT32; this
 *		produces slightly more efficient code.
 *	  - The DD/FD XY CB opcodes where XY is 40-7F and Y is not 6/E
 *		are changed to calls to the X6/XE opcodes to reduce object size.
 *		They're hardly ever used so this should not yield a speed penalty.
 *	 New in 2.0:
 *    - Optional more exact Z80 emulation (#define Z80_EXACT 1) according
 *		to a detailed description by Sean Young which can be found at:
 *		http://www.msxnet.org/tech/Z80/z80undoc.txt
 *****************************************************************************/

#include "cpuintrf.h"
#include "shared.h"


#include "shared.h"
extern void cpu_writemem16(int address, int data);
extern void cpu_writeport(int port, int data);
extern int cpu_readport(int port);
unsigned char *cpu_readmap[8];
unsigned char *cpu_writemap[8];

#define cpu_readmem16(a)  ((cpu_readmap[(a) >> 13] <0x80000)?read_rom(cpu_readmap[(a) >> 13]+(a & 0x1FFF)):cpu_readmap[(a) >> 13][(a) & 0x1FFF])
#define cpu_readop(a)     ((cpu_readmap[(a) >> 13] <0x80000)?read_rom(cpu_readmap[(a) >> 13]+(a & 0x1FFF)):cpu_readmap[(a) >> 13][(a) & 0x1FFF])
#define cpu_readop_arg(a) ((cpu_readmap[(a) >> 13] <0x80000)?read_rom(cpu_readmap[(a) >> 13]+(a & 0x1FFF)):cpu_readmap[(a) >> 13][(a) & 0x1FFF])

//#define cpu_readmem16(a)        cpu_readmap[(a) >> 13][(a) & 0x1FFF]
//#define cpu_readop(a)           cpu_readmap[(a) >> 13][(a) & 0x1FFF]
//#define cpu_readop_arg(a)       cpu_readmap[(a) >> 13][(a) & 0x1FFF]

/* execute main opcodes inside a big switch statement */
#ifndef BIG_SWITCH
#define BIG_SWITCH              1
#endif

/* big flags array for ADD/ADC/SUB/SBC/CP results */
#define BIG_FLAGS_ARRAY         1

/* Set to 1 for a more exact (but somewhat slower) Z80 emulation */
#define Z80_EXACT               1

/* repetitive commands (ldir,cpdr etc.) repeat at
   once until cycles used up or B(C) counted down. */
#define REPEAT_AT_ONCE		1

/* on JP and JR opcodes check for tight loops */
#define BUSY_LOOP_HACKS 	1

/* check for delay loops counting down BC */
#define TIME_LOOP_HACKS 	1

/* #ifdef X86_ASM */
#undef	BIG_FLAGS_ARRAY
#define BIG_FLAGS_ARRAY 	0
/*#endif */

/*
#ifdef PSX
#undef  BIG_FLAGS_ARRAY
#define BIG_FLAGS_ARRAY         0
#endif
*/

#define CF  0x01
#define NF	0x02
#define PF	0x04
#define VF	PF
#define XF	0x08
#define HF	0x10
#define YF	0x20
#define ZF	0x40
#define SF	0x80

#define INT_IRQ 0x01
#define NMI_IRQ 0x02

#define	_PPC	Z80.PREPC.d		/* previous program counter */

#define _PCD	Z80.PC.d
#define _PC 	Z80.PC.w.l

#define _SPD	Z80.SP.d
#define _SP 	Z80.SP.w.l

#define _AFD	Z80.AF.d
#define _AF 	Z80.AF.w.l
#define _A		Z80.AF.b.h
#define _F		Z80.AF.b.l

#define _BCD	Z80.BC.d
#define _BC 	Z80.BC.w.l
#define _B		Z80.BC.b.h
#define _C		Z80.BC.b.l

#define _DED	Z80.DE.d
#define _DE 	Z80.DE.w.l
#define _D		Z80.DE.b.h
#define _E		Z80.DE.b.l

#define _HLD	Z80.HL.d
#define _HL 	Z80.HL.w.l
#define _H		Z80.HL.b.h
#define _L		Z80.HL.b.l

#define _IXD	Z80.IX.d
#define _IX 	Z80.IX.w.l
#define _HX 	Z80.IX.b.h
#define _LX 	Z80.IX.b.l

#define _IYD	Z80.IY.d
#define _IY 	Z80.IY.w.l
#define _HY 	Z80.IY.b.h
#define _LY 	Z80.IY.b.l

#define _I      Z80.I
#define _R      Z80.R
#define _R2     Z80.R2
#define _IM     Z80.IM
#define _IFF1	Z80.IFF1
#define _IFF2	Z80.IFF2
#define _HALT	Z80.HALT

int z80_ICount;
static Z80_Regs Z80;
Z80_Regs *Z80_Context = &Z80;
static UINT32 EA;
int after_EI = 0;

static UINT8 SZ[256];		/* zero and sign flags */
static UINT8 SZ_BIT[256];	/* zero, sign and parity/overflow (=zero) flags for BIT opcode */
static UINT8 SZP[256];		/* zero, sign and parity flags */
static UINT8 SZHV_inc[256]; /* zero, sign, half carry and overflow flags INC r8 */
static UINT8 SZHV_dec[256]; /* zero, sign, half carry and overflow flags DEC r8 */
#include "z80daa.h"
/*
#if BIG_FLAGS_ARRAY
#include <signal.h>
static UINT8 *SZHVC_add = 0;
static UINT8 *SZHVC_sub = 0;
#endif
*/

#if Z80_EXACT
/* tmp1 value for ini/inir/outi/otir for [C.1-0][io.1-0] */
static UINT8 irep_tmp1[4][4] = {
	{0,0,1,0},{0,1,0,1},{1,0,1,1},{0,1,1,0}
};

/* tmp1 value for ind/indr/outd/otdr for [C.1-0][io.1-0] */
static UINT8 drep_tmp1[4][4] = {
	{0,1,0,0},{1,0,0,1},{0,0,1,0},{0,1,0,1}
};

/* tmp2 value for all in/out repeated opcodes for B.7-0 */
static UINT8 breg_tmp2[256] = {
	0,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,
	0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,
	1,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,
	1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,
	0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,
	1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,
	0,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,
	0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,
	1,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,
	1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,
	0,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,
	0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,
	1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,
	0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,
	1,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,
	1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1
};
#endif

static const UINT8 cc_op[0x100] = {
 4,10, 7, 6, 4, 4, 7, 4, 4,11, 7, 6, 4, 4, 7, 4,
 8,10, 7, 6, 4, 4, 7, 4,12,11, 7, 6, 4, 4, 7, 4,
 7,10,16, 6, 4, 4, 7, 4, 7,11,16, 6, 4, 4, 7, 4,
 7,10,13, 6,11,11,10, 4, 7,11,13, 6, 4, 4, 7, 4,
 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
 7, 7, 7, 7, 7, 7, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
 5,10,10,10,10,11, 7,11, 5, 4,10, 0,10,10, 7,11,
 5,10,10,11,10,11, 7,11, 5, 4,10,11,10, 0, 7,11,
 5,10,10,19,10,11, 7,11, 5, 4,10, 4,10, 0, 7,11,
 5,10,10, 4,10,11, 7,11, 5, 6,10, 4,10, 0, 7,11};


static const UINT8 cc_cb[0x100] = {
 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
 8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
 8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
 8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
 8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8};

static const UINT8 cc_dd[0x100] = {
 4, 4, 4, 4, 4, 4, 4, 4, 4,15, 4, 4, 4, 4, 4, 4,
 4, 4, 4, 4, 4, 4, 4, 4, 4,15, 4, 4, 4, 4, 4, 4,
 4,14,20,10, 9, 9, 9, 4, 4,15,20,10, 9, 9, 9, 4,
 4, 4, 4, 4,23,23,19, 4, 4,15, 4, 4, 4, 4, 4, 4,
 4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
 4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
 9, 9, 9, 9, 9, 9,19, 9, 9, 9, 9, 9, 9, 9,19, 9,
19,19,19,19,19,19, 4,19, 4, 4, 4, 4, 9, 9,19, 4,
 4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
 4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
 4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
 4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4,
 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
 4,14, 4,23, 4,15, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4,
 4, 4, 4, 4, 4, 4, 4, 4, 4,10, 4, 4, 4, 4, 4, 4};

/* dd/fd cycles are identical */
#define cc_fd cc_dd

static const UINT8 cc_xxcb[0x100] = {
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23};

static const UINT8 cc_ed[0x100] = {
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
12,12,15,20, 8, 8, 8, 9,12,12,15,20, 8, 8, 8, 9,
12,12,15,20, 8, 8, 8, 9,12,12,15,20, 8, 8, 8, 9,
12,12,15,20, 8, 8, 8,18,12,12,15,20, 8, 8, 8,18,
12,12,15,20, 8, 8, 8, 8,12,12,15,20, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
16,16,16,16, 8, 8, 8, 8,16,16,16,16, 8, 8, 8, 8,
16,16,16,16, 8, 8, 8, 8,16,16,16,16, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};

static void take_interrupt(void);

#define PROTOTYPES(tablename,prefix) \
    static __inline__ void prefix##_00(void); static __inline__ void prefix##_01(void); static __inline__ void prefix##_02(void); static __inline__ void prefix##_03(void); \
    static __inline__ void prefix##_04(void); static __inline__ void prefix##_05(void); static __inline__ void prefix##_06(void); static __inline__ void prefix##_07(void); \
    static __inline__ void prefix##_08(void); static __inline__ void prefix##_09(void); static __inline__ void prefix##_0a(void); static __inline__ void prefix##_0b(void); \
    static __inline__ void prefix##_0c(void); static __inline__ void prefix##_0d(void); static __inline__ void prefix##_0e(void); static __inline__ void prefix##_0f(void); \
    static __inline__ void prefix##_10(void); static __inline__ void prefix##_11(void); static __inline__ void prefix##_12(void); static __inline__ void prefix##_13(void); \
    static __inline__ void prefix##_14(void); static __inline__ void prefix##_15(void); static __inline__ void prefix##_16(void); static __inline__ void prefix##_17(void); \
    static __inline__ void prefix##_18(void); static __inline__ void prefix##_19(void); static __inline__ void prefix##_1a(void); static __inline__ void prefix##_1b(void); \
    static __inline__ void prefix##_1c(void); static __inline__ void prefix##_1d(void); static __inline__ void prefix##_1e(void); static __inline__ void prefix##_1f(void); \
    static __inline__ void prefix##_20(void); static __inline__ void prefix##_21(void); static __inline__ void prefix##_22(void); static __inline__ void prefix##_23(void); \
    static __inline__ void prefix##_24(void); static __inline__ void prefix##_25(void); static __inline__ void prefix##_26(void); static __inline__ void prefix##_27(void); \
    static __inline__ void prefix##_28(void); static __inline__ void prefix##_29(void); static __inline__ void prefix##_2a(void); static __inline__ void prefix##_2b(void); \
    static __inline__ void prefix##_2c(void); static __inline__ void prefix##_2d(void); static __inline__ void prefix##_2e(void); static __inline__ void prefix##_2f(void); \
    static __inline__ void prefix##_30(void); static __inline__ void prefix##_31(void); static __inline__ void prefix##_32(void); static __inline__ void prefix##_33(void); \
    static __inline__ void prefix##_34(void); static __inline__ void prefix##_35(void); static __inline__ void prefix##_36(void); static __inline__ void prefix##_37(void); \
    static __inline__ void prefix##_38(void); static __inline__ void prefix##_39(void); static __inline__ void prefix##_3a(void); static __inline__ void prefix##_3b(void); \
    static __inline__ void prefix##_3c(void); static __inline__ void prefix##_3d(void); static __inline__ void prefix##_3e(void); static __inline__ void prefix##_3f(void); \
    static __inline__ void prefix##_40(void); static __inline__ void prefix##_41(void); static __inline__ void prefix##_42(void); static __inline__ void prefix##_43(void); \
    static __inline__ void prefix##_44(void); static __inline__ void prefix##_45(void); static __inline__ void prefix##_46(void); static __inline__ void prefix##_47(void); \
    static __inline__ void prefix##_48(void); static __inline__ void prefix##_49(void); static __inline__ void prefix##_4a(void); static __inline__ void prefix##_4b(void); \
    static __inline__ void prefix##_4c(void); static __inline__ void prefix##_4d(void); static __inline__ void prefix##_4e(void); static __inline__ void prefix##_4f(void); \
    static __inline__ void prefix##_50(void); static __inline__ void prefix##_51(void); static __inline__ void prefix##_52(void); static __inline__ void prefix##_53(void); \
    static __inline__ void prefix##_54(void); static __inline__ void prefix##_55(void); static __inline__ void prefix##_56(void); static __inline__ void prefix##_57(void); \
    static __inline__ void prefix##_58(void); static __inline__ void prefix##_59(void); static __inline__ void prefix##_5a(void); static __inline__ void prefix##_5b(void); \
    static __inline__ void prefix##_5c(void); static __inline__ void prefix##_5d(void); static __inline__ void prefix##_5e(void); static __inline__ void prefix##_5f(void); \
    static __inline__ void prefix##_60(void); static __inline__ void prefix##_61(void); static __inline__ void prefix##_62(void); static __inline__ void prefix##_63(void); \
    static __inline__ void prefix##_64(void); static __inline__ void prefix##_65(void); static __inline__ void prefix##_66(void); static __inline__ void prefix##_67(void); \
    static __inline__ void prefix##_68(void); static __inline__ void prefix##_69(void); static __inline__ void prefix##_6a(void); static __inline__ void prefix##_6b(void); \
    static __inline__ void prefix##_6c(void); static __inline__ void prefix##_6d(void); static __inline__ void prefix##_6e(void); static __inline__ void prefix##_6f(void); \
    static __inline__ void prefix##_70(void); static __inline__ void prefix##_71(void); static __inline__ void prefix##_72(void); static __inline__ void prefix##_73(void); \
    static __inline__ void prefix##_74(void); static __inline__ void prefix##_75(void); static __inline__ void prefix##_76(void); static __inline__ void prefix##_77(void); \
    static __inline__ void prefix##_78(void); static __inline__ void prefix##_79(void); static __inline__ void prefix##_7a(void); static __inline__ void prefix##_7b(void); \
    static __inline__ void prefix##_7c(void); static __inline__ void prefix##_7d(void); static __inline__ void prefix##_7e(void); static __inline__ void prefix##_7f(void); \
    static __inline__ void prefix##_80(void); static __inline__ void prefix##_81(void); static __inline__ void prefix##_82(void); static __inline__ void prefix##_83(void); \
    static __inline__ void prefix##_84(void); static __inline__ void prefix##_85(void); static __inline__ void prefix##_86(void); static __inline__ void prefix##_87(void); \
    static __inline__ void prefix##_88(void); static __inline__ void prefix##_89(void); static __inline__ void prefix##_8a(void); static __inline__ void prefix##_8b(void); \
    static __inline__ void prefix##_8c(void); static __inline__ void prefix##_8d(void); static __inline__ void prefix##_8e(void); static __inline__ void prefix##_8f(void); \
    static __inline__ void prefix##_90(void); static __inline__ void prefix##_91(void); static __inline__ void prefix##_92(void); static __inline__ void prefix##_93(void); \
    static __inline__ void prefix##_94(void); static __inline__ void prefix##_95(void); static __inline__ void prefix##_96(void); static __inline__ void prefix##_97(void); \
    static __inline__ void prefix##_98(void); static __inline__ void prefix##_99(void); static __inline__ void prefix##_9a(void); static __inline__ void prefix##_9b(void); \
    static __inline__ void prefix##_9c(void); static __inline__ void prefix##_9d(void); static __inline__ void prefix##_9e(void); static __inline__ void prefix##_9f(void); \
    static __inline__ void prefix##_a0(void); static __inline__ void prefix##_a1(void); static __inline__ void prefix##_a2(void); static __inline__ void prefix##_a3(void); \
    static __inline__ void prefix##_a4(void); static __inline__ void prefix##_a5(void); static __inline__ void prefix##_a6(void); static __inline__ void prefix##_a7(void); \
    static __inline__ void prefix##_a8(void); static __inline__ void prefix##_a9(void); static __inline__ void prefix##_aa(void); static __inline__ void prefix##_ab(void); \
    static __inline__ void prefix##_ac(void); static __inline__ void prefix##_ad(void); static __inline__ void prefix##_ae(void); static __inline__ void prefix##_af(void); \
    static __inline__ void prefix##_b0(void); static __inline__ void prefix##_b1(void); static __inline__ void prefix##_b2(void); static __inline__ void prefix##_b3(void); \
    static __inline__ void prefix##_b4(void); static __inline__ void prefix##_b5(void); static __inline__ void prefix##_b6(void); static __inline__ void prefix##_b7(void); \
    static __inline__ void prefix##_b8(void); static __inline__ void prefix##_b9(void); static __inline__ void prefix##_ba(void); static __inline__ void prefix##_bb(void); \
    static __inline__ void prefix##_bc(void); static __inline__ void prefix##_bd(void); static __inline__ void prefix##_be(void); static __inline__ void prefix##_bf(void); \
    static __inline__ void prefix##_c0(void); static __inline__ void prefix##_c1(void); static __inline__ void prefix##_c2(void); static __inline__ void prefix##_c3(void); \
    static __inline__ void prefix##_c4(void); static __inline__ void prefix##_c5(void); static __inline__ void prefix##_c6(void); static __inline__ void prefix##_c7(void); \
    static __inline__ void prefix##_c8(void); static __inline__ void prefix##_c9(void); static __inline__ void prefix##_ca(void); static __inline__ void prefix##_cb(void); \
    static __inline__ void prefix##_cc(void); static __inline__ void prefix##_cd(void); static __inline__ void prefix##_ce(void); static __inline__ void prefix##_cf(void); \
    static __inline__ void prefix##_d0(void); static __inline__ void prefix##_d1(void); static __inline__ void prefix##_d2(void); static __inline__ void prefix##_d3(void); \
    static __inline__ void prefix##_d4(void); static __inline__ void prefix##_d5(void); static __inline__ void prefix##_d6(void); static __inline__ void prefix##_d7(void); \
    static __inline__ void prefix##_d8(void); static __inline__ void prefix##_d9(void); static __inline__ void prefix##_da(void); static __inline__ void prefix##_db(void); \
    static __inline__ void prefix##_dc(void); static __inline__ void prefix##_dd(void); static __inline__ void prefix##_de(void); static __inline__ void prefix##_df(void); \
    static __inline__ void prefix##_e0(void); static __inline__ void prefix##_e1(void); static __inline__ void prefix##_e2(void); static __inline__ void prefix##_e3(void); \
    static __inline__ void prefix##_e4(void); static __inline__ void prefix##_e5(void); static __inline__ void prefix##_e6(void); static __inline__ void prefix##_e7(void); \
    static __inline__ void prefix##_e8(void); static __inline__ void prefix##_e9(void); static __inline__ void prefix##_ea(void); static __inline__ void prefix##_eb(void); \
    static __inline__ void prefix##_ec(void); static __inline__ void prefix##_ed(void); static __inline__ void prefix##_ee(void); static __inline__ void prefix##_ef(void); \
    static __inline__ void prefix##_f0(void); static __inline__ void prefix##_f1(void); static __inline__ void prefix##_f2(void); static __inline__ void prefix##_f3(void); \
    static __inline__ void prefix##_f4(void); static __inline__ void prefix##_f5(void); static __inline__ void prefix##_f6(void); static __inline__ void prefix##_f7(void); \
    static __inline__ void prefix##_f8(void); static __inline__ void prefix##_f9(void); static __inline__ void prefix##_fa(void); static __inline__ void prefix##_fb(void); \
    static __inline__ void prefix##_fc(void); static __inline__ void prefix##_fd(void); static __inline__ void prefix##_fe(void); static __inline__ void prefix##_ff(void); \
static const void (*tablename[0x100])(void) = {	\
    prefix##_00,prefix##_01,prefix##_02,prefix##_03,prefix##_04,prefix##_05,prefix##_06,prefix##_07, \
    prefix##_08,prefix##_09,prefix##_0a,prefix##_0b,prefix##_0c,prefix##_0d,prefix##_0e,prefix##_0f, \
    prefix##_10,prefix##_11,prefix##_12,prefix##_13,prefix##_14,prefix##_15,prefix##_16,prefix##_17, \
    prefix##_18,prefix##_19,prefix##_1a,prefix##_1b,prefix##_1c,prefix##_1d,prefix##_1e,prefix##_1f, \
    prefix##_20,prefix##_21,prefix##_22,prefix##_23,prefix##_24,prefix##_25,prefix##_26,prefix##_27, \
    prefix##_28,prefix##_29,prefix##_2a,prefix##_2b,prefix##_2c,prefix##_2d,prefix##_2e,prefix##_2f, \
    prefix##_30,prefix##_31,prefix##_32,prefix##_33,prefix##_34,prefix##_35,prefix##_36,prefix##_37, \
    prefix##_38,prefix##_39,prefix##_3a,prefix##_3b,prefix##_3c,prefix##_3d,prefix##_3e,prefix##_3f, \
    prefix##_40,prefix##_41,prefix##_42,prefix##_43,prefix##_44,prefix##_45,prefix##_46,prefix##_47, \
    prefix##_48,prefix##_49,prefix##_4a,prefix##_4b,prefix##_4c,prefix##_4d,prefix##_4e,prefix##_4f, \
    prefix##_50,prefix##_51,prefix##_52,prefix##_53,prefix##_54,prefix##_55,prefix##_56,prefix##_57, \
    prefix##_58,prefix##_59,prefix##_5a,prefix##_5b,prefix##_5c,prefix##_5d,prefix##_5e,prefix##_5f, \
    prefix##_60,prefix##_61,prefix##_62,prefix##_63,prefix##_64,prefix##_65,prefix##_66,prefix##_67, \
    prefix##_68,prefix##_69,prefix##_6a,prefix##_6b,prefix##_6c,prefix##_6d,prefix##_6e,prefix##_6f, \
    prefix##_70,prefix##_71,prefix##_72,prefix##_73,prefix##_74,prefix##_75,prefix##_76,prefix##_77, \
    prefix##_78,prefix##_79,prefix##_7a,prefix##_7b,prefix##_7c,prefix##_7d,prefix##_7e,prefix##_7f, \
    prefix##_80,prefix##_81,prefix##_82,prefix##_83,prefix##_84,prefix##_85,prefix##_86,prefix##_87, \
    prefix##_88,prefix##_89,prefix##_8a,prefix##_8b,prefix##_8c,prefix##_8d,prefix##_8e,prefix##_8f, \
    prefix##_90,prefix##_91,prefix##_92,prefix##_93,prefix##_94,prefix##_95,prefix##_96,prefix##_97, \
    prefix##_98,prefix##_99,prefix##_9a,prefix##_9b,prefix##_9c,prefix##_9d,prefix##_9e,prefix##_9f, \
    prefix##_a0,prefix##_a1,prefix##_a2,prefix##_a3,prefix##_a4,prefix##_a5,prefix##_a6,prefix##_a7, \
    prefix##_a8,prefix##_a9,prefix##_aa,prefix##_ab,prefix##_ac,prefix##_ad,prefix##_ae,prefix##_af, \
    prefix##_b0,prefix##_b1,prefix##_b2,prefix##_b3,prefix##_b4,prefix##_b5,prefix##_b6,prefix##_b7, \
    prefix##_b8,prefix##_b9,prefix##_ba,prefix##_bb,prefix##_bc,prefix##_bd,prefix##_be,prefix##_bf, \
    prefix##_c0,prefix##_c1,prefix##_c2,prefix##_c3,prefix##_c4,prefix##_c5,prefix##_c6,prefix##_c7, \
    prefix##_c8,prefix##_c9,prefix##_ca,prefix##_cb,prefix##_cc,prefix##_cd,prefix##_ce,prefix##_cf, \
    prefix##_d0,prefix##_d1,prefix##_d2,prefix##_d3,prefix##_d4,prefix##_d5,prefix##_d6,prefix##_d7, \
    prefix##_d8,prefix##_d9,prefix##_da,prefix##_db,prefix##_dc,prefix##_dd,prefix##_de,prefix##_df, \
    prefix##_e0,prefix##_e1,prefix##_e2,prefix##_e3,prefix##_e4,prefix##_e5,prefix##_e6,prefix##_e7, \
    prefix##_e8,prefix##_e9,prefix##_ea,prefix##_eb,prefix##_ec,prefix##_ed,prefix##_ee,prefix##_ef, \
    prefix##_f0,prefix##_f1,prefix##_f2,prefix##_f3,prefix##_f4,prefix##_f5,prefix##_f6,prefix##_f7, \
	prefix##_f8,prefix##_f9,prefix##_fa,prefix##_fb,prefix##_fc,prefix##_fd,prefix##_fe,prefix##_ff  \
}

PROTOTYPES(Z80op,op);
PROTOTYPES(Z80cb,cb);
PROTOTYPES(Z80dd,dd);
PROTOTYPES(Z80ed,ed);
PROTOTYPES(Z80fd,fd);
PROTOTYPES(Z80xxcb,xxcb);

/****************************************************************************/
/* Burn an odd amount of cycles, that is instructions taking something      */
/* different from 4 T-states per opcode (and R increment)                   */
/****************************************************************************/
static __inline__ void BURNODD(int cycles, int opcodes, int cyclesum)
{
    if( cycles > 0 )
    {
		_R += (cycles / cyclesum) * opcodes;
		z80_ICount -= (cycles / cyclesum) * cyclesum;
    }
}

/***************************************************************
 * define an opcode function
 ***************************************************************/
#define OP(prefix,opcode)  static __inline__ void prefix##_##opcode(void)

/***************************************************************
 * adjust cycle count by n T-states
 ***************************************************************/
#define CY(cycles) z80_ICount -= cycles

/***************************************************************
 * execute an opcode
 ***************************************************************/
#define EXEC(prefix,opcode) 									\
{																\
	unsigned op = opcode;										\
	CY(cc_##prefix[op]);										\
	(*Z80##prefix[op])();										\
}

#if BIG_SWITCH
#define EXEC_INLINE(prefix,opcode)								\
{																\
	unsigned op = opcode;										\
	CY(cc_##prefix[op]);										\
	switch(op)													\
	{															\
	case 0x00:prefix##_##00();break; case 0x01:prefix##_##01();break; case 0x02:prefix##_##02();break; case 0x03:prefix##_##03();break; \
	case 0x04:prefix##_##04();break; case 0x05:prefix##_##05();break; case 0x06:prefix##_##06();break; case 0x07:prefix##_##07();break; \
	case 0x08:prefix##_##08();break; case 0x09:prefix##_##09();break; case 0x0a:prefix##_##0a();break; case 0x0b:prefix##_##0b();break; \
	case 0x0c:prefix##_##0c();break; case 0x0d:prefix##_##0d();break; case 0x0e:prefix##_##0e();break; case 0x0f:prefix##_##0f();break; \
	case 0x10:prefix##_##10();break; case 0x11:prefix##_##11();break; case 0x12:prefix##_##12();break; case 0x13:prefix##_##13();break; \
	case 0x14:prefix##_##14();break; case 0x15:prefix##_##15();break; case 0x16:prefix##_##16();break; case 0x17:prefix##_##17();break; \
	case 0x18:prefix##_##18();break; case 0x19:prefix##_##19();break; case 0x1a:prefix##_##1a();break; case 0x1b:prefix##_##1b();break; \
	case 0x1c:prefix##_##1c();break; case 0x1d:prefix##_##1d();break; case 0x1e:prefix##_##1e();break; case 0x1f:prefix##_##1f();break; \
	case 0x20:prefix##_##20();break; case 0x21:prefix##_##21();break; case 0x22:prefix##_##22();break; case 0x23:prefix##_##23();break; \
	case 0x24:prefix##_##24();break; case 0x25:prefix##_##25();break; case 0x26:prefix##_##26();break; case 0x27:prefix##_##27();break; \
	case 0x28:prefix##_##28();break; case 0x29:prefix##_##29();break; case 0x2a:prefix##_##2a();break; case 0x2b:prefix##_##2b();break; \
	case 0x2c:prefix##_##2c();break; case 0x2d:prefix##_##2d();break; case 0x2e:prefix##_##2e();break; case 0x2f:prefix##_##2f();break; \
	case 0x30:prefix##_##30();break; case 0x31:prefix##_##31();break; case 0x32:prefix##_##32();break; case 0x33:prefix##_##33();break; \
	case 0x34:prefix##_##34();break; case 0x35:prefix##_##35();break; case 0x36:prefix##_##36();break; case 0x37:prefix##_##37();break; \
	case 0x38:prefix##_##38();break; case 0x39:prefix##_##39();break; case 0x3a:prefix##_##3a();break; case 0x3b:prefix##_##3b();break; \
	case 0x3c:prefix##_##3c();break; case 0x3d:prefix##_##3d();break; case 0x3e:prefix##_##3e();break; case 0x3f:prefix##_##3f();break; \
	case 0x40:prefix##_##40();break; case 0x41:prefix##_##41();break; case 0x42:prefix##_##42();break; case 0x43:prefix##_##43();break; \
	case 0x44:prefix##_##44();break; case 0x45:prefix##_##45();break; case 0x46:prefix##_##46();break; case 0x47:prefix##_##47();break; \
	case 0x48:prefix##_##48();break; case 0x49:prefix##_##49();break; case 0x4a:prefix##_##4a();break; case 0x4b:prefix##_##4b();break; \
	case 0x4c:prefix##_##4c();break; case 0x4d:prefix##_##4d();break; case 0x4e:prefix##_##4e();break; case 0x4f:prefix##_##4f();break; \
	case 0x50:prefix##_##50();break; case 0x51:prefix##_##51();break; case 0x52:prefix##_##52();break; case 0x53:prefix##_##53();break; \
	case 0x54:prefix##_##54();break; case 0x55:prefix##_##55();break; case 0x56:prefix##_##56();break; case 0x57:prefix##_##57();break; \
	case 0x58:prefix##_##58();break; case 0x59:prefix##_##59();break; case 0x5a:prefix##_##5a();break; case 0x5b:prefix##_##5b();break; \
	case 0x5c:prefix##_##5c();break; case 0x5d:prefix##_##5d();break; case 0x5e:prefix##_##5e();break; case 0x5f:prefix##_##5f();break; \
	case 0x60:prefix##_##60();break; case 0x61:prefix##_##61();break; case 0x62:prefix##_##62();break; case 0x63:prefix##_##63();break; \
	case 0x64:prefix##_##64();break; case 0x65:prefix##_##65();break; case 0x66:prefix##_##66();break; case 0x67:prefix##_##67();break; \
	case 0x68:prefix##_##68();break; case 0x69:prefix##_##69();break; case 0x6a:prefix##_##6a();break; case 0x6b:prefix##_##6b();break; \
	case 0x6c:prefix##_##6c();break; case 0x6d:prefix##_##6d();break; case 0x6e:prefix##_##6e();break; case 0x6f:prefix##_##6f();break; \
	case 0x70:prefix##_##70();break; case 0x71:prefix##_##71();break; case 0x72:prefix##_##72();break; case 0x73:prefix##_##73();break; \
	case 0x74:prefix##_##74();break; case 0x75:prefix##_##75();break; case 0x76:prefix##_##76();break; case 0x77:prefix##_##77();break; \
	case 0x78:prefix##_##78();break; case 0x79:prefix##_##79();break; case 0x7a:prefix##_##7a();break; case 0x7b:prefix##_##7b();break; \
	case 0x7c:prefix##_##7c();break; case 0x7d:prefix##_##7d();break; case 0x7e:prefix##_##7e();break; case 0x7f:prefix##_##7f();break; \
	case 0x80:prefix##_##80();break; case 0x81:prefix##_##81();break; case 0x82:prefix##_##82();break; case 0x83:prefix##_##83();break; \
	case 0x84:prefix##_##84();break; case 0x85:prefix##_##85();break; case 0x86:prefix##_##86();break; case 0x87:prefix##_##87();break; \
	case 0x88:prefix##_##88();break; case 0x89:prefix##_##89();break; case 0x8a:prefix##_##8a();break; case 0x8b:prefix##_##8b();break; \
	case 0x8c:prefix##_##8c();break; case 0x8d:prefix##_##8d();break; case 0x8e:prefix##_##8e();break; case 0x8f:prefix##_##8f();break; \
	case 0x90:prefix##_##90();break; case 0x91:prefix##_##91();break; case 0x92:prefix##_##92();break; case 0x93:prefix##_##93();break; \
	case 0x94:prefix##_##94();break; case 0x95:prefix##_##95();break; case 0x96:prefix##_##96();break; case 0x97:prefix##_##97();break; \
	case 0x98:prefix##_##98();break; case 0x99:prefix##_##99();break; case 0x9a:prefix##_##9a();break; case 0x9b:prefix##_##9b();break; \
	case 0x9c:prefix##_##9c();break; case 0x9d:prefix##_##9d();break; case 0x9e:prefix##_##9e();break; case 0x9f:prefix##_##9f();break; \
	case 0xa0:prefix##_##a0();break; case 0xa1:prefix##_##a1();break; case 0xa2:prefix##_##a2();break; case 0xa3:prefix##_##a3();break; \
	case 0xa4:prefix##_##a4();break; case 0xa5:prefix##_##a5();break; case 0xa6:prefix##_##a6();break; case 0xa7:prefix##_##a7();break; \
	case 0xa8:prefix##_##a8();break; case 0xa9:prefix##_##a9();break; case 0xaa:prefix##_##aa();break; case 0xab:prefix##_##ab();break; \
	case 0xac:prefix##_##ac();break; case 0xad:prefix##_##ad();break; case 0xae:prefix##_##ae();break; case 0xaf:prefix##_##af();break; \
	case 0xb0:prefix##_##b0();break; case 0xb1:prefix##_##b1();break; case 0xb2:prefix##_##b2();break; case 0xb3:prefix##_##b3();break; \
	case 0xb4:prefix##_##b4();break; case 0xb5:prefix##_##b5();break; case 0xb6:prefix##_##b6();break; case 0xb7:prefix##_##b7();break; \
	case 0xb8:prefix##_##b8();break; case 0xb9:prefix##_##b9();break; case 0xba:prefix##_##ba();break; case 0xbb:prefix##_##bb();break; \
	case 0xbc:prefix##_##bc();break; case 0xbd:prefix##_##bd();break; case 0xbe:prefix##_##be();break; case 0xbf:prefix##_##bf();break; \
	case 0xc0:prefix##_##c0();break; case 0xc1:prefix##_##c1();break; case 0xc2:prefix##_##c2();break; case 0xc3:prefix##_##c3();break; \
	case 0xc4:prefix##_##c4();break; case 0xc5:prefix##_##c5();break; case 0xc6:prefix##_##c6();break; case 0xc7:prefix##_##c7();break; \
	case 0xc8:prefix##_##c8();break; case 0xc9:prefix##_##c9();break; case 0xca:prefix##_##ca();break; case 0xcb:prefix##_##cb();break; \
	case 0xcc:prefix##_##cc();break; case 0xcd:prefix##_##cd();break; case 0xce:prefix##_##ce();break; case 0xcf:prefix##_##cf();break; \
	case 0xd0:prefix##_##d0();break; case 0xd1:prefix##_##d1();break; case 0xd2:prefix##_##d2();break; case 0xd3:prefix##_##d3();break; \
	case 0xd4:prefix##_##d4();break; case 0xd5:prefix##_##d5();break; case 0xd6:prefix##_##d6();break; case 0xd7:prefix##_##d7();break; \
	case 0xd8:prefix##_##d8();break; case 0xd9:prefix##_##d9();break; case 0xda:prefix##_##da();break; case 0xdb:prefix##_##db();break; \
	case 0xdc:prefix##_##dc();break; case 0xdd:prefix##_##dd();break; case 0xde:prefix##_##de();break; case 0xdf:prefix##_##df();break; \
	case 0xe0:prefix##_##e0();break; case 0xe1:prefix##_##e1();break; case 0xe2:prefix##_##e2();break; case 0xe3:prefix##_##e3();break; \
	case 0xe4:prefix##_##e4();break; case 0xe5:prefix##_##e5();break; case 0xe6:prefix##_##e6();break; case 0xe7:prefix##_##e7();break; \
	case 0xe8:prefix##_##e8();break; case 0xe9:prefix##_##e9();break; case 0xea:prefix##_##ea();break; case 0xeb:prefix##_##eb();break; \
	case 0xec:prefix##_##ec();break; case 0xed:prefix##_##ed();break; case 0xee:prefix##_##ee();break; case 0xef:prefix##_##ef();break; \
	case 0xf0:prefix##_##f0();break; case 0xf1:prefix##_##f1();break; case 0xf2:prefix##_##f2();break; case 0xf3:prefix##_##f3();break; \
	case 0xf4:prefix##_##f4();break; case 0xf5:prefix##_##f5();break; case 0xf6:prefix##_##f6();break; case 0xf7:prefix##_##f7();break; \
	case 0xf8:prefix##_##f8();break; case 0xf9:prefix##_##f9();break; case 0xfa:prefix##_##fa();break; case 0xfb:prefix##_##fb();break; \
	case 0xfc:prefix##_##fc();break; case 0xfd:prefix##_##fd();break; case 0xfe:prefix##_##fe();break; case 0xff:prefix##_##ff();break; \
	}																																	\
}
#else
#define EXEC_INLINE EXEC
#endif


/***************************************************************
 * Enter HALT state; write 1 to fake port on first execution
 ***************************************************************/
#define ENTER_HALT {											\
    _PC--;                                                      \
    _HALT = 1;                                                  \
	if( !after_EI ) 											\
		z80_burn( z80_ICount ); 								\
}

/***************************************************************
 * Leave HALT state; write 0 to fake port
 ***************************************************************/
#define LEAVE_HALT {                                            \
	if( _HALT ) 												\
	{															\
		_HALT = 0;												\
		_PC++;													\
	}															\
}

/***************************************************************
 * Input a byte from given I/O port
 ***************************************************************/
#define IN(port)   ((UINT8)cpu_readport(port))

/***************************************************************
 * Output a byte to given I/O port
 ***************************************************************/
#define OUT(port,value) cpu_writeport(port,value)

/***************************************************************
 * Read a byte from given memory location
 ***************************************************************/
#define RM(addr) (UINT8)cpu_readmem16(addr)

/***************************************************************
 * Read a word from given memory location
 ***************************************************************/
static __inline__ void RM16( UINT32 addr, PAIR *r )
{
	r->b.l = RM(addr);
	r->b.h = RM((addr+1)&0xffff);
}

/***************************************************************
 * Write a byte to given memory location
 ***************************************************************/
#define WM(addr,value) cpu_writemem16(addr,value)

/***************************************************************
 * Write a word to given memory location
 ***************************************************************/
static __inline__ void WM16( UINT32 addr, PAIR *r )
{
	WM(addr,r->b.l);
	WM((addr+1)&0xffff,r->b.h);
}

/***************************************************************
 * ROP() is identical to RM() except it is used for
 * reading opcodes. In case of system with memory mapped I/O,
 * this function can be used to greatly speed up emulation
 ***************************************************************/
static __inline__ UINT8 ROP(void)
{
	unsigned pc = _PCD;
	_PC++;
	return cpu_readop(pc);
}

/****************************************************************
 * ARG() is identical to ROP() except it is used
 * for reading opcode arguments. This difference can be used to
 * support systems that use different encoding mechanisms for
 * opcodes and opcode arguments
 ***************************************************************/
static __inline__ UINT8 ARG(void)
{
	unsigned pc = _PCD;
    _PC++;
	return cpu_readop_arg(pc);
}

static __inline__ UINT32 ARG16(void)
{
	unsigned pc = _PCD;
    _PC += 2;
	return cpu_readop_arg(pc) | (cpu_readop_arg((pc+1)&0xffff) << 8);
}

/***************************************************************
 * Calculate the effective address EA of an opcode using
 * IX+offset resp. IY+offset addressing.
 ***************************************************************/
#define EAX EA = (UINT32)(UINT16)(_IX+(INT8)ARG())
#define EAY EA = (UINT32)(UINT16)(_IY+(INT8)ARG())

/***************************************************************
 * POP
 ***************************************************************/
#define POP(DR) { RM16( _SPD, &Z80.DR ); _SP += 2; }

/***************************************************************
 * PUSH
 ***************************************************************/
#define PUSH(SR) { _SP -= 2; WM16( _SPD, &Z80.SR ); }

/***************************************************************
 * JP
 ***************************************************************/
#if BUSY_LOOP_HACKS
#define JP {													\
	unsigned oldpc = _PCD-1;									\
	_PCD = ARG16(); 											\
    /* speed up busy loop */                                    \
	if( _PCD == oldpc ) 										\
	{															\
		if( !after_EI ) 										\
			BURNODD( z80_ICount, 1, 10 );						\
	}															\
	else														\
	{															\
		UINT8 op = cpu_readop(_PCD);							\
		if( _PCD == oldpc-1 )									\
		{														\
			/* NOP - JP $-1 or EI - JP $-1 */					\
			if ( op == 0x00 || op == 0xfb ) 					\
			{													\
				if( !after_EI ) 								\
					BURNODD( z80_ICount-4, 2, 4+10 );			\
			}													\
		}														\
		else													\
		/* LD SP,#xxxx - JP $-3 (Galaga) */ 					\
		if( _PCD == oldpc-3 && op == 0x31 ) 					\
		{														\
			if( !after_EI ) 									\
				BURNODD( z80_ICount-10, 2, 10+10 ); 			\
		}														\
	}															\
}
#else
#define JP {													\
	_PCD = ARG16(); 											\
}
#endif

/***************************************************************
 * JP_COND
 ***************************************************************/

#define JP_COND(cond)											\
	if( cond )													\
	{															\
		_PCD = ARG16(); 										\
	}															\
	else														\
	{															\
		_PC += 2;												\
    }

/***************************************************************
 * JR
 ***************************************************************/
#define JR()													\
{																\
	unsigned oldpc = _PCD-1;									\
	INT8 arg = (INT8)ARG(); /* ARG() also increments _PC */ 	\
	_PC += arg; 			/* so don't do _PC += ARG() */      \
    /* speed up busy loop */                                    \
	if( _PCD == oldpc ) 										\
	{															\
		if( !after_EI ) 										\
			BURNODD( z80_ICount, 1, 12 );						\
	}															\
	else														\
	{															\
		UINT8 op = cpu_readop(_PCD);							\
		if( _PCD == oldpc-1 )									\
		{														\
			/* NOP - JR $-1 or EI - JR $-1 */					\
			if ( op == 0x00 || op == 0xfb ) 					\
			{													\
				if( !after_EI ) 								\
					BURNODD( z80_ICount-4, 2, 4+12 );			\
			}													\
		}														\
		else													\
		/* LD SP,#xxxx - JR $-3 */								\
		if( _PCD == oldpc-3 && op == 0x31 ) 					\
		{														\
			if( !after_EI ) 									\
				BURNODD( z80_ICount-12, 2, 10+12 ); 			\
		}														\
    }                                                           \
}

/***************************************************************
 * JR_COND
 ***************************************************************/
#define JR_COND(cond)											\
	if( cond )													\
	{															\
		INT8 arg = (INT8)ARG(); /* ARG() also increments _PC */ \
		_PC += arg; 			/* so don't do _PC += ARG() */  \
        CY(5);                                                  \
	}															\
	else _PC++; 												\

/***************************************************************
 * CALL
 ***************************************************************/
#define CALL(cond)												\
	if( cond )													\
	{															\
		EA = ARG16();											\
		PUSH( PC ); 											\
		_PCD = EA;												\
        CY(7);                                                  \
	}															\
	else														\
	{															\
		_PC+=2; 												\
	}

/***************************************************************
 * RET
 ***************************************************************/
#define RET(cond)												\
	if( cond )													\
	{															\
		POP(PC);												\
		CY(6);													\
	}

/***************************************************************
 * RETN
 ***************************************************************/
#define RETN	{												\
    RET(1);                                                     \
	if( _IFF1 == 0 && _IFF2 == 1 )								\
	{															\
		_IFF1 = 1;												\
		if( Z80.irq_state != CLEAR_LINE ||						\
			Z80.request_irq >= 0 )								\
		{														\
			take_interrupt();									\
        }                                                       \
	}															\
	else _IFF1 = _IFF2; 										\
}

/***************************************************************
 * RETI
 ***************************************************************/
#define RETI	{												\
	int device = Z80.service_irq;								\
    RET(1);                                                     \
/* according to http://www.msxnet.org/tech/Z80/z80undoc.txt */	\
/*	_IFF1 = _IFF2;	*/											\
	if( device >= 0 )											\
	{															\
		Z80.irq[device].interrupt_reti(Z80.irq[device].irq_param); \
	}															\
}

/***************************************************************
 * LD	R,A
 ***************************************************************/
#define LD_R_A {												\
	_R = _A;													\
	_R2 = _A & 0x80;				/* keep bit 7 of R */		\
}

/***************************************************************
 * LD	A,R
 ***************************************************************/
#define LD_A_R {												\
	_A = (_R & 0x7f) | _R2; 									\
	_F = (_F & CF) | SZ[_A] | ( _IFF2 << 2 );					\
}

/***************************************************************
 * LD	I,A
 ***************************************************************/
#define LD_I_A {												\
	_I = _A;													\
}

/***************************************************************
 * LD	A,I
 ***************************************************************/
#define LD_A_I {												\
	_A = _I;													\
	_F = (_F & CF) | SZ[_A] | ( _IFF2 << 2 );					\
}

/***************************************************************
 * RST
 ***************************************************************/
#define RST(addr)												\
	PUSH( PC ); 												\
    _PCD = addr;                                                

/***************************************************************
 * INC	r8
 ***************************************************************/
static __inline__ UINT8 INC(UINT8 value)
{
	UINT8 res = value + 1;
	_F = (_F & CF) | SZHV_inc[res];
	return (UINT8)res;
}

/***************************************************************
 * DEC	r8
 ***************************************************************/
static __inline__ UINT8 DEC(UINT8 value)
{
	UINT8 res = value - 1;
	_F = (_F & CF) | SZHV_dec[res];
    return res;
}

/***************************************************************
 * RLCA
 ***************************************************************/
#if Z80_EXACT
#define RLCA													\
	_A = (_A << 1) | (_A >> 7); 								\
	_F = (_F & (SF | ZF | PF)) | (_A & (YF | XF | CF))
#else
#define RLCA                                                    \
	_A = (_A << 1) | (_A >> 7); 								\
	_F = (_F & (SF | ZF | YF | XF | PF)) | (_A & CF)
#endif

/***************************************************************
 * RRCA
 ***************************************************************/
#if Z80_EXACT
#define RRCA                                                    \
	_F = (_F & (SF | ZF | PF)) | (_A & (YF | XF | CF)); 		\
    _A = (_A >> 1) | (_A << 7)
#else
#define RRCA                                                    \
	_F = (_F & (SF | ZF | YF | XF | PF)) | (_A & CF);			\
	_A = (_A >> 1) | (_A << 7)
#endif

/***************************************************************
 * RLA
 ***************************************************************/
#if Z80_EXACT
#define RLA {													\
	UINT8 res = (_A << 1) | (_F & CF);							\
	UINT8 c = (_A & 0x80) ? CF : 0; 							\
	_F = (_F & (SF | ZF | PF)) | c | (res & (YF | XF)); 		\
	_A = res;													\
}
#else
#define RLA {                                                   \
	UINT8 res = (_A << 1) | (_F & CF);							\
	UINT8 c = (_A & 0x80) ? CF : 0; 							\
	_F = (_F & (SF | ZF | YF | XF | PF)) | c;					\
	_A = res;													\
}
#endif

/***************************************************************
 * RRA
 ***************************************************************/
#if Z80_EXACT
#define RRA {                                                   \
	UINT8 res = (_A >> 1) | (_F << 7);							\
	UINT8 c = (_A & 0x01) ? CF : 0; 							\
	_F = (_F & (SF | ZF | PF)) | c | (res & (YF | XF)); 		\
	_A = res;													\
}
#else
#define RRA {                                                   \
	UINT8 res = (_A >> 1) | (_F << 7);							\
	UINT8 c = (_A & 0x01) ? CF : 0; 							\
    _F = (_F & (SF | ZF | YF | XF | PF)) | c;                   \
	_A = res;													\
}
#endif

/***************************************************************
 * RRD
 ***************************************************************/
#define RRD {													\
	UINT8 n = RM(_HL);											\
	WM( _HL, (n >> 4) | (_A << 4) );							\
	_A = (_A & 0xf0) | (n & 0x0f);								\
	_F = (_F & CF) | SZP[_A];									\
}

/***************************************************************
 * RLD
 ***************************************************************/
#define RLD {                                                   \
    UINT8 n = RM(_HL);                                          \
	WM( _HL, (n << 4) | (_A & 0x0f) );							\
    _A = (_A & 0xf0) | (n >> 4);                                \
	_F = (_F & CF) | SZP[_A];									\
}

/***************************************************************
 * ADD	A,n
 ***************************************************************/
#ifdef X86_ASM
#if Z80_EXACT
#define ADD(value)												\
 asm (															\
 " addb %2,%0           \n"                                     \
 " lahf                 \n"                                     \
 " setob %1             \n" /* al = 1 if overflow */            \
 " addb %1,%1           \n"                                     \
 " addb %1,%1           \n" /* shift to P/V bit position */     \
 " andb $0xd1,%%ah      \n" /* sign, zero, half carry, carry */ \
 " orb %%ah,%1          \n"                                     \
 " movb %0,%%ah         \n" /* get result */                    \
 " andb $0x28,%%ah      \n" /* maks flags 5+3 */                \
 " orb %%ah,%1          \n" /* put them into flags */           \
 :"=r" (_A), "=r" (_F)                                          \
 :"r" (value), "1" (_F), "0" (_A)                               \
 )
#else
#define ADD(value)                                              \
 asm (															\
 " addb %2,%0           \n"                                     \
 " lahf                 \n"                                     \
 " setob %1             \n" /* al = 1 if overflow */            \
 " addb %1,%1           \n"                                     \
 " addb %1,%1           \n" /* shift to P/V bit position */     \
 " andb $0xd1,%%ah      \n" /* sign, zero, half carry, carry */ \
 " orb %%ah,%1          \n"                                     \
 :"=r" (_A), "=r" (_F)                                          \
 :"r" (value), "1" (_F), "0" (_A)                               \
 )
#endif
#else
#if BIG_FLAGS_ARRAY
#define ADD(value)												\
{																\
	UINT32 ah = _AFD & 0xff00;									\
	UINT32 res = (UINT8)((ah >> 8) + value);					\
	_F = SZHVC_add[ah | res];									\
    _A = res;                                                   \
}
#else
#define ADD(value)												\
{																\
	unsigned val = value;										\
    unsigned res = _A + val;                                    \
    _F = SZ[(UINT8)res] | ((res >> 8) & CF) |                   \
        ((_A ^ res ^ val) & HF) |                               \
        (((val ^ _A ^ 0x80) & (val ^ res) & 0x80) >> 5);        \
    _A = (UINT8)res;                                            \
}
#endif
#endif

/***************************************************************
 * ADC	A,n
 ***************************************************************/
#ifdef X86_ASM
#if Z80_EXACT
#define ADC(value)												\
 asm (															\
 " shrb $1,%1           \n"                                     \
 " adcb %2,%0           \n"                                     \
 " lahf                 \n"                                     \
 " setob %1             \n" /* al = 1 if overflow */            \
 " addb %1,%1           \n" /* shift to P/V bit position */     \
 " addb %1,%1           \n"                                     \
 " andb $0xd1,%%ah      \n" /* sign, zero, half carry, carry */ \
 " orb %%ah,%1          \n" /* combine with P/V */              \
 " movb %0,%%ah         \n" /* get result */                    \
 " andb $0x28,%%ah      \n" /* maks flags 5+3 */                \
 " orb %%ah,%1          \n" /* put them into flags */           \
 :"=r" (_A), "=r" (_F)                                          \
 :"r" (value), "1" (_F), "0" (_A)                               \
 )
#else
#define ADC(value)                                              \
 asm (															\
 " shrb $1,%1           \n"                                     \
 " adcb %2,%0           \n"                                     \
 " lahf                 \n"                                     \
 " setob %1             \n" /* al = 1 if overflow */            \
 " addb %1,%1           \n" /* shift to P/V bit position */     \
 " addb %1,%1           \n"                                     \
 " andb $0xd1,%%ah      \n" /* sign, zero, half carry, carry */ \
 " orb %%ah,%1          \n" /* combine with P/V */              \
 :"=r" (_A), "=r" (_F)                                          \
 :"r" (value), "1" (_F), "0" (_A)                               \
 )
#endif
#else
#if BIG_FLAGS_ARRAY
#define ADC(value)												\
{																\
	UINT32 ah = _AFD & 0xff00, c = _AFD & 1;					\
	UINT32 res = (UINT8)((ah >> 8) + value + c);				\
	_F = SZHVC_add[(c << 16) | ah | res];						\
    _A = res;                                                   \
}
#else
#define ADC(value)												\
{																\
	unsigned val = value;										\
	unsigned res = _A + val + (_F & CF);						\
	_F = SZ[res & 0xff] | ((res >> 8) & CF) |					\
		((_A ^ res ^ val) & HF) |								\
		(((val ^ _A ^ 0x80) & (val ^ res) & 0x80) >> 5);		\
	_A = res;													\
}
#endif
#endif

/***************************************************************
 * SUB	n
 ***************************************************************/
#ifdef X86_ASM
#if Z80_EXACT
#define SUB(value)												\
 asm (															\
 " subb %2,%0           \n"                                     \
 " lahf                 \n"                                     \
 " setob %1             \n" /* al = 1 if overflow */            \
 " stc                  \n" /* prepare to set N flag */         \
 " adcb %1,%1           \n" /* shift to P/V bit position */     \
 " addb %1,%1           \n"                                     \
 " andb $0xd1,%%ah      \n" /* sign, zero, half carry, carry */ \
 " orb %%ah,%1          \n" /* combine with P/V */              \
 " movb %0,%%ah         \n" /* get result */                    \
 " andb $0x28,%%ah      \n" /* maks flags 5+3 */                \
 " orb %%ah,%1          \n" /* put them into flags */           \
 :"=r" (_A), "=r" (_F)                                          \
 :"r" (value), "1" (_F), "0" (_A)                               \
 )
#else
#define SUB(value)                                              \
 asm (															\
 " subb %2,%0           \n"                                     \
 " lahf                 \n"                                     \
 " setob %1             \n" /* al = 1 if overflow */            \
 " stc                  \n" /* prepare to set N flag */         \
 " adcb %1,%1           \n" /* shift to P/V bit position */     \
 " addb %1,%1           \n"                                     \
 " andb $0xd1,%%ah      \n" /* sign, zero, half carry, carry */ \
 " orb %%ah,%1          \n" /* combine with P/V */              \
 :"=r" (_A), "=r" (_F)                                          \
 :"r" (value), "1" (_F), "0" (_A)                               \
 )
#endif
#else
#if BIG_FLAGS_ARRAY
#define SUB(value)												\
{																\
	UINT32 ah = _AFD & 0xff00;									\
	UINT32 res = (UINT8)((ah >> 8) - value);					\
	_F = SZHVC_sub[ah | res];									\
    _A = res;                                                   \
}
#else
#define SUB(value)												\
{																\
	unsigned val = value;										\
	unsigned res = _A - val;									\
	_F = SZ[res & 0xff] | ((res >> 8) & CF) | NF |				\
		((_A ^ res ^ val) & HF) |								\
		(((val ^ _A) & (_A ^ res) & 0x80) >> 5);				\
	_A = res;													\
}
#endif
#endif

/***************************************************************
 * SBC	A,n
 ***************************************************************/
#ifdef X86_ASM
#if Z80_EXACT
#define SBC(value)												\
 asm (															\
 " shrb $1,%1           \n"                                     \
 " sbbb %2,%0           \n"                                     \
 " lahf                 \n"                                     \
 " setob %1             \n" /* al = 1 if overflow */            \
 " stc                  \n" /* prepare to set N flag */         \
 " adcb %1,%1           \n" /* shift to P/V bit position */     \
 " addb %1,%1           \n"                                     \
 " andb $0xd1,%%ah      \n" /* sign, zero, half carry, carry */ \
 " orb %%ah,%1          \n" /* combine with P/V */              \
 " movb %0,%%ah         \n" /* get result */                    \
 " andb $0x28,%%ah      \n" /* maks flags 5+3 */                \
 " orb %%ah,%1          \n" /* put them into flags */           \
 :"=r" (_A), "=r" (_F)                                          \
 :"r" (value), "1" (_F), "0" (_A)                               \
 )
#else
#define SBC(value)                                              \
 asm (															\
 " shrb $1,%1           \n"                                     \
 " sbbb %2,%0           \n"                                     \
 " lahf                 \n"                                     \
 " setob %1             \n" /* al = 1 if overflow */            \
 " stc                  \n" /* prepare to set N flag */         \
 " adcb %1,%1           \n" /* shift to P/V bit position */     \
 " addb %1,%1           \n"                                     \
 " andb $0xd1,%%ah      \n" /* sign, zero, half carry, carry */ \
 " orb %%ah,%1          \n" /* combine with P/V */              \
 :"=r" (_A), "=r" (_F)                                          \
 :"r" (value), "1" (_F), "0" (_A)                               \
 )
#endif
#else
#if BIG_FLAGS_ARRAY
#define SBC(value)												\
{																\
	UINT32 ah = _AFD & 0xff00, c = _AFD & 1;					\
	UINT32 res = (UINT8)((ah >> 8) - value - c);				\
	_F = SZHVC_sub[(c<<16) | ah | res]; 						\
    _A = res;                                                   \
}
#else
#define SBC(value)												\
{																\
	unsigned val = value;										\
	unsigned res = _A - val - (_F & CF);						\
	_F = SZ[res & 0xff] | ((res >> 8) & CF) | NF |				\
		((_A ^ res ^ val) & HF) |								\
		(((val ^ _A) & (_A ^ res) & 0x80) >> 5);				\
	_A = res;													\
}
#endif
#endif

/***************************************************************
 * NEG
 ***************************************************************/
#define NEG {                                                   \
	UINT8 value = _A;											\
	_A = 0; 													\
	SUB(value); 												\
}

/***************************************************************
 * DAA
 ***************************************************************/
#define DAA {													\
	int idx = _A;												\
	if( _F & CF ) idx |= 0x100; 								\
	if( _F & HF ) idx |= 0x200; 								\
	if( _F & NF ) idx |= 0x400; 								\
	_AF = DAATable[idx];										\
}

/***************************************************************
 * AND	n
 ***************************************************************/
#define AND(value)												\
	_A &= value;												\
	_F = SZP[_A] | HF

/***************************************************************
 * OR	n
 ***************************************************************/
#define OR(value)												\
	_A |= value;												\
	_F = SZP[_A]

/***************************************************************
 * XOR	n
 ***************************************************************/
#define XOR(value)												\
	_A ^= value;												\
	_F = SZP[_A]

/***************************************************************
 * CP	n
 ***************************************************************/
#ifdef X86_ASM
#if Z80_EXACT
#define CP(value)												\
 asm (															\
 " cmpb %2,%0           \n"                                     \
 " lahf                 \n"                                     \
 " setob %1             \n" /* al = 1 if overflow */            \
 " stc                  \n" /* prepare to set N flag */         \
 " adcb %1,%1           \n" /* shift to P/V bit position */     \
 " addb %1,%1           \n"                                     \
 " andb $0xd1,%%ah      \n" /* sign, zero, half carry, carry */ \
 " orb %%ah,%1          \n" /* combine with P/V */              \
 " movb %2,%%ah         \n" /* get result */                    \
 " andb $0x28,%%ah      \n" /* maks flags 5+3 */                \
 " orb %%ah,%1          \n" /* put them into flags */           \
 :"=r" (_A), "=r" (_F)                                          \
 :"r" (value), "1" (_F), "0" (_A)                               \
 )
#else
#define CP(value)                                               \
 asm (															\
 " cmpb %2,%0           \n"                                     \
 " lahf                 \n"                                     \
 " setob %1             \n" /* al = 1 if overflow */            \
 " stc                  \n" /* prepare to set N flag */         \
 " adcb %1,%1           \n" /* shift to P/V bit position */     \
 " addb %1,%1           \n"                                     \
 " andb $0xd1,%%ah      \n" /* sign, zero, half carry, carry */ \
 " orb %%ah,%1          \n" /* combine with P/V */              \
 :"=r" (_A), "=r" (_F)                                          \
 :"r" (value), "1" (_F), "0" (_A)                               \
 )
#endif
#else
#if BIG_FLAGS_ARRAY
#define CP(value)												\
{																\
	UINT32 ah = _AFD & 0xff00;									\
	UINT32 res = (UINT8)((ah >> 8) - value);					\
	_F = SZHVC_sub[ah | res];									\
}
#else
#define CP(value)												\
{																\
	unsigned val = value;										\
	unsigned res = _A - val;									\
	_F = SZ[res & 0xff] | ((res >> 8) & CF) | NF |				\
		((_A ^ res ^ val) & HF) |								\
		((((val ^ _A) & (_A ^ res)) >> 5) & VF);				\
}
#endif
#endif

/***************************************************************
 * EX   AF,AF'
 ***************************************************************/
#define EX_AF {                                                 \
	PAIR tmp;													\
    tmp = Z80.AF; Z80.AF = Z80.AF2; Z80.AF2 = tmp;              \
}

/***************************************************************
 * EX   DE,HL
 ***************************************************************/
#define EX_DE_HL {                                              \
	PAIR tmp;													\
    tmp = Z80.DE; Z80.DE = Z80.HL; Z80.HL = tmp;                \
}

/***************************************************************
 * EXX
 ***************************************************************/
#define EXX {                                                   \
	PAIR tmp;													\
    tmp = Z80.BC; Z80.BC = Z80.BC2; Z80.BC2 = tmp;              \
    tmp = Z80.DE; Z80.DE = Z80.DE2; Z80.DE2 = tmp;              \
    tmp = Z80.HL; Z80.HL = Z80.HL2; Z80.HL2 = tmp;              \
}

/***************************************************************
 * EX   (SP),r16
 ***************************************************************/
#define EXSP(DR)												\
{																\
	PAIR tmp = { { 0, 0, 0, 0 } };								\
	RM16( _SPD, &tmp ); 										\
	WM16( _SPD, &Z80.DR );										\
	Z80.DR = tmp;												\
}


/***************************************************************
 * ADD16
 ***************************************************************/
#ifdef	X86_ASM
#if Z80_EXACT
#define ADD16(DR,SR)											\
 asm (															\
 " andb $0xc4,%1        \n"                                     \
 " addb %%dl,%%cl       \n"                                     \
 " adcb %%dh,%%ch       \n"                                     \
 " lahf                 \n"                                     \
 " andb $0x11,%%ah      \n"                                     \
 " orb %%ah,%1          \n"                                     \
 " movb %%ch,%%ah       \n" /* get result MSB */                \
 " andb $0x28,%%ah      \n" /* maks flags 5+3 */                \
 " orb %%ah,%1          \n" /* put them into flags */           \
 :"=c" (Z80.DR.d), "=r" (_F)                                    \
 :"0" (Z80.DR.d), "1" (_F), "d" (Z80.SR.d)                      \
 )
#else
#define ADD16(DR,SR)                                            \
 asm (															\
 " andb $0xc4,%1        \n"                                     \
 " addb %%dl,%%cl       \n"                                     \
 " adcb %%dh,%%ch       \n"                                     \
 " lahf                 \n"                                     \
 " andb $0x11,%%ah      \n"                                     \
 " orb %%ah,%1          \n"                                     \
 :"=c" (Z80.DR.d), "=r" (_F)                                    \
 :"0" (Z80.DR.d), "1" (_F), "d" (Z80.SR.d)                      \
 )
#endif
#else
#define ADD16(DR,SR)											\
{																\
	UINT32 res = Z80.DR.d + Z80.SR.d;							\
	_F = (_F & (SF | ZF | VF)) |								\
		(((Z80.DR.d ^ res ^ Z80.SR.d) >> 8) & HF) | 			\
		((res >> 16) & CF); 									\
	Z80.DR.w.l = (UINT16)res;									\
}
#endif

/***************************************************************
 * ADC	r16,r16
 ***************************************************************/
#ifdef	X86_ASM
#if Z80_EXACT
#define ADC16(Reg)												\
 asm (                                                          \
 " shrb $1,%1           \n"                                     \
 " adcb %%dl,%%cl       \n"                                     \
 " lahf                 \n"                                     \
 " movb %%ah,%%dl       \n"                                     \
 " adcb %%dh,%%ch       \n"                                     \
 " lahf                 \n"                                     \
 " setob %1             \n"                                     \
 " orb $0xbf,%%dl       \n" /* set all but zero */              \
 " addb %1,%1           \n"                                     \
 " andb $0xd1,%%ah      \n" /* sign,zero,half carry and carry */\
 " addb %1,%1           \n"                                     \
 " orb %%ah,%1          \n" /* overflow into P/V */             \
 " andb %%dl,%1         \n" /* mask zero */                     \
 " movb %%ch,%%ah       \n" /* get result MSB */                \
 " andb $0x28,%%ah      \n" /* maks flags 5+3 */                \
 " orb %%ah,%1          \n" /* put them into flags */           \
 :"=c" (_HLD), "=r" (_F)                                        \
 :"0" (_HLD), "1" (_F), "d" (Z80.Reg.d)                         \
 )
#else
#define ADC16(Reg)                                              \
 asm (                                                          \
 " shrb $1,%1           \n"                                     \
 " adcb %%dl,%%cl       \n"                                     \
 " lahf                 \n"                                     \
 " movb %%ah,%%dl       \n"                                     \
 " adcb %%dh,%%ch       \n"                                     \
 " lahf                 \n"                                     \
 " setob %1             \n"                                     \
 " orb $0xbf,%%dl       \n" /* set all but zero */              \
 " addb %1,%1           \n"                                     \
 " andb $0xd1,%%ah      \n" /* sign,zero,half carry and carry */\
 " addb %1,%1           \n"                                     \
 " orb %%ah,%1          \n" /* overflow into P/V */             \
 " andb %%dl,%1         \n" /* mask zero */                     \
 :"=c" (_HLD), "=r" (_F)                                        \
 :"0" (_HLD), "1" (_F), "d" (Z80.Reg.d)                         \
 )
#endif
#else
#define ADC16(Reg)												\
{																\
	UINT32 res = _HLD + Z80.Reg.d + (_F & CF);					\
	_F = (((_HLD ^ res ^ Z80.Reg.d) >> 8) & HF) |				\
		((res >> 16) & CF) |									\
		((res >> 8) & SF) | 									\
		((res & 0xffff) ? 0 : ZF) | 							\
		(((Z80.Reg.d ^ _HLD ^ 0x8000) & (Z80.Reg.d ^ res) & 0x8000) >> 13); \
	_HL = (UINT16)res;											\
}
#endif

/***************************************************************
 * SBC	r16,r16
 ***************************************************************/
#ifdef	X86_ASM
#if Z80_EXACT
#define SBC16(Reg)												\
asm (															\
 " shrb $1,%1           \n"                                     \
 " sbbb %%dl,%%cl       \n"                                     \
 " lahf                 \n"                                     \
 " movb %%ah,%%dl       \n"                                     \
 " sbbb %%dh,%%ch       \n"                                     \
 " lahf                 \n"                                     \
 " setob %1             \n"                                     \
 " orb $0xbf,%%dl       \n" /* set all but zero */              \
 " stc                  \n"                                     \
 " adcb %1,%1           \n"                                     \
 " andb $0xd1,%%ah      \n" /* sign,zero,half carry and carry */\
 " addb %1,%1           \n"                                     \
 " orb %%ah,%1          \n" /* overflow into P/V */             \
 " andb %%dl,%1         \n" /* mask zero */                     \
 " movb %%ch,%%ah       \n" /* get result MSB */                \
 " andb $0x28,%%ah      \n" /* maks flags 5+3 */                \
 " orb %%ah,%1          \n" /* put them into flags */           \
 :"=c" (_HLD), "=r" (_F)                                        \
 :"0" (_HLD), "1" (_F), "d" (Z80.Reg.d)                         \
 )
#else
#define SBC16(Reg)                                              \
asm (															\
 " shrb $1,%1           \n"                                     \
 " sbbb %%dl,%%cl       \n"                                     \
 " lahf                 \n"                                     \
 " movb %%ah,%%dl       \n"                                     \
 " sbbb %%dh,%%ch       \n"                                     \
 " lahf                 \n"                                     \
 " setob %1             \n"                                     \
 " orb $0xbf,%%dl       \n" /* set all but zero */              \
 " stc                  \n"                                     \
 " adcb %1,%1           \n"                                     \
 " andb $0xd1,%%ah      \n" /* sign,zero,half carry and carry */\
 " addb %1,%1           \n"                                     \
 " orb %%ah,%1          \n" /* overflow into P/V */             \
 " andb %%dl,%1         \n" /* mask zero */                     \
 :"=c" (_HLD), "=r" (_F)                                        \
 :"0" (_HLD), "1" (_F), "d" (Z80.Reg.d)                         \
 )
#endif
#else
#define SBC16(Reg)												\
{																\
	UINT32 res = _HLD - Z80.Reg.d - (_F & CF);					\
	_F = (((_HLD ^ res ^ Z80.Reg.d) >> 8) & HF) | NF |			\
		((res >> 16) & CF) |									\
		((res >> 8) & SF) | 									\
		((res & 0xffff) ? 0 : ZF) | 							\
		(((Z80.Reg.d ^ _HLD) & (_HLD ^ res) &0x8000) >> 13);	\
	_HL = (UINT16)res;											\
}
#endif

/***************************************************************
 * RLC	r8
 ***************************************************************/
static __inline__ UINT8 RLC(UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x80) ? CF : 0;
	res = ((res << 1) | (res >> 7)) & 0xff;
	_F = SZP[res] | c;
	return res;
}

/***************************************************************
 * RRC	r8
 ***************************************************************/
static __inline__ UINT8 RRC(UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x01) ? CF : 0;
	res = ((res >> 1) | (res << 7)) & 0xff;
	_F = SZP[res] | c;
	return res;
}

/***************************************************************
 * RL	r8
 ***************************************************************/
static __inline__ UINT8 RL(UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x80) ? CF : 0;
	res = ((res << 1) | (_F & CF)) & 0xff;
	_F = SZP[res] | c;
	return res;
}

/***************************************************************
 * RR	r8
 ***************************************************************/
static __inline__ UINT8 RR(UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x01) ? CF : 0;
	res = ((res >> 1) | (_F << 7)) & 0xff;
	_F = SZP[res] | c;
	return res;
}

/***************************************************************
 * SLA	r8
 ***************************************************************/
static __inline__ UINT8 SLA(UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x80) ? CF : 0;
	res = (res << 1) & 0xff;
	_F = SZP[res] | c;
	return res;
}

/***************************************************************
 * SRA	r8
 ***************************************************************/
static __inline__ UINT8 SRA(UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x01) ? CF : 0;
	res = ((res >> 1) | (res & 0x80)) & 0xff;
	_F = SZP[res] | c;
	return res;
}

/***************************************************************
 * SLL	r8
 ***************************************************************/
static __inline__ UINT8 SLL(UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x80) ? CF : 0;
	res = ((res << 1) | 0x01) & 0xff;
	_F = SZP[res] | c;
	return res;
}

/***************************************************************
 * SRL	r8
 ***************************************************************/
static __inline__ UINT8 SRL(UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x01) ? CF : 0;
	res = (res >> 1) & 0xff;
	_F = SZP[res] | c;
	return res;
}

/***************************************************************
 * BIT  bit,r8
 ***************************************************************/
#define BIT(bit,reg)                                            \
	_F = (_F & CF) | HF | SZ_BIT[reg & (1<<bit)]

/***************************************************************
 * BIT	bit,(IX/Y+o)
 ***************************************************************/
#if Z80_EXACT
#define BIT_XY(bit,reg)                                         \
    _F = (_F & CF) | HF | (SZ_BIT[reg & (1<<bit)] & ~(YF|XF)) | ((EA>>8) & (YF|XF))
#else
#define BIT_XY	BIT
#endif

/***************************************************************
 * RES	bit,r8
 ***************************************************************/
static __inline__ UINT8 RES(UINT8 bit, UINT8 value)
{
	return value & ~(1<<bit);
}

/***************************************************************
 * SET  bit,r8
 ***************************************************************/
static __inline__ UINT8 SET(UINT8 bit, UINT8 value)
{
	return value | (1<<bit);
}

/***************************************************************
 * LDI
 ***************************************************************/
#if Z80_EXACT
#define LDI {													\
	UINT8 io = RM(_HL); 										\
	WM( _DE, io );												\
	_F &= SF | ZF | CF; 										\
	if( (_A + io) & 0x02 ) _F |= YF; /* bit 1 -> flag 5 */		\
    if( (_A + io) & 0x08 ) _F |= XF; /* bit 3 -> flag 3 */      \
    _HL++; _DE++; _BC--;                                        \
	if( _BC ) _F |= VF; 										\
}
#else
#define LDI {                                                   \
	WM( _DE, RM(_HL) ); 										\
    _F &= SF | ZF | YF | XF | CF;                               \
	_HL++; _DE++; _BC--;										\
	if( _BC ) _F |= VF; 										\
}
#endif

/***************************************************************
 * CPI
 ***************************************************************/
#if Z80_EXACT
#define CPI {													\
	UINT8 val = RM(_HL);										\
	UINT8 res = _A - val;										\
	_HL++; _BC--;												\
	_F = (_F & CF) | (SZ[res] & ~(YF|XF)) | ((_A ^ val ^ res) & HF) | NF;  \
	if( _F & HF ) res -= 1; 									\
	if( res & 0x02 ) _F |= YF; /* bit 1 -> flag 5 */			\
	if( res & 0x08 ) _F |= XF; /* bit 3 -> flag 3 */			\
    if( _BC ) _F |= VF;                                         \
}
#else
#define CPI {                                                   \
	UINT8 val = RM(_HL);										\
	UINT8 res = _A - val;										\
	_HL++; _BC--;												\
	_F = (_F & CF) | SZ[res] | ((_A ^ val ^ res) & HF) | NF;	\
	if( _BC ) _F |= VF; 										\
}
#endif

/***************************************************************
 * INI
 ***************************************************************/
#if Z80_EXACT
#define INI {													\
	UINT8 io = IN(_BC); 										\
	_B--;														\
	WM( _HL, io );												\
	_HL++;														\
	_F = SZ[_B];												\
	if( io & SF ) _F |= NF; 									\
	if( (_C + io + 1) & 0x100 ) _F |= HF | CF;					\
    if( (irep_tmp1[_C & 3][io & 3] ^                            \
		 breg_tmp2[_B] ^										\
		 (_C >> 2) ^											\
		 (io >> 2)) & 1 )										\
		_F |= PF;												\
}
#else
#define INI {													\
	_B--;														\
	WM( _HL, IN(_BC) ); 										\
	_HL++;														\
	_F = (_B) ? NF : NF | ZF;									\
}
#endif

/***************************************************************
 * OUTI
 ***************************************************************/
#if Z80_EXACT
#define OUTI {													\
	UINT8 io = RM(_HL); 										\
	OUT( _BC, io ); 											\
    _B--;                                                       \
	_HL++;														\
	_F = SZ[_B];												\
	if( io & SF ) _F |= NF; 									\
	if( (_C + io + 1) & 0x100 ) _F |= HF | CF;					\
    if( (irep_tmp1[_C & 3][io & 3] ^                            \
		 breg_tmp2[_B] ^										\
		 (_C >> 2) ^											\
		 (io >> 2)) & 1 )										\
        _F |= PF;                                               \
}
#else
#define OUTI {													\
    OUT( _BC, RM(_HL) );                                        \
	_B--;														\
    _HL++;                                                      \
    _F = (_B) ? NF : NF | ZF;                                   \
}
#endif

/***************************************************************
 * LDD
 ***************************************************************/
#if Z80_EXACT
#define LDD {													\
	UINT8 io = RM(_HL); 										\
	WM( _DE, io );												\
	_F &= SF | ZF | CF; 										\
	if( (_A + io) & 0x02 ) _F |= YF; /* bit 1 -> flag 5 */		\
	if( (_A + io) & 0x08 ) _F |= XF; /* bit 3 -> flag 3 */		\
	_HL--; _DE--; _BC--;										\
	if( _BC ) _F |= VF; 										\
}
#else
#define LDD {                                                   \
	WM( _DE, RM(_HL) ); 										\
    _F &= SF | ZF | YF | XF | CF;                               \
	_HL--; _DE--; _BC--;										\
	if( _BC ) _F |= VF; 										\
}
#endif

/***************************************************************
 * CPD
 ***************************************************************/
#if Z80_EXACT
#define CPD {													\
	UINT8 val = RM(_HL);										\
	UINT8 res = _A - val;										\
	_HL--; _BC--;												\
	_F = (_F & CF) | (SZ[res] & ~(YF|XF)) | ((_A ^ val ^ res) & HF) | NF;  \
	if( _F & HF ) res -= 1; 									\
	if( res & 0x02 ) _F |= YF; /* bit 1 -> flag 5 */			\
	if( res & 0x08 ) _F |= XF; /* bit 3 -> flag 3 */			\
    if( _BC ) _F |= VF;                                         \
}
#else
#define CPD {                                                   \
	UINT8 val = RM(_HL);										\
	UINT8 res = _A - val;										\
	_HL--; _BC--;												\
	_F = (_F & CF) | SZ[res] | ((_A ^ val ^ res) & HF) | NF;	\
	if( _BC ) _F |= VF; 										\
}
#endif

/***************************************************************
 * IND
 ***************************************************************/
#if Z80_EXACT
#define IND {													\
    UINT8 io = IN(_BC);                                         \
	_B--;														\
	WM( _HL, io );												\
	_HL--;														\
	_F = SZ[_B];												\
    if( io & SF ) _F |= NF;                                     \
	if( (_C + io - 1) & 0x100 ) _F |= HF | CF;					\
	if( (drep_tmp1[_C & 3][io & 3] ^							\
		 breg_tmp2[_B] ^										\
		 (_C >> 2) ^											\
		 (io >> 2)) & 1 )										\
        _F |= PF;                                               \
}
#else
#define IND {                                                   \
	_B--;														\
	WM( _HL, IN(_BC) ); 										\
	_HL--;														\
	_F = (_B) ? NF : NF | ZF;									\
}
#endif

/***************************************************************
 * OUTD
 ***************************************************************/
#if Z80_EXACT
#define OUTD {													\
	UINT8 io = RM(_HL); 										\
	OUT( _BC, io ); 											\
	_B--;														\
	_HL--;														\
	_F = SZ[_B];												\
    if( io & SF ) _F |= NF;                                     \
	if( (_C + io - 1) & 0x100 ) _F |= HF | CF;					\
	if( (drep_tmp1[_C & 3][io & 3] ^							\
		 breg_tmp2[_B] ^										\
		 (_C >> 2) ^											\
		 (io >> 2)) & 1 )										\
        _F |= PF;                                               \
}
#else
#define OUTD {                                                  \
    OUT( _BC, RM(_HL) );                                        \
	_B--;														\
	_HL--;														\
	_F = (_B) ? NF : NF | ZF;									\
}
#endif

/***************************************************************
 * LDIR
 ***************************************************************/
#if REPEAT_AT_ONCE
#define LDIR {                                                  \
	CY(5);														\
	_PC -= 2;													\
	do															\
	{															\
		LDI;													\
		if( _BC )												\
		{														\
			if( z80_ICount > 0 )								\
			{													\
				_R += 2;  /* increment R twice */				\
				CY(21); 										\
			}													\
			else break; 										\
		}														\
		else													\
		{														\
			_PC += 2;											\
            z80_ICount += 5;                                    \
            break;                                              \
		}														\
	} while( z80_ICount > 0 );									\
}
#else
#define LDIR LDI; if( _BC ) { _PC -= 2; CY(5); }
#endif

/***************************************************************
 * CPIR
 ***************************************************************/
#if REPEAT_AT_ONCE
#define CPIR {                                                  \
	CY(5);														\
	_PC -= 2;													\
    do                                                          \
	{															\
		CPI;													\
		if( _BC && !(_F & ZF) ) 								\
		{														\
			if( z80_ICount > 0 )								\
            {                                                   \
				_R += 2;  /* increment R twice */				\
				CY(21); 										\
			}													\
            else break;                                         \
        }                                                       \
		else													\
		{														\
			_PC += 2;											\
            z80_ICount += 5;                                    \
            break;                                              \
		}														\
	} while( z80_ICount > 0 );									\
}
#else
#define CPIR CPI; if( _BC && !(_F & ZF) ) { _PC -= 2; CY(5); }
#endif

/***************************************************************
 * INIR
 ***************************************************************/
#if REPEAT_AT_ONCE
#define INIR {                                                  \
	CY(5);														\
	_PC -= 2;													\
    do                                                          \
	{															\
		INI;													\
		if( _B )												\
		{														\
			if( z80_ICount > 0 )								\
            {                                                   \
				_R += 2;  /* increment R twice */				\
				CY(21); 										\
			}													\
            else break;                                         \
        }                                                       \
		else													\
		{														\
			_PC += 2;											\
            z80_ICount += 5;                                    \
            break;                                              \
		}														\
	} while( z80_ICount > 0 );									\
}
#else
#define INIR INI; if( _B ) { _PC -= 2; CY(5); }
#endif

/***************************************************************
 * OTIR
 ***************************************************************/
#if REPEAT_AT_ONCE
#define OTIR {                                                  \
	CY(5);														\
	_PC -= 2;													\
    do                                                          \
	{															\
		OUTI;													\
		if( _B	)												\
		{														\
			if( z80_ICount > 0 )								\
            {                                                   \
				_R += 2;  /* increment R twice */				\
				CY(21); 										\
			}													\
            else break;                                         \
        }                                                       \
		else													\
		{														\
			_PC += 2;											\
            z80_ICount += 5;                                    \
            break;                                              \
		}														\
	} while( z80_ICount > 0 );									\
}
#else
#define OTIR OUTI; if( _B ) { _PC -= 2; CY(5); }
#endif

/***************************************************************
 * LDDR
 ***************************************************************/
#if REPEAT_AT_ONCE
#define LDDR {                                                  \
	CY(5);														\
	_PC -= 2;													\
    do                                                          \
	{															\
		LDD;													\
		if( _BC )												\
		{														\
			if( z80_ICount > 0 )								\
            {                                                   \
				_R += 2;  /* increment R twice */				\
				CY(21); 										\
			}													\
            else break;                                         \
        }                                                       \
		else													\
		{														\
			_PC += 2;											\
            z80_ICount += 5;                                    \
            break;                                              \
		}														\
	} while( z80_ICount > 0 );									\
}
#else
#define LDDR LDD; if( _BC ) { _PC -= 2; CY(5); }
#endif

/***************************************************************
 * CPDR
 ***************************************************************/
#if REPEAT_AT_ONCE
#define CPDR {                                                  \
	CY(5);														\
	_PC -= 2;													\
    do                                                          \
	{															\
		CPD;													\
		if( _BC && !(_F & ZF) ) 								\
		{														\
			if( z80_ICount > 0 )								\
            {                                                   \
				_R += 2;  /* increment R twice */				\
				CY(21); 										\
			}													\
            else break;                                         \
        }                                                       \
		else													\
		{														\
			_PC += 2;											\
            z80_ICount += 5;                                    \
            break;                                              \
		}														\
	} while( z80_ICount > 0 );									\
}
#else
#define CPDR CPD; if( _BC && !(_F & ZF) ) { _PC -= 2; CY(5); }
#endif

/***************************************************************
 * INDR
 ***************************************************************/
#if REPEAT_AT_ONCE
#define INDR {                                                  \
	CY(5);														\
	_PC -= 2;													\
    do                                                          \
	{															\
		IND;													\
		if( _B )												\
		{														\
			if( z80_ICount > 0 )								\
            {                                                   \
				_R += 2;  /* increment R twice */				\
				CY(21); 										\
			}													\
            else break;                                         \
        }                                                       \
		else													\
		{														\
			_PC += 2;											\
            z80_ICount += 5;                                    \
            break;                                              \
		}														\
	} while( z80_ICount > 0 );									\
}
#else
#define INDR IND; if( _B ) { _PC -= 2; CY(5); }
#endif

/***************************************************************
 * OTDR
 ***************************************************************/
#if REPEAT_AT_ONCE
#define OTDR {                                                  \
	CY(5);														\
	_PC -= 2;													\
    do                                                          \
	{															\
		OUTD;													\
		if( _B )												\
		{														\
			if( z80_ICount > 0 )								\
            {                                                   \
				_R += 2;  /* increment R twice */				\
				CY(21); 										\
			}													\
            else break;                                         \
        }                                                       \
		else													\
		{														\
			_PC += 2;											\
            z80_ICount += 5;                                    \
            break;                                              \
		}														\
	} while( z80_ICount > 0 );									\
}
#else
#define OTDR OUTD; if( _B ) { _PC -= 2; CY(5); }
#endif

/***************************************************************
 * EI
 ***************************************************************/
#define EI {													\
	/* If interrupts were disabled, execute one more			\
     * instruction and check the IRQ line.                      \
     * If not, simply set interrupt flip-flop 2                 \
     */                                                         \
	if( _IFF1 == 0 )											\
	{															\
        _IFF1 = _IFF2 = 1;                                      \
        _PPC = _PCD;                                            \
		_R++;													\
		if( Z80.irq_state != CLEAR_LINE ||						\
			Z80.request_irq >= 0 )								\
		{														\
			after_EI = 1;	/* avoid cycle skip hacks */		\
			EXEC(op,ROP()); 									\
			after_EI = 0;										\
            take_interrupt();                                   \
        }                                                       \
		else EXEC(op,ROP()); 									\
    } else _IFF2 = 1;                                           \
}

/**********************************************************
 * opcodes with CB prefix
 * rotate, shift and bit operations
 **********************************************************/
OP(cb,00) { _B = RLC(_B);											} /* RLC  B 		  */
OP(cb,01) { _C = RLC(_C);											} /* RLC  C 		  */
OP(cb,02) { _D = RLC(_D);											} /* RLC  D 		  */
OP(cb,03) { _E = RLC(_E);											} /* RLC  E 		  */
OP(cb,04) { _H = RLC(_H);											} /* RLC  H 		  */
OP(cb,05) { _L = RLC(_L);											} /* RLC  L 		  */
OP(cb,06) { WM( _HL, RLC(RM(_HL)) );								} /* RLC  (HL)		  */
OP(cb,07) { _A = RLC(_A);											} /* RLC  A 		  */

OP(cb,08) { _B = RRC(_B);											} /* RRC  B 		  */
OP(cb,09) { _C = RRC(_C);											} /* RRC  C 		  */
OP(cb,0a) { _D = RRC(_D);											} /* RRC  D 		  */
OP(cb,0b) { _E = RRC(_E);											} /* RRC  E 		  */
OP(cb,0c) { _H = RRC(_H);											} /* RRC  H 		  */
OP(cb,0d) { _L = RRC(_L);											} /* RRC  L 		  */
OP(cb,0e) { WM( _HL, RRC(RM(_HL)) );								} /* RRC  (HL)		  */
OP(cb,0f) { _A = RRC(_A);											} /* RRC  A 		  */

OP(cb,10) { _B = RL(_B);											} /* RL   B 		  */
OP(cb,11) { _C = RL(_C);											} /* RL   C 		  */
OP(cb,12) { _D = RL(_D);											} /* RL   D 		  */
OP(cb,13) { _E = RL(_E);											} /* RL   E 		  */
OP(cb,14) { _H = RL(_H);											} /* RL   H 		  */
OP(cb,15) { _L = RL(_L);											} /* RL   L 		  */
OP(cb,16) { WM( _HL, RL(RM(_HL)) ); 								} /* RL   (HL)		  */
OP(cb,17) { _A = RL(_A);											} /* RL   A 		  */

OP(cb,18) { _B = RR(_B);											} /* RR   B 		  */
OP(cb,19) { _C = RR(_C);											} /* RR   C 		  */
OP(cb,1a) { _D = RR(_D);											} /* RR   D 		  */
OP(cb,1b) { _E = RR(_E);											} /* RR   E 		  */
OP(cb,1c) { _H = RR(_H);											} /* RR   H 		  */
OP(cb,1d) { _L = RR(_L);											} /* RR   L 		  */
OP(cb,1e) { WM( _HL, RR(RM(_HL)) ); 								} /* RR   (HL)		  */
OP(cb,1f) { _A = RR(_A);											} /* RR   A 		  */

OP(cb,20) { _B = SLA(_B);											} /* SLA  B 		  */
OP(cb,21) { _C = SLA(_C);											} /* SLA  C 		  */
OP(cb,22) { _D = SLA(_D);											} /* SLA  D 		  */
OP(cb,23) { _E = SLA(_E);											} /* SLA  E 		  */
OP(cb,24) { _H = SLA(_H);											} /* SLA  H 		  */
OP(cb,25) { _L = SLA(_L);											} /* SLA  L 		  */
OP(cb,26) { WM( _HL, SLA(RM(_HL)) );								} /* SLA  (HL)		  */
OP(cb,27) { _A = SLA(_A);											} /* SLA  A 		  */

OP(cb,28) { _B = SRA(_B);											} /* SRA  B 		  */
OP(cb,29) { _C = SRA(_C);											} /* SRA  C 		  */
OP(cb,2a) { _D = SRA(_D);											} /* SRA  D 		  */
OP(cb,2b) { _E = SRA(_E);											} /* SRA  E 		  */
OP(cb,2c) { _H = SRA(_H);											} /* SRA  H 		  */
OP(cb,2d) { _L = SRA(_L);											} /* SRA  L 		  */
OP(cb,2e) { WM( _HL, SRA(RM(_HL)) );								} /* SRA  (HL)		  */
OP(cb,2f) { _A = SRA(_A);											} /* SRA  A 		  */

OP(cb,30) { _B = SLL(_B);											} /* SLL  B 		  */
OP(cb,31) { _C = SLL(_C);											} /* SLL  C 		  */
OP(cb,32) { _D = SLL(_D);											} /* SLL  D 		  */
OP(cb,33) { _E = SLL(_E);											} /* SLL  E 		  */
OP(cb,34) { _H = SLL(_H);											} /* SLL  H 		  */
OP(cb,35) { _L = SLL(_L);											} /* SLL  L 		  */
OP(cb,36) { WM( _HL, SLL(RM(_HL)) );								} /* SLL  (HL)		  */
OP(cb,37) { _A = SLL(_A);											} /* SLL  A 		  */

OP(cb,38) { _B = SRL(_B);											} /* SRL  B 		  */
OP(cb,39) { _C = SRL(_C);											} /* SRL  C 		  */
OP(cb,3a) { _D = SRL(_D);											} /* SRL  D 		  */
OP(cb,3b) { _E = SRL(_E);											} /* SRL  E 		  */
OP(cb,3c) { _H = SRL(_H);											} /* SRL  H 		  */
OP(cb,3d) { _L = SRL(_L);											} /* SRL  L 		  */
OP(cb,3e) { WM( _HL, SRL(RM(_HL)) );								} /* SRL  (HL)		  */
OP(cb,3f) { _A = SRL(_A);											} /* SRL  A 		  */

OP(cb,40) { BIT(0,_B);												} /* BIT  0,B		  */
OP(cb,41) { BIT(0,_C);												} /* BIT  0,C		  */
OP(cb,42) { BIT(0,_D);												} /* BIT  0,D		  */
OP(cb,43) { BIT(0,_E);												} /* BIT  0,E		  */
OP(cb,44) { BIT(0,_H);												} /* BIT  0,H		  */
OP(cb,45) { BIT(0,_L);												} /* BIT  0,L		  */
OP(cb,46) { BIT(0,RM(_HL)); 										} /* BIT  0,(HL)	  */
OP(cb,47) { BIT(0,_A);												} /* BIT  0,A		  */

OP(cb,48) { BIT(1,_B);												} /* BIT  1,B		  */
OP(cb,49) { BIT(1,_C);												} /* BIT  1,C		  */
OP(cb,4a) { BIT(1,_D);												} /* BIT  1,D		  */
OP(cb,4b) { BIT(1,_E);												} /* BIT  1,E		  */
OP(cb,4c) { BIT(1,_H);												} /* BIT  1,H		  */
OP(cb,4d) { BIT(1,_L);												} /* BIT  1,L		  */
OP(cb,4e) { BIT(1,RM(_HL)); 										} /* BIT  1,(HL)	  */
OP(cb,4f) { BIT(1,_A);												} /* BIT  1,A		  */

OP(cb,50) { BIT(2,_B);												} /* BIT  2,B		  */
OP(cb,51) { BIT(2,_C);												} /* BIT  2,C		  */
OP(cb,52) { BIT(2,_D);												} /* BIT  2,D		  */
OP(cb,53) { BIT(2,_E);												} /* BIT  2,E		  */
OP(cb,54) { BIT(2,_H);												} /* BIT  2,H		  */
OP(cb,55) { BIT(2,_L);												} /* BIT  2,L		  */
OP(cb,56) { BIT(2,RM(_HL)); 										} /* BIT  2,(HL)	  */
OP(cb,57) { BIT(2,_A);												} /* BIT  2,A		  */

OP(cb,58) { BIT(3,_B);												} /* BIT  3,B		  */
OP(cb,59) { BIT(3,_C);												} /* BIT  3,C		  */
OP(cb,5a) { BIT(3,_D);												} /* BIT  3,D		  */
OP(cb,5b) { BIT(3,_E);												} /* BIT  3,E		  */
OP(cb,5c) { BIT(3,_H);												} /* BIT  3,H		  */
OP(cb,5d) { BIT(3,_L);												} /* BIT  3,L		  */
OP(cb,5e) { BIT(3,RM(_HL)); 										} /* BIT  3,(HL)	  */
OP(cb,5f) { BIT(3,_A);												} /* BIT  3,A		  */

OP(cb,60) { BIT(4,_B);												} /* BIT  4,B		  */
OP(cb,61) { BIT(4,_C);												} /* BIT  4,C		  */
OP(cb,62) { BIT(4,_D);												} /* BIT  4,D		  */
OP(cb,63) { BIT(4,_E);												} /* BIT  4,E		  */
OP(cb,64) { BIT(4,_H);												} /* BIT  4,H		  */
OP(cb,65) { BIT(4,_L);												} /* BIT  4,L		  */
OP(cb,66) { BIT(4,RM(_HL)); 										} /* BIT  4,(HL)	  */
OP(cb,67) { BIT(4,_A);												} /* BIT  4,A		  */

OP(cb,68) { BIT(5,_B);												} /* BIT  5,B		  */
OP(cb,69) { BIT(5,_C);												} /* BIT  5,C		  */
OP(cb,6a) { BIT(5,_D);												} /* BIT  5,D		  */
OP(cb,6b) { BIT(5,_E);												} /* BIT  5,E		  */
OP(cb,6c) { BIT(5,_H);												} /* BIT  5,H		  */
OP(cb,6d) { BIT(5,_L);												} /* BIT  5,L		  */
OP(cb,6e) { BIT(5,RM(_HL)); 										} /* BIT  5,(HL)	  */
OP(cb,6f) { BIT(5,_A);												} /* BIT  5,A		  */

OP(cb,70) { BIT(6,_B);												} /* BIT  6,B		  */
OP(cb,71) { BIT(6,_C);												} /* BIT  6,C		  */
OP(cb,72) { BIT(6,_D);												} /* BIT  6,D		  */
OP(cb,73) { BIT(6,_E);												} /* BIT  6,E		  */
OP(cb,74) { BIT(6,_H);												} /* BIT  6,H		  */
OP(cb,75) { BIT(6,_L);												} /* BIT  6,L		  */
OP(cb,76) { BIT(6,RM(_HL)); 										} /* BIT  6,(HL)	  */
OP(cb,77) { BIT(6,_A);												} /* BIT  6,A		  */

OP(cb,78) { BIT(7,_B);												} /* BIT  7,B		  */
OP(cb,79) { BIT(7,_C);												} /* BIT  7,C		  */
OP(cb,7a) { BIT(7,_D);												} /* BIT  7,D		  */
OP(cb,7b) { BIT(7,_E);												} /* BIT  7,E		  */
OP(cb,7c) { BIT(7,_H);												} /* BIT  7,H		  */
OP(cb,7d) { BIT(7,_L);												} /* BIT  7,L		  */
OP(cb,7e) { BIT(7,RM(_HL)); 										} /* BIT  7,(HL)	  */
OP(cb,7f) { BIT(7,_A);												} /* BIT  7,A		  */

OP(cb,80) { _B = RES(0,_B); 										} /* RES  0,B		  */
OP(cb,81) { _C = RES(0,_C); 										} /* RES  0,C		  */
OP(cb,82) { _D = RES(0,_D); 										} /* RES  0,D		  */
OP(cb,83) { _E = RES(0,_E); 										} /* RES  0,E		  */
OP(cb,84) { _H = RES(0,_H); 										} /* RES  0,H		  */
OP(cb,85) { _L = RES(0,_L); 										} /* RES  0,L		  */
OP(cb,86) { WM( _HL, RES(0,RM(_HL)) );								} /* RES  0,(HL)	  */
OP(cb,87) { _A = RES(0,_A); 										} /* RES  0,A		  */

OP(cb,88) { _B = RES(1,_B); 										} /* RES  1,B		  */
OP(cb,89) { _C = RES(1,_C); 										} /* RES  1,C		  */
OP(cb,8a) { _D = RES(1,_D); 										} /* RES  1,D		  */
OP(cb,8b) { _E = RES(1,_E); 										} /* RES  1,E		  */
OP(cb,8c) { _H = RES(1,_H); 										} /* RES  1,H		  */
OP(cb,8d) { _L = RES(1,_L); 										} /* RES  1,L		  */
OP(cb,8e) { WM( _HL, RES(1,RM(_HL)) );								} /* RES  1,(HL)	  */
OP(cb,8f) { _A = RES(1,_A); 										} /* RES  1,A		  */

OP(cb,90) { _B = RES(2,_B); 										} /* RES  2,B		  */
OP(cb,91) { _C = RES(2,_C); 										} /* RES  2,C		  */
OP(cb,92) { _D = RES(2,_D); 										} /* RES  2,D		  */
OP(cb,93) { _E = RES(2,_E); 										} /* RES  2,E		  */
OP(cb,94) { _H = RES(2,_H); 										} /* RES  2,H		  */
OP(cb,95) { _L = RES(2,_L); 										} /* RES  2,L		  */
OP(cb,96) { WM( _HL, RES(2,RM(_HL)) );								} /* RES  2,(HL)	  */
OP(cb,97) { _A = RES(2,_A); 										} /* RES  2,A		  */

OP(cb,98) { _B = RES(3,_B); 										} /* RES  3,B		  */
OP(cb,99) { _C = RES(3,_C); 										} /* RES  3,C		  */
OP(cb,9a) { _D = RES(3,_D); 										} /* RES  3,D		  */
OP(cb,9b) { _E = RES(3,_E); 										} /* RES  3,E		  */
OP(cb,9c) { _H = RES(3,_H); 										} /* RES  3,H		  */
OP(cb,9d) { _L = RES(3,_L); 										} /* RES  3,L		  */
OP(cb,9e) { WM( _HL, RES(3,RM(_HL)) );								} /* RES  3,(HL)	  */
OP(cb,9f) { _A = RES(3,_A); 										} /* RES  3,A		  */

OP(cb,a0) { _B = RES(4,_B); 										} /* RES  4,B		  */
OP(cb,a1) { _C = RES(4,_C); 										} /* RES  4,C		  */
OP(cb,a2) { _D = RES(4,_D); 										} /* RES  4,D		  */
OP(cb,a3) { _E = RES(4,_E); 										} /* RES  4,E		  */
OP(cb,a4) { _H = RES(4,_H); 										} /* RES  4,H		  */
OP(cb,a5) { _L = RES(4,_L); 										} /* RES  4,L		  */
OP(cb,a6) { WM( _HL, RES(4,RM(_HL)) );								} /* RES  4,(HL)	  */
OP(cb,a7) { _A = RES(4,_A); 										} /* RES  4,A		  */

OP(cb,a8) { _B = RES(5,_B); 										} /* RES  5,B		  */
OP(cb,a9) { _C = RES(5,_C); 										} /* RES  5,C		  */
OP(cb,aa) { _D = RES(5,_D); 										} /* RES  5,D		  */
OP(cb,ab) { _E = RES(5,_E); 										} /* RES  5,E		  */
OP(cb,ac) { _H = RES(5,_H); 										} /* RES  5,H		  */
OP(cb,ad) { _L = RES(5,_L); 										} /* RES  5,L		  */
OP(cb,ae) { WM( _HL, RES(5,RM(_HL)) );								} /* RES  5,(HL)	  */
OP(cb,af) { _A = RES(5,_A); 										} /* RES  5,A		  */

OP(cb,b0) { _B = RES(6,_B); 										} /* RES  6,B		  */
OP(cb,b1) { _C = RES(6,_C); 										} /* RES  6,C		  */
OP(cb,b2) { _D = RES(6,_D); 										} /* RES  6,D		  */
OP(cb,b3) { _E = RES(6,_E); 										} /* RES  6,E		  */
OP(cb,b4) { _H = RES(6,_H); 										} /* RES  6,H		  */
OP(cb,b5) { _L = RES(6,_L); 										} /* RES  6,L		  */
OP(cb,b6) { WM( _HL, RES(6,RM(_HL)) );								} /* RES  6,(HL)	  */
OP(cb,b7) { _A = RES(6,_A); 										} /* RES  6,A		  */

OP(cb,b8) { _B = RES(7,_B); 										} /* RES  7,B		  */
OP(cb,b9) { _C = RES(7,_C); 										} /* RES  7,C		  */
OP(cb,ba) { _D = RES(7,_D); 										} /* RES  7,D		  */
OP(cb,bb) { _E = RES(7,_E); 										} /* RES  7,E		  */
OP(cb,bc) { _H = RES(7,_H); 										} /* RES  7,H		  */
OP(cb,bd) { _L = RES(7,_L); 										} /* RES  7,L		  */
OP(cb,be) { WM( _HL, RES(7,RM(_HL)) );								} /* RES  7,(HL)	  */
OP(cb,bf) { _A = RES(7,_A); 										} /* RES  7,A		  */

OP(cb,c0) { _B = SET(0,_B); 										} /* SET  0,B		  */
OP(cb,c1) { _C = SET(0,_C); 										} /* SET  0,C		  */
OP(cb,c2) { _D = SET(0,_D); 										} /* SET  0,D		  */
OP(cb,c3) { _E = SET(0,_E); 										} /* SET  0,E		  */
OP(cb,c4) { _H = SET(0,_H); 										} /* SET  0,H		  */
OP(cb,c5) { _L = SET(0,_L); 										} /* SET  0,L		  */
OP(cb,c6) { WM( _HL, SET(0,RM(_HL)) );								} /* SET  0,(HL)	  */
OP(cb,c7) { _A = SET(0,_A); 										} /* SET  0,A		  */

OP(cb,c8) { _B = SET(1,_B); 										} /* SET  1,B		  */
OP(cb,c9) { _C = SET(1,_C); 										} /* SET  1,C		  */
OP(cb,ca) { _D = SET(1,_D); 										} /* SET  1,D		  */
OP(cb,cb) { _E = SET(1,_E); 										} /* SET  1,E		  */
OP(cb,cc) { _H = SET(1,_H); 										} /* SET  1,H		  */
OP(cb,cd) { _L = SET(1,_L); 										} /* SET  1,L		  */
OP(cb,ce) { WM( _HL, SET(1,RM(_HL)) );								} /* SET  1,(HL)	  */
OP(cb,cf) { _A = SET(1,_A); 										} /* SET  1,A		  */

OP(cb,d0) { _B = SET(2,_B); 										} /* SET  2,B		  */
OP(cb,d1) { _C = SET(2,_C); 										} /* SET  2,C		  */
OP(cb,d2) { _D = SET(2,_D); 										} /* SET  2,D		  */
OP(cb,d3) { _E = SET(2,_E); 										} /* SET  2,E		  */
OP(cb,d4) { _H = SET(2,_H); 										} /* SET  2,H		  */
OP(cb,d5) { _L = SET(2,_L); 										} /* SET  2,L		  */
OP(cb,d6) { WM( _HL, SET(2,RM(_HL)) );								}/* SET  2,(HL) 	 */
OP(cb,d7) { _A = SET(2,_A); 										} /* SET  2,A		  */

OP(cb,d8) { _B = SET(3,_B); 										} /* SET  3,B		  */
OP(cb,d9) { _C = SET(3,_C); 										} /* SET  3,C		  */
OP(cb,da) { _D = SET(3,_D); 										} /* SET  3,D		  */
OP(cb,db) { _E = SET(3,_E); 										} /* SET  3,E		  */
OP(cb,dc) { _H = SET(3,_H); 										} /* SET  3,H		  */
OP(cb,dd) { _L = SET(3,_L); 										} /* SET  3,L		  */
OP(cb,de) { WM( _HL, SET(3,RM(_HL)) );								} /* SET  3,(HL)	  */
OP(cb,df) { _A = SET(3,_A); 										} /* SET  3,A		  */

OP(cb,e0) { _B = SET(4,_B); 										} /* SET  4,B		  */
OP(cb,e1) { _C = SET(4,_C); 										} /* SET  4,C		  */
OP(cb,e2) { _D = SET(4,_D); 										} /* SET  4,D		  */
OP(cb,e3) { _E = SET(4,_E); 										} /* SET  4,E		  */
OP(cb,e4) { _H = SET(4,_H); 										} /* SET  4,H		  */
OP(cb,e5) { _L = SET(4,_L); 										} /* SET  4,L		  */
OP(cb,e6) { WM( _HL, SET(4,RM(_HL)) );								} /* SET  4,(HL)	  */
OP(cb,e7) { _A = SET(4,_A); 										} /* SET  4,A		  */

OP(cb,e8) { _B = SET(5,_B); 										} /* SET  5,B		  */
OP(cb,e9) { _C = SET(5,_C); 										} /* SET  5,C		  */
OP(cb,ea) { _D = SET(5,_D); 										} /* SET  5,D		  */
OP(cb,eb) { _E = SET(5,_E); 										} /* SET  5,E		  */
OP(cb,ec) { _H = SET(5,_H); 										} /* SET  5,H		  */
OP(cb,ed) { _L = SET(5,_L); 										} /* SET  5,L		  */
OP(cb,ee) { WM( _HL, SET(5,RM(_HL)) );								} /* SET  5,(HL)	  */
OP(cb,ef) { _A = SET(5,_A); 										} /* SET  5,A		  */

OP(cb,f0) { _B = SET(6,_B); 										} /* SET  6,B		  */
OP(cb,f1) { _C = SET(6,_C); 										} /* SET  6,C		  */
OP(cb,f2) { _D = SET(6,_D); 										} /* SET  6,D		  */
OP(cb,f3) { _E = SET(6,_E); 										} /* SET  6,E		  */
OP(cb,f4) { _H = SET(6,_H); 										} /* SET  6,H		  */
OP(cb,f5) { _L = SET(6,_L); 										} /* SET  6,L		  */
OP(cb,f6) { WM( _HL, SET(6,RM(_HL)) );								} /* SET  6,(HL)	  */
OP(cb,f7) { _A = SET(6,_A); 										} /* SET  6,A		  */

OP(cb,f8) { _B = SET(7,_B); 										} /* SET  7,B		  */
OP(cb,f9) { _C = SET(7,_C); 										} /* SET  7,C		  */
OP(cb,fa) { _D = SET(7,_D); 										} /* SET  7,D		  */
OP(cb,fb) { _E = SET(7,_E); 										} /* SET  7,E		  */
OP(cb,fc) { _H = SET(7,_H); 										} /* SET  7,H		  */
OP(cb,fd) { _L = SET(7,_L); 										} /* SET  7,L		  */
OP(cb,fe) { WM( _HL, SET(7,RM(_HL)) );								} /* SET  7,(HL)	  */
OP(cb,ff) { _A = SET(7,_A); 										} /* SET  7,A		  */


/**********************************************************
* opcodes with DD/FD CB prefix
* rotate, shift and bit operations with (IX+o)
**********************************************************/
OP(xxcb,00) { _B = RLC( RM(EA) ); WM( EA,_B );						} /* RLC  B=(XY+o)	  */
OP(xxcb,01) { _C = RLC( RM(EA) ); WM( EA,_C );						} /* RLC  C=(XY+o)	  */
OP(xxcb,02) { _D = RLC( RM(EA) ); WM( EA,_D );						} /* RLC  D=(XY+o)	  */
OP(xxcb,03) { _E = RLC( RM(EA) ); WM( EA,_E );						} /* RLC  E=(XY+o)	  */
OP(xxcb,04) { _H = RLC( RM(EA) ); WM( EA,_H );						} /* RLC  H=(XY+o)	  */
OP(xxcb,05) { _L = RLC( RM(EA) ); WM( EA,_L );						} /* RLC  L=(XY+o)	  */
OP(xxcb,06) { WM( EA, RLC( RM(EA) ) );								} /* RLC  (XY+o)	  */
OP(xxcb,07) { _A = RLC( RM(EA) ); WM( EA,_A );						} /* RLC  A=(XY+o)	  */

OP(xxcb,08) { _B = RRC( RM(EA) ); WM( EA,_B );						} /* RRC  B=(XY+o)	  */
OP(xxcb,09) { _C = RRC( RM(EA) ); WM( EA,_C );						} /* RRC  C=(XY+o)	  */
OP(xxcb,0a) { _D = RRC( RM(EA) ); WM( EA,_D );						} /* RRC  D=(XY+o)	  */
OP(xxcb,0b) { _E = RRC( RM(EA) ); WM( EA,_E );						} /* RRC  E=(XY+o)	  */
OP(xxcb,0c) { _H = RRC( RM(EA) ); WM( EA,_H );						} /* RRC  H=(XY+o)	  */
OP(xxcb,0d) { _L = RRC( RM(EA) ); WM( EA,_L );						} /* RRC  L=(XY+o)	  */
OP(xxcb,0e) { WM( EA,RRC( RM(EA) ) );								} /* RRC  (XY+o)	  */
OP(xxcb,0f) { _A = RRC( RM(EA) ); WM( EA,_A );						} /* RRC  A=(XY+o)	  */

OP(xxcb,10) { _B = RL( RM(EA) ); WM( EA,_B );						} /* RL   B=(XY+o)	  */
OP(xxcb,11) { _C = RL( RM(EA) ); WM( EA,_C );						} /* RL   C=(XY+o)	  */
OP(xxcb,12) { _D = RL( RM(EA) ); WM( EA,_D );						} /* RL   D=(XY+o)	  */
OP(xxcb,13) { _E = RL( RM(EA) ); WM( EA,_E );						} /* RL   E=(XY+o)	  */
OP(xxcb,14) { _H = RL( RM(EA) ); WM( EA,_H );						} /* RL   H=(XY+o)	  */
OP(xxcb,15) { _L = RL( RM(EA) ); WM( EA,_L );						} /* RL   L=(XY+o)	  */
OP(xxcb,16) { WM( EA,RL( RM(EA) ) );								} /* RL   (XY+o)	  */
OP(xxcb,17) { _A = RL( RM(EA) ); WM( EA,_A );						} /* RL   A=(XY+o)	  */

OP(xxcb,18) { _B = RR( RM(EA) ); WM( EA,_B );						} /* RR   B=(XY+o)	  */
OP(xxcb,19) { _C = RR( RM(EA) ); WM( EA,_C );						} /* RR   C=(XY+o)	  */
OP(xxcb,1a) { _D = RR( RM(EA) ); WM( EA,_D );						} /* RR   D=(XY+o)	  */
OP(xxcb,1b) { _E = RR( RM(EA) ); WM( EA,_E );						} /* RR   E=(XY+o)	  */
OP(xxcb,1c) { _H = RR( RM(EA) ); WM( EA,_H );						} /* RR   H=(XY+o)	  */
OP(xxcb,1d) { _L = RR( RM(EA) ); WM( EA,_L );						} /* RR   L=(XY+o)	  */
OP(xxcb,1e) { WM( EA,RR( RM(EA) ) );								} /* RR   (XY+o)	  */
OP(xxcb,1f) { _A = RR( RM(EA) ); WM( EA,_A );						} /* RR   A=(XY+o)	  */

OP(xxcb,20) { _B = SLA( RM(EA) ); WM( EA,_B );						} /* SLA  B=(XY+o)	  */
OP(xxcb,21) { _C = SLA( RM(EA) ); WM( EA,_C );						} /* SLA  C=(XY+o)	  */
OP(xxcb,22) { _D = SLA( RM(EA) ); WM( EA,_D );						} /* SLA  D=(XY+o)	  */
OP(xxcb,23) { _E = SLA( RM(EA) ); WM( EA,_E );						} /* SLA  E=(XY+o)	  */
OP(xxcb,24) { _H = SLA( RM(EA) ); WM( EA,_H );						} /* SLA  H=(XY+o)	  */
OP(xxcb,25) { _L = SLA( RM(EA) ); WM( EA,_L );						} /* SLA  L=(XY+o)	  */
OP(xxcb,26) { WM( EA,SLA( RM(EA) ) );								} /* SLA  (XY+o)	  */
OP(xxcb,27) { _A = SLA( RM(EA) ); WM( EA,_A );						} /* SLA  A=(XY+o)	  */

OP(xxcb,28) { _B = SRA( RM(EA) ); WM( EA,_B );						} /* SRA  B=(XY+o)	  */
OP(xxcb,29) { _C = SRA( RM(EA) ); WM( EA,_C );						} /* SRA  C=(XY+o)	  */
OP(xxcb,2a) { _D = SRA( RM(EA) ); WM( EA,_D );						} /* SRA  D=(XY+o)	  */
OP(xxcb,2b) { _E = SRA( RM(EA) ); WM( EA,_E );						} /* SRA  E=(XY+o)	  */
OP(xxcb,2c) { _H = SRA( RM(EA) ); WM( EA,_H );						} /* SRA  H=(XY+o)	  */
OP(xxcb,2d) { _L = SRA( RM(EA) ); WM( EA,_L );						} /* SRA  L=(XY+o)	  */
OP(xxcb,2e) { WM( EA,SRA( RM(EA) ) );								} /* SRA  (XY+o)	  */
OP(xxcb,2f) { _A = SRA( RM(EA) ); WM( EA,_A );						} /* SRA  A=(XY+o)	  */

OP(xxcb,30) { _B = SLL( RM(EA) ); WM( EA,_B );						} /* SLL  B=(XY+o)	  */
OP(xxcb,31) { _C = SLL( RM(EA) ); WM( EA,_C );						} /* SLL  C=(XY+o)	  */
OP(xxcb,32) { _D = SLL( RM(EA) ); WM( EA,_D );						} /* SLL  D=(XY+o)	  */
OP(xxcb,33) { _E = SLL( RM(EA) ); WM( EA,_E );						} /* SLL  E=(XY+o)	  */
OP(xxcb,34) { _H = SLL( RM(EA) ); WM( EA,_H );						} /* SLL  H=(XY+o)	  */
OP(xxcb,35) { _L = SLL( RM(EA) ); WM( EA,_L );						} /* SLL  L=(XY+o)	  */
OP(xxcb,36) { WM( EA,SLL( RM(EA) ) );								} /* SLL  (XY+o)	  */
OP(xxcb,37) { _A = SLL( RM(EA) ); WM( EA,_A );						} /* SLL  A=(XY+o)	  */

OP(xxcb,38) { _B = SRL( RM(EA) ); WM( EA,_B );						} /* SRL  B=(XY+o)	  */
OP(xxcb,39) { _C = SRL( RM(EA) ); WM( EA,_C );						} /* SRL  C=(XY+o)	  */
OP(xxcb,3a) { _D = SRL( RM(EA) ); WM( EA,_D );						} /* SRL  D=(XY+o)	  */
OP(xxcb,3b) { _E = SRL( RM(EA) ); WM( EA,_E );						} /* SRL  E=(XY+o)	  */
OP(xxcb,3c) { _H = SRL( RM(EA) ); WM( EA,_H );						} /* SRL  H=(XY+o)	  */
OP(xxcb,3d) { _L = SRL( RM(EA) ); WM( EA,_L );						} /* SRL  L=(XY+o)	  */
OP(xxcb,3e) { WM( EA,SRL( RM(EA) ) );								} /* SRL  (XY+o)	  */
OP(xxcb,3f) { _A = SRL( RM(EA) ); WM( EA,_A );						} /* SRL  A=(XY+o)	  */

OP(xxcb,40) { xxcb_46();											} /* BIT  0,B=(XY+o)  */
OP(xxcb,41) { xxcb_46();													  } /* BIT	0,C=(XY+o)	*/
OP(xxcb,42) { xxcb_46();											} /* BIT  0,D=(XY+o)  */
OP(xxcb,43) { xxcb_46();											} /* BIT  0,E=(XY+o)  */
OP(xxcb,44) { xxcb_46();											} /* BIT  0,H=(XY+o)  */
OP(xxcb,45) { xxcb_46();											} /* BIT  0,L=(XY+o)  */
OP(xxcb,46) { BIT_XY(0,RM(EA)); 									} /* BIT  0,(XY+o)	  */
OP(xxcb,47) { xxcb_46();											} /* BIT  0,A=(XY+o)  */

OP(xxcb,48) { xxcb_4e();											} /* BIT  1,B=(XY+o)  */
OP(xxcb,49) { xxcb_4e();													  } /* BIT	1,C=(XY+o)	*/
OP(xxcb,4a) { xxcb_4e();											} /* BIT  1,D=(XY+o)  */
OP(xxcb,4b) { xxcb_4e();											} /* BIT  1,E=(XY+o)  */
OP(xxcb,4c) { xxcb_4e();											} /* BIT  1,H=(XY+o)  */
OP(xxcb,4d) { xxcb_4e();											} /* BIT  1,L=(XY+o)  */
OP(xxcb,4e) { BIT_XY(1,RM(EA)); 									} /* BIT  1,(XY+o)	  */
OP(xxcb,4f) { xxcb_4e();											} /* BIT  1,A=(XY+o)  */

OP(xxcb,50) { xxcb_56();											} /* BIT  2,B=(XY+o)  */
OP(xxcb,51) { xxcb_56();													  } /* BIT	2,C=(XY+o)	*/
OP(xxcb,52) { xxcb_56();											} /* BIT  2,D=(XY+o)  */
OP(xxcb,53) { xxcb_56();											} /* BIT  2,E=(XY+o)  */
OP(xxcb,54) { xxcb_56();											} /* BIT  2,H=(XY+o)  */
OP(xxcb,55) { xxcb_56();											} /* BIT  2,L=(XY+o)  */
OP(xxcb,56) { BIT_XY(2,RM(EA)); 									} /* BIT  2,(XY+o)	  */
OP(xxcb,57) { xxcb_56();											} /* BIT  2,A=(XY+o)  */

OP(xxcb,58) { xxcb_5e();											} /* BIT  3,B=(XY+o)  */
OP(xxcb,59) { xxcb_5e();													  } /* BIT	3,C=(XY+o)	*/
OP(xxcb,5a) { xxcb_5e();											} /* BIT  3,D=(XY+o)  */
OP(xxcb,5b) { xxcb_5e();											} /* BIT  3,E=(XY+o)  */
OP(xxcb,5c) { xxcb_5e();											} /* BIT  3,H=(XY+o)  */
OP(xxcb,5d) { xxcb_5e();											} /* BIT  3,L=(XY+o)  */
OP(xxcb,5e) { BIT_XY(3,RM(EA)); 									} /* BIT  3,(XY+o)	  */
OP(xxcb,5f) { xxcb_5e();											} /* BIT  3,A=(XY+o)  */

OP(xxcb,60) { xxcb_66();											} /* BIT  4,B=(XY+o)  */
OP(xxcb,61) { xxcb_66();													  } /* BIT	4,C=(XY+o)	*/
OP(xxcb,62) { xxcb_66();											} /* BIT  4,D=(XY+o)  */
OP(xxcb,63) { xxcb_66();											} /* BIT  4,E=(XY+o)  */
OP(xxcb,64) { xxcb_66();											} /* BIT  4,H=(XY+o)  */
OP(xxcb,65) { xxcb_66();											} /* BIT  4,L=(XY+o)  */
OP(xxcb,66) { BIT_XY(4,RM(EA)); 									} /* BIT  4,(XY+o)	  */
OP(xxcb,67) { xxcb_66();											} /* BIT  4,A=(XY+o)  */

OP(xxcb,68) { xxcb_6e();											} /* BIT  5,B=(XY+o)  */
OP(xxcb,69) { xxcb_6e();													  } /* BIT	5,C=(XY+o)	*/
OP(xxcb,6a) { xxcb_6e();											} /* BIT  5,D=(XY+o)  */
OP(xxcb,6b) { xxcb_6e();											} /* BIT  5,E=(XY+o)  */
OP(xxcb,6c) { xxcb_6e();											} /* BIT  5,H=(XY+o)  */
OP(xxcb,6d) { xxcb_6e();											} /* BIT  5,L=(XY+o)  */
OP(xxcb,6e) { BIT_XY(5,RM(EA)); 									} /* BIT  5,(XY+o)	  */
OP(xxcb,6f) { xxcb_6e();											} /* BIT  5,A=(XY+o)  */

OP(xxcb,70) { xxcb_76();											} /* BIT  6,B=(XY+o)  */
OP(xxcb,71) { xxcb_76();													  } /* BIT	6,C=(XY+o)	*/
OP(xxcb,72) { xxcb_76();											} /* BIT  6,D=(XY+o)  */
OP(xxcb,73) { xxcb_76();											} /* BIT  6,E=(XY+o)  */
OP(xxcb,74) { xxcb_76();											} /* BIT  6,H=(XY+o)  */
OP(xxcb,75) { xxcb_76();											} /* BIT  6,L=(XY+o)  */
OP(xxcb,76) { BIT_XY(6,RM(EA)); 									} /* BIT  6,(XY+o)	  */
OP(xxcb,77) { xxcb_76();											} /* BIT  6,A=(XY+o)  */

OP(xxcb,78) { xxcb_7e();											} /* BIT  7,B=(XY+o)  */
OP(xxcb,79) { xxcb_7e();													  } /* BIT	7,C=(XY+o)	*/
OP(xxcb,7a) { xxcb_7e();											} /* BIT  7,D=(XY+o)  */
OP(xxcb,7b) { xxcb_7e();											} /* BIT  7,E=(XY+o)  */
OP(xxcb,7c) { xxcb_7e();											} /* BIT  7,H=(XY+o)  */
OP(xxcb,7d) { xxcb_7e();											} /* BIT  7,L=(XY+o)  */
OP(xxcb,7e) { BIT_XY(7,RM(EA)); 									} /* BIT  7,(XY+o)	  */
OP(xxcb,7f) { xxcb_7e();											} /* BIT  7,A=(XY+o)  */

OP(xxcb,80) { _B = RES(0, RM(EA) ); WM( EA,_B );					} /* RES  0,B=(XY+o)  */
OP(xxcb,81) { _C = RES(0, RM(EA) ); WM( EA,_C );					} /* RES  0,C=(XY+o)  */
OP(xxcb,82) { _D = RES(0, RM(EA) ); WM( EA,_D );					} /* RES  0,D=(XY+o)  */
OP(xxcb,83) { _E = RES(0, RM(EA) ); WM( EA,_E );					} /* RES  0,E=(XY+o)  */
OP(xxcb,84) { _H = RES(0, RM(EA) ); WM( EA,_H );					} /* RES  0,H=(XY+o)  */
OP(xxcb,85) { _L = RES(0, RM(EA) ); WM( EA,_L );					} /* RES  0,L=(XY+o)  */
OP(xxcb,86) { WM( EA, RES(0,RM(EA)) );								} /* RES  0,(XY+o)	  */
OP(xxcb,87) { _A = RES(0, RM(EA) ); WM( EA,_A );					} /* RES  0,A=(XY+o)  */

OP(xxcb,88) { _B = RES(1, RM(EA) ); WM( EA,_B );					} /* RES  1,B=(XY+o)  */
OP(xxcb,89) { _C = RES(1, RM(EA) ); WM( EA,_C );					} /* RES  1,C=(XY+o)  */
OP(xxcb,8a) { _D = RES(1, RM(EA) ); WM( EA,_D );					} /* RES  1,D=(XY+o)  */
OP(xxcb,8b) { _E = RES(1, RM(EA) ); WM( EA,_E );					} /* RES  1,E=(XY+o)  */
OP(xxcb,8c) { _H = RES(1, RM(EA) ); WM( EA,_H );					} /* RES  1,H=(XY+o)  */
OP(xxcb,8d) { _L = RES(1, RM(EA) ); WM( EA,_L );					} /* RES  1,L=(XY+o)  */
OP(xxcb,8e) { WM( EA, RES(1,RM(EA)) );								} /* RES  1,(XY+o)	  */
OP(xxcb,8f) { _A = RES(1, RM(EA) ); WM( EA,_A );					} /* RES  1,A=(XY+o)  */

OP(xxcb,90) { _B = RES(2, RM(EA) ); WM( EA,_B );					} /* RES  2,B=(XY+o)  */
OP(xxcb,91) { _C = RES(2, RM(EA) ); WM( EA,_C );					} /* RES  2,C=(XY+o)  */
OP(xxcb,92) { _D = RES(2, RM(EA) ); WM( EA,_D );					} /* RES  2,D=(XY+o)  */
OP(xxcb,93) { _E = RES(2, RM(EA) ); WM( EA,_E );					} /* RES  2,E=(XY+o)  */
OP(xxcb,94) { _H = RES(2, RM(EA) ); WM( EA,_H );					} /* RES  2,H=(XY+o)  */
OP(xxcb,95) { _L = RES(2, RM(EA) ); WM( EA,_L );					} /* RES  2,L=(XY+o)  */
OP(xxcb,96) { WM( EA, RES(2,RM(EA)) );								} /* RES  2,(XY+o)	  */
OP(xxcb,97) { _A = RES(2, RM(EA) ); WM( EA,_A );					} /* RES  2,A=(XY+o)  */

OP(xxcb,98) { _B = RES(3, RM(EA) ); WM( EA,_B );					} /* RES  3,B=(XY+o)  */
OP(xxcb,99) { _C = RES(3, RM(EA) ); WM( EA,_C );					} /* RES  3,C=(XY+o)  */
OP(xxcb,9a) { _D = RES(3, RM(EA) ); WM( EA,_D );					} /* RES  3,D=(XY+o)  */
OP(xxcb,9b) { _E = RES(3, RM(EA) ); WM( EA,_E );					} /* RES  3,E=(XY+o)  */
OP(xxcb,9c) { _H = RES(3, RM(EA) ); WM( EA,_H );					} /* RES  3,H=(XY+o)  */
OP(xxcb,9d) { _L = RES(3, RM(EA) ); WM( EA,_L );					} /* RES  3,L=(XY+o)  */
OP(xxcb,9e) { WM( EA, RES(3,RM(EA)) );								} /* RES  3,(XY+o)	  */
OP(xxcb,9f) { _A = RES(3, RM(EA) ); WM( EA,_A );					} /* RES  3,A=(XY+o)  */

OP(xxcb,a0) { _B = RES(4, RM(EA) ); WM( EA,_B );					} /* RES  4,B=(XY+o)  */
OP(xxcb,a1) { _C = RES(4, RM(EA) ); WM( EA,_C );					} /* RES  4,C=(XY+o)  */
OP(xxcb,a2) { _D = RES(4, RM(EA) ); WM( EA,_D );					} /* RES  4,D=(XY+o)  */
OP(xxcb,a3) { _E = RES(4, RM(EA) ); WM( EA,_E );					} /* RES  4,E=(XY+o)  */
OP(xxcb,a4) { _H = RES(4, RM(EA) ); WM( EA,_H );					} /* RES  4,H=(XY+o)  */
OP(xxcb,a5) { _L = RES(4, RM(EA) ); WM( EA,_L );					} /* RES  4,L=(XY+o)  */
OP(xxcb,a6) { WM( EA, RES(4,RM(EA)) );								} /* RES  4,(XY+o)	  */
OP(xxcb,a7) { _A = RES(4, RM(EA) ); WM( EA,_A );					} /* RES  4,A=(XY+o)  */

OP(xxcb,a8) { _B = RES(5, RM(EA) ); WM( EA,_B );					} /* RES  5,B=(XY+o)  */
OP(xxcb,a9) { _C = RES(5, RM(EA) ); WM( EA,_C );					} /* RES  5,C=(XY+o)  */
OP(xxcb,aa) { _D = RES(5, RM(EA) ); WM( EA,_D );					} /* RES  5,D=(XY+o)  */
OP(xxcb,ab) { _E = RES(5, RM(EA) ); WM( EA,_E );					} /* RES  5,E=(XY+o)  */
OP(xxcb,ac) { _H = RES(5, RM(EA) ); WM( EA,_H );					} /* RES  5,H=(XY+o)  */
OP(xxcb,ad) { _L = RES(5, RM(EA) ); WM( EA,_L );					} /* RES  5,L=(XY+o)  */
OP(xxcb,ae) { WM( EA, RES(5,RM(EA)) );								} /* RES  5,(XY+o)	  */
OP(xxcb,af) { _A = RES(5, RM(EA) ); WM( EA,_A );					} /* RES  5,A=(XY+o)  */

OP(xxcb,b0) { _B = RES(6, RM(EA) ); WM( EA,_B );					} /* RES  6,B=(XY+o)  */
OP(xxcb,b1) { _C = RES(6, RM(EA) ); WM( EA,_C );					} /* RES  6,C=(XY+o)  */
OP(xxcb,b2) { _D = RES(6, RM(EA) ); WM( EA,_D );					} /* RES  6,D=(XY+o)  */
OP(xxcb,b3) { _E = RES(6, RM(EA) ); WM( EA,_E );					} /* RES  6,E=(XY+o)  */
OP(xxcb,b4) { _H = RES(6, RM(EA) ); WM( EA,_H );					} /* RES  6,H=(XY+o)  */
OP(xxcb,b5) { _L = RES(6, RM(EA) ); WM( EA,_L );					} /* RES  6,L=(XY+o)  */
OP(xxcb,b6) { WM( EA, RES(6,RM(EA)) );								} /* RES  6,(XY+o)	  */
OP(xxcb,b7) { _A = RES(6, RM(EA) ); WM( EA,_A );					} /* RES  6,A=(XY+o)  */

OP(xxcb,b8) { _B = RES(7, RM(EA) ); WM( EA,_B );					} /* RES  7,B=(XY+o)  */
OP(xxcb,b9) { _C = RES(7, RM(EA) ); WM( EA,_C );					} /* RES  7,C=(XY+o)  */
OP(xxcb,ba) { _D = RES(7, RM(EA) ); WM( EA,_D );					} /* RES  7,D=(XY+o)  */
OP(xxcb,bb) { _E = RES(7, RM(EA) ); WM( EA,_E );					} /* RES  7,E=(XY+o)  */
OP(xxcb,bc) { _H = RES(7, RM(EA) ); WM( EA,_H );					} /* RES  7,H=(XY+o)  */
OP(xxcb,bd) { _L = RES(7, RM(EA) ); WM( EA,_L );					} /* RES  7,L=(XY+o)  */
OP(xxcb,be) { WM( EA, RES(7,RM(EA)) );								} /* RES  7,(XY+o)	  */
OP(xxcb,bf) { _A = RES(7, RM(EA) ); WM( EA,_A );					} /* RES  7,A=(XY+o)  */

OP(xxcb,c0) { _B = SET(0, RM(EA) ); WM( EA,_B );					} /* SET  0,B=(XY+o)  */
OP(xxcb,c1) { _C = SET(0, RM(EA) ); WM( EA,_C );					} /* SET  0,C=(XY+o)  */
OP(xxcb,c2) { _D = SET(0, RM(EA) ); WM( EA,_D );					} /* SET  0,D=(XY+o)  */
OP(xxcb,c3) { _E = SET(0, RM(EA) ); WM( EA,_E );					} /* SET  0,E=(XY+o)  */
OP(xxcb,c4) { _H = SET(0, RM(EA) ); WM( EA,_H );					} /* SET  0,H=(XY+o)  */
OP(xxcb,c5) { _L = SET(0, RM(EA) ); WM( EA,_L );					} /* SET  0,L=(XY+o)  */
OP(xxcb,c6) { WM( EA, SET(0,RM(EA)) );								} /* SET  0,(XY+o)	  */
OP(xxcb,c7) { _A = SET(0, RM(EA) ); WM( EA,_A );					} /* SET  0,A=(XY+o)  */

OP(xxcb,c8) { _B = SET(1, RM(EA) ); WM( EA,_B );					} /* SET  1,B=(XY+o)  */
OP(xxcb,c9) { _C = SET(1, RM(EA) ); WM( EA,_C );					} /* SET  1,C=(XY+o)  */
OP(xxcb,ca) { _D = SET(1, RM(EA) ); WM( EA,_D );					} /* SET  1,D=(XY+o)  */
OP(xxcb,cb) { _E = SET(1, RM(EA) ); WM( EA,_E );					} /* SET  1,E=(XY+o)  */
OP(xxcb,cc) { _H = SET(1, RM(EA) ); WM( EA,_H );					} /* SET  1,H=(XY+o)  */
OP(xxcb,cd) { _L = SET(1, RM(EA) ); WM( EA,_L );					} /* SET  1,L=(XY+o)  */
OP(xxcb,ce) { WM( EA, SET(1,RM(EA)) );								} /* SET  1,(XY+o)	  */
OP(xxcb,cf) { _A = SET(1, RM(EA) ); WM( EA,_A );					} /* SET  1,A=(XY+o)  */

OP(xxcb,d0) { _B = SET(2, RM(EA) ); WM( EA,_B );					} /* SET  2,B=(XY+o)  */
OP(xxcb,d1) { _C = SET(2, RM(EA) ); WM( EA,_C );					} /* SET  2,C=(XY+o)  */
OP(xxcb,d2) { _D = SET(2, RM(EA) ); WM( EA,_D );					} /* SET  2,D=(XY+o)  */
OP(xxcb,d3) { _E = SET(2, RM(EA) ); WM( EA,_E );					} /* SET  2,E=(XY+o)  */
OP(xxcb,d4) { _H = SET(2, RM(EA) ); WM( EA,_H );					} /* SET  2,H=(XY+o)  */
OP(xxcb,d5) { _L = SET(2, RM(EA) ); WM( EA,_L );					} /* SET  2,L=(XY+o)  */
OP(xxcb,d6) { WM( EA, SET(2,RM(EA)) );								} /* SET  2,(XY+o)	  */
OP(xxcb,d7) { _A = SET(2, RM(EA) ); WM( EA,_A );					} /* SET  2,A=(XY+o)  */

OP(xxcb,d8) { _B = SET(3, RM(EA) ); WM( EA,_B );					} /* SET  3,B=(XY+o)  */
OP(xxcb,d9) { _C = SET(3, RM(EA) ); WM( EA,_C );					} /* SET  3,C=(XY+o)  */
OP(xxcb,da) { _D = SET(3, RM(EA) ); WM( EA,_D );					} /* SET  3,D=(XY+o)  */
OP(xxcb,db) { _E = SET(3, RM(EA) ); WM( EA,_E );					} /* SET  3,E=(XY+o)  */
OP(xxcb,dc) { _H = SET(3, RM(EA) ); WM( EA,_H );					} /* SET  3,H=(XY+o)  */
OP(xxcb,dd) { _L = SET(3, RM(EA) ); WM( EA,_L );					} /* SET  3,L=(XY+o)  */
OP(xxcb,de) { WM( EA, SET(3,RM(EA)) );								} /* SET  3,(XY+o)	  */
OP(xxcb,df) { _A = SET(3, RM(EA) ); WM( EA,_A );					} /* SET  3,A=(XY+o)  */

OP(xxcb,e0) { _B = SET(4, RM(EA) ); WM( EA,_B );					} /* SET  4,B=(XY+o)  */
OP(xxcb,e1) { _C = SET(4, RM(EA) ); WM( EA,_C );					} /* SET  4,C=(XY+o)  */
OP(xxcb,e2) { _D = SET(4, RM(EA) ); WM( EA,_D );					} /* SET  4,D=(XY+o)  */
OP(xxcb,e3) { _E = SET(4, RM(EA) ); WM( EA,_E );					} /* SET  4,E=(XY+o)  */
OP(xxcb,e4) { _H = SET(4, RM(EA) ); WM( EA,_H );					} /* SET  4,H=(XY+o)  */
OP(xxcb,e5) { _L = SET(4, RM(EA) ); WM( EA,_L );					} /* SET  4,L=(XY+o)  */
OP(xxcb,e6) { WM( EA, SET(4,RM(EA)) );								} /* SET  4,(XY+o)	  */
OP(xxcb,e7) { _A = SET(4, RM(EA) ); WM( EA,_A );					} /* SET  4,A=(XY+o)  */

OP(xxcb,e8) { _B = SET(5, RM(EA) ); WM( EA,_B );					} /* SET  5,B=(XY+o)  */
OP(xxcb,e9) { _C = SET(5, RM(EA) ); WM( EA,_C );					} /* SET  5,C=(XY+o)  */
OP(xxcb,ea) { _D = SET(5, RM(EA) ); WM( EA,_D );					} /* SET  5,D=(XY+o)  */
OP(xxcb,eb) { _E = SET(5, RM(EA) ); WM( EA,_E );					} /* SET  5,E=(XY+o)  */
OP(xxcb,ec) { _H = SET(5, RM(EA) ); WM( EA,_H );					} /* SET  5,H=(XY+o)  */
OP(xxcb,ed) { _L = SET(5, RM(EA) ); WM( EA,_L );					} /* SET  5,L=(XY+o)  */
OP(xxcb,ee) { WM( EA, SET(5,RM(EA)) );								} /* SET  5,(XY+o)	  */
OP(xxcb,ef) { _A = SET(5, RM(EA) ); WM( EA,_A );					} /* SET  5,A=(XY+o)  */

OP(xxcb,f0) { _B = SET(6, RM(EA) ); WM( EA,_B );					} /* SET  6,B=(XY+o)  */
OP(xxcb,f1) { _C = SET(6, RM(EA) ); WM( EA,_C );					} /* SET  6,C=(XY+o)  */
OP(xxcb,f2) { _D = SET(6, RM(EA) ); WM( EA,_D );					} /* SET  6,D=(XY+o)  */
OP(xxcb,f3) { _E = SET(6, RM(EA) ); WM( EA,_E );					} /* SET  6,E=(XY+o)  */
OP(xxcb,f4) { _H = SET(6, RM(EA) ); WM( EA,_H );					} /* SET  6,H=(XY+o)  */
OP(xxcb,f5) { _L = SET(6, RM(EA) ); WM( EA,_L );					} /* SET  6,L=(XY+o)  */
OP(xxcb,f6) { WM( EA, SET(6,RM(EA)) );								} /* SET  6,(XY+o)	  */
OP(xxcb,f7) { _A = SET(6, RM(EA) ); WM( EA,_A );					} /* SET  6,A=(XY+o)  */

OP(xxcb,f8) { _B = SET(7, RM(EA) ); WM( EA,_B );					} /* SET  7,B=(XY+o)  */
OP(xxcb,f9) { _C = SET(7, RM(EA) ); WM( EA,_C );					} /* SET  7,C=(XY+o)  */
OP(xxcb,fa) { _D = SET(7, RM(EA) ); WM( EA,_D );					} /* SET  7,D=(XY+o)  */
OP(xxcb,fb) { _E = SET(7, RM(EA) ); WM( EA,_E );					} /* SET  7,E=(XY+o)  */
OP(xxcb,fc) { _H = SET(7, RM(EA) ); WM( EA,_H );					} /* SET  7,H=(XY+o)  */
OP(xxcb,fd) { _L = SET(7, RM(EA) ); WM( EA,_L );					} /* SET  7,L=(XY+o)  */
OP(xxcb,fe) { WM( EA, SET(7,RM(EA)) );								} /* SET  7,(XY+o)	  */
OP(xxcb,ff) { _A = SET(7, RM(EA) ); WM( EA,_A );					} /* SET  7,A=(XY+o)  */

OP(illegal,1) {
	_PC--;
}

/**********************************************************
 * IX register related opcodes (DD prefix)
 **********************************************************/
OP(dd,00) { illegal_1();											} /* DB   DD		  */
OP(dd,01) { illegal_1();											} /* DB   DD		  */
OP(dd,02) { illegal_1();											} /* DB   DD		  */
OP(dd,03) { illegal_1();											} /* DB   DD		  */
OP(dd,04) { illegal_1();											} /* DB   DD		  */
OP(dd,05) { illegal_1();											} /* DB   DD		  */
OP(dd,06) { illegal_1();											} /* DB   DD		  */
OP(dd,07) { illegal_1();											} /* DB   DD		  */

OP(dd,08) { illegal_1();											} /* DB   DD		  */
OP(dd,09) { ADD16(IX,BC);											} /* ADD  IX,BC 	  */
OP(dd,0a) { illegal_1();											} /* DB   DD		  */
OP(dd,0b) { illegal_1();											} /* DB   DD		  */
OP(dd,0c) { illegal_1();											} /* DB   DD		  */
OP(dd,0d) { illegal_1();											} /* DB   DD		  */
OP(dd,0e) { illegal_1();											} /* DB   DD		  */
OP(dd,0f) { illegal_1();											} /* DB   DD		  */

OP(dd,10) { illegal_1();											} /* DB   DD		  */
OP(dd,11) { illegal_1();											} /* DB   DD		  */
OP(dd,12) { illegal_1();											} /* DB   DD		  */
OP(dd,13) { illegal_1();											} /* DB   DD		  */
OP(dd,14) { illegal_1();											} /* DB   DD		  */
OP(dd,15) { illegal_1();											} /* DB   DD		  */
OP(dd,16) { illegal_1();											} /* DB   DD		  */
OP(dd,17) { illegal_1();											} /* DB   DD		  */

OP(dd,18) { illegal_1();											} /* DB   DD		  */
OP(dd,19) { ADD16(IX,DE);											} /* ADD  IX,DE 	  */
OP(dd,1a) { illegal_1();											} /* DB   DD		  */
OP(dd,1b) { illegal_1();											} /* DB   DD		  */
OP(dd,1c) { illegal_1();											} /* DB   DD		  */
OP(dd,1d) { illegal_1();											} /* DB   DD		  */
OP(dd,1e) { illegal_1();											} /* DB   DD		  */
OP(dd,1f) { illegal_1();											} /* DB   DD		  */

OP(dd,20) { illegal_1();											} /* DB   DD		  */
OP(dd,21) { _IX = ARG16();											} /* LD   IX,w		  */
OP(dd,22) { EA = ARG16(); WM16( EA, &Z80.IX );						} /* LD   (w),IX	  */
OP(dd,23) { _IX++;													} /* INC  IX		  */
OP(dd,24) { _HX = INC(_HX); 										} /* INC  HX		  */
OP(dd,25) { _HX = DEC(_HX); 										} /* DEC  HX		  */
OP(dd,26) { _HX = ARG();											} /* LD   HX,n		  */
OP(dd,27) { illegal_1();											} /* DB   DD		  */

OP(dd,28) { illegal_1();											} /* DB   DD		  */
OP(dd,29) { ADD16(IX,IX);											} /* ADD  IX,IX 	  */
OP(dd,2a) { EA = ARG16(); RM16( EA, &Z80.IX );						} /* LD   IX,(w)	  */
OP(dd,2b) { _IX--;													} /* DEC  IX		  */
OP(dd,2c) { _LX = INC(_LX); 										} /* INC  LX		  */
OP(dd,2d) { _LX = DEC(_LX); 										} /* DEC  LX		  */
OP(dd,2e) { _LX = ARG();											} /* LD   LX,n		  */
OP(dd,2f) { illegal_1();											} /* DB   DD		  */

OP(dd,30) { illegal_1();											} /* DB   DD		  */
OP(dd,31) { illegal_1();											} /* DB   DD		  */
OP(dd,32) { illegal_1();											} /* DB   DD		  */
OP(dd,33) { illegal_1();											} /* DB   DD		  */
OP(dd,34) { EAX; WM( EA, INC(RM(EA)) ); 							} /* INC  (IX+o)	  */
OP(dd,35) { EAX; WM( EA, DEC(RM(EA)) ); 							} /* DEC  (IX+o)	  */
OP(dd,36) { EAX; WM( EA, ARG() );									} /* LD   (IX+o),n	  */
OP(dd,37) { illegal_1();											} /* DB   DD		  */

OP(dd,38) { illegal_1();											} /* DB   DD		  */
OP(dd,39) { ADD16(IX,SP);											} /* ADD  IX,SP 	  */
OP(dd,3a) { illegal_1();											} /* DB   DD		  */
OP(dd,3b) { illegal_1();											} /* DB   DD		  */
OP(dd,3c) { illegal_1();											} /* DB   DD		  */
OP(dd,3d) { illegal_1();											} /* DB   DD		  */
OP(dd,3e) { illegal_1();											} /* DB   DD		  */
OP(dd,3f) { illegal_1();											} /* DB   DD		  */

OP(dd,40) { illegal_1();											} /* DB   DD		  */
OP(dd,41) { illegal_1();											} /* DB   DD		  */
OP(dd,42) { illegal_1();											} /* DB   DD		  */
OP(dd,43) { illegal_1();											} /* DB   DD		  */
OP(dd,44) { _B = _HX;												} /* LD   B,HX		  */
OP(dd,45) { _B = _LX;												} /* LD   B,LX		  */
OP(dd,46) { EAX; _B = RM(EA);										} /* LD   B,(IX+o)	  */
OP(dd,47) { illegal_1();											} /* DB   DD		  */

OP(dd,48) { illegal_1();											} /* DB   DD		  */
OP(dd,49) { illegal_1();											} /* DB   DD		  */
OP(dd,4a) { illegal_1();											} /* DB   DD		  */
OP(dd,4b) { illegal_1();											} /* DB   DD		  */
OP(dd,4c) { _C = _HX;												} /* LD   C,HX		  */
OP(dd,4d) { _C = _LX;												} /* LD   C,LX		  */
OP(dd,4e) { EAX; _C = RM(EA);										} /* LD   C,(IX+o)	  */
OP(dd,4f) { illegal_1();											} /* DB   DD		  */

OP(dd,50) { illegal_1();											} /* DB   DD		  */
OP(dd,51) { illegal_1();											} /* DB   DD		  */
OP(dd,52) { illegal_1();											} /* DB   DD		  */
OP(dd,53) { illegal_1();											} /* DB   DD		  */
OP(dd,54) { _D = _HX;												} /* LD   D,HX		  */
OP(dd,55) { _D = _LX;												} /* LD   D,LX		  */
OP(dd,56) { EAX; _D = RM(EA);										} /* LD   D,(IX+o)	  */
OP(dd,57) { illegal_1();											} /* DB   DD		  */

OP(dd,58) { illegal_1();											} /* DB   DD		  */
OP(dd,59) { illegal_1();											} /* DB   DD		  */
OP(dd,5a) { illegal_1();											} /* DB   DD		  */
OP(dd,5b) { illegal_1();											} /* DB   DD		  */
OP(dd,5c) { _E = _HX;												} /* LD   E,HX		  */
OP(dd,5d) { _E = _LX;												} /* LD   E,LX		  */
OP(dd,5e) { EAX; _E = RM(EA);										} /* LD   E,(IX+o)	  */
OP(dd,5f) { illegal_1();											} /* DB   DD		  */

OP(dd,60) { _HX = _B;												} /* LD   HX,B		  */
OP(dd,61) { _HX = _C;												} /* LD   HX,C		  */
OP(dd,62) { _HX = _D;												} /* LD   HX,D		  */
OP(dd,63) { _HX = _E;												} /* LD   HX,E		  */
OP(dd,64) { 														} /* LD   HX,HX 	  */
OP(dd,65) { _HX = _LX;												} /* LD   HX,LX 	  */
OP(dd,66) { EAX; _H = RM(EA);										} /* LD   H,(IX+o)	  */
OP(dd,67) { _HX = _A;												} /* LD   HX,A		  */

OP(dd,68) { _LX = _B;												} /* LD   LX,B		  */
OP(dd,69) { _LX = _C;												} /* LD   LX,C		  */
OP(dd,6a) { _LX = _D;												} /* LD   LX,D		  */
OP(dd,6b) { _LX = _E;												} /* LD   LX,E		  */
OP(dd,6c) { _LX = _HX;												} /* LD   LX,HX 	  */
OP(dd,6d) { 														} /* LD   LX,LX 	  */
OP(dd,6e) { EAX; _L = RM(EA);										} /* LD   L,(IX+o)	  */
OP(dd,6f) { _LX = _A;												} /* LD   LX,A		  */

OP(dd,70) { EAX; WM( EA, _B );										} /* LD   (IX+o),B	  */
OP(dd,71) { EAX; WM( EA, _C );										} /* LD   (IX+o),C	  */
OP(dd,72) { EAX; WM( EA, _D );										} /* LD   (IX+o),D	  */
OP(dd,73) { EAX; WM( EA, _E );										} /* LD   (IX+o),E	  */
OP(dd,74) { EAX; WM( EA, _H );										} /* LD   (IX+o),H	  */
OP(dd,75) { EAX; WM( EA, _L );										} /* LD   (IX+o),L	  */
OP(dd,76) { illegal_1();											}		  /* DB   DD		  */
OP(dd,77) { EAX; WM( EA, _A );										} /* LD   (IX+o),A	  */

OP(dd,78) { illegal_1();											} /* DB   DD		  */
OP(dd,79) { illegal_1();											} /* DB   DD		  */
OP(dd,7a) { illegal_1();											} /* DB   DD		  */
OP(dd,7b) { illegal_1();											} /* DB   DD		  */
OP(dd,7c) { _A = _HX;												} /* LD   A,HX		  */
OP(dd,7d) { _A = _LX;												} /* LD   A,LX		  */
OP(dd,7e) { EAX; _A = RM(EA);										} /* LD   A,(IX+o)	  */
OP(dd,7f) { illegal_1();											} /* DB   DD		  */

OP(dd,80) { illegal_1();											} /* DB   DD		  */
OP(dd,81) { illegal_1();											} /* DB   DD		  */
OP(dd,82) { illegal_1();											} /* DB   DD		  */
OP(dd,83) { illegal_1();											} /* DB   DD		  */
OP(dd,84) { ADD(_HX);												} /* ADD  A,HX		  */
OP(dd,85) { ADD(_LX);												} /* ADD  A,LX		  */
OP(dd,86) { EAX; ADD(RM(EA));										} /* ADD  A,(IX+o)	  */
OP(dd,87) { illegal_1();											} /* DB   DD		  */

OP(dd,88) { illegal_1();											} /* DB   DD		  */
OP(dd,89) { illegal_1();											} /* DB   DD		  */
OP(dd,8a) { illegal_1();											} /* DB   DD		  */
OP(dd,8b) { illegal_1();											} /* DB   DD		  */
OP(dd,8c) { ADC(_HX);												} /* ADC  A,HX		  */
OP(dd,8d) { ADC(_LX);												} /* ADC  A,LX		  */
OP(dd,8e) { EAX; ADC(RM(EA));										} /* ADC  A,(IX+o)	  */
OP(dd,8f) { illegal_1();											} /* DB   DD		  */

OP(dd,90) { illegal_1();											} /* DB   DD		  */
OP(dd,91) { illegal_1();											} /* DB   DD		  */
OP(dd,92) { illegal_1();											} /* DB   DD		  */
OP(dd,93) { illegal_1();											} /* DB   DD		  */
OP(dd,94) { SUB(_HX);												} /* SUB  HX		  */
OP(dd,95) { SUB(_LX);												} /* SUB  LX		  */
OP(dd,96) { EAX; SUB(RM(EA));										} /* SUB  (IX+o)	  */
OP(dd,97) { illegal_1();											} /* DB   DD		  */

OP(dd,98) { illegal_1();											} /* DB   DD		  */
OP(dd,99) { illegal_1();											} /* DB   DD		  */
OP(dd,9a) { illegal_1();											} /* DB   DD		  */
OP(dd,9b) { illegal_1();											} /* DB   DD		  */
OP(dd,9c) { SBC(_HX);												} /* SBC  A,HX		  */
OP(dd,9d) { SBC(_LX);												} /* SBC  A,LX		  */
OP(dd,9e) { EAX; SBC(RM(EA));										} /* SBC  A,(IX+o)	  */
OP(dd,9f) { illegal_1();											} /* DB   DD		  */

OP(dd,a0) { illegal_1();											} /* DB   DD		  */
OP(dd,a1) { illegal_1();											} /* DB   DD		  */
OP(dd,a2) { illegal_1();											} /* DB   DD		  */
OP(dd,a3) { illegal_1();											} /* DB   DD		  */
OP(dd,a4) { AND(_HX);												} /* AND  HX		  */
OP(dd,a5) { AND(_LX);												} /* AND  LX		  */
OP(dd,a6) { EAX; AND(RM(EA));										} /* AND  (IX+o)	  */
OP(dd,a7) { illegal_1();											} /* DB   DD		  */

OP(dd,a8) { illegal_1();											} /* DB   DD		  */
OP(dd,a9) { illegal_1();											} /* DB   DD		  */
OP(dd,aa) { illegal_1();											} /* DB   DD		  */
OP(dd,ab) { illegal_1();											} /* DB   DD		  */
OP(dd,ac) { XOR(_HX);												} /* XOR  HX		  */
OP(dd,ad) { XOR(_LX);												} /* XOR  LX		  */
OP(dd,ae) { EAX; XOR(RM(EA));										} /* XOR  (IX+o)	  */
OP(dd,af) { illegal_1();											} /* DB   DD		  */

OP(dd,b0) { illegal_1();											} /* DB   DD		  */
OP(dd,b1) { illegal_1();											} /* DB   DD		  */
OP(dd,b2) { illegal_1();											} /* DB   DD		  */
OP(dd,b3) { illegal_1();											} /* DB   DD		  */
OP(dd,b4) { OR(_HX);												} /* OR   HX		  */
OP(dd,b5) { OR(_LX);												} /* OR   LX		  */
OP(dd,b6) { EAX; OR(RM(EA));										} /* OR   (IX+o)	  */
OP(dd,b7) { illegal_1();											} /* DB   DD		  */

OP(dd,b8) { illegal_1();											} /* DB   DD		  */
OP(dd,b9) { illegal_1();											} /* DB   DD		  */
OP(dd,ba) { illegal_1();											} /* DB   DD		  */
OP(dd,bb) { illegal_1();											} /* DB   DD		  */
OP(dd,bc) { CP(_HX);												} /* CP   HX		  */
OP(dd,bd) { CP(_LX);												} /* CP   LX		  */
OP(dd,be) { EAX; CP(RM(EA));										} /* CP   (IX+o)	  */
OP(dd,bf) { illegal_1();											} /* DB   DD		  */

OP(dd,c0) { illegal_1();											} /* DB   DD		  */
OP(dd,c1) { illegal_1();											} /* DB   DD		  */
OP(dd,c2) { illegal_1();											} /* DB   DD		  */
OP(dd,c3) { illegal_1();											} /* DB   DD		  */
OP(dd,c4) { illegal_1();											} /* DB   DD		  */
OP(dd,c5) { illegal_1();											} /* DB   DD		  */
OP(dd,c6) { illegal_1();											} /* DB   DD		  */
OP(dd,c7) { illegal_1();											}		  /* DB   DD		  */

OP(dd,c8) { illegal_1();											} /* DB   DD		  */
OP(dd,c9) { illegal_1();											} /* DB   DD		  */
OP(dd,ca) { illegal_1();											} /* DB   DD		  */
OP(dd,cb) { EAX; EXEC(xxcb,ARG());									} /* **   DD CB xx	  */
OP(dd,cc) { illegal_1();											} /* DB   DD		  */
OP(dd,cd) { illegal_1();											} /* DB   DD		  */
OP(dd,ce) { illegal_1();											} /* DB   DD		  */
OP(dd,cf) { illegal_1();											} /* DB   DD		  */

OP(dd,d0) { illegal_1();											} /* DB   DD		  */
OP(dd,d1) { illegal_1();											} /* DB   DD		  */
OP(dd,d2) { illegal_1();											} /* DB   DD		  */
OP(dd,d3) { illegal_1();											} /* DB   DD		  */
OP(dd,d4) { illegal_1();											} /* DB   DD		  */
OP(dd,d5) { illegal_1();											} /* DB   DD		  */
OP(dd,d6) { illegal_1();											} /* DB   DD		  */
OP(dd,d7) { illegal_1();											} /* DB   DD		  */

OP(dd,d8) { illegal_1();											} /* DB   DD		  */
OP(dd,d9) { illegal_1();											} /* DB   DD		  */
OP(dd,da) { illegal_1();											} /* DB   DD		  */
OP(dd,db) { illegal_1();											} /* DB   DD		  */
OP(dd,dc) { illegal_1();											} /* DB   DD		  */
OP(dd,dd) { illegal_1();											} /* DB   DD		  */
OP(dd,de) { illegal_1();											} /* DB   DD		  */
OP(dd,df) { illegal_1();											} /* DB   DD		  */

OP(dd,e0) { illegal_1();											} /* DB   DD		  */
OP(dd,e1) { POP(IX);												} /* POP  IX		  */
OP(dd,e2) { illegal_1();											} /* DB   DD		  */
OP(dd,e3) { EXSP(IX);												} /* EX   (SP),IX	  */
OP(dd,e4) { illegal_1();											} /* DB   DD		  */
OP(dd,e5) { PUSH( IX ); 											} /* PUSH IX		  */
OP(dd,e6) { illegal_1();											} /* DB   DD		  */
OP(dd,e7) { illegal_1();											} /* DB   DD		  */

OP(dd,e8) { illegal_1();											} /* DB   DD		  */
OP(dd,e9) { _PC = _IX;                                              } /* JP   (IX)        */
OP(dd,ea) { illegal_1();											} /* DB   DD		  */
OP(dd,eb) { illegal_1();											} /* DB   DD		  */
OP(dd,ec) { illegal_1();											} /* DB   DD		  */
OP(dd,ed) { illegal_1();											} /* DB   DD		  */
OP(dd,ee) { illegal_1();											} /* DB   DD		  */
OP(dd,ef) { illegal_1();											} /* DB   DD		  */

OP(dd,f0) { illegal_1();											} /* DB   DD		  */
OP(dd,f1) { illegal_1();											} /* DB   DD		  */
OP(dd,f2) { illegal_1();											} /* DB   DD		  */
OP(dd,f3) { illegal_1();											} /* DB   DD		  */
OP(dd,f4) { illegal_1();											} /* DB   DD		  */
OP(dd,f5) { illegal_1();											} /* DB   DD		  */
OP(dd,f6) { illegal_1();											} /* DB   DD		  */
OP(dd,f7) { illegal_1();											} /* DB   DD		  */

OP(dd,f8) { illegal_1();											} /* DB   DD		  */
OP(dd,f9) { _SP = _IX;												} /* LD   SP,IX 	  */
OP(dd,fa) { illegal_1();											} /* DB   DD		  */
OP(dd,fb) { illegal_1();											} /* DB   DD		  */
OP(dd,fc) { illegal_1();											} /* DB   DD		  */
OP(dd,fd) { illegal_1();											} /* DB   DD		  */
OP(dd,fe) { illegal_1();											} /* DB   DD		  */
OP(dd,ff) { illegal_1();											} /* DB   DD		  */

/**********************************************************
 * IY register related opcodes (FD prefix)
 **********************************************************/
OP(fd,00) { illegal_1();											} /* DB   FD		  */
OP(fd,01) { illegal_1();											} /* DB   FD		  */
OP(fd,02) { illegal_1();											} /* DB   FD		  */
OP(fd,03) { illegal_1();											} /* DB   FD		  */
OP(fd,04) { illegal_1();											} /* DB   FD		  */
OP(fd,05) { illegal_1();											} /* DB   FD		  */
OP(fd,06) { illegal_1();											} /* DB   FD		  */
OP(fd,07) { illegal_1();											} /* DB   FD		  */

OP(fd,08) { illegal_1();											} /* DB   FD		  */
OP(fd,09) { ADD16(IY,BC);											} /* ADD  IY,BC 	  */
OP(fd,0a) { illegal_1();											} /* DB   FD		  */
OP(fd,0b) { illegal_1();											} /* DB   FD		  */
OP(fd,0c) { illegal_1();											} /* DB   FD		  */
OP(fd,0d) { illegal_1();											} /* DB   FD		  */
OP(fd,0e) { illegal_1();											} /* DB   FD		  */
OP(fd,0f) { illegal_1();											} /* DB   FD		  */

OP(fd,10) { illegal_1();											} /* DB   FD		  */
OP(fd,11) { illegal_1();											} /* DB   FD		  */
OP(fd,12) { illegal_1();											} /* DB   FD		  */
OP(fd,13) { illegal_1();											} /* DB   FD		  */
OP(fd,14) { illegal_1();											} /* DB   FD		  */
OP(fd,15) { illegal_1();											} /* DB   FD		  */
OP(fd,16) { illegal_1();											} /* DB   FD		  */
OP(fd,17) { illegal_1();											} /* DB   FD		  */

OP(fd,18) { illegal_1();											} /* DB   FD		  */
OP(fd,19) { ADD16(IY,DE);											} /* ADD  IY,DE 	  */
OP(fd,1a) { illegal_1();											} /* DB   FD		  */
OP(fd,1b) { illegal_1();											} /* DB   FD		  */
OP(fd,1c) { illegal_1();											} /* DB   FD		  */
OP(fd,1d) { illegal_1();											} /* DB   FD		  */
OP(fd,1e) { illegal_1();											} /* DB   FD		  */
OP(fd,1f) { illegal_1();											} /* DB   FD		  */

OP(fd,20) { illegal_1();											} /* DB   FD		  */
OP(fd,21) { _IY = ARG16();											} /* LD   IY,w		  */
OP(fd,22) { EA = ARG16(); WM16( EA, &Z80.IY );						} /* LD   (w),IY	  */
OP(fd,23) { _IY++;													} /* INC  IY		  */
OP(fd,24) { _HY = INC(_HY); 										} /* INC  HY		  */
OP(fd,25) { _HY = DEC(_HY); 										} /* DEC  HY		  */
OP(fd,26) { _HY = ARG();											} /* LD   HY,n		  */
OP(fd,27) { illegal_1();											} /* DB   FD		  */

OP(fd,28) { illegal_1();											} /* DB   FD		  */
OP(fd,29) { ADD16(IY,IY);											} /* ADD  IY,IY 	  */
OP(fd,2a) { EA = ARG16(); RM16( EA, &Z80.IY );						} /* LD   IY,(w)	  */
OP(fd,2b) { _IY--;													} /* DEC  IY		  */
OP(fd,2c) { _LY = INC(_LY); 										} /* INC  LY		  */
OP(fd,2d) { _LY = DEC(_LY); 										} /* DEC  LY		  */
OP(fd,2e) { _LY = ARG();											} /* LD   LY,n		  */
OP(fd,2f) { illegal_1();											} /* DB   FD		  */

OP(fd,30) { illegal_1();											} /* DB   FD		  */
OP(fd,31) { illegal_1();											} /* DB   FD		  */
OP(fd,32) { illegal_1();											} /* DB   FD		  */
OP(fd,33) { illegal_1();											} /* DB   FD		  */
OP(fd,34) { EAY; WM( EA, INC(RM(EA)) ); 							} /* INC  (IY+o)	  */
OP(fd,35) { EAY; WM( EA, DEC(RM(EA)) ); 							} /* DEC  (IY+o)	  */
OP(fd,36) { EAY; WM( EA, ARG() );									} /* LD   (IY+o),n	  */
OP(fd,37) { illegal_1();											} /* DB   FD		  */

OP(fd,38) { illegal_1();											} /* DB   FD		  */
OP(fd,39) { ADD16(IY,SP);											} /* ADD  IY,SP 	  */
OP(fd,3a) { illegal_1();											} /* DB   FD		  */
OP(fd,3b) { illegal_1();											} /* DB   FD		  */
OP(fd,3c) { illegal_1();											} /* DB   FD		  */
OP(fd,3d) { illegal_1();											} /* DB   FD		  */
OP(fd,3e) { illegal_1();											} /* DB   FD		  */
OP(fd,3f) { illegal_1();											} /* DB   FD		  */

OP(fd,40) { illegal_1();											} /* DB   FD		  */
OP(fd,41) { illegal_1();											} /* DB   FD		  */
OP(fd,42) { illegal_1();											} /* DB   FD		  */
OP(fd,43) { illegal_1();											} /* DB   FD		  */
OP(fd,44) { _B = _HY;												} /* LD   B,HY		  */
OP(fd,45) { _B = _LY;												} /* LD   B,LY		  */
OP(fd,46) { EAY; _B = RM(EA);										} /* LD   B,(IY+o)	  */
OP(fd,47) { illegal_1();											} /* DB   FD		  */

OP(fd,48) { illegal_1();											} /* DB   FD		  */
OP(fd,49) { illegal_1();											} /* DB   FD		  */
OP(fd,4a) { illegal_1();											} /* DB   FD		  */
OP(fd,4b) { illegal_1();											} /* DB   FD		  */
OP(fd,4c) { _C = _HY;												} /* LD   C,HY		  */
OP(fd,4d) { _C = _LY;												} /* LD   C,LY		  */
OP(fd,4e) { EAY; _C = RM(EA);										} /* LD   C,(IY+o)	  */
OP(fd,4f) { illegal_1();											} /* DB   FD		  */

OP(fd,50) { illegal_1();											} /* DB   FD		  */
OP(fd,51) { illegal_1();											} /* DB   FD		  */
OP(fd,52) { illegal_1();											} /* DB   FD		  */
OP(fd,53) { illegal_1();											} /* DB   FD		  */
OP(fd,54) { _D = _HY;												} /* LD   D,HY		  */
OP(fd,55) { _D = _LY;												} /* LD   D,LY		  */
OP(fd,56) { EAY; _D = RM(EA);										} /* LD   D,(IY+o)	  */
OP(fd,57) { illegal_1();											} /* DB   FD		  */

OP(fd,58) { illegal_1();											} /* DB   FD		  */
OP(fd,59) { illegal_1();											} /* DB   FD		  */
OP(fd,5a) { illegal_1();											} /* DB   FD		  */
OP(fd,5b) { illegal_1();											} /* DB   FD		  */
OP(fd,5c) { _E = _HY;												} /* LD   E,HY		  */
OP(fd,5d) { _E = _LY;												} /* LD   E,LY		  */
OP(fd,5e) { EAY; _E = RM(EA);										} /* LD   E,(IY+o)	  */
OP(fd,5f) { illegal_1();											} /* DB   FD		  */

OP(fd,60) { _HY = _B;												} /* LD   HY,B		  */
OP(fd,61) { _HY = _C;												} /* LD   HY,C		  */
OP(fd,62) { _HY = _D;												} /* LD   HY,D		  */
OP(fd,63) { _HY = _E;												} /* LD   HY,E		  */
OP(fd,64) { 														} /* LD   HY,HY 	  */
OP(fd,65) { _HY = _LY;												} /* LD   HY,LY 	  */
OP(fd,66) { EAY; _H = RM(EA);										} /* LD   H,(IY+o)	  */
OP(fd,67) { _HY = _A;												} /* LD   HY,A		  */

OP(fd,68) { _LY = _B;												} /* LD   LY,B		  */
OP(fd,69) { _LY = _C;												} /* LD   LY,C		  */
OP(fd,6a) { _LY = _D;												} /* LD   LY,D		  */
OP(fd,6b) { _LY = _E;												} /* LD   LY,E		  */
OP(fd,6c) { _LY = _HY;												} /* LD   LY,HY 	  */
OP(fd,6d) { 														} /* LD   LY,LY 	  */
OP(fd,6e) { EAY; _L = RM(EA);										} /* LD   L,(IY+o)	  */
OP(fd,6f) { _LY = _A;												} /* LD   LY,A		  */

OP(fd,70) { EAY; WM( EA, _B );										} /* LD   (IY+o),B	  */
OP(fd,71) { EAY; WM( EA, _C );										} /* LD   (IY+o),C	  */
OP(fd,72) { EAY; WM( EA, _D );										} /* LD   (IY+o),D	  */
OP(fd,73) { EAY; WM( EA, _E );										} /* LD   (IY+o),E	  */
OP(fd,74) { EAY; WM( EA, _H );										} /* LD   (IY+o),H	  */
OP(fd,75) { EAY; WM( EA, _L );										} /* LD   (IY+o),L	  */
OP(fd,76) { illegal_1();											}		  /* DB   FD		  */
OP(fd,77) { EAY; WM( EA, _A );										} /* LD   (IY+o),A	  */

OP(fd,78) { illegal_1();											} /* DB   FD		  */
OP(fd,79) { illegal_1();											} /* DB   FD		  */
OP(fd,7a) { illegal_1();											} /* DB   FD		  */
OP(fd,7b) { illegal_1();											} /* DB   FD		  */
OP(fd,7c) { _A = _HY;												} /* LD   A,HY		  */
OP(fd,7d) { _A = _LY;												} /* LD   A,LY		  */
OP(fd,7e) { EAY; _A = RM(EA);										} /* LD   A,(IY+o)	  */
OP(fd,7f) { illegal_1();											} /* DB   FD		  */

OP(fd,80) { illegal_1();											} /* DB   FD		  */
OP(fd,81) { illegal_1();											} /* DB   FD		  */
OP(fd,82) { illegal_1();											} /* DB   FD		  */
OP(fd,83) { illegal_1();											} /* DB   FD		  */
OP(fd,84) { ADD(_HY);												} /* ADD  A,HY		  */
OP(fd,85) { ADD(_LY);												} /* ADD  A,LY		  */
OP(fd,86) { EAY; ADD(RM(EA));										} /* ADD  A,(IY+o)	  */
OP(fd,87) { illegal_1();											} /* DB   FD		  */

OP(fd,88) { illegal_1();											} /* DB   FD		  */
OP(fd,89) { illegal_1();											} /* DB   FD		  */
OP(fd,8a) { illegal_1();											} /* DB   FD		  */
OP(fd,8b) { illegal_1();											} /* DB   FD		  */
OP(fd,8c) { ADC(_HY);												} /* ADC  A,HY		  */
OP(fd,8d) { ADC(_LY);												} /* ADC  A,LY		  */
OP(fd,8e) { EAY; ADC(RM(EA));										} /* ADC  A,(IY+o)	  */
OP(fd,8f) { illegal_1();											} /* DB   FD		  */

OP(fd,90) { illegal_1();											} /* DB   FD		  */
OP(fd,91) { illegal_1();											} /* DB   FD		  */
OP(fd,92) { illegal_1();											} /* DB   FD		  */
OP(fd,93) { illegal_1();											} /* DB   FD		  */
OP(fd,94) { SUB(_HY);												} /* SUB  HY		  */
OP(fd,95) { SUB(_LY);												} /* SUB  LY		  */
OP(fd,96) { EAY; SUB(RM(EA));										} /* SUB  (IY+o)	  */
OP(fd,97) { illegal_1();											} /* DB   FD		  */

OP(fd,98) { illegal_1();											} /* DB   FD		  */
OP(fd,99) { illegal_1();											} /* DB   FD		  */
OP(fd,9a) { illegal_1();											} /* DB   FD		  */
OP(fd,9b) { illegal_1();											} /* DB   FD		  */
OP(fd,9c) { SBC(_HY);												} /* SBC  A,HY		  */
OP(fd,9d) { SBC(_LY);												} /* SBC  A,LY		  */
OP(fd,9e) { EAY; SBC(RM(EA));										} /* SBC  A,(IY+o)	  */
OP(fd,9f) { illegal_1();											} /* DB   FD		  */

OP(fd,a0) { illegal_1();											} /* DB   FD		  */
OP(fd,a1) { illegal_1();											} /* DB   FD		  */
OP(fd,a2) { illegal_1();											} /* DB   FD		  */
OP(fd,a3) { illegal_1();											} /* DB   FD		  */
OP(fd,a4) { AND(_HY);												} /* AND  HY		  */
OP(fd,a5) { AND(_LY);												} /* AND  LY		  */
OP(fd,a6) { EAY; AND(RM(EA));										} /* AND  (IY+o)	  */
OP(fd,a7) { illegal_1();											} /* DB   FD		  */

OP(fd,a8) { illegal_1();											} /* DB   FD		  */
OP(fd,a9) { illegal_1();											} /* DB   FD		  */
OP(fd,aa) { illegal_1();											} /* DB   FD		  */
OP(fd,ab) { illegal_1();											} /* DB   FD		  */
OP(fd,ac) { XOR(_HY);												} /* XOR  HY		  */
OP(fd,ad) { XOR(_LY);												} /* XOR  LY		  */
OP(fd,ae) { EAY; XOR(RM(EA));										} /* XOR  (IY+o)	  */
OP(fd,af) { illegal_1();											} /* DB   FD		  */

OP(fd,b0) { illegal_1();											} /* DB   FD		  */
OP(fd,b1) { illegal_1();											} /* DB   FD		  */
OP(fd,b2) { illegal_1();											} /* DB   FD		  */
OP(fd,b3) { illegal_1();											} /* DB   FD		  */
OP(fd,b4) { OR(_HY);												} /* OR   HY		  */
OP(fd,b5) { OR(_LY);												} /* OR   LY		  */
OP(fd,b6) { EAY; OR(RM(EA));										} /* OR   (IY+o)	  */
OP(fd,b7) { illegal_1();											} /* DB   FD		  */

OP(fd,b8) { illegal_1();											} /* DB   FD		  */
OP(fd,b9) { illegal_1();											} /* DB   FD		  */
OP(fd,ba) { illegal_1();											} /* DB   FD		  */
OP(fd,bb) { illegal_1();											} /* DB   FD		  */
OP(fd,bc) { CP(_HY);												} /* CP   HY		  */
OP(fd,bd) { CP(_LY);												} /* CP   LY		  */
OP(fd,be) { EAY; CP(RM(EA));										} /* CP   (IY+o)	  */
OP(fd,bf) { illegal_1();											} /* DB   FD		  */

OP(fd,c0) { illegal_1();											} /* DB   FD		  */
OP(fd,c1) { illegal_1();											} /* DB   FD		  */
OP(fd,c2) { illegal_1();											} /* DB   FD		  */
OP(fd,c3) { illegal_1();											} /* DB   FD		  */
OP(fd,c4) { illegal_1();											} /* DB   FD		  */
OP(fd,c5) { illegal_1();											} /* DB   FD		  */
OP(fd,c6) { illegal_1();											} /* DB   FD		  */
OP(fd,c7) { illegal_1();											} /* DB   FD		  */

OP(fd,c8) { illegal_1();											} /* DB   FD		  */
OP(fd,c9) { illegal_1();											} /* DB   FD		  */
OP(fd,ca) { illegal_1();											} /* DB   FD		  */
OP(fd,cb) { EAY; EXEC(xxcb,ARG());									} /* **   FD CB xx	  */
OP(fd,cc) { illegal_1();											} /* DB   FD		  */
OP(fd,cd) { illegal_1();											} /* DB   FD		  */
OP(fd,ce) { illegal_1();											} /* DB   FD		  */
OP(fd,cf) { illegal_1();											} /* DB   FD		  */

OP(fd,d0) { illegal_1();											} /* DB   FD		  */
OP(fd,d1) { illegal_1();											} /* DB   FD		  */
OP(fd,d2) { illegal_1();											} /* DB   FD		  */
OP(fd,d3) { illegal_1();											} /* DB   FD		  */
OP(fd,d4) { illegal_1();											} /* DB   FD		  */
OP(fd,d5) { illegal_1();											} /* DB   FD		  */
OP(fd,d6) { illegal_1();											} /* DB   FD		  */
OP(fd,d7) { illegal_1();											} /* DB   FD		  */

OP(fd,d8) { illegal_1();											} /* DB   FD		  */
OP(fd,d9) { illegal_1();											} /* DB   FD		  */
OP(fd,da) { illegal_1();											} /* DB   FD		  */
OP(fd,db) { illegal_1();											} /* DB   FD		  */
OP(fd,dc) { illegal_1();											} /* DB   FD		  */
OP(fd,dd) { illegal_1();											} /* DB   FD		  */
OP(fd,de) { illegal_1();											} /* DB   FD		  */
OP(fd,df) { illegal_1();											} /* DB   FD		  */

OP(fd,e0) { illegal_1();											} /* DB   FD		  */
OP(fd,e1) { POP(IY);												} /* POP  IY		  */
OP(fd,e2) { illegal_1();											} /* DB   FD		  */
OP(fd,e3) { EXSP(IY);												} /* EX   (SP),IY	  */
OP(fd,e4) { illegal_1();											} /* DB   FD		  */
OP(fd,e5) { PUSH( IY ); 											} /* PUSH IY		  */
OP(fd,e6) { illegal_1();											} /* DB   FD		  */
OP(fd,e7) { illegal_1();											} /* DB   FD		  */

OP(fd,e8) { illegal_1();											} /* DB   FD		  */
OP(fd,e9) { _PC = _IY;                                              } /* JP   (IY)        */
OP(fd,ea) { illegal_1();											} /* DB   FD		  */
OP(fd,eb) { illegal_1();											} /* DB   FD		  */
OP(fd,ec) { illegal_1();											} /* DB   FD		  */
OP(fd,ed) { illegal_1();											} /* DB   FD		  */
OP(fd,ee) { illegal_1();											} /* DB   FD		  */
OP(fd,ef) { illegal_1();											} /* DB   FD		  */

OP(fd,f0) { illegal_1();											} /* DB   FD		  */
OP(fd,f1) { illegal_1();											} /* DB   FD		  */
OP(fd,f2) { illegal_1();											} /* DB   FD		  */
OP(fd,f3) { illegal_1();											} /* DB   FD		  */
OP(fd,f4) { illegal_1();											} /* DB   FD		  */
OP(fd,f5) { illegal_1();											} /* DB   FD		  */
OP(fd,f6) { illegal_1();											} /* DB   FD		  */
OP(fd,f7) { illegal_1();											} /* DB   FD		  */

OP(fd,f8) { illegal_1();											} /* DB   FD		  */
OP(fd,f9) { _SP = _IY;												} /* LD   SP,IY 	  */
OP(fd,fa) { illegal_1();											} /* DB   FD		  */
OP(fd,fb) { illegal_1();											} /* DB   FD		  */
OP(fd,fc) { illegal_1();											} /* DB   FD		  */
OP(fd,fd) { illegal_1();											} /* DB   FD		  */
OP(fd,fe) { illegal_1();											} /* DB   FD		  */
OP(fd,ff) { illegal_1();											} /* DB   FD		  */

OP(illegal,2)
{
}

/**********************************************************
 * special opcodes (ED prefix)
 **********************************************************/
OP(ed,00) { illegal_2();											} /* DB   ED		  */
OP(ed,01) { illegal_2();											} /* DB   ED		  */
OP(ed,02) { illegal_2();											} /* DB   ED		  */
OP(ed,03) { illegal_2();											} /* DB   ED		  */
OP(ed,04) { illegal_2();											} /* DB   ED		  */
OP(ed,05) { illegal_2();											} /* DB   ED		  */
OP(ed,06) { illegal_2();											} /* DB   ED		  */
OP(ed,07) { illegal_2();											} /* DB   ED		  */

OP(ed,08) { illegal_2();											} /* DB   ED		  */
OP(ed,09) { illegal_2();											} /* DB   ED		  */
OP(ed,0a) { illegal_2();											} /* DB   ED		  */
OP(ed,0b) { illegal_2();											} /* DB   ED		  */
OP(ed,0c) { illegal_2();											} /* DB   ED		  */
OP(ed,0d) { illegal_2();											} /* DB   ED		  */
OP(ed,0e) { illegal_2();											} /* DB   ED		  */
OP(ed,0f) { illegal_2();											} /* DB   ED		  */

OP(ed,10) { illegal_2();											} /* DB   ED		  */
OP(ed,11) { illegal_2();											} /* DB   ED		  */
OP(ed,12) { illegal_2();											} /* DB   ED		  */
OP(ed,13) { illegal_2();											} /* DB   ED		  */
OP(ed,14) { illegal_2();											} /* DB   ED		  */
OP(ed,15) { illegal_2();											} /* DB   ED		  */
OP(ed,16) { illegal_2();											} /* DB   ED		  */
OP(ed,17) { illegal_2();											} /* DB   ED		  */

OP(ed,18) { illegal_2();											} /* DB   ED		  */
OP(ed,19) { illegal_2();											} /* DB   ED		  */
OP(ed,1a) { illegal_2();											} /* DB   ED		  */
OP(ed,1b) { illegal_2();											} /* DB   ED		  */
OP(ed,1c) { illegal_2();											} /* DB   ED		  */
OP(ed,1d) { illegal_2();											} /* DB   ED		  */
OP(ed,1e) { illegal_2();											} /* DB   ED		  */
OP(ed,1f) { illegal_2();											} /* DB   ED		  */

OP(ed,20) { illegal_2();											} /* DB   ED		  */
OP(ed,21) { illegal_2();											} /* DB   ED		  */
OP(ed,22) { illegal_2();											} /* DB   ED		  */
OP(ed,23) { illegal_2();											} /* DB   ED		  */
OP(ed,24) { illegal_2();											} /* DB   ED		  */
OP(ed,25) { illegal_2();											} /* DB   ED		  */
OP(ed,26) { illegal_2();											} /* DB   ED		  */
OP(ed,27) { illegal_2();											} /* DB   ED		  */

OP(ed,28) { illegal_2();											} /* DB   ED		  */
OP(ed,29) { illegal_2();											} /* DB   ED		  */
OP(ed,2a) { illegal_2();											} /* DB   ED		  */
OP(ed,2b) { illegal_2();											} /* DB   ED		  */
OP(ed,2c) { illegal_2();											} /* DB   ED		  */
OP(ed,2d) { illegal_2();											} /* DB   ED		  */
OP(ed,2e) { illegal_2();											} /* DB   ED		  */
OP(ed,2f) { illegal_2();											} /* DB   ED		  */

OP(ed,30) { illegal_2();											} /* DB   ED		  */
OP(ed,31) { illegal_2();											} /* DB   ED		  */
OP(ed,32) { illegal_2();											} /* DB   ED		  */
OP(ed,33) { illegal_2();											} /* DB   ED		  */
OP(ed,34) { illegal_2();											} /* DB   ED		  */
OP(ed,35) { illegal_2();											} /* DB   ED		  */
OP(ed,36) { illegal_2();											} /* DB   ED		  */
OP(ed,37) { illegal_2();											} /* DB   ED		  */

OP(ed,38) { illegal_2();											} /* DB   ED		  */
OP(ed,39) { illegal_2();											} /* DB   ED		  */
OP(ed,3a) { illegal_2();											} /* DB   ED		  */
OP(ed,3b) { illegal_2();											} /* DB   ED		  */
OP(ed,3c) { illegal_2();											} /* DB   ED		  */
OP(ed,3d) { illegal_2();											} /* DB   ED		  */
OP(ed,3e) { illegal_2();											} /* DB   ED		  */
OP(ed,3f) { illegal_2();											} /* DB   ED		  */

OP(ed,40) { _B = IN(_BC); _F = (_F & CF) | SZP[_B]; 				} /* IN   B,(C) 	  */
OP(ed,41) { OUT(_BC,_B);											} /* OUT  (C),B 	  */
OP(ed,42) { SBC16( BC );											} /* SBC  HL,BC 	  */
OP(ed,43) { EA = ARG16(); WM16( EA, &Z80.BC );						} /* LD   (w),BC	  */
OP(ed,44) { NEG;													} /* NEG			  */
OP(ed,45) { RETN;													} /* RETN;			  */
OP(ed,46) { _IM = 0;												} /* IM   0 		  */
OP(ed,47) { LD_I_A; 												} /* LD   I,A		  */

OP(ed,48) { _C = IN(_BC); _F = (_F & CF) | SZP[_C]; 				} /* IN   C,(C) 	  */
OP(ed,49) { OUT(_BC,_C);											} /* OUT  (C),C 	  */
OP(ed,4a) { ADC16( BC );											} /* ADC  HL,BC 	  */
OP(ed,4b) { EA = ARG16(); RM16( EA, &Z80.BC );						} /* LD   BC,(w)	  */
OP(ed,4c) { NEG;													} /* NEG			  */
OP(ed,4d) { RETI;													} /* RETI			  */
OP(ed,4e) { _IM = 0;												} /* IM   0 		  */
OP(ed,4f) { LD_R_A; 												} /* LD   R,A		  */

OP(ed,50) { _D = IN(_BC); _F = (_F & CF) | SZP[_D]; 				} /* IN   D,(C) 	  */
OP(ed,51) { OUT(_BC,_D);											} /* OUT  (C),D 	  */
OP(ed,52) { SBC16( DE );											} /* SBC  HL,DE 	  */
OP(ed,53) { EA = ARG16(); WM16( EA, &Z80.DE );						} /* LD   (w),DE	  */
OP(ed,54) { NEG;													} /* NEG			  */
OP(ed,55) { RETN;													} /* RETN;			  */
OP(ed,56) { _IM = 1;												} /* IM   1 		  */
OP(ed,57) { LD_A_I; 												} /* LD   A,I		  */

OP(ed,58) { _E = IN(_BC); _F = (_F & CF) | SZP[_E]; 				} /* IN   E,(C) 	  */
OP(ed,59) { OUT(_BC,_E);											} /* OUT  (C),E 	  */
OP(ed,5a) { ADC16( DE );											} /* ADC  HL,DE 	  */
OP(ed,5b) { EA = ARG16(); RM16( EA, &Z80.DE );						} /* LD   DE,(w)	  */
OP(ed,5c) { NEG;													} /* NEG			  */
OP(ed,5d) { RETI;													} /* RETI			  */
OP(ed,5e) { _IM = 2;												} /* IM   2 		  */
OP(ed,5f) { LD_A_R; 												} /* LD   A,R		  */

OP(ed,60) { _H = IN(_BC); _F = (_F & CF) | SZP[_H]; 				} /* IN   H,(C) 	  */
OP(ed,61) { OUT(_BC,_H);											} /* OUT  (C),H 	  */
OP(ed,62) { SBC16( HL );											} /* SBC  HL,HL 	  */
OP(ed,63) { EA = ARG16(); WM16( EA, &Z80.HL );						} /* LD   (w),HL	  */
OP(ed,64) { NEG;													} /* NEG			  */
OP(ed,65) { RETN;													} /* RETN;			  */
OP(ed,66) { _IM = 0;												} /* IM   0 		  */
OP(ed,67) { RRD;													} /* RRD  (HL)		  */

OP(ed,68) { _L = IN(_BC); _F = (_F & CF) | SZP[_L]; 				} /* IN   L,(C) 	  */
OP(ed,69) { OUT(_BC,_L);											} /* OUT  (C),L 	  */
OP(ed,6a) { ADC16( HL );											} /* ADC  HL,HL 	  */
OP(ed,6b) { EA = ARG16(); RM16( EA, &Z80.HL );						} /* LD   HL,(w)	  */
OP(ed,6c) { NEG;													} /* NEG			  */
OP(ed,6d) { RETI;													} /* RETI			  */
OP(ed,6e) { _IM = 0;												} /* IM   0 		  */
OP(ed,6f) { RLD;													} /* RLD  (HL)		  */

OP(ed,70) { UINT8 res = IN(_BC); _F = (_F & CF) | SZP[res]; 		} /* IN   0,(C) 	  */
OP(ed,71) { OUT(_BC,0); 											} /* OUT  (C),0 	  */
OP(ed,72) { SBC16( SP );											} /* SBC  HL,SP 	  */
OP(ed,73) { EA = ARG16(); WM16( EA, &Z80.SP );						} /* LD   (w),SP	  */
OP(ed,74) { NEG;													} /* NEG			  */
OP(ed,75) { RETN;													} /* RETN;			  */
OP(ed,76) { _IM = 1;												} /* IM   1 		  */
OP(ed,77) { illegal_2();											} /* DB   ED,77 	  */

OP(ed,78) { _A = IN(_BC); _F = (_F & CF) | SZP[_A]; 				} /* IN   E,(C) 	  */
OP(ed,79) { OUT(_BC,_A);											} /* OUT  (C),E 	  */
OP(ed,7a) { ADC16( SP );											} /* ADC  HL,SP 	  */
OP(ed,7b) { EA = ARG16(); RM16( EA, &Z80.SP );						} /* LD   SP,(w)	  */
OP(ed,7c) { NEG;													} /* NEG			  */
OP(ed,7d) { RETI;													} /* RETI			  */
OP(ed,7e) { _IM = 2;												} /* IM   2 		  */
OP(ed,7f) { illegal_2();											} /* DB   ED,7F 	  */

OP(ed,80) { illegal_2();											} /* DB   ED		  */
OP(ed,81) { illegal_2();											} /* DB   ED		  */
OP(ed,82) { illegal_2();											} /* DB   ED		  */
OP(ed,83) { illegal_2();											} /* DB   ED		  */
OP(ed,84) { illegal_2();											} /* DB   ED		  */
OP(ed,85) { illegal_2();											} /* DB   ED		  */
OP(ed,86) { illegal_2();											} /* DB   ED		  */
OP(ed,87) { illegal_2();											} /* DB   ED		  */

OP(ed,88) { illegal_2();											} /* DB   ED		  */
OP(ed,89) { illegal_2();											} /* DB   ED		  */
OP(ed,8a) { illegal_2();											} /* DB   ED		  */
OP(ed,8b) { illegal_2();											} /* DB   ED		  */
OP(ed,8c) { illegal_2();											} /* DB   ED		  */
OP(ed,8d) { illegal_2();											} /* DB   ED		  */
OP(ed,8e) { illegal_2();											} /* DB   ED		  */
OP(ed,8f) { illegal_2();											} /* DB   ED		  */

OP(ed,90) { illegal_2();											} /* DB   ED		  */
OP(ed,91) { illegal_2();											} /* DB   ED		  */
OP(ed,92) { illegal_2();											} /* DB   ED		  */
OP(ed,93) { illegal_2();											} /* DB   ED		  */
OP(ed,94) { illegal_2();											} /* DB   ED		  */
OP(ed,95) { illegal_2();											} /* DB   ED		  */
OP(ed,96) { illegal_2();											} /* DB   ED		  */
OP(ed,97) { illegal_2();											} /* DB   ED		  */

OP(ed,98) { illegal_2();											} /* DB   ED		  */
OP(ed,99) { illegal_2();											} /* DB   ED		  */
OP(ed,9a) { illegal_2();											} /* DB   ED		  */
OP(ed,9b) { illegal_2();											} /* DB   ED		  */
OP(ed,9c) { illegal_2();											} /* DB   ED		  */
OP(ed,9d) { illegal_2();											} /* DB   ED		  */
OP(ed,9e) { illegal_2();											} /* DB   ED		  */
OP(ed,9f) { illegal_2();											} /* DB   ED		  */

OP(ed,a0) { LDI;													} /* LDI			  */
OP(ed,a1) { CPI;													} /* CPI			  */
OP(ed,a2) { INI;													} /* INI			  */
OP(ed,a3) { OUTI;													} /* OUTI			  */
OP(ed,a4) { illegal_2();											} /* DB   ED		  */
OP(ed,a5) { illegal_2();											} /* DB   ED		  */
OP(ed,a6) { illegal_2();											} /* DB   ED		  */
OP(ed,a7) { illegal_2();											} /* DB   ED		  */

OP(ed,a8) { LDD;													} /* LDD			  */
OP(ed,a9) { CPD;													} /* CPD			  */
OP(ed,aa) { IND;													} /* IND			  */
OP(ed,ab) { OUTD;													} /* OUTD			  */
OP(ed,ac) { illegal_2();											} /* DB   ED		  */
OP(ed,ad) { illegal_2();											} /* DB   ED		  */
OP(ed,ae) { illegal_2();											} /* DB   ED		  */
OP(ed,af) { illegal_2();											} /* DB   ED		  */

OP(ed,b0) { LDIR;													} /* LDIR			  */
OP(ed,b1) { CPIR;													} /* CPIR			  */
OP(ed,b2) { INIR;													} /* INIR			  */
OP(ed,b3) { OTIR;													} /* OTIR			  */
OP(ed,b4) { illegal_2();											} /* DB   ED		  */
OP(ed,b5) { illegal_2();											} /* DB   ED		  */
OP(ed,b6) { illegal_2();											} /* DB   ED		  */
OP(ed,b7) { illegal_2();											} /* DB   ED		  */

OP(ed,b8) { LDDR;													} /* LDDR			  */
OP(ed,b9) { CPDR;													} /* CPDR			  */
OP(ed,ba) { INDR;													} /* INDR			  */
OP(ed,bb) { OTDR;													} /* OTDR			  */
OP(ed,bc) { illegal_2();											} /* DB   ED		  */
OP(ed,bd) { illegal_2();											} /* DB   ED		  */
OP(ed,be) { illegal_2();											} /* DB   ED		  */
OP(ed,bf) { illegal_2();											} /* DB   ED		  */

OP(ed,c0) { illegal_2();											} /* DB   ED		  */
OP(ed,c1) { illegal_2();											} /* DB   ED		  */
OP(ed,c2) { illegal_2();											} /* DB   ED		  */
OP(ed,c3) { illegal_2();											} /* DB   ED		  */
OP(ed,c4) { illegal_2();											} /* DB   ED		  */
OP(ed,c5) { illegal_2();											} /* DB   ED		  */
OP(ed,c6) { illegal_2();											} /* DB   ED		  */
OP(ed,c7) { illegal_2();											} /* DB   ED		  */

OP(ed,c8) { illegal_2();											} /* DB   ED		  */
OP(ed,c9) { illegal_2();											} /* DB   ED		  */
OP(ed,ca) { illegal_2();											} /* DB   ED		  */
OP(ed,cb) { illegal_2();											} /* DB   ED		  */
OP(ed,cc) { illegal_2();											} /* DB   ED		  */
OP(ed,cd) { illegal_2();											} /* DB   ED		  */
OP(ed,ce) { illegal_2();											} /* DB   ED		  */
OP(ed,cf) { illegal_2();											} /* DB   ED		  */

OP(ed,d0) { illegal_2();											} /* DB   ED		  */
OP(ed,d1) { illegal_2();											} /* DB   ED		  */
OP(ed,d2) { illegal_2();											} /* DB   ED		  */
OP(ed,d3) { illegal_2();											} /* DB   ED		  */
OP(ed,d4) { illegal_2();											} /* DB   ED		  */
OP(ed,d5) { illegal_2();											} /* DB   ED		  */
OP(ed,d6) { illegal_2();											} /* DB   ED		  */
OP(ed,d7) { illegal_2();											} /* DB   ED		  */

OP(ed,d8) { illegal_2();											} /* DB   ED		  */
OP(ed,d9) { illegal_2();											} /* DB   ED		  */
OP(ed,da) { illegal_2();											} /* DB   ED		  */
OP(ed,db) { illegal_2();											} /* DB   ED		  */
OP(ed,dc) { illegal_2();											} /* DB   ED		  */
OP(ed,dd) { illegal_2();											} /* DB   ED		  */
OP(ed,de) { illegal_2();											} /* DB   ED		  */
OP(ed,df) { illegal_2();											} /* DB   ED		  */

OP(ed,e0) { illegal_2();											} /* DB   ED		  */
OP(ed,e1) { illegal_2();											} /* DB   ED		  */
OP(ed,e2) { illegal_2();											} /* DB   ED		  */
OP(ed,e3) { illegal_2();											} /* DB   ED		  */
OP(ed,e4) { illegal_2();											} /* DB   ED		  */
OP(ed,e5) { illegal_2();											} /* DB   ED		  */
OP(ed,e6) { illegal_2();											} /* DB   ED		  */
OP(ed,e7) { illegal_2();											} /* DB   ED		  */

OP(ed,e8) { illegal_2();											} /* DB   ED		  */
OP(ed,e9) { illegal_2();											} /* DB   ED		  */
OP(ed,ea) { illegal_2();											} /* DB   ED		  */
OP(ed,eb) { illegal_2();											} /* DB   ED		  */
OP(ed,ec) { illegal_2();											} /* DB   ED		  */
OP(ed,ed) { illegal_2();											} /* DB   ED		  */
OP(ed,ee) { illegal_2();											} /* DB   ED		  */
OP(ed,ef) { illegal_2();											} /* DB   ED		  */

OP(ed,f0) { illegal_2();											} /* DB   ED		  */
OP(ed,f1) { illegal_2();											} /* DB   ED		  */
OP(ed,f2) { illegal_2();											} /* DB   ED		  */
OP(ed,f3) { illegal_2();											} /* DB   ED		  */
OP(ed,f4) { illegal_2();											} /* DB   ED		  */
OP(ed,f5) { illegal_2();											} /* DB   ED		  */
OP(ed,f6) { illegal_2();											} /* DB   ED		  */
OP(ed,f7) { illegal_2();											} /* DB   ED		  */

OP(ed,f8) { illegal_2();											} /* DB   ED		  */
OP(ed,f9) { illegal_2();											} /* DB   ED		  */
OP(ed,fa) { illegal_2();											} /* DB   ED		  */
OP(ed,fb) { illegal_2();											} /* DB   ED		  */
OP(ed,fc) { illegal_2();											} /* DB   ED		  */
OP(ed,fd) { illegal_2();											} /* DB   ED		  */
OP(ed,fe) { illegal_2();											} /* DB   ED		  */
OP(ed,ff) { illegal_2();											} /* DB   ED		  */

#if TIME_LOOP_HACKS

#define CHECK_BC_LOOP                                               \
if( _BC > 1 && _PCD < 0xfffc ) {									\
	UINT8 op1 = cpu_readop(_PCD);									\
	UINT8 op2 = cpu_readop(_PCD+1); 								\
	if( (op1==0x78 && op2==0xb1) || (op1==0x79 && op2==0xb0) )		\
	{																\
		UINT8 op3 = cpu_readop(_PCD+2); 							\
		UINT8 op4 = cpu_readop(_PCD+3); 							\
		if( op3==0x20 && op4==0xfb )								\
		{															\
			while( _BC > 0 && z80_ICount > 4+4+12+6 )				\
			{														\
				BURNODD( 4+4+12+6, 4, 4+4+12+6 );					\
				_BC--;												\
			}														\
		}															\
		else														\
		if( op3 == 0xc2 )											\
		{															\
			UINT8 ad1 = cpu_readop_arg(_PCD+3); 					\
			UINT8 ad2 = cpu_readop_arg(_PCD+4); 					\
			if( (ad1 + 256 * ad2) == (_PCD - 1) )					\
			{														\
				while( _BC > 0 && z80_ICount > 4+4+10+6 )			\
				{													\
					BURNODD( 4+4+10+6, 4, 4+4+10+6 );				\
					_BC--;											\
				}													\
			}														\
		}															\
	}																\
}

#define CHECK_DE_LOOP                                               \
if( _DE > 1 && _PCD < 0xfffc ) {                                    \
	UINT8 op1 = cpu_readop(_PCD);									\
	UINT8 op2 = cpu_readop(_PCD+1); 								\
	if( (op1==0x7a && op2==0xb3) || (op1==0x7b && op2==0xb2) )		\
	{																\
		UINT8 op3 = cpu_readop(_PCD+2); 							\
		UINT8 op4 = cpu_readop(_PCD+3); 							\
		if( op3==0x20 && op4==0xfb )								\
		{															\
			while( _DE > 0 && z80_ICount > 4+4+12+6 )				\
			{														\
				BURNODD( 4+4+12+6, 4, 4+4+12+6 );					\
				_DE--;												\
			}														\
		}															\
		else														\
		if( op3==0xc2 ) 											\
		{															\
			UINT8 ad1 = cpu_readop_arg(_PCD+3); 					\
			UINT8 ad2 = cpu_readop_arg(_PCD+4); 					\
			if( (ad1 + 256 * ad2) == (_PCD - 1) )					\
			{														\
				while( _DE > 0 && z80_ICount > 4+4+10+6 )			\
				{													\
					BURNODD( 4+4+10+6, 4, 4+4+10+6 );				\
					_DE--;											\
				}													\
			}														\
		}															\
	}																\
}

#define CHECK_HL_LOOP                                               \
if( _HL > 1 && _PCD < 0xfffc ) {                                    \
	UINT8 op1 = cpu_readop(_PCD);									\
	UINT8 op2 = cpu_readop(_PCD+1); 								\
	if( (op1==0x7c && op2==0xb5) || (op1==0x7d && op2==0xb4) )		\
	{																\
		UINT8 op3 = cpu_readop(_PCD+2); 							\
		UINT8 op4 = cpu_readop(_PCD+3); 							\
		if( op3==0x20 && op4==0xfb )								\
		{															\
			while( _HL > 0 && z80_ICount > 4+4+12+6 )				\
			{														\
				BURNODD( 4+4+12+6, 4, 4+4+12+6 );					\
				_HL--;												\
			}														\
		}															\
		else														\
		if( op3==0xc2 ) 											\
		{															\
			UINT8 ad1 = cpu_readop_arg(_PCD+3); 					\
			UINT8 ad2 = cpu_readop_arg(_PCD+4); 					\
			if( (ad1 + 256 * ad2) == (_PCD - 1) )					\
			{														\
				while( _HL > 0 && z80_ICount > 4+4+10+6 )			\
				{													\
					BURNODD( 4+4+10+6, 4, 4+4+10+6 );				\
					_HL--;											\
				}													\
			}														\
		}															\
	}																\
}

#else

#define CHECK_BC_LOOP
#define CHECK_DE_LOOP
#define CHECK_HL_LOOP

#endif

/**********************************************************
 * main opcodes
 **********************************************************/
OP(op,00) { 														} /* NOP			  */
OP(op,01) { _BC = ARG16();											} /* LD   BC,w		  */
OP(op,02) { WM( _BC, _A );											} /* LD   (BC),A	  */
OP(op,03) { _BC++;													} /* INC  BC		  */
OP(op,04) { _B = INC(_B);											} /* INC  B 		  */
OP(op,05) { _B = DEC(_B);											} /* DEC  B 		  */
OP(op,06) { _B = ARG(); 											} /* LD   B,n		  */
OP(op,07) { RLCA;													} /* RLCA			  */

OP(op,08) { EX_AF;													} /* EX   AF,AF'      */
OP(op,09) { ADD16(HL,BC);											} /* ADD  HL,BC 	  */
OP(op,0a) { _A = RM(_BC);											} /* LD   A,(BC)	  */
OP(op,0b) { _BC--; CHECK_BC_LOOP;									} /* DEC  BC		  */
OP(op,0c) { _C = INC(_C);											} /* INC  C 		  */
OP(op,0d) { _C = DEC(_C);											} /* DEC  C 		  */
OP(op,0e) { _C = ARG(); 											} /* LD   C,n		  */
OP(op,0f) { RRCA;													} /* RRCA			  */

OP(op,10) { _B--; JR_COND(_B);										} /* DJNZ o 		  */
OP(op,11) { _DE = ARG16();											} /* LD   DE,w		  */
OP(op,12) { WM( _DE, _A );											} /* LD   (DE),A	  */
OP(op,13) { _DE++;													} /* INC  DE		  */
OP(op,14) { _D = INC(_D);											} /* INC  D 		  */
OP(op,15) { _D = DEC(_D);											} /* DEC  D 		  */
OP(op,16) { _D = ARG(); 											} /* LD   D,n		  */
OP(op,17) { RLA;													} /* RLA			  */

OP(op,18) { JR();													} /* JR   o 		  */
OP(op,19) { ADD16(HL,DE);											} /* ADD  HL,DE 	  */
OP(op,1a) { _A = RM(_DE);											} /* LD   A,(DE)	  */
OP(op,1b) { _DE--; CHECK_DE_LOOP;									} /* DEC  DE		  */
OP(op,1c) { _E = INC(_E);											} /* INC  E 		  */
OP(op,1d) { _E = DEC(_E);											} /* DEC  E 		  */
OP(op,1e) { _E = ARG(); 											} /* LD   E,n		  */
OP(op,1f) { RRA;													} /* RRA			  */

OP(op,20) { JR_COND( !(_F & ZF) );									} /* JR   NZ,o		  */
OP(op,21) { _HL = ARG16();											} /* LD   HL,w		  */
OP(op,22) { EA = ARG16(); WM16( EA, &Z80.HL );						} /* LD   (w),HL	  */
OP(op,23) { _HL++;													} /* INC  HL		  */
OP(op,24) { _H = INC(_H);											} /* INC  H 		  */
OP(op,25) { _H = DEC(_H);											} /* DEC  H 		  */
OP(op,26) { _H = ARG(); 											} /* LD   H,n		  */
OP(op,27) { DAA;													} /* DAA			  */

OP(op,28) { JR_COND( _F & ZF ); 									} /* JR   Z,o		  */
OP(op,29) { ADD16(HL,HL);											} /* ADD  HL,HL 	  */
OP(op,2a) { EA = ARG16(); RM16( EA, &Z80.HL );						} /* LD   HL,(w)	  */
OP(op,2b) { _HL--; CHECK_HL_LOOP;									} /* DEC  HL		  */
OP(op,2c) { _L = INC(_L);											} /* INC  L 		  */
OP(op,2d) { _L = DEC(_L);											} /* DEC  L 		  */
OP(op,2e) { _L = ARG(); 											} /* LD   L,n		  */
OP(op,2f) { _A ^= 0xff; _F = (_F&(SF|ZF|PF|CF))|HF|NF|(_A&(YF|XF)); } /* CPL			  */

OP(op,30) { JR_COND( !(_F & CF) );									} /* JR   NC,o		  */
OP(op,31) { _SP = ARG16();											} /* LD   SP,w		  */
OP(op,32) { EA = ARG16(); WM( EA, _A ); 							} /* LD   (w),A 	  */
OP(op,33) { _SP++;													} /* INC  SP		  */
OP(op,34) { WM( _HL, INC(RM(_HL)) );								} /* INC  (HL)		  */
OP(op,35) { WM( _HL, DEC(RM(_HL)) );								} /* DEC  (HL)		  */
OP(op,36) { WM( _HL, ARG() );										} /* LD   (HL),n	  */
OP(op,37) { _F = (_F & (SF|ZF|PF)) | CF | (_A & (YF|XF));			} /* SCF			  */

OP(op,38) { JR_COND( _F & CF ); 									} /* JR   C,o		  */
OP(op,39) { ADD16(HL,SP);											} /* ADD  HL,SP 	  */
OP(op,3a) { EA = ARG16(); _A = RM( EA );							} /* LD   A,(w) 	  */
OP(op,3b) { _SP--;													} /* DEC  SP		  */
OP(op,3c) { _A = INC(_A);											} /* INC  A 		  */
OP(op,3d) { _A = DEC(_A);											} /* DEC  A 		  */
OP(op,3e) { _A = ARG(); 											} /* LD   A,n		  */
OP(op,3f) { _F = ((_F&(SF|ZF|PF|CF))|((_F&CF)<<4)|(_A&(YF|XF)))^CF; } /* CCF			  */
/*OP(op,3f) { _F = ((_F & ~(HF|NF)) | ((_F & CF)<<4)) ^ CF; 		  } */ /* CCF				*/

OP(op,40) { 														} /* LD   B,B		  */
OP(op,41) { _B = _C;												} /* LD   B,C		  */
OP(op,42) { _B = _D;												} /* LD   B,D		  */
OP(op,43) { _B = _E;												} /* LD   B,E		  */
OP(op,44) { _B = _H;												} /* LD   B,H		  */
OP(op,45) { _B = _L;												} /* LD   B,L		  */
OP(op,46) { _B = RM(_HL);											} /* LD   B,(HL)	  */
OP(op,47) { _B = _A;												} /* LD   B,A		  */

OP(op,48) { _C = _B;												} /* LD   C,B		  */
OP(op,49) { 														} /* LD   C,C		  */
OP(op,4a) { _C = _D;												} /* LD   C,D		  */
OP(op,4b) { _C = _E;												} /* LD   C,E		  */
OP(op,4c) { _C = _H;												} /* LD   C,H		  */
OP(op,4d) { _C = _L;												} /* LD   C,L		  */
OP(op,4e) { _C = RM(_HL);											} /* LD   C,(HL)	  */
OP(op,4f) { _C = _A;												} /* LD   C,A		  */

OP(op,50) { _D = _B;												} /* LD   D,B		  */
OP(op,51) { _D = _C;												} /* LD   D,C		  */
OP(op,52) { 														} /* LD   D,D		  */
OP(op,53) { _D = _E;												} /* LD   D,E		  */
OP(op,54) { _D = _H;												} /* LD   D,H		  */
OP(op,55) { _D = _L;												} /* LD   D,L		  */
OP(op,56) { _D = RM(_HL);											} /* LD   D,(HL)	  */
OP(op,57) { _D = _A;												} /* LD   D,A		  */

OP(op,58) { _E = _B;												} /* LD   E,B		  */
OP(op,59) { _E = _C;												} /* LD   E,C		  */
OP(op,5a) { _E = _D;												} /* LD   E,D		  */
OP(op,5b) { 														} /* LD   E,E		  */
OP(op,5c) { _E = _H;												} /* LD   E,H		  */
OP(op,5d) { _E = _L;												} /* LD   E,L		  */
OP(op,5e) { _E = RM(_HL);											} /* LD   E,(HL)	  */
OP(op,5f) { _E = _A;												} /* LD   E,A		  */

OP(op,60) { _H = _B;												} /* LD   H,B		  */
OP(op,61) { _H = _C;												} /* LD   H,C		  */
OP(op,62) { _H = _D;												} /* LD   H,D		  */
OP(op,63) { _H = _E;												} /* LD   H,E		  */
OP(op,64) { 														} /* LD   H,H		  */
OP(op,65) { _H = _L;												} /* LD   H,L		  */
OP(op,66) { _H = RM(_HL);											} /* LD   H,(HL)	  */
OP(op,67) { _H = _A;												} /* LD   H,A		  */

OP(op,68) { _L = _B;												} /* LD   L,B		  */
OP(op,69) { _L = _C;												} /* LD   L,C		  */
OP(op,6a) { _L = _D;												} /* LD   L,D		  */
OP(op,6b) { _L = _E;												} /* LD   L,E		  */
OP(op,6c) { _L = _H;												} /* LD   L,H		  */
OP(op,6d) { 														} /* LD   L,L		  */
OP(op,6e) { _L = RM(_HL);											} /* LD   L,(HL)	  */
OP(op,6f) { _L = _A;												} /* LD   L,A		  */

OP(op,70) { WM( _HL, _B );											} /* LD   (HL),B	  */
OP(op,71) { WM( _HL, _C );											} /* LD   (HL),C	  */
OP(op,72) { WM( _HL, _D );											} /* LD   (HL),D	  */
OP(op,73) { WM( _HL, _E );											} /* LD   (HL),E	  */
OP(op,74) { WM( _HL, _H );											} /* LD   (HL),H	  */
OP(op,75) { WM( _HL, _L );											} /* LD   (HL),L	  */
OP(op,76) { ENTER_HALT; 											} /* HALT			  */
OP(op,77) { WM( _HL, _A );											} /* LD   (HL),A	  */

OP(op,78) { _A = _B;												} /* LD   A,B		  */
OP(op,79) { _A = _C;												} /* LD   A,C		  */
OP(op,7a) { _A = _D;												} /* LD   A,D		  */
OP(op,7b) { _A = _E;												} /* LD   A,E		  */
OP(op,7c) { _A = _H;												} /* LD   A,H		  */
OP(op,7d) { _A = _L;												} /* LD   A,L		  */
OP(op,7e) { _A = RM(_HL);											} /* LD   A,(HL)	  */
OP(op,7f) { 														} /* LD   A,A		  */

OP(op,80) { ADD(_B);												} /* ADD  A,B		  */
OP(op,81) { ADD(_C);												} /* ADD  A,C		  */
OP(op,82) { ADD(_D);												} /* ADD  A,D		  */
OP(op,83) { ADD(_E);												} /* ADD  A,E		  */
OP(op,84) { ADD(_H);												} /* ADD  A,H		  */
OP(op,85) { ADD(_L);												} /* ADD  A,L		  */
OP(op,86) { ADD(RM(_HL));											} /* ADD  A,(HL)	  */
OP(op,87) { ADD(_A);												} /* ADD  A,A		  */

OP(op,88) { ADC(_B);												} /* ADC  A,B		  */
OP(op,89) { ADC(_C);												} /* ADC  A,C		  */
OP(op,8a) { ADC(_D);												} /* ADC  A,D		  */
OP(op,8b) { ADC(_E);												} /* ADC  A,E		  */
OP(op,8c) { ADC(_H);												} /* ADC  A,H		  */
OP(op,8d) { ADC(_L);												} /* ADC  A,L		  */
OP(op,8e) { ADC(RM(_HL));											} /* ADC  A,(HL)	  */
OP(op,8f) { ADC(_A);												} /* ADC  A,A		  */

OP(op,90) { SUB(_B);												} /* SUB  B 		  */
OP(op,91) { SUB(_C);												} /* SUB  C 		  */
OP(op,92) { SUB(_D);												} /* SUB  D 		  */
OP(op,93) { SUB(_E);												} /* SUB  E 		  */
OP(op,94) { SUB(_H);												} /* SUB  H 		  */
OP(op,95) { SUB(_L);												} /* SUB  L 		  */
OP(op,96) { SUB(RM(_HL));											} /* SUB  (HL)		  */
OP(op,97) { SUB(_A);												} /* SUB  A 		  */

OP(op,98) { SBC(_B);												} /* SBC  A,B		  */
OP(op,99) { SBC(_C);												} /* SBC  A,C		  */
OP(op,9a) { SBC(_D);												} /* SBC  A,D		  */
OP(op,9b) { SBC(_E);												} /* SBC  A,E		  */
OP(op,9c) { SBC(_H);												} /* SBC  A,H		  */
OP(op,9d) { SBC(_L);												} /* SBC  A,L		  */
OP(op,9e) { SBC(RM(_HL));											} /* SBC  A,(HL)	  */
OP(op,9f) { SBC(_A);												} /* SBC  A,A		  */

OP(op,a0) { AND(_B);												} /* AND  B 		  */
OP(op,a1) { AND(_C);												} /* AND  C 		  */
OP(op,a2) { AND(_D);												} /* AND  D 		  */
OP(op,a3) { AND(_E);												} /* AND  E 		  */
OP(op,a4) { AND(_H);												} /* AND  H 		  */
OP(op,a5) { AND(_L);												} /* AND  L 		  */
OP(op,a6) { AND(RM(_HL));											} /* AND  (HL)		  */
OP(op,a7) { AND(_A);												} /* AND  A 		  */

OP(op,a8) { XOR(_B);												} /* XOR  B 		  */
OP(op,a9) { XOR(_C);												} /* XOR  C 		  */
OP(op,aa) { XOR(_D);												} /* XOR  D 		  */
OP(op,ab) { XOR(_E);												} /* XOR  E 		  */
OP(op,ac) { XOR(_H);												} /* XOR  H 		  */
OP(op,ad) { XOR(_L);												} /* XOR  L 		  */
OP(op,ae) { XOR(RM(_HL));											} /* XOR  (HL)		  */
OP(op,af) { XOR(_A);												} /* XOR  A 		  */

OP(op,b0) { OR(_B); 												} /* OR   B 		  */
OP(op,b1) { OR(_C); 												} /* OR   C 		  */
OP(op,b2) { OR(_D); 												} /* OR   D 		  */
OP(op,b3) { OR(_E); 												} /* OR   E 		  */
OP(op,b4) { OR(_H); 												} /* OR   H 		  */
OP(op,b5) { OR(_L); 												} /* OR   L 		  */
OP(op,b6) { OR(RM(_HL));											} /* OR   (HL)		  */
OP(op,b7) { OR(_A); 												} /* OR   A 		  */

OP(op,b8) { CP(_B); 												} /* CP   B 		  */
OP(op,b9) { CP(_C); 												} /* CP   C 		  */
OP(op,ba) { CP(_D); 												} /* CP   D 		  */
OP(op,bb) { CP(_E); 												} /* CP   E 		  */
OP(op,bc) { CP(_H); 												} /* CP   H 		  */
OP(op,bd) { CP(_L); 												} /* CP   L 		  */
OP(op,be) { CP(RM(_HL));											} /* CP   (HL)		  */
OP(op,bf) { CP(_A); 												} /* CP   A 		  */

OP(op,c0) { RET( !(_F & ZF) );										} /* RET  NZ		  */
OP(op,c1) { POP(BC);												} /* POP  BC		  */
OP(op,c2) { JP_COND( !(_F & ZF) );									} /* JP   NZ,a		  */
OP(op,c3) { JP; 													} /* JP   a 		  */
OP(op,c4) { CALL( !(_F & ZF) ); 									} /* CALL NZ,a		  */
OP(op,c5) { PUSH( BC ); 											} /* PUSH BC		  */
OP(op,c6) { ADD(ARG()); 											} /* ADD  A,n		  */
OP(op,c7) { RST(0x00);												} /* RST  0 		  */

OP(op,c8) { RET( _F & ZF ); 										} /* RET  Z 		  */
OP(op,c9) { RET(1); 												} /* RET			  */
OP(op,ca) { JP_COND( _F & ZF ); 									} /* JP   Z,a		  */
OP(op,cb) { _R++; EXEC(cb,ROP());									} /* **** CB xx 	  */
OP(op,cc) { CALL( _F & ZF );										} /* CALL Z,a		  */
OP(op,cd) { CALL(1);												} /* CALL a 		  */
OP(op,ce) { ADC(ARG()); 											} /* ADC  A,n		  */
OP(op,cf) { RST(0x08);												} /* RST  1 		  */

OP(op,d0) { RET( !(_F & CF) );										} /* RET  NC		  */
OP(op,d1) { POP(DE);												} /* POP  DE		  */
OP(op,d2) { JP_COND( !(_F & CF) );									} /* JP   NC,a		  */
OP(op,d3) { unsigned n = ARG() | (_A << 8); OUT( n, _A );			} /* OUT  (n),A 	  */
OP(op,d4) { CALL( !(_F & CF) ); 									} /* CALL NC,a		  */
OP(op,d5) { PUSH( DE ); 											} /* PUSH DE		  */
OP(op,d6) { SUB(ARG()); 											} /* SUB  n 		  */
OP(op,d7) { RST(0x10);												} /* RST  2 		  */

OP(op,d8) { RET( _F & CF ); 										} /* RET  C 		  */
OP(op,d9) { EXX;													} /* EXX			  */
OP(op,da) { JP_COND( _F & CF ); 									} /* JP   C,a		  */
OP(op,db) { unsigned n = ARG() | (_A << 8); _A = IN( n );			} /* IN   A,(n) 	  */
OP(op,dc) { CALL( _F & CF );										} /* CALL C,a		  */
OP(op,dd) { _R++; EXEC(dd,ROP());									} /* **** DD xx 	  */
OP(op,de) { SBC(ARG()); 											} /* SBC  A,n		  */
OP(op,df) { RST(0x18);												} /* RST  3 		  */

OP(op,e0) { RET( !(_F & PF) );										} /* RET  PO		  */
OP(op,e1) { POP(HL);												} /* POP  HL		  */
OP(op,e2) { JP_COND( !(_F & PF) );									} /* JP   PO,a		  */
OP(op,e3) { EXSP(HL);												} /* EX   HL,(SP)	  */
OP(op,e4) { CALL( !(_F & PF) ); 									} /* CALL PO,a		  */
OP(op,e5) { PUSH( HL ); 											} /* PUSH HL		  */
OP(op,e6) { AND(ARG()); 											} /* AND  n 		  */
OP(op,e7) { RST(0x20);												} /* RST  4 		  */

OP(op,e8) { RET( _F & PF ); 										} /* RET  PE		  */
OP(op,e9) { _PC = _HL;                                              } /* JP   (HL)        */
OP(op,ea) { JP_COND( _F & PF ); 									} /* JP   PE,a		  */
OP(op,eb) { EX_DE_HL;												} /* EX   DE,HL 	  */
OP(op,ec) { CALL( _F & PF );										} /* CALL PE,a		  */
OP(op,ed) { _R++; EXEC(ed,ROP());									} /* **** ED xx 	  */
OP(op,ee) { XOR(ARG()); 											} /* XOR  n 		  */
OP(op,ef) { RST(0x28);												} /* RST  5 		  */

OP(op,f0) { RET( !(_F & SF) );										} /* RET  P 		  */
OP(op,f1) { POP(AF);												} /* POP  AF		  */
OP(op,f2) { JP_COND( !(_F & SF) );									} /* JP   P,a		  */
OP(op,f3) { _IFF1 = _IFF2 = 0;										} /* DI 			  */
OP(op,f4) { CALL( !(_F & SF) ); 									} /* CALL P,a		  */
OP(op,f5) { PUSH( AF ); 											} /* PUSH AF		  */
OP(op,f6) { OR(ARG());												} /* OR   n 		  */
OP(op,f7) { RST(0x30);												} /* RST  6 		  */

OP(op,f8) { RET(_F & SF);											} /* RET  M 		  */
OP(op,f9) { _SP = _HL;												} /* LD   SP,HL 	  */
OP(op,fa) { JP_COND(_F & SF);										} /* JP   M,a		  */
OP(op,fb) { EI; 													} /* EI 			  */
OP(op,fc) { CALL(_F & SF);											} /* CALL M,a		  */
OP(op,fd) { _R++; EXEC(fd,ROP());									} /* **** FD xx 	  */
OP(op,fe) { CP(ARG());												} /* CP   n 		  */
OP(op,ff) { RST(0x38);												} /* RST  7 		  */


static void take_interrupt(void)
{
    if( _IFF1 )
    {
        int irq_vector;

        /* there isn't a valid previous program counter */
        _PPC = -1;

        /* Check if processor was halted */
		LEAVE_HALT;

        if( Z80.irq_max )           /* daisy chain mode */
        {
            if( Z80.request_irq >= 0 )
            {
                /* Clear both interrupt flip flops */
                _IFF1 = _IFF2 = 0;
                irq_vector = Z80.irq[Z80.request_irq].interrupt_entry(Z80.irq[Z80.request_irq].irq_param);
                Z80.request_irq = -1;
            } else return;
        }
        else
        {
            /* Clear both interrupt flip flops */
            _IFF1 = _IFF2 = 0;
            /* call back the cpu interface to retrieve the vector */
            irq_vector = (*Z80.irq_callback)(0);
        }

        /* Interrupt mode 2. Call [Z80.I:databyte] */
        if( _IM == 2 )
        {
			irq_vector = (irq_vector & 0xff) | (_I << 8);
            PUSH( PC );
			RM16( irq_vector, &Z80.PC );
            Z80.extra_cycles += 19;
        }
        else
        /* Interrupt mode 1. RST 38h */
        if( _IM == 1 )
        {
            PUSH( PC );
            _PCD = 0x0038;
            Z80.extra_cycles += 11+2; /* RST $38 + 2 cycles */
        }
        else
        {
            /* Interrupt mode 0. We check for CALL and JP instructions, */
            /* if neither of these were found we assume a 1 byte opcode */
            /* was placed on the databus                                */
            switch (irq_vector & 0xff0000)
            {
                case 0xcd0000:  /* call */
                    PUSH( PC );
                    Z80.extra_cycles += 5;  /* CALL $xxxx cycles (JP $xxxx follows)*/
                case 0xc30000:  /* jump */
                    _PCD = irq_vector & 0xffff;
                    Z80.extra_cycles += 10 + 2; /* JP $xxxx + 2 cycles */
                    break;
                default:        /* rst */
                    PUSH( PC );
                    _PCD = irq_vector & 0x0038;
                    Z80.extra_cycles += 11 + 2; /* RST $xx + 2 cycles */
                    break;
            }
        }
    }
}

/****************************************************************************
 * Reset registers to their initial values
 ****************************************************************************/
void z80_reset(void *param)
{
	Z80_DaisyChain *daisy_chain = (Z80_DaisyChain *)param;
	int i, p;
#if BIG_FLAGS_ARRAY
	if( !SZHVC_add || !SZHVC_sub )
    {
		int oldval, newval, val;
		UINT8 *padd, *padc, *psub, *psbc;
        /* allocate big flag arrays once */
		SZHVC_add = (UINT8 *)malloc(2*256*256);
		SZHVC_sub = (UINT8 *)malloc(2*256*256);
		if( !SZHVC_add || !SZHVC_sub )
		{
			raise(SIGABRT);
		}
		padd = &SZHVC_add[	0*256];
		padc = &SZHVC_add[256*256];
		psub = &SZHVC_sub[	0*256];
		psbc = &SZHVC_sub[256*256];
		for (oldval = 0; oldval < 256; oldval++)
		{
			for (newval = 0; newval < 256; newval++)
			{
				/* add or adc w/o carry set */
				val = newval - oldval;
				*padd = (newval) ? ((newval & 0x80) ? SF : 0) : ZF;
#if Z80_EXACT
				*padd |= (newval & (YF | XF));	/* undocumented flag bits 5+3 */
#endif
                if( (newval & 0x0f) < (oldval & 0x0f) ) *padd |= HF;
				if( newval < oldval ) *padd |= CF;
				if( (val^oldval^0x80) & (val^newval) & 0x80 ) *padd |= VF;
				padd++;

				/* adc with carry set */
				val = newval - oldval - 1;
				*padc = (newval) ? ((newval & 0x80) ? SF : 0) : ZF;
#if Z80_EXACT
				*padc |= (newval & (YF | XF));	/* undocumented flag bits 5+3 */
#endif
                if( (newval & 0x0f) <= (oldval & 0x0f) ) *padc |= HF;
				if( newval <= oldval ) *padc |= CF;
				if( (val^oldval^0x80) & (val^newval) & 0x80 ) *padc |= VF;
				padc++;

				/* cp, sub or sbc w/o carry set */
				val = oldval - newval;
				*psub = NF | ((newval) ? ((newval & 0x80) ? SF : 0) : ZF);
#if Z80_EXACT
				*psub |= (newval & (YF | XF));	/* undocumented flag bits 5+3 */
#endif
                if( (newval & 0x0f) > (oldval & 0x0f) ) *psub |= HF;
				if( newval > oldval ) *psub |= CF;
				if( (val^oldval) & (oldval^newval) & 0x80 ) *psub |= VF;
				psub++;

				/* sbc with carry set */
				val = oldval - newval - 1;
				*psbc = NF | ((newval) ? ((newval & 0x80) ? SF : 0) : ZF);
#if Z80_EXACT
				*psbc |= (newval & (YF | XF));	/* undocumented flag bits 5+3 */
#endif
                if( (newval & 0x0f) >= (oldval & 0x0f) ) *psbc |= HF;
				if( newval >= oldval ) *psbc |= CF;
				if( (val^oldval) & (oldval^newval) & 0x80 ) *psbc |= VF;
				psbc++;
			}
        }
    }
#endif
	for (i = 0; i < 256; i++)
	{
		p = 0;
		if( i&0x01 ) ++p;
		if( i&0x02 ) ++p;
		if( i&0x04 ) ++p;
		if( i&0x08 ) ++p;
		if( i&0x10 ) ++p;
		if( i&0x20 ) ++p;
		if( i&0x40 ) ++p;
		if( i&0x80 ) ++p;
		SZ[i] = i ? i & SF : ZF;
#if Z80_EXACT
		SZ[i] |= (i & (YF | XF));		/* undocumented flag bits 5+3 */
#endif
		SZ_BIT[i] = i ? i & SF : ZF | PF;
#if Z80_EXACT
		SZ_BIT[i] |= (i & (YF | XF));	/* undocumented flag bits 5+3 */
#endif
        SZP[i] = SZ[i] | ((p & 1) ? 0 : PF);
		SZHV_inc[i] = SZ[i];
		if( i == 0x80 ) SZHV_inc[i] |= VF;
		if( (i & 0x0f) == 0x00 ) SZHV_inc[i] |= HF;
		SZHV_dec[i] = SZ[i] | NF;
		if( i == 0x7f ) SZHV_dec[i] |= VF;
		if( (i & 0x0f) == 0x0f ) SZHV_dec[i] |= HF;
	}

	memset(&Z80, 0, sizeof(Z80));
	_IX = _IY = 0xffff; /* IX and IY are FFFF after a reset! */
	_F = ZF;			/* Zero flag is set */
	Z80.request_irq = -1;
	Z80.service_irq = -1;
    Z80.nmi_state = CLEAR_LINE;
	Z80.irq_state = CLEAR_LINE;

    if( daisy_chain )
	{
		while( daisy_chain->irq_param != -1 && Z80.irq_max < Z80_MAXDAISY )
		{
            /* set callbackhandler after reti */
			Z80.irq[Z80.irq_max] = *daisy_chain;
            /* device reset */
			if( Z80.irq[Z80.irq_max].reset )
				Z80.irq[Z80.irq_max].reset(Z80.irq[Z80.irq_max].irq_param);
			Z80.irq_max++;
            daisy_chain++;
        }
    }
}

void z80_exit(void)
{
#if BIG_FLAGS_ARRAY
	if (SZHVC_add) free(SZHVC_add);
    SZHVC_add = 0;
	if (SZHVC_sub) free(SZHVC_sub);
    SZHVC_sub = 0;
#endif
}

/****************************************************************************
 * Execute 'cycles' T-states. Return number of T-states really executed
 ****************************************************************************/
int z80_execute(int cycles)
{
	z80_ICount = cycles - Z80.extra_cycles;
	Z80.extra_cycles = 0;

    do
	{
        _PPC = _PCD;
		_R++;
        EXEC_INLINE(op,ROP());
	} while( z80_ICount > 0 );

	z80_ICount -= Z80.extra_cycles;
    Z80.extra_cycles = 0;

    return cycles - z80_ICount;
}

/****************************************************************************
 * Burn 'cycles' T-states. Adjust R register for the lost time
 ****************************************************************************/
void z80_burn(int cycles)
{
	if( cycles > 0 )
	{
		/* NOP takes 4 cycles per instruction */
		int n = (cycles + 3) / 4;
		_R += n;
		z80_ICount -= 4 * n;
	}
}

/****************************************************************************
 * Get all registers in given buffer
 ****************************************************************************/
unsigned z80_get_context (void *dst)
{
	if( dst )
	    *(Z80_Regs*)dst = Z80;
	return sizeof(Z80_Regs);
}

/****************************************************************************
 * Set all registers to given values
 ****************************************************************************/
void z80_set_context (void *src)
{
	if( src )
		Z80 = *(Z80_Regs*)src;
}

/****************************************************************************
 * Return program counter
 ****************************************************************************/
unsigned z80_get_pc (void)
{
    return _PCD;
}

/****************************************************************************
 * Set program counter
 ****************************************************************************/
void z80_set_pc (unsigned val)
{
	_PC = val;
}

/****************************************************************************
 * Return stack pointer
 ****************************************************************************/
unsigned z80_get_sp (void)
{
	return _SPD;
}

/****************************************************************************
 * Set stack pointer
 ****************************************************************************/
void z80_set_sp (unsigned val)
{
	_SP = val;
}

/****************************************************************************
 * Return a specific register
 ****************************************************************************/
unsigned z80_get_reg (int regnum)
{
	switch( regnum )
	{
		case Z80_PC: return Z80.PC.w.l;
		case Z80_SP: return Z80.SP.w.l;
		case Z80_AF: return Z80.AF.w.l;
		case Z80_BC: return Z80.BC.w.l;
		case Z80_DE: return Z80.DE.w.l;
		case Z80_HL: return Z80.HL.w.l;
		case Z80_IX: return Z80.IX.w.l;
		case Z80_IY: return Z80.IY.w.l;
        case Z80_R: return (Z80.R & 0x7f) | (Z80.R2 & 0x80);
		case Z80_I: return Z80.I;
		case Z80_AF2: return Z80.AF2.w.l;
		case Z80_BC2: return Z80.BC2.w.l;
		case Z80_DE2: return Z80.DE2.w.l;
		case Z80_HL2: return Z80.HL2.w.l;
		case Z80_IM: return Z80.IM;
		case Z80_IFF1: return Z80.IFF1;
		case Z80_IFF2: return Z80.IFF2;
		case Z80_HALT: return Z80.HALT;
		case Z80_NMI_STATE: return Z80.nmi_state;
		case Z80_IRQ_STATE: return Z80.irq_state;
		case Z80_DC0: return Z80.int_state[0];
		case Z80_DC1: return Z80.int_state[1];
		case Z80_DC2: return Z80.int_state[2];
		case Z80_DC3: return Z80.int_state[3];
        case REG_PREVIOUSPC: return Z80.PREPC.w.l;
		default:
			if( regnum <= REG_SP_CONTENTS )
			{
				unsigned offset = _SPD + 2 * (REG_SP_CONTENTS - regnum);
				if( offset < 0xffff )
					return RM( offset ) | ( RM( offset + 1) << 8 );
			}
	}
    return 0;
}

/****************************************************************************
 * Set a specific register
 ****************************************************************************/
void z80_set_reg (int regnum, unsigned val)
{
	switch( regnum )
	{
		case Z80_PC: Z80.PC.w.l = val; break;
		case Z80_SP: Z80.SP.w.l = val; break;
		case Z80_AF: Z80.AF.w.l = val; break;
		case Z80_BC: Z80.BC.w.l = val; break;
		case Z80_DE: Z80.DE.w.l = val; break;
		case Z80_HL: Z80.HL.w.l = val; break;
		case Z80_IX: Z80.IX.w.l = val; break;
		case Z80_IY: Z80.IY.w.l = val; break;
        case Z80_R: Z80.R = val; Z80.R2 = val & 0x80; break;
		case Z80_I: Z80.I = val; break;
		case Z80_AF2: Z80.AF2.w.l = val; break;
		case Z80_BC2: Z80.BC2.w.l = val; break;
		case Z80_DE2: Z80.DE2.w.l = val; break;
		case Z80_HL2: Z80.HL2.w.l = val; break;
		case Z80_IM: Z80.IM = val; break;
		case Z80_IFF1: Z80.IFF1 = val; break;
		case Z80_IFF2: Z80.IFF2 = val; break;
		case Z80_HALT: Z80.HALT = val; break;
		case Z80_NMI_STATE: z80_set_nmi_line(val); break;
		case Z80_IRQ_STATE: z80_set_irq_line(0,val); break;
		case Z80_DC0: Z80.int_state[0] = val; break;
		case Z80_DC1: Z80.int_state[1] = val; break;
		case Z80_DC2: Z80.int_state[2] = val; break;
		case Z80_DC3: Z80.int_state[3] = val; break;
        default:
			if( regnum <= REG_SP_CONTENTS )
			{
				unsigned offset = _SPD + 2 * (REG_SP_CONTENTS - regnum);
				if( offset < 0xffff )
				{
					WM( offset, val & 0xff );
					WM( offset+1, (val >> 8) & 0xff );
				}
			}
    }
}

/****************************************************************************
 * Set NMI line state
 ****************************************************************************/
void z80_set_nmi_line(int state)
{
	if( Z80.nmi_state == state ) return;

    Z80.nmi_state = state;
	if( state == CLEAR_LINE ) return;

	_PPC = -1;			/* there isn't a valid previous program counter */
	LEAVE_HALT; 		/* Check if processor was halted */

	_IFF1 = 0;
    PUSH( PC );
	_PCD = 0x0066;
	Z80.extra_cycles += 11;
}

/****************************************************************************
 * Set IRQ line state
 ****************************************************************************/
void z80_set_irq_line(int irqline, int state)
{
    Z80.irq_state = state;
	if( state == CLEAR_LINE ) return;

	if( Z80.irq_max )
	{
		int daisychain, device, int_state;
		daisychain = (*Z80.irq_callback)(irqline);
		device = daisychain >> 8;
		int_state = daisychain & 0xff;

		if( Z80.int_state[device] != int_state )
		{
			/* set new interrupt status */
            Z80.int_state[device] = int_state;
			/* check interrupt status */
			Z80.request_irq = Z80.service_irq = -1;

            /* search higher IRQ or IEO */
			for( device = 0 ; device < Z80.irq_max ; device ++ )
			{
				/* IEO = disable ? */
				if( Z80.int_state[device] & Z80_INT_IEO )
				{
					Z80.request_irq = -1;		/* if IEO is disable , masking lower IRQ */
					Z80.service_irq = device;	/* set highest interrupt service device */
				}
				/* IRQ = request ? */
				if( Z80.int_state[device] & Z80_INT_REQ )
					Z80.request_irq = device;
			}
			if( Z80.request_irq < 0 ) return;
		}
		else
		{
			return;
		}
	}
	take_interrupt();
}

/****************************************************************************
 * Set IRQ vector callback
 ****************************************************************************/
void z80_set_irq_callback(int (*callback)(int))
{
    Z80.irq_callback = callback;
}
