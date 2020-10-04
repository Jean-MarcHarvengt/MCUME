#include "shared.h"

#include "machdep/m68k.h"
#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "compiler.h"
#include "cputbl.h"
#if !defined (MEMFUNCS_DIRECT_REQUESTED) || defined (DIRECT_MEMFUNCS_SUCCESSFUL)
void REGPARAM2 CPU_OP_NAME(_d000)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d010)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
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
void REGPARAM2 CPU_OP_NAME(_d018)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
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
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d020)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
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
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d028)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
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
void REGPARAM2 CPU_OP_NAME(_d030)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
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
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d038)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
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
void REGPARAM2 CPU_OP_NAME(_d039)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
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
void REGPARAM2 CPU_OP_NAME(_d03a)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s8 src = get_byte(srca);
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
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d03b)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s8 src = get_byte(srca);
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
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d03c)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = nextibyte();
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
void REGPARAM2 CPU_OP_NAME(_d040)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d048)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d050)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
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
void REGPARAM2 CPU_OP_NAME(_d058)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
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
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d060)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
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
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d068)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
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
void REGPARAM2 CPU_OP_NAME(_d070)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
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
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d078)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
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
void REGPARAM2 CPU_OP_NAME(_d079)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
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
void REGPARAM2 CPU_OP_NAME(_d07a)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
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
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d07b)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
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
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d07c)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = nextiword();
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
void REGPARAM2 CPU_OP_NAME(_d080)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d088)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d090)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
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
void REGPARAM2 CPU_OP_NAME(_d098)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
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
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d0a0)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
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
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d0a8)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
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
void REGPARAM2 CPU_OP_NAME(_d0b0)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
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
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d0b8)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
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
void REGPARAM2 CPU_OP_NAME(_d0b9)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
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
void REGPARAM2 CPU_OP_NAME(_d0ba)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s32 src = get_long(srca);
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
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d0bb)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s32 src = get_long(srca);
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
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d0bc)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = nextilong();
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
void REGPARAM2 CPU_OP_NAME(_d0c0)(uae_u32 opcode) /* ADDA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_d0c8)(uae_u32 opcode) /* ADDA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg(regs, srcreg);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_d0d0)(uae_u32 opcode) /* ADDA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_d0d8)(uae_u32 opcode) /* ADDA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_d0e0)(uae_u32 opcode) /* ADDA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_d0e8)(uae_u32 opcode) /* ADDA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_d0f0)(uae_u32 opcode) /* ADDA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_d0f8)(uae_u32 opcode) /* ADDA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_d0f9)(uae_u32 opcode) /* ADDA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_d0fa)(uae_u32 opcode) /* ADDA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_d0fb)(uae_u32 opcode) /* ADDA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_d0fc)(uae_u32 opcode) /* ADDA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = nextiword();
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_d100)(uae_u32 opcode) /* ADDX */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = dst + src + (XFLG ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	VFLG = (flgs && flgo && !flgn) || (!flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && flgo) || (!flgn && (flgo || flgs));
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_d108)(uae_u32 opcode) /* ADDX */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	uae_u32 newv = dst + src + (XFLG ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	VFLG = (flgs && flgo && !flgn) || (!flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && flgo) || (!flgn && (flgo || flgs));
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	put_byte(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d110)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d118)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d120)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d128)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d130)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d138)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d139)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d140)(uae_u32 opcode) /* ADDX */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = dst + src + (XFLG ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	VFLG = (flgs && flgo && !flgn) || (!flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && flgo) || (!flgn && (flgo || flgs));
	if (((uae_s16)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s16)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_d148)(uae_u32 opcode) /* ADDX */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
{	uae_u32 newv = dst + src + (XFLG ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	VFLG = (flgs && flgo && !flgn) || (!flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && flgo) || (!flgn && (flgo || flgs));
	if (((uae_s16)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s16)(newv)) < 0;
	put_word(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d150)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d158)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d160)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d168)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d170)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d178)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d179)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d180)(uae_u32 opcode) /* ADDX */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = dst + src + (XFLG ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	VFLG = (flgs && flgo && !flgn) || (!flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && flgo) || (!flgn && (flgo || flgs));
	if (((uae_s32)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_d188)(uae_u32 opcode) /* ADDX */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{	uae_u32 newv = dst + src + (XFLG ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	VFLG = (flgs && flgo && !flgn) || (!flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && flgo) || (!flgn && (flgo || flgs));
	if (((uae_s32)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s32)(newv)) < 0;
	put_long(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_d190)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d198)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d1a0)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d1a8)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d1b0)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d1b8)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d1b9)(uae_u32 opcode) /* ADD */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg(regs, srcreg);
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
void REGPARAM2 CPU_OP_NAME(_d1c0)(uae_u32 opcode) /* ADDA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_d1c8)(uae_u32 opcode) /* ADDA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg(regs, srcreg);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_d1d0)(uae_u32 opcode) /* ADDA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_d1d8)(uae_u32 opcode) /* ADDA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_d1e0)(uae_u32 opcode) /* ADDA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_d1e8)(uae_u32 opcode) /* ADDA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_d1f0)(uae_u32 opcode) /* ADDA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_d1f8)(uae_u32 opcode) /* ADDA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_d1f9)(uae_u32 opcode) /* ADDA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_d1fa)(uae_u32 opcode) /* ADDA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_d1fb)(uae_u32 opcode) /* ADDA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_d1fc)(uae_u32 opcode) /* ADDA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = nextilong();
{	uae_s32 dst = m68k_areg(regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg(regs, dstreg) = (newv);
}}}}}
#endif
