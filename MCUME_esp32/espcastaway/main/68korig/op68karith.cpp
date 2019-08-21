/*
* Castaway
*  (C) 1994 - 2002 Martin Doering, Joachim Hoenig
*
* $File$ - 68k arithmetic instructions
*
* This file is distributed under the GPL, version 2 or at your
* option any later version.  See doc/license.txt for details.
*
* revision history
*  23.05.2002  JH  FAST1.0.1 code import: KR -> ANSI, restructuring
*  12.06.2002  JH  Correct bus error/address error exception stack frame
*  13.06.2002  JH  Merged in Martin's BCD implementation (untested),
*                  completed jump table. STOP is now the only
*                  unimplemented instruction.
*  08.10.2002  JH  Fixed Z-Flag for ADD.B 0x80+0x80 and Add.W 0x8000+0x8000.
*/
#ifndef PROTOH
static char     sccsid[] = "$Id: op68karith.c,v 1.5 2002/10/08 00:18:02 jhoenig Exp $";
#include "68000.h"
#include "op68k.h"

#include<stdio.h>

/*
* Opfuncs.
*/

#define DoCmpB(target,source) \
{register int32 tgt,cvnz=0; register int8 tgt8;\
    tgt = target - source;\
    if ((uint32) source > (uint32) target) cvnz+=Cflag;\
    tgt8=tgt;\
	if (tgt!=tgt8) cvnz+=Vflag;\
    if (tgt8==0) cvnz+=Zflag;\
	else if (tgt8<0) cvnz+=Nflag;\
	ForceCVNZ(cvnz);\
};

#define DoCmpW(target,source) \
{register int32 tgt,cvnz=0; register int16 tgt16;\
    tgt = target - source;\
    if ((uint32) source > (uint32) target) cvnz+=Cflag;\
	tgt16=tgt;\
    if (tgt!=tgt16) cvnz+=Vflag;\
    if (tgt16==0) cvnz+=Zflag;\
	else if (tgt16<0) cvnz+=Nflag;\
	ForceCVNZ(cvnz);\
};

#define DoCmpL(target,source) \
    /* Overflow precondition: source has different sign */\
{register int32 precon=(target>>31)-(source>>31),cvnz=0;\
    if ((uint32) source > (uint32) target) cvnz+=Cflag;\
    target -= source;\
    /* target and source now have same sign: overflow! */\
    if (precon && (target>>31)==(source>>31)) cvnz+=Vflag;\
	if (target==0) cvnz+=Zflag;\
	else if (target<0) cvnz+=Nflag;\
	ForceCVNZ(cvnz);\
};

#define DoCmpa(target,source) DoCmpL(target,source)
#define DoCmpm(target,source) DoCmpL(target,source)


#define DoDivs(target,source) \
    if (source == 0) {\
    ExceptionGroup2(DIVZ);\
    } else {\
	int16  quot, rest;\
    if ((target / source) > 32767 || (target / source) < -32768) {\
	ForceV (1);\
	ForceC (0);\
    } else {\
	rest = target % source;\
	quot = target / source;\
	target = (rest << 16) | (uint16) quot;\
	ClrCVSetNZ ((int16) target);\
}\
}

#define DoDivu(target,source) \
    if (source == 0) {\
    ExceptionGroup2(DIVZ);\
    } else {\
	uint16  quot, rest;\
    if (((uint32) target / (uint16) source) & 0xffff0000) {\
	ForceV (1);\
	ForceC (0);\
    } else {\
	rest = (uint32) target % (uint16) source;\
	quot = (uint32) target / (uint16) source;\
	target = (rest << 16) | quot;\
	ClrCVSetNZ ((int16) target);\
}\
}

#define DoMuls(target,source) \
    target = ((int16) target * source);\
ClrCVSetNZ (target);

#define DoMulu(target,source) \
    target = ((uint16) target * (uint16) source);\
ClrCVSetNZ (target);

//#define DoSuba(target,source) target -= source;

/*
#define DoSubx(target,source) \
// Overflow precondition: source has different sign \
SetV ((target < 0) != (source < 0));\
if (GetX ()) {\
SetXC ((uint32) source >= (uint32) target);\
target -= source;\
target -= 1;\
} else {\
SetXC ((uint32) source > (uint32) target);\
target -= source;\
}\
//target and source now have same sign: overflow! 
SetV (GetV() && (target<0) == (source<0));\
SetNZ (target);
*/

/* BCD all totally untested (MAD) */

#define DoNBCD(target, source) \
{   register uint32 lo = (target & 0xF) + GetX();\
	register uint32 hi = (target & 0xF0);\
	if (lo>10) {lo=20-lo; hi+=0x20;}\
	else if (lo>0) {lo=10-lo; hi+=0x10;}\
	if (hi>0xa0) {hi=0x140-hi; ForceX(1); ForceC(1);}\
	else if (hi>0) {hi=0xa0-hi; ForceX(1); ForceC(1);}\
	else {ForceC(0); ForceX(0);}\
	target=hi+lo;\
	if (target!=0) ForceZ(0);\
};

#define DoABCD(target, source) \
    { register int32 tgt; \
    tgt = (source & 0xF) + (target & 0xF) + GetX(); \
    if(tgt >= 0xA) \
	tgt += 0x6; \
    tgt += (source & 0xF0) + (target & 0xF0); \
    if(tgt >= 0xA0) \
	tgt += 0x60; \
    SetXC (tgt & 0xFF00); \
    tgt &= 0xFF; \
    SetZ(tgt == 0); \
    target = tgt; \
    };

#define DoSBCD(target, source) \
    { register int32 tgt, hi; \
    tgt = (target & 0xF) - (source & 0xF) - GetX(); \
    hi = (target & 0xF0) - (source & 0xF0); \
    if(tgt < 0) { \
	tgt += 10; \
	hi -= 0x10; \
    } \
    if(hi < 0) { \
	hi -= 0x60; \
    } \
    tgt += hi; \
    SetXC (tgt & 0xFF00); \
    tgt &= 0xFF; \
    SetZ(tgt == 0); \
    target = tgt; \
    };

#endif

Oper (Opc0c0, DoMulu, DW, SdW, ins7, DL, DR, Cd, 0, GRL, SRL, 72)
Oper (Opc0d0, DoMulu, DW, SainW, ins7, DL, DR, Cd, 0, GRL, SRL, 76)
Oper (Opc0d8, DoMulu, DW, SaipW, ins7, DL, DR, Cd, 0, GRL, SRL, 76)
Oper (Opc0e0, DoMulu, DW, SmaiW, ins7, DL, DR, Cd, 0, GRL, SRL, 76)
Oper (Opc0e8, DoMulu, DW, SdaiW, ins7, DL, DR, Cd, 0, GRL, SRL, 80)
Oper (Opc0f0, DoMulu, DW, SaixW, ins7, DL, DR, Cd, 0, GRL, SRL, 80)
Oper (Opc0f8, DoMulu, DW, SearW, ins7, DL, DR, Cd, 0, GRL, SRL, (cycles+77)&0xfffffffc)

Oper (Opc1c0, DoMuls, DW, SdW, ins7, DL, DR, Cd, 0, GRL, SRL, 72)
Oper (Opc1d0, DoMuls, DW, SainW, ins7, DL, DR, Cd, 0, GRL, SRL, 76)
Oper (Opc1d8, DoMuls, DW, SaipW, ins7, DL, DR, Cd, 0, GRL, SRL, 76)
Oper (Opc1e0, DoMuls, DW, SmaiW, ins7, DL, DR, Cd, 0, GRL, SRL, 76)
Oper (Opc1e8, DoMuls, DW, SdaiW, ins7, DL, DR, Cd, 0, GRL, SRL, 80)
Oper (Opc1f0, DoMuls, DW, SaixW, ins7, DL, DR, Cd, 0, GRL, SRL, 80)
Oper (Opc1f8, DoMuls, DW, SearW, ins7, DL, DR, Cd, 0, GRL, SRL, (cycles+77)&0xfffffffc)

Oper (Opc2c0, DoMulu, DW, SdW, ins7, DL, DR, Cd, 1, GRL, SRL, 72)
Oper (Opc2d0, DoMulu, DW, SainW, ins7, DL, DR, Cd, 1, GRL, SRL, 76)
Oper (Opc2d8, DoMulu, DW, SaipW, ins7, DL, DR, Cd, 1, GRL, SRL, 76)
Oper (Opc2e0, DoMulu, DW, SmaiW, ins7, DL, DR, Cd, 1, GRL, SRL, 76)
Oper (Opc2e8, DoMulu, DW, SdaiW, ins7, DL, DR, Cd, 1, GRL, SRL, 80)
Oper (Opc2f0, DoMulu, DW, SaixW, ins7, DL, DR, Cd, 1, GRL, SRL, 80)
Oper (Opc2f8, DoMulu, DW, SearW, ins7, DL, DR, Cd, 1, GRL, SRL, (cycles+77)&0xfffffffc)

Oper (Opc3c0, DoMuls, DW, SdW, ins7, DL, DR, Cd, 1, GRL, SRL, 72)
Oper (Opc3d0, DoMuls, DW, SainW, ins7, DL, DR, Cd, 1, GRL, SRL, 76)
Oper (Opc3d8, DoMuls, DW, SaipW, ins7, DL, DR, Cd, 1, GRL, SRL, 76)
Oper (Opc3e0, DoMuls, DW, SmaiW, ins7, DL, DR, Cd, 1, GRL, SRL, 76)
Oper (Opc3e8, DoMuls, DW, SdaiW, ins7, DL, DR, Cd, 1, GRL, SRL, 80)
Oper (Opc3f0, DoMuls, DW, SaixW, ins7, DL, DR, Cd, 1, GRL, SRL, 80)
Oper (Opc3f8, DoMuls, DW, SearW, ins7, DL, DR, Cd, 1, GRL, SRL, (cycles+77)&0xfffffffc)

Oper (Opc4c0, DoMulu, DW, SdW, ins7, DL, DR, Cd, 2, GRL, SRL, 72)
Oper (Opc4d0, DoMulu, DW, SainW, ins7, DL, DR, Cd, 2, GRL, SRL, 76)
Oper (Opc4d8, DoMulu, DW, SaipW, ins7, DL, DR, Cd, 2, GRL, SRL, 76)
Oper (Opc4e0, DoMulu, DW, SmaiW, ins7, DL, DR, Cd, 2, GRL, SRL, 76)
Oper (Opc4e8, DoMulu, DW, SdaiW, ins7, DL, DR, Cd, 2, GRL, SRL, 80)
Oper (Opc4f0, DoMulu, DW, SaixW, ins7, DL, DR, Cd, 2, GRL, SRL, 80)
Oper (Opc4f8, DoMulu, DW, SearW, ins7, DL, DR, Cd, 2, GRL, SRL, (cycles+77)&0xfffffffc)

Oper (Opc5c0, DoMuls, DW, SdW, ins7, DL, DR, Cd, 2, GRL, SRL, 72)
Oper (Opc5d0, DoMuls, DW, SainW, ins7, DL, DR, Cd, 2, GRL, SRL, 76)
Oper (Opc5d8, DoMuls, DW, SaipW, ins7, DL, DR, Cd, 2, GRL, SRL, 76)
Oper (Opc5e0, DoMuls, DW, SmaiW, ins7, DL, DR, Cd, 2, GRL, SRL, 76)
Oper (Opc5e8, DoMuls, DW, SdaiW, ins7, DL, DR, Cd, 2, GRL, SRL, 80)
Oper (Opc5f0, DoMuls, DW, SaixW, ins7, DL, DR, Cd, 2, GRL, SRL, 80)
Oper (Opc5f8, DoMuls, DW, SearW, ins7, DL, DR, Cd, 2, GRL, SRL, (cycles+77)&0xfffffffc)

Oper (Opc6c0, DoMulu, DW, SdW, ins7, DL, DR, Cd, 3, GRL, SRL, 72)
Oper (Opc6d0, DoMulu, DW, SainW, ins7, DL, DR, Cd, 3, GRL, SRL, 76)
Oper (Opc6d8, DoMulu, DW, SaipW, ins7, DL, DR, Cd, 3, GRL, SRL, 76)
Oper (Opc6e0, DoMulu, DW, SmaiW, ins7, DL, DR, Cd, 3, GRL, SRL, 76)
Oper (Opc6e8, DoMulu, DW, SdaiW, ins7, DL, DR, Cd, 3, GRL, SRL, 80)
Oper (Opc6f0, DoMulu, DW, SaixW, ins7, DL, DR, Cd, 3, GRL, SRL, 80)
Oper (Opc6f8, DoMulu, DW, SearW, ins7, DL, DR, Cd, 3, GRL, SRL, (cycles+77)&0xfffffffc)

Oper (Opc7c0, DoMuls, DW, SdW, ins7, DL, DR, Cd, 3, GRL, SRL, 72)
Oper (Opc7d0, DoMuls, DW, SainW, ins7, DL, DR, Cd, 3, GRL, SRL, 76)
Oper (Opc7d8, DoMuls, DW, SaipW, ins7, DL, DR, Cd, 3, GRL, SRL, 76)
Oper (Opc7e0, DoMuls, DW, SmaiW, ins7, DL, DR, Cd, 3, GRL, SRL, 76)
Oper (Opc7e8, DoMuls, DW, SdaiW, ins7, DL, DR, Cd, 3, GRL, SRL, 80)
Oper (Opc7f0, DoMuls, DW, SaixW, ins7, DL, DR, Cd, 3, GRL, SRL, 80)
Oper (Opc7f8, DoMuls, DW, SearW, ins7, DL, DR, Cd, 3, GRL, SRL, (cycles+77)&0xfffffffc)

Oper (Opc8c0, DoMulu, DW, SdW, ins7, DL, DR, Cd, 4, GRL, SRL, 72)
Oper (Opc8d0, DoMulu, DW, SainW, ins7, DL, DR, Cd, 4, GRL, SRL, 76)
Oper (Opc8d8, DoMulu, DW, SaipW, ins7, DL, DR, Cd, 4, GRL, SRL, 76)
Oper (Opc8e0, DoMulu, DW, SmaiW, ins7, DL, DR, Cd, 4, GRL, SRL, 76)
Oper (Opc8e8, DoMulu, DW, SdaiW, ins7, DL, DR, Cd, 4, GRL, SRL, 80)
Oper (Opc8f0, DoMulu, DW, SaixW, ins7, DL, DR, Cd, 4, GRL, SRL, 80)
Oper (Opc8f8, DoMulu, DW, SearW, ins7, DL, DR, Cd, 4, GRL, SRL, (cycles+77)&0xfffffffc)

Oper (Opc9c0, DoMuls, DW, SdW, ins7, DL, DR, Cd, 4, GRL, SRL, 72)
Oper (Opc9d0, DoMuls, DW, SainW, ins7, DL, DR, Cd, 4, GRL, SRL, 76)
Oper (Opc9d8, DoMuls, DW, SaipW, ins7, DL, DR, Cd, 4, GRL, SRL, 76)
Oper (Opc9e0, DoMuls, DW, SmaiW, ins7, DL, DR, Cd, 4, GRL, SRL, 76)
Oper (Opc9e8, DoMuls, DW, SdaiW, ins7, DL, DR, Cd, 4, GRL, SRL, 80)
Oper (Opc9f0, DoMuls, DW, SaixW, ins7, DL, DR, Cd, 4, GRL, SRL, 80)
Oper (Opc9f8, DoMuls, DW, SearW, ins7, DL, DR, Cd, 4, GRL, SRL, (cycles+77)&0xfffffffc)

Oper (Opcac0, DoMulu, DW, SdW, ins7, DL, DR, Cd, 5, GRL, SRL, 72)
Oper (Opcad0, DoMulu, DW, SainW, ins7, DL, DR, Cd, 5, GRL, SRL, 76)
Oper (Opcad8, DoMulu, DW, SaipW, ins7, DL, DR, Cd, 5, GRL, SRL, 76)
Oper (Opcae0, DoMulu, DW, SmaiW, ins7, DL, DR, Cd, 5, GRL, SRL, 76)
Oper (Opcae8, DoMulu, DW, SdaiW, ins7, DL, DR, Cd, 5, GRL, SRL, 80)
Oper (Opcaf0, DoMulu, DW, SaixW, ins7, DL, DR, Cd, 5, GRL, SRL, 80)
Oper (Opcaf8, DoMulu, DW, SearW, ins7, DL, DR, Cd, 5, GRL, SRL, (cycles+77)&0xfffffffc)

Oper (Opcbc0, DoMuls, DW, SdW, ins7, DL, DR, Cd, 5, GRL, SRL, 72)
Oper (Opcbd0, DoMuls, DW, SainW, ins7, DL, DR, Cd, 5, GRL, SRL, 76)
Oper (Opcbd8, DoMuls, DW, SaipW, ins7, DL, DR, Cd, 5, GRL, SRL, 76)
Oper (Opcbe0, DoMuls, DW, SmaiW, ins7, DL, DR, Cd, 5, GRL, SRL, 76)
Oper (Opcbe8, DoMuls, DW, SdaiW, ins7, DL, DR, Cd, 5, GRL, SRL, 80)
Oper (Opcbf0, DoMuls, DW, SaixW, ins7, DL, DR, Cd, 5, GRL, SRL, 80)
Oper (Opcbf8, DoMuls, DW, SearW, ins7, DL, DR, Cd, 5, GRL, SRL, (cycles+77)&0xfffffffc)

Oper (Opccc0, DoMulu, DW, SdW, ins7, DL, DR, Cd, 6, GRL, SRL, 72)
Oper (Opccd0, DoMulu, DW, SainW, ins7, DL, DR, Cd, 6, GRL, SRL, 76)
Oper (Opccd8, DoMulu, DW, SaipW, ins7, DL, DR, Cd, 6, GRL, SRL, 76)
Oper (Opcce0, DoMulu, DW, SmaiW, ins7, DL, DR, Cd, 6, GRL, SRL, 76)
Oper (Opcce8, DoMulu, DW, SdaiW, ins7, DL, DR, Cd, 6, GRL, SRL, 80)
Oper (Opccf0, DoMulu, DW, SaixW, ins7, DL, DR, Cd, 6, GRL, SRL, 80)
Oper (Opccf8, DoMulu, DW, SearW, ins7, DL, DR, Cd, 6, GRL, SRL, (cycles+77)&0xfffffffc)

Oper (Opcdc0, DoMuls, DW, SdW, ins7, DL, DR, Cd, 6, GRL, SRL, 72)
Oper (Opcdd0, DoMuls, DW, SainW, ins7, DL, DR, Cd, 6, GRL, SRL, 76)
Oper (Opcdd8, DoMuls, DW, SaipW, ins7, DL, DR, Cd, 6, GRL, SRL, 76)
Oper (Opcde0, DoMuls, DW, SmaiW, ins7, DL, DR, Cd, 6, GRL, SRL, 76)
Oper (Opcde8, DoMuls, DW, SdaiW, ins7, DL, DR, Cd, 6, GRL, SRL, 80)
Oper (Opcdf0, DoMuls, DW, SaixW, ins7, DL, DR, Cd, 6, GRL, SRL, 80)
Oper (Opcdf8, DoMuls, DW, SearW, ins7, DL, DR, Cd, 6, GRL, SRL, (cycles+77)&0xfffffffc)

Oper (Opcec0, DoMulu, DW, SdW, ins7, DL, DR, Cd, 7, GRL, SRL, 72)
Oper (Opced0, DoMulu, DW, SainW, ins7, DL, DR, Cd, 7, GRL, SRL, 76)
Oper (Opced8, DoMulu, DW, SaipW, ins7, DL, DR, Cd, 7, GRL, SRL, 76)
Oper (Opcee0, DoMulu, DW, SmaiW, ins7, DL, DR, Cd, 7, GRL, SRL, 76)
Oper (Opcee8, DoMulu, DW, SdaiW, ins7, DL, DR, Cd, 7, GRL, SRL, 80)
Oper (Opcef0, DoMulu, DW, SaixW, ins7, DL, DR, Cd, 7, GRL, SRL, 80)
Oper (Opcef8, DoMulu, DW, SearW, ins7, DL, DR, Cd, 7, GRL, SRL, (cycles+77)&0xfffffffc)

Oper (Opcfc0, DoMuls, DW, SdW, ins7, DL, DR, Cd, 7, GRL, SRL, 72)
Oper (Opcfd0, DoMuls, DW, SainW, ins7, DL, DR, Cd, 7, GRL, SRL, 76)
Oper (Opcfd8, DoMuls, DW, SaipW, ins7, DL, DR, Cd, 7, GRL, SRL, 76)
Oper (Opcfe0, DoMuls, DW, SmaiW, ins7, DL, DR, Cd, 7, GRL, SRL, 76)
Oper (Opcfe8, DoMuls, DW, SdaiW, ins7, DL, DR, Cd, 7, GRL, SRL, 80)
Oper (Opcff0, DoMuls, DW, SaixW, ins7, DL, DR, Cd, 7, GRL, SRL, 80)
Oper (Opcff8, DoMuls, DW, SearW, ins7, DL, DR, Cd, 7, GRL, SRL, (cycles+77)&0xfffffffc)

Oper (Opb000, DoCmpB, DB, SdB, ins7, DB, DR, Cd, 0, GRB, SN, 4)
Oper (Opb010, DoCmpB, DB, SainB, ins7, DB, DR, Cd, 0, GRB, SN, 8)
Oper (Opb018, DoCmpB, DB, SaipB, ins7, DB, DR, Cd, 0, GRB, SN, 8)
Oper (Opb020, DoCmpB, DB, SmaiB, ins7, DB, DR, Cd, 0, GRB, SN, 12)
Oper (Opb028, DoCmpB, DB, SdaiB, ins7, DB, DR, Cd, 0, GRB, SN, 12)
Oper (Opb030, DoCmpB, DB, SaixB, ins7, DB, DR, Cd, 0, GRB, SN, 16)
Oper (Opb038, DoCmpB, DB, SearB, ins7, DB, DR, Cd, 0, GRB, SN, (cycles+11)&0xfffffffc)
Oper (Opb040, DoCmpW, DW, SdW, ins7, DW, DR, Cd, 0, GRW, SN, 4)
Oper (Opb048, DoCmpW, DW, SaW, ins7, DW, DR, Cd, 0, GRW, SN, 4)
Oper (Opb050, DoCmpW, DW, SainW, ins7, DW, DR, Cd, 0, GRW, SN, 8)
Oper (Opb058, DoCmpW, DW, SaipW, ins7, DW, DR, Cd, 0, GRW, SN, 8)
Oper (Opb060, DoCmpW, DW, SmaiW, ins7, DW, DR, Cd, 0, GRW, SN, 12)
Oper (Opb068, DoCmpW, DW, SdaiW, ins7, DW, DR, Cd, 0, GRW, SN, 12)
Oper (Opb070, DoCmpW, DW, SaixW, ins7, DW, DR, Cd, 0, GRW, SN, 16)
Oper (Opb078, DoCmpW, DW, SearW, ins7, DW, DR, Cd, 0, GRW, SN, (cycles+11)&0xfffffffc)
Oper (Opb080, DoCmpL, DL, SdL, ins7, DL, DR, Cd, 0, GRL, SN, 8)
Oper (Opb088, DoCmpL, DL, SaL, ins7, DL, DR, Cd, 0, GRL, SN, 8)
Oper (Opb090, DoCmpL, DL, SainL, ins7, DL, DR, Cd, 0, GRL, SN, 16)
Oper (Opb098, DoCmpL, DL, SaipL, ins7, DL, DR, Cd, 0, GRL, SN, 16)
Oper (Opb0a0, DoCmpL, DL, SmaiL, ins7, DL, DR, Cd, 0, GRL, SN, 16)
Oper (Opb0a8, DoCmpL, DL, SdaiL, ins7, DL, DR, Cd, 0, GRL, SN, 20)
Oper (Opb0b0, DoCmpL, DL, SaixL, ins7, DL, DR, Cd, 0, GRL, SN, 20)
Oper (Opb0b8, DoCmpL, DL, SearL, ins7, DL, DR, Cd, 0, GRL, SN, (cycles+17)&0xfffffffc)
Oper (Opb0c0, DoCmpa, DL, SdW, ins7, DL, DR, Ca, 0, GRL, SN, 8)
Oper (Opb0c8, DoCmpa, DL, SaW, ins7, DL, DR, Ca, 0, GRL, SN, 8)
Oper (Opb0d0, DoCmpa, DL, SainW, ins7, DL, DR, Ca, 0, GRL, SN, 12)
Oper (Opb0d8, DoCmpa, DL, SaipW, ins7, DL, DR, Ca, 0, GRL, SN, 12)
Oper (Opb0e0, DoCmpa, DL, SmaiW, ins7, DL, DR, Ca, 0, GRL, SN, 12)
Oper (Opb0e8, DoCmpa, DL, SdaiW, ins7, DL, DR, Ca, 0, GRL, SN, 16)
Oper (Opb0f0, DoCmpa, DL, SaixW, ins7, DL, DR, Ca, 0, GRL, SN, 16)
Oper (Opb0f8, DoCmpa, DL, SearW, ins7, DL, DR, Ca, 0, GRL, SN, (cycles+13)&0xfffffffc)

Oper (Opb108, DoCmpm, DB, SaipB, ins7, DB, DA, CaipB, 0, GMB, SN, 12)
Oper (Opb148, DoCmpm, DW, SaipW, ins7, DW, DA, CaipW, 0, GMW, SN, 12)
Oper (Opb188, DoCmpm, DL, SaipL, ins7, DL, DA, CaipL, 0, GML, SN, 20)
Oper (Opb1c0, DoCmpa, DL, SdL, ins7, DL, DR, Ca, 0, GRL, SN, 8)
Oper (Opb1c8, DoCmpa, DL, SaL, ins7, DL, DR, Ca, 0, GRL, SN, 8)
Oper (Opb1d0, DoCmpa, DL, SainL, ins7, DL, DR, Ca, 0, GRL, SN, 16)
Oper (Opb1d8, DoCmpa, DL, SaipL, ins7, DL, DR, Ca, 0, GRL, SN, 16)
Oper (Opb1e0, DoCmpa, DL, SmaiL, ins7, DL, DR, Ca, 0, GRL, SN, 16)
Oper (Opb1e8, DoCmpa, DL, SdaiL, ins7, DL, DR, Ca, 0, GRL, SN, 20)
Oper (Opb1f0, DoCmpa, DL, SaixL, ins7, DL, DR, Ca, 0, GRL, SN, 20)
Oper (Opb1f8, DoCmpa, DL, SearL, ins7, DL, DR, Ca, 0, GRL, SN, (cycles+17)&0xfffffffc)

Oper (Opb200, DoCmpB, DB, SdB, ins7, DB, DR, Cd, 1, GRB, SN, 4)
Oper (Opb210, DoCmpB, DB, SainB, ins7, DB, DR, Cd, 1, GRB, SN, 8)
Oper (Opb218, DoCmpB, DB, SaipB, ins7, DB, DR, Cd, 1, GRB, SN, 8)
Oper (Opb220, DoCmpB, DB, SmaiB, ins7, DB, DR, Cd, 1, GRB, SN, 12)
Oper (Opb228, DoCmpB, DB, SdaiB, ins7, DB, DR, Cd, 1, GRB, SN, 12)
Oper (Opb230, DoCmpB, DB, SaixB, ins7, DB, DR, Cd, 1, GRB, SN, 16)
Oper (Opb238, DoCmpB, DB, SearB, ins7, DB, DR, Cd, 1, GRB, SN, (cycles+11)&0xfffffffc)
Oper (Opb240, DoCmpW, DW, SdW, ins7, DW, DR, Cd, 1, GRW, SN, 4)
Oper (Opb248, DoCmpW, DW, SaW, ins7, DW, DR, Cd, 1, GRW, SN, 4)
Oper (Opb250, DoCmpW, DW, SainW, ins7, DW, DR, Cd, 1, GRW, SN, 8)
Oper (Opb258, DoCmpW, DW, SaipW, ins7, DW, DR, Cd, 1, GRW, SN, 8)
Oper (Opb260, DoCmpW, DW, SmaiW, ins7, DW, DR, Cd, 1, GRW, SN, 12)
Oper (Opb268, DoCmpW, DW, SdaiW, ins7, DW, DR, Cd, 1, GRW, SN, 12)
Oper (Opb270, DoCmpW, DW, SaixW, ins7, DW, DR, Cd, 1, GRW, SN, 16)
Oper (Opb278, DoCmpW, DW, SearW, ins7, DW, DR, Cd, 1, GRW, SN, (cycles+11)&0xfffffffc)
Oper (Opb280, DoCmpL, DL, SdL, ins7, DL, DR, Cd, 1, GRL, SN, 8)
Oper (Opb288, DoCmpL, DL, SaL, ins7, DL, DR, Cd, 1, GRL, SN, 8)
Oper (Opb290, DoCmpL, DL, SainL, ins7, DL, DR, Cd, 1, GRL, SN, 16)
Oper (Opb298, DoCmpL, DL, SaipL, ins7, DL, DR, Cd, 1, GRL, SN, 16)
Oper (Opb2a0, DoCmpL, DL, SmaiL, ins7, DL, DR, Cd, 1, GRL, SN, 16)
Oper (Opb2a8, DoCmpL, DL, SdaiL, ins7, DL, DR, Cd, 1, GRL, SN, 20)
Oper (Opb2b0, DoCmpL, DL, SaixL, ins7, DL, DR, Cd, 1, GRL, SN, 20)
Oper (Opb2b8, DoCmpL, DL, SearL, ins7, DL, DR, Cd, 1, GRL, SN, (cycles+17)&0xfffffffc)
Oper (Opb2c0, DoCmpa, DL, SdW, ins7, DL, DR, Ca, 1, GRL, SN, 8)
Oper (Opb2c8, DoCmpa, DL, SaW, ins7, DL, DR, Ca, 1, GRL, SN, 8)
Oper (Opb2d0, DoCmpa, DL, SainW, ins7, DL, DR, Ca, 1, GRL, SN, 12)
Oper (Opb2d8, DoCmpa, DL, SaipW, ins7, DL, DR, Ca, 1, GRL, SN, 12)
Oper (Opb2e0, DoCmpa, DL, SmaiW, ins7, DL, DR, Ca, 1, GRL, SN, 12)
Oper (Opb2e8, DoCmpa, DL, SdaiW, ins7, DL, DR, Ca, 1, GRL, SN, 16)
Oper (Opb2f0, DoCmpa, DL, SaixW, ins7, DL, DR, Ca, 1, GRL, SN, 16)
Oper (Opb2f8, DoCmpa, DL, SearW, ins7, DL, DR, Ca, 1, GRL, SN, (cycles+13)&0xfffffffc)

Oper (Opb308, DoCmpm, DB, SaipB, ins7, DB, DA, CaipB, 1, GMB, SN, 12)
Oper (Opb348, DoCmpm, DW, SaipW, ins7, DW, DA, CaipW, 1, GMW, SN, 12)
Oper (Opb388, DoCmpm, DL, SaipL, ins7, DL, DA, CaipL, 1, GML, SN, 20)
Oper (Opb3c0, DoCmpa, DL, SdL, ins7, DL, DR, Ca, 1, GRL, SN, 8)
Oper (Opb3c8, DoCmpa, DL, SaL, ins7, DL, DR, Ca, 1, GRL, SN, 8)
Oper (Opb3d0, DoCmpa, DL, SainL, ins7, DL, DR, Ca, 1, GRL, SN, 16)
Oper (Opb3d8, DoCmpa, DL, SaipL, ins7, DL, DR, Ca, 1, GRL, SN, 16)
Oper (Opb3e0, DoCmpa, DL, SmaiL, ins7, DL, DR, Ca, 1, GRL, SN, 16)
Oper (Opb3e8, DoCmpa, DL, SdaiL, ins7, DL, DR, Ca, 1, GRL, SN, 20)
Oper (Opb3f0, DoCmpa, DL, SaixL, ins7, DL, DR, Ca, 1, GRL, SN, 20)
Oper (Opb3f8, DoCmpa, DL, SearL, ins7, DL, DR, Ca, 1, GRL, SN, (cycles+17)&0xfffffffc)

Oper (Opb400, DoCmpB, DB, SdB, ins7, DB, DR, Cd, 2, GRB, SN, 4)
Oper (Opb410, DoCmpB, DB, SainB, ins7, DB, DR, Cd, 2, GRB, SN, 8)
Oper (Opb418, DoCmpB, DB, SaipB, ins7, DB, DR, Cd, 2, GRB, SN, 8)
Oper (Opb420, DoCmpB, DB, SmaiB, ins7, DB, DR, Cd, 2, GRB, SN, 12)
Oper (Opb428, DoCmpB, DB, SdaiB, ins7, DB, DR, Cd, 2, GRB, SN, 12)
Oper (Opb430, DoCmpB, DB, SaixB, ins7, DB, DR, Cd, 2, GRB, SN, 16)
Oper (Opb438, DoCmpB, DB, SearB, ins7, DB, DR, Cd, 2, GRB, SN, (cycles+11)&0xfffffffc)
Oper (Opb440, DoCmpW, DW, SdW, ins7, DW, DR, Cd, 2, GRW, SN, 4)
Oper (Opb448, DoCmpW, DW, SaW, ins7, DW, DR, Cd, 2, GRW, SN, 4)
Oper (Opb450, DoCmpW, DW, SainW, ins7, DW, DR, Cd, 2, GRW, SN, 8)
Oper (Opb458, DoCmpW, DW, SaipW, ins7, DW, DR, Cd, 2, GRW, SN, 8)
Oper (Opb460, DoCmpW, DW, SmaiW, ins7, DW, DR, Cd, 2, GRW, SN, 12)
Oper (Opb468, DoCmpW, DW, SdaiW, ins7, DW, DR, Cd, 2, GRW, SN, 12)
Oper (Opb470, DoCmpW, DW, SaixW, ins7, DW, DR, Cd, 2, GRW, SN, 16)
Oper (Opb478, DoCmpW, DW, SearW, ins7, DW, DR, Cd, 2, GRW, SN, (cycles+11)&0xfffffffc)
Oper (Opb480, DoCmpL, DL, SdL, ins7, DL, DR, Cd, 2, GRL, SN, 8)
Oper (Opb488, DoCmpL, DL, SaL, ins7, DL, DR, Cd, 2, GRL, SN, 8)
Oper (Opb490, DoCmpL, DL, SainL, ins7, DL, DR, Cd, 2, GRL, SN, 16)
Oper (Opb498, DoCmpL, DL, SaipL, ins7, DL, DR, Cd, 2, GRL, SN, 16)
Oper (Opb4a0, DoCmpL, DL, SmaiL, ins7, DL, DR, Cd, 2, GRL, SN, 16)
Oper (Opb4a8, DoCmpL, DL, SdaiL, ins7, DL, DR, Cd, 2, GRL, SN, 20)
Oper (Opb4b0, DoCmpL, DL, SaixL, ins7, DL, DR, Cd, 2, GRL, SN, 20)
Oper (Opb4b8, DoCmpL, DL, SearL, ins7, DL, DR, Cd, 2, GRL, SN, (cycles+17)&0xfffffffc)
Oper (Opb4c0, DoCmpa, DL, SdW, ins7, DL, DR, Ca, 2, GRL, SN, 8)
Oper (Opb4c8, DoCmpa, DL, SaW, ins7, DL, DR, Ca, 2, GRL, SN, 8)
Oper (Opb4d0, DoCmpa, DL, SainW, ins7, DL, DR, Ca, 2, GRL, SN, 12)
Oper (Opb4d8, DoCmpa, DL, SaipW, ins7, DL, DR, Ca, 2, GRL, SN, 12)
Oper (Opb4e0, DoCmpa, DL, SmaiW, ins7, DL, DR, Ca, 2, GRL, SN, 12)
Oper (Opb4e8, DoCmpa, DL, SdaiW, ins7, DL, DR, Ca, 2, GRL, SN, 16)
Oper (Opb4f0, DoCmpa, DL, SaixW, ins7, DL, DR, Ca, 2, GRL, SN, 16)
Oper (Opb4f8, DoCmpa, DL, SearW, ins7, DL, DR, Ca, 2, GRL, SN, (cycles+13)&0xfffffffc)

Oper (Opb508, DoCmpm, DB, SaipB, ins7, DB, DA, CaipB, 2, GMB, SN, 12)
Oper (Opb548, DoCmpm, DW, SaipW, ins7, DW, DA, CaipW, 2, GMW, SN, 12)
Oper (Opb588, DoCmpm, DL, SaipL, ins7, DL, DA, CaipL, 2, GML, SN, 20)
Oper (Opb5c0, DoCmpa, DL, SdL, ins7, DL, DR, Ca, 2, GRL, SN, 8)
Oper (Opb5c8, DoCmpa, DL, SaL, ins7, DL, DR, Ca, 2, GRL, SN, 8)
Oper (Opb5d0, DoCmpa, DL, SainL, ins7, DL, DR, Ca, 2, GRL, SN, 16)
Oper (Opb5d8, DoCmpa, DL, SaipL, ins7, DL, DR, Ca, 2, GRL, SN, 16)
Oper (Opb5e0, DoCmpa, DL, SmaiL, ins7, DL, DR, Ca, 2, GRL, SN, 16)
Oper (Opb5e8, DoCmpa, DL, SdaiL, ins7, DL, DR, Ca, 2, GRL, SN, 20)
Oper (Opb5f0, DoCmpa, DL, SaixL, ins7, DL, DR, Ca, 2, GRL, SN, 20)
Oper (Opb5f8, DoCmpa, DL, SearL, ins7, DL, DR, Ca, 2, GRL, SN, (cycles+17)&0xfffffffc)

Oper (Opb600, DoCmpB, DB, SdB, ins7, DB, DR, Cd, 3, GRB, SN, 4)
Oper (Opb610, DoCmpB, DB, SainB, ins7, DB, DR, Cd, 3, GRB, SN, 8)
Oper (Opb618, DoCmpB, DB, SaipB, ins7, DB, DR, Cd, 3, GRB, SN, 8)
Oper (Opb620, DoCmpB, DB, SmaiB, ins7, DB, DR, Cd, 3, GRB, SN, 12)
Oper (Opb628, DoCmpB, DB, SdaiB, ins7, DB, DR, Cd, 3, GRB, SN, 12)
Oper (Opb630, DoCmpB, DB, SaixB, ins7, DB, DR, Cd, 3, GRB, SN, 16)
Oper (Opb638, DoCmpB, DB, SearB, ins7, DB, DR, Cd, 3, GRB, SN, (cycles+11)&0xfffffffc)
Oper (Opb640, DoCmpW, DW, SdW, ins7, DW, DR, Cd, 3, GRW, SN, 4)
Oper (Opb648, DoCmpW, DW, SaW, ins7, DW, DR, Cd, 3, GRW, SN, 4)
Oper (Opb650, DoCmpW, DW, SainW, ins7, DW, DR, Cd, 3, GRW, SN, 8)
Oper (Opb658, DoCmpW, DW, SaipW, ins7, DW, DR, Cd, 3, GRW, SN, 8)
Oper (Opb660, DoCmpW, DW, SmaiW, ins7, DW, DR, Cd, 3, GRW, SN, 12)
Oper (Opb668, DoCmpW, DW, SdaiW, ins7, DW, DR, Cd, 3, GRW, SN, 12)
Oper (Opb670, DoCmpW, DW, SaixW, ins7, DW, DR, Cd, 3, GRW, SN, 16)
Oper (Opb678, DoCmpW, DW, SearW, ins7, DW, DR, Cd, 3, GRW, SN, (cycles+11)&0xfffffffc)
Oper (Opb680, DoCmpL, DL, SdL, ins7, DL, DR, Cd, 3, GRL, SN, 8)
Oper (Opb688, DoCmpL, DL, SaL, ins7, DL, DR, Cd, 3, GRL, SN, 8)
Oper (Opb690, DoCmpL, DL, SainL, ins7, DL, DR, Cd, 3, GRL, SN, 16)
Oper (Opb698, DoCmpL, DL, SaipL, ins7, DL, DR, Cd, 3, GRL, SN, 16)
Oper (Opb6a0, DoCmpL, DL, SmaiL, ins7, DL, DR, Cd, 3, GRL, SN, 16)
Oper (Opb6a8, DoCmpL, DL, SdaiL, ins7, DL, DR, Cd, 3, GRL, SN, 20)
Oper (Opb6b0, DoCmpL, DL, SaixL, ins7, DL, DR, Cd, 3, GRL, SN, 20)
Oper (Opb6b8, DoCmpL, DL, SearL, ins7, DL, DR, Cd, 3, GRL, SN, (cycles+17)&0xfffffffc)
Oper (Opb6c0, DoCmpa, DL, SdW, ins7, DL, DR, Ca, 3, GRL, SN, 8)
Oper (Opb6c8, DoCmpa, DL, SaW, ins7, DL, DR, Ca, 3, GRL, SN, 8)
Oper (Opb6d0, DoCmpa, DL, SainW, ins7, DL, DR, Ca, 3, GRL, SN, 12)
Oper (Opb6d8, DoCmpa, DL, SaipW, ins7, DL, DR, Ca, 3, GRL, SN, 12)
Oper (Opb6e0, DoCmpa, DL, SmaiW, ins7, DL, DR, Ca, 3, GRL, SN, 12)
Oper (Opb6e8, DoCmpa, DL, SdaiW, ins7, DL, DR, Ca, 3, GRL, SN, 16)
Oper (Opb6f0, DoCmpa, DL, SaixW, ins7, DL, DR, Ca, 3, GRL, SN, 16)
Oper (Opb6f8, DoCmpa, DL, SearW, ins7, DL, DR, Ca, 3, GRL, SN, (cycles+13)&0xfffffffc)

Oper (Opb708, DoCmpm, DB, SaipB, ins7, DB, DA, CaipB, 3, GMB, SN, 12)
Oper (Opb748, DoCmpm, DW, SaipW, ins7, DW, DA, CaipW, 3, GMW, SN, 12)
Oper (Opb788, DoCmpm, DL, SaipL, ins7, DL, DA, CaipL, 3, GML, SN, 20)
Oper (Opb7c0, DoCmpa, DL, SdL, ins7, DL, DR, Ca, 3, GRL, SN, 8)
Oper (Opb7c8, DoCmpa, DL, SaL, ins7, DL, DR, Ca, 3, GRL, SN, 8)
Oper (Opb7d0, DoCmpa, DL, SainL, ins7, DL, DR, Ca, 3, GRL, SN, 16)
Oper (Opb7d8, DoCmpa, DL, SaipL, ins7, DL, DR, Ca, 3, GRL, SN, 16)
Oper (Opb7e0, DoCmpa, DL, SmaiL, ins7, DL, DR, Ca, 3, GRL, SN, 16)
Oper (Opb7e8, DoCmpa, DL, SdaiL, ins7, DL, DR, Ca, 3, GRL, SN, 20)
Oper (Opb7f0, DoCmpa, DL, SaixL, ins7, DL, DR, Ca, 3, GRL, SN, 20)
Oper (Opb7f8, DoCmpa, DL, SearL, ins7, DL, DR, Ca, 3, GRL, SN, (cycles+17)&0xfffffffc)

Oper (Opb800, DoCmpB, DB, SdB, ins7, DB, DR, Cd, 4, GRB, SN, 4)
Oper (Opb810, DoCmpB, DB, SainB, ins7, DB, DR, Cd, 4, GRB, SN, 8)
Oper (Opb818, DoCmpB, DB, SaipB, ins7, DB, DR, Cd, 4, GRB, SN, 8)
Oper (Opb820, DoCmpB, DB, SmaiB, ins7, DB, DR, Cd, 4, GRB, SN, 12)
Oper (Opb828, DoCmpB, DB, SdaiB, ins7, DB, DR, Cd, 4, GRB, SN, 12)
Oper (Opb830, DoCmpB, DB, SaixB, ins7, DB, DR, Cd, 4, GRB, SN, 16)
Oper (Opb838, DoCmpB, DB, SearB, ins7, DB, DR, Cd, 4, GRB, SN, (cycles+11)&0xfffffffc)
Oper (Opb840, DoCmpW, DW, SdW, ins7, DW, DR, Cd, 4, GRW, SN, 4)
Oper (Opb848, DoCmpW, DW, SaW, ins7, DW, DR, Cd, 4, GRW, SN, 4)
Oper (Opb850, DoCmpW, DW, SainW, ins7, DW, DR, Cd, 4, GRW, SN, 8)
Oper (Opb858, DoCmpW, DW, SaipW, ins7, DW, DR, Cd, 4, GRW, SN, 8)
Oper (Opb860, DoCmpW, DW, SmaiW, ins7, DW, DR, Cd, 4, GRW, SN, 12)
Oper (Opb868, DoCmpW, DW, SdaiW, ins7, DW, DR, Cd, 4, GRW, SN, 12)
Oper (Opb870, DoCmpW, DW, SaixW, ins7, DW, DR, Cd, 4, GRW, SN, 16)
Oper (Opb878, DoCmpW, DW, SearW, ins7, DW, DR, Cd, 4, GRW, SN, (cycles+11)&0xfffffffc)
Oper (Opb880, DoCmpL, DL, SdL, ins7, DL, DR, Cd, 4, GRL, SN, 8)
Oper (Opb888, DoCmpL, DL, SaL, ins7, DL, DR, Cd, 4, GRL, SN, 8)
Oper (Opb890, DoCmpL, DL, SainL, ins7, DL, DR, Cd, 4, GRL, SN, 16)
Oper (Opb898, DoCmpL, DL, SaipL, ins7, DL, DR, Cd, 4, GRL, SN, 16)
Oper (Opb8a0, DoCmpL, DL, SmaiL, ins7, DL, DR, Cd, 4, GRL, SN, 16)
Oper (Opb8a8, DoCmpL, DL, SdaiL, ins7, DL, DR, Cd, 4, GRL, SN, 20)
Oper (Opb8b0, DoCmpL, DL, SaixL, ins7, DL, DR, Cd, 4, GRL, SN, 20)
Oper (Opb8b8, DoCmpL, DL, SearL, ins7, DL, DR, Cd, 4, GRL, SN, (cycles+17)&0xfffffffc)
Oper (Opb8c0, DoCmpa, DL, SdW, ins7, DL, DR, Ca, 4, GRL, SN, 8)
Oper (Opb8c8, DoCmpa, DL, SaW, ins7, DL, DR, Ca, 4, GRL, SN, 8)
Oper (Opb8d0, DoCmpa, DL, SainW, ins7, DL, DR, Ca, 4, GRL, SN, 12)
Oper (Opb8d8, DoCmpa, DL, SaipW, ins7, DL, DR, Ca, 4, GRL, SN, 12)
Oper (Opb8e0, DoCmpa, DL, SmaiW, ins7, DL, DR, Ca, 4, GRL, SN, 12)
Oper (Opb8e8, DoCmpa, DL, SdaiW, ins7, DL, DR, Ca, 4, GRL, SN, 16)
Oper (Opb8f0, DoCmpa, DL, SaixW, ins7, DL, DR, Ca, 4, GRL, SN, 16)
Oper (Opb8f8, DoCmpa, DL, SearW, ins7, DL, DR, Ca, 4, GRL, SN, (cycles+13)&0xfffffffc)

Oper (Opb908, DoCmpm, DB, SaipB, ins7, DB, DA, CaipB, 4, GMB, SN, 12)
Oper (Opb948, DoCmpm, DW, SaipW, ins7, DW, DA, CaipW, 4, GMW, SN, 12)
Oper (Opb988, DoCmpm, DL, SaipL, ins7, DL, DA, CaipL, 4, GML, SN, 20)
Oper (Opb9c0, DoCmpa, DL, SdL, ins7, DL, DR, Ca, 4, GRL, SN, 8)
Oper (Opb9c8, DoCmpa, DL, SaL, ins7, DL, DR, Ca, 4, GRL, SN, 8)
Oper (Opb9d0, DoCmpa, DL, SainL, ins7, DL, DR, Ca, 4, GRL, SN, 16)
Oper (Opb9d8, DoCmpa, DL, SaipL, ins7, DL, DR, Ca, 4, GRL, SN, 16)
Oper (Opb9e0, DoCmpa, DL, SmaiL, ins7, DL, DR, Ca, 4, GRL, SN, 16)
Oper (Opb9e8, DoCmpa, DL, SdaiL, ins7, DL, DR, Ca, 4, GRL, SN, 20)
Oper (Opb9f0, DoCmpa, DL, SaixL, ins7, DL, DR, Ca, 4, GRL, SN, 20)
Oper (Opb9f8, DoCmpa, DL, SearL, ins7, DL, DR, Ca, 4, GRL, SN, (cycles+17)&0xfffffffc)

Oper (Opba00, DoCmpB, DB, SdB, ins7, DB, DR, Cd, 5, GRB, SN, 4)
Oper (Opba10, DoCmpB, DB, SainB, ins7, DB, DR, Cd, 5, GRB, SN, 8)
Oper (Opba18, DoCmpB, DB, SaipB, ins7, DB, DR, Cd, 5, GRB, SN, 8)
Oper (Opba20, DoCmpB, DB, SmaiB, ins7, DB, DR, Cd, 5, GRB, SN, 12)
Oper (Opba28, DoCmpB, DB, SdaiB, ins7, DB, DR, Cd, 5, GRB, SN, 12)
Oper (Opba30, DoCmpB, DB, SaixB, ins7, DB, DR, Cd, 5, GRB, SN, 16)
Oper (Opba38, DoCmpB, DB, SearB, ins7, DB, DR, Cd, 5, GRB, SN, (cycles+11)&0xfffffffc)
Oper (Opba40, DoCmpW, DW, SdW, ins7, DW, DR, Cd, 5, GRW, SN, 4)
Oper (Opba48, DoCmpW, DW, SaW, ins7, DW, DR, Cd, 5, GRW, SN, 4)
Oper (Opba50, DoCmpW, DW, SainW, ins7, DW, DR, Cd, 5, GRW, SN, 8)
Oper (Opba58, DoCmpW, DW, SaipW, ins7, DW, DR, Cd, 5, GRW, SN, 8)
Oper (Opba60, DoCmpW, DW, SmaiW, ins7, DW, DR, Cd, 5, GRW, SN, 12)
Oper (Opba68, DoCmpW, DW, SdaiW, ins7, DW, DR, Cd, 5, GRW, SN, 12)
Oper (Opba70, DoCmpW, DW, SaixW, ins7, DW, DR, Cd, 5, GRW, SN, 16)
Oper (Opba78, DoCmpW, DW, SearW, ins7, DW, DR, Cd, 5, GRW, SN, (cycles+11)&0xfffffffc)
Oper (Opba80, DoCmpL, DL, SdL, ins7, DL, DR, Cd, 5, GRL, SN, 8)
Oper (Opba88, DoCmpL, DL, SaL, ins7, DL, DR, Cd, 5, GRL, SN, 8)
Oper (Opba90, DoCmpL, DL, SainL, ins7, DL, DR, Cd, 5, GRL, SN, 16)
Oper (Opba98, DoCmpL, DL, SaipL, ins7, DL, DR, Cd, 5, GRL, SN, 16)
Oper (Opbaa0, DoCmpL, DL, SmaiL, ins7, DL, DR, Cd, 5, GRL, SN, 16)
Oper (Opbaa8, DoCmpL, DL, SdaiL, ins7, DL, DR, Cd, 5, GRL, SN, 20)
Oper (Opbab0, DoCmpL, DL, SaixL, ins7, DL, DR, Cd, 5, GRL, SN, 20)
Oper (Opbab8, DoCmpL, DL, SearL, ins7, DL, DR, Cd, 5, GRL, SN, (cycles+17)&0xfffffffc)
Oper (Opbac0, DoCmpa, DL, SdW, ins7, DL, DR, Ca, 5, GRL, SN, 8)
Oper (Opbac8, DoCmpa, DL, SaW, ins7, DL, DR, Ca, 5, GRL, SN, 8)
Oper (Opbad0, DoCmpa, DL, SainW, ins7, DL, DR, Ca, 5, GRL, SN, 12)
Oper (Opbad8, DoCmpa, DL, SaipW, ins7, DL, DR, Ca, 5, GRL, SN, 12)
Oper (Opbae0, DoCmpa, DL, SmaiW, ins7, DL, DR, Ca, 5, GRL, SN, 12)
Oper (Opbae8, DoCmpa, DL, SdaiW, ins7, DL, DR, Ca, 5, GRL, SN, 16)
Oper (Opbaf0, DoCmpa, DL, SaixW, ins7, DL, DR, Ca, 5, GRL, SN, 16)
Oper (Opbaf8, DoCmpa, DL, SearW, ins7, DL, DR, Ca, 5, GRL, SN, (cycles+13)&0xfffffffc)

Oper (Opbb08, DoCmpm, DB, SaipB, ins7, DB, DA, CaipB, 5, GMB, SN, 12)
Oper (Opbb48, DoCmpm, DW, SaipW, ins7, DW, DA, CaipW, 5, GMW, SN, 12)
Oper (Opbb88, DoCmpm, DL, SaipL, ins7, DL, DA, CaipL, 5, GML, SN, 20)
Oper (Opbbc0, DoCmpa, DL, SdL, ins7, DL, DR, Ca, 5, GRL, SN, 8)
Oper (Opbbc8, DoCmpa, DL, SaL, ins7, DL, DR, Ca, 5, GRL, SN, 8)
Oper (Opbbd0, DoCmpa, DL, SainL, ins7, DL, DR, Ca, 5, GRL, SN, 16)
Oper (Opbbd8, DoCmpa, DL, SaipL, ins7, DL, DR, Ca, 5, GRL, SN, 16)
Oper (Opbbe0, DoCmpa, DL, SmaiL, ins7, DL, DR, Ca, 5, GRL, SN, 16)
Oper (Opbbe8, DoCmpa, DL, SdaiL, ins7, DL, DR, Ca, 5, GRL, SN, 20)
Oper (Opbbf0, DoCmpa, DL, SaixL, ins7, DL, DR, Ca, 5, GRL, SN, 20)
Oper (Opbbf8, DoCmpa, DL, SearL, ins7, DL, DR, Ca, 5, GRL, SN, (cycles+17)&0xfffffffc)

Oper (Opbc00, DoCmpB, DB, SdB, ins7, DB, DR, Cd, 6, GRB, SN, 4)
Oper (Opbc10, DoCmpB, DB, SainB, ins7, DB, DR, Cd, 6, GRB, SN, 8)
Oper (Opbc18, DoCmpB, DB, SaipB, ins7, DB, DR, Cd, 6, GRB, SN, 8)
Oper (Opbc20, DoCmpB, DB, SmaiB, ins7, DB, DR, Cd, 6, GRB, SN, 12)
Oper (Opbc28, DoCmpB, DB, SdaiB, ins7, DB, DR, Cd, 6, GRB, SN, 12)
Oper (Opbc30, DoCmpB, DB, SaixB, ins7, DB, DR, Cd, 6, GRB, SN, 16)
Oper (Opbc38, DoCmpB, DB, SearB, ins7, DB, DR, Cd, 6, GRB, SN, (cycles+11)&0xfffffffc)
Oper (Opbc40, DoCmpW, DW, SdW, ins7, DW, DR, Cd, 6, GRW, SN, 4)
Oper (Opbc48, DoCmpW, DW, SaW, ins7, DW, DR, Cd, 6, GRW, SN, 4)
Oper (Opbc50, DoCmpW, DW, SainW, ins7, DW, DR, Cd, 6, GRW, SN, 8)
Oper (Opbc58, DoCmpW, DW, SaipW, ins7, DW, DR, Cd, 6, GRW, SN, 8)
Oper (Opbc60, DoCmpW, DW, SmaiW, ins7, DW, DR, Cd, 6, GRW, SN, 12)
Oper (Opbc68, DoCmpW, DW, SdaiW, ins7, DW, DR, Cd, 6, GRW, SN, 12)
Oper (Opbc70, DoCmpW, DW, SaixW, ins7, DW, DR, Cd, 6, GRW, SN, 16)
Oper (Opbc78, DoCmpW, DW, SearW, ins7, DW, DR, Cd, 6, GRW, SN, (cycles+11)&0xfffffffc)
Oper (Opbc80, DoCmpL, DL, SdL, ins7, DL, DR, Cd, 6, GRL, SN, 8)
Oper (Opbc88, DoCmpL, DL, SaL, ins7, DL, DR, Cd, 6, GRL, SN, 8)
Oper (Opbc90, DoCmpL, DL, SainL, ins7, DL, DR, Cd, 6, GRL, SN, 16)
Oper (Opbc98, DoCmpL, DL, SaipL, ins7, DL, DR, Cd, 6, GRL, SN, 16)
Oper (Opbca0, DoCmpL, DL, SmaiL, ins7, DL, DR, Cd, 6, GRL, SN, 16)
Oper (Opbca8, DoCmpL, DL, SdaiL, ins7, DL, DR, Cd, 6, GRL, SN, 20)
Oper (Opbcb0, DoCmpL, DL, SaixL, ins7, DL, DR, Cd, 6, GRL, SN, 20)
Oper (Opbcb8, DoCmpL, DL, SearL, ins7, DL, DR, Cd, 6, GRL, SN, (cycles+17)&0xfffffffc)
Oper (Opbcc0, DoCmpa, DL, SdW, ins7, DL, DR, Ca, 6, GRL, SN, 8)
Oper (Opbcc8, DoCmpa, DL, SaW, ins7, DL, DR, Ca, 6, GRL, SN, 8)
Oper (Opbcd0, DoCmpa, DL, SainW, ins7, DL, DR, Ca, 6, GRL, SN, 12)
Oper (Opbcd8, DoCmpa, DL, SaipW, ins7, DL, DR, Ca, 6, GRL, SN, 12)
Oper (Opbce0, DoCmpa, DL, SmaiW, ins7, DL, DR, Ca, 6, GRL, SN, 12)
Oper (Opbce8, DoCmpa, DL, SdaiW, ins7, DL, DR, Ca, 6, GRL, SN, 16)
Oper (Opbcf0, DoCmpa, DL, SaixW, ins7, DL, DR, Ca, 6, GRL, SN, 16)
Oper (Opbcf8, DoCmpa, DL, SearW, ins7, DL, DR, Ca, 6, GRL, SN, (cycles+13)&0xfffffffc)

Oper (Opbd08, DoCmpm, DB, SaipB, ins7, DB, DA, CaipB, 6, GMB, SN, 12)
Oper (Opbd48, DoCmpm, DW, SaipW, ins7, DW, DA, CaipW, 6, GMW, SN, 12)
Oper (Opbd88, DoCmpm, DL, SaipL, ins7, DL, DA, CaipL, 6, GML, SN, 20)
Oper (Opbdc0, DoCmpa, DL, SdL, ins7, DL, DR, Ca, 6, GRL, SN, 8)
Oper (Opbdc8, DoCmpa, DL, SaL, ins7, DL, DR, Ca, 6, GRL, SN, 8)
Oper (Opbdd0, DoCmpa, DL, SainL, ins7, DL, DR, Ca, 6, GRL, SN, 16)
Oper (Opbdd8, DoCmpa, DL, SaipL, ins7, DL, DR, Ca, 6, GRL, SN, 16)
Oper (Opbde0, DoCmpa, DL, SmaiL, ins7, DL, DR, Ca, 6, GRL, SN, 16)
Oper (Opbde8, DoCmpa, DL, SdaiL, ins7, DL, DR, Ca, 6, GRL, SN, 20)
Oper (Opbdf0, DoCmpa, DL, SaixL, ins7, DL, DR, Ca, 6, GRL, SN, 20)
Oper (Opbdf8, DoCmpa, DL, SearL, ins7, DL, DR, Ca, 6, GRL, SN, (cycles+17)&0xfffffffc)

Oper (Opbe00, DoCmpB, DB, SdB, ins7, DB, DR, Cd, 7, GRB, SN, 4)
Oper (Opbe10, DoCmpB, DB, SainB, ins7, DB, DR, Cd, 7, GRB, SN, 8)
Oper (Opbe18, DoCmpB, DB, SaipB, ins7, DB, DR, Cd, 7, GRB, SN, 8)
Oper (Opbe20, DoCmpB, DB, SmaiB, ins7, DB, DR, Cd, 7, GRB, SN, 12)
Oper (Opbe28, DoCmpB, DB, SdaiB, ins7, DB, DR, Cd, 7, GRB, SN, 12)
Oper (Opbe30, DoCmpB, DB, SaixB, ins7, DB, DR, Cd, 7, GRB, SN, 16)
Oper (Opbe38, DoCmpB, DB, SearB, ins7, DB, DR, Cd, 7, GRB, SN, (cycles+11)&0xfffffffc)
Oper (Opbe40, DoCmpW, DW, SdW, ins7, DW, DR, Cd, 7, GRW, SN, 4)
Oper (Opbe48, DoCmpW, DW, SaW, ins7, DW, DR, Cd, 7, GRW, SN, 4)
Oper (Opbe50, DoCmpW, DW, SainW, ins7, DW, DR, Cd, 7, GRW, SN, 8)
Oper (Opbe58, DoCmpW, DW, SaipW, ins7, DW, DR, Cd, 7, GRW, SN, 8)
Oper (Opbe60, DoCmpW, DW, SmaiW, ins7, DW, DR, Cd, 7, GRW, SN, 12)
Oper (Opbe68, DoCmpW, DW, SdaiW, ins7, DW, DR, Cd, 7, GRW, SN, 12)
Oper (Opbe70, DoCmpW, DW, SaixW, ins7, DW, DR, Cd, 7, GRW, SN, 16)
Oper (Opbe78, DoCmpW, DW, SearW, ins7, DW, DR, Cd, 7, GRW, SN, (cycles+11)&0xfffffffc)
Oper (Opbe80, DoCmpL, DL, SdL, ins7, DL, DR, Cd, 7, GRL, SN, 8)
Oper (Opbe88, DoCmpL, DL, SaL, ins7, DL, DR, Cd, 7, GRL, SN, 8)
Oper (Opbe90, DoCmpL, DL, SainL, ins7, DL, DR, Cd, 7, GRL, SN, 16)
Oper (Opbe98, DoCmpL, DL, SaipL, ins7, DL, DR, Cd, 7, GRL, SN, 16)
Oper (Opbea0, DoCmpL, DL, SmaiL, ins7, DL, DR, Cd, 7, GRL, SN, 16)
Oper (Opbea8, DoCmpL, DL, SdaiL, ins7, DL, DR, Cd, 7, GRL, SN, 20)
Oper (Opbeb0, DoCmpL, DL, SaixL, ins7, DL, DR, Cd, 7, GRL, SN, 20)
Oper (Opbeb8, DoCmpL, DL, SearL, ins7, DL, DR, Cd, 7, GRL, SN, (cycles+17)&0xfffffffc)
Oper (Opbec0, DoCmpa, DL, SdW, ins7, DL, DR, Ca, 7, GRL, SN, 8)
Oper (Opbec8, DoCmpa, DL, SaW, ins7, DL, DR, Ca, 7, GRL, SN, 8)
Oper (Opbed0, DoCmpa, DL, SainW, ins7, DL, DR, Ca, 7, GRL, SN, 12)
Oper (Opbed8, DoCmpa, DL, SaipW, ins7, DL, DR, Ca, 7, GRL, SN, 12)
Oper (Opbee0, DoCmpa, DL, SmaiW, ins7, DL, DR, Ca, 7, GRL, SN, 12)
Oper (Opbee8, DoCmpa, DL, SdaiW, ins7, DL, DR, Ca, 7, GRL, SN, 16)
Oper (Opbef0, DoCmpa, DL, SaixW, ins7, DL, DR, Ca, 7, GRL, SN, 16)
Oper (Opbef8, DoCmpa, DL, SearW, ins7, DL, DR, Ca, 7, GRL, SN, (cycles+13)&0xfffffffc)

Oper (Opbf08, DoCmpm, DB, SaipB, ins7, DB, DA, CaipB15, 7, GMB, SN, 12)
Oper (Opbf48, DoCmpm, DW, SaipW, ins7, DW, DA, CaipW, 7, GMW, SN, 12)
Oper (Opbf88, DoCmpm, DL, SaipL, ins7, DL, DA, CaipL, 7, GML, SN, 20)
Oper (Opbfc0, DoCmpa, DL, SdL, ins7, DL, DR, Ca, 7, GRL, SN, 8)
Oper (Opbfc8, DoCmpa, DL, SaL, ins7, DL, DR, Ca, 7, GRL, SN, 8)
Oper (Opbfd0, DoCmpa, DL, SainL, ins7, DL, DR, Ca, 7, GRL, SN, 16)
Oper (Opbfd8, DoCmpa, DL, SaipL, ins7, DL, DR, Ca, 7, GRL, SN, 16)
Oper (Opbfe0, DoCmpa, DL, SmaiL, ins7, DL, DR, Ca, 7, GRL, SN, 16)
Oper (Opbfe8, DoCmpa, DL, SdaiL, ins7, DL, DR, Ca, 7, GRL, SN, 20)
Oper (Opbff0, DoCmpa, DL, SaixL, ins7, DL, DR, Ca, 7, GRL, SN, 20)
Oper (Opbff8, DoCmpa, DL, SearL, ins7, DL, DR, Ca, 7, GRL, SN, (cycles+17)&0xfffffffc)

Oper (Op80c0, DoDivu, DW, SdW, ins7, DL, DR, Cd, 0, GRL, SRL, 140)
Oper (Op80d0, DoDivu, DW, SainW, ins7, DL, DR, Cd, 0, GRL, SRL, 144)
Oper (Op80d8, DoDivu, DW, SaipW, ins7, DL, DR, Cd, 0, GRL, SRL, 144)
Oper (Op80e0, DoDivu, DW, SmaiW, ins7, DL, DR, Cd, 0, GRL, SRL, 148)
Oper (Op80e8, DoDivu, DW, SdaiW, ins7, DL, DR, Cd, 0, GRL, SRL, 148)
Oper (Op80f0, DoDivu, DW, SaixW, ins7, DL, DR, Cd, 0, GRL, SRL, 152)
Oper (Op80f8, DoDivu, DW, SearW, ins7, DL, DR, Cd, 0, GRL, SRL, (cycles+147)&0xfffffffc)

Oper (Op81c0, DoDivs, DW, SdW, ins7, DL, DR, Cd, 0, GRL, SRL, 160)
Oper (Op81d0, DoDivs, DW, SainW, ins7, DL, DR, Cd, 0, GRL, SRL, 164)
Oper (Op81d8, DoDivs, DW, SaipW, ins7, DL, DR, Cd, 0, GRL, SRL, 164)
Oper (Op81e0, DoDivs, DW, SmaiW, ins7, DL, DR, Cd, 0, GRL, SRL, 164)
Oper (Op81e8, DoDivs, DW, SdaiW, ins7, DL, DR, Cd, 0, GRL, SRL, 168)
Oper (Op81f0, DoDivs, DW, SaixW, ins7, DL, DR, Cd, 0, GRL, SRL, 168)
Oper (Op81f8, DoDivs, DW, SearW, ins7, DL, DR, Cd, 0, GRL, SRL, (cycles+165)&0xfffffffc)

Oper (Op82c0, DoDivu, DW, SdW, ins7, DL, DR, Cd, 1, GRL, SRL, 140)
Oper (Op82d0, DoDivu, DW, SainW, ins7, DL, DR, Cd, 1, GRL, SRL, 144)
Oper (Op82d8, DoDivu, DW, SaipW, ins7, DL, DR, Cd, 1, GRL, SRL, 144)
Oper (Op82e0, DoDivu, DW, SmaiW, ins7, DL, DR, Cd, 1, GRL, SRL, 148)
Oper (Op82e8, DoDivu, DW, SdaiW, ins7, DL, DR, Cd, 1, GRL, SRL, 148)
Oper (Op82f0, DoDivu, DW, SaixW, ins7, DL, DR, Cd, 1, GRL, SRL, 152)
Oper (Op82f8, DoDivu, DW, SearW, ins7, DL, DR, Cd, 1, GRL, SRL, (cycles+147)&0xfffffffc)

Oper (Op83c0, DoDivs, DW, SdW, ins7, DL, DR, Cd, 1, GRL, SRL, 160)
Oper (Op83d0, DoDivs, DW, SainW, ins7, DL, DR, Cd, 1, GRL, SRL, 164)
Oper (Op83d8, DoDivs, DW, SaipW, ins7, DL, DR, Cd, 1, GRL, SRL, 164)
Oper (Op83e0, DoDivs, DW, SmaiW, ins7, DL, DR, Cd, 1, GRL, SRL, 164)
Oper (Op83e8, DoDivs, DW, SdaiW, ins7, DL, DR, Cd, 1, GRL, SRL, 168)
Oper (Op83f0, DoDivs, DW, SaixW, ins7, DL, DR, Cd, 1, GRL, SRL, 168)
Oper (Op83f8, DoDivs, DW, SearW, ins7, DL, DR, Cd, 1, GRL, SRL, (cycles+165)&0xfffffffc)

Oper (Op84c0, DoDivu, DW, SdW, ins7, DL, DR, Cd, 2, GRL, SRL, 140)
Oper (Op84d0, DoDivu, DW, SainW, ins7, DL, DR, Cd, 2, GRL, SRL, 144)
Oper (Op84d8, DoDivu, DW, SaipW, ins7, DL, DR, Cd, 2, GRL, SRL, 144)
Oper (Op84e0, DoDivu, DW, SmaiW, ins7, DL, DR, Cd, 2, GRL, SRL, 148)
Oper (Op84e8, DoDivu, DW, SdaiW, ins7, DL, DR, Cd, 2, GRL, SRL, 148)
Oper (Op84f0, DoDivu, DW, SaixW, ins7, DL, DR, Cd, 2, GRL, SRL, 152)
Oper (Op84f8, DoDivu, DW, SearW, ins7, DL, DR, Cd, 2, GRL, SRL, (cycles+147)&0xfffffffc)

Oper (Op85c0, DoDivs, DW, SdW, ins7, DL, DR, Cd, 2, GRL, SRL, 160)
Oper (Op85d0, DoDivs, DW, SainW, ins7, DL, DR, Cd, 2, GRL, SRL, 164)
Oper (Op85d8, DoDivs, DW, SaipW, ins7, DL, DR, Cd, 2, GRL, SRL, 164)
Oper (Op85e0, DoDivs, DW, SmaiW, ins7, DL, DR, Cd, 2, GRL, SRL, 164)
Oper (Op85e8, DoDivs, DW, SdaiW, ins7, DL, DR, Cd, 2, GRL, SRL, 168)
Oper (Op85f0, DoDivs, DW, SaixW, ins7, DL, DR, Cd, 2, GRL, SRL, 168)
Oper (Op85f8, DoDivs, DW, SearW, ins7, DL, DR, Cd, 2, GRL, SRL, (cycles+165)&0xfffffffc)

Oper (Op86c0, DoDivu, DW, SdW, ins7, DL, DR, Cd, 3, GRL, SRL, 140)
Oper (Op86d0, DoDivu, DW, SainW, ins7, DL, DR, Cd, 3, GRL, SRL, 144)
Oper (Op86d8, DoDivu, DW, SaipW, ins7, DL, DR, Cd, 3, GRL, SRL, 144)
Oper (Op86e0, DoDivu, DW, SmaiW, ins7, DL, DR, Cd, 3, GRL, SRL, 148)
Oper (Op86e8, DoDivu, DW, SdaiW, ins7, DL, DR, Cd, 3, GRL, SRL, 148)
Oper (Op86f0, DoDivu, DW, SaixW, ins7, DL, DR, Cd, 3, GRL, SRL, 152)
Oper (Op86f8, DoDivu, DW, SearW, ins7, DL, DR, Cd, 3, GRL, SRL, (cycles+147)&0xfffffffc)

Oper (Op87c0, DoDivs, DW, SdW, ins7, DL, DR, Cd, 3, GRL, SRL, 160)
Oper (Op87d0, DoDivs, DW, SainW, ins7, DL, DR, Cd, 3, GRL, SRL, 164)
Oper (Op87d8, DoDivs, DW, SaipW, ins7, DL, DR, Cd, 3, GRL, SRL, 164)
Oper (Op87e0, DoDivs, DW, SmaiW, ins7, DL, DR, Cd, 3, GRL, SRL, 164)
Oper (Op87e8, DoDivs, DW, SdaiW, ins7, DL, DR, Cd, 3, GRL, SRL, 168)
Oper (Op87f0, DoDivs, DW, SaixW, ins7, DL, DR, Cd, 3, GRL, SRL, 168)
Oper (Op87f8, DoDivs, DW, SearW, ins7, DL, DR, Cd, 3, GRL, SRL, (cycles+165)&0xfffffffc)

Oper (Op88c0, DoDivu, DW, SdW, ins7, DL, DR, Cd, 4, GRL, SRL, 140)
Oper (Op88d0, DoDivu, DW, SainW, ins7, DL, DR, Cd, 4, GRL, SRL, 144)
Oper (Op88d8, DoDivu, DW, SaipW, ins7, DL, DR, Cd, 4, GRL, SRL, 144)
Oper (Op88e0, DoDivu, DW, SmaiW, ins7, DL, DR, Cd, 4, GRL, SRL, 148)
Oper (Op88e8, DoDivu, DW, SdaiW, ins7, DL, DR, Cd, 4, GRL, SRL, 148)
Oper (Op88f0, DoDivu, DW, SaixW, ins7, DL, DR, Cd, 4, GRL, SRL, 152)
Oper (Op88f8, DoDivu, DW, SearW, ins7, DL, DR, Cd, 4, GRL, SRL, (cycles+147)&0xfffffffc)

Oper (Op89c0, DoDivs, DW, SdW, ins7, DL, DR, Cd, 4, GRL, SRL, 160)
Oper (Op89d0, DoDivs, DW, SainW, ins7, DL, DR, Cd, 4, GRL, SRL, 164)
Oper (Op89d8, DoDivs, DW, SaipW, ins7, DL, DR, Cd, 4, GRL, SRL, 164)
Oper (Op89e0, DoDivs, DW, SmaiW, ins7, DL, DR, Cd, 4, GRL, SRL, 164)
Oper (Op89e8, DoDivs, DW, SdaiW, ins7, DL, DR, Cd, 4, GRL, SRL, 168)
Oper (Op89f0, DoDivs, DW, SaixW, ins7, DL, DR, Cd, 4, GRL, SRL, 168)
Oper (Op89f8, DoDivs, DW, SearW, ins7, DL, DR, Cd, 4, GRL, SRL, (cycles+165)&0xfffffffc)

Oper (Op8ac0, DoDivu, DW, SdW, ins7, DL, DR, Cd, 5, GRL, SRL, 140)
Oper (Op8ad0, DoDivu, DW, SainW, ins7, DL, DR, Cd, 5, GRL, SRL, 144)
Oper (Op8ad8, DoDivu, DW, SaipW, ins7, DL, DR, Cd, 5, GRL, SRL, 144)
Oper (Op8ae0, DoDivu, DW, SmaiW, ins7, DL, DR, Cd, 5, GRL, SRL, 148)
Oper (Op8ae8, DoDivu, DW, SdaiW, ins7, DL, DR, Cd, 5, GRL, SRL, 148)
Oper (Op8af0, DoDivu, DW, SaixW, ins7, DL, DR, Cd, 5, GRL, SRL, 152)
Oper (Op8af8, DoDivu, DW, SearW, ins7, DL, DR, Cd, 5, GRL, SRL, (cycles+147)&0xfffffffc)

Oper (Op8bc0, DoDivs, DW, SdW, ins7, DL, DR, Cd, 5, GRL, SRL, 160)
Oper (Op8bd0, DoDivs, DW, SainW, ins7, DL, DR, Cd, 5, GRL, SRL, 164)
Oper (Op8bd8, DoDivs, DW, SaipW, ins7, DL, DR, Cd, 5, GRL, SRL, 164)
Oper (Op8be0, DoDivs, DW, SmaiW, ins7, DL, DR, Cd, 5, GRL, SRL, 164)
Oper (Op8be8, DoDivs, DW, SdaiW, ins7, DL, DR, Cd, 5, GRL, SRL, 168)
Oper (Op8bf0, DoDivs, DW, SaixW, ins7, DL, DR, Cd, 5, GRL, SRL, 168)
Oper (Op8bf8, DoDivs, DW, SearW, ins7, DL, DR, Cd, 5, GRL, SRL, (cycles+165)&0xfffffffc)

Oper (Op8cc0, DoDivu, DW, SdW, ins7, DL, DR, Cd, 6, GRL, SRL, 140)
Oper (Op8cd0, DoDivu, DW, SainW, ins7, DL, DR, Cd, 6, GRL, SRL, 144)
Oper (Op8cd8, DoDivu, DW, SaipW, ins7, DL, DR, Cd, 6, GRL, SRL, 144)
Oper (Op8ce0, DoDivu, DW, SmaiW, ins7, DL, DR, Cd, 6, GRL, SRL, 148)
Oper (Op8ce8, DoDivu, DW, SdaiW, ins7, DL, DR, Cd, 6, GRL, SRL, 148)
Oper (Op8cf0, DoDivu, DW, SaixW, ins7, DL, DR, Cd, 6, GRL, SRL, 152)
Oper (Op8cf8, DoDivu, DW, SearW, ins7, DL, DR, Cd, 6, GRL, SRL, (cycles+147)&0xfffffffc)

Oper (Op8dc0, DoDivs, DW, SdW, ins7, DL, DR, Cd, 6, GRL, SRL, 160)
Oper (Op8dd0, DoDivs, DW, SainW, ins7, DL, DR, Cd, 6, GRL, SRL, 164)
Oper (Op8dd8, DoDivs, DW, SaipW, ins7, DL, DR, Cd, 6, GRL, SRL, 164)
Oper (Op8de0, DoDivs, DW, SmaiW, ins7, DL, DR, Cd, 6, GRL, SRL, 164)
Oper (Op8de8, DoDivs, DW, SdaiW, ins7, DL, DR, Cd, 6, GRL, SRL, 168)
Oper (Op8df0, DoDivs, DW, SaixW, ins7, DL, DR, Cd, 6, GRL, SRL, 168)
Oper (Op8df8, DoDivs, DW, SearW, ins7, DL, DR, Cd, 6, GRL, SRL, (cycles+165)&0xfffffffc)

Oper (Op8ec0, DoDivu, DW, SdW, ins7, DL, DR, Cd, 7, GRL, SRL, 140)
Oper (Op8ed0, DoDivu, DW, SainW, ins7, DL, DR, Cd, 7, GRL, SRL, 144)
Oper (Op8ed8, DoDivu, DW, SaipW, ins7, DL, DR, Cd, 7, GRL, SRL, 144)
Oper (Op8ee0, DoDivu, DW, SmaiW, ins7, DL, DR, Cd, 7, GRL, SRL, 148)
Oper (Op8ee8, DoDivu, DW, SdaiW, ins7, DL, DR, Cd, 7, GRL, SRL, 148)
Oper (Op8ef0, DoDivu, DW, SaixW, ins7, DL, DR, Cd, 7, GRL, SRL, 152)
Oper (Op8ef8, DoDivu, DW, SearW, ins7, DL, DR, Cd, 7, GRL, SRL, (cycles+147)&0xfffffffc)

Oper (Op8fc0, DoDivs, DW, SdW, ins7, DL, DR, Cd, 7, GRL, SRL, 160)
Oper (Op8fd0, DoDivs, DW, SainW, ins7, DL, DR, Cd, 7, GRL, SRL, 164)
Oper (Op8fd8, DoDivs, DW, SaipW, ins7, DL, DR, Cd, 7, GRL, SRL, 164)
Oper (Op8fe0, DoDivs, DW, SmaiW, ins7, DL, DR, Cd, 7, GRL, SRL, 164)
Oper (Op8fe8, DoDivs, DW, SdaiW, ins7, DL, DR, Cd, 7, GRL, SRL, 168)
Oper (Op8ff0, DoDivs, DW, SaixW, ins7, DL, DR, Cd, 7, GRL, SRL, 168)
Oper (Op8ff8, DoDivs, DW, SearW, ins7, DL, DR, Cd, 7, GRL, SRL, (cycles+165)&0xfffffffc)

Oper (Op0c00, DoCmpB, DB, SimmB, 0, DB, DR, Cd, ins7, GRB, SN, 8)
Oper (Op0c10, DoCmpB, DB, SimmB, 0, DB, DA, Cain, ins7, GMB, SN, 12)
Oper (Op0c18, DoCmpB, DB, SimmB, 0, DB, DA, CaipB, ins7, GMB, SN, 12)
Oper (Op0c20, DoCmpB, DB, SimmB, 0, DB, DA, CmaiB, ins7, GMB, SN, 16)
Oper (Op0c28, DoCmpB, DB, SimmB, 0, DB, DA, Cdai, ins7, GMB, SN, 16)
Oper (Op0c30, DoCmpB, DB, SimmB, 0, DB, DA, Caix, ins7, GMB, SN, 20)
Oper (Op0c38, DoCmpB, DB, SimmB, 0, DB, DA, Ceaw, ins7, GMB, SN, (cycles+19)&0xfffffffc)
Oper (Op0c40, DoCmpW, DW, SimmW, 0, DW, DR, Cd, ins7, GRW, SN, 8)
Oper (Op0c50, DoCmpW, DW, SimmW, 0, DW, DA, Cain, ins7, GMW, SN, 12)
Oper (Op0c58, DoCmpW, DW, SimmW, 0, DW, DA, CaipW, ins7, GMW, SN, 12)
Oper (Op0c60, DoCmpW, DW, SimmW, 0, DW, DA, CmaiW, ins7, GMW, SN, 16)
Oper (Op0c68, DoCmpW, DW, SimmW, 0, DW, DA, Cdai, ins7, GMW, SN, 16)
Oper (Op0c70, DoCmpW, DW, SimmW, 0, DW, DA, Caix, ins7, GMW, SN, 20)
Oper (Op0c78, DoCmpW, DW, SimmW, 0, DW, DA, Ceaw, ins7, GMW, SN, (cycles+19)&0xfffffffc)
Oper (Op0c80, DoCmpL, DL, SimmL, 0, DL, DR, Cd, ins7, GRL, SN, 16)
Oper (Op0c90, DoCmpL, DL, SimmL, 0, DL, DA, Cain, ins7, GML, SN, 20)
Oper (Op0c98, DoCmpL, DL, SimmL, 0, DL, DA, CaipL, ins7, GML, SN, 20)
Oper (Op0ca0, DoCmpL, DL, SimmL, 0, DL, DA, CmaiL, ins7, GML, SN, 24)
Oper (Op0ca8, DoCmpL, DL, SimmL, 0, DL, DA, Cdai, ins7, GML, SN, 24)
Oper (Op0cb0, DoCmpL, DL, SimmL, 0, DL, DA, Caix, ins7, GML, SN, 28)
Oper (Op0cb8, DoCmpL, DL, SimmL, 0, DL, DA, Ceaw, ins7, GML, SN, (cycles+27)&0xfffffffc)

Oper (Opc100, DoABCD, DB, SdB,   ins7, DB, DR, Cd, 0, GRB, SRB, 8)
Oper (Opc108, DoABCD, DB, SmaiB, ins7, DB, DA, CmaiB, 0, GMB, SMB, 20)
Oper (Opc300, DoABCD, DB, SdB,   ins7, DB, DR, Cd, 1, GRB, SRB, 8)
Oper (Opc308, DoABCD, DB, SmaiB, ins7, DB, DA, CmaiB, 1, GMB, SMB, 20)
Oper (Opc500, DoABCD, DB, SdB,   ins7, DB, DR, Cd, 2, GRB, SRB, 8)
Oper (Opc508, DoABCD, DB, SmaiB, ins7, DB, DA, CmaiB, 2, GMB, SMB, 20)
Oper (Opc700, DoABCD, DB, SdB,   ins7, DB, DR, Cd, 3, GRB, SRB, 8)
Oper (Opc708, DoABCD, DB, SmaiB, ins7, DB, DA, CmaiB, 3, GMB, SMB, 20)
Oper (Opc900, DoABCD, DB, SdB,   ins7, DB, DR, Cd, 4, GRB, SRB, 8)
Oper (Opc908, DoABCD, DB, SmaiB, ins7, DB, DA, CmaiB, 4, GMB, SMB, 20)
Oper (Opcb00, DoABCD, DB, SdB,   ins7, DB, DR, Cd, 5, GRB, SRB, 8)
Oper (Opcb08, DoABCD, DB, SmaiB, ins7, DB, DA, CmaiB, 5, GMB, SMB, 20)
Oper (Opcd00, DoABCD, DB, SdB,   ins7, DB, DR, Cd, 6, GRB, SRB, 8)
Oper (Opcd08, DoABCD, DB, SmaiB, ins7, DB, DA, CmaiB, 6, GMB, SMB, 20)
Oper (Opcf00, DoABCD, DB, SdB,   ins7, DB, DR, Cd, 7, GRB, SRB, 8)
Oper (Opcf08, DoABCD, DB, SmaiB, ins7, DB, DA, CmaiB, 7, GMB, SMB, 20)

Oper (Op8100, DoSBCD, DB, SdB,   ins7, DB, DR, Cd, 0, GRB, SRB, 8)
Oper (Op8108, DoSBCD, DB, SmaiB, ins7, DB, DA, CmaiB, 0, GMB, SMB, 20)
Oper (Op8300, DoSBCD, DB, SdB,   ins7, DB, DR, Cd, 1, GRB, SRB, 8)
Oper (Op8308, DoSBCD, DB, SmaiB, ins7, DB, DA, CmaiB, 1, GMB, SMB, 20)
Oper (Op8500, DoSBCD, DB, SdB,   ins7, DB, DR, Cd, 2, GRB, SRB, 8)
Oper (Op8508, DoSBCD, DB, SmaiB, ins7, DB, DA, CmaiB, 2, GMB, SMB, 20)
Oper (Op8700, DoSBCD, DB, SdB,   ins7, DB, DR, Cd, 3, GRB, SRB, 8)
Oper (Op8708, DoSBCD, DB, SmaiB, ins7, DB, DA, CmaiB, 3, GMB, SMB, 20)
Oper (Op8900, DoSBCD, DB, SdB,   ins7, DB, DR, Cd, 4, GRB, SRB, 8)
Oper (Op8908, DoSBCD, DB, SmaiB, ins7, DB, DA, CmaiB, 4, GMB, SMB, 20)
Oper (Op8b00, DoSBCD, DB, SdB,   ins7, DB, DR, Cd, 5, GRB, SRB, 8)
Oper (Op8b08, DoSBCD, DB, SmaiB, ins7, DB, DA, CmaiB, 5, GMB, SMB, 20)
Oper (Op8d00, DoSBCD, DB, SdB,   ins7, DB, DR, Cd, 6, GRB, SRB, 8)
Oper (Op8d08, DoSBCD, DB, SmaiB, ins7, DB, DA, CmaiB, 6, GMB, SMB, 20)
Oper (Op8f00, DoSBCD, DB, SdB,   ins7, DB, DR, Cd, 7, GRB, SRB, 8)
Oper (Op8f08, DoSBCD, DB, SmaiB, ins7, DB, DA, CmaiB, 7, GMB, SMB, 20)

Oper (Op4800, DoNBCD, DN, SNN, 0, DB, DR, Cd, ins7, GRB, SRB, 8)
Oper (Op4810, DoNBCD, DN, SNN, 0, DB, DA, Cain, ins7, GMB, SMB, 12)
Oper (Op4818, DoNBCD, DN, SNN, 0, DB, DA, CaipB, ins7, GMB, SMB, 12)
Oper (Op4820, DoNBCD, DN, SNN, 0, DB, DA, CmaiB, ins7, GMB, SMB, 16)
Oper (Op4828, DoNBCD, DN, SNN, 0, DB, DA, Fdai, ins7, GMB, SMB, 16)
Oper (Op4830, DoNBCD, DN, SNN, 0, DB, DA, Faix, ins7, GMB, SMB, 20)
Oper (Op4838, DoNBCD, DN, SNN, 0, DB, DA, Feaw, ins7, GMB, SMB, (cycles+19)&0xfffffffc)
