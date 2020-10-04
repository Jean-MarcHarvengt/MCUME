#include "shared.h"

#include "machdep/m68k.h"
#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "compiler.h"
#include "cputbl.h"
#if !defined (MEMFUNCS_DIRECT_REQUESTED) || defined (DIRECT_MEMFUNCS_SUCCESSFUL)
void REGPARAM2 CPU_OP_NAME(_6000)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s16 src = nextiword();
	if (cctrue(0)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6001)(uae_u32 opcode) /* Bcc */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	char *oldpcp = (char *)regs.pc_p;
{	uae_u32 src = srcreg;
	if (cctrue(0)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_60ff)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s32 src = nextilong();
	if (cctrue(0)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6100)(uae_u32 opcode) /* BSR */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s16 src = nextiword();
	uae_s32 s = (uae_s32)src - (((char *)regs.pc_p) - oldpcp);
	m68k_do_bsr(s);
	fill_prefetch();
}}}
void REGPARAM2 CPU_OP_NAME(_6101)(uae_u32 opcode) /* BSR */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	char *oldpcp = (char *)regs.pc_p;
{	uae_u32 src = srcreg;
	uae_s32 s = (uae_s32)src - (((char *)regs.pc_p) - oldpcp);
	m68k_do_bsr(s);
	fill_prefetch();
}}}
void REGPARAM2 CPU_OP_NAME(_61ff)(uae_u32 opcode) /* BSR */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s32 src = nextilong();
	uae_s32 s = (uae_s32)src - (((char *)regs.pc_p) - oldpcp);
	m68k_do_bsr(s);
	fill_prefetch();
}}}
void REGPARAM2 CPU_OP_NAME(_6200)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s16 src = nextiword();
	if (cctrue(2)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6201)(uae_u32 opcode) /* Bcc */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	char *oldpcp = (char *)regs.pc_p;
{	uae_u32 src = srcreg;
	if (cctrue(2)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_62ff)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s32 src = nextilong();
	if (cctrue(2)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6300)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s16 src = nextiword();
	if (cctrue(3)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6301)(uae_u32 opcode) /* Bcc */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	char *oldpcp = (char *)regs.pc_p;
{	uae_u32 src = srcreg;
	if (cctrue(3)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_63ff)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s32 src = nextilong();
	if (cctrue(3)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6400)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s16 src = nextiword();
	if (cctrue(4)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6401)(uae_u32 opcode) /* Bcc */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	char *oldpcp = (char *)regs.pc_p;
{	uae_u32 src = srcreg;
	if (cctrue(4)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_64ff)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s32 src = nextilong();
	if (cctrue(4)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6500)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s16 src = nextiword();
	if (cctrue(5)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6501)(uae_u32 opcode) /* Bcc */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	char *oldpcp = (char *)regs.pc_p;
{	uae_u32 src = srcreg;
	if (cctrue(5)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_65ff)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s32 src = nextilong();
	if (cctrue(5)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6600)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s16 src = nextiword();
	if (cctrue(6)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6601)(uae_u32 opcode) /* Bcc */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	char *oldpcp = (char *)regs.pc_p;
{	uae_u32 src = srcreg;
	if (cctrue(6)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_66ff)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s32 src = nextilong();
	if (cctrue(6)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6700)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s16 src = nextiword();
	if (cctrue(7)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6701)(uae_u32 opcode) /* Bcc */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	char *oldpcp = (char *)regs.pc_p;
{	uae_u32 src = srcreg;
	if (cctrue(7)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_67ff)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s32 src = nextilong();
	if (cctrue(7)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6800)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s16 src = nextiword();
	if (cctrue(8)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6801)(uae_u32 opcode) /* Bcc */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	char *oldpcp = (char *)regs.pc_p;
{	uae_u32 src = srcreg;
	if (cctrue(8)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_68ff)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s32 src = nextilong();
	if (cctrue(8)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6900)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s16 src = nextiword();
	if (cctrue(9)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6901)(uae_u32 opcode) /* Bcc */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	char *oldpcp = (char *)regs.pc_p;
{	uae_u32 src = srcreg;
	if (cctrue(9)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_69ff)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s32 src = nextilong();
	if (cctrue(9)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6a00)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s16 src = nextiword();
	if (cctrue(10)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6a01)(uae_u32 opcode) /* Bcc */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	char *oldpcp = (char *)regs.pc_p;
{	uae_u32 src = srcreg;
	if (cctrue(10)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6aff)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s32 src = nextilong();
	if (cctrue(10)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6b00)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s16 src = nextiword();
	if (cctrue(11)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6b01)(uae_u32 opcode) /* Bcc */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	char *oldpcp = (char *)regs.pc_p;
{	uae_u32 src = srcreg;
	if (cctrue(11)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6bff)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s32 src = nextilong();
	if (cctrue(11)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6c00)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s16 src = nextiword();
	if (cctrue(12)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6c01)(uae_u32 opcode) /* Bcc */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	char *oldpcp = (char *)regs.pc_p;
{	uae_u32 src = srcreg;
	if (cctrue(12)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6cff)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s32 src = nextilong();
	if (cctrue(12)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6d00)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s16 src = nextiword();
	if (cctrue(13)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6d01)(uae_u32 opcode) /* Bcc */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	char *oldpcp = (char *)regs.pc_p;
{	uae_u32 src = srcreg;
	if (cctrue(13)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6dff)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s32 src = nextilong();
	if (cctrue(13)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6e00)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s16 src = nextiword();
	if (cctrue(14)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6e01)(uae_u32 opcode) /* Bcc */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	char *oldpcp = (char *)regs.pc_p;
{	uae_u32 src = srcreg;
	if (cctrue(14)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6eff)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s32 src = nextilong();
	if (cctrue(14)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6f00)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s16 src = nextiword();
	if (cctrue(15)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6f01)(uae_u32 opcode) /* Bcc */
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	char *oldpcp = (char *)regs.pc_p;
{	uae_u32 src = srcreg;
	if (cctrue(15)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
void REGPARAM2 CPU_OP_NAME(_6fff)(uae_u32 opcode) /* Bcc */
{
{	char *oldpcp = (char *)regs.pc_p;
{	uae_s32 src = nextilong();
	if (cctrue(15)) {
	regs.pc_p = (uae_u8 *)(oldpcp + (uae_s32)src);
	fill_prefetch();
	}
}}}
#endif
