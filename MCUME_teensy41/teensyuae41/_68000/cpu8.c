#include "shared.h"

#include "machdep/m68k.h"
#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "compiler.h"
#include "cputbl.h"
#if !defined (MEMFUNCS_DIRECT_REQUESTED) || defined (DIRECT_MEMFUNCS_SUCCESSFUL)
void REGPARAM2 CPU_OP_NAME(_8000)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_8010)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_8018)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8020)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8028)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_8030)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8038)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_8039)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_803a)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s8 src = get_byte(srca);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_803b)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s8 src = get_byte(srca);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_803c)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = nextibyte();
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_8040)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_8050)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_8058)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8060)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8068)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_8070)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8078)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_8079)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_807a)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_807b)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_807c)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = nextiword();
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_8080)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_8090)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_8098)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_80a0)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_80a8)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_80b0)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_80b8)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_80b9)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_80ba)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_80bb)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s32 src = get_long(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_80bc)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = nextilong();
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_80c0)(uae_u32 opcode) /* DIVU */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
	uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	if (newv > 0xffff) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_80d0)(uae_u32 opcode) /* DIVU */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
	uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	if (newv > 0xffff) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_80d8)(uae_u32 opcode) /* DIVU */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
	uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	if (newv > 0xffff) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}}
void REGPARAM2 CPU_OP_NAME(_80e0)(uae_u32 opcode) /* DIVU */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
	uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	if (newv > 0xffff) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}}
void REGPARAM2 CPU_OP_NAME(_80e8)(uae_u32 opcode) /* DIVU */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
	uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	if (newv > 0xffff) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_80f0)(uae_u32 opcode) /* DIVU */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
	uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	if (newv > 0xffff) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}}
void REGPARAM2 CPU_OP_NAME(_80f8)(uae_u32 opcode) /* DIVU */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
	uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	if (newv > 0xffff) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_80f9)(uae_u32 opcode) /* DIVU */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
	uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	if (newv > 0xffff) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_80fa)(uae_u32 opcode) /* DIVU */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
	uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	if (newv > 0xffff) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}}
void REGPARAM2 CPU_OP_NAME(_80fb)(uae_u32 opcode) /* DIVU */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
	uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	if (newv > 0xffff) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}}
void REGPARAM2 CPU_OP_NAME(_80fc)(uae_u32 opcode) /* DIVU */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 src = nextiword();
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
	uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	if (newv > 0xffff) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_8100)(uae_u32 opcode) /* SBCD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uae_s8 dst = m68k_dreg(regs, dstreg);
{	uae_u16 newv_lo = (dst & 0xF) - (src & 0xF) - (XFLG ? 1 : 0);
	uae_u16 newv_hi = (dst & 0xF0) - (src & 0xF0);
	uae_u16 newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = XFLG = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_8108)(uae_u32 opcode) /* SBCD */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	uae_u16 newv_lo = (dst & 0xF) - (src & 0xF) - (XFLG ? 1 : 0);
	uae_u16 newv_hi = (dst & 0xF0) - (src & 0xF0);
	uae_u16 newv;
	if (newv_lo > 9) { newv_lo-=6; newv_hi-=0x10; }
	newv = newv_hi + (newv_lo & 0xF);	CFLG = XFLG = (newv_hi & 0x1F0) > 0x90;
	if (CFLG) newv -= 0x60;
	if (((uae_s8)(newv)) != 0) ZFLG = 0;
	NFLG = ((uae_s8)(newv)) < 0;
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	put_byte(dsta,newv);
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_8110)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_8118)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8120)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8128)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_8130)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s8 dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8138)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_8139)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = nextilong();
	uae_s8 dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_8150)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_8158)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
{	m68k_areg(regs, dstreg) += 2;
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8160)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8168)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_8170)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s16 dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8178)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s16 dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_8179)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = nextilong();
	uae_s16 dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_8190)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_8198)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{	m68k_areg(regs, dstreg) += 4;
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_81a0)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_81a8)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_81b0)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s32 dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_81b8)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_81b9)(uae_u32 opcode) /* OR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = nextilong();
	uae_s32 dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_81c0)(uae_u32 opcode) /* DIVS */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
	uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
	if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_81d0)(uae_u32 opcode) /* DIVS */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
	uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
	if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_81d8)(uae_u32 opcode) /* DIVS */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
	uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
	if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}}
void REGPARAM2 CPU_OP_NAME(_81e0)(uae_u32 opcode) /* DIVS */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
	uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
	if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}}
void REGPARAM2 CPU_OP_NAME(_81e8)(uae_u32 opcode) /* DIVS */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
	uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
	if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_81f0)(uae_u32 opcode) /* DIVS */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
	uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
	if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}}
void REGPARAM2 CPU_OP_NAME(_81f8)(uae_u32 opcode) /* DIVS */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
	uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
	if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_81f9)(uae_u32 opcode) /* DIVS */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
	uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
	if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}
void REGPARAM2 CPU_OP_NAME(_81fa)(uae_u32 opcode) /* DIVS */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
	uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
	if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}}
void REGPARAM2 CPU_OP_NAME(_81fb)(uae_u32 opcode) /* DIVS */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
	uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
	if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}}
void REGPARAM2 CPU_OP_NAME(_81fc)(uae_u32 opcode) /* DIVS */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr oldpc = m68k_getpc();
{	uae_s16 src = nextiword();
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	if(src == 0) Exception(5,oldpc-2); else {
	uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
	uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
	if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) { VFLG = NFLG = 1; CFLG = 0; } else
	{
	if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	NFLG = ((uae_s16)(newv)) < 0;
	newv = (newv & 0xffff) | ((uae_u32)rem << 16);
	m68k_dreg(regs, dstreg) = (newv);
	}
	}
}}}}
#endif
