#include "shared.h"

#include "memory.h"
#include "custom.h"
#include "newcpu.h"
#include "cputbl.h"
void op_5000(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5010(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5018(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5020(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5028(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5030(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5038(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
{{	ULONG src = srcreg;
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
void op_5039(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
{{	ULONG src = srcreg;
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
void op_5040(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5048(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
{	LONG dst = regs.a[dstreg];
{	ULONG newv = dst + src;
	regs.a[dstreg] = (newv);
}}}}}
void op_5050(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5058(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5060(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5068(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5070(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5078(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
{{	ULONG src = srcreg;
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
void op_5079(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
{{	ULONG src = srcreg;
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
void op_5080(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5088(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
{	LONG dst = regs.a[dstreg];
{	ULONG newv = dst + src;
	regs.a[dstreg] = (newv);
}}}}}
void op_5090(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5098(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_50a0(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_50a8(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_50b0(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_50b8(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
{{	ULONG src = srcreg;
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
void op_50b9(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
{{	ULONG src = srcreg;
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
void op_50c0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{{	int val = cctrue(0) ? 0xff : 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (val) & 0xff;
}}}}
void op_50c8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	WORD offs = nextiword();
	if (!cctrue(0)) {
	if (src--) regs.pc_p = (UWORD *)((char *)regs.pc_p + (LONG)offs - 2);
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (src) & 0xffff;
	}
}}}}
void op_50d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(0) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_50d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	int val = cctrue(0) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_50e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(0) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_50e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int val = cctrue(0) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_50f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int val = cctrue(0) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_50f8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	int val = cctrue(0) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_50f9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	int val = cctrue(0) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5100(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5110(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5118(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5120(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5128(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5130(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5138(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
{{	ULONG src = srcreg;
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
void op_5139(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
{{	ULONG src = srcreg;
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
void op_5140(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5148(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
{	LONG dst = regs.a[dstreg];
{	ULONG newv = dst - src;
	regs.a[dstreg] = (newv);
}}}}}
void op_5150(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5158(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5160(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5168(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5170(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5178(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
{{	ULONG src = srcreg;
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
void op_5179(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
{{	ULONG src = srcreg;
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
void op_5180(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5188(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
{	LONG dst = regs.a[dstreg];
{	ULONG newv = dst - src;
	regs.a[dstreg] = (newv);
}}}}}
void op_5190(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_5198(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_51a0(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_51a8(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_51b0(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG src = srcreg;
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
void op_51b8(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
{{	ULONG src = srcreg;
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
void op_51b9(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
{{	ULONG src = srcreg;
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
void op_51c0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{{	int val = cctrue(1) ? 0xff : 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (val) & 0xff;
}}}}
void op_51c8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	WORD offs = nextiword();
	if (!cctrue(1)) {
	if (src--) regs.pc_p = (UWORD *)((char *)regs.pc_p + (LONG)offs - 2);
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (src) & 0xffff;
	}
}}}}
void op_51d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(1) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_51d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	int val = cctrue(1) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_51e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(1) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_51e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int val = cctrue(1) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_51f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int val = cctrue(1) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_51f8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	int val = cctrue(1) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_51f9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	int val = cctrue(1) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_52c0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{{	int val = cctrue(2) ? 0xff : 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (val) & 0xff;
}}}}
void op_52c8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	WORD offs = nextiword();
	if (!cctrue(2)) {
	if (src--) regs.pc_p = (UWORD *)((char *)regs.pc_p + (LONG)offs - 2);
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (src) & 0xffff;
	}
}}}}
void op_52d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(2) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_52d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	int val = cctrue(2) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_52e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(2) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_52e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int val = cctrue(2) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_52f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int val = cctrue(2) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_52f8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	int val = cctrue(2) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_52f9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	int val = cctrue(2) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_53c0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{{	int val = cctrue(3) ? 0xff : 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (val) & 0xff;
}}}}
void op_53c8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	WORD offs = nextiword();
	if (!cctrue(3)) {
	if (src--) regs.pc_p = (UWORD *)((char *)regs.pc_p + (LONG)offs - 2);
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (src) & 0xffff;
	}
}}}}
void op_53d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(3) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_53d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	int val = cctrue(3) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_53e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(3) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_53e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int val = cctrue(3) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_53f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int val = cctrue(3) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_53f8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	int val = cctrue(3) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_53f9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	int val = cctrue(3) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_54c0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{{	int val = cctrue(4) ? 0xff : 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (val) & 0xff;
}}}}
void op_54c8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	WORD offs = nextiword();
	if (!cctrue(4)) {
	if (src--) regs.pc_p = (UWORD *)((char *)regs.pc_p + (LONG)offs - 2);
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (src) & 0xffff;
	}
}}}}
void op_54d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(4) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_54d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	int val = cctrue(4) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_54e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(4) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_54e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int val = cctrue(4) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_54f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int val = cctrue(4) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_54f8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	int val = cctrue(4) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_54f9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	int val = cctrue(4) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_55c0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{{	int val = cctrue(5) ? 0xff : 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (val) & 0xff;
}}}}
void op_55c8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	WORD offs = nextiword();
	if (!cctrue(5)) {
	if (src--) regs.pc_p = (UWORD *)((char *)regs.pc_p + (LONG)offs - 2);
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (src) & 0xffff;
	}
}}}}
void op_55d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(5) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_55d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	int val = cctrue(5) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_55e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(5) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_55e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int val = cctrue(5) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_55f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int val = cctrue(5) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_55f8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	int val = cctrue(5) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_55f9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	int val = cctrue(5) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_56c0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{{	int val = cctrue(6) ? 0xff : 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (val) & 0xff;
}}}}
void op_56c8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	WORD offs = nextiword();
	if (!cctrue(6)) {
	if (src--) regs.pc_p = (UWORD *)((char *)regs.pc_p + (LONG)offs - 2);
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (src) & 0xffff;
	}
}}}}
void op_56d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(6) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_56d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	int val = cctrue(6) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_56e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(6) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_56e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int val = cctrue(6) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_56f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int val = cctrue(6) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_56f8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	int val = cctrue(6) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_56f9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	int val = cctrue(6) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_57c0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{{	int val = cctrue(7) ? 0xff : 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (val) & 0xff;
}}}}
void op_57c8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	WORD offs = nextiword();
	if (!cctrue(7)) {
	if (src--) regs.pc_p = (UWORD *)((char *)regs.pc_p + (LONG)offs - 2);
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (src) & 0xffff;
	}
}}}}
void op_57d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(7) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_57d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	int val = cctrue(7) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_57e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(7) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_57e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int val = cctrue(7) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_57f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int val = cctrue(7) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_57f8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	int val = cctrue(7) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_57f9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	int val = cctrue(7) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_58c0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{{	int val = cctrue(8) ? 0xff : 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (val) & 0xff;
}}}}
void op_58c8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	WORD offs = nextiword();
	if (!cctrue(8)) {
	if (src--) regs.pc_p = (UWORD *)((char *)regs.pc_p + (LONG)offs - 2);
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (src) & 0xffff;
	}
}}}}
void op_58d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(8) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_58d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	int val = cctrue(8) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_58e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(8) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_58e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int val = cctrue(8) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_58f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int val = cctrue(8) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_58f8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	int val = cctrue(8) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_58f9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	int val = cctrue(8) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_59c0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{{	int val = cctrue(9) ? 0xff : 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (val) & 0xff;
}}}}
void op_59c8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	WORD offs = nextiword();
	if (!cctrue(9)) {
	if (src--) regs.pc_p = (UWORD *)((char *)regs.pc_p + (LONG)offs - 2);
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (src) & 0xffff;
	}
}}}}
void op_59d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(9) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_59d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	int val = cctrue(9) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_59e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(9) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_59e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int val = cctrue(9) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_59f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int val = cctrue(9) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_59f8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	int val = cctrue(9) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_59f9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	int val = cctrue(9) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5ac0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{{	int val = cctrue(10) ? 0xff : 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (val) & 0xff;
}}}}
void op_5ac8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	WORD offs = nextiword();
	if (!cctrue(10)) {
	if (src--) regs.pc_p = (UWORD *)((char *)regs.pc_p + (LONG)offs - 2);
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (src) & 0xffff;
	}
}}}}
void op_5ad0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(10) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5ad8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	int val = cctrue(10) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_5ae0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(10) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_5ae8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int val = cctrue(10) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5af0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int val = cctrue(10) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5af8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	int val = cctrue(10) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5af9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	int val = cctrue(10) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5bc0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{{	int val = cctrue(11) ? 0xff : 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (val) & 0xff;
}}}}
void op_5bc8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	WORD offs = nextiword();
	if (!cctrue(11)) {
	if (src--) regs.pc_p = (UWORD *)((char *)regs.pc_p + (LONG)offs - 2);
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (src) & 0xffff;
	}
}}}}
void op_5bd0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(11) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5bd8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	int val = cctrue(11) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_5be0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(11) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_5be8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int val = cctrue(11) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5bf0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int val = cctrue(11) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5bf8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	int val = cctrue(11) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5bf9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	int val = cctrue(11) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5cc0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{{	int val = cctrue(12) ? 0xff : 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (val) & 0xff;
}}}}
void op_5cc8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	WORD offs = nextiword();
	if (!cctrue(12)) {
	if (src--) regs.pc_p = (UWORD *)((char *)regs.pc_p + (LONG)offs - 2);
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (src) & 0xffff;
	}
}}}}
void op_5cd0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(12) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5cd8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	int val = cctrue(12) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_5ce0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(12) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_5ce8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int val = cctrue(12) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5cf0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int val = cctrue(12) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5cf8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	int val = cctrue(12) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5cf9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	int val = cctrue(12) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5dc0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{{	int val = cctrue(13) ? 0xff : 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (val) & 0xff;
}}}}
void op_5dc8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	WORD offs = nextiword();
	if (!cctrue(13)) {
	if (src--) regs.pc_p = (UWORD *)((char *)regs.pc_p + (LONG)offs - 2);
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (src) & 0xffff;
	}
}}}}
void op_5dd0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(13) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5dd8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	int val = cctrue(13) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_5de0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(13) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_5de8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int val = cctrue(13) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5df0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int val = cctrue(13) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5df8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	int val = cctrue(13) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5df9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	int val = cctrue(13) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5ec0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{{	int val = cctrue(14) ? 0xff : 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (val) & 0xff;
}}}}
void op_5ec8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	WORD offs = nextiword();
	if (!cctrue(14)) {
	if (src--) regs.pc_p = (UWORD *)((char *)regs.pc_p + (LONG)offs - 2);
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (src) & 0xffff;
	}
}}}}
void op_5ed0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(14) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5ed8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	int val = cctrue(14) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_5ee0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(14) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_5ee8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int val = cctrue(14) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5ef0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int val = cctrue(14) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5ef8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	int val = cctrue(14) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5ef9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	int val = cctrue(14) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5fc0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{{	int val = cctrue(15) ? 0xff : 0;
	regs.d[srcreg] &= ~0xff; regs.d[srcreg] |= (val) & 0xff;
}}}}
void op_5fc8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	WORD src = regs.d[srcreg];
{	WORD offs = nextiword();
	if (!cctrue(15)) {
	if (src--) regs.pc_p = (UWORD *)((char *)regs.pc_p + (LONG)offs - 2);
	regs.d[srcreg] &= ~0xffff; regs.d[srcreg] |= (src) & 0xffff;
	}
}}}}
void op_5fd0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(15) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5fd8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	int val = cctrue(15) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_5fe0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
{	int val = cctrue(15) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void op_5fe8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
{	int val = cctrue(15) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5ff0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR srca = regs.a[srcreg];
	srca = get_disp_ea (srca, nextiword());
{	int val = cctrue(15) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5ff8(ULONG opcode)
{
{{	CPTR srca = (LONG)(WORD)nextiword();
{	int val = cctrue(15) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void op_5ff9(ULONG opcode)
{
{{	CPTR srca = nextilong();
{	int val = cctrue(15) ? 0xff : 0;
	put_byte(srca,val);
}}}}
