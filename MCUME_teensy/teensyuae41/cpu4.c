#include "shared.h"

#include "memory.h"
#include "custom.h"
#include "newcpu.h"
#include "cputbl.h"
void op_4000(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	BYTE src = regs.d[srcreg];
{	src += regs.x;
{ULONG dst = ((BYTE)(0)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(0)) < 0;
	int flgn = ((BYTE)(dst)) < 0;
	ZFLG = ((BYTE)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(0));
	NFLG = flgn != 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (dst) & 0xff;
}}}}}}
void op_4010(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{	src += regs.x;
{ULONG dst = ((BYTE)(0)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(0)) < 0;
	int flgn = ((BYTE)(dst)) < 0;
	ZFLG = ((BYTE)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}
void op_4018(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	src += regs.x;
{ULONG dst = ((BYTE)(0)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(0)) < 0;
	int flgn = ((BYTE)(dst)) < 0;
	ZFLG = ((BYTE)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}}
void op_4020(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{	src += regs.x;
{ULONG dst = ((BYTE)(0)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(0)) < 0;
	int flgn = ((BYTE)(dst)) < 0;
	ZFLG = ((BYTE)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}}
void op_4028(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	BYTE src = get_byte(srca);
{	src += regs.x;
{ULONG dst = ((BYTE)(0)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(0)) < 0;
	int flgn = ((BYTE)(dst)) < 0;
	ZFLG = ((BYTE)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}
void op_4030(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	BYTE src = get_byte(srca);
{	src += regs.x;
{ULONG dst = ((BYTE)(0)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(0)) < 0;
	int flgn = ((BYTE)(dst)) < 0;
	ZFLG = ((BYTE)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}}
void op_4038(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	BYTE src = get_byte(srca);
{	src += regs.x;
{ULONG dst = ((BYTE)(0)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(0)) < 0;
	int flgn = ((BYTE)(dst)) < 0;
	ZFLG = ((BYTE)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}
void op_4039(ULONG opcode)
{
{{	CPTR srca = nextilong();
	BYTE src = get_byte(srca);
{	src += regs.x;
{ULONG dst = ((BYTE)(0)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(0)) < 0;
	int flgn = ((BYTE)(dst)) < 0;
	ZFLG = ((BYTE)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}
void op_4040(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	src += regs.x;
{ULONG dst = ((WORD)(0)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(0)) < 0;
	int flgn = ((WORD)(dst)) < 0;
	ZFLG = ((WORD)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(0));
	NFLG = flgn != 0;
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (dst) & 0xffff;
}}}}}}
void op_4050(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	src += regs.x;
{ULONG dst = ((WORD)(0)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(0)) < 0;
	int flgn = ((WORD)(dst)) < 0;
	ZFLG = ((WORD)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}
void op_4058(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	regs.a[srcreg] += 2;
{	src += regs.x;
{ULONG dst = ((WORD)(0)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(0)) < 0;
	int flgn = ((WORD)(dst)) < 0;
	ZFLG = ((WORD)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}}
void op_4060(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 2;
{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	src += regs.x;
{ULONG dst = ((WORD)(0)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(0)) < 0;
	int flgn = ((WORD)(dst)) < 0;
	ZFLG = ((WORD)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}}
void op_4068(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	WORD src = get_word(srca);
{	src += regs.x;
{ULONG dst = ((WORD)(0)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(0)) < 0;
	int flgn = ((WORD)(dst)) < 0;
	ZFLG = ((WORD)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}
void op_4070(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	WORD src = get_word(srca);
{	src += regs.x;
{ULONG dst = ((WORD)(0)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(0)) < 0;
	int flgn = ((WORD)(dst)) < 0;
	ZFLG = ((WORD)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}}
void op_4078(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	WORD src = get_word(srca);
{	src += regs.x;
{ULONG dst = ((WORD)(0)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(0)) < 0;
	int flgn = ((WORD)(dst)) < 0;
	ZFLG = ((WORD)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}
void op_4079(ULONG opcode)
{
{{	CPTR srca = nextilong();
	WORD src = get_word(srca);
{	src += regs.x;
{ULONG dst = ((WORD)(0)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(0)) < 0;
	int flgn = ((WORD)(dst)) < 0;
	ZFLG = ((WORD)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}
void op_4080(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	LONG src = regs.d[srcreg];
{	src += regs.x;
{ULONG dst = ((LONG)(0)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(0)) < 0;
	int flgn = ((LONG)(dst)) < 0;
	ZFLG = ((LONG)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(0));
	NFLG = flgn != 0;
	regs.d[srcreg] = (dst);
}}}}}}
void op_4090(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{	src += regs.x;
{ULONG dst = ((LONG)(0)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(0)) < 0;
	int flgn = ((LONG)(dst)) < 0;
	ZFLG = ((LONG)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}
void op_4098(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{	regs.a[srcreg] += 4;
{	src += regs.x;
{ULONG dst = ((LONG)(0)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(0)) < 0;
	int flgn = ((LONG)(dst)) < 0;
	ZFLG = ((LONG)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}}
void op_40a0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 4;
{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{	src += regs.x;
{ULONG dst = ((LONG)(0)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(0)) < 0;
	int flgn = ((LONG)(dst)) < 0;
	ZFLG = ((LONG)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}}
void op_40a8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	LONG src = get_long(srca);
{	src += regs.x;
{ULONG dst = ((LONG)(0)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(0)) < 0;
	int flgn = ((LONG)(dst)) < 0;
	ZFLG = ((LONG)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}
void op_40b0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	LONG src = get_long(srca);
{	src += regs.x;
{ULONG dst = ((LONG)(0)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(0)) < 0;
	int flgn = ((LONG)(dst)) < 0;
	ZFLG = ((LONG)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}}
void op_40b8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	LONG src = get_long(srca);
{	src += regs.x;
{ULONG dst = ((LONG)(0)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(0)) < 0;
	int flgn = ((LONG)(dst)) < 0;
	ZFLG = ((LONG)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}
void op_40b9(ULONG opcode)
{
{{	CPTR srca = nextilong();
	LONG src = get_long(srca);
{	src += regs.x;
{ULONG dst = ((LONG)(0)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(0)) < 0;
	int flgn = ((LONG)(dst)) < 0;
	ZFLG = ((LONG)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}
void op_40c0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	MakeSR();
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (regs.sr) & 0xffff;
}}}
void op_40d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	MakeSR();
	put_word(srca,regs.sr);
}}}
void op_40d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += 2;
	MakeSR();
	put_word(srca,regs.sr);
}}}}
void op_40e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 2;
{	CPTR srca = regs.a[srcreg];
	MakeSR();
	put_word(srca,regs.sr);
}}}}
void op_40e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	MakeSR();
	put_word(srca,regs.sr);
}}}
void op_40f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
	MakeSR();
	put_word(srca,regs.sr);
}}}
void op_40f8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	MakeSR();
	put_word(srca,regs.sr);
}}}
void op_40f9(ULONG opcode)
{
{{	CPTR srca = nextilong();
	MakeSR();
	put_word(srca,regs.sr);
}}}
void op_4180(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
	ULONG dstreg = (opcode & 3584) >> 9;
{{	WORD src = regs.d[srcreg];
{	WORD dst = regs.d[dstreg];
	if ((WORD)dst < 0) { NFLG=1; Exception(6); }
	else if ((WORD)dst > (WORD)src) { NFLG=0; Exception(6); }
}}}}
void op_4188(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
	ULONG dstreg = (opcode & 3584) >> 9;
{{	WORD src = regs.a[srcreg];
{	WORD dst = regs.d[dstreg];
	if ((WORD)dst < 0) { NFLG=1; Exception(6); }
	else if ((WORD)dst > (WORD)src) { NFLG=0; Exception(6); }
}}}}
void op_4190(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
	ULONG dstreg = (opcode & 3584) >> 9;
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	WORD dst = regs.d[dstreg];
	if ((WORD)dst < 0) { NFLG=1; Exception(6); }
	else if ((WORD)dst > (WORD)src) { NFLG=0; Exception(6); }
}}}}
void op_4198(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
	ULONG dstreg = (opcode & 3584) >> 9;
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	regs.a[srcreg] += 2;
{	WORD dst = regs.d[dstreg];
	if ((WORD)dst < 0) { NFLG=1; Exception(6); }
	else if ((WORD)dst > (WORD)src) { NFLG=0; Exception(6); }
}}}}}
void op_41a0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
	ULONG dstreg = (opcode & 3584) >> 9;
{{	regs.a[srcreg] -= 2;
{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	WORD dst = regs.d[dstreg];
	if ((WORD)dst < 0) { NFLG=1; Exception(6); }
	else if ((WORD)dst > (WORD)src) { NFLG=0; Exception(6); }
}}}}}
void op_41a8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
	ULONG dstreg = (opcode & 3584) >> 9;
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	WORD src = get_word(srca);
{	WORD dst = regs.d[dstreg];
	if ((WORD)dst < 0) { NFLG=1; Exception(6); }
	else if ((WORD)dst > (WORD)src) { NFLG=0; Exception(6); }
}}}}
void op_41b0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
	ULONG dstreg = (opcode & 3584) >> 9;
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	WORD src = get_word(srca);
{	WORD dst = regs.d[dstreg];
	if ((WORD)dst < 0) { NFLG=1; Exception(6); }
	else if ((WORD)dst > (WORD)src) { NFLG=0; Exception(6); }
}}}}}
void op_41b8(ULONG opcode)
{
	ULONG dstreg = (opcode & 3584) >> 9;
{{	CPTR srca = (LONG)(WORD)nextiword();
	WORD src = get_word(srca);
{	WORD dst = regs.d[dstreg];
	if ((WORD)dst < 0) { NFLG=1; Exception(6); }
	else if ((WORD)dst > (WORD)src) { NFLG=0; Exception(6); }
}}}}
void op_41b9(ULONG opcode)
{
	ULONG dstreg = (opcode & 3584) >> 9;
{{	CPTR srca = nextilong();
	WORD src = get_word(srca);
{	WORD dst = regs.d[dstreg];
	if ((WORD)dst < 0) { NFLG=1; Exception(6); }
	else if ((WORD)dst > (WORD)src) { NFLG=0; Exception(6); }
}}}}
void op_41ba(ULONG opcode)
{
	ULONG dstreg = (opcode & 3584) >> 9;
{{	CPTR srca = m68k_getpc();
	srca += (LONG)(WORD)nextiword();
{	WORD src = get_word(srca);
{	WORD dst = regs.d[dstreg];
	if ((WORD)dst < 0) { NFLG=1; Exception(6); }
	else if ((WORD)dst > (WORD)src) { NFLG=0; Exception(6); }
}}}}}
void op_41bb(ULONG opcode)
{
	ULONG dstreg = (opcode & 3584) >> 9;
{{	CPTR srca = m68k_getpc();
	srca = get_disp_ea (srca, nextiword());
{	WORD src = get_word(srca);
{	WORD dst = regs.d[dstreg];
	if ((WORD)dst < 0) { NFLG=1; Exception(6); }
	else if ((WORD)dst > (WORD)src) { NFLG=0; Exception(6); }
}}}}}
void op_41bc(ULONG opcode)
{
	ULONG dstreg = (opcode & 3584) >> 9;
{{	WORD src = nextiword();
{	WORD dst = regs.d[dstreg];
	if ((WORD)dst < 0) { NFLG=1; Exception(6); }
	else if ((WORD)dst > (WORD)src) { NFLG=0; Exception(6); }
}}}}
void op_41d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
	ULONG dstreg = (opcode & 3584) >> 9;
{{	CPTR srca = regs.a[srcreg];
{	regs.a[dstreg] = (srca);
}}}}
void op_41d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
	ULONG dstreg = (opcode & 3584) >> 9;
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += 4;
{	regs.a[dstreg] = (srca);
}}}}}
void op_41e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
	ULONG dstreg = (opcode & 3584) >> 9;
{{	regs.a[srcreg] -= 4;
{	CPTR srca = regs.a[srcreg];
{	regs.a[dstreg] = (srca);
}}}}}
void op_41e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
	ULONG dstreg = (opcode & 3584) >> 9;
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	regs.a[dstreg] = (srca);
}}}}
void op_41f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
	ULONG dstreg = (opcode & 3584) >> 9;
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	regs.a[dstreg] = (srca);
}}}}
void op_41f8(ULONG opcode)
{
	ULONG dstreg = (opcode & 3584) >> 9;
{{	CPTR srca = (LONG)(WORD)nextiword();
{	regs.a[dstreg] = (srca);
}}}}
void op_41f9(ULONG opcode)
{
	ULONG dstreg = (opcode & 3584) >> 9;
{{	CPTR srca = nextilong();
{	regs.a[dstreg] = (srca);
}}}}
void op_41fa(ULONG opcode)
{
	ULONG dstreg = (opcode & 3584) >> 9;
{{	CPTR srca = m68k_getpc();
	srca += (LONG)(WORD)nextiword();
{	regs.a[dstreg] = (srca);
}}}}
void op_41fb(ULONG opcode)
{
	ULONG dstreg = (opcode & 3584) >> 9;
{{	CPTR srca = m68k_getpc();
	srca = get_disp_ea (srca, nextiword());
{	regs.a[dstreg] = (srca);
}}}}
void op_4200(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(0)) == 0;
	NFLG = ((BYTE)(0)) < 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (0) & 0xff;
}}}
void op_4210(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(0)) == 0;
	NFLG = ((BYTE)(0)) < 0;
	put_byte(srca,0);
}}}
void op_4218(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(0)) == 0;
	NFLG = ((BYTE)(0)) < 0;
	put_byte(srca,0);
}}}}
void op_4220(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(0)) == 0;
	NFLG = ((BYTE)(0)) < 0;
	put_byte(srca,0);
}}}}
void op_4228(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(0)) == 0;
	NFLG = ((BYTE)(0)) < 0;
	put_byte(srca,0);
}}}
void op_4230(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(0)) == 0;
	NFLG = ((BYTE)(0)) < 0;
	put_byte(srca,0);
}}}
void op_4238(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(0)) == 0;
	NFLG = ((BYTE)(0)) < 0;
	put_byte(srca,0);
}}}
void op_4239(ULONG opcode)
{
{{	CPTR srca = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(0)) == 0;
	NFLG = ((BYTE)(0)) < 0;
	put_byte(srca,0);
}}}
void op_4240(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	VFLG = CFLG = 0;
	ZFLG = ((WORD)(0)) == 0;
	NFLG = ((WORD)(0)) < 0;
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (0) & 0xffff;
}}}
void op_4250(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(0)) == 0;
	NFLG = ((WORD)(0)) < 0;
	put_word(srca,0);
}}}
void op_4258(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += 2;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(0)) == 0;
	NFLG = ((WORD)(0)) < 0;
	put_word(srca,0);
}}}}
void op_4260(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 2;
{	CPTR srca = regs.a[srcreg];
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(0)) == 0;
	NFLG = ((WORD)(0)) < 0;
	put_word(srca,0);
}}}}
void op_4268(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(0)) == 0;
	NFLG = ((WORD)(0)) < 0;
	put_word(srca,0);
}}}
void op_4270(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(0)) == 0;
	NFLG = ((WORD)(0)) < 0;
	put_word(srca,0);
}}}
void op_4278(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(0)) == 0;
	NFLG = ((WORD)(0)) < 0;
	put_word(srca,0);
}}}
void op_4279(ULONG opcode)
{
{{	CPTR srca = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(0)) == 0;
	NFLG = ((WORD)(0)) < 0;
	put_word(srca,0);
}}}
void op_4280(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	VFLG = CFLG = 0;
	ZFLG = ((LONG)(0)) == 0;
	NFLG = ((LONG)(0)) < 0;
	regs.d[srcreg] = (0);
}}}
void op_4290(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(0)) == 0;
	NFLG = ((LONG)(0)) < 0;
	put_long(srca,0);
}}}
void op_4298(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += 4;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(0)) == 0;
	NFLG = ((LONG)(0)) < 0;
	put_long(srca,0);
}}}}
void op_42a0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 4;
{	CPTR srca = regs.a[srcreg];
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(0)) == 0;
	NFLG = ((LONG)(0)) < 0;
	put_long(srca,0);
}}}}
void op_42a8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(0)) == 0;
	NFLG = ((LONG)(0)) < 0;
	put_long(srca,0);
}}}
void op_42b0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(0)) == 0;
	NFLG = ((LONG)(0)) < 0;
	put_long(srca,0);
}}}
void op_42b8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(0)) == 0;
	NFLG = ((LONG)(0)) < 0;
	put_long(srca,0);
}}}
void op_42b9(ULONG opcode)
{
{{	CPTR srca = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(0)) == 0;
	NFLG = ((LONG)(0)) < 0;
	put_long(srca,0);
}}}
void op_4400(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	BYTE src = regs.d[srcreg];
{{ULONG dst = ((BYTE)(0)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(0)) < 0;
	int flgn = ((BYTE)(dst)) < 0;
	ZFLG = ((BYTE)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(0));
	NFLG = flgn != 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (dst) & 0xff;
}}}}}}
void op_4410(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{{ULONG dst = ((BYTE)(0)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(0)) < 0;
	int flgn = ((BYTE)(dst)) < 0;
	ZFLG = ((BYTE)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}
void op_4418(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{	regs.a[srcreg] += areg_byteinc[srcreg];
{{ULONG dst = ((BYTE)(0)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(0)) < 0;
	int flgn = ((BYTE)(dst)) < 0;
	ZFLG = ((BYTE)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}}
void op_4420(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{{ULONG dst = ((BYTE)(0)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(0)) < 0;
	int flgn = ((BYTE)(dst)) < 0;
	ZFLG = ((BYTE)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}}
void op_4428(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	BYTE src = get_byte(srca);
{{ULONG dst = ((BYTE)(0)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(0)) < 0;
	int flgn = ((BYTE)(dst)) < 0;
	ZFLG = ((BYTE)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}
void op_4430(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	BYTE src = get_byte(srca);
{{ULONG dst = ((BYTE)(0)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(0)) < 0;
	int flgn = ((BYTE)(dst)) < 0;
	ZFLG = ((BYTE)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}}
void op_4438(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	BYTE src = get_byte(srca);
{{ULONG dst = ((BYTE)(0)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(0)) < 0;
	int flgn = ((BYTE)(dst)) < 0;
	ZFLG = ((BYTE)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}
void op_4439(ULONG opcode)
{
{{	CPTR srca = nextilong();
	BYTE src = get_byte(srca);
{{ULONG dst = ((BYTE)(0)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(0)) < 0;
	int flgn = ((BYTE)(dst)) < 0;
	ZFLG = ((BYTE)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UBYTE)(src)) > ((UBYTE)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}
void op_4440(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{{ULONG dst = ((WORD)(0)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(0)) < 0;
	int flgn = ((WORD)(dst)) < 0;
	ZFLG = ((WORD)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(0));
	NFLG = flgn != 0;
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (dst) & 0xffff;
}}}}}}
void op_4450(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{{ULONG dst = ((WORD)(0)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(0)) < 0;
	int flgn = ((WORD)(dst)) < 0;
	ZFLG = ((WORD)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}
void op_4458(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	regs.a[srcreg] += 2;
{{ULONG dst = ((WORD)(0)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(0)) < 0;
	int flgn = ((WORD)(dst)) < 0;
	ZFLG = ((WORD)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}}
void op_4460(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 2;
{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{{ULONG dst = ((WORD)(0)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(0)) < 0;
	int flgn = ((WORD)(dst)) < 0;
	ZFLG = ((WORD)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}}
void op_4468(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	WORD src = get_word(srca);
{{ULONG dst = ((WORD)(0)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(0)) < 0;
	int flgn = ((WORD)(dst)) < 0;
	ZFLG = ((WORD)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}
void op_4470(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	WORD src = get_word(srca);
{{ULONG dst = ((WORD)(0)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(0)) < 0;
	int flgn = ((WORD)(dst)) < 0;
	ZFLG = ((WORD)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}}
void op_4478(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	WORD src = get_word(srca);
{{ULONG dst = ((WORD)(0)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(0)) < 0;
	int flgn = ((WORD)(dst)) < 0;
	ZFLG = ((WORD)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}
void op_4479(ULONG opcode)
{
{{	CPTR srca = nextilong();
	WORD src = get_word(srca);
{{ULONG dst = ((WORD)(0)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(0)) < 0;
	int flgn = ((WORD)(dst)) < 0;
	ZFLG = ((WORD)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((UWORD)(src)) > ((UWORD)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}
void op_4480(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	LONG src = regs.d[srcreg];
{{ULONG dst = ((LONG)(0)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(0)) < 0;
	int flgn = ((LONG)(dst)) < 0;
	ZFLG = ((LONG)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(0));
	NFLG = flgn != 0;
	regs.d[srcreg] = (dst);
}}}}}}
void op_4490(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{{ULONG dst = ((LONG)(0)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(0)) < 0;
	int flgn = ((LONG)(dst)) < 0;
	ZFLG = ((LONG)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}
void op_4498(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{	regs.a[srcreg] += 4;
{{ULONG dst = ((LONG)(0)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(0)) < 0;
	int flgn = ((LONG)(dst)) < 0;
	ZFLG = ((LONG)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}}
void op_44a0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 4;
{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{{ULONG dst = ((LONG)(0)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(0)) < 0;
	int flgn = ((LONG)(dst)) < 0;
	ZFLG = ((LONG)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}}
void op_44a8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	LONG src = get_long(srca);
{{ULONG dst = ((LONG)(0)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(0)) < 0;
	int flgn = ((LONG)(dst)) < 0;
	ZFLG = ((LONG)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}
void op_44b0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	LONG src = get_long(srca);
{{ULONG dst = ((LONG)(0)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(0)) < 0;
	int flgn = ((LONG)(dst)) < 0;
	ZFLG = ((LONG)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}}
void op_44b8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	LONG src = get_long(srca);
{{ULONG dst = ((LONG)(0)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(0)) < 0;
	int flgn = ((LONG)(dst)) < 0;
	ZFLG = ((LONG)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}
void op_44b9(ULONG opcode)
{
{{	CPTR srca = nextilong();
	LONG src = get_long(srca);
{{ULONG dst = ((LONG)(0)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(0)) < 0;
	int flgn = ((LONG)(dst)) < 0;
	ZFLG = ((LONG)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = regs.x = ((ULONG)(src)) > ((ULONG)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}
void op_44c0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	BYTE src = regs.d[srcreg];
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}
void op_44d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}
void op_44d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{	regs.a[srcreg] += areg_byteinc[srcreg];
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}}
void op_44e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}}
void op_44e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	BYTE src = get_byte(srca);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}
void op_44f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	BYTE src = get_byte(srca);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}}
void op_44f8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	BYTE src = get_byte(srca);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}
void op_44f9(ULONG opcode)
{
{{	CPTR srca = nextilong();
	BYTE src = get_byte(srca);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}
void op_44fa(ULONG opcode)
{
{{	CPTR srca = m68k_getpc();
	srca += (LONG)(WORD)nextiword();
{	BYTE src = get_byte(srca);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}}
void op_44fb(ULONG opcode)
{
{{	CPTR srca = m68k_getpc();
	srca = get_disp_ea (srca, nextiword());
{	BYTE src = get_byte(srca);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}}
void op_44fc(ULONG opcode)
{
{{	BYTE src = nextiword();
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}
void op_4600(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	BYTE src = regs.d[srcreg];
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(dst)) == 0;
	NFLG = ((BYTE)(dst)) < 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (dst) & 0xff;
}}}}
void op_4610(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(dst)) == 0;
	NFLG = ((BYTE)(dst)) < 0;
	put_byte(srca,dst);
}}}}
void op_4618(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(dst)) == 0;
	NFLG = ((BYTE)(dst)) < 0;
	put_byte(srca,dst);
}}}}}
void op_4620(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(dst)) == 0;
	NFLG = ((BYTE)(dst)) < 0;
	put_byte(srca,dst);
}}}}}
void op_4628(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	BYTE src = get_byte(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(dst)) == 0;
	NFLG = ((BYTE)(dst)) < 0;
	put_byte(srca,dst);
}}}}
void op_4630(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	BYTE src = get_byte(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(dst)) == 0;
	NFLG = ((BYTE)(dst)) < 0;
	put_byte(srca,dst);
}}}}}
void op_4638(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	BYTE src = get_byte(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(dst)) == 0;
	NFLG = ((BYTE)(dst)) < 0;
	put_byte(srca,dst);
}}}}
void op_4639(ULONG opcode)
{
{{	CPTR srca = nextilong();
	BYTE src = get_byte(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(dst)) == 0;
	NFLG = ((BYTE)(dst)) < 0;
	put_byte(srca,dst);
}}}}
void op_4640(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(dst)) == 0;
	NFLG = ((WORD)(dst)) < 0;
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (dst) & 0xffff;
}}}}
void op_4650(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(dst)) == 0;
	NFLG = ((WORD)(dst)) < 0;
	put_word(srca,dst);
}}}}
void op_4658(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	regs.a[srcreg] += 2;
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(dst)) == 0;
	NFLG = ((WORD)(dst)) < 0;
	put_word(srca,dst);
}}}}}
void op_4660(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 2;
{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(dst)) == 0;
	NFLG = ((WORD)(dst)) < 0;
	put_word(srca,dst);
}}}}}
void op_4668(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	WORD src = get_word(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(dst)) == 0;
	NFLG = ((WORD)(dst)) < 0;
	put_word(srca,dst);
}}}}
void op_4670(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	WORD src = get_word(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(dst)) == 0;
	NFLG = ((WORD)(dst)) < 0;
	put_word(srca,dst);
}}}}}
void op_4678(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	WORD src = get_word(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(dst)) == 0;
	NFLG = ((WORD)(dst)) < 0;
	put_word(srca,dst);
}}}}
void op_4679(ULONG opcode)
{
{{	CPTR srca = nextilong();
	WORD src = get_word(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(dst)) == 0;
	NFLG = ((WORD)(dst)) < 0;
	put_word(srca,dst);
}}}}
void op_4680(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	LONG src = regs.d[srcreg];
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(dst)) == 0;
	NFLG = ((LONG)(dst)) < 0;
	regs.d[srcreg] = (dst);
}}}}
void op_4690(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(dst)) == 0;
	NFLG = ((LONG)(dst)) < 0;
	put_long(srca,dst);
}}}}
void op_4698(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{	regs.a[srcreg] += 4;
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(dst)) == 0;
	NFLG = ((LONG)(dst)) < 0;
	put_long(srca,dst);
}}}}}
void op_46a0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 4;
{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(dst)) == 0;
	NFLG = ((LONG)(dst)) < 0;
	put_long(srca,dst);
}}}}}
void op_46a8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	LONG src = get_long(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(dst)) == 0;
	NFLG = ((LONG)(dst)) < 0;
	put_long(srca,dst);
}}}}
void op_46b0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	LONG src = get_long(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(dst)) == 0;
	NFLG = ((LONG)(dst)) < 0;
	put_long(srca,dst);
}}}}}
void op_46b8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	LONG src = get_long(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(dst)) == 0;
	NFLG = ((LONG)(dst)) < 0;
	put_long(srca,dst);
}}}}
void op_46b9(ULONG opcode)
{
{{	CPTR srca = nextilong();
	LONG src = get_long(srca);
{	ULONG dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(dst)) == 0;
	NFLG = ((LONG)(dst)) < 0;
	put_long(srca,dst);
}}}}
void op_46c0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{{	WORD src = regs.d[srcreg];
	regs.sr = src;
	MakeFromSR();
}}}}
void op_46d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
	regs.sr = src;
	MakeFromSR();
}}}}
void op_46d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	regs.a[srcreg] += 2;
	regs.sr = src;
	MakeFromSR();
}}}}}
void op_46e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{{	regs.a[srcreg] -= 2;
{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
	regs.sr = src;
	MakeFromSR();
}}}}}
void op_46e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	WORD src = get_word(srca);
	regs.sr = src;
	MakeFromSR();
}}}}
void op_46f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	WORD src = get_word(srca);
	regs.sr = src;
	MakeFromSR();
}}}}}
void op_46f8(ULONG opcode)
{
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{{	CPTR srca = (LONG)(WORD)nextiword();
	WORD src = get_word(srca);
	regs.sr = src;
	MakeFromSR();
}}}}
void op_46f9(ULONG opcode)
{
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{{	CPTR srca = nextilong();
	WORD src = get_word(srca);
	regs.sr = src;
	MakeFromSR();
}}}}
void op_46fa(ULONG opcode)
{
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{{	CPTR srca = m68k_getpc();
	srca += (LONG)(WORD)nextiword();
{	WORD src = get_word(srca);
	regs.sr = src;
	MakeFromSR();
}}}}}
void op_46fb(ULONG opcode)
{
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{{	CPTR srca = m68k_getpc();
	srca = get_disp_ea (srca, nextiword());
{	WORD src = get_word(srca);
	regs.sr = src;
	MakeFromSR();
}}}}}
void op_46fc(ULONG opcode)
{
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{{	WORD src = nextiword();
	regs.sr = src;
	MakeFromSR();
}}}}
void op_4800(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	BYTE src = regs.d[srcreg];
{	UWORD newv_lo = - (src & 0xF) - regs.x;
	UWORD newv_hi = - (src & 0xF0);
	UWORD newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = regs.x = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (newv != 0) ZFLG = 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (newv) & 0xff;
}}}}
void op_4810(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{	UWORD newv_lo = - (src & 0xF) - regs.x;
	UWORD newv_hi = - (src & 0xF0);
	UWORD newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = regs.x = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (newv != 0) ZFLG = 0;
	put_byte(srca,newv);
}}}}
void op_4818(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	UWORD newv_lo = - (src & 0xF) - regs.x;
	UWORD newv_hi = - (src & 0xF0);
	UWORD newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = regs.x = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (newv != 0) ZFLG = 0;
	put_byte(srca,newv);
}}}}}
void op_4820(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{	UWORD newv_lo = - (src & 0xF) - regs.x;
	UWORD newv_hi = - (src & 0xF0);
	UWORD newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = regs.x = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (newv != 0) ZFLG = 0;
	put_byte(srca,newv);
}}}}}
void op_4828(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	BYTE src = get_byte(srca);
{	UWORD newv_lo = - (src & 0xF) - regs.x;
	UWORD newv_hi = - (src & 0xF0);
	UWORD newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = regs.x = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (newv != 0) ZFLG = 0;
	put_byte(srca,newv);
}}}}
void op_4830(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	BYTE src = get_byte(srca);
{	UWORD newv_lo = - (src & 0xF) - regs.x;
	UWORD newv_hi = - (src & 0xF0);
	UWORD newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = regs.x = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (newv != 0) ZFLG = 0;
	put_byte(srca,newv);
}}}}}
void op_4838(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	BYTE src = get_byte(srca);
{	UWORD newv_lo = - (src & 0xF) - regs.x;
	UWORD newv_hi = - (src & 0xF0);
	UWORD newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = regs.x = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (newv != 0) ZFLG = 0;
	put_byte(srca,newv);
}}}}
void op_4839(ULONG opcode)
{
{{	CPTR srca = nextilong();
	BYTE src = get_byte(srca);
{	UWORD newv_lo = - (src & 0xF) - regs.x;
	UWORD newv_hi = - (src & 0xF0);
	UWORD newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = regs.x = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (newv != 0) ZFLG = 0;
	put_byte(srca,newv);
}}}}
void op_4840(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	LONG src = regs.d[srcreg];
{	ULONG dst = ((src >> 16)&0xFFFF) | ((src&0xFFFF)<<16);
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(dst)) == 0;
	NFLG = ((LONG)(dst)) < 0;
	regs.d[srcreg] = (dst);
}}}}
void op_4850(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[7] -= 4;
{	CPTR dsta = regs.a[7];
	put_long(dsta,srca);
}}}}}
void op_4858(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += 4;
{	regs.a[7] -= 4;
{	CPTR dsta = regs.a[7];
	put_long(dsta,srca);
}}}}}}
void op_4860(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 4;
{	CPTR srca = regs.a[srcreg];
{	regs.a[7] -= 4;
{	CPTR dsta = regs.a[7];
	put_long(dsta,srca);
}}}}}}
void op_4868(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	regs.a[7] -= 4;
{	CPTR dsta = regs.a[7];
	put_long(dsta,srca);
}}}}}
void op_4870(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	regs.a[7] -= 4;
{	CPTR dsta = regs.a[7];
	put_long(dsta,srca);
}}}}}
void op_4878(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	regs.a[7] -= 4;
{	CPTR dsta = regs.a[7];
	put_long(dsta,srca);
}}}}}
void op_4879(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	regs.a[7] -= 4;
{	CPTR dsta = regs.a[7];
	put_long(dsta,srca);
}}}}}
void op_487a(ULONG opcode)
{
{{	CPTR srca = m68k_getpc();
	srca += (LONG)(WORD)nextiword();
{	regs.a[7] -= 4;
{	CPTR dsta = regs.a[7];
	put_long(dsta,srca);
}}}}}
void op_487b(ULONG opcode)
{
{{	CPTR srca = m68k_getpc();
	srca = get_disp_ea (srca, nextiword());
{	regs.a[7] -= 4;
{	CPTR dsta = regs.a[7];
	put_long(dsta,srca);
}}}}}
void op_4880(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	ULONG dst = (LONG)(BYTE)src;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(dst)) == 0;
	NFLG = ((WORD)(dst)) < 0;
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (dst) & 0xffff;
}}}}
void op_4890(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = regs.a[srcreg];
{	int i, bitcnt = 0;
	ULONG rd[8], ra[8];
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { rd[i] = regs.d[i]; ra[i] = regs.a[i]; }
	for(i=0;i<8;i++) { if (mask & 1) { put_word(srca, rd[i]); srca += 2; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { put_word(srca, ra[i]); srca += 2; } mask >>= 1; }
}}}}
void op_4898(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = regs.a[srcreg];
{	int i, bitcnt = 0;
	ULONG rd[8], ra[8];
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { rd[i] = regs.d[i]; ra[i] = regs.a[i]; }
	for(i=0;i<8;i++) { if (mask & 1) { put_word(srca, rd[i]); srca += 2; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { put_word(srca, ra[i]); srca += 2; } mask >>= 1; }
}}}}
void op_48a0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{{	CPTR srca = regs.a[srcreg];
{	int i, bitcnt = 0;
	ULONG rd[8], ra[8];
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { rd[i] = regs.d[i]; ra[i] = regs.a[i]; }
	srca -= 2*bitcnt;
	regs.a[srcreg] = srca;
	for(i=0;i<8;i++) { if (mask & 32768) { put_word(srca, rd[i]); srca += 2; } mask <<= 1; }
	for(i=0;i<8;i++) { if (mask & 32768) { put_word(srca, ra[i]); srca += 2; } mask <<= 1; }
}}}}}
void op_48a8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int i, bitcnt = 0;
	ULONG rd[8], ra[8];
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { rd[i] = regs.d[i]; ra[i] = regs.a[i]; }
	for(i=0;i<8;i++) { if (mask & 1) { put_word(srca, rd[i]); srca += 2; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { put_word(srca, ra[i]); srca += 2; } mask >>= 1; }
}}}}
void op_48b0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int i, bitcnt = 0;
	ULONG rd[8], ra[8];
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { rd[i] = regs.d[i]; ra[i] = regs.a[i]; }
	for(i=0;i<8;i++) { if (mask & 1) { put_word(srca, rd[i]); srca += 2; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { put_word(srca, ra[i]); srca += 2; } mask >>= 1; }
}}}}
void op_48b8(ULONG opcode)
{
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = (LONG)(WORD)nextiword();
{	int i, bitcnt = 0;
	ULONG rd[8], ra[8];
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { rd[i] = regs.d[i]; ra[i] = regs.a[i]; }
	for(i=0;i<8;i++) { if (mask & 1) { put_word(srca, rd[i]); srca += 2; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { put_word(srca, ra[i]); srca += 2; } mask >>= 1; }
}}}}
void op_48b9(ULONG opcode)
{
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = nextilong();
{	int i, bitcnt = 0;
	ULONG rd[8], ra[8];
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { rd[i] = regs.d[i]; ra[i] = regs.a[i]; }
	for(i=0;i<8;i++) { if (mask & 1) { put_word(srca, rd[i]); srca += 2; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { put_word(srca, ra[i]); srca += 2; } mask >>= 1; }
}}}}
void op_48c0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	LONG src = regs.d[srcreg];
{	ULONG dst = (LONG)(WORD)src;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(dst)) == 0;
	NFLG = ((LONG)(dst)) < 0;
	regs.d[srcreg] = (dst);
}}}}
void op_48d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = regs.a[srcreg];
{	int i, bitcnt = 0;
	ULONG rd[8], ra[8];
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { rd[i] = regs.d[i]; ra[i] = regs.a[i]; }
	for(i=0;i<8;i++) { if (mask & 1) { put_long(srca, rd[i]); srca += 4; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { put_long(srca, ra[i]); srca += 4; } mask >>= 1; }
}}}}
void op_48d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = regs.a[srcreg];
{	int i, bitcnt = 0;
	ULONG rd[8], ra[8];
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { rd[i] = regs.d[i]; ra[i] = regs.a[i]; }
	for(i=0;i<8;i++) { if (mask & 1) { put_long(srca, rd[i]); srca += 4; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { put_long(srca, ra[i]); srca += 4; } mask >>= 1; }
}}}}
void op_48e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{{	CPTR srca = regs.a[srcreg];
{	int i, bitcnt = 0;
	ULONG rd[8], ra[8];
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { rd[i] = regs.d[i]; ra[i] = regs.a[i]; }
	srca -= 4*bitcnt;
	regs.a[srcreg] = srca;
	for(i=0;i<8;i++) { if (mask & 32768) { put_long(srca, rd[i]); srca += 4; } mask <<= 1; }
	for(i=0;i<8;i++) { if (mask & 32768) { put_long(srca, ra[i]); srca += 4; } mask <<= 1; }
}}}}}
void op_48e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int i, bitcnt = 0;
	ULONG rd[8], ra[8];
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { rd[i] = regs.d[i]; ra[i] = regs.a[i]; }
	for(i=0;i<8;i++) { if (mask & 1) { put_long(srca, rd[i]); srca += 4; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { put_long(srca, ra[i]); srca += 4; } mask >>= 1; }
}}}}
void op_48f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int i, bitcnt = 0;
	ULONG rd[8], ra[8];
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { rd[i] = regs.d[i]; ra[i] = regs.a[i]; }
	for(i=0;i<8;i++) { if (mask & 1) { put_long(srca, rd[i]); srca += 4; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { put_long(srca, ra[i]); srca += 4; } mask >>= 1; }
}}}}
void op_48f8(ULONG opcode)
{
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = (LONG)(WORD)nextiword();
{	int i, bitcnt = 0;
	ULONG rd[8], ra[8];
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { rd[i] = regs.d[i]; ra[i] = regs.a[i]; }
	for(i=0;i<8;i++) { if (mask & 1) { put_long(srca, rd[i]); srca += 4; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { put_long(srca, ra[i]); srca += 4; } mask >>= 1; }
}}}}
void op_48f9(ULONG opcode)
{
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = nextilong();
{	int i, bitcnt = 0;
	ULONG rd[8], ra[8];
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { rd[i] = regs.d[i]; ra[i] = regs.a[i]; }
	for(i=0;i<8;i++) { if (mask & 1) { put_long(srca, rd[i]); srca += 4; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { put_long(srca, ra[i]); srca += 4; } mask >>= 1; }
}}}}
void op_4a00(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	BYTE src = regs.d[srcreg];
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
}}}
void op_4a10(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
}}}
void op_4a18(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{	regs.a[srcreg] += areg_byteinc[srcreg];
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
}}}}
void op_4a20(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
}}}}
void op_4a28(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	BYTE src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
}}}
void op_4a30(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	BYTE src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
}}}}
void op_4a38(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	BYTE src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
}}}
void op_4a39(ULONG opcode)
{
{{	CPTR srca = nextilong();
	BYTE src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
}}}
void op_4a40(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
}}}
void op_4a48(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.a[srcreg];
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
}}}
void op_4a50(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
}}}
void op_4a58(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	regs.a[srcreg] += 2;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
}}}}
void op_4a60(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 2;
{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
}}}}
void op_4a68(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	WORD src = get_word(srca);
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
}}}
void op_4a70(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	WORD src = get_word(srca);
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
}}}}
void op_4a78(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	WORD src = get_word(srca);
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
}}}
void op_4a79(ULONG opcode)
{
{{	CPTR srca = nextilong();
	WORD src = get_word(srca);
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
}}}
void op_4a80(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	LONG src = regs.d[srcreg];
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
}}}
void op_4a88(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	LONG src = regs.a[srcreg];
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
}}}
void op_4a90(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
}}}
void op_4a98(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{	regs.a[srcreg] += 4;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
}}}}
void op_4aa0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 4;
{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
}}}}
void op_4aa8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	LONG src = get_long(srca);
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
}}}
void op_4ab0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	LONG src = get_long(srca);
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
}}}}
void op_4ab8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	LONG src = get_long(srca);
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
}}}
void op_4ab9(ULONG opcode)
{
{{	CPTR srca = nextilong();
	LONG src = get_long(srca);
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
}}}
void op_4c90(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = regs.a[srcreg];
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
}}}}
void op_4c98(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = regs.a[srcreg];
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
	regs.a[srcreg] = srca;
}}}}
void op_4ca0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{{	CPTR srca = regs.a[srcreg];
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
}}}}}
void op_4ca8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
}}}}
void op_4cb0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
}}}}
void op_4cb8(ULONG opcode)
{
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = (LONG)(WORD)nextiword();
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
}}}}
void op_4cb9(ULONG opcode)
{
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = nextilong();
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
}}}}
void op_4cba(ULONG opcode)
{
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = m68k_getpc();
	srca += (LONG)(WORD)nextiword();
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
}}}}
void op_4cbb(ULONG opcode)
{
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = m68k_getpc();
	srca = get_disp_ea (srca, nextiword());
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = (LONG)(WORD)get_word(srca); srca += 2; } mask >>= 1; }
}}}}
void op_4cd0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = regs.a[srcreg];
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = get_long(srca); srca += 4; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = get_long(srca); srca += 4; } mask >>= 1; }
}}}}
void op_4cd8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = regs.a[srcreg];
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = get_long(srca); srca += 4; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = get_long(srca); srca += 4; } mask >>= 1; }
	regs.a[srcreg] = srca;
}}}}
void op_4ce0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{{	CPTR srca = regs.a[srcreg];
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = get_long(srca); srca += 4; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = get_long(srca); srca += 4; } mask >>= 1; }
}}}}}
void op_4ce8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = get_long(srca); srca += 4; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = get_long(srca); srca += 4; } mask >>= 1; }
}}}}
void op_4cf0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = get_long(srca); srca += 4; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = get_long(srca); srca += 4; } mask >>= 1; }
}}}}
void op_4cf8(ULONG opcode)
{
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = (LONG)(WORD)nextiword();
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = get_long(srca); srca += 4; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = get_long(srca); srca += 4; } mask >>= 1; }
}}}}
void op_4cf9(ULONG opcode)
{
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = nextilong();
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = get_long(srca); srca += 4; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = get_long(srca); srca += 4; } mask >>= 1; }
}}}}
void op_4cfa(ULONG opcode)
{
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = m68k_getpc();
	srca += (LONG)(WORD)nextiword();
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = get_long(srca); srca += 4; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = get_long(srca); srca += 4; } mask >>= 1; }
}}}}
void op_4cfb(ULONG opcode)
{
{	UWORD mask = nextiword(), bitmask = mask;
{	CPTR srca = m68k_getpc();
	srca = get_disp_ea (srca, nextiword());
{	int i, bitcnt = 0;
	for(i=0;i<16;i++) { bitcnt += bitmask & 1; bitmask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.d[i] = get_long(srca); srca += 4; } mask >>= 1; }
	for(i=0;i<8;i++) { if (mask & 1) { regs.a[i] = get_long(srca); srca += 4; } mask >>= 1; }
}}}}
void op_4e40(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 15) >> 0);
{{	ULONG src = srcreg;
	Exception(src+32);
}}}
void op_4e50(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[7] -= 4;
{	CPTR olda = regs.a[7];
{	LONG src = regs.a[srcreg];
	put_long(olda,src);
	regs.a[srcreg] = (regs.a[7]);
{	WORD offs = nextiword();
	regs.a[7] += offs;
}}}}}}
void op_4e58(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	LONG src = regs.a[srcreg];
	regs.a[7] = src;
{	CPTR olda = regs.a[7];
	LONG old = get_long(olda);
{	regs.a[7] += 4;
	regs.a[srcreg] = (old);
}}}}}
void op_4e60(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{{	LONG src = regs.a[srcreg];
	regs.usp = src;
}}}}
void op_4e68(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{{	regs.a[srcreg] = (regs.usp);
}}}}
void op_4e70(ULONG opcode)
{
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{	customreset();
}}}
void op_4e71(ULONG opcode)
{
{}}
void op_4e72(ULONG opcode)
{
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{{	WORD src = nextiword();
	regs.sr = src;
	MakeFromSR();
	m68k_setstopped(1);
}}}}
void op_4e73(ULONG opcode)
{
{if (!regs.s) { regs.pc_p--; Exception(8); } else
{{	CPTR sra = regs.a[7];
	WORD sr = get_word(sra);
{	regs.a[7] += 2;
{	CPTR pca = regs.a[7];
	LONG pc = get_long(pca);
{	regs.a[7] += 4;
	regs.sr = sr; m68k_setpc(pc);
	MakeFromSR();
}}}}}}}
void op_4e74(ULONG opcode)
{
{}}
void op_4e75(ULONG opcode)
{
{{	CPTR pca = regs.a[7];
	LONG pc = get_long(pca);
{	regs.a[7] += 4;
	m68k_setpc(pc);
}}}}
void op_4e76(ULONG opcode)
{
{	if(VFLG) Exception(7);
}}
void op_4e77(ULONG opcode)
{
{	MakeSR();
{	CPTR sra = regs.a[7];
	WORD sr = get_word(sra);
{	regs.a[7] += 2;
{	CPTR pca = regs.a[7];
	LONG pc = get_long(pca);
{	regs.a[7] += 4;
	regs.sr &= 0xFF00; sr &= 0xFF;
	regs.sr |= sr; m68k_setpc(pc);
	MakeFromSR();
}}}}}}
void op_4e90(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[7] -= 4;
{	CPTR spa = regs.a[7];
	put_long(spa,m68k_getpc());
	m68k_setpc(srca);
}}}}}
void op_4e98(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += 4;
{	regs.a[7] -= 4;
{	CPTR spa = regs.a[7];
	put_long(spa,m68k_getpc());
	m68k_setpc(srca);
}}}}}}
void op_4ea0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 4;
{	CPTR srca = regs.a[srcreg];
{	regs.a[7] -= 4;
{	CPTR spa = regs.a[7];
	put_long(spa,m68k_getpc());
	m68k_setpc(srca);
}}}}}}
void op_4ea8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	regs.a[7] -= 4;
{	CPTR spa = regs.a[7];
	put_long(spa,m68k_getpc());
	m68k_setpc(srca);
}}}}}
void op_4eb0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	regs.a[7] -= 4;
{	CPTR spa = regs.a[7];
	put_long(spa,m68k_getpc());
	m68k_setpc(srca);
}}}}}
void op_4eb8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	regs.a[7] -= 4;
{	CPTR spa = regs.a[7];
	put_long(spa,m68k_getpc());
	m68k_setpc(srca);
}}}}}
void op_4eb9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	regs.a[7] -= 4;
{	CPTR spa = regs.a[7];
	put_long(spa,m68k_getpc());
	m68k_setpc(srca);
}}}}}
void op_4eba(ULONG opcode)
{
{{	CPTR srca = m68k_getpc();
	srca += (LONG)(WORD)nextiword();
{	regs.a[7] -= 4;
{	CPTR spa = regs.a[7];
	put_long(spa,m68k_getpc());
	m68k_setpc(srca);
}}}}}
void op_4ebb(ULONG opcode)
{
{{	CPTR srca = m68k_getpc();
	srca = get_disp_ea (srca, nextiword());
{	regs.a[7] -= 4;
{	CPTR spa = regs.a[7];
	put_long(spa,m68k_getpc());
	m68k_setpc(srca);
}}}}}
void op_4ed0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	m68k_setpc(srca);
}}}
void op_4ed8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += 4;
	m68k_setpc(srca);
}}}}
void op_4ee0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 4;
{	CPTR srca = regs.a[srcreg];
	m68k_setpc(srca);
}}}}
void op_4ee8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	m68k_setpc(srca);
}}}
void op_4ef0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
	m68k_setpc(srca);
}}}
void op_4ef8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
	m68k_setpc(srca);
}}}
void op_4ef9(ULONG opcode)
{
{{	CPTR srca = nextilong();
	m68k_setpc(srca);
}}}
void op_4efa(ULONG opcode)
{
{{	CPTR srca = m68k_getpc();
	srca += (LONG)(WORD)nextiword();
	m68k_setpc(srca);
}}}
void op_4efb(ULONG opcode)
{
{{	CPTR srca = m68k_getpc();
	srca = get_disp_ea (srca, nextiword());
	m68k_setpc(srca);
}}}
