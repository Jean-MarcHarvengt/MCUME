#include "shared.h"

#include "machdep/m68k.h"
#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "compiler.h"
#include "cputbl.h"
#if !defined (MEMFUNCS_DIRECT_REQUESTED) || defined (DIRECT_MEMFUNCS_SUCCESSFUL)
void REGPARAM2 CPU_OP_NAME(_2000)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2008)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg(regs, srcreg);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2010)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2018)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2020)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2028)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2030)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2038)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2039)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_203a)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s32 src = get_long(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_203b)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s32 src = get_long(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_203c)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = nextilong();
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2040)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_u32 val = src;
	m68k_areg(regs, dstreg) = (val);
}}}}
void REGPARAM2 CPU_OP_NAME(_2048)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg(regs, srcreg);
{	uae_u32 val = src;
	m68k_areg(regs, dstreg) = (val);
}}}}
void REGPARAM2 CPU_OP_NAME(_2050)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uae_u32 val = src;
	m68k_areg(regs, dstreg) = (val);
}}}}
void REGPARAM2 CPU_OP_NAME(_2058)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
{	uae_u32 val = src;
	m68k_areg(regs, dstreg) = (val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2060)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uae_u32 val = src;
	m68k_areg(regs, dstreg) = (val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2068)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uae_u32 val = src;
	m68k_areg(regs, dstreg) = (val);
}}}}
void REGPARAM2 CPU_OP_NAME(_2070)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
{	uae_u32 val = src;
	m68k_areg(regs, dstreg) = (val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2078)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uae_u32 val = src;
	m68k_areg(regs, dstreg) = (val);
}}}}
void REGPARAM2 CPU_OP_NAME(_2079)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
{	uae_u32 val = src;
	m68k_areg(regs, dstreg) = (val);
}}}}
void REGPARAM2 CPU_OP_NAME(_207a)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s32 src = get_long(srca);
{	uae_u32 val = src;
	m68k_areg(regs, dstreg) = (val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_207b)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s32 src = get_long(srca);
{	uae_u32 val = src;
	m68k_areg(regs, dstreg) = (val);
}}}}}
void REGPARAM2 CPU_OP_NAME(_207c)(uae_u32 opcode) /* MOVEA */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = nextilong();
{	uae_u32 val = src;
	m68k_areg(regs, dstreg) = (val);
}}}}
void REGPARAM2 CPU_OP_NAME(_2080)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2088)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2090)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2098)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_20a0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_20a8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_20b0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_20b8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_20b9)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_20ba)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_20bb)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_20bc)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_20c0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 4;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_20c8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 4;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_20d0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 4;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_20d8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 4;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_20e0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 4;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_20e8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 4;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_20f0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 4;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_20f8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 4;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_20f9)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 4;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_20fa)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 4;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_20fb)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 4;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_20fc)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += 4;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2100)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2108)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg(regs, srcreg);
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2110)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2118)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_2120)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_2128)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2130)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_2138)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2139)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_213a)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s32 src = get_long(srca);
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_213b)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s32 src = get_long(srca);
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_213c)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = nextilong();
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2140)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2148)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2150)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2158)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2160)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2168)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2170)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2178)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2179)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_217a)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_217b)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_217c)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2180)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2188)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2190)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2198)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_21a0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_21a8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_21b0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_21b8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_21b9)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_21ba)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_21bb)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_21bc)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_21c0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_21c8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg(regs, srcreg);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_21d0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_21d8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_21e0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_21e8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_21f0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_21f8)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_21f9)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_21fa)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_21fb)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_21fc)(uae_u32 opcode) /* MOVE */
{
{{	uae_s32 src = nextilong();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_23c0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_23c8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg(regs, srcreg);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_23d0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_23d8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	m68k_areg(regs, srcreg) += 4;
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_23e0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= 4;
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s32 src = get_long(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_23e8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_23f0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_23f8)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_23f9)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = nextilong();
	uae_s32 src = get_long(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_23fa)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_23fb)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s32 src = get_long(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_23fc)(uae_u32 opcode) /* MOVE */
{
{{	uae_s32 src = nextilong();
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
#endif
