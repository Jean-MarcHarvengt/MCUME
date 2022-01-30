#include "shared.h"

#include "machdep/m68k.h"
#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "compiler.h"
#include "cputbl.h"
#if !defined (MEMFUNCS_DIRECT_REQUESTED) || defined (DIRECT_MEMFUNCS_SUCCESSFUL)
void REGPARAM2 CPU_OP_NAME(_4000)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((newv) & 0xff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4010)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	put_byte(srca,newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4018)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	put_byte(srca,newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4020)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	put_byte(srca,newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4028)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	put_byte(srca,newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4030)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	put_byte(srca,newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4038)(uae_u32 opcode) /* NEGX */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	put_byte(srca,newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4039)(uae_u32 opcode) /* NEGX */
{
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	put_byte(srca,newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4040)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s16)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s16)(newv)) < 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4050)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s16)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s16)(newv)) < 0;
	put_word(srca,newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4058)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s16)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s16)(newv)) < 0;
	put_word(srca,newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4060)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s16)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s16)(newv)) < 0;
	put_word(srca,newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4068)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s16)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s16)(newv)) < 0;
	put_word(srca,newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4070)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s16)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s16)(newv)) < 0;
	put_word(srca,newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4078)(uae_u32 opcode) /* NEGX */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s16)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s16)(newv)) < 0;
	put_word(srca,newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4079)(uae_u32 opcode) /* NEGX */
{
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s16)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s16)(newv)) < 0;
	put_word(srca,newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4080)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s32)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, srcreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4090)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s32)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s32)(newv)) < 0;
	put_long(srca,newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4098)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s32)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s32)(newv)) < 0;
	put_long(srca,newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_40a0)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s32)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s32)(newv)) < 0;
	put_long(srca,newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_40a8)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s32)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s32)(newv)) < 0;
	put_long(srca,newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_40b0)(uae_u32 opcode) /* NEGX */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s32)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s32)(newv)) < 0;
	put_long(srca,newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_40b8)(uae_u32 opcode) /* NEGX */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s32)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s32)(newv)) < 0;
	put_long(srca,newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_40b9)(uae_u32 opcode) /* NEGX */
{
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
{	uae_u32 newv = 0 - src - (XFLG ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	VFLG = (!flgs && flgo && !flgn) || (flgs && !flgo && flgn);
	XFLG = CFLG = (flgs && !flgo) || (flgn && (!flgo || flgs));
	if (((uae_s32)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s32)(newv)) < 0;
	put_long(srca,newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_40c0)(uae_u32 opcode) /* MVSR2 */
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	MakeSR();
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((regs.sr) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_40d0)(uae_u32 opcode) /* MVSR2 */
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uaecptr srca = m68k_areg(regs, srcreg);
	MakeSR();
	put_word(srca,regs.sr);
}}}}
void REGPARAM2 CPU_OP_NAME(_40d8)(uae_u32 opcode) /* MVSR2 */
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += 2;
	MakeSR();
	put_word(srca,regs.sr);
}}}}}
void REGPARAM2 CPU_OP_NAME(_40e0)(uae_u32 opcode) /* MVSR2 */
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	MakeSR();
	put_word(srca,regs.sr);
}}}}}
void REGPARAM2 CPU_OP_NAME(_40e8)(uae_u32 opcode) /* MVSR2 */
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	MakeSR();
	put_word(srca,regs.sr);
}}}}
void REGPARAM2 CPU_OP_NAME(_40f0)(uae_u32 opcode) /* MVSR2 */
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
	MakeSR();
	put_word(srca,regs.sr);
}}}}
void REGPARAM2 CPU_OP_NAME(_40f8)(uae_u32 opcode) /* MVSR2 */
{
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	MakeSR();
	put_word(srca,regs.sr);
}}}}
void REGPARAM2 CPU_OP_NAME(_40f9)(uae_u32 opcode) /* MVSR2 */
{
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uaecptr srca = nextilong();
	MakeSR();
	put_word(srca,regs.sr);
}}}}
void REGPARAM2 CPU_OP_NAME(_4100)(uae_u32 opcode) /* CHK */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}
void REGPARAM2 CPU_OP_NAME(_4110)(uae_u32 opcode) /* CHK */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}
void REGPARAM2 CPU_OP_NAME(_4118)(uae_u32 opcode) /* CHK */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}}
void REGPARAM2 CPU_OP_NAME(_4120)(uae_u32 opcode) /* CHK */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}}
void REGPARAM2 CPU_OP_NAME(_4128)(uae_u32 opcode) /* CHK */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}
void REGPARAM2 CPU_OP_NAME(_4130)(uae_u32 opcode) /* CHK */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}}
void REGPARAM2 CPU_OP_NAME(_4138)(uae_u32 opcode) /* CHK */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}
void REGPARAM2 CPU_OP_NAME(_4139)(uae_u32 opcode) /* CHK */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}
void REGPARAM2 CPU_OP_NAME(_413a)(uae_u32 opcode) /* CHK */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}}
void REGPARAM2 CPU_OP_NAME(_413b)(uae_u32 opcode) /* CHK */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}}
void REGPARAM2 CPU_OP_NAME(_413c)(uae_u32 opcode) /* CHK */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uae_s32 src = nextilong();
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}
void REGPARAM2 CPU_OP_NAME(_4180)(uae_u32 opcode) /* CHK */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}
void REGPARAM2 CPU_OP_NAME(_4190)(uae_u32 opcode) /* CHK */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}
void REGPARAM2 CPU_OP_NAME(_4198)(uae_u32 opcode) /* CHK */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}}
void REGPARAM2 CPU_OP_NAME(_41a0)(uae_u32 opcode) /* CHK */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}}
void REGPARAM2 CPU_OP_NAME(_41a8)(uae_u32 opcode) /* CHK */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}
void REGPARAM2 CPU_OP_NAME(_41b0)(uae_u32 opcode) /* CHK */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}}
void REGPARAM2 CPU_OP_NAME(_41b8)(uae_u32 opcode) /* CHK */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}
void REGPARAM2 CPU_OP_NAME(_41b9)(uae_u32 opcode) /* CHK */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}
void REGPARAM2 CPU_OP_NAME(_41ba)(uae_u32 opcode) /* CHK */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}}
void REGPARAM2 CPU_OP_NAME(_41bb)(uae_u32 opcode) /* CHK */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}}
void REGPARAM2 CPU_OP_NAME(_41bc)(uae_u32 opcode) /* CHK */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 src = nextiword();
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	if ((uae_s32)dst < 0) { NFLG=1; Exception(6,oldpc-2); }
	else if (dst > src) { NFLG=0; Exception(6,oldpc-2); }
}}}}
void REGPARAM2 CPU_OP_NAME(_41d0)(uae_u32 opcode) /* LEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, dstreg) = (srca);
}}}}
void REGPARAM2 CPU_OP_NAME(_41e8)(uae_u32 opcode) /* LEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	m68k_areg(regs, dstreg) = (srca);
}}}}
void REGPARAM2 CPU_OP_NAME(_41f0)(uae_u32 opcode) /* LEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	m68k_areg(regs, dstreg) = (srca);
}}}}
void REGPARAM2 CPU_OP_NAME(_41f8)(uae_u32 opcode) /* LEA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	m68k_areg(regs, dstreg) = (srca);
}}}}
void REGPARAM2 CPU_OP_NAME(_41f9)(uae_u32 opcode) /* LEA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
{	m68k_areg(regs, dstreg) = (srca);
}}}}
void REGPARAM2 CPU_OP_NAME(_41fa)(uae_u32 opcode) /* LEA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	m68k_areg(regs, dstreg) = (srca);
}}}}
void REGPARAM2 CPU_OP_NAME(_41fb)(uae_u32 opcode) /* LEA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	m68k_areg(regs, dstreg) = (srca);
}}}}
void REGPARAM2 CPU_OP_NAME(_4200)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(0)) == 0;
	NFLG = ((uae_s8)(0)) < 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((0) & 0xff);
}}}
void REGPARAM2 CPU_OP_NAME(_4210)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(0)) == 0;
	NFLG = ((uae_s8)(0)) < 0;
	put_byte(srca,0);
}}}
void REGPARAM2 CPU_OP_NAME(_4218)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(0)) == 0;
	NFLG = ((uae_s8)(0)) < 0;
	put_byte(srca,0);
}}}}
void REGPARAM2 CPU_OP_NAME(_4220)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(0)) == 0;
	NFLG = ((uae_s8)(0)) < 0;
	put_byte(srca,0);
}}}}
void REGPARAM2 CPU_OP_NAME(_4228)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(0)) == 0;
	NFLG = ((uae_s8)(0)) < 0;
	put_byte(srca,0);
}}}
void REGPARAM2 CPU_OP_NAME(_4230)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(0)) == 0;
	NFLG = ((uae_s8)(0)) < 0;
	put_byte(srca,0);
}}}
void REGPARAM2 CPU_OP_NAME(_4238)(uae_u32 opcode) /* CLR */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(0)) == 0;
	NFLG = ((uae_s8)(0)) < 0;
	put_byte(srca,0);
}}}
void REGPARAM2 CPU_OP_NAME(_4239)(uae_u32 opcode) /* CLR */
{
{{	uaecptr srca = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(0)) == 0;
	NFLG = ((uae_s8)(0)) < 0;
	put_byte(srca,0);
}}}
void REGPARAM2 CPU_OP_NAME(_4240)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(0)) == 0;
	NFLG = ((uae_s16)(0)) < 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((0) & 0xffff);
}}}
void REGPARAM2 CPU_OP_NAME(_4250)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(0)) == 0;
	NFLG = ((uae_s16)(0)) < 0;
	put_word(srca,0);
}}}
void REGPARAM2 CPU_OP_NAME(_4258)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += 2;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(0)) == 0;
	NFLG = ((uae_s16)(0)) < 0;
	put_word(srca,0);
}}}}
void REGPARAM2 CPU_OP_NAME(_4260)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(0)) == 0;
	NFLG = ((uae_s16)(0)) < 0;
	put_word(srca,0);
}}}}
void REGPARAM2 CPU_OP_NAME(_4268)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(0)) == 0;
	NFLG = ((uae_s16)(0)) < 0;
	put_word(srca,0);
}}}
void REGPARAM2 CPU_OP_NAME(_4270)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(0)) == 0;
	NFLG = ((uae_s16)(0)) < 0;
	put_word(srca,0);
}}}
void REGPARAM2 CPU_OP_NAME(_4278)(uae_u32 opcode) /* CLR */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(0)) == 0;
	NFLG = ((uae_s16)(0)) < 0;
	put_word(srca,0);
}}}
void REGPARAM2 CPU_OP_NAME(_4279)(uae_u32 opcode) /* CLR */
{
{{	uaecptr srca = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(0)) == 0;
	NFLG = ((uae_s16)(0)) < 0;
	put_word(srca,0);
}}}
void REGPARAM2 CPU_OP_NAME(_4280)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(0)) == 0;
	NFLG = ((uae_s32)(0)) < 0;
	m68k_dreg(regs, srcreg) = (0);
}}}
void REGPARAM2 CPU_OP_NAME(_4290)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(0)) == 0;
	NFLG = ((uae_s32)(0)) < 0;
	put_long(srca,0);
}}}
void REGPARAM2 CPU_OP_NAME(_4298)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += 4;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(0)) == 0;
	NFLG = ((uae_s32)(0)) < 0;
	put_long(srca,0);
}}}}
void REGPARAM2 CPU_OP_NAME(_42a0)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(0)) == 0;
	NFLG = ((uae_s32)(0)) < 0;
	put_long(srca,0);
}}}}
void REGPARAM2 CPU_OP_NAME(_42a8)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(0)) == 0;
	NFLG = ((uae_s32)(0)) < 0;
	put_long(srca,0);
}}}
void REGPARAM2 CPU_OP_NAME(_42b0)(uae_u32 opcode) /* CLR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(0)) == 0;
	NFLG = ((uae_s32)(0)) < 0;
	put_long(srca,0);
}}}
void REGPARAM2 CPU_OP_NAME(_42b8)(uae_u32 opcode) /* CLR */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(0)) == 0;
	NFLG = ((uae_s32)(0)) < 0;
	put_long(srca,0);
}}}
void REGPARAM2 CPU_OP_NAME(_42b9)(uae_u32 opcode) /* CLR */
{
{{	uaecptr srca = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(0)) == 0;
	NFLG = ((uae_s32)(0)) < 0;
	put_long(srca,0);
}}}
void REGPARAM2 CPU_OP_NAME(_42c0)(uae_u32 opcode) /* MVSR2 */
{
	uae_u32 srcreg = (opcode & 7);
{{	MakeSR();
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((regs.sr & 0xff) & 0xffff);
}}}
void REGPARAM2 CPU_OP_NAME(_42d0)(uae_u32 opcode) /* MVSR2 */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	MakeSR();
	put_word(srca,regs.sr & 0xff);
}}}
void REGPARAM2 CPU_OP_NAME(_42d8)(uae_u32 opcode) /* MVSR2 */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, srcreg) += 2;
	MakeSR();
	put_word(srca,regs.sr & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_42e0)(uae_u32 opcode) /* MVSR2 */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	MakeSR();
	put_word(srca,regs.sr & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_42e8)(uae_u32 opcode) /* MVSR2 */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	MakeSR();
	put_word(srca,regs.sr & 0xff);
}}}
void REGPARAM2 CPU_OP_NAME(_42f0)(uae_u32 opcode) /* MVSR2 */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
	MakeSR();
	put_word(srca,regs.sr & 0xff);
}}}
void REGPARAM2 CPU_OP_NAME(_42f8)(uae_u32 opcode) /* MVSR2 */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	MakeSR();
	put_word(srca,regs.sr & 0xff);
}}}
void REGPARAM2 CPU_OP_NAME(_42f9)(uae_u32 opcode) /* MVSR2 */
{
{{	uaecptr srca = nextilong();
	MakeSR();
	put_word(srca,regs.sr & 0xff);
}}}
void REGPARAM2 CPU_OP_NAME(_4400)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{{uae_u32 dst = ((uae_s8)(0)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(dst)) < 0;
	ZFLG = ((uae_s8)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(src)) > ((uae_u8)(0));
	NFLG = flgn != 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((dst) & 0xff);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4410)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{{uae_u32 dst = ((uae_s8)(0)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(dst)) < 0;
	ZFLG = ((uae_s8)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(src)) > ((uae_u8)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4418)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{{uae_u32 dst = ((uae_s8)(0)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(dst)) < 0;
	ZFLG = ((uae_s8)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(src)) > ((uae_u8)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_4420)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{{uae_u32 dst = ((uae_s8)(0)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(dst)) < 0;
	ZFLG = ((uae_s8)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(src)) > ((uae_u8)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_4428)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{{uae_u32 dst = ((uae_s8)(0)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(dst)) < 0;
	ZFLG = ((uae_s8)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(src)) > ((uae_u8)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4430)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
{{uae_u32 dst = ((uae_s8)(0)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(dst)) < 0;
	ZFLG = ((uae_s8)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(src)) > ((uae_u8)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_4438)(uae_u32 opcode) /* NEG */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{{uae_u32 dst = ((uae_s8)(0)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(dst)) < 0;
	ZFLG = ((uae_s8)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(src)) > ((uae_u8)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4439)(uae_u32 opcode) /* NEG */
{
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
{{uae_u32 dst = ((uae_s8)(0)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(dst)) < 0;
	ZFLG = ((uae_s8)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u8)(src)) > ((uae_u8)(0));
	NFLG = flgn != 0;
	put_byte(srca,dst);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4440)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{{uae_u32 dst = ((uae_s16)(0)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(dst)) < 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(src)) > ((uae_u16)(0));
	NFLG = flgn != 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((dst) & 0xffff);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4450)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{{uae_u32 dst = ((uae_s16)(0)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(dst)) < 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(src)) > ((uae_u16)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4458)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{{uae_u32 dst = ((uae_s16)(0)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(dst)) < 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(src)) > ((uae_u16)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_4460)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{{uae_u32 dst = ((uae_s16)(0)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(dst)) < 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(src)) > ((uae_u16)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_4468)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{{uae_u32 dst = ((uae_s16)(0)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(dst)) < 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(src)) > ((uae_u16)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4470)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{{uae_u32 dst = ((uae_s16)(0)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(dst)) < 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(src)) > ((uae_u16)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_4478)(uae_u32 opcode) /* NEG */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{{uae_u32 dst = ((uae_s16)(0)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(dst)) < 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(src)) > ((uae_u16)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4479)(uae_u32 opcode) /* NEG */
{
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{{uae_u32 dst = ((uae_s16)(0)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(dst)) < 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u16)(src)) > ((uae_u16)(0));
	NFLG = flgn != 0;
	put_word(srca,dst);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4480)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{{uae_u32 dst = ((uae_s32)(0)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(dst)) < 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(src)) > ((uae_u32)(0));
	NFLG = flgn != 0;
	m68k_dreg(regs, srcreg) = (dst);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4490)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{{uae_u32 dst = ((uae_s32)(0)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(dst)) < 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(src)) > ((uae_u32)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4498)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
{{uae_u32 dst = ((uae_s32)(0)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(dst)) < 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(src)) > ((uae_u32)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_44a0)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{{uae_u32 dst = ((uae_s32)(0)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(dst)) < 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(src)) > ((uae_u32)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_44a8)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{{uae_u32 dst = ((uae_s32)(0)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(dst)) < 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(src)) > ((uae_u32)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_44b0)(uae_u32 opcode) /* NEG */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
{{uae_u32 dst = ((uae_s32)(0)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(dst)) < 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(src)) > ((uae_u32)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_44b8)(uae_u32 opcode) /* NEG */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{{uae_u32 dst = ((uae_s32)(0)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(dst)) < 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(src)) > ((uae_u32)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_44b9)(uae_u32 opcode) /* NEG */
{
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
{{uae_u32 dst = ((uae_s32)(0)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(dst)) < 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = XFLG = ((uae_u32)(src)) > ((uae_u32)(0));
	NFLG = flgn != 0;
	put_long(srca,dst);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_44c0)(uae_u32 opcode) /* MV2SR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}
void REGPARAM2 CPU_OP_NAME(_44d0)(uae_u32 opcode) /* MV2SR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}
void REGPARAM2 CPU_OP_NAME(_44d8)(uae_u32 opcode) /* MV2SR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}}
void REGPARAM2 CPU_OP_NAME(_44e0)(uae_u32 opcode) /* MV2SR */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}}
void REGPARAM2 CPU_OP_NAME(_44e8)(uae_u32 opcode) /* MV2SR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}
void REGPARAM2 CPU_OP_NAME(_44f0)(uae_u32 opcode) /* MV2SR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}}
void REGPARAM2 CPU_OP_NAME(_44f8)(uae_u32 opcode) /* MV2SR */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}
void REGPARAM2 CPU_OP_NAME(_44f9)(uae_u32 opcode) /* MV2SR */
{
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}
void REGPARAM2 CPU_OP_NAME(_44fa)(uae_u32 opcode) /* MV2SR */
{
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}}
void REGPARAM2 CPU_OP_NAME(_44fb)(uae_u32 opcode) /* MV2SR */
{
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}}
void REGPARAM2 CPU_OP_NAME(_44fc)(uae_u32 opcode) /* MV2SR */
{
{{	uae_s16 src = nextiword();
	MakeSR();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR();
}}}
void REGPARAM2 CPU_OP_NAME(_4600)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(dst)) == 0;
	NFLG = ((uae_s8)(dst)) < 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((dst) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_4610)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(dst)) == 0;
	NFLG = ((uae_s8)(dst)) < 0;
	put_byte(srca,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_4618)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(dst)) == 0;
	NFLG = ((uae_s8)(dst)) < 0;
	put_byte(srca,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4620)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(dst)) == 0;
	NFLG = ((uae_s8)(dst)) < 0;
	put_byte(srca,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4628)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(dst)) == 0;
	NFLG = ((uae_s8)(dst)) < 0;
	put_byte(srca,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_4630)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(dst)) == 0;
	NFLG = ((uae_s8)(dst)) < 0;
	put_byte(srca,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4638)(uae_u32 opcode) /* NOT */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(dst)) == 0;
	NFLG = ((uae_s8)(dst)) < 0;
	put_byte(srca,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_4639)(uae_u32 opcode) /* NOT */
{
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(dst)) == 0;
	NFLG = ((uae_s8)(dst)) < 0;
	put_byte(srca,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_4640)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	NFLG = ((uae_s16)(dst)) < 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((dst) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_4650)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	NFLG = ((uae_s16)(dst)) < 0;
	put_word(srca,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_4658)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	NFLG = ((uae_s16)(dst)) < 0;
	put_word(srca,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4660)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	NFLG = ((uae_s16)(dst)) < 0;
	put_word(srca,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4668)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	NFLG = ((uae_s16)(dst)) < 0;
	put_word(srca,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_4670)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	NFLG = ((uae_s16)(dst)) < 0;
	put_word(srca,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4678)(uae_u32 opcode) /* NOT */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	NFLG = ((uae_s16)(dst)) < 0;
	put_word(srca,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_4679)(uae_u32 opcode) /* NOT */
{
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	NFLG = ((uae_s16)(dst)) < 0;
	put_word(srca,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_4680)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	NFLG = ((uae_s32)(dst)) < 0;
	m68k_dreg(regs, srcreg) = (dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_4690)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	NFLG = ((uae_s32)(dst)) < 0;
	put_long(srca,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_4698)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	NFLG = ((uae_s32)(dst)) < 0;
	put_long(srca,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_46a0)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	NFLG = ((uae_s32)(dst)) < 0;
	put_long(srca,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_46a8)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	NFLG = ((uae_s32)(dst)) < 0;
	put_long(srca,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_46b0)(uae_u32 opcode) /* NOT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	NFLG = ((uae_s32)(dst)) < 0;
	put_long(srca,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_46b8)(uae_u32 opcode) /* NOT */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	NFLG = ((uae_s32)(dst)) < 0;
	put_long(srca,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_46b9)(uae_u32 opcode) /* NOT */
{
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
{	uae_u32 dst = ~src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	NFLG = ((uae_s32)(dst)) < 0;
	put_long(srca,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_46c0)(uae_u32 opcode) /* MV2SR */
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uae_s16 src = m68k_dreg(regs, srcreg);
	regs.sr = src;
	MakeFromSR();
}}}}
void REGPARAM2 CPU_OP_NAME(_46d0)(uae_u32 opcode) /* MV2SR */
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
	regs.sr = src;
	MakeFromSR();
}}}}
void REGPARAM2 CPU_OP_NAME(_46d8)(uae_u32 opcode) /* MV2SR */
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
	regs.sr = src;
	MakeFromSR();
}}}}}
void REGPARAM2 CPU_OP_NAME(_46e0)(uae_u32 opcode) /* MV2SR */
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
	regs.sr = src;
	MakeFromSR();
}}}}}
void REGPARAM2 CPU_OP_NAME(_46e8)(uae_u32 opcode) /* MV2SR */
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
	regs.sr = src;
	MakeFromSR();
}}}}
void REGPARAM2 CPU_OP_NAME(_46f0)(uae_u32 opcode) /* MV2SR */
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
	regs.sr = src;
	MakeFromSR();
}}}}}
void REGPARAM2 CPU_OP_NAME(_46f8)(uae_u32 opcode) /* MV2SR */
{
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
	regs.sr = src;
	MakeFromSR();
}}}}
void REGPARAM2 CPU_OP_NAME(_46f9)(uae_u32 opcode) /* MV2SR */
{
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
	regs.sr = src;
	MakeFromSR();
}}}}
void REGPARAM2 CPU_OP_NAME(_46fa)(uae_u32 opcode) /* MV2SR */
{
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
	regs.sr = src;
	MakeFromSR();
}}}}}
void REGPARAM2 CPU_OP_NAME(_46fb)(uae_u32 opcode) /* MV2SR */
{
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
	regs.sr = src;
	MakeFromSR();
}}}}}
void REGPARAM2 CPU_OP_NAME(_46fc)(uae_u32 opcode) /* MV2SR */
{
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uae_s16 src = nextiword();
	regs.sr = src;
	MakeFromSR();
}}}}
void REGPARAM2 CPU_OP_NAME(_4800)(uae_u32 opcode) /* NBCD */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uae_u16 newv_lo = - (src & 0xF) - (XFLG ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = XFLG = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((newv) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_4808)(uae_u32 opcode) /* LINK */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, 7) -= 4;
{	uaecptr olda = m68k_areg(regs, 7);
{	uae_s32 src = m68k_areg(regs, srcreg);
	put_long(olda,src);
	m68k_areg(regs, srcreg) = (m68k_areg(regs, 7));
{	uae_s32 offs = nextilong();
	m68k_areg(regs, 7) += offs;
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4810)(uae_u32 opcode) /* NBCD */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uae_u16 newv_lo = - (src & 0xF) - (XFLG ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = XFLG = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	put_byte(srca,newv);
}}}}
void REGPARAM2 CPU_OP_NAME(_4818)(uae_u32 opcode) /* NBCD */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	uae_u16 newv_lo = - (src & 0xF) - (XFLG ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = XFLG = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	put_byte(srca,newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4820)(uae_u32 opcode) /* NBCD */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uae_u16 newv_lo = - (src & 0xF) - (XFLG ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = XFLG = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	put_byte(srca,newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4828)(uae_u32 opcode) /* NBCD */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uae_u16 newv_lo = - (src & 0xF) - (XFLG ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = XFLG = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	put_byte(srca,newv);
}}}}
void REGPARAM2 CPU_OP_NAME(_4830)(uae_u32 opcode) /* NBCD */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
{	uae_u16 newv_lo = - (src & 0xF) - (XFLG ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = XFLG = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	put_byte(srca,newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4838)(uae_u32 opcode) /* NBCD */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uae_u16 newv_lo = - (src & 0xF) - (XFLG ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = XFLG = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	put_byte(srca,newv);
}}}}
void REGPARAM2 CPU_OP_NAME(_4839)(uae_u32 opcode) /* NBCD */
{
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
{	uae_u16 newv_lo = - (src & 0xF) - (XFLG ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = XFLG = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
	put_byte(srca,newv);
}}}}
void REGPARAM2 CPU_OP_NAME(_4840)(uae_u32 opcode) /* SWAP */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_u32 dst = ((src >> 16)&0xFFFF) | ((src&0xFFFF)<<16);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	NFLG = ((uae_s32)(dst)) < 0;
	m68k_dreg(regs, srcreg) = (dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_4848)(uae_u32 opcode) /* BKPT */
{
	uae_u32 srcreg = (opcode & 7);
{	op_illg(opcode);
}}
void REGPARAM2 CPU_OP_NAME(_4850)(uae_u32 opcode) /* PEA */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
{	m68k_areg(regs, 7) -= 4;
{	uaecptr dsta = m68k_areg(regs, 7);
	put_long(dsta,srca);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4868)(uae_u32 opcode) /* PEA */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
{	m68k_areg(regs, 7) -= 4;
{	uaecptr dsta = m68k_areg(regs, 7);
	put_long(dsta,srca);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4870)(uae_u32 opcode) /* PEA */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	m68k_areg(regs, 7) -= 4;
{	uaecptr dsta = m68k_areg(regs, 7);
	put_long(dsta,srca);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4878)(uae_u32 opcode) /* PEA */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	m68k_areg(regs, 7) -= 4;
{	uaecptr dsta = m68k_areg(regs, 7);
	put_long(dsta,srca);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4879)(uae_u32 opcode) /* PEA */
{
{{	uaecptr srca = nextilong();
{	m68k_areg(regs, 7) -= 4;
{	uaecptr dsta = m68k_areg(regs, 7);
	put_long(dsta,srca);
}}}}}
void REGPARAM2 CPU_OP_NAME(_487a)(uae_u32 opcode) /* PEA */
{
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	m68k_areg(regs, 7) -= 4;
{	uaecptr dsta = m68k_areg(regs, 7);
	put_long(dsta,srca);
}}}}}
void REGPARAM2 CPU_OP_NAME(_487b)(uae_u32 opcode) /* PEA */
{
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	m68k_areg(regs, 7) -= 4;
{	uaecptr dsta = m68k_areg(regs, 7);
	put_long(dsta,srca);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4880)(uae_u32 opcode) /* EXT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_u16 dst = (uae_s16)(uae_s8)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(dst)) == 0;
	NFLG = ((uae_s16)(dst)) < 0;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((dst) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_4890)(uae_u32 opcode) /* MVMLE */
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = nextiword();
{	uaecptr srca = m68k_areg(regs, dstreg);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { put_word(srca, m68k_dreg(regs, movem_index1[dmask])); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { put_word(srca, m68k_areg(regs, movem_index1[amask])); srca += 2; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_48a0)(uae_u32 opcode) /* MVMLE */
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = nextiword();
{{	uaecptr srca = m68k_areg(regs, dstreg);
{	uae_u16 amask = mask & 0xff, dmask = (mask >> 8) & 0xff;
	while (amask) { srca -= 2; put_word(srca, m68k_areg(regs, movem_index2[amask])); amask = movem_next[amask]; }
	while (dmask) { srca -= 2; put_word(srca, m68k_dreg(regs, movem_index2[dmask])); dmask = movem_next[dmask]; }
	m68k_areg(regs, dstreg) = srca;
}}}}}
void REGPARAM2 CPU_OP_NAME(_48a8)(uae_u32 opcode) /* MVMLE */
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = nextiword();
{	uaecptr srca = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { put_word(srca, m68k_dreg(regs, movem_index1[dmask])); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { put_word(srca, m68k_areg(regs, movem_index1[amask])); srca += 2; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_48b0)(uae_u32 opcode) /* MVMLE */
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = nextiword();
{	uaecptr srca = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { put_word(srca, m68k_dreg(regs, movem_index1[dmask])); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { put_word(srca, m68k_areg(regs, movem_index1[amask])); srca += 2; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_48b8)(uae_u32 opcode) /* MVMLE */
{
{	uae_u16 mask = nextiword();
{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { put_word(srca, m68k_dreg(regs, movem_index1[dmask])); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { put_word(srca, m68k_areg(regs, movem_index1[amask])); srca += 2; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_48b9)(uae_u32 opcode) /* MVMLE */
{
{	uae_u16 mask = nextiword();
{	uaecptr srca = nextilong();
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { put_word(srca, m68k_dreg(regs, movem_index1[dmask])); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { put_word(srca, m68k_areg(regs, movem_index1[amask])); srca += 2; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_48c0)(uae_u32 opcode) /* EXT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_u32 dst = (uae_s32)(uae_s16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	NFLG = ((uae_s32)(dst)) < 0;
	m68k_dreg(regs, srcreg) = (dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_48d0)(uae_u32 opcode) /* MVMLE */
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = nextiword();
{	uaecptr srca = m68k_areg(regs, dstreg);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { put_long(srca, m68k_dreg(regs, movem_index1[dmask])); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { put_long(srca, m68k_areg(regs, movem_index1[amask])); srca += 4; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_48e0)(uae_u32 opcode) /* MVMLE */
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = nextiword();
{{	uaecptr srca = m68k_areg(regs, dstreg);
{	uae_u16 amask = mask & 0xff, dmask = (mask >> 8) & 0xff;
	while (amask) { srca -= 4; put_long(srca, m68k_areg(regs, movem_index2[amask])); amask = movem_next[amask]; }
	while (dmask) { srca -= 4; put_long(srca, m68k_dreg(regs, movem_index2[dmask])); dmask = movem_next[dmask]; }
	m68k_areg(regs, dstreg) = srca;
}}}}}
void REGPARAM2 CPU_OP_NAME(_48e8)(uae_u32 opcode) /* MVMLE */
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = nextiword();
{	uaecptr srca = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { put_long(srca, m68k_dreg(regs, movem_index1[dmask])); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { put_long(srca, m68k_areg(regs, movem_index1[amask])); srca += 4; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_48f0)(uae_u32 opcode) /* MVMLE */
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = nextiword();
{	uaecptr srca = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { put_long(srca, m68k_dreg(regs, movem_index1[dmask])); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { put_long(srca, m68k_areg(regs, movem_index1[amask])); srca += 4; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_48f8)(uae_u32 opcode) /* MVMLE */
{
{	uae_u16 mask = nextiword();
{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { put_long(srca, m68k_dreg(regs, movem_index1[dmask])); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { put_long(srca, m68k_areg(regs, movem_index1[amask])); srca += 4; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_48f9)(uae_u32 opcode) /* MVMLE */
{
{	uae_u16 mask = nextiword();
{	uaecptr srca = nextilong();
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { put_long(srca, m68k_dreg(regs, movem_index1[dmask])); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { put_long(srca, m68k_areg(regs, movem_index1[amask])); srca += 4; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_49c0)(uae_u32 opcode) /* EXT */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_u32 dst = (uae_s32)(uae_s8)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(dst)) == 0;
	NFLG = ((uae_s32)(dst)) < 0;
	m68k_dreg(regs, srcreg) = (dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_4a00)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4a10)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4a18)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
}}}}
void REGPARAM2 CPU_OP_NAME(_4a20)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
}}}}
void REGPARAM2 CPU_OP_NAME(_4a28)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4a30)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
}}}}
void REGPARAM2 CPU_OP_NAME(_4a38)(uae_u32 opcode) /* TST */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4a39)(uae_u32 opcode) /* TST */
{
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4a3a)(uae_u32 opcode) /* TST */
{
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s8 src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
}}}}
void REGPARAM2 CPU_OP_NAME(_4a3b)(uae_u32 opcode) /* TST */
{
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s8 src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
}}}}
void REGPARAM2 CPU_OP_NAME(_4a3c)(uae_u32 opcode) /* TST */
{
{{	uae_s8 src = nextibyte();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4a40)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4a48)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_areg(regs, srcreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4a50)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4a58)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
}}}}
void REGPARAM2 CPU_OP_NAME(_4a60)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
}}}}
void REGPARAM2 CPU_OP_NAME(_4a68)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4a70)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
}}}}
void REGPARAM2 CPU_OP_NAME(_4a78)(uae_u32 opcode) /* TST */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4a79)(uae_u32 opcode) /* TST */
{
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4a7a)(uae_u32 opcode) /* TST */
{
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
}}}}
void REGPARAM2 CPU_OP_NAME(_4a7b)(uae_u32 opcode) /* TST */
{
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
}}}}
void REGPARAM2 CPU_OP_NAME(_4a7c)(uae_u32 opcode) /* TST */
{
{{	uae_s16 src = nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4a80)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg(regs, srcreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4a88)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg(regs, srcreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4a90)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4a98)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
}}}}
void REGPARAM2 CPU_OP_NAME(_4aa0)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
}}}}
void REGPARAM2 CPU_OP_NAME(_4aa8)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4ab0)(uae_u32 opcode) /* TST */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
}}}}
void REGPARAM2 CPU_OP_NAME(_4ab8)(uae_u32 opcode) /* TST */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4ab9)(uae_u32 opcode) /* TST */
{
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4aba)(uae_u32 opcode) /* TST */
{
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s32 src = get_long(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
}}}}
void REGPARAM2 CPU_OP_NAME(_4abb)(uae_u32 opcode) /* TST */
{
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s32 src = get_long(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
}}}}
void REGPARAM2 CPU_OP_NAME(_4abc)(uae_u32 opcode) /* TST */
{
{{	uae_s32 src = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
}}}
void REGPARAM2 CPU_OP_NAME(_4ac0)(uae_u32 opcode) /* TAS */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	src |= 0x80;
	m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xff) | ((src) & 0xff);
}}}
void REGPARAM2 CPU_OP_NAME(_4ad0)(uae_u32 opcode) /* TAS */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	src |= 0x80;
	put_byte(srca,src);
}}}
void REGPARAM2 CPU_OP_NAME(_4ad8)(uae_u32 opcode) /* TAS */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	src |= 0x80;
	put_byte(srca,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_4ae0)(uae_u32 opcode) /* TAS */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	src |= 0x80;
	put_byte(srca,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_4ae8)(uae_u32 opcode) /* TAS */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	src |= 0x80;
	put_byte(srca,src);
}}}
void REGPARAM2 CPU_OP_NAME(_4af0)(uae_u32 opcode) /* TAS */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	src |= 0x80;
	put_byte(srca,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_4af8)(uae_u32 opcode) /* TAS */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	src |= 0x80;
	put_byte(srca,src);
}}}
void REGPARAM2 CPU_OP_NAME(_4af9)(uae_u32 opcode) /* TAS */
{
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	src |= 0x80;
	put_byte(srca,src);
}}}
void REGPARAM2 CPU_OP_NAME(_4c00)(uae_u32 opcode) /* MULL */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = nextiword();
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	m68k_mull(opcode, dst, extra);
}}}}
void REGPARAM2 CPU_OP_NAME(_4c10)(uae_u32 opcode) /* MULL */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
	m68k_mull(opcode, dst, extra);
}}}}
void REGPARAM2 CPU_OP_NAME(_4c18)(uae_u32 opcode) /* MULL */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{	m68k_areg(regs, dstreg) += 4;
	m68k_mull(opcode, dst, extra);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4c20)(uae_u32 opcode) /* MULL */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = nextiword();
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
	m68k_mull(opcode, dst, extra);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4c28)(uae_u32 opcode) /* MULL */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
	m68k_mull(opcode, dst, extra);
}}}}
void REGPARAM2 CPU_OP_NAME(_4c30)(uae_u32 opcode) /* MULL */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s32 dst = get_long(dsta);
	m68k_mull(opcode, dst, extra);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4c38)(uae_u32 opcode) /* MULL */
{
{{	uae_s16 extra = nextiword();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
	m68k_mull(opcode, dst, extra);
}}}}
void REGPARAM2 CPU_OP_NAME(_4c39)(uae_u32 opcode) /* MULL */
{
{{	uae_s16 extra = nextiword();
{	uaecptr dsta = nextilong();
	uae_s32 dst = get_long(dsta);
	m68k_mull(opcode, dst, extra);
}}}}
void REGPARAM2 CPU_OP_NAME(_4c3a)(uae_u32 opcode) /* MULL */
{
	uae_u32 dstreg = 2;
{{	uae_s16 extra = nextiword();
{	uaecptr dsta = m68k_getpc();
	dsta += (uae_s32)(uae_s16)nextiword();
{	uae_s32 dst = get_long(dsta);
	m68k_mull(opcode, dst, extra);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4c3b)(uae_u32 opcode) /* MULL */
{
	uae_u32 dstreg = 3;
{{	uae_s16 extra = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_getpc());
{	uae_s32 dst = get_long(dsta);
	m68k_mull(opcode, dst, extra);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4c3c)(uae_u32 opcode) /* MULL */
{
{{	uae_s16 extra = nextiword();
{	uae_s32 dst = nextilong();
	m68k_mull(opcode, dst, extra);
}}}}
void REGPARAM2 CPU_OP_NAME(_4c40)(uae_u32 opcode) /* DIVL */
{
	uae_u32 dstreg = opcode & 7;
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 extra = nextiword();
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	m68k_divl(opcode, dst, extra, oldpc);
}}}}
void REGPARAM2 CPU_OP_NAME(_4c50)(uae_u32 opcode) /* DIVL */
{
	uae_u32 dstreg = opcode & 7;
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 extra = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
	m68k_divl(opcode, dst, extra, oldpc);
}}}}
void REGPARAM2 CPU_OP_NAME(_4c58)(uae_u32 opcode) /* DIVL */
{
	uae_u32 dstreg = opcode & 7;
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 extra = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{	m68k_areg(regs, dstreg) += 4;
	m68k_divl(opcode, dst, extra, oldpc);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4c60)(uae_u32 opcode) /* DIVL */
{
	uae_u32 dstreg = opcode & 7;
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 extra = nextiword();
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
	m68k_divl(opcode, dst, extra, oldpc);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4c68)(uae_u32 opcode) /* DIVL */
{
	uae_u32 dstreg = opcode & 7;
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 extra = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
	m68k_divl(opcode, dst, extra, oldpc);
}}}}
void REGPARAM2 CPU_OP_NAME(_4c70)(uae_u32 opcode) /* DIVL */
{
	uae_u32 dstreg = opcode & 7;
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 extra = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s32 dst = get_long(dsta);
	m68k_divl(opcode, dst, extra, oldpc);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4c78)(uae_u32 opcode) /* DIVL */
{
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 extra = nextiword();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
	m68k_divl(opcode, dst, extra, oldpc);
}}}}
void REGPARAM2 CPU_OP_NAME(_4c79)(uae_u32 opcode) /* DIVL */
{
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 extra = nextiword();
{	uaecptr dsta = nextilong();
	uae_s32 dst = get_long(dsta);
	m68k_divl(opcode, dst, extra, oldpc);
}}}}
void REGPARAM2 CPU_OP_NAME(_4c7a)(uae_u32 opcode) /* DIVL */
{
	uae_u32 dstreg = 2;
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 extra = nextiword();
{	uaecptr dsta = m68k_getpc();
	dsta += (uae_s32)(uae_s16)nextiword();
{	uae_s32 dst = get_long(dsta);
	m68k_divl(opcode, dst, extra, oldpc);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4c7b)(uae_u32 opcode) /* DIVL */
{
	uae_u32 dstreg = 3;
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 extra = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_getpc());
{	uae_s32 dst = get_long(dsta);
	m68k_divl(opcode, dst, extra, oldpc);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4c7c)(uae_u32 opcode) /* DIVL */
{
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 extra = nextiword();
{	uae_s32 dst = nextilong();
	m68k_divl(opcode, dst, extra, oldpc);
}}}}
void REGPARAM2 CPU_OP_NAME(_4c90)(uae_u32 opcode) /* MVMEL */
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = nextiword(), dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca = m68k_areg(regs, dstreg);
{	while (dmask) { m68k_dreg(regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word(srca); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg(regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word(srca); srca += 2; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_4c98)(uae_u32 opcode) /* MVMEL */
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = nextiword(), dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca = m68k_areg(regs, dstreg);
{	while (dmask) { m68k_dreg(regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word(srca); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg(regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word(srca); srca += 2; amask = movem_next[amask]; }
	m68k_areg(regs, dstreg) = srca;
}}}}
void REGPARAM2 CPU_OP_NAME(_4ca8)(uae_u32 opcode) /* MVMEL */
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = nextiword(), dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
{	while (dmask) { m68k_dreg(regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word(srca); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg(regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word(srca); srca += 2; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_4cb0)(uae_u32 opcode) /* MVMEL */
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = nextiword(), dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca = get_disp_ea(m68k_areg(regs, dstreg));
{	while (dmask) { m68k_dreg(regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word(srca); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg(regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word(srca); srca += 2; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_4cb8)(uae_u32 opcode) /* MVMEL */
{
{	uae_u16 mask = nextiword(), dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	while (dmask) { m68k_dreg(regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word(srca); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg(regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word(srca); srca += 2; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_4cb9)(uae_u32 opcode) /* MVMEL */
{
{	uae_u16 mask = nextiword(), dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca = nextilong();
{	while (dmask) { m68k_dreg(regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word(srca); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg(regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word(srca); srca += 2; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_4cba)(uae_u32 opcode) /* MVMEL */
{
	uae_u32 dstreg = 2;
{	uae_u16 mask = nextiword(), dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	while (dmask) { m68k_dreg(regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word(srca); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg(regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word(srca); srca += 2; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_4cbb)(uae_u32 opcode) /* MVMEL */
{
	uae_u32 dstreg = 3;
{	uae_u16 mask = nextiword(), dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca = get_disp_ea(m68k_getpc());
{	while (dmask) { m68k_dreg(regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word(srca); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg(regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word(srca); srca += 2; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_4cd0)(uae_u32 opcode) /* MVMEL */
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = nextiword(), dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca = m68k_areg(regs, dstreg);
{	while (dmask) { m68k_dreg(regs, movem_index1[dmask]) = get_long(srca); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg(regs, movem_index1[amask]) = get_long(srca); srca += 4; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_4cd8)(uae_u32 opcode) /* MVMEL */
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = nextiword(), dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca = m68k_areg(regs, dstreg);
{	while (dmask) { m68k_dreg(regs, movem_index1[dmask]) = get_long(srca); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg(regs, movem_index1[amask]) = get_long(srca); srca += 4; amask = movem_next[amask]; }
	m68k_areg(regs, dstreg) = srca;
}}}}
void REGPARAM2 CPU_OP_NAME(_4ce8)(uae_u32 opcode) /* MVMEL */
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = nextiword(), dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
{	while (dmask) { m68k_dreg(regs, movem_index1[dmask]) = get_long(srca); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg(regs, movem_index1[amask]) = get_long(srca); srca += 4; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_4cf0)(uae_u32 opcode) /* MVMEL */
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = nextiword(), dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca = get_disp_ea(m68k_areg(regs, dstreg));
{	while (dmask) { m68k_dreg(regs, movem_index1[dmask]) = get_long(srca); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg(regs, movem_index1[amask]) = get_long(srca); srca += 4; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_4cf8)(uae_u32 opcode) /* MVMEL */
{
{	uae_u16 mask = nextiword(), dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
{	while (dmask) { m68k_dreg(regs, movem_index1[dmask]) = get_long(srca); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg(regs, movem_index1[amask]) = get_long(srca); srca += 4; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_4cf9)(uae_u32 opcode) /* MVMEL */
{
{	uae_u16 mask = nextiword(), dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca = nextilong();
{	while (dmask) { m68k_dreg(regs, movem_index1[dmask]) = get_long(srca); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg(regs, movem_index1[amask]) = get_long(srca); srca += 4; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_4cfa)(uae_u32 opcode) /* MVMEL */
{
	uae_u32 dstreg = 2;
{	uae_u16 mask = nextiword(), dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	while (dmask) { m68k_dreg(regs, movem_index1[dmask]) = get_long(srca); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg(regs, movem_index1[amask]) = get_long(srca); srca += 4; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_4cfb)(uae_u32 opcode) /* MVMEL */
{
	uae_u32 dstreg = 3;
{	uae_u16 mask = nextiword(), dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca = get_disp_ea(m68k_getpc());
{	while (dmask) { m68k_dreg(regs, movem_index1[dmask]) = get_long(srca); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg(regs, movem_index1[amask]) = get_long(srca); srca += 4; amask = movem_next[amask]; }
}}}}
void REGPARAM2 CPU_OP_NAME(_4e40)(uae_u32 opcode) /* TRAP */
{
	uae_u32 srcreg = (opcode & 15);
{{	uae_u32 src = srcreg;
	Exception(src+32,0);
}}}
void REGPARAM2 CPU_OP_NAME(_4e50)(uae_u32 opcode) /* LINK */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, 7) -= 4;
{	uaecptr olda = m68k_areg(regs, 7);
{	uae_s32 src = m68k_areg(regs, srcreg);
	put_long(olda,src);
	m68k_areg(regs, srcreg) = (m68k_areg(regs, 7));
{	uae_s16 offs = nextiword();
	m68k_areg(regs, 7) += offs;
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4e58)(uae_u32 opcode) /* UNLK */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg(regs, srcreg);
	m68k_areg(regs, 7) = src;
{	uaecptr olda = m68k_areg(regs, 7);
	uae_s32 old = get_long(olda);
{	m68k_areg(regs, 7) += 4;
	m68k_areg(regs, srcreg) = (old);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4e60)(uae_u32 opcode) /* MVR2USP */
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uae_s32 src = m68k_areg(regs, srcreg);
	regs.usp = src;
}}}}
void REGPARAM2 CPU_OP_NAME(_4e68)(uae_u32 opcode) /* MVUSP2R */
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	m68k_areg(regs, srcreg) = (regs.usp);
}}}}
void REGPARAM2 CPU_OP_NAME(_4e70)(uae_u32 opcode) /* RESET */
{
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{	customreset();
}}}
void REGPARAM2 CPU_OP_NAME(_4e71)(uae_u32 opcode) /* NOP */
{
{}}
void REGPARAM2 CPU_OP_NAME(_4e72)(uae_u32 opcode) /* STOP */
{
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uae_s16 src = nextiword();
	regs.sr = src;
	MakeFromSR();
	m68k_setstopped(1);
}}}}
void REGPARAM2 CPU_OP_NAME(_4e73)(uae_u32 opcode) /* RTE */
{
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{	uae_u16 newsr; uae_u32 newpc; for (;;) {
{	uaecptr sra = m68k_areg(regs, 7);
	uae_s16 sr = get_word(sra);
{	m68k_areg(regs, 7) += 2;
{	uaecptr pca = m68k_areg(regs, 7);
	uae_s32 pc = get_long(pca);
{	m68k_areg(regs, 7) += 4;
{	uaecptr formata = m68k_areg(regs, 7);
	uae_s16 format = get_word(formata);
{	m68k_areg(regs, 7) += 2;
	newsr = sr; newpc = pc;
	if ((format & 0xF000) == 0x0000) { break; }
	else if ((format & 0xF000) == 0x1000) { ; }
	else if ((format & 0xF000) == 0x2000) { m68k_areg(regs, 7) += 4; break; }
	else if ((format & 0xF000) == 0x8000) { m68k_areg(regs, 7) += 50; break; }
	else if ((format & 0xF000) == 0x9000) { m68k_areg(regs, 7) += 12; break; }
	else if ((format & 0xF000) == 0xa000) { m68k_areg(regs, 7) += 24; break; }
	else if ((format & 0xF000) == 0xb000) { m68k_areg(regs, 7) += 84; break; }
	else { Exception(14,0); return; }
	regs.sr = newsr; MakeFromSR();
}
}}}}}}	regs.sr = newsr; MakeFromSR();
	m68k_setpc_rte(newpc);
}}}
void REGPARAM2 CPU_OP_NAME(_4e74)(uae_u32 opcode) /* RTD */
{
{	compiler_flush_jsr_stack();
{	uaecptr pca = m68k_areg(regs, 7);
	uae_s32 pc = get_long(pca);
{	m68k_areg(regs, 7) += 4;
{	uae_s16 offs = nextiword();
	m68k_areg(regs, 7) += offs;
	m68k_setpc_rte(pc);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4e75)(uae_u32 opcode) /* RTS */
{
{	m68k_do_rts();
}}
void REGPARAM2 CPU_OP_NAME(_4e76)(uae_u32 opcode) /* TRAPV */
{
{	if(VFLG) Exception(7,m68k_getpc()-2);
}}
void REGPARAM2 CPU_OP_NAME(_4e77)(uae_u32 opcode) /* RTR */
{
{	compiler_flush_jsr_stack();
	MakeSR();
{	uaecptr sra = m68k_areg(regs, 7);
	uae_s16 sr = get_word(sra);
{	m68k_areg(regs, 7) += 2;
{	uaecptr pca = m68k_areg(regs, 7);
	uae_s32 pc = get_long(pca);
{	m68k_areg(regs, 7) += 4;
	regs.sr &= 0xFF00; sr &= 0xFF;
	regs.sr |= sr; m68k_setpc(pc);
	MakeFromSR();
}}}}}}
void REGPARAM2 CPU_OP_NAME(_4e7a)(uae_u32 opcode) /* MOVEC2 */
{
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uae_s16 src = nextiword();
{	int regno = (src >> 12) & 15;
	uae_u32 *regp = regs.regs + regno;
	m68k_movec2(src & 0xFFF, regp);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4e7b)(uae_u32 opcode) /* MOVE2C */
{
{if (!regs.s) { regs.pc_p -= 2; Exception(8,0); } else
{{	uae_s16 src = nextiword();
{	int regno = (src >> 12) & 15;
	uae_u32 *regp = regs.regs + regno;
	m68k_move2c(src & 0xFFF, regp);
}}}}}
void REGPARAM2 CPU_OP_NAME(_4e90)(uae_u32 opcode) /* JSR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	m68k_do_jsr(srca);
}}}
void REGPARAM2 CPU_OP_NAME(_4ea8)(uae_u32 opcode) /* JSR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	m68k_do_jsr(srca);
}}}
void REGPARAM2 CPU_OP_NAME(_4eb0)(uae_u32 opcode) /* JSR */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
	m68k_do_jsr(srca);
}}}
void REGPARAM2 CPU_OP_NAME(_4eb8)(uae_u32 opcode) /* JSR */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	m68k_do_jsr(srca);
}}}
void REGPARAM2 CPU_OP_NAME(_4eb9)(uae_u32 opcode) /* JSR */
{
{{	uaecptr srca = nextilong();
	m68k_do_jsr(srca);
}}}
void REGPARAM2 CPU_OP_NAME(_4eba)(uae_u32 opcode) /* JSR */
{
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
	m68k_do_jsr(srca);
}}}
void REGPARAM2 CPU_OP_NAME(_4ebb)(uae_u32 opcode) /* JSR */
{
{{	uaecptr srca = get_disp_ea(m68k_getpc());
	m68k_do_jsr(srca);
}}}
void REGPARAM2 CPU_OP_NAME(_4ed0)(uae_u32 opcode) /* JMP */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	m68k_setpc(srca);
}}}
void REGPARAM2 CPU_OP_NAME(_4ee8)(uae_u32 opcode) /* JMP */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	m68k_setpc(srca);
}}}
void REGPARAM2 CPU_OP_NAME(_4ef0)(uae_u32 opcode) /* JMP */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
	m68k_setpc(srca);
}}}
void REGPARAM2 CPU_OP_NAME(_4ef8)(uae_u32 opcode) /* JMP */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	m68k_setpc(srca);
}}}
void REGPARAM2 CPU_OP_NAME(_4ef9)(uae_u32 opcode) /* JMP */
{
{{	uaecptr srca = nextilong();
	m68k_setpc(srca);
}}}
void REGPARAM2 CPU_OP_NAME(_4efa)(uae_u32 opcode) /* JMP */
{
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
	m68k_setpc(srca);
}}}
void REGPARAM2 CPU_OP_NAME(_4efb)(uae_u32 opcode) /* JMP */
{
{{	uaecptr srca = get_disp_ea(m68k_getpc());
	m68k_setpc(srca);
}}}
#endif
