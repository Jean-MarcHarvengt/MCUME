#include "shared.h"

#include "machdep/m68k.h"
#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "compiler.h"
#include "cputbl.h"
#if !defined (MEMFUNCS_DIRECT_REQUESTED) || defined (DIRECT_MEMFUNCS_SUCCESSFUL)
void REGPARAM2 CPU_OP_NAME(_5000)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s8 dst = m68k_dreg(regs, dstreg);
{{uae_u32 newv = ((uae_s8)(dst)) + ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(~dst)) < ((uae_u8)(src));
	NFLG = flgn != 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5010)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) + ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(~dst)) < ((uae_u8)(src));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5018)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
{{uae_u32 newv = ((uae_s8)(dst)) + ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(~dst)) < ((uae_u8)(src));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5020)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) + ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(~dst)) < ((uae_u8)(src));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5028)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) + ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(~dst)) < ((uae_u8)(src));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5030)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) + ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(~dst)) < ((uae_u8)(src));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5038)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) + ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(~dst)) < ((uae_u8)(src));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5039)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta = nextilong();
	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) + ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(~dst)) < ((uae_u8)(src));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5040)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{{uae_u32 newv = ((uae_s16)(dst)) + ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(~dst)) < ((uae_u16)(src));
	NFLG = flgn != 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5048)(uae_u32 opcode) /* ADDA */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_5050)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) + ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(~dst)) < ((uae_u16)(src));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5058)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
{	m68k_areg(regs, dstreg) += 2;
{{uae_u32 newv = ((uae_s16)(dst)) + ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(~dst)) < ((uae_u16)(src));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5060)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) + ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(~dst)) < ((uae_u16)(src));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5068)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) + ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(~dst)) < ((uae_u16)(src));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5070)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) + ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(~dst)) < ((uae_u16)(src));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5078)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) + ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(~dst)) < ((uae_u16)(src));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5079)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta = nextilong();
	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) + ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(~dst)) < ((uae_u16)(src));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5080)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_dreg(regs, dstreg);
{{uae_u32 newv = ((uae_s32)(dst)) + ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(~dst)) < ((uae_u32)(src));
	NFLG = flgn != 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5088)(uae_u32 opcode) /* ADDA */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_5090)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) + ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(~dst)) < ((uae_u32)(src));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5098)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{	m68k_areg(regs, dstreg) += 4;
{{uae_u32 newv = ((uae_s32)(dst)) + ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(~dst)) < ((uae_u32)(src));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_50a0)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) + ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(~dst)) < ((uae_u32)(src));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_50a8)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) + ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(~dst)) < ((uae_u32)(src));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_50b0)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) + ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(~dst)) < ((uae_u32)(src));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_50b8)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) + ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(~dst)) < ((uae_u32)(src));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_50b9)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta = nextilong();
	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) + ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs == flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(~dst)) < ((uae_u32)(src));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_50c0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{{	int val = cctrue(0) ? 0xff : 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_50c8)(uae_u32 opcode) /* DBcc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 offs = nextiword();
	if (!cctrue(0)) {
		if (src-- & 0xFFFF) {
#ifdef USE_POINTER
			regs.pc_p = (uae_u8 *)((char *)regs.pc_p + (uae_s32)offs - 2);
#else
			regs.pc = regs.pc + (uae_s32)offs - 2;
#endif			
			fill_prefetch();
		}
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((src) & 0xffff);
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_50d0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(0) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_50d8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	int val = cctrue(0) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_50e0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(0) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_50e8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(0) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_50f0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	int val = cctrue(0) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_50f8)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(0) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_50f9)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = nextilong();
{	int val = cctrue(0) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5100)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s8 dst = m68k_dreg(regs, dstreg);
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5110)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5118)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5120)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5128)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5130)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5138)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5139)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta = nextilong();
	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
	put_byte(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5140)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5148)(uae_u32 opcode) /* SUBA */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_5150)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5158)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
{	m68k_areg(regs, dstreg) += 2;
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5160)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5168)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5170)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5178)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5179)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta = nextilong();
	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
	put_word(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5180)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_dreg(regs, dstreg);
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5188)(uae_u32 opcode) /* SUBA */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_5190)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_5198)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{	m68k_areg(regs, dstreg) += 4;
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_51a0)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_51a8)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_51b0)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_51b8)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_51b9)(uae_u32 opcode) /* SUB */
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta = nextilong();
	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
	put_long(dsta,newv);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_51c0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{{	int val = cctrue(1) ? 0xff : 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_51c8)(uae_u32 opcode) /* DBcc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 offs = nextiword();
	if (!cctrue(1)) {
		if (src-- & 0xFFFF) {
#ifdef USE_POINTER
			regs.pc_p = (uae_u8 *)((char *)regs.pc_p + (uae_s32)offs - 2);
#else
			regs.pc = regs.pc + (uae_s32)offs - 2;
#endif			
			fill_prefetch();
		}
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((src) & 0xffff);
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_51d0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(1) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_51d8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	int val = cctrue(1) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_51e0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(1) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_51e8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(1) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_51f0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	int val = cctrue(1) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_51f8)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(1) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_51f9)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = nextilong();
{	int val = cctrue(1) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_52c0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{{	int val = cctrue(2) ? 0xff : 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_52c8)(uae_u32 opcode) /* DBcc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 offs = nextiword();
	if (!cctrue(2)) {
		if (src-- & 0xFFFF) {
#ifdef USE_POINTER
			regs.pc_p = (uae_u8 *)((char *)regs.pc_p + (uae_s32)offs - 2);
#else
			regs.pc = regs.pc + (uae_s32)offs - 2;
#endif			
			fill_prefetch();
		}
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((src) & 0xffff);
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_52d0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(2) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_52d8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	int val = cctrue(2) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_52e0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(2) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_52e8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(2) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_52f0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	int val = cctrue(2) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_52f8)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(2) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_52f9)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = nextilong();
{	int val = cctrue(2) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_53c0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{{	int val = cctrue(3) ? 0xff : 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_53c8)(uae_u32 opcode) /* DBcc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 offs = nextiword();
	if (!cctrue(3)) {
		if (src-- & 0xFFFF) {
#ifdef USE_POINTER
			regs.pc_p = (uae_u8 *)((char *)regs.pc_p + (uae_s32)offs - 2);
#else
			regs.pc = regs.pc + (uae_s32)offs - 2;
#endif			
			fill_prefetch();
		}
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((src) & 0xffff);
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_53d0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(3) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_53d8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	int val = cctrue(3) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_53e0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(3) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_53e8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(3) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_53f0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	int val = cctrue(3) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_53f8)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(3) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_53f9)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = nextilong();
{	int val = cctrue(3) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_54c0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{{	int val = cctrue(4) ? 0xff : 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_54c8)(uae_u32 opcode) /* DBcc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 offs = nextiword();
	if (!cctrue(4)) {
		if (src-- & 0xFFFF) {
#ifdef USE_POINTER
			regs.pc_p = (uae_u8 *)((char *)regs.pc_p + (uae_s32)offs - 2);
#else
			regs.pc = regs.pc + (uae_s32)offs - 2;
#endif			
			fill_prefetch();
		}
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((src) & 0xffff);
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_54d0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(4) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_54d8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	int val = cctrue(4) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_54e0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(4) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_54e8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(4) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_54f0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	int val = cctrue(4) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_54f8)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(4) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_54f9)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = nextilong();
{	int val = cctrue(4) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_55c0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{{	int val = cctrue(5) ? 0xff : 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_55c8)(uae_u32 opcode) /* DBcc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 offs = nextiword();
	if (!cctrue(5)) {
		if (src-- & 0xFFFF) {
#ifdef USE_POINTER
			regs.pc_p = (uae_u8 *)((char *)regs.pc_p + (uae_s32)offs - 2);
#else
			regs.pc = regs.pc + (uae_s32)offs - 2;
#endif			
			fill_prefetch();
		}
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((src) & 0xffff);
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_55d0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(5) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_55d8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	int val = cctrue(5) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_55e0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(5) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_55e8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(5) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_55f0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	int val = cctrue(5) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_55f8)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(5) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_55f9)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = nextilong();
{	int val = cctrue(5) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_56c0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{{	int val = cctrue(6) ? 0xff : 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_56c8)(uae_u32 opcode) /* DBcc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 offs = nextiword();
	if (!cctrue(6)) {
		if (src-- & 0xFFFF) {
#ifdef USE_POINTER
			regs.pc_p = (uae_u8 *)((char *)regs.pc_p + (uae_s32)offs - 2);
#else
			regs.pc = regs.pc + (uae_s32)offs - 2;
#endif			
			fill_prefetch();
		}
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((src) & 0xffff);
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_56d0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(6) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_56d8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	int val = cctrue(6) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_56e0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(6) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_56e8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(6) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_56f0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	int val = cctrue(6) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_56f8)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(6) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_56f9)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = nextilong();
{	int val = cctrue(6) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_57c0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{{	int val = cctrue(7) ? 0xff : 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_57c8)(uae_u32 opcode) /* DBcc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 offs = nextiword();
	if (!cctrue(7)) {
		if (src-- & 0xFFFF) {
#ifdef USE_POINTER
			regs.pc_p = (uae_u8 *)((char *)regs.pc_p + (uae_s32)offs - 2);
#else
			regs.pc = regs.pc + (uae_s32)offs - 2;
#endif			
			fill_prefetch();
		}
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((src) & 0xffff);
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_57d0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(7) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_57d8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	int val = cctrue(7) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_57e0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(7) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_57e8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(7) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_57f0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	int val = cctrue(7) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_57f8)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(7) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_57f9)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = nextilong();
{	int val = cctrue(7) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_58c0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{{	int val = cctrue(8) ? 0xff : 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_58c8)(uae_u32 opcode) /* DBcc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 offs = nextiword();
	if (!cctrue(8)) {
		if (src-- & 0xFFFF) {
#ifdef USE_POINTER
			regs.pc_p = (uae_u8 *)((char *)regs.pc_p + (uae_s32)offs - 2);
#else
			regs.pc = regs.pc + (uae_s32)offs - 2;
#endif			
			fill_prefetch();
		}
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((src) & 0xffff);
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_58d0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(8) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_58d8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	int val = cctrue(8) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_58e0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(8) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_58e8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(8) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_58f0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	int val = cctrue(8) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_58f8)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(8) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_58f9)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = nextilong();
{	int val = cctrue(8) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_59c0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{{	int val = cctrue(9) ? 0xff : 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_59c8)(uae_u32 opcode) /* DBcc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 offs = nextiword();
	if (!cctrue(9)) {
		if (src-- & 0xFFFF) {
#ifdef USE_POINTER
			regs.pc_p = (uae_u8 *)((char *)regs.pc_p + (uae_s32)offs - 2);
#else
			regs.pc = regs.pc + (uae_s32)offs - 2;
#endif			
			fill_prefetch();
		}
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((src) & 0xffff);
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_59d0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(9) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_59d8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	int val = cctrue(9) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_59e0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(9) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_59e8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(9) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_59f0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	int val = cctrue(9) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_59f8)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(9) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_59f9)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = nextilong();
{	int val = cctrue(9) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5ac0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{{	int val = cctrue(10) ? 0xff : 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_5ac8)(uae_u32 opcode) /* DBcc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 offs = nextiword();
	if (!cctrue(10)) {
		if (src-- & 0xFFFF) {
#ifdef USE_POINTER
			regs.pc_p = (uae_u8 *)((char *)regs.pc_p + (uae_s32)offs - 2);
#else
			regs.pc = regs.pc + (uae_s32)offs - 2;
#endif			
			fill_prefetch();
		}
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((src) & 0xffff);
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_5ad0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(10) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5ad8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	int val = cctrue(10) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_5ae0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(10) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_5ae8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(10) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5af0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	int val = cctrue(10) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5af8)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(10) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5af9)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = nextilong();
{	int val = cctrue(10) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5bc0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{{	int val = cctrue(11) ? 0xff : 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_5bc8)(uae_u32 opcode) /* DBcc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 offs = nextiword();
	if (!cctrue(11)) {
		if (src-- & 0xFFFF) {
#ifdef USE_POINTER
			regs.pc_p = (uae_u8 *)((char *)regs.pc_p + (uae_s32)offs - 2);
#else
			regs.pc = regs.pc + (uae_s32)offs - 2;
#endif			
			fill_prefetch();
		}
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((src) & 0xffff);
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_5bd0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(11) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5bd8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	int val = cctrue(11) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_5be0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(11) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_5be8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(11) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5bf0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	int val = cctrue(11) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5bf8)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(11) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5bf9)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = nextilong();
{	int val = cctrue(11) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5cc0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{{	int val = cctrue(12) ? 0xff : 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_5cc8)(uae_u32 opcode) /* DBcc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 offs = nextiword();
	if (!cctrue(12)) {
		if (src-- & 0xFFFF) {
#ifdef USE_POINTER
			regs.pc_p = (uae_u8 *)((char *)regs.pc_p + (uae_s32)offs - 2);
#else
			regs.pc = regs.pc + (uae_s32)offs - 2;
#endif			
			fill_prefetch();
		}
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((src) & 0xffff);
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_5cd0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(12) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5cd8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	int val = cctrue(12) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_5ce0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(12) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_5ce8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(12) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5cf0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	int val = cctrue(12) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5cf8)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(12) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5cf9)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = nextilong();
{	int val = cctrue(12) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5dc0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{{	int val = cctrue(13) ? 0xff : 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_5dc8)(uae_u32 opcode) /* DBcc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 offs = nextiword();
	if (!cctrue(13)) {
		if (src-- & 0xFFFF) {
#ifdef USE_POINTER
			regs.pc_p = (uae_u8 *)((char *)regs.pc_p + (uae_s32)offs - 2);
#else
			regs.pc = regs.pc + (uae_s32)offs - 2;
#endif			
			fill_prefetch();
		}
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((src) & 0xffff);
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_5dd0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(13) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5dd8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	int val = cctrue(13) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_5de0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(13) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_5de8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(13) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5df0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	int val = cctrue(13) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5df8)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(13) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5df9)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = nextilong();
{	int val = cctrue(13) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5ec0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{{	int val = cctrue(14) ? 0xff : 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_5ec8)(uae_u32 opcode) /* DBcc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 offs = nextiword();
	if (!cctrue(14)) {
		if (src-- & 0xFFFF) {
#ifdef USE_POINTER
			regs.pc_p = (uae_u8 *)((char *)regs.pc_p + (uae_s32)offs - 2);
#else
			regs.pc = regs.pc + (uae_s32)offs - 2;
#endif			
			fill_prefetch();
		}
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((src) & 0xffff);
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_5ed0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(14) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5ed8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	int val = cctrue(14) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_5ee0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(14) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_5ee8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(14) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5ef0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	int val = cctrue(14) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5ef8)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(14) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5ef9)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = nextilong();
{	int val = cctrue(14) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5fc0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{{	int val = cctrue(15) ? 0xff : 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_5fc8)(uae_u32 opcode) /* DBcc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 offs = nextiword();
	if (!cctrue(15)) {
		if (src-- & 0xFFFF) {
#ifdef USE_POINTER
			regs.pc_p = (uae_u8 *)((char *)regs.pc_p + (uae_s32)offs - 2);
#else
			regs.pc = regs.pc + (uae_s32)offs - 2;
#endif			
			fill_prefetch();
		}
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((src) & 0xffff);
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_5fd0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(15) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5fd8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	int val = cctrue(15) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_5fe0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
{	int val = cctrue(15) ? 0xff : 0;
	put_byte(srca,val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_5fe8)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(15) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5ff0)(uae_u32 opcode) /* Scc */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	int val = cctrue(15) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5ff8)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	int val = cctrue(15) ? 0xff : 0;
	put_byte(srca,val);
}}}}
void REGPARAM2 CPU_OP_NAME(_5ff9)(uae_u32 opcode) /* Scc */
{
{{	uaecptr srca = nextilong();
{	int val = cctrue(15) ? 0xff : 0;
	put_byte(srca,val);
}}}}
#endif
