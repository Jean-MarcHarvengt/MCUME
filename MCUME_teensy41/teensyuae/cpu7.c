#include "shared.h"

#include "machdep/m68k.h"
#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "compiler.h"
#include "cputbl.h"
#if !defined (MEMFUNCS_DIRECT_REQUESTED) || defined (DIRECT_MEMFUNCS_SUCCESSFUL)
void REGPARAM2 CPU_OP_NAME(_7000)(uae_u32 opcode) /* MOVE */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_u32 src = srcreg;
{	VFLG = CFLG = 0;
	ZFLG = ((uae_s32)(src)) == 0;
	NFLG = ((uae_s32)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}}}}
#endif

