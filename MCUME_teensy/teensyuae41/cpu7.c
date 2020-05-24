#include "shared.h"

#include "memory.h"
#include "custom.h"
#include "newcpu.h"
#include "cputbl.h"
void op_7000(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
	ULONG dstreg = (opcode & 3584) >> 9;
{{	ULONG src = srcreg;
{	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	regs.d[dstreg] = (src);
}}}}
