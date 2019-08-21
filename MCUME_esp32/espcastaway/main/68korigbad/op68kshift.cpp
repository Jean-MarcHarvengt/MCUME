/*
* Castaway
*  (C) 1994 - 2002 Martin Doering, Joachim Hoenig
*
* $File$ - 68k shift instructions
*
* This file is distributed under the GPL, version 2 or at your
* option any later version.  See doc/license.txt for details.
*
* revision history
*  23.05.2002  JH  FAST1.0.1 code import: KR -> ANSI, restructuring
*  22.06.2002  JH  V-flag after ASL was broken in some special cases.
*  09.07.2002  JH  ROXL/ROXR: X-flag not modified when shift count is 0.
*  10.07.2002  JH  other shift ops: X-flag not modified when shift count is 0.
*  10.09.2002  JH  Bugfix: ROXL.L
*  15.09.2002  JH  Minor SR/Bcc/Scc optimization. Fixed V-Flag for ASL.L.
*                  Wrapped 68k operand types in typedefs.
*  01.10.2002  JH  Bugfix: -(Ax) and (Ax)+ addressing modes were swapped.
*                  *** Many thx to olivencia@wanado.fr for finding this!! ***
*  02.10.2002  JH  SetV simplified for ASL.
*/
#ifndef PROTOH
static char     sccsid[] = "$Id: op68kshift.c,v 1.10 2002/10/05 08:25:08 jhoenig Exp $";
#include "68000.h"
#include "op68k.h"

#define addcycles cycles+=source<<1;

#define DoAslB(target,source) \
	source&=0x3f; \
	addcycles \
    if (source> 0) {\
	register uint32 cvnz=0;\
	register uint32 tgt=target<<source;\
	if (tgt&0x100) {cvnz+=Cflag; ForceX(1);} else ForceX(0);\
	if ((tgt&0x80)!=(target&0x80)) cvnz+=Vflag;\
	target=tgt;\
	if (target==0) cvnz+=Zflag;\
	else if (target<0) cvnz+=Nflag;\
	ForceCVNZ(cvnz);\
    } else {\
	ClrCVSetNZ(target);\
}

#define DoAslW(target,source) \
	source&=0x3f; \
	addcycles \
    if (source> 0) {\
	register uint32 cvnz=0;\
	register uint32 tgt=target<<source;\
	if (tgt&0x10000) {cvnz+=Cflag; ForceX(1);} else ForceX(0);\
	if ((tgt&0x8000)!=(target&0x8000)) cvnz+=Vflag;\
	target=tgt;\
	if (target==0) cvnz+=Zflag;\
	else if (target<0) cvnz+=Nflag;\
	ForceCVNZ(cvnz);\
    } else {\
	ClrCVSetNZ(target);\
}

#define DoAslL(target,source) \
	source&=0x3f; \
	addcycles \
    if (source>0) {\
	register uint32 cvnz=0;\
	register uint32 tgt=target<<(source-1);\
	if (tgt&0x80000000) {cvnz+=Cflag; ForceX(1);} else ForceX(0);\
	tgt<<=1;\
	if ((((uint32)target)>>31)!=(tgt>>31)) cvnz+=Vflag;\
	target=tgt;\
	if (target==0) cvnz+=Zflag;\
	else if (target<0) cvnz+=Nflag;\
	ForceCVNZ(cvnz);\
    } else {\
	ClrCVSetNZ(target);\
}

#define DoAsr(target,source) \
	source&=0x3f; \
	addcycles \
    if (source>0) {\
	register uint32 cvnz=0;\
	target >>= source - 1;\
	if (target & 0x1) {cvnz+=Cflag; ForceX(1);} else ForceX(0);\
	target >>= 1;\
	if (target==0) cvnz+=Zflag;\
	else if (target<0) cvnz+=Nflag;\
	ForceCVNZ(cvnz);\
    } else {\
	ClrCVSetNZ(target);\
}

#define DoAsrB(target,source) DoAsr(target,source)
#define DoAsrW(target,source) DoAsr(target,source)
#define DoAsrL(target,source) DoAsr(target,source)

#define DoLslB(target,source) \
	source&=0x3f; \
	addcycles \
    if (source>0) {\
	register uint32 cvnz=0;\
	register uint32 tgt=target<<source;\
	if (target&0x100) {cvnz+=Cflag; ForceX(1);} else ForceX(0);\
	target=tgt;\
	if (target==0) cvnz+=Zflag;\
	else if (target<0) cvnz+=Nflag;\
	ForceCVNZ(cvnz);\
    } else {\
	ClrCVSetNZ(target);\
}

#define DoLslW(target,source) \
	source&=0x3f; \
	addcycles \
    if (source>0) {\
	register uint32 cvnz=0;\
	register uint32 tgt=target<<source;\
	if (target&0x10000) {cvnz+=Cflag; ForceX(1);} else ForceX(0);\
	target=tgt;\
	if (target==0) cvnz+=Zflag;\
	else if (target<0) cvnz+=Nflag;\
	ForceCVNZ(cvnz);\
    } else {\
	ClrCVSetNZ(target);\
    }

#define DoLslL(target,source) \
	source&=0x3f; \
	addcycles \
    if (source> 0) {\
	register uint32 cvnz=0;\
	target <<= source - 1;\
	if (target & 0x80000000) {cvnz+=Cflag; ForceX(1);} else ForceX(0);\
	target <<= 1;\
	if (target==0) cvnz+=Zflag;\
	else if (target<0) cvnz+=Nflag;\
	ForceCVNZ(cvnz);\
    } else {\
	ClrCVSetNZ(target);\
    }

#define DoLsr(target,source,type) \
	source&=0x3f; \
	addcycles \
    if (source> 0) {\
	register uint32 cvnz=0;\
	if (target&(1<<(source-1))) {cvnz+=Cflag; ForceX(1);} else ForceX(0);\
	target=((type)target)>>source;\
	if (target==0) cvnz+=Zflag;\
	else if (target<0) cvnz+=Nflag;\
	ForceCVNZ(cvnz);\
    } else {\
	ClrCVSetNZ(target);\
    }

#define DoLsrB(target,source) DoLsr(target,source,uint8)
#define DoLsrW(target,source) DoLsr(target,source,uint16)
#define DoLsrL(target,source) DoLsr(target,source,uint32)


#define DoRoxlB(target,source) {\
    source &= 0x3f;\
	addcycles \
    ForceV (0);\
    if (source == 0) {\
	SetC(GetX());\
    } else {\
	register uint16 tgt1, tgt2;\
	source %= 9;\
	tgt1 = (uint8) target;\
	tgt1 |= (uint16) (GetX()) << 8;\
	tgt2 = tgt1;\
	tgt2 >>= (9 - source);\
	tgt1 <<= source;\
	tgt1 |= tgt2;\
	SetXC ((tgt1 >> 8) & 0x1);\
	target = tgt1;\
    }\
    SetNZ (target);\
}

#define DoRoxlW(target,source) {\
    source &= 0x3f;\
	addcycles \
    ForceV (0);\
    if (source == 0) {\
	SetC(GetX());\
    } else {\
	register uint32 tgt1, tgt2;\
	source %= 17;\
	tgt1 = (uint16) target;\
	tgt1 |= (uint32) (GetX()) << 16;\
	tgt2 = tgt1;\
	tgt2 >>= (17 - source);\
	tgt1 <<= source;\
	tgt1 |= tgt2;\
	SetXC ((tgt1 >> 16) & 0x1);\
	target = tgt1;\
    }\
    SetNZ (target);\
}

#define DoRoxlL(target,source) {\
    source &= 0x3f;\
	addcycles \
    ForceV (0);\
    if (source == 0) {\
	SetC(GetX());\
    } else {\
	register uint32 tgt1, tgt2;\
	source %= 33;\
	tgt2 = tgt1 = (uint32) target;\
	if (source != 0) {\
	tgt1 <<= 1;\
	tgt1 |= (uint32) (GetX());\
	tgt1 <<= (source - 1);\
	tgt2 >>= (32 - source);\
	SetXC (tgt2 & 0x1);\
	tgt2 >>= 1;\
	tgt1 |= tgt2;\
	target = tgt1;\
	}\
    }\
    SetNZ (target);\
}

#define DoRoxrB(target,source) {\
    source &= 0x3f;\
	addcycles \
    ForceV (0);\
    if (source == 0) {\
	SetC(GetX());\
    } else {\
	register uint16 tgt1, tgt2;\
	source %= 9;\
	tgt1 = (uint8) target;\
	tgt1 |= (uint16) (GetX()) << 8;\
	tgt2 = tgt1;\
	tgt2 <<= (9 - source);\
	tgt1 >>= source;\
	tgt1 |= tgt2;\
	SetXC ((tgt1 >> 8) & 0x1);\
	target = tgt1;\
    }\
    SetNZ (target);\
}

#define DoRoxrW(target,source) {\
    source &= 0x3f;\
	addcycles \
    ForceV (0);\
    if (source == 0) {\
	SetC(GetX());\
    } else {\
	register uint32 tgt1, tgt2;\
	source %= 17;\
	tgt1 = (uint16) target;\
	tgt1 |= (uint32) (GetX()) << 16;\
	tgt2 = tgt1;\
	tgt2 <<= (17 - source);\
	tgt1 >>= source;\
	tgt1 |= tgt2;\
	SetXC ((tgt1 >> 16) & 0x1);\
	target = tgt1;\
    }\
    SetNZ (target);\
}

#define DoRoxrL(target,source) {\
    source &= 0x3f;\
	addcycles \
    ForceV (0);\
    if (source == 0) {\
	SetC(GetX());\
    } else {\
	register uint32 tgt1, tgt2;\
	source %= 33;\
	tgt2 = tgt1 = (uint32) target;\
	if (source != 0) {\
	tgt2 <<= 1;\
	tgt2 |= (uint32) (GetX());\
	tgt2 <<= (32 - source);\
	tgt1 >>= (source - 1);\
	SetXC (tgt1 & 0x1);\
	tgt1 >>= 1;\
	tgt1 |= tgt2;\
	target = tgt1;\
	}\
    }\
    SetNZ (target);\
}

#define DoRolB(target,source) {\
    source &= 0x3f;\
	addcycles \
    ForceV (0);\
    if (source == 0) {\
	ForceC (0);\
    } else {\
	register uint8 tgt1, tgt2;\
	source &= 0x7;\
	tgt2 = tgt1 = target;\
	tgt2 >>= (8 - source);\
	tgt1 <<= source;\
	tgt1 |= tgt2;\
	SetC (tgt1 & 0x1);\
	target = tgt1;\
    }\
    SetNZ (target);\
}

#define DoRolW(target,source) {\
    source &= 0x3f;\
	addcycles \
    ForceV (0);\
    if (source == 0) {\
	ForceC (0);\
    } else {\
	register uint16 tgt1, tgt2;\
	source &= 0xf;\
	tgt2 = tgt1 = target;\
	tgt2 >>= (16 - source);\
	tgt1 <<= source;\
	tgt1 |= tgt2;\
	SetC (tgt1 & 0x1);\
	target = tgt1;\
    }\
    SetNZ (target);\
}

#define DoRolL(target,source) {\
    source &= 0x3f;\
	addcycles \
    ForceV (0);\
    if (source == 0) {\
	ForceC (0);\
    } else {\
	register uint32 tgt1, tgt2;\
	source &= 0x1f;\
	tgt2 = tgt1 = (uint32) target;\
	tgt2 >>= (32 - source);\
	tgt1 <<= source;\
	tgt1 |= tgt2;\
	SetC (tgt1 & 0x1);\
	target = tgt1;\
    }\
    SetNZ (target);\
}

#define DoRorB(target,source) {\
    source &= 0x3f;\
	addcycles \
    ForceV (0);\
    if (source == 0) {\
	ForceC (0);\
    } else {\
	register uint8 tgt1, tgt2;\
	source &= 0x7;\
	tgt2 = tgt1 = target;\
	tgt2 <<= (8 - source);\
	tgt1 >>= source;\
	tgt1 |= tgt2;\
	SetC ((tgt1 >> 7) & 0x1);\
	target = tgt1;\
    }\
    SetNZ (target);\
}

#define DoRorW(target,source) {\
    source &= 0x3f;\
	addcycles \
    ForceV (0);\
    if (source == 0) {\
	ForceC (0);\
    } else {\
	register uint16 tgt1, tgt2;\
	source &= 0xf;\
	tgt2 = tgt1 = target;\
	tgt2 <<= (16 - source);\
	tgt1 >>= source;\
	tgt1 |= tgt2;\
	SetC ((tgt1 >> 15) & 0x1);\
	target = tgt1;\
    }\
    SetNZ (target);\
}

#define DoRorL(target,source) {\
    source &= 0x3f;\
	addcycles \
    ForceV (0);\
    if (source == 0) {\
	ForceC (0);\
    } else {\
	register uint32 tgt1, tgt2;\
	source &= 0x1f;\
	tgt2 = tgt1 = target;\
	tgt2 <<= (32 - source);\
	tgt1 >>= source;\
	tgt1 |= tgt2;\
	SetC ((tgt1 >> 31) & 0x1);\
	target = tgt1;\
    }\
    SetNZ (target);\
}

#endif

Oper (Ope000, DoAsrB, DB, Ss, 8, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope008, DoLsrB, DB, Ss, 8, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope010, DoRoxrB, DB, Ss, 8, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope018, DoRorB, DB, Ss, 8, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope020, DoAsrB, DB, SdB, 0, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope028, DoLsrB, DB, SdB, 0, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope030, DoRoxrB, DB, SdB, 0, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope038, DoRorB, DB, SdB, 0, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope040, DoAsrW, DW, Ss, 8, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope048, DoLsrW, DW, Ss, 8, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope050, DoRoxrW, DW, Ss, 8, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope058, DoRorW, DW, Ss, 8, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope060, DoAsrW, DW, SdW, 0, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope068, DoLsrW, DW, SdW, 0, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope070, DoRoxrW, DW, SdW, 0, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope078, DoRorW, DW, SdW, 0, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope080, DoAsrL, DL, Ss, 8, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope088, DoLsrL, DL, Ss, 8, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope090, DoRoxrL, DL, Ss, 8, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope098, DoRorL, DL, Ss, 8, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope0a0, DoAsrL, DL, SdL, 0, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope0a8, DoLsrL, DL, SdL, 0, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope0b0, DoRoxrL, DL, SdL, 0, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope0b8, DoRorL, DL, SdL, 0, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)

Oper (Ope100, DoAslB, DB, Ss, 8, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope108, DoLslB, DB, Ss, 8, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope110, DoRoxlB, DB, Ss, 8, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope118, DoRolB, DB, Ss, 8, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope120, DoAslB, DB, SdB, 0, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope128, DoLslB, DB, SdB, 0, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope130, DoRoxlB, DB, SdB, 0, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope138, DoRolB, DB, SdB, 0, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope140, DoAslW, DW, Ss, 8, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope148, DoLslW, DW, Ss, 8, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope150, DoRoxlW, DW, Ss, 8, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope158, DoRolW, DW, Ss, 8, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope160, DoAslW, DW, SdW, 0, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope168, DoLslW, DW, SdW, 0, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope170, DoRoxlW, DW, SdW, 0, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope178, DoRolW, DW, SdW, 0, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope180, DoAslL, DL, Ss, 8, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope188, DoLslL, DL, Ss, 8, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope190, DoRoxlL, DL, Ss, 8, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope198, DoRolL, DL, Ss, 8, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope1a0, DoAslL, DL, SdL, 0, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope1a8, DoLslL, DL, SdL, 0, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope1b0, DoRoxlL, DL, SdL, 0, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope1b8, DoRolL, DL, SdL, 0, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)

Oper (Ope200, DoAsrB, DB, Ss, 1, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope208, DoLsrB, DB, Ss, 1, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope210, DoRoxrB, DB, Ss, 1, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope218, DoRorB, DB, Ss, 1, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope220, DoAsrB, DB, SdB, 1, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope228, DoLsrB, DB, SdB, 1, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope230, DoRoxrB, DB, SdB, 1, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope238, DoRorB, DB, SdB, 1, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope240, DoAsrW, DW, Ss, 1, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope248, DoLsrW, DW, Ss, 1, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope250, DoRoxrW, DW, Ss, 1, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope258, DoRorW, DW, Ss, 1, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope260, DoAsrW, DW, SdW, 1, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope268, DoLsrW, DW, SdW, 1, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope270, DoRoxrW, DW, SdW, 1, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope278, DoRorW, DW, SdW, 1, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope280, DoAsrL, DL, Ss, 1, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope288, DoLsrL, DL, Ss, 1, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope290, DoRoxrL, DL, Ss, 1, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope298, DoRorL, DL, Ss, 1, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope2a0, DoAsrL, DL, SdL, 1, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope2a8, DoLsrL, DL, SdL, 1, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope2b0, DoRoxrL, DL, SdL, 1, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope2b8, DoRorL, DL, SdL, 1, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)

Oper (Ope300, DoAslB, DB, Ss, 1, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope308, DoLslB, DB, Ss, 1, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope310, DoRoxlB, DB, Ss, 1, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope318, DoRolB, DB, Ss, 1, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope320, DoAslB, DB, SdB, 1, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope328, DoLslB, DB, SdB, 1, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope330, DoRoxlB, DB, SdB, 1, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope338, DoRolB, DB, SdB, 1, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope340, DoAslW, DW, Ss, 1, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope348, DoLslW, DW, Ss, 1, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope350, DoRoxlW, DW, Ss, 1, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope358, DoRolW, DW, Ss, 1, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope360, DoAslW, DW, SdW, 1, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope368, DoLslW, DW, SdW, 1, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope370, DoRoxlW, DW, SdW, 1, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope378, DoRolW, DW, SdW, 1, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope380, DoAslL, DL, Ss, 1, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope388, DoLslL, DL, Ss, 1, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope390, DoRoxlL, DL, Ss, 1, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope398, DoRolL, DL, Ss, 1, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope3a0, DoAslL, DL, SdL, 1, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope3a8, DoLslL, DL, SdL, 1, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope3b0, DoRoxlL, DL, SdL, 1, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope3b8, DoRolL, DL, SdL, 1, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)

Oper (Ope400, DoAsrB, DB, Ss, 2, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope408, DoLsrB, DB, Ss, 2, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope410, DoRoxrB, DB, Ss, 2, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope418, DoRorB, DB, Ss, 2, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope420, DoAsrB, DB, SdB, 2, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope428, DoLsrB, DB, SdB, 2, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope430, DoRoxrB, DB, SdB, 2, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope438, DoRorB, DB, SdB, 2, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope440, DoAsrW, DW, Ss, 2, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope448, DoLsrW, DW, Ss, 2, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope450, DoRoxrW, DW, Ss, 2, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope458, DoRorW, DW, Ss, 2, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope460, DoAsrW, DW, SdW, 2, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope468, DoLsrW, DW, SdW, 2, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope470, DoRoxrW, DW, SdW, 2, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope478, DoRorW, DW, SdW, 2, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope480, DoAsrL, DL, Ss, 2, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope488, DoLsrL, DL, Ss, 2, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope490, DoRoxrL, DL, Ss, 2, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope498, DoRorL, DL, Ss, 2, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope4a0, DoAsrL, DL, SdL, 2, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope4a8, DoLsrL, DL, SdL, 2, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope4b0, DoRoxrL, DL, SdL, 2, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope4b8, DoRorL, DL, SdL, 2, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)

Oper (Ope500, DoAslB, DB, Ss, 2, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope508, DoLslB, DB, Ss, 2, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope510, DoRoxlB, DB, Ss, 2, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope518, DoRolB, DB, Ss, 2, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope520, DoAslB, DB, SdB, 2, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope528, DoLslB, DB, SdB, 2, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope530, DoRoxlB, DB, SdB, 2, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope538, DoRolB, DB, SdB, 2, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope540, DoAslW, DW, Ss, 2, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope548, DoLslW, DW, Ss, 2, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope550, DoRoxlW, DW, Ss, 2, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope558, DoRolW, DW, Ss, 2, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope560, DoAslW, DW, SdW, 2, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope568, DoLslW, DW, SdW, 2, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope570, DoRoxlW, DW, SdW, 2, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope578, DoRolW, DW, SdW, 2, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope580, DoAslL, DL, Ss, 2, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope588, DoLslL, DL, Ss, 2, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope590, DoRoxlL, DL, Ss, 2, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope598, DoRolL, DL, Ss, 2, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope5a0, DoAslL, DL, SdL, 2, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope5a8, DoLslL, DL, SdL, 2, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope5b0, DoRoxlL, DL, SdL, 2, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope5b8, DoRolL, DL, SdL, 2, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)

Oper (Ope600, DoAsrB, DB, Ss, 3, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope608, DoLsrB, DB, Ss, 3, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope610, DoRoxrB, DB, Ss, 3, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope618, DoRorB, DB, Ss, 3, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope620, DoAsrB, DB, SdB, 3, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope628, DoLsrB, DB, SdB, 3, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope630, DoRoxrB, DB, SdB, 3, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope638, DoRorB, DB, SdB, 3, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope640, DoAsrW, DW, Ss, 3, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope648, DoLsrW, DW, Ss, 3, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope650, DoRoxrW, DW, Ss, 3, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope658, DoRorW, DW, Ss, 3, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope660, DoAsrW, DW, SdW, 3, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope668, DoLsrW, DW, SdW, 3, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope670, DoRoxrW, DW, SdW, 3, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope678, DoRorW, DW, SdW, 3, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope680, DoAsrL, DL, Ss, 3, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope688, DoLsrL, DL, Ss, 3, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope690, DoRoxrL, DL, Ss, 3, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope698, DoRorL, DL, Ss, 3, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope6a0, DoAsrL, DL, SdL, 3, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope6a8, DoLsrL, DL, SdL, 3, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope6b0, DoRoxrL, DL, SdL, 3, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope6b8, DoRorL, DL, SdL, 3, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)

Oper (Ope700, DoAslB, DB, Ss, 3, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope708, DoLslB, DB, Ss, 3, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope710, DoRoxlB, DB, Ss, 3, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope718, DoRolB, DB, Ss, 3, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope720, DoAslB, DB, SdB, 3, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope728, DoLslB, DB, SdB, 3, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope730, DoRoxlB, DB, SdB, 3, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope738, DoRolB, DB, SdB, 3, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope740, DoAslW, DW, Ss, 3, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope748, DoLslW, DW, Ss, 3, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope750, DoRoxlW, DW, Ss, 3, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope758, DoRolW, DW, Ss, 3, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope760, DoAslW, DW, SdW, 3, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope768, DoLslW, DW, SdW, 3, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope770, DoRoxlW, DW, SdW, 3, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope778, DoRolW, DW, SdW, 3, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope780, DoAslL, DL, Ss, 3, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope788, DoLslL, DL, Ss, 3, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope790, DoRoxlL, DL, Ss, 3, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope798, DoRolL, DL, Ss, 3, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope7a0, DoAslL, DL, SdL, 3, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope7a8, DoLslL, DL, SdL, 3, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope7b0, DoRoxlL, DL, SdL, 3, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope7b8, DoRolL, DL, SdL, 3, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)

Oper (Ope800, DoAsrB, DB, Ss, 4, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope808, DoLsrB, DB, Ss, 4, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope810, DoRoxrB, DB, Ss, 4, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope818, DoRorB, DB, Ss, 4, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope820, DoAsrB, DB, SdB, 4, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope828, DoLsrB, DB, SdB, 4, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope830, DoRoxrB, DB, SdB, 4, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope838, DoRorB, DB, SdB, 4, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope840, DoAsrW, DW, Ss, 4, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope848, DoLsrW, DW, Ss, 4, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope850, DoRoxrW, DW, Ss, 4, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope858, DoRorW, DW, Ss, 4, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope860, DoAsrW, DW, SdW, 4, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope868, DoLsrW, DW, SdW, 4, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope870, DoRoxrW, DW, SdW, 4, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope878, DoRorW, DW, SdW, 4, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope880, DoAsrL, DL, Ss, 4, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope888, DoLsrL, DL, Ss, 4, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope890, DoRoxrL, DL, Ss, 4, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope898, DoRorL, DL, Ss, 4, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope8a0, DoAsrL, DL, SdL, 4, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope8a8, DoLsrL, DL, SdL, 4, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope8b0, DoRoxrL, DL, SdL, 4, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope8b8, DoRorL, DL, SdL, 4, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)

Oper (Ope900, DoAslB, DB, Ss, 4, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope908, DoLslB, DB, Ss, 4, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope910, DoRoxlB, DB, Ss, 4, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope918, DoRolB, DB, Ss, 4, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope920, DoAslB, DB, SdB, 4, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope928, DoLslB, DB, SdB, 4, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope930, DoRoxlB, DB, SdB, 4, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope938, DoRolB, DB, SdB, 4, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Ope940, DoAslW, DW, Ss, 4, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope948, DoLslW, DW, Ss, 4, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope950, DoRoxlW, DW, Ss, 4, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope958, DoRolW, DW, Ss, 4, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope960, DoAslW, DW, SdW, 4, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope968, DoLslW, DW, SdW, 4, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope970, DoRoxlW, DW, SdW, 4, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope978, DoRolW, DW, SdW, 4, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Ope980, DoAslL, DL, Ss, 4, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope988, DoLslL, DL, Ss, 4, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope990, DoRoxlL, DL, Ss, 4, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope998, DoRolL, DL, Ss, 4, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope9a0, DoAslL, DL, SdL, 4, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope9a8, DoLslL, DL, SdL, 4, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope9b0, DoRoxlL, DL, SdL, 4, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Ope9b8, DoRolL, DL, SdL, 4, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)

Oper (Opea00, DoAsrB, DB, Ss, 5, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opea08, DoLsrB, DB, Ss, 5, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opea10, DoRoxrB, DB, Ss, 5, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opea18, DoRorB, DB, Ss, 5, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opea20, DoAsrB, DB, SdB, 5, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opea28, DoLsrB, DB, SdB, 5, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opea30, DoRoxrB, DB, SdB, 5, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opea38, DoRorB, DB, SdB, 5, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opea40, DoAsrW, DW, Ss, 5, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opea48, DoLsrW, DW, Ss, 5, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opea50, DoRoxrW, DW, Ss, 5, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opea58, DoRorW, DW, Ss, 5, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opea60, DoAsrW, DW, SdW, 5, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opea68, DoLsrW, DW, SdW, 5, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opea70, DoRoxrW, DW, SdW, 5, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opea78, DoRorW, DW, SdW, 5, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opea80, DoAsrL, DL, Ss, 5, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opea88, DoLsrL, DL, Ss, 5, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opea90, DoRoxrL, DL, Ss, 5, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opea98, DoRorL, DL, Ss, 5, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeaa0, DoAsrL, DL, SdL, 5, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeaa8, DoLsrL, DL, SdL, 5, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeab0, DoRoxrL, DL, SdL, 5, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeab8, DoRorL, DL, SdL, 5, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)

Oper (Opeb00, DoAslB, DB, Ss, 5, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opeb08, DoLslB, DB, Ss, 5, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opeb10, DoRoxlB, DB, Ss, 5, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opeb18, DoRolB, DB, Ss, 5, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opeb20, DoAslB, DB, SdB, 5, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opeb28, DoLslB, DB, SdB, 5, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opeb30, DoRoxlB, DB, SdB, 5, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opeb38, DoRolB, DB, SdB, 5, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opeb40, DoAslW, DW, Ss, 5, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opeb48, DoLslW, DW, Ss, 5, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opeb50, DoRoxlW, DW, Ss, 5, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opeb58, DoRolW, DW, Ss, 5, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opeb60, DoAslW, DW, SdW, 5, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opeb68, DoLslW, DW, SdW, 5, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opeb70, DoRoxlW, DW, SdW, 5, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opeb78, DoRolW, DW, SdW, 5, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opeb80, DoAslL, DL, Ss, 5, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeb88, DoLslL, DL, Ss, 5, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeb90, DoRoxlL, DL, Ss, 5, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeb98, DoRolL, DL, Ss, 5, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeba0, DoAslL, DL, SdL, 5, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeba8, DoLslL, DL, SdL, 5, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opebb0, DoRoxlL, DL, SdL, 5, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opebb8, DoRolL, DL, SdL, 5, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)

Oper (Opec00, DoAsrB, DB, Ss, 6, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opec08, DoLsrB, DB, Ss, 6, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opec10, DoRoxrB, DB, Ss, 6, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opec18, DoRorB, DB, Ss, 6, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opec20, DoAsrB, DB, SdB, 6, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opec28, DoLsrB, DB, SdB, 6, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opec30, DoRoxrB, DB, SdB, 6, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opec38, DoRorB, DB, SdB, 6, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opec40, DoAsrW, DW, Ss, 6, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opec48, DoLsrW, DW, Ss, 6, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opec50, DoRoxrW, DW, Ss, 6, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opec58, DoRorW, DW, Ss, 6, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opec60, DoAsrW, DW, SdW, 6, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opec68, DoLsrW, DW, SdW, 6, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opec70, DoRoxrW, DW, SdW, 6, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opec78, DoRorW, DW, SdW, 6, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opec80, DoAsrL, DL, Ss, 6, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opec88, DoLsrL, DL, Ss, 6, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opec90, DoRoxrL, DL, Ss, 6, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opec98, DoRorL, DL, Ss, 6, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeca0, DoAsrL, DL, SdL, 6, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeca8, DoLsrL, DL, SdL, 6, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opecb0, DoRoxrL, DL, SdL, 6, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opecb8, DoRorL, DL, SdL, 6, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)

Oper (Oped00, DoAslB, DB, Ss, 6, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Oped08, DoLslB, DB, Ss, 6, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Oped10, DoRoxlB, DB, Ss, 6, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Oped18, DoRolB, DB, Ss, 6, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Oped20, DoAslB, DB, SdB, 6, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Oped28, DoLslB, DB, SdB, 6, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Oped30, DoRoxlB, DB, SdB, 6, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Oped38, DoRolB, DB, SdB, 6, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Oped40, DoAslW, DW, Ss, 6, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Oped48, DoLslW, DW, Ss, 6, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Oped50, DoRoxlW, DW, Ss, 6, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Oped58, DoRolW, DW, Ss, 6, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Oped60, DoAslW, DW, SdW, 6, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Oped68, DoLslW, DW, SdW, 6, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Oped70, DoRoxlW, DW, SdW, 6, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Oped78, DoRolW, DW, SdW, 6, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Oped80, DoAslL, DL, Ss, 6, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Oped88, DoLslL, DL, Ss, 6, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Oped90, DoRoxlL, DL, Ss, 6, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Oped98, DoRolL, DL, Ss, 6, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeda0, DoAslL, DL, SdL, 6, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeda8, DoLslL, DL, SdL, 6, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opedb0, DoRoxlL, DL, SdL, 6, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opedb8, DoRolL, DL, SdL, 6, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)

Oper (Opee00, DoAsrB, DB, Ss, 7, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opee08, DoLsrB, DB, Ss, 7, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opee10, DoRoxrB, DB, Ss, 7, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opee18, DoRorB, DB, Ss, 7, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opee20, DoAsrB, DB, SdB, 7, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opee28, DoLsrB, DB, SdB, 7, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opee30, DoRoxrB, DB, SdB, 7, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opee38, DoRorB, DB, SdB, 7, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opee40, DoAsrW, DW, Ss, 7, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opee48, DoLsrW, DW, Ss, 7, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opee50, DoRoxrW, DW, Ss, 7, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opee58, DoRorW, DW, Ss, 7, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opee60, DoAsrW, DW, SdW, 7, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opee68, DoLsrW, DW, SdW, 7, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opee70, DoRoxrW, DW, SdW, 7, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opee78, DoRorW, DW, SdW, 7, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opee80, DoAsrL, DL, Ss, 7, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opee88, DoLsrL, DL, Ss, 7, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opee90, DoRoxrL, DL, Ss, 7, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opee98, DoRorL, DL, Ss, 7, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeea0, DoAsrL, DL, SdL, 7, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeea8, DoLsrL, DL, SdL, 7, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeeb0, DoRoxrL, DL, SdL, 7, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opeeb8, DoRorL, DL, SdL, 7, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)

Oper (Opef00, DoAslB, DB, Ss, 7, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opef08, DoLslB, DB, Ss, 7, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opef10, DoRoxlB, DB, Ss, 7, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opef18, DoRolB, DB, Ss, 7, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opef20, DoAslB, DB, SdB, 7, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opef28, DoLslB, DB, SdB, 7, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opef30, DoRoxlB, DB, SdB, 7, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opef38, DoRolB, DB, SdB, 7, DB, DR, Cd, ins7, GRB, SRB, (cycles+9)&0xfffffffc)
Oper (Opef40, DoAslW, DW, Ss, 7, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opef48, DoLslW, DW, Ss, 7, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opef50, DoRoxlW, DW, Ss, 7, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opef58, DoRolW, DW, Ss, 7, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opef60, DoAslW, DW, SdW, 7, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opef68, DoLslW, DW, SdW, 7, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opef70, DoRoxlW, DW, SdW, 7, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opef78, DoRolW, DW, SdW, 7, DW, DR, Cd, ins7, GRW, SRW, (cycles+9)&0xfffffffc)
Oper (Opef80, DoAslL, DL, Ss, 7, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opef88, DoLslL, DL, Ss, 7, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opef90, DoRoxlL, DL, Ss, 7, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opef98, DoRolL, DL, Ss, 7, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opefa0, DoAslL, DL, SdL, 7, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opefa8, DoLslL, DL, SdL, 7, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opefb0, DoRoxlL, DL, SdL, 7, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)
Oper (Opefb8, DoRolL, DL, SdL, 7, DL, DR, Cd, ins7, GRL, SRL, (cycles+11)&0xfffffffc)

Oper (Ope0d0, DoAsrW, DW, Ss, 1, DW, DA, Cain, ins7, GMW, SMW, 12)
Oper (Ope0d8, DoAsrW, DW, Ss, 1, DW, DA, CaipW, ins7, GMW, SMW, 12)
Oper (Ope0e0, DoAsrW, DW, Ss, 1, DW, DA, CmaiW, ins7, GMW, SMW, 16)
Oper (Ope0e8, DoAsrW, DW, Ss, 1, DW, DA, Fdai, ins7, GMW, SMW, 16)
Oper (Ope0f0, DoAsrW, DW, Ss, 1, DW, DA, Faix, ins7, GMW, SMW, 20)
Oper (Ope0f8, DoAsrW, DW, Ss, 1, DW, DA, Feaw, ins7, GMW, SMW, (cycles+19)&0xfffffffc)

Oper (Ope1d0, DoAslW, DW, Ss, 1, DW, DA, Cain, ins7, GMW, SMW, 12)
Oper (Ope1d8, DoAslW, DW, Ss, 1, DW, DA, CaipW, ins7, GMW, SMW, 12)
Oper (Ope1e0, DoAslW, DW, Ss, 1, DW, DA, CmaiW, ins7, GMW, SMW, 16)
Oper (Ope1e8, DoAslW, DW, Ss, 1, DW, DA, Fdai, ins7, GMW, SMW, 16)
Oper (Ope1f0, DoAslW, DW, Ss, 1, DW, DA, Faix, ins7, GMW, SMW, 20)
Oper (Ope1f8, DoAslW, DW, Ss, 1, DW, DA, Feaw, ins7, GMW, SMW, (cycles+19)&0xfffffffc)

Oper (Ope2d0, DoLsrW, DW, Ss, 1, DW, DA, Cain, ins7, GMW, SMW, 12)
Oper (Ope2d8, DoLsrW, DW, Ss, 1, DW, DA, CaipW, ins7, GMW, SMW, 12)
Oper (Ope2e0, DoLsrW, DW, Ss, 1, DW, DA, CmaiW, ins7, GMW, SMW, 16)
Oper (Ope2e8, DoLsrW, DW, Ss, 1, DW, DA, Fdai, ins7, GMW, SMW, 16)
Oper (Ope2f0, DoLsrW, DW, Ss, 1, DW, DA, Faix, ins7, GMW, SMW, 20)
Oper (Ope2f8, DoLsrW, DW, Ss, 1, DW, DA, Feaw, ins7, GMW, SMW, (cycles+19)&0xfffffffc)

Oper (Ope3d0, DoLslW, DW, Ss, 1, DW, DA, Cain, ins7, GMW, SMW, 12)
Oper (Ope3d8, DoLslW, DW, Ss, 1, DW, DA, CaipW, ins7, GMW, SMW, 12)
Oper (Ope3e0, DoLslW, DW, Ss, 1, DW, DA, CmaiW, ins7, GMW, SMW, 16)
Oper (Ope3e8, DoLslW, DW, Ss, 1, DW, DA, Fdai, ins7, GMW, SMW, 16)
Oper (Ope3f0, DoLslW, DW, Ss, 1, DW, DA, Faix, ins7, GMW, SMW, 20)
Oper (Ope3f8, DoLslW, DW, Ss, 1, DW, DA, Feaw, ins7, GMW, SMW, (cycles+19)&0xfffffffc)

Oper (Ope4d0, DoRoxrW, DW, Ss, 1, DW, DA, Cain, ins7, GMW, SMW, 12)
Oper (Ope4d8, DoRoxrW, DW, Ss, 1, DW, DA, CaipW, ins7, GMW, SMW, 12)
Oper (Ope4e0, DoRoxrW, DW, Ss, 1, DW, DA, CmaiW, ins7, GMW, SMW, 16)
Oper (Ope4e8, DoRoxrW, DW, Ss, 1, DW, DA, Fdai, ins7, GMW, SMW, 16)
Oper (Ope4f0, DoRoxrW, DW, Ss, 1, DW, DA, Faix, ins7, GMW, SMW, 20)
Oper (Ope4f8, DoRoxrW, DW, Ss, 1, DW, DA, Feaw, ins7, GMW, SMW, (cycles+19)&0xfffffffc)

Oper (Ope5d0, DoRoxlW, DW, Ss, 1, DW, DA, Cain, ins7, GMW, SMW, 12)
Oper (Ope5d8, DoRoxlW, DW, Ss, 1, DW, DA, CaipW, ins7, GMW, SMW, 12)
Oper (Ope5e0, DoRoxlW, DW, Ss, 1, DW, DA, CmaiW, ins7, GMW, SMW, 16)
Oper (Ope5e8, DoRoxlW, DW, Ss, 1, DW, DA, Fdai, ins7, GMW, SMW, 16)
Oper (Ope5f0, DoRoxlW, DW, Ss, 1, DW, DA, Faix, ins7, GMW, SMW, 20)
Oper (Ope5f8, DoRoxlW, DW, Ss, 1, DW, DA, Feaw, ins7, GMW, SMW, (cycles+19)&0xfffffffc)

Oper (Ope6d0, DoRorW, DW, Ss, 1, DW, DA, Cain, ins7, GMW, SMW, 12)
Oper (Ope6d8, DoRorW, DW, Ss, 1, DW, DA, CaipW, ins7, GMW, SMW, 12)
Oper (Ope6e0, DoRorW, DW, Ss, 1, DW, DA, CmaiW, ins7, GMW, SMW, 16)
Oper (Ope6e8, DoRorW, DW, Ss, 1, DW, DA, Fdai, ins7, GMW, SMW, 16)
Oper (Ope6f0, DoRorW, DW, Ss, 1, DW, DA, Faix, ins7, GMW, SMW, 20)
Oper (Ope6f8, DoRorW, DW, Ss, 1, DW, DA, Feaw, ins7, GMW, SMW, (cycles+19)&0xfffffffc)

Oper (Ope7d0, DoRolW, DW, Ss, 1, DW, DA, Cain, ins7, GMW, SMW, 12)
Oper (Ope7d8, DoRolW, DW, Ss, 1, DW, DA, CaipW, ins7, GMW, SMW, 12)
Oper (Ope7e0, DoRolW, DW, Ss, 1, DW, DA, CmaiW, ins7, GMW, SMW, 16)
Oper (Ope7e8, DoRolW, DW, Ss, 1, DW, DA, Fdai, ins7, GMW, SMW, 16)
Oper (Ope7f0, DoRolW, DW, Ss, 1, DW, DA, Faix, ins7, GMW, SMW, 20)
Oper (Ope7f8, DoRolW, DW, Ss, 1, DW, DA, Feaw, ins7, GMW, SMW, (cycles+19)&0xfffffffc)

