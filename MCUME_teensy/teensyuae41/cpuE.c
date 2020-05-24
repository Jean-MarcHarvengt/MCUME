#include "shared.h"

#include "memory.h"
#include "custom.h"
#include "newcpu.h"
#include "cputbl.h"
void op_e000(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	BYTE data = regs.d[dstreg];
{	UBYTE val = data;
	ULONG cmask = 0x80;
	ULONG sign = cmask & val;
	cnt &= 63;
	VFLG = 0;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	CFLG=regs.x=val&1; val = ((ULONG)val >> 1) | sign;
	}}
	NFLG = sign != 0;
	ZFLG = val == 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (val) & 0xff;
}}}}}
void op_e008(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	BYTE data = regs.d[dstreg];
{	UBYTE val = data;
	ULONG cmask = 0x80;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	int carry = 0;
	for(;cnt;--cnt){
	carry=val&1; val >>= 1;
	}
	CFLG = regs.x = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (val) & 0xff;
}}}}}
void op_e010(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	BYTE data = regs.d[dstreg];
{	UBYTE val = data;
	ULONG cmask = 0x80;
	int carry = 0;
	cnt &= 63;
	for(;cnt;--cnt){
	carry=val&1; val >>= 1;
	if(regs.x) val |= cmask;
	regs.x = carry != 0;
	}
	CFLG = regs.x;
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (val) & 0xff;
}}}}}
void op_e018(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	BYTE data = regs.d[dstreg];
{	UBYTE val = data;
	ULONG cmask = 0x80;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&1; val = (ULONG)val >> 1;
	if(carry) val |= cmask;
	}
	CFLG = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (val) & 0xff;
}}}}}
void op_e020(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE cnt = regs.d[srcreg];
{	BYTE data = regs.d[dstreg];
{	UBYTE val = data;
	ULONG cmask = 0x80;
	ULONG sign = cmask & val;
	cnt &= 63;
	VFLG = 0;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	CFLG=regs.x=val&1; val = ((ULONG)val >> 1) | sign;
	}}
	NFLG = sign != 0;
	ZFLG = val == 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (val) & 0xff;
}}}}}
void op_e028(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE cnt = regs.d[srcreg];
{	BYTE data = regs.d[dstreg];
{	UBYTE val = data;
	ULONG cmask = 0x80;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	int carry = 0;
	for(;cnt;--cnt){
	carry=val&1; val >>= 1;
	}
	CFLG = regs.x = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (val) & 0xff;
}}}}}
void op_e030(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE cnt = regs.d[srcreg];
{	BYTE data = regs.d[dstreg];
{	UBYTE val = data;
	ULONG cmask = 0x80;
	int carry = 0;
	cnt &= 63;
	for(;cnt;--cnt){
	carry=val&1; val >>= 1;
	if(regs.x) val |= cmask;
	regs.x = carry != 0;
	}
	CFLG = regs.x;
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (val) & 0xff;
}}}}}
void op_e038(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE cnt = regs.d[srcreg];
{	BYTE data = regs.d[dstreg];
{	UBYTE val = data;
	ULONG cmask = 0x80;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&1; val = (ULONG)val >> 1;
	if(carry) val |= cmask;
	}
	CFLG = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (val) & 0xff;
}}}}}
void op_e040(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	WORD data = regs.d[dstreg];
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	cnt &= 63;
	VFLG = 0;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	CFLG=regs.x=val&1; val = ((ULONG)val >> 1) | sign;
	}}
	NFLG = sign != 0;
	ZFLG = val == 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}}}
void op_e048(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	WORD data = regs.d[dstreg];
{	UWORD val = data;
	ULONG cmask = 0x8000;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	int carry = 0;
	for(;cnt;--cnt){
	carry=val&1; val >>= 1;
	}
	CFLG = regs.x = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}}}
void op_e050(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	WORD data = regs.d[dstreg];
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = 0;
	cnt &= 63;
	for(;cnt;--cnt){
	carry=val&1; val >>= 1;
	if(regs.x) val |= cmask;
	regs.x = carry != 0;
	}
	CFLG = regs.x;
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}}}
void op_e058(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	WORD data = regs.d[dstreg];
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&1; val = (ULONG)val >> 1;
	if(carry) val |= cmask;
	}
	CFLG = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}}}
void op_e060(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD cnt = regs.d[srcreg];
{	WORD data = regs.d[dstreg];
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	cnt &= 63;
	VFLG = 0;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	CFLG=regs.x=val&1; val = ((ULONG)val >> 1) | sign;
	}}
	NFLG = sign != 0;
	ZFLG = val == 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}}}
void op_e068(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD cnt = regs.d[srcreg];
{	WORD data = regs.d[dstreg];
{	UWORD val = data;
	ULONG cmask = 0x8000;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	int carry = 0;
	for(;cnt;--cnt){
	carry=val&1; val >>= 1;
	}
	CFLG = regs.x = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}}}
void op_e070(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD cnt = regs.d[srcreg];
{	WORD data = regs.d[dstreg];
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = 0;
	cnt &= 63;
	for(;cnt;--cnt){
	carry=val&1; val >>= 1;
	if(regs.x) val |= cmask;
	regs.x = carry != 0;
	}
	CFLG = regs.x;
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}}}
void op_e078(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD cnt = regs.d[srcreg];
{	WORD data = regs.d[dstreg];
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&1; val = (ULONG)val >> 1;
	if(carry) val |= cmask;
	}
	CFLG = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}}}
void op_e080(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	LONG data = regs.d[dstreg];
{	ULONG val = data;
	ULONG cmask = 0x80000000;
	ULONG sign = cmask & val;
	cnt &= 63;
	VFLG = 0;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	CFLG=regs.x=val&1; val = ((ULONG)val >> 1) | sign;
	}}
	NFLG = sign != 0;
	ZFLG = val == 0;
	regs.d[dstreg] = (val);
}}}}}
void op_e088(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	LONG data = regs.d[dstreg];
{	ULONG val = data;
	ULONG cmask = 0x80000000;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	int carry = 0;
	for(;cnt;--cnt){
	carry=val&1; val >>= 1;
	}
	CFLG = regs.x = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] = (val);
}}}}}
void op_e090(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	LONG data = regs.d[dstreg];
{	ULONG val = data;
	ULONG cmask = 0x80000000;
	int carry = 0;
	cnt &= 63;
	for(;cnt;--cnt){
	carry=val&1; val >>= 1;
	if(regs.x) val |= cmask;
	regs.x = carry != 0;
	}
	CFLG = regs.x;
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] = (val);
}}}}}
void op_e098(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	LONG data = regs.d[dstreg];
{	ULONG val = data;
	ULONG cmask = 0x80000000;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&1; val = (ULONG)val >> 1;
	if(carry) val |= cmask;
	}
	CFLG = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] = (val);
}}}}}
void op_e0a0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG cnt = regs.d[srcreg];
{	LONG data = regs.d[dstreg];
{	ULONG val = data;
	ULONG cmask = 0x80000000;
	ULONG sign = cmask & val;
	cnt &= 63;
	VFLG = 0;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	CFLG=regs.x=val&1; val = ((ULONG)val >> 1) | sign;
	}}
	NFLG = sign != 0;
	ZFLG = val == 0;
	regs.d[dstreg] = (val);
}}}}}
void op_e0a8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG cnt = regs.d[srcreg];
{	LONG data = regs.d[dstreg];
{	ULONG val = data;
	ULONG cmask = 0x80000000;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	int carry = 0;
	for(;cnt;--cnt){
	carry=val&1; val >>= 1;
	}
	CFLG = regs.x = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] = (val);
}}}}}
void op_e0b0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG cnt = regs.d[srcreg];
{	LONG data = regs.d[dstreg];
{	ULONG val = data;
	ULONG cmask = 0x80000000;
	int carry = 0;
	cnt &= 63;
	for(;cnt;--cnt){
	carry=val&1; val >>= 1;
	if(regs.x) val |= cmask;
	regs.x = carry != 0;
	}
	CFLG = regs.x;
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] = (val);
}}}}}
void op_e0b8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG cnt = regs.d[srcreg];
{	LONG data = regs.d[dstreg];
{	ULONG val = data;
	ULONG cmask = 0x80000000;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&1; val = (ULONG)val >> 1;
	if(carry) val |= cmask;
	}
	CFLG = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] = (val);
}}}}}
void op_e0d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
	VFLG = 0;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	CFLG=regs.x=val&1; val = (val >> 1) | sign;
	NFLG = sign != 0;
	ZFLG = val == 0;
	put_word(dataa,val);
}}}}
void op_e0d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	regs.a[srcreg] += 2;
	VFLG = 0;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	CFLG=regs.x=val&1; val = (val >> 1) | sign;
	NFLG = sign != 0;
	ZFLG = val == 0;
	put_word(dataa,val);
}}}}}
void op_e0e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 2;
{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
	VFLG = 0;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	CFLG=regs.x=val&1; val = (val >> 1) | sign;
	NFLG = sign != 0;
	ZFLG = val == 0;
	put_word(dataa,val);
}}}}}
void op_e0e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg] + (LONG)(WORD)nextiword();
	WORD data = get_word(dataa);
	VFLG = 0;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	CFLG=regs.x=val&1; val = (val >> 1) | sign;
	NFLG = sign != 0;
	ZFLG = val == 0;
	put_word(dataa,val);
}}}}
void op_e0f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	dataa = get_disp_ea (dataa, nextiword());
{	WORD data = get_word(dataa);
	VFLG = 0;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	CFLG=regs.x=val&1; val = (val >> 1) | sign;
	NFLG = sign != 0;
	ZFLG = val == 0;
	put_word(dataa,val);
}}}}}
void op_e0f8(ULONG opcode)
{
{{	CPTR dataa = (LONG)(WORD)nextiword();
	WORD data = get_word(dataa);
	VFLG = 0;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	CFLG=regs.x=val&1; val = (val >> 1) | sign;
	NFLG = sign != 0;
	ZFLG = val == 0;
	put_word(dataa,val);
}}}}
void op_e0f9(ULONG opcode)
{
{{	CPTR dataa = nextilong();
	WORD data = get_word(dataa);
	VFLG = 0;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	CFLG=regs.x=val&1; val = (val >> 1) | sign;
	NFLG = sign != 0;
	ZFLG = val == 0;
	put_word(dataa,val);
}}}}
void op_e100(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	BYTE data = regs.d[dstreg];
{	UBYTE val = data;
	ULONG cmask = 0x80;
	ULONG sign = cmask & val;
	cnt &= 63;
	VFLG = 0;
	if (!cnt) { CFLG = 0; } else { 	for(;cnt;--cnt){
	CFLG=regs.x=(val&cmask)!=0; val <<= 1;
	if ((val&cmask)!=sign)VFLG=1;
	}}
	NFLG = (val&cmask) != 0;
	ZFLG = val == 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (val) & 0xff;
}}}}}
void op_e108(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	BYTE data = regs.d[dstreg];
{	UBYTE val = data;
	ULONG cmask = 0x80;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	}
	CFLG = regs.x = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (val) & 0xff;
}}}}}
void op_e110(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	BYTE data = regs.d[dstreg];
{	UBYTE val = data;
	ULONG cmask = 0x80;
	int carry = 0;
	cnt &= 63;
	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	if(regs.x) val |= 1;
	regs.x = carry != 0;
	}
	CFLG = regs.x;
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (val) & 0xff;
}}}}}
void op_e118(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	BYTE data = regs.d[dstreg];
{	UBYTE val = data;
	ULONG cmask = 0x80;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	if(carry)  val |= 1;
	}
	CFLG = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (val) & 0xff;
}}}}}
void op_e120(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE cnt = regs.d[srcreg];
{	BYTE data = regs.d[dstreg];
{	UBYTE val = data;
	ULONG cmask = 0x80;
	ULONG sign = cmask & val;
	cnt &= 63;
	VFLG = 0;
	if (!cnt) { CFLG = 0; } else { 	for(;cnt;--cnt){
	CFLG=regs.x=(val&cmask)!=0; val <<= 1;
	if ((val&cmask)!=sign)VFLG=1;
	}}
	NFLG = (val&cmask) != 0;
	ZFLG = val == 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (val) & 0xff;
}}}}}
void op_e128(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE cnt = regs.d[srcreg];
{	BYTE data = regs.d[dstreg];
{	UBYTE val = data;
	ULONG cmask = 0x80;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	}
	CFLG = regs.x = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (val) & 0xff;
}}}}}
void op_e130(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE cnt = regs.d[srcreg];
{	BYTE data = regs.d[dstreg];
{	UBYTE val = data;
	ULONG cmask = 0x80;
	int carry = 0;
	cnt &= 63;
	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	if(regs.x) val |= 1;
	regs.x = carry != 0;
	}
	CFLG = regs.x;
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (val) & 0xff;
}}}}}
void op_e138(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	BYTE cnt = regs.d[srcreg];
{	BYTE data = regs.d[dstreg];
{	UBYTE val = data;
	ULONG cmask = 0x80;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	if(carry)  val |= 1;
	}
	CFLG = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xff; regs.d[dstreg] |= (val) & 0xff;
}}}}}
void op_e140(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	WORD data = regs.d[dstreg];
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	cnt &= 63;
	VFLG = 0;
	if (!cnt) { CFLG = 0; } else { 	for(;cnt;--cnt){
	CFLG=regs.x=(val&cmask)!=0; val <<= 1;
	if ((val&cmask)!=sign)VFLG=1;
	}}
	NFLG = (val&cmask) != 0;
	ZFLG = val == 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}}}
void op_e148(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	WORD data = regs.d[dstreg];
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	}
	CFLG = regs.x = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}}}
void op_e150(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	WORD data = regs.d[dstreg];
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = 0;
	cnt &= 63;
	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	if(regs.x) val |= 1;
	regs.x = carry != 0;
	}
	CFLG = regs.x;
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}}}
void op_e158(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	WORD data = regs.d[dstreg];
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	if(carry)  val |= 1;
	}
	CFLG = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}}}
void op_e160(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD cnt = regs.d[srcreg];
{	WORD data = regs.d[dstreg];
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	cnt &= 63;
	VFLG = 0;
	if (!cnt) { CFLG = 0; } else { 	for(;cnt;--cnt){
	CFLG=regs.x=(val&cmask)!=0; val <<= 1;
	if ((val&cmask)!=sign)VFLG=1;
	}}
	NFLG = (val&cmask) != 0;
	ZFLG = val == 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}}}
void op_e168(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD cnt = regs.d[srcreg];
{	WORD data = regs.d[dstreg];
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	}
	CFLG = regs.x = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}}}
void op_e170(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD cnt = regs.d[srcreg];
{	WORD data = regs.d[dstreg];
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = 0;
	cnt &= 63;
	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	if(regs.x) val |= 1;
	regs.x = carry != 0;
	}
	CFLG = regs.x;
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}}}
void op_e178(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	WORD cnt = regs.d[srcreg];
{	WORD data = regs.d[dstreg];
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	if(carry)  val |= 1;
	}
	CFLG = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] &= ~0xffff; regs.d[dstreg] |= (val) & 0xffff;
}}}}}
void op_e180(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	LONG data = regs.d[dstreg];
{	ULONG val = data;
	ULONG cmask = 0x80000000;
	ULONG sign = cmask & val;
	cnt &= 63;
	VFLG = 0;
	if (!cnt) { CFLG = 0; } else { 	for(;cnt;--cnt){
	CFLG=regs.x=(val&cmask)!=0; val <<= 1;
	if ((val&cmask)!=sign)VFLG=1;
	}}
	NFLG = (val&cmask) != 0;
	ZFLG = val == 0;
	regs.d[dstreg] = (val);
}}}}}
void op_e188(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	LONG data = regs.d[dstreg];
{	ULONG val = data;
	ULONG cmask = 0x80000000;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	}
	CFLG = regs.x = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] = (val);
}}}}}
void op_e190(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	LONG data = regs.d[dstreg];
{	ULONG val = data;
	ULONG cmask = 0x80000000;
	int carry = 0;
	cnt &= 63;
	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	if(regs.x) val |= 1;
	regs.x = carry != 0;
	}
	CFLG = regs.x;
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] = (val);
}}}}}
void op_e198(ULONG opcode)
{
	ULONG srcreg = imm8_table[(opcode & 3584) >> 9];
	ULONG dstreg = (opcode & 7) >> 0;
{{	ULONG cnt = srcreg;
{	LONG data = regs.d[dstreg];
{	ULONG val = data;
	ULONG cmask = 0x80000000;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	if(carry)  val |= 1;
	}
	CFLG = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] = (val);
}}}}}
void op_e1a0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG cnt = regs.d[srcreg];
{	LONG data = regs.d[dstreg];
{	ULONG val = data;
	ULONG cmask = 0x80000000;
	ULONG sign = cmask & val;
	cnt &= 63;
	VFLG = 0;
	if (!cnt) { CFLG = 0; } else { 	for(;cnt;--cnt){
	CFLG=regs.x=(val&cmask)!=0; val <<= 1;
	if ((val&cmask)!=sign)VFLG=1;
	}}
	NFLG = (val&cmask) != 0;
	ZFLG = val == 0;
	regs.d[dstreg] = (val);
}}}}}
void op_e1a8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG cnt = regs.d[srcreg];
{	LONG data = regs.d[dstreg];
{	ULONG val = data;
	ULONG cmask = 0x80000000;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	}
	CFLG = regs.x = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] = (val);
}}}}}
void op_e1b0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG cnt = regs.d[srcreg];
{	LONG data = regs.d[dstreg];
{	ULONG val = data;
	ULONG cmask = 0x80000000;
	int carry = 0;
	cnt &= 63;
	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	if(regs.x) val |= 1;
	regs.x = carry != 0;
	}
	CFLG = regs.x;
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] = (val);
}}}}}
void op_e1b8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 3584) >> 9);
	ULONG dstreg = (opcode & 7) >> 0;
{{	LONG cnt = regs.d[srcreg];
{	LONG data = regs.d[dstreg];
{	ULONG val = data;
	ULONG cmask = 0x80000000;
	int carry = 0;
	cnt &= 63;
	if (!cnt) { CFLG = 0; } else {	for(;cnt;--cnt){
	carry=val&cmask; val <<= 1;
	if(carry)  val |= 1;
	}
	CFLG = carry!=0;
}
	NFLG = (val & cmask) != 0; ZFLG = val == 0; VFLG = 0;
	regs.d[dstreg] = (val);
}}}}}
void op_e1d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
	VFLG = 0;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	CFLG=regs.x=(val&cmask)!=0; val <<= 1;
	if ((val&cmask)!=sign) VFLG=1;
	NFLG = (val&cmask) != 0;
	ZFLG = val == 0;
	put_word(dataa,val);
}}}}
void op_e1d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	regs.a[srcreg] += 2;
	VFLG = 0;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	CFLG=regs.x=(val&cmask)!=0; val <<= 1;
	if ((val&cmask)!=sign) VFLG=1;
	NFLG = (val&cmask) != 0;
	ZFLG = val == 0;
	put_word(dataa,val);
}}}}}
void op_e1e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 2;
{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
	VFLG = 0;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	CFLG=regs.x=(val&cmask)!=0; val <<= 1;
	if ((val&cmask)!=sign) VFLG=1;
	NFLG = (val&cmask) != 0;
	ZFLG = val == 0;
	put_word(dataa,val);
}}}}}
void op_e1e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg] + (LONG)(WORD)nextiword();
	WORD data = get_word(dataa);
	VFLG = 0;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	CFLG=regs.x=(val&cmask)!=0; val <<= 1;
	if ((val&cmask)!=sign) VFLG=1;
	NFLG = (val&cmask) != 0;
	ZFLG = val == 0;
	put_word(dataa,val);
}}}}
void op_e1f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	dataa = get_disp_ea (dataa, nextiword());
{	WORD data = get_word(dataa);
	VFLG = 0;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	CFLG=regs.x=(val&cmask)!=0; val <<= 1;
	if ((val&cmask)!=sign) VFLG=1;
	NFLG = (val&cmask) != 0;
	ZFLG = val == 0;
	put_word(dataa,val);
}}}}}
void op_e1f8(ULONG opcode)
{
{{	CPTR dataa = (LONG)(WORD)nextiword();
	WORD data = get_word(dataa);
	VFLG = 0;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	CFLG=regs.x=(val&cmask)!=0; val <<= 1;
	if ((val&cmask)!=sign) VFLG=1;
	NFLG = (val&cmask) != 0;
	ZFLG = val == 0;
	put_word(dataa,val);
}}}}
void op_e1f9(ULONG opcode)
{
{{	CPTR dataa = nextilong();
	WORD data = get_word(dataa);
	VFLG = 0;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	ULONG sign = cmask & val;
	CFLG=regs.x=(val&cmask)!=0; val <<= 1;
	if ((val&cmask)!=sign) VFLG=1;
	NFLG = (val&cmask) != 0;
	ZFLG = val == 0;
	put_word(dataa,val);
}}}}
void op_e2d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	carry=val&1; val >>= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = regs.x = carry!=0;
	put_word(dataa,val);
}}}}
void op_e2d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	regs.a[srcreg] += 2;
{	UWORD val = data;
	int carry = val&1;
	carry=val&1; val >>= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = regs.x = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e2e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 2;
{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	carry=val&1; val >>= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = regs.x = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e2e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg] + (LONG)(WORD)nextiword();
	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	carry=val&1; val >>= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = regs.x = carry!=0;
	put_word(dataa,val);
}}}}
void op_e2f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	dataa = get_disp_ea (dataa, nextiword());
{	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	carry=val&1; val >>= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = regs.x = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e2f8(ULONG opcode)
{
{{	CPTR dataa = (LONG)(WORD)nextiword();
	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	carry=val&1; val >>= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = regs.x = carry!=0;
	put_word(dataa,val);
}}}}
void op_e2f9(ULONG opcode)
{
{{	CPTR dataa = nextilong();
	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	carry=val&1; val >>= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = regs.x = carry!=0;
	put_word(dataa,val);
}}}}
void op_e3d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = regs.x = carry!=0;
	put_word(dataa,val);
}}}}
void op_e3d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	regs.a[srcreg] += 2;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = regs.x = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e3e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 2;
{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = regs.x = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e3e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg] + (LONG)(WORD)nextiword();
	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = regs.x = carry!=0;
	put_word(dataa,val);
}}}}
void op_e3f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	dataa = get_disp_ea (dataa, nextiword());
{	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = regs.x = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e3f8(ULONG opcode)
{
{{	CPTR dataa = (LONG)(WORD)nextiword();
	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = regs.x = carry!=0;
	put_word(dataa,val);
}}}}
void op_e3f9(ULONG opcode)
{
{{	CPTR dataa = nextilong();
	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = regs.x = carry!=0;
	put_word(dataa,val);
}}}}
void op_e4d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	ULONG cmask = 0x8000;
	val >>= 1;
	if(regs.x) val |= cmask;
	regs.x = carry != 0;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
regs.x = CFLG = carry!=0;
	put_word(dataa,val);
}}}}
void op_e4d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	regs.a[srcreg] += 2;
{	UWORD val = data;
	int carry = val&1;
	ULONG cmask = 0x8000;
	val >>= 1;
	if(regs.x) val |= cmask;
	regs.x = carry != 0;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
regs.x = CFLG = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e4e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 2;
{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	ULONG cmask = 0x8000;
	val >>= 1;
	if(regs.x) val |= cmask;
	regs.x = carry != 0;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
regs.x = CFLG = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e4e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg] + (LONG)(WORD)nextiword();
	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	ULONG cmask = 0x8000;
	val >>= 1;
	if(regs.x) val |= cmask;
	regs.x = carry != 0;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
regs.x = CFLG = carry!=0;
	put_word(dataa,val);
}}}}
void op_e4f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	dataa = get_disp_ea (dataa, nextiword());
{	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	ULONG cmask = 0x8000;
	val >>= 1;
	if(regs.x) val |= cmask;
	regs.x = carry != 0;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
regs.x = CFLG = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e4f8(ULONG opcode)
{
{{	CPTR dataa = (LONG)(WORD)nextiword();
	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	ULONG cmask = 0x8000;
	val >>= 1;
	if(regs.x) val |= cmask;
	regs.x = carry != 0;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
regs.x = CFLG = carry!=0;
	put_word(dataa,val);
}}}}
void op_e4f9(ULONG opcode)
{
{{	CPTR dataa = nextilong();
	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	ULONG cmask = 0x8000;
	val >>= 1;
	if(regs.x) val |= cmask;
	regs.x = carry != 0;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
regs.x = CFLG = carry!=0;
	put_word(dataa,val);
}}}}
void op_e5d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	if(regs.x) val |= 1;
	regs.x = carry != 0;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
regs.x = CFLG = carry!=0;
	put_word(dataa,val);
}}}}
void op_e5d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	regs.a[srcreg] += 2;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	if(regs.x) val |= 1;
	regs.x = carry != 0;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
regs.x = CFLG = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e5e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 2;
{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	if(regs.x) val |= 1;
	regs.x = carry != 0;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
regs.x = CFLG = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e5e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg] + (LONG)(WORD)nextiword();
	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	if(regs.x) val |= 1;
	regs.x = carry != 0;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
regs.x = CFLG = carry!=0;
	put_word(dataa,val);
}}}}
void op_e5f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	dataa = get_disp_ea (dataa, nextiword());
{	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	if(regs.x) val |= 1;
	regs.x = carry != 0;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
regs.x = CFLG = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e5f8(ULONG opcode)
{
{{	CPTR dataa = (LONG)(WORD)nextiword();
	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	if(regs.x) val |= 1;
	regs.x = carry != 0;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
regs.x = CFLG = carry!=0;
	put_word(dataa,val);
}}}}
void op_e5f9(ULONG opcode)
{
{{	CPTR dataa = nextilong();
	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	if(regs.x) val |= 1;
	regs.x = carry != 0;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
regs.x = CFLG = carry!=0;
	put_word(dataa,val);
}}}}
void op_e6d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	ULONG cmask = 0x8000;
	val >>= 1;
	if(carry) val |= cmask;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = carry!=0;
	put_word(dataa,val);
}}}}
void op_e6d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	regs.a[srcreg] += 2;
{	UWORD val = data;
	int carry = val&1;
	ULONG cmask = 0x8000;
	val >>= 1;
	if(carry) val |= cmask;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e6e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 2;
{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	ULONG cmask = 0x8000;
	val >>= 1;
	if(carry) val |= cmask;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e6e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg] + (LONG)(WORD)nextiword();
	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	ULONG cmask = 0x8000;
	val >>= 1;
	if(carry) val |= cmask;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = carry!=0;
	put_word(dataa,val);
}}}}
void op_e6f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	dataa = get_disp_ea (dataa, nextiword());
{	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	ULONG cmask = 0x8000;
	val >>= 1;
	if(carry) val |= cmask;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e6f8(ULONG opcode)
{
{{	CPTR dataa = (LONG)(WORD)nextiword();
	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	ULONG cmask = 0x8000;
	val >>= 1;
	if(carry) val |= cmask;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = carry!=0;
	put_word(dataa,val);
}}}}
void op_e6f9(ULONG opcode)
{
{{	CPTR dataa = nextilong();
	WORD data = get_word(dataa);
{	UWORD val = data;
	int carry = val&1;
	ULONG cmask = 0x8000;
	val >>= 1;
	if(carry) val |= cmask;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = carry!=0;
	put_word(dataa,val);
}}}}
void op_e7d0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	if(carry)  val |= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = carry!=0;
	put_word(dataa,val);
}}}}
void op_e7d8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	regs.a[srcreg] += 2;
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	if(carry)  val |= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e7e0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	regs.a[srcreg] -= 2;
{	CPTR dataa = regs.a[srcreg];
	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	if(carry)  val |= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e7e8(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg] + (LONG)(WORD)nextiword();
	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	if(carry)  val |= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = carry!=0;
	put_word(dataa,val);
}}}}
void op_e7f0(ULONG opcode)
{
	ULONG srcreg = (LONG)(BYTE)((opcode & 7) >> 0);
{{	CPTR dataa = regs.a[srcreg];
	dataa = get_disp_ea (dataa, nextiword());
{	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	if(carry)  val |= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = carry!=0;
	put_word(dataa,val);
}}}}}
void op_e7f8(ULONG opcode)
{
{{	CPTR dataa = (LONG)(WORD)nextiword();
	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	if(carry)  val |= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = carry!=0;
	put_word(dataa,val);
}}}}
void op_e7f9(ULONG opcode)
{
{{	CPTR dataa = nextilong();
	WORD data = get_word(dataa);
{	UWORD val = data;
	ULONG cmask = 0x8000;
	int carry = val&cmask;
	val <<= 1;
	if(carry)  val |= 1;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(val)) == 0;
	NFLG = ((WORD)(val)) < 0;
CFLG = carry!=0;
	put_word(dataa,val);
}}}}
