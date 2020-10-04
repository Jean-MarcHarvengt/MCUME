#include "shared.h"

#include "machdep/m68k.h"
#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "compiler.h"
#include "cputbl.h"
#if !defined (MEMFUNCS_DIRECT_REQUESTED) || defined (DIRECT_MEMFUNCS_SUCCESSFUL)
void REGPARAM2 CPU_OP_NAME(_3000)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_3008)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg(regs, srcreg);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_3010)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_3018)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_3020)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_3028)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_3030)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_3038)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_3039)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_303a)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_303b)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_303c)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = nextiword();
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_3040)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uae_u32 val = (uae_s32)(uae_s16)src;
	m68k_areg(regs, dstreg) = (val);
}}}}
void REGPARAM2 CPU_OP_NAME(_3048)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg(regs, srcreg);
{	uae_u32 val = (uae_s32)(uae_s16)src;
	m68k_areg(regs, dstreg) = (val);
}}}}
void REGPARAM2 CPU_OP_NAME(_3050)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_u32 val = (uae_s32)(uae_s16)src;
	m68k_areg(regs, dstreg) = (val);
}}}}
void REGPARAM2 CPU_OP_NAME(_3058)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uae_u32 val = (uae_s32)(uae_s16)src;
	m68k_areg(regs, dstreg) = (val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_3060)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uae_u32 val = (uae_s32)(uae_s16)src;
	m68k_areg(regs, dstreg) = (val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_3068)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_u32 val = (uae_s32)(uae_s16)src;
	m68k_areg(regs, dstreg) = (val);
}}}}
void REGPARAM2 CPU_OP_NAME(_3070)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uae_u32 val = (uae_s32)(uae_s16)src;
	m68k_areg(regs, dstreg) = (val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_3078)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uae_u32 val = (uae_s32)(uae_s16)src;
	m68k_areg(regs, dstreg) = (val);
}}}}
void REGPARAM2 CPU_OP_NAME(_3079)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uae_u32 val = (uae_s32)(uae_s16)src;
	m68k_areg(regs, dstreg) = (val);
}}}}
void REGPARAM2 CPU_OP_NAME(_307a)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	uae_u32 val = (uae_s32)(uae_s16)src;
	m68k_areg(regs, dstreg) = (val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_307b)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	uae_u32 val = (uae_s32)(uae_s16)src;
	m68k_areg(regs, dstreg) = (val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_307c)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = nextiword();
{	uae_u32 val = (uae_s32)(uae_s16)src;
	m68k_areg(regs, dstreg) = (val);
}}}}
void REGPARAM2 CPU_OP_NAME(_3080)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_3088)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_3090)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_3098)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_30a0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_30a8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_30b0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_30b8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_30b9)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_30ba)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_30bb)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_30bc)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_30c0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 2;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_30c8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 2;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_30d0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 2;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_30d8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 2;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_30e0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 2;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_30e8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 2;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_30f0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 2;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_30f8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 2;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_30f9)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 2;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_30fa)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 2;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_30fb)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 2;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_30fc)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 2;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_3100)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_3108)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg(regs, srcreg);
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_3110)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_3118)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_3120)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_3128)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_3130)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_3138)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_3139)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_313a)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_313b)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_313c)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = nextiword();
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_3140)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_3148)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_3150)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_3158)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_3160)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_3168)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_3170)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_3178)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_3179)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_317a)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_317b)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_317c)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_3180)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_3188)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_3190)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_3198)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_31a0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_31a8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_31b0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_31b8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_31b9)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_31ba)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_31bb)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_31bc)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_31c0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_31c8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_areg(regs, srcreg);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_31d0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_31d8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_31e0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_31e8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_31f0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_31f8)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_31f9)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_31fa)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_31fb)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_31fc)(uae_u32 opcode) /* MOVE */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_33c0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_33c8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_areg(regs, srcreg);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_33d0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_33d8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	m68k_areg(regs, srcreg) += 2;
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_33e0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= 2;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s16 src = get_word(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_33e8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_33f0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_33f8)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_33f9)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = nextilong();
	uae_s16 src = get_word(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_33fa)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_33fb)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s16 src = get_word(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_33fc)(uae_u32 opcode) /* MOVE */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
#endif
