#include "shared.h"

#include "machdep/m68k.h"
#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "compiler.h"
#include "cputbl.h"
#if !defined (MEMFUNCS_DIRECT_REQUESTED) || defined (DIRECT_MEMFUNCS_SUCCESSFUL)
void REGPARAM2 CPU_OP_NAME(_c000)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_c010)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_c018)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c020)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c028)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_c030)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c038)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_c039)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_c03a)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s8 src = get_byte(srca);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c03b)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s8 src = get_byte(srca);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c03c)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = nextibyte();
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_c040)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_c050)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_c058)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c060)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c068)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_c070)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c078)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_c079)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_c07a)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c07b)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c07c)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = nextiword();
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_c080)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c090)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c098)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c0a0)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c0a8)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c0b0)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c0b8)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c0b9)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c0ba)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c0bb)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c0bc)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = nextilong();
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c0c0)(uae_u32 opcode) /* MULU */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c0d0)(uae_u32 opcode) /* MULU */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c0d8)(uae_u32 opcode) /* MULU */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_c0e0)(uae_u32 opcode) /* MULU */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_c0e8)(uae_u32 opcode) /* MULU */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c0f0)(uae_u32 opcode) /* MULU */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_c0f8)(uae_u32 opcode) /* MULU */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c0f9)(uae_u32 opcode) /* MULU */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c0fa)(uae_u32 opcode) /* MULU */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_c0fb)(uae_u32 opcode) /* MULU */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_c0fc)(uae_u32 opcode) /* MULU */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = nextiword();
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c100)(uae_u32 opcode) /* ABCD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
{	uae_u16 newv_lo = (src & 0xF) + (dst & 0xF) + (XFLG ? 1 : 0);
	uae_u16 newv_hi = (src & 0xF0) + (dst & 0xF0);
	uae_u16 newv;
	if (newv_lo > 9) { newv_lo +=6; }
	newv = newv_hi + newv_lo;	CFLG = XFLG = (newv & 0x1F0) > 0x90;
	if (CFLG) newv += 0x60;
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_c108)(uae_u32 opcode) /* ABCD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	uae_u16 newv_lo = (src & 0xF) + (dst & 0xF) + (XFLG ? 1 : 0);
	uae_u16 newv_hi = (src & 0xF0) + (dst & 0xF0);
	uae_u16 newv;
	if (newv_lo > 9) { newv_lo +=6; }
	newv = newv_hi + newv_lo;	CFLG = XFLG = (newv & 0x1F0) > 0x90;
	if (CFLG) newv += 0x60;
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	put_byte(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c110)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c118)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c120)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c128)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c130)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s8 dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c138)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c139)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = nextilong();
	uae_s8 dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c140)(uae_u32 opcode) /* EXG */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	m68k_dreg(regs, srcreg) = (dst);
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c148)(uae_u32 opcode) /* EXG */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_areg(regs, srcreg);
{	uae_s32 dst = m68k_areg(regs, dstreg);
	m68k_areg(regs, srcreg) = (dst);
	m68k_areg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c150)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c158)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
{	m68k_areg(regs, dstreg) += 2;
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c160)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c168)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c170)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s16 dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c178)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s16 dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c179)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = nextilong();
	uae_s16 dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c188)(uae_u32 opcode) /* EXG */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_s32 dst = m68k_areg(regs, dstreg);
	m68k_dreg(regs, srcreg) = (dst);
	m68k_areg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c190)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c198)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{	m68k_areg(regs, dstreg) += 4;
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c1a0)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c1a8)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c1b0)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s32 dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c1b8)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c1b9)(uae_u32 opcode) /* AND */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = nextilong();
	uae_s32 dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c1c0)(uae_u32 opcode) /* MULS */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c1d0)(uae_u32 opcode) /* MULS */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c1d8)(uae_u32 opcode) /* MULS */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_c1e0)(uae_u32 opcode) /* MULS */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_c1e8)(uae_u32 opcode) /* MULS */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c1f0)(uae_u32 opcode) /* MULS */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_c1f8)(uae_u32 opcode) /* MULS */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c1f9)(uae_u32 opcode) /* MULS */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}
void REGPARAM2 CPU_OP_NAME(_c1fa)(uae_u32 opcode) /* MULS */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_c1fb)(uae_u32 opcode) /* MULS */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_c1fc)(uae_u32 opcode) /* MULS */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = nextiword();
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	NFLG = ((uae_s32)(newv)) < 0;
	m68k_dreg(regs, dstreg) = (newv);
}}}}}
#endif
