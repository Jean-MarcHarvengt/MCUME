/*
 * Castaway
 *  (C) 1994 - 2002 Martin Doering, Joachim Hoenig
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#ifndef OP68KH
#define OP68KH

#include "config.h"
#include "operin.h"

extern short GetMemWpc(unsigned long address);
extern long GetMemLpc(unsigned long address);
extern void SetMemPL(unsigned long address, unsigned long value);
extern void SetMemPW(unsigned long address, unsigned long value);


/*
 * Status Register Access
 */
//AG: Changed to yet another SR implementation which is faster
//Note this code is little endian dependant and will need changing for big endian machines

//reg[16]=ZNVC
//reg[17]=XTS-
//reg[18]=inst

#define Zflag 0x1
#define Nflag 0x100
#define Vflag 0x10000
#define Cflag 0x1000000

#define SetC(flag)  if (flag) *(((int8*)reg)+67)=1; else *(((int8*)reg)+67)=0;
#define SetV(flag)  if (flag) *(((int8*)reg)+66)=1; else *(((int8*)reg)+66)=0;
#define SetX(flag)  if (flag) *(((int8*)reg)+68)=1; else *(((int8*)reg)+68)=0;
#define SetT(flag)  if (flag) *(((int8*)reg)+69)=1; else *(((int8*)reg)+69)=0;
#define SetZ(flag)  if (flag) *(((int8*)reg)+64)=1; else *(((int8*)reg)+64)=0;
#define SetZI(flag)  if (flag) *(((int8*)reg)+64)=0; else *(((int8*)reg)+64)=1;
#define SetN(flag)  if (flag) *(((int8*)reg)+65)=1; else *(((int8*)reg)+65)=0;
#define SetXC(flag) if (flag) *(((int8*)reg)+68)=*(((int8*)reg)+67)=1; else *(((int8*)reg)+68)=*(((int8*)reg)+67)=0;

#define ForceX(flag) *(((int8*)reg)+68)=flag;
#define ForceC(flag) *(((int8*)reg)+67)=flag;
#define ForceV(flag) *(((int8*)reg)+66)=flag;
#define ForceZ(flag) *(((int8*)reg)+64)=flag;
#define ForceCVNZ(operand) reg[16]=operand;

#define ZeroZ(flag) if (flag) *(((int8*)reg)+64)=0;

#define SetNZ(operand) \
	if (operand==0) *(((int16*)reg)+32)=0x1; \
	else if (operand<0) *(((int16*)reg)+32)=0x100; \
	else *(((int16*)reg)+32)=0;

#define SetI(imask) intmask=(imask); recalc_int=1;

#define SetS(flag) \
    if (*(((int8*)reg)+70)) ssp=reg[15]; else usp=reg[15]; \
	if (flag) {*(((int8*)reg)+70)=1; reg[15]=ssp;} else {*(((int8*)reg)+70)=0; reg[15]=usp;}

#define ClrCVSetNZ(operand) \
	if (operand==0) reg[16]=0x1; \
	else if (operand<0) reg[16]=0x100; \
	else reg[16]=0;

#define GetC() *(((int8*)reg)+67)
#define GetV() *(((int8*)reg)+66)
#define GetZ() *(((int8*)reg)+64)
#define GetN() *(((int8*)reg)+65)
#define GetX() *(((int8*)reg)+68)
#define GetI() intmask
#define GetS() *(((int8*)reg)+70)
#define GetT() *(((int8*)reg)+69)

//-------------------------------

extern uint16   GetSRB(void);
extern uint16   GetSRW(void);
extern void     SetSRB(unsigned long);
extern void     SetSRW(unsigned long);


/*
 * Other Register Access
 */
#define GetPC() pc
#define SetPC(addr) pc = addr

#define FastMPCB() (char)(biginst>>16)
#define FastMPCW() (((int32)biginst)>>16)
#define FastMPCW32() (biginst>>16)

#define GetMPCB() GetMemBpc (pc + 1)
#define GetMPCW() GetMemWpc (pc)
#define GetMPCL() GetMemLpc (pc)

#define IncMPCB(var) {register uint32 regpc=pc; pc=regpc+2; var=GetMemBpc (regpc+1);}
#define IncMPCW(var) {register uint32 regpc=pc; pc=regpc+2; var=GetMemWpc (regpc);}
#define IncMPCL(var) {register uint32 regpc=pc; pc=regpc+4; var=GetMemLpc (regpc);}

#define GetRegB(regno) (int8) reg[regno]
#define GetRegW(regno) (int16) reg[regno]
#define GetRegL(regno) (int32) reg[regno]

#define GetARegW(regno) (int16) reg[regno+8]
#define GetARegL(regno) (int32) reg[regno+8]

#ifndef USE_LITTLE_ENDIAN       /* USE_BIG_ENDIAN */
#define SetRegB(regno, value) *(((int8 *) &reg[regno]) + 3) = value
#define SetRegW(regno, value) *(((int16 *) &reg[regno]) + 1) = value
#define SetARegW(regno, value) *(((int16 *) &reg[regno+8]) + 1) = value
#else                       /* USE_LITTLE_ENDIAN */
#define SetRegB(regno, value) *((int8 *) &reg[regno]) = value
#define SetRegW(regno, value) *((int16 *) &reg[regno]) = value
#define SetARegW(regno, value) *((int16 *) &reg[regno+8]) = value
#endif
#define SetRegL(regno, value) reg[regno] = value
#define SetARegL(regno, value) reg[regno+8] = value



/*
 * Addressing modes (target)
 */
/* d, a, ain, aip, mai, dai, aix, imm, (eaw,ear,eac) */
#define CN(address,spec)
#define Cs(address,spec) address=spec;
#define Cd(address,spec) address=spec;
#define Ca(address,spec) address=8+spec;
#define Cain(address,spec) address=reg[spec+8];
#define CaipW(address,spec) address=reg[spec+8]; reg[spec+8]=address+2;
#define CaipL(address,spec) address=reg[spec+8]; reg[spec+8]=address+4;
#define CmaiW(address,spec) address=reg[spec+8]; address-=2; reg[spec+8]=address;
#define CmaiL(address,spec) address=reg[spec+8]; address-=4; reg[spec+8]=address;
#define Cdai(address,spec) IncMPCW(address); address+=reg[spec+8];
#define Caix(address,spec) { register uint16 mod = GetMPCW(); pc += 2; address = reg[spec+8] + (int8) mod + ((mod & 0x800) ? GetRegL(mod >> 12) : GetRegW(mod >> 12)); }
#define CaipB(address,spec) address=reg[spec+8]; if (spec==7) reg[spec+8]=address+2; else reg[spec+8]=address+1;
#define CaipB15(address,spec) address=reg[spec+8]; reg[spec+8]=address+2;

#define CmaiB(address,spec) address=reg[spec+8]; if (spec==7) address-=2; else address--; reg[spec+8]=address;
#define CmaiB15(address,spec) address=reg[spec+8]; address-=2; reg[spec+8]=address;

#define Ceaw(address, spec) \
switch (spec) {\
case 0x0:\
	IncMPCW(address);\
    break;\
case 0x1:\
	IncMPCL(address);\
	cycles += 4;\
    break;\
default:\
    ExceptionGroup1(ILLINSTR);\
    address = 0;\
    break;\
}
#define Cear(address,spec) \
{ register uint16 mod;\
  register uint32 regpc=pc;\
switch (spec) {\
case 0x0:\
	pc=regpc+2;\
    address = GetMemWpc(regpc);\
    break;\
case 0x1:\
	pc=regpc+4;\
    address = GetMemLpc(regpc);\
	cycles += 4;\
    break;\
case 0x2:\
	pc=regpc+2;\
    address = regpc + GetMemWpc(regpc);\
    break;\
case 0x3:\
	pc=regpc+2;\
    mod = GetMemWpc(regpc);\
    address = regpc + (int8) mod + ((mod & 0x800) ? GetRegL(mod >> 12) : GetRegW(mod >> 12));\
	cycles += 2;\
    break;\
default:\
    ExceptionGroup1(ILLINSTR);\
    address = 0;\
    break;\
}}

//Faster version of effective address macros 
//Only use these when the pc hasn't been advanced by an earlier macro
#define Fdai(address,spec) address=FastMPCW()+reg[spec+8]; pc+=2;
#define Faix(address,spec) {register uint32 mod=FastMPCW32(); pc += 2; address = reg[spec+8] + (int8) mod + ((mod & 0x800) ? GetRegL(mod >> 12) : GetRegW(mod >> 12)); }
#define Feaw(address, spec) \
switch (spec) {\
case 0x0:\
	address=FastMPCW(); pc+=2;\
    break;\
case 0x1:\
	IncMPCL(address);\
	cycles += 4;\
    break;\
default:\
    ExceptionGroup1(ILLINSTR);\
    address = 0;\
    break;\
}
#define Fear(address,spec) \
{ register uint32 mod;\
  register uint32 regpc=pc;\
switch (spec) {\
case 0x0:\
	pc=regpc+2;\
    address = FastMPCW();\
    break;\
case 0x1:\
	pc=regpc+4;\
    address = GetMemLpc(regpc);\
	cycles += 4;\
    break;\
case 0x2:\
	pc=regpc+2;\
    address = regpc + FastMPCW();\
    break;\
case 0x3:\
	pc=regpc+2;\
    mod = FastMPCW32();\
    address = regpc + (int8) mod + ((mod & 0x800) ? GetRegL(mod >> 12) : GetRegW(mod >> 12));\
	cycles += 2;\
    break;\
default:\
    ExceptionGroup1(ILLINSTR);\
    address = 0;\
    break;\
}}



/*
 * Operand types: B byte, W word, L long, R regno, A address
 */
#define DN(name)
#define DB(name) register int8 name;
#define DW(name) register int16 name;
#define DL(name) register int32 name;
#define DR(name) register uint16 name;
#define DA(name) register int32 name;


/*
 * target macros
 */
#define GRB(regno, name) name = GetRegB (regno);
#define GRW(regno, name) name = GetRegW (regno);
#define GRL(regno, name) name = GetRegL (regno);
#define SRB(regno, name) SetRegB (regno, name);
#define SRW(regno, name) SetRegW (regno, name);
#define SRL(regno, name) SetRegL (regno, name);
#define GMB(address, name) name = GetMemB (address);
#define GMW(address, name) name = GetMemW (address);
#define GML(address, name) name = GetMemL (address);
#define GN(address, name)
#define SMB(address, name) SetMemB (address, name);
#define SMW(address, name) SetMemW (address, name);
#define SML(address, name) SetMemL (address, name);
#define SN(address, name)
#define GCB(dummy, name) name = GetSRB ();
#define GCW(dummy, name) name = GetSRW ();
#define SCB(dummy, name) SetSRB (name);
#define SCW(dummy, name) SetSRW (name);
#define GPW(address, name) \
    name = GetMemB(address);\
    name <<= 8;\
    name |= (uint8) GetMemB(address + 2);
#define GPL(address, name) \
    name = GetMemB(address);\
    name <<= 8;\
    name |= (uint8) GetMemB(address + 2);\
    name <<= 8;\
    name |= (uint8) GetMemB(address + 4);\
    name <<= 8;\
    name |= (uint8) GetMemB(address + 6);
#define SPW(address, name) \
	SetMemPW(address, name);
#define SPL(address, name) \
	SetMemPL(address, name);


/*
 * source macros
 */
#define SdaiPW(var,spec) {uint32 a; Cdai(a,spec); var = GPW(a,var);}
#define SdaiPL(var,spec) {uint32 a; Cdai(a,spec); var = GPL(a,var);}
#define SNN(var,spec)
#define SNQ(var, dummy) var = (int8) biginst;
#define ScB(var,spec) var = GetSRB();
#define ScW(var,spec) var = GetSRW();
#define Ss(var,spec) var = spec;
#define SimmB(var,spec) var=FastMPCB(); pc+=2;
#define SimmW(var,spec) var=FastMPCW(); pc+=2;
#define SimmL(var,spec) IncMPCL(var);
#define SdB(var,spec) var = GetRegB (spec);
#define SdW(var,spec) var = GetRegW (spec);
#define SdL(var,spec) var = GetRegL (spec);
#define SaW(var,spec) var = GetARegW (spec);
#define SaL(var,spec) var = GetARegL (spec);
#define SainB(var,spec) var = GetMemB (reg[spec+8]);
#define SainW(var,spec) var = GetMemW (reg[spec+8]);
#define SainL(var,spec) var = GetMemL (reg[spec+8]);
#define SainA(var,spec) var = (reg[spec+8]);
#define SaipW(var,spec) {register uint32 myadr=reg[spec+8]; reg[spec+8]=myadr+2; var=GetMemW(myadr);}
#define SaipL(var,spec) {register uint32 myadr=reg[spec+8]; reg[spec+8]=myadr+4; var=GetMemL(myadr);}
#define SmaiW(var,spec) {register uint32 myadr=reg[spec+8]; myadr-=2; reg[spec+8]=myadr; var=GetMemW(myadr);}
#define SmaiL(var,spec) {register uint32 myadr=reg[spec+8]; myadr-=4; reg[spec+8]=myadr; var=GetMemL(myadr);}
#define SdaiB(var,spec) var = GetMemB(reg[spec+8]+FastMPCW()); pc+=2;
#define SdaiW(var,spec) var = GetMemW(reg[spec+8]+FastMPCW()); pc+=2;
#define SdaiL(var,spec) var = GetMemL(reg[spec+8]+FastMPCW()); pc+=2;
#define SdaiA(var,spec) var = reg[spec+8]+FastMPCW(); pc+=2;
#define SaixB(var,spec) {register uint32 mod=FastMPCW32(); pc+=2; var = GetMemB(reg[spec+8] + (int8) mod + ((mod & 0x800) ? GetRegL(mod >> 12) : GetRegW(mod >> 12)));};
#define SaixW(var,spec) {register uint32 mod=FastMPCW32(); pc+=2; var = GetMemW(reg[spec+8] + (int8) mod + ((mod & 0x800) ? GetRegL(mod >> 12) : GetRegW(mod >> 12)));};
#define SaixL(var,spec) {register uint32 mod=FastMPCW32(); pc+=2; var = GetMemL(reg[spec+8] + (int8) mod + ((mod & 0x800) ? GetRegL(mod >> 12) : GetRegW(mod >> 12)));};
#define SaixA(var,spec) {register uint32 mod=FastMPCW32(); pc+=2; var = (reg[spec+8] + (int8) mod + ((mod & 0x800) ? GetRegL(mod >> 12) : GetRegW(mod >> 12)));};
#define SaipB(var,spec) {register uint32 myadr=reg[spec+8]; if (spec==7) reg[spec+8]=myadr+2; else reg[spec+8]=myadr+1; var=GetMemB(myadr);}
#define SmaiB(var,spec) {register uint32 myadr=reg[spec+8]; if (spec==7) myadr-=2; else myadr--; reg[spec+8]=myadr; var=GetMemB(myadr);}

#define SearB(var,spec) \
{ register uint32 mod;\
  register uint32 regpc=pc;\
switch (spec) {\
case 0x0:\
	pc=regpc+2;\
    var = GetMemB (FastMPCW());\
	cycles += 4;\
    break;\
case 0x1:\
	pc=regpc+4;\
    var = GetMemB (GetMemLpc(regpc));\
	cycles += 8;\
    break;\
case 0x2:\
	pc=regpc+2;\
    var = GetMemB (regpc+FastMPCW());\
	cycles += 4;\
    break;\
case 0x3:\
	pc=regpc+2;\
    mod = FastMPCW32();\
    var = GetMemB (regpc + (int8) mod + ((mod & 0x800) ? GetRegL(mod >> 12) : GetRegW(mod >> 12)));\
	cycles += 6;\
    break;\
case 0x4:\
	pc=regpc+2;\
    var = FastMPCB();\
    break;\
default:\
    ExceptionGroup1(ILLINSTR);\
    var = 0;\
    break;\
}}
#define SearW(var,spec) \
{ register uint32 mod;\
  register uint32 regpc=pc;\
switch (spec) {\
case 0x0:\
	pc=regpc+2;\
    var = GetMemW (FastMPCW());\
	cycles += 4;\
    break;\
case 0x1:\
	pc=regpc+4;\
    var = GetMemW (GetMemLpc(regpc));\
	cycles += 8;\
    break;\
case 0x2:\
	pc=regpc+2;\
    var = GetMemW (regpc+FastMPCW());\
	cycles += 4;\
    break;\
case 0x3:\
	pc=regpc+2;\
    mod = FastMPCW32();\
    var = GetMemW (regpc + (int8) mod + ((mod & 0x800) ? GetRegL(mod >> 12) : GetRegW(mod >> 12)));\
	cycles += 6;\
    break;\
case 0x4:\
	pc=regpc+2;\
    var = FastMPCW();\
    break;\
default:\
    ExceptionGroup1(ILLINSTR);\
    var = 0;\
    break;\
}}
#define SearL(var,spec) \
{ register uint32 mod;\
  register uint32 regpc=pc;\
switch (spec) {\
case 0x0:\
	pc=regpc+2;\
    var = GetMemL (FastMPCW());\
	cycles += 4;\
    break;\
case 0x1:\
	pc=regpc+4;\
    var = GetMemL (GetMemLpc(regpc));\
	cycles += 8;\
    break;\
case 0x2:\
	pc=regpc+2;\
    var = GetMemL (regpc+FastMPCW());\
	cycles += 4;\
    break;\
case 0x3:\
	pc=regpc+2;\
    mod = FastMPCW32();\
    var = GetMemL (regpc + (int8) mod + ((mod & 0x800) ? GetRegL(mod >> 12) : GetRegW(mod >> 12)));\
	cycles += 6;\
    break;\
case 0x4:\
	pc=regpc+4;\
    var = GetMemLpc(regpc);\
    break;\
default:\
    ExceptionGroup1(ILLINSTR);\
    var = 0;\
    break;\
}}
#define SearA(var,spec) \
{ register uint32 mod;\
  register uint32 regpc=pc;\
switch (spec) {\
case 0x0:\
	pc=regpc+2;\
    var = FastMPCW();\
    break;\
case 0x1:\
	pc=regpc+4;\
    var = GetMemLpc(regpc);\
	cycles += 4;\
    break;\
case 0x2:\
	pc=regpc+2;\
    var = regpc+FastMPCW();\
    break;\
case 0x3:\
	pc=regpc+2;\
    mod = FastMPCW32();\
    var = regpc + (int8) mod + ((mod & 0x800) ? GetRegL(mod >> 12) : GetRegW(mod >> 12));\
	cycles += 2;\
    break;\
default:\
    ExceptionGroup1(ILLINSTR);\
    var = 0;\
    break;\
}}

/*
 * Condition-Code queries
 */
#define CCt  0xff
#define CCf  0x00
#define CChi (!GetC() && !GetZ())
#define CCls (GetC() || GetZ())
#define CCcc !GetC()
#define CCcs GetC()
#define CCne !GetZ()
#define CCeq GetZ()
#define CCvc !GetV()
#define CCvs GetV()
#define CCpl !GetN()
#define CCmi GetN()
#define CCge !(GetN() ^ GetV())
#define CClt (GetN() ^ GetV())
#define CCgt (!(GetN() ^ GetV()) && !GetZ())
#define CCle ((GetN() ^ GetV()) || GetZ())

/*
 * Oper arguments:
 * Code                     Opcode
 * Op(target,source)        Macro used to execute operation
 * DeclS(name)              Macro to declare source (such as DB, DW or DL, or
                            DN=no source, DA=address, DR=register number)
 * GetS(var,spec)           Macro to fetch source, with addressing mode evaluation.
                            SNN = no fetch
                            SNQ = used in MOVEQ
                            ScB = condition code register
                            ScW = status register word (s-flag not checked)
                            Ss = source equals source specifier (spec1)
                            SaW, SaL = get #<spec1> data register value
                            SdB, SdW, SdL = get data register value
                            SainB, SainW, SainL = (A<spec>)
                            SainA = (LEA only)
                            SaixB, SaixW, SaixL = (d8,A<spec>,X<>)
                            SaixA = (LEA only)
                            SdaiB, SdaiW, SdaiL = d16(A<spec>)
                            SdaiA = (LEA only)
                            SaipB, SaipW, SaipL = (A<spec>)+
                            SmaiB, SmaiW, SmaiL = -(A<spec>)
                            SimmB, SimmW, SimmL = immediate
                            SdaiPW, SdaiPL = used in MOVEP
                            SearB, SearW, SearL = other addressing modes
                            SearA = (LEA only)
 * spec1                    source specifier, usually the data or address register number.
 * DeclT(name)              Macro to declare target (such as DB, DW or DL, or
                            DN=no target, DA=address, DR=register number)
 * DeclEA2(name)            Macro to declare target address (typically DA, DR, or DN)
 * CalcEA2(address,spec)    Macro to calculate address, with addressing mode evaluation.
                            CN = no address
                            Cs = equals target specifier (spec2)
                            Cd = equals target specifier (spec2) (data register number)
                            Ca = equals target specifier (spec2) + 8 (address register number)
                            Cain = (A<spec>)
                            Caix = (d8,A<spec>,X<>)
                            Cdai = d16(A<spec>)
                            CaipB, CaipW, CaipL = (A<spec>)+,
                            CaipB15 = stack pointer only
                            CmaiB, CmaiW, CmaiL = -(A<spec>)
                            CmaiB15 = stack pointer only
                            Ceaw = other addressing modes (if target is modified)
                            Cear = other addressing modes (if target is not modified)
 * spec2                    target specifier, usually the data or address register number.
 * GetEA2(address,name)     Macro to get target value
                            GCB = get condition code
                            GCW = get status (s-flag not checked)
                            GN = no read
                            GMB, GMW, GML = read from memory
                            GPW, GPL = (MOVEP)
                            GRB, GRW, GRL = read from register
 * SetEA2(address,name)     Macro to set target value
                            SCB = set condition code
                            SCW = set status (s-flag not checked)
                            SN = no write
                            SMB, SMW, SML = write to memory
                            SPW, SPL = (MOVEP)
                            SRB, SRW, SRL = write to register
 */

#if (!defined(DEBUG) || !defined(INTERNALTRACE))
#define Oper(Code, Op, DeclS, GetS, spec1, DeclT, DeclEA2, CalcEA2, spec2, GetEA2, SetEA2, rval)\
unsigned long            Code(operin)\
{\
	register unsigned long cycles=0;\
    DeclS (source)\
    DeclT (target)\
    DeclEA2 (address2)\
    GetS (source, spec1)\
    CalcEA2 (address2, spec2)\
    GetEA2 (address2, target)\
    Op (target, source)\
    SetEA2 (address2, target)\
	return rval; \
}
#else
/* quick'n dirty debug hack */
#undef DN
#define DN(name) int name;
#define Oper(Code, Op, DeclS, GetS, spec1, DeclT, DeclEA2, CalcEA2, spec2, GetEA2, SetEA2, rval)\
unsigned long            Code(operin)\
{\
	register unsigned long cycles=0;\
    DeclS (source)\
    DeclT (target)\
    DeclEA2 (address2)\
    source = 0; \
    target = 0; \
    address2 = 0; \
    GetS (source, spec1)\
    CalcEA2 (address2, spec2)\
    GetEA2 (address2, target)\
    traceback[tbi].address = address2; \
    traceback[tbi].source = source; \
    traceback[tbi].target = target; \
    Op (target, source)\
    traceback[tbi].result = target; \
    SetEA2 (address2, target)\
	return rval; \
}
#endif
#endif
