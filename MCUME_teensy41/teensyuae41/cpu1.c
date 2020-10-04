#include "shared.h"

#include "machdep/m68k.h"
#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "compiler.h"
#include "cputbl.h"
#if !defined (MEMFUNCS_DIRECT_REQUESTED) || defined (DIRECT_MEMFUNCS_SUCCESSFUL)
void REGPARAM2 CPU_OP_NAME(_1000)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_1010)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_1018)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1020)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1028)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_1030)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1038)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_1039)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_103a)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s8 src = get_byte(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_103b)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s8 src = get_byte(srca);
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}
void REGPARAM2 CPU_OP_NAME(_103c)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = nextibyte();
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_1080)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_1090)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_1098)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_10a0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_10a8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_10b0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_10b8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_10b9)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_10ba)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_10bb)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_10bc)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_10c0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_10d0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_10d8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_10e0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_10e8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_10f0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_10f8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_10f9)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_10fa)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_10fb)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_10fc)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = m68k_areg(regs, dstreg);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1100)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1110)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1118)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_1120)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_1128)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1130)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_1138)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1139)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_113a)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_113b)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}}
void REGPARAM2 CPU_OP_NAME(_113c)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = nextibyte();
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1140)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_1150)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_1158)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1160)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1168)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_1170)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1178)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_1179)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_117a)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_117b)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_117c)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_1180)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_1190)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_1198)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_11a0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_11a8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_11b0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_11b8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_11b9)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_11ba)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_11bb)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_11bc)(uae_u32 opcode) /* MOVE */
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_11c0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_11d0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_11d8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_11e0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_11e8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_11f0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_11f8)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_11f9)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_11fa)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_11fb)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_11fc)(uae_u32 opcode) /* MOVE */
{
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_13c0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_13d0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_13d8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	m68k_areg(regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_13e0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	m68k_areg(regs, srcreg) -= areg_byteinc[srcreg];
{	uaecptr srca = m68k_areg(regs, srcreg);
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_13e8)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = m68k_areg(regs, srcreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_13f0)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca = get_disp_ea(m68k_areg(regs, srcreg));
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_13f8)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = (uae_s32)(uae_s16)nextiword();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_13f9)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = nextilong();
	uae_s8 src = get_byte(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_13fa)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = m68k_getpc();
	srca += (uae_s32)(uae_s16)nextiword();
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_13fb)(uae_u32 opcode) /* MOVE */
{
{{	uaecptr srca = get_disp_ea(m68k_getpc());
{	uae_s8 src = get_byte(srca);
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_13fc)(uae_u32 opcode) /* MOVE */
{
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = nextilong();
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
#endif
