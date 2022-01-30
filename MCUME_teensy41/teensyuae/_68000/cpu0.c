#include "shared.h"

#include "machdep/m68k.h"
#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "compiler.h"
#include "cputbl.h"
#if !defined (MEMFUNCS_DIRECT_REQUESTED) || defined (DIRECT_MEMFUNCS_SUCCESSFUL)
void REGPARAM2 CPU_OP_NAME(_0)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_10)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_18)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_20)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_28)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_30)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s8 dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_38)(uae_u32 opcode) /* OR */
{
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_39)(uae_u32 opcode) /* OR */
{
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = nextilong();
	uae_s8 dst = get_byte(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_3c)(uae_u32 opcode) /* ORSR */
{
{	MakeSR();
{	uae_s16 src = nextiword();
	src &= 0xFF;
	regs.sr |= src;
	MakeFromSR();
}}}
void REGPARAM2 CPU_OP_NAME(_40)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_50)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_58)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
{	m68k_areg(regs, dstreg) += 2;
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_60)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_68)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_70)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s16 dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_78)(uae_u32 opcode) /* OR */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s16 dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_79)(uae_u32 opcode) /* OR */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = nextilong();
	uae_s16 dst = get_word(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_7c)(uae_u32 opcode) /* ORSR */
{
{if (!regs.s) { 
#ifdef USE_POINTER
    regs.pc_p -= 2;
#else
    regs.pc -= 2;
#endif
	Exception(8,0); } else
{	MakeSR();
{	uae_s16 src = nextiword();
	regs.sr |= src;
	MakeFromSR();
}}}}
void REGPARAM2 CPU_OP_NAME(_80)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_90)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_98)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{	m68k_areg(regs, dstreg) += 4;
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_a0)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_a8)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_b0)(uae_u32 opcode) /* OR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s32 dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_b8)(uae_u32 opcode) /* OR */
{
{{	uae_s32 src = nextilong();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_b9)(uae_u32 opcode) /* OR */
{
{{	uae_s32 src = nextilong();
{	uaecptr dsta = nextilong();
	uae_s32 dst = get_long(dsta);
	src |= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_100)(uae_u32 opcode) /* BTST */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= 31;
	ZFLG = !(dst & (1 << src));
}}}}
void REGPARAM2 CPU_OP_NAME(_108)(uae_u32 opcode) /* MVPMR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr memp = m68k_areg(regs, srcreg) + nextiword();
{	uae_u16 val = (get_byte(memp) << 8) + get_byte(memp + 2);
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}
void REGPARAM2 CPU_OP_NAME(_110)(uae_u32 opcode) /* BTST */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void REGPARAM2 CPU_OP_NAME(_118)(uae_u32 opcode) /* BTST */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void REGPARAM2 CPU_OP_NAME(_120)(uae_u32 opcode) /* BTST */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void REGPARAM2 CPU_OP_NAME(_128)(uae_u32 opcode) /* BTST */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void REGPARAM2 CPU_OP_NAME(_130)(uae_u32 opcode) /* BTST */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void REGPARAM2 CPU_OP_NAME(_138)(uae_u32 opcode) /* BTST */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void REGPARAM2 CPU_OP_NAME(_139)(uae_u32 opcode) /* BTST */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = nextilong();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void REGPARAM2 CPU_OP_NAME(_13a)(uae_u32 opcode) /* BTST */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = 2;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_getpc();
	dsta += (uae_s32)(uae_s16)nextiword();
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void REGPARAM2 CPU_OP_NAME(_13b)(uae_u32 opcode) /* BTST */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = 3;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_getpc());
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void REGPARAM2 CPU_OP_NAME(_13c)(uae_u32 opcode) /* BTST */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uae_s8 dst = nextibyte();
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void REGPARAM2 CPU_OP_NAME(_140)(uae_u32 opcode) /* BCHG */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= 31;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	m68k_dreg(regs, dstreg) = (dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_148)(uae_u32 opcode) /* MVPMR */
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr memp = m68k_areg(regs, srcreg) + nextiword();
{	uae_u32 val = (get_byte(memp) << 24) + (get_byte(memp + 2) << 16)
              + (get_byte(memp + 4) << 8) + get_byte(memp + 6);
	m68k_dreg(regs, dstreg) = (val);
}}}
void REGPARAM2 CPU_OP_NAME(_150)(uae_u32 opcode) /* BCHG */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_158)(uae_u32 opcode) /* BCHG */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_160)(uae_u32 opcode) /* BCHG */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_168)(uae_u32 opcode) /* BCHG */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_170)(uae_u32 opcode) /* BCHG */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_178)(uae_u32 opcode) /* BCHG */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_179)(uae_u32 opcode) /* BCHG */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = nextilong();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_17a)(uae_u32 opcode) /* BCHG */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = 2;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_getpc();
	dsta += (uae_s32)(uae_s16)nextiword();
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_17b)(uae_u32 opcode) /* BCHG */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = 3;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_getpc());
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_180)(uae_u32 opcode) /* BCLR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= 31;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	m68k_dreg(regs, dstreg) = (dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_188)(uae_u32 opcode) /* MVPRM */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg(regs, srcreg);
	uaecptr memp = m68k_areg(regs, dstreg) + nextiword();
	put_byte(memp, src >> 8); put_byte(memp + 2, src);
}}}
void REGPARAM2 CPU_OP_NAME(_190)(uae_u32 opcode) /* BCLR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_198)(uae_u32 opcode) /* BCLR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1a0)(uae_u32 opcode) /* BCLR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1a8)(uae_u32 opcode) /* BCLR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_1b0)(uae_u32 opcode) /* BCLR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1b8)(uae_u32 opcode) /* BCLR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_1b9)(uae_u32 opcode) /* BCLR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = nextilong();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_1ba)(uae_u32 opcode) /* BCLR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = 2;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_getpc();
	dsta += (uae_s32)(uae_s16)nextiword();
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1bb)(uae_u32 opcode) /* BCLR */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = 3;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_getpc());
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1c0)(uae_u32 opcode) /* BSET */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= 31;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	m68k_dreg(regs, dstreg) = (dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_1c8)(uae_u32 opcode) /* MVPRM */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg(regs, srcreg);
	uaecptr memp = m68k_areg(regs, dstreg) + nextiword();
	put_byte(memp, src >> 24); put_byte(memp + 2, src >> 16);
	put_byte(memp + 4, src >> 8); put_byte(memp + 6, src);
}}}
void REGPARAM2 CPU_OP_NAME(_1d0)(uae_u32 opcode) /* BSET */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_1d8)(uae_u32 opcode) /* BSET */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1e0)(uae_u32 opcode) /* BSET */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1e8)(uae_u32 opcode) /* BSET */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_1f0)(uae_u32 opcode) /* BSET */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1f8)(uae_u32 opcode) /* BSET */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_1f9)(uae_u32 opcode) /* BSET */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = nextilong();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_1fa)(uae_u32 opcode) /* BSET */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = 2;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = m68k_getpc();
	dsta += (uae_s32)(uae_s16)nextiword();
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_1fb)(uae_u32 opcode) /* BSET */
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = 3;
{{	uae_s8 src = m68k_dreg(regs, srcreg);
{	uaecptr dsta = get_disp_ea(m68k_getpc());
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_200)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_210)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_218)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_220)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_228)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_230)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s8 dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_238)(uae_u32 opcode) /* AND */
{
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_239)(uae_u32 opcode) /* AND */
{
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = nextilong();
	uae_s8 dst = get_byte(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_23c)(uae_u32 opcode) /* ANDSR */
{
{	MakeSR();
{	uae_s16 src = nextiword();
	src |= 0xFF00;
	regs.sr &= src;
	MakeFromSR();
}}}
void REGPARAM2 CPU_OP_NAME(_240)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_250)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_258)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
{	m68k_areg(regs, dstreg) += 2;
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_260)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_268)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_270)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s16 dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_278)(uae_u32 opcode) /* AND */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s16 dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_279)(uae_u32 opcode) /* AND */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = nextilong();
	uae_s16 dst = get_word(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_27c)(uae_u32 opcode) /* ANDSR */
{

{if (!regs.s) { 
#ifdef USE_POINTER
    regs.pc_p -= 2;
#else
    regs.pc -= 2;
#endif
	Exception(8,0); } else
{	MakeSR();
{	uae_s16 src = nextiword();
	regs.sr &= src;
	MakeFromSR();
}}}}
void REGPARAM2 CPU_OP_NAME(_280)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_290)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_298)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{	m68k_areg(regs, dstreg) += 4;
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2a0)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2a8)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2b0)(uae_u32 opcode) /* AND */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s32 dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_2b8)(uae_u32 opcode) /* AND */
{
{{	uae_s32 src = nextilong();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_2b9)(uae_u32 opcode) /* AND */
{
{{	uae_s32 src = nextilong();
{	uaecptr dsta = nextilong();
	uae_s32 dst = get_long(dsta);
	src &= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_400)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
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
void REGPARAM2 CPU_OP_NAME(_410)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
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
void REGPARAM2 CPU_OP_NAME(_418)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
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
void REGPARAM2 CPU_OP_NAME(_420)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
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
void REGPARAM2 CPU_OP_NAME(_428)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
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
void REGPARAM2 CPU_OP_NAME(_430)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
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
void REGPARAM2 CPU_OP_NAME(_438)(uae_u32 opcode) /* SUB */
{
{{	uae_s8 src = nextibyte();
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
void REGPARAM2 CPU_OP_NAME(_439)(uae_u32 opcode) /* SUB */
{
{{	uae_s8 src = nextibyte();
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
void REGPARAM2 CPU_OP_NAME(_440)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
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
void REGPARAM2 CPU_OP_NAME(_450)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
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
void REGPARAM2 CPU_OP_NAME(_458)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
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
void REGPARAM2 CPU_OP_NAME(_460)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
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
void REGPARAM2 CPU_OP_NAME(_468)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
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
void REGPARAM2 CPU_OP_NAME(_470)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
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
void REGPARAM2 CPU_OP_NAME(_478)(uae_u32 opcode) /* SUB */
{
{{	uae_s16 src = nextiword();
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
void REGPARAM2 CPU_OP_NAME(_479)(uae_u32 opcode) /* SUB */
{
{{	uae_s16 src = nextiword();
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
void REGPARAM2 CPU_OP_NAME(_480)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
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
void REGPARAM2 CPU_OP_NAME(_490)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
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
void REGPARAM2 CPU_OP_NAME(_498)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
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
void REGPARAM2 CPU_OP_NAME(_4a0)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
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
void REGPARAM2 CPU_OP_NAME(_4a8)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
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
void REGPARAM2 CPU_OP_NAME(_4b0)(uae_u32 opcode) /* SUB */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
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
void REGPARAM2 CPU_OP_NAME(_4b8)(uae_u32 opcode) /* SUB */
{
{{	uae_s32 src = nextilong();
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
void REGPARAM2 CPU_OP_NAME(_4b9)(uae_u32 opcode) /* SUB */
{
{{	uae_s32 src = nextilong();
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
void REGPARAM2 CPU_OP_NAME(_600)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
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
void REGPARAM2 CPU_OP_NAME(_610)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
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
void REGPARAM2 CPU_OP_NAME(_618)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
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
void REGPARAM2 CPU_OP_NAME(_620)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
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
void REGPARAM2 CPU_OP_NAME(_628)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
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
void REGPARAM2 CPU_OP_NAME(_630)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
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
void REGPARAM2 CPU_OP_NAME(_638)(uae_u32 opcode) /* ADD */
{
{{	uae_s8 src = nextibyte();
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
void REGPARAM2 CPU_OP_NAME(_639)(uae_u32 opcode) /* ADD */
{
{{	uae_s8 src = nextibyte();
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
void REGPARAM2 CPU_OP_NAME(_640)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
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
void REGPARAM2 CPU_OP_NAME(_650)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
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
void REGPARAM2 CPU_OP_NAME(_658)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
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
void REGPARAM2 CPU_OP_NAME(_660)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
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
void REGPARAM2 CPU_OP_NAME(_668)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
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
void REGPARAM2 CPU_OP_NAME(_670)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
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
void REGPARAM2 CPU_OP_NAME(_678)(uae_u32 opcode) /* ADD */
{
{{	uae_s16 src = nextiword();
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
void REGPARAM2 CPU_OP_NAME(_679)(uae_u32 opcode) /* ADD */
{
{{	uae_s16 src = nextiword();
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
void REGPARAM2 CPU_OP_NAME(_680)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
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
void REGPARAM2 CPU_OP_NAME(_690)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
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
void REGPARAM2 CPU_OP_NAME(_698)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
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
void REGPARAM2 CPU_OP_NAME(_6a0)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
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
void REGPARAM2 CPU_OP_NAME(_6a8)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
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
void REGPARAM2 CPU_OP_NAME(_6b0)(uae_u32 opcode) /* ADD */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
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
void REGPARAM2 CPU_OP_NAME(_6b8)(uae_u32 opcode) /* ADD */
{
{{	uae_s32 src = nextilong();
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
void REGPARAM2 CPU_OP_NAME(_6b9)(uae_u32 opcode) /* ADD */
{
{{	uae_s32 src = nextilong();
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
void REGPARAM2 CPU_OP_NAME(_800)(uae_u32 opcode) /* BTST */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= 31;
	ZFLG = !(dst & (1 << src));
}}}}
void REGPARAM2 CPU_OP_NAME(_810)(uae_u32 opcode) /* BTST */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void REGPARAM2 CPU_OP_NAME(_818)(uae_u32 opcode) /* BTST */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void REGPARAM2 CPU_OP_NAME(_820)(uae_u32 opcode) /* BTST */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void REGPARAM2 CPU_OP_NAME(_828)(uae_u32 opcode) /* BTST */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void REGPARAM2 CPU_OP_NAME(_830)(uae_u32 opcode) /* BTST */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void REGPARAM2 CPU_OP_NAME(_838)(uae_u32 opcode) /* BTST */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void REGPARAM2 CPU_OP_NAME(_839)(uae_u32 opcode) /* BTST */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = nextilong();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void REGPARAM2 CPU_OP_NAME(_83a)(uae_u32 opcode) /* BTST */
{
	uae_u32 dstreg = 2;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_getpc();
	dsta += (uae_s32)(uae_s16)nextiword();
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void REGPARAM2 CPU_OP_NAME(_83b)(uae_u32 opcode) /* BTST */
{
	uae_u32 dstreg = 3;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_getpc());
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}}
void REGPARAM2 CPU_OP_NAME(_83c)(uae_u32 opcode) /* BTST */
{
{{	uae_s16 src = nextiword();
{	uae_s8 dst = nextibyte();
	src &= 7;
	ZFLG = !(dst & (1 << src));
}}}}
void REGPARAM2 CPU_OP_NAME(_840)(uae_u32 opcode) /* BCHG */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= 31;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	m68k_dreg(regs, dstreg) = (dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_850)(uae_u32 opcode) /* BCHG */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_858)(uae_u32 opcode) /* BCHG */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_860)(uae_u32 opcode) /* BCHG */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_868)(uae_u32 opcode) /* BCHG */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_870)(uae_u32 opcode) /* BCHG */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_878)(uae_u32 opcode) /* BCHG */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_879)(uae_u32 opcode) /* BCHG */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = nextilong();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_87a)(uae_u32 opcode) /* BCHG */
{
	uae_u32 dstreg = 2;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_getpc();
	dsta += (uae_s32)(uae_s16)nextiword();
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_87b)(uae_u32 opcode) /* BCHG */
{
	uae_u32 dstreg = 3;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_getpc());
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst ^= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_880)(uae_u32 opcode) /* BCLR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= 31;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	m68k_dreg(regs, dstreg) = (dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_890)(uae_u32 opcode) /* BCLR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_898)(uae_u32 opcode) /* BCLR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8a0)(uae_u32 opcode) /* BCLR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8a8)(uae_u32 opcode) /* BCLR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_8b0)(uae_u32 opcode) /* BCLR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8b8)(uae_u32 opcode) /* BCLR */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_8b9)(uae_u32 opcode) /* BCLR */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = nextilong();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_8ba)(uae_u32 opcode) /* BCLR */
{
	uae_u32 dstreg = 2;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_getpc();
	dsta += (uae_s32)(uae_s16)nextiword();
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8bb)(uae_u32 opcode) /* BCLR */
{
	uae_u32 dstreg = 3;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_getpc());
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst &= ~(1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8c0)(uae_u32 opcode) /* BSET */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src &= 31;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	m68k_dreg(regs, dstreg) = (dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_8d0)(uae_u32 opcode) /* BSET */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_8d8)(uae_u32 opcode) /* BSET */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8e0)(uae_u32 opcode) /* BSET */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8e8)(uae_u32 opcode) /* BSET */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_8f0)(uae_u32 opcode) /* BSET */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8f8)(uae_u32 opcode) /* BSET */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_8f9)(uae_u32 opcode) /* BSET */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = nextilong();
	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}
void REGPARAM2 CPU_OP_NAME(_8fa)(uae_u32 opcode) /* BSET */
{
	uae_u32 dstreg = 2;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_getpc();
	dsta += (uae_s32)(uae_s16)nextiword();
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_8fb)(uae_u32 opcode) /* BSET */
{
	uae_u32 dstreg = 3;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_getpc());
{	uae_s8 dst = get_byte(dsta);
	src &= 7;
	ZFLG = !(dst & (1 << src));
	dst |= (1 << src);
	put_byte(dsta,dst);
}}}}}
void REGPARAM2 CPU_OP_NAME(_a00)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uae_s8 dst = m68k_dreg(regs, dstreg);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}
void REGPARAM2 CPU_OP_NAME(_a10)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_a18)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_a20)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_a28)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_a30)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s8 dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_a38)(uae_u32 opcode) /* EOR */
{
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_a39)(uae_u32 opcode) /* EOR */
{
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = nextilong();
	uae_s8 dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s8)(src)) == 0;
	NFLG = ((uae_s8)(src)) < 0;
	put_byte(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_a3c)(uae_u32 opcode) /* EORSR */
{
{	MakeSR();
{	uae_s16 src = nextiword();
	src &= 0xFF;
	regs.sr ^= src;
	MakeFromSR();
}}}
void REGPARAM2 CPU_OP_NAME(_a40)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uae_s16 dst = m68k_dreg(regs, dstreg);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}
void REGPARAM2 CPU_OP_NAME(_a50)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_a58)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
{	m68k_areg(regs, dstreg) += 2;
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_a60)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_a68)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_a70)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s16 dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_a78)(uae_u32 opcode) /* EOR */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s16 dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_a79)(uae_u32 opcode) /* EOR */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = nextilong();
	uae_s16 dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s16)(src)) == 0;
	NFLG = ((uae_s16)(src)) < 0;
	put_word(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_a7c)(uae_u32 opcode) /* EORSR */
{
{if (!regs.s) { 
#ifdef USE_POINTER
    regs.pc_p -= 2;
#else
    regs.pc -= 2;
#endif
	Exception(8,0); } else
{	MakeSR();
{	uae_s16 src = nextiword();
	regs.sr ^= src;
	MakeFromSR();
}}}}
void REGPARAM2 CPU_OP_NAME(_a80)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uae_s32 dst = m68k_dreg(regs, dstreg);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
void REGPARAM2 CPU_OP_NAME(_a90)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_a98)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{	m68k_areg(regs, dstreg) += 4;
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_aa0)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_aa8)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_ab0)(uae_u32 opcode) /* EOR */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s32 dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}}
void REGPARAM2 CPU_OP_NAME(_ab8)(uae_u32 opcode) /* EOR */
{
{{	uae_s32 src = nextilong();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_ab9)(uae_u32 opcode) /* EOR */
{
{{	uae_s32 src = nextilong();
{	uaecptr dsta = nextilong();
	uae_s32 dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	put_long(dsta,src);
}}}}
void REGPARAM2 CPU_OP_NAME(_c00)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uae_s8 dst = m68k_dreg(regs, dstreg);
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c10)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c18)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{	m68k_areg(regs, dstreg) += areg_byteinc[dstreg];
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c20)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	m68k_areg(regs, dstreg) -= areg_byteinc[dstreg];
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c28)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c30)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c38)(uae_u32 opcode) /* CMP */
{
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c39)(uae_u32 opcode) /* CMP */
{
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = nextilong();
	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c3a)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = 2;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = m68k_getpc();
	dsta += (uae_s32)(uae_s16)nextiword();
{	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c3b)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = 3;
{{	uae_s8 src = nextibyte();
{	uaecptr dsta = get_disp_ea(m68k_getpc());
{	uae_s8 dst = get_byte(dsta);
{{uae_u32 newv = ((uae_s8)(dst)) - ((uae_s8)(src));
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	ZFLG = ((uae_s8)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u8)(src)) > ((uae_u8)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c40)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uae_s16 dst = m68k_dreg(regs, dstreg);
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c50)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c58)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
{	m68k_areg(regs, dstreg) += 2;
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c60)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	m68k_areg(regs, dstreg) -= 2;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c68)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c70)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c78)(uae_u32 opcode) /* CMP */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c79)(uae_u32 opcode) /* CMP */
{
{{	uae_s16 src = nextiword();
{	uaecptr dsta = nextilong();
	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c7a)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = 2;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = m68k_getpc();
	dsta += (uae_s32)(uae_s16)nextiword();
{	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c7b)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = 3;
{{	uae_s16 src = nextiword();
{	uaecptr dsta = get_disp_ea(m68k_getpc());
{	uae_s16 dst = get_word(dsta);
{{uae_u32 newv = ((uae_s16)(dst)) - ((uae_s16)(src));
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	ZFLG = ((uae_s16)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u16)(src)) > ((uae_u16)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c80)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uae_s32 dst = m68k_dreg(regs, dstreg);
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c90)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_c98)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{	m68k_areg(regs, dstreg) += 4;
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_ca0)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	m68k_areg(regs, dstreg) -= 4;
{	uaecptr dsta = m68k_areg(regs, dstreg);
	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_ca8)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = m68k_areg(regs, dstreg) + (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_cb0)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = get_disp_ea(m68k_areg(regs, dstreg));
{	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_cb8)(uae_u32 opcode) /* CMP */
{
{{	uae_s32 src = nextilong();
{	uaecptr dsta = (uae_s32)(uae_s16)nextiword();
	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_cb9)(uae_u32 opcode) /* CMP */
{
{{	uae_s32 src = nextilong();
{	uaecptr dsta = nextilong();
	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
}}}}}}}
void REGPARAM2 CPU_OP_NAME(_cba)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = 2;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = m68k_getpc();
	dsta += (uae_s32)(uae_s16)nextiword();
{	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void REGPARAM2 CPU_OP_NAME(_cbb)(uae_u32 opcode) /* CMP */
{
	uae_u32 dstreg = 3;
{{	uae_s32 src = nextilong();
{	uaecptr dsta = get_disp_ea(m68k_getpc());
{	uae_s32 dst = get_long(dsta);
{{uae_u32 newv = ((uae_s32)(dst)) - ((uae_s32)(src));
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	ZFLG = ((uae_s32)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((uae_u32)(src)) > ((uae_u32)(dst));
	NFLG = flgn != 0;
}}}}}}}}
#endif
