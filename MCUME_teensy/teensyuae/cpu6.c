#include "shared.h"

#include "memory.h"
#include "custom.h"
#include "newcpu.h"
#include "cputbl.h"
void op_6000(ULONG opcode)
{
{	char *oldpcp = (char *)regs.pc_p;
{	WORD src = nextiword();
	if (cctrue(0)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6001(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
{	char *oldpcp = (char *)regs.pc_p;
{	ULONG src = srcreg;
	if (cctrue(0)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6100(ULONG opcode)
{
{	char *oldpcp = (char *)regs.pc_p;
{	WORD src = nextiword();
{	regs.a[7] -= 4;
{	CPTR spa = regs.a[7];
	put_long(spa,m68k_getpc());
	regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}}}
void op_6101(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
{	char *oldpcp = (char *)regs.pc_p;
{	ULONG src = srcreg;
{	regs.a[7] -= 4;
{	CPTR spa = regs.a[7];
	put_long(spa,m68k_getpc());
	regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}}}
void op_6200(ULONG opcode)
{
{	char *oldpcp = (char *)regs.pc_p;
{	WORD src = nextiword();
	if (cctrue(2)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6201(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
{	char *oldpcp = (char *)regs.pc_p;
{	ULONG src = srcreg;
	if (cctrue(2)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6300(ULONG opcode)
{
{	char *oldpcp = (char *)regs.pc_p;
{	WORD src = nextiword();
	if (cctrue(3)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6301(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
{	char *oldpcp = (char *)regs.pc_p;
{	ULONG src = srcreg;
	if (cctrue(3)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6400(ULONG opcode)
{
{	char *oldpcp = (char *)regs.pc_p;
{	WORD src = nextiword();
	if (cctrue(4)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6401(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
{	char *oldpcp = (char *)regs.pc_p;
{	ULONG src = srcreg;
	if (cctrue(4)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6500(ULONG opcode)
{
{	char *oldpcp = (char *)regs.pc_p;
{	WORD src = nextiword();
	if (cctrue(5)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6501(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
{	char *oldpcp = (char *)regs.pc_p;
{	ULONG src = srcreg;
	if (cctrue(5)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6600(ULONG opcode)
{
{	char *oldpcp = (char *)regs.pc_p;
{	WORD src = nextiword();
	if (cctrue(6)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6601(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
{	char *oldpcp = (char *)regs.pc_p;
{	ULONG src = srcreg;
	if (cctrue(6)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6700(ULONG opcode)
{
{	char *oldpcp = (char *)regs.pc_p;
{	WORD src = nextiword();
	if (cctrue(7)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6701(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
{	char *oldpcp = (char *)regs.pc_p;
{	ULONG src = srcreg;
	if (cctrue(7)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6800(ULONG opcode)
{
{	char *oldpcp = (char *)regs.pc_p;
{	WORD src = nextiword();
	if (cctrue(8)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6801(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
{	char *oldpcp = (char *)regs.pc_p;
{	ULONG src = srcreg;
	if (cctrue(8)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6900(ULONG opcode)
{
{	char *oldpcp = (char *)regs.pc_p;
{	WORD src = nextiword();
	if (cctrue(9)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6901(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
{	char *oldpcp = (char *)regs.pc_p;
{	ULONG src = srcreg;
	if (cctrue(9)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6a00(ULONG opcode)
{
{	char *oldpcp = (char *)regs.pc_p;
{	WORD src = nextiword();
	if (cctrue(10)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6a01(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
{	char *oldpcp = (char *)regs.pc_p;
{	ULONG src = srcreg;
	if (cctrue(10)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6b00(ULONG opcode)
{
{	char *oldpcp = (char *)regs.pc_p;
{	WORD src = nextiword();
	if (cctrue(11)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6b01(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
{	char *oldpcp = (char *)regs.pc_p;
{	ULONG src = srcreg;
	if (cctrue(11)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6c00(ULONG opcode)
{
{	char *oldpcp = (char *)regs.pc_p;
{	WORD src = nextiword();
	if (cctrue(12)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6c01(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
{	char *oldpcp = (char *)regs.pc_p;
{	ULONG src = srcreg;
	if (cctrue(12)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6d00(ULONG opcode)
{
{	char *oldpcp = (char *)regs.pc_p;
{	WORD src = nextiword();
	if (cctrue(13)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6d01(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
{	char *oldpcp = (char *)regs.pc_p;
{	ULONG src = srcreg;
	if (cctrue(13)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6e00(ULONG opcode)
{
{	char *oldpcp = (char *)regs.pc_p;
{	WORD src = nextiword();
	if (cctrue(14)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6e01(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
{	char *oldpcp = (char *)regs.pc_p;
{	ULONG src = srcreg;
	if (cctrue(14)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6f00(ULONG opcode)
{
{	char *oldpcp = (char *)regs.pc_p;
{	WORD src = nextiword();
	if (cctrue(15)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
void op_6f01(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 255) >> 0);
{	char *oldpcp = (char *)regs.pc_p;
{	ULONG src = srcreg;
	if (cctrue(15)) regs.pc_p = (UWORD *)(oldpcp + (LONG)src);
}}}
