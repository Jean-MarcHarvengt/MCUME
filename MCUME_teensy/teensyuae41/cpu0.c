#include "shared.h"

#include "memory.h"
#include "custom.h"
#include "newcpu.h"
#include "cputbl.h"
void op_0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	BYTE dst = regs.d[dstreg];
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (src) & 0xff;
}}}}
void op_10(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}
void op_18(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{	regs.a[dstreg] += areg_byteinc[dstreg];
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void op_20(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	regs.a[dstreg] -= areg_byteinc[dstreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void op_28(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}
void op_30(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void op_38(ULONG opcode)
{
{{	BYTE src = nextiword();
{	CPTR dsta = (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}
void op_39(ULONG opcode)
{
{{	BYTE src = nextiword();
{	CPTR dsta = nextilong();
	BYTE dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}
void op_3c(ULONG opcode)
{
{	MakeSR();
{	WORD src = nextiword();
	src &= 0xFF;
	regs.sr |= src;
	MakeFromSR();
}}}
void op_40(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	WORD dst = regs.d[dstreg];
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (src) & 0xffff;
}}}}
void op_50(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}
void op_58(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
{	regs.a[dstreg] += 2;
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}}
void op_60(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	regs.a[dstreg] -= 2;
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}}
void op_68(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	WORD dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}
void op_70(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	WORD dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}}
void op_78(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = (LONG)(WORD)nextiword();
	WORD dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}
void op_79(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = nextilong();
	WORD dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}
void op_7c(ULONG opcode)
{
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{	MakeSR();
{	WORD src = nextiword();
	regs.sr |= src;
	MakeFromSR();
}}}}
void op_80(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	LONG dst = regs.d[dstreg];
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	regs.d[dstreg] = (src);
}}}}
void op_90(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}
void op_98(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
{	regs.a[dstreg] += 4;
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}}
void op_a0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	regs.a[dstreg] -= 4;
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}}
void op_a8(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	LONG dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}
void op_b0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	LONG dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}}
void op_b8(ULONG opcode)
{
{{	LONG src = nextilong();
{	CPTR dsta = (LONG)(WORD)nextiword();
	LONG dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}
void op_b9(ULONG opcode)
{
{{	LONG src = nextilong();
{	CPTR dsta = nextilong();
	LONG dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}
void op_100(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = regs.d[srcreg];
{	LONG dst = regs.d[dstreg];
	src &= 31;
	ZFLG = !(dst & (1 << src));
}}}}
void op_108(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
	ULONG dstreg = (opcode & 3584) >> 9;
{	CPTR memp = regs.a[srcreg] + nextiword();
{	UWORD val = (get_byte(memp) << 8) + get_byte(memp + 2);
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}
void op_110(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void op_118(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{	regs.a[dstreg] += areg_byteinc[dstreg];
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void op_120(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	regs.a[dstreg] -= areg_byteinc[dstreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void op_128(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void op_130(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void op_138(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void op_139(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = nextilong();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void op_13a(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (LONG)(BYTE)2;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = m68k_getpc();
	dsta += (LONG)(WORD)nextiword();
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void op_13b(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (LONG)(BYTE)3;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = m68k_getpc();
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void op_13c(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
{{	BYTE src = regs.d[srcreg];
{	BYTE dst = nextiword();
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void op_140(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = regs.d[srcreg];
{	LONG dst = regs.d[dstreg];
	src &= 31;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	regs.d[dstreg] = (dst);
}}}}
void op_148(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
	ULONG dstreg = (opcode & 3584) >> 9;
{	CPTR memp = regs.a[srcreg] + nextiword();
{	ULONG val = (get_byte(memp) << 24) + (get_byte(memp + 2) << 16)
              + (get_byte(memp + 4) << 8) + get_byte(memp + 6);
	regs.d[dstreg] = (val);
}}}
void op_150(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}
void op_158(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{	regs.a[dstreg] += areg_byteinc[dstreg];
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_160(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	regs.a[dstreg] -= areg_byteinc[dstreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_168(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}
void op_170(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_178(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}
void op_179(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = nextilong();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}
void op_17a(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (LONG)(BYTE)2;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = m68k_getpc();
	dsta += (LONG)(WORD)nextiword();
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_17b(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (LONG)(BYTE)3;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = m68k_getpc();
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_180(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = regs.d[srcreg];
{	LONG dst = regs.d[dstreg];
	src &= 31;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	regs.d[dstreg] = (dst);
}}}}
void op_188(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = regs.d[srcreg];
	CPTR memp = regs.a[dstreg] + nextiword();
	put_byte(memp, src >> 8); put_byte(memp + 2, src);
}}}
void op_190(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}
void op_198(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{	regs.a[dstreg] += areg_byteinc[dstreg];
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void op_1a0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	regs.a[dstreg] -= areg_byteinc[dstreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void op_1a8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}
void op_1b0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void op_1b8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}
void op_1b9(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = nextilong();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}
void op_1ba(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (LONG)(BYTE)2;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = m68k_getpc();
	dsta += (LONG)(WORD)nextiword();
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void op_1bb(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (LONG)(BYTE)3;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = m68k_getpc();
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void op_1c0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = regs.d[srcreg];
{	LONG dst = regs.d[dstreg];
	src &= 31;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	regs.d[dstreg] = (dst);
}}}}
void op_1c8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = regs.d[srcreg];
	CPTR memp = regs.a[dstreg] + nextiword();
	put_byte(memp, src >> 24); put_byte(memp + 2, src >> 16);
	put_byte(memp + 4, src >> 8); put_byte(memp + 6, src);
}}}
void op_1d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}
void op_1d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{	regs.a[dstreg] += areg_byteinc[dstreg];
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_1e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	regs.a[dstreg] -= areg_byteinc[dstreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_1e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}
void op_1f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_1f8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}
void op_1f9(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = nextilong();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}
void op_1fa(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (LONG)(BYTE)2;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = m68k_getpc();
	dsta += (LONG)(WORD)nextiword();
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_1fb(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (LONG)(BYTE)3;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = m68k_getpc();
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_200(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	BYTE dst = regs.d[dstreg];
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (src) & 0xff;
}}}}
void op_210(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}
void op_218(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{	regs.a[dstreg] += areg_byteinc[dstreg];
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void op_220(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	regs.a[dstreg] -= areg_byteinc[dstreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void op_228(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}
void op_230(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void op_238(ULONG opcode)
{
{{	BYTE src = nextiword();
{	CPTR dsta = (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}
void op_239(ULONG opcode)
{
{{	BYTE src = nextiword();
{	CPTR dsta = nextilong();
	BYTE dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}
void op_23c(ULONG opcode)
{
{	MakeSR();
{	WORD src = nextiword();
	src |= 0xFF00;
	regs.sr &= src;
	MakeFromSR();
}}}
void op_240(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	WORD dst = regs.d[dstreg];
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (src) & 0xffff;
}}}}
void op_250(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}
void op_258(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
{	regs.a[dstreg] += 2;
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}}
void op_260(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	regs.a[dstreg] -= 2;
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}}
void op_268(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	WORD dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}
void op_270(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	WORD dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}}
void op_278(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = (LONG)(WORD)nextiword();
	WORD dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}
void op_279(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = nextilong();
	WORD dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}
void op_27c(ULONG opcode)
{
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{	MakeSR();
{	WORD src = nextiword();
	regs.sr &= src;
	MakeFromSR();
}}}}
void op_280(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	LONG dst = regs.d[dstreg];
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	regs.d[dstreg] = (src);
}}}}
void op_290(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}
void op_298(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
{	regs.a[dstreg] += 4;
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}}
void op_2a0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	regs.a[dstreg] -= 4;
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}}
void op_2a8(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	LONG dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}
void op_2b0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	LONG dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}}
void op_2b8(ULONG opcode)
{
{{	LONG src = nextilong();
{	CPTR dsta = (LONG)(WORD)nextiword();
	LONG dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}
void op_2b9(ULONG opcode)
{
{{	LONG src = nextilong();
{	CPTR dsta = nextilong();
	LONG dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}
void op_400(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	BYTE dst = regs.d[dstreg];
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (newv) & 0xff;
}}}}}}}
void op_410(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}
void op_418(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{	regs.a[dstreg] += areg_byteinc[dstreg];
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}}
void op_420(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	regs.a[dstreg] -= areg_byteinc[dstreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}}
void op_428(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}
void op_430(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}}
void op_438(ULONG opcode)
{
{{	BYTE src = nextiword();
{	CPTR dsta = (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}
void op_439(ULONG opcode)
{
{{	BYTE src = nextiword();
{	CPTR dsta = nextilong();
	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}
void op_440(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	WORD dst = regs.d[dstreg];
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (newv) & 0xffff;
}}}}}}}
void op_450(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}
void op_458(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
{	regs.a[dstreg] += 2;
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}}
void op_460(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	regs.a[dstreg] -= 2;
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}}
void op_468(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}
void op_470(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}}
void op_478(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = (LONG)(WORD)nextiword();
	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}
void op_479(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = nextilong();
	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}
void op_480(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	LONG dst = regs.d[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
	regs.d[dstreg] = (newv);
}}}}}}}
void op_490(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}
void op_498(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
{	regs.a[dstreg] += 4;
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}}
void op_4a0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	regs.a[dstreg] -= 4;
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}}
void op_4a8(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}
void op_4b0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}}
void op_4b8(ULONG opcode)
{
{{	LONG src = nextilong();
{	CPTR dsta = (LONG)(WORD)nextiword();
	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}
void op_4b9(ULONG opcode)
{
{{	LONG src = nextilong();
{	CPTR dsta = nextilong();
	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}
void op_600(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	BYTE dst = regs.d[dstreg];
{{ULONG newv = ((BYTE)(dst)) + ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(~dst)) < ((UBYTE)(src));
	NFLG = flgn != 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (newv) & 0xff;
}}}}}}}
void op_610(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) + ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(~dst)) < ((UBYTE)(src));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}
void op_618(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{	regs.a[dstreg] += areg_byteinc[dstreg];
{{ULONG newv = ((BYTE)(dst)) + ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(~dst)) < ((UBYTE)(src));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}}
void op_620(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	regs.a[dstreg] -= areg_byteinc[dstreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) + ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(~dst)) < ((UBYTE)(src));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}}
void op_628(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) + ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(~dst)) < ((UBYTE)(src));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}
void op_630(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) + ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(~dst)) < ((UBYTE)(src));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}}
void op_638(ULONG opcode)
{
{{	BYTE src = nextiword();
{	CPTR dsta = (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) + ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(~dst)) < ((UBYTE)(src));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}
void op_639(ULONG opcode)
{
{{	BYTE src = nextiword();
{	CPTR dsta = nextilong();
	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) + ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(~dst)) < ((UBYTE)(src));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}
void op_640(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	WORD dst = regs.d[dstreg];
{{ULONG newv = ((WORD)(dst)) + ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(~dst)) < ((UWORD)(src));
	NFLG = flgn != 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (newv) & 0xffff;
}}}}}}}
void op_650(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) + ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(~dst)) < ((UWORD)(src));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}
void op_658(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
{	regs.a[dstreg] += 2;
{{ULONG newv = ((WORD)(dst)) + ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(~dst)) < ((UWORD)(src));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}}
void op_660(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	regs.a[dstreg] -= 2;
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) + ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(~dst)) < ((UWORD)(src));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}}
void op_668(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) + ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(~dst)) < ((UWORD)(src));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}
void op_670(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) + ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(~dst)) < ((UWORD)(src));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}}
void op_678(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = (LONG)(WORD)nextiword();
	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) + ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(~dst)) < ((UWORD)(src));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}
void op_679(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = nextilong();
	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) + ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(~dst)) < ((UWORD)(src));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}
void op_680(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	LONG dst = regs.d[dstreg];
{{ULONG newv = ((LONG)(dst)) + ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(~dst)) < ((ULONG)(src));
	NFLG = flgn != 0;
	regs.d[dstreg] = (newv);
}}}}}}}
void op_690(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) + ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(~dst)) < ((ULONG)(src));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}
void op_698(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
{	regs.a[dstreg] += 4;
{{ULONG newv = ((LONG)(dst)) + ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(~dst)) < ((ULONG)(src));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}}
void op_6a0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	regs.a[dstreg] -= 4;
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) + ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(~dst)) < ((ULONG)(src));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}}
void op_6a8(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) + ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(~dst)) < ((ULONG)(src));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}
void op_6b0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) + ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(~dst)) < ((ULONG)(src));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}}
void op_6b8(ULONG opcode)
{
{{	LONG src = nextilong();
{	CPTR dsta = (LONG)(WORD)nextiword();
	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) + ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(~dst)) < ((ULONG)(src));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}
void op_6b9(ULONG opcode)
{
{{	LONG src = nextilong();
{	CPTR dsta = nextilong();
	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) + ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(~dst)) < ((ULONG)(src));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}
void op_800(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	LONG dst = regs.d[dstreg];
	src &= 31;
	ZFLG = !(dst & (1 << src));
}}}}
void op_810(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void op_818(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{	regs.a[dstreg] += areg_byteinc[dstreg];
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void op_820(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	regs.a[dstreg] -= areg_byteinc[dstreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void op_828(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void op_830(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void op_838(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void op_839(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = nextilong();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void op_83a(ULONG opcode)
{
	ULONG dstreg = (LONG)(BYTE)2;
{{	WORD src = nextiword();
{	CPTR dsta = m68k_getpc();
	dsta += (LONG)(WORD)nextiword();
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void op_83b(ULONG opcode)
{
	ULONG dstreg = (LONG)(BYTE)3;
{{	WORD src = nextiword();
{	CPTR dsta = m68k_getpc();
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void op_83c(ULONG opcode)
{
{{	WORD src = nextiword();
{	BYTE dst = nextiword();
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void op_840(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	LONG dst = regs.d[dstreg];
	src &= 31;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	regs.d[dstreg] = (dst);
}}}}
void op_850(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}
void op_858(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{	regs.a[dstreg] += areg_byteinc[dstreg];
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_860(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	regs.a[dstreg] -= areg_byteinc[dstreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_868(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}
void op_870(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_878(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}
void op_879(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = nextilong();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}
void op_87a(ULONG opcode)
{
	ULONG dstreg = (LONG)(BYTE)2;
{{	WORD src = nextiword();
{	CPTR dsta = m68k_getpc();
	dsta += (LONG)(WORD)nextiword();
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_87b(ULONG opcode)
{
	ULONG dstreg = (LONG)(BYTE)3;
{{	WORD src = nextiword();
{	CPTR dsta = m68k_getpc();
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_880(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	LONG dst = regs.d[dstreg];
	src &= 31;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	regs.d[dstreg] = (dst);
}}}}
void op_890(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}
void op_898(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{	regs.a[dstreg] += areg_byteinc[dstreg];
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void op_8a0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	regs.a[dstreg] -= areg_byteinc[dstreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void op_8a8(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}
void op_8b0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void op_8b8(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}
void op_8b9(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = nextilong();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}
void op_8ba(ULONG opcode)
{
	ULONG dstreg = (LONG)(BYTE)2;
{{	WORD src = nextiword();
{	CPTR dsta = m68k_getpc();
	dsta += (LONG)(WORD)nextiword();
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void op_8bb(ULONG opcode)
{
	ULONG dstreg = (LONG)(BYTE)3;
{{	WORD src = nextiword();
{	CPTR dsta = m68k_getpc();
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void op_8c0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	LONG dst = regs.d[dstreg];
	src &= 31;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	regs.d[dstreg] = (dst);
}}}}
void op_8d0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}
void op_8d8(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{	regs.a[dstreg] += areg_byteinc[dstreg];
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_8e0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	regs.a[dstreg] -= areg_byteinc[dstreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_8e8(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}
void op_8f0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_8f8(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}
void op_8f9(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = nextilong();
	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}
void op_8fa(ULONG opcode)
{
	ULONG dstreg = (LONG)(BYTE)2;
{{	WORD src = nextiword();
{	CPTR dsta = m68k_getpc();
	dsta += (LONG)(WORD)nextiword();
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_8fb(ULONG opcode)
{
	ULONG dstreg = (LONG)(BYTE)3;
{{	WORD src = nextiword();
{	CPTR dsta = m68k_getpc();
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void op_a00(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	BYTE dst = regs.d[dstreg];
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (src) & 0xff;
}}}}
void op_a10(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}
void op_a18(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{	regs.a[dstreg] += areg_byteinc[dstreg];
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void op_a20(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	regs.a[dstreg] -= areg_byteinc[dstreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void op_a28(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}
void op_a30(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void op_a38(ULONG opcode)
{
{{	BYTE src = nextiword();
{	CPTR dsta = (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}
void op_a39(ULONG opcode)
{
{{	BYTE src = nextiword();
{	CPTR dsta = nextilong();
	BYTE dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}
void op_a3c(ULONG opcode)
{
{	MakeSR();
{	WORD src = nextiword();
	src &= 0xFF;
	regs.sr ^= src;
	MakeFromSR();
}}}
void op_a40(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	WORD dst = regs.d[dstreg];
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (src) & 0xffff;
}}}}
void op_a50(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}
void op_a58(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
{	regs.a[dstreg] += 2;
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}}
void op_a60(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	regs.a[dstreg] -= 2;
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}}
void op_a68(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	WORD dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}
void op_a70(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	WORD dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}}
void op_a78(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = (LONG)(WORD)nextiword();
	WORD dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}
void op_a79(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = nextilong();
	WORD dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}
void op_a7c(ULONG opcode)
{
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{	MakeSR();
{	WORD src = nextiword();
	regs.sr ^= src;
	MakeFromSR();
}}}}
void op_a80(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	LONG dst = regs.d[dstreg];
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	regs.d[dstreg] = (src);
}}}}
void op_a90(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}
void op_a98(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
{	regs.a[dstreg] += 4;
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}}
void op_aa0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	regs.a[dstreg] -= 4;
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}}
void op_aa8(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	LONG dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}
void op_ab0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	LONG dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}}
void op_ab8(ULONG opcode)
{
{{	LONG src = nextilong();
{	CPTR dsta = (LONG)(WORD)nextiword();
	LONG dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}
void op_ab9(ULONG opcode)
{
{{	LONG src = nextilong();
{	CPTR dsta = nextilong();
	LONG dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}
void op_c00(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	BYTE dst = regs.d[dstreg];
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_c10(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_c18(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{	regs.a[dstreg] += areg_byteinc[dstreg];
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_c20(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	regs.a[dstreg] -= areg_byteinc[dstreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_c28(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_c30(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_c38(ULONG opcode)
{
{{	BYTE src = nextiword();
{	CPTR dsta = (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_c39(ULONG opcode)
{
{{	BYTE src = nextiword();
{	CPTR dsta = nextilong();
	BYTE dst = get_byte(dsta);
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_c40(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	WORD dst = regs.d[dstreg];
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_c50(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_c58(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
{	regs.a[dstreg] += 2;
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_c60(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	regs.a[dstreg] -= 2;
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_c68(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_c70(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD src = nextiword();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_c78(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = (LONG)(WORD)nextiword();
	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_c79(ULONG opcode)
{
{{	WORD src = nextiword();
{	CPTR dsta = nextilong();
	WORD dst = get_word(dsta);
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_c80(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	LONG dst = regs.d[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_c90(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_c98(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
{	regs.a[dstreg] += 4;
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_ca0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	regs.a[dstreg] -= 4;
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_ca8(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_cb0(ULONG opcode)
{
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG src = nextilong();
{	CPTR dsta = regs.a[dstreg];
	dsta = get_disp_ea (dsta, nextiword());
{	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_cb8(ULONG opcode)
{
{{	LONG src = nextilong();
{	CPTR dsta = (LONG)(WORD)nextiword();
	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_cb9(ULONG opcode)
{
{{	LONG src = nextilong();
{	CPTR dsta = nextilong();
	LONG dst = get_long(dsta);
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
