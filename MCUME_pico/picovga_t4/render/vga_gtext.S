
// ****************************************************************************
//
//                              VGA render GF_GTEXT
//
// ****************************************************************************
// u32 par SSEGM_PAR pointer to the font
// u32 par2 SSEGM_PAR2 pointer to font gradient
// u16 par3 LOW background color, HIGH font height

#include "../define.h"		// common definitions of C and ASM
#include "hardware/regs/sio.h"	// registers of hardware divider
#include "hardware/regs/addressmap.h" // SIO base address

	.syntax unified
	.section .time_critical.Render, "ax"
	.cpu cortex-m0plus
	.thumb			// use 16-bit instructions

// render font pixel mask
.extern	RenderTextMask		// u32 RenderTextMask[512];

// extern "C" u8* RenderGText(u8* dbuf, int x, int y, int w, sSegm* segm)

// render 8-pixel gradient color text GF_GTEXT
//  R0 ... destination data buffer
//  R1 ... start X coordinate (in pixels, must be multiple of 4)
//  R2 ... start Y coordinate (in graphics lines)
//  R3 ... width to display (must be multiple of 4 and > 0)
//  [stack] ... segm video segment sSegm
// Output new pointer to destination data buffer.
// 320 pixels takes 8.3 us on 151 MHz.

.thumb_func
.global RenderGText
RenderGText:

	// push registers
	push	{r1-r7,lr}
	mov	r4,r8
	push	{r4}

// Stack content:
//  SP+0: R8
//  SP+4: R1 start X coordinate (later: base pointer to gradient array)
//  SP+8: R2 start Y coordinate (later: base pointer to text data row)
//  SP+12: R3 width to display
//  SP+16: R4
//  SP+20: R5
//  SP+24: R6
//  SP+28: R7
//  SP+32: LR
//  SP+36: video segment (later: wrap width in X direction)

	// get pointer to video segment -> R4
	ldr	r4,[sp,#36]	// load video segment -> R4

	// start divide Y/font height
	ldr	r6,RenderGText_pSioBase // get address of SIO base -> R6
	str	r2,[r6,#SIO_DIV_UDIVIDEND_OFFSET] // store dividend, Y coordinate
	ldrh	r2,[r4,#SSEGM_PAR3] // font height -> R2
	str	r2,[r6,#SIO_DIV_UDIVISOR_OFFSET] // store divisor, font height

// - now we must wait at least 8 clock cycles to get result of division

	// [6] get wrap width -> [SP+36]
	ldrh	r5,[r4,#SSEGM_WRAPX] // [2] get wrap width
	movs	r7,#3		// [1] mask to align to 32-bit
	bics	r5,r7		// [1] align wrap
	str	r5,[sp,#36]	// [2] save wrap width

	// [1] align X coordinate to 32-bit
	bics	r1,r7		// [1]

	// [3] align remaining width
	bics	r3,r7		// [1]
	str	r3,[sp,#12]	// [2] save new width

	// load result of division Y/font_height -> R6 Y relative at row, R7 Y row
	//  Note: QUOTIENT must be read last
	ldr	r5,[r6,#SIO_DIV_REMAINDER_OFFSET] // get remainder of result -> R5, Y coordinate relative to current row
	ldr	r2,[r6,#SIO_DIV_QUOTIENT_OFFSET] // get quotient-> R2, index of row

	// pointer to font line -> R8
	lsls	r5,#8		// multiply Y relative * 256 (1 font line is 256 bytes long)
	ldr	r3,[r4,#SSEGM_PAR] // get pointer to font
	add	r3,r5		// line offset + font base -> pointer to current font line R3
	mov	r8,r3

	// base pointer to text data (without X) -> [SP+8], R2
	ldrh	r5,[r4,#SSEGM_WB] // get pitch of rows
	muls	r2,r5		// Y * WB -> offset of row in text buffer
	ldr	r5,[r4,#SSEGM_DATA] // pointer to data
	add	r2,r5		// base address of text buffer
	str	r2,[sp,#8]	// save pointer to text buffer

	// base pointer to gradient array -> [SP+4], R3
	ldr	r3,[r4,#SSEGM_PAR2] // pointer to graient array
	str	r3,[sp,#4]	// save pointer to gradient array

	// prepare pointer to text data with X -> R2
	add	r3,r1		// pointer to source gradient array
	lsrs	r6,r1,#3	// convert X to character index (1 character is 8 pixels width)
	add	r2,r6		// pointer to source text buffer -> R2

	// prepare background color, expand to 32 bits -> R4
	ldrb	r4,[r4,#SSEGM_PAR3] // load background color
	lsls	r5,r4,#8	// shift background color << 8
	orrs	r5,r4		// color expanded to 16 bits
	lsls	r4,r5,#16	// shift 16-bit color << 16
	orrs	r4,r5		// color expanded to 32 bits

	// prepare pointer to conversion table -> LR
	ldr	r5,RenderGText_Addr // get pointer to conversion table -> R5
	mov	lr,r5		// conversion table -> LR

// ---- render 2nd half of first character
//  R0 ... pointer to destination data buffer
//  R1 ... start X coordinate
//  R2 ... pointer to source text buffer
//  R3 ... pointer to gradient array
//  R4 ... background color (expanded to 32-bit)
//  R5..R7 ... (temporary)
//  R8 ... pointer to font line
//  LR ... pointer to conversion table
//  [SP+4] ... base pointer to gradient array
//  [SP+8] ... base pointer to text data (without X)
//  [SP+12] ... remaining width
//  [SP+36] ... wrap width

	// check bit 2 of X coordinate - check if image starts with 2nd half of first character
	lsls	r6,r1,#29	// check bit 2 of X coordinate
	bpl	2f		// bit 2 not set, starting even 4-pixels

	// [6] load font sample -> R5
	ldrb	r5,[r2,#0]	// [2] load character from source text buffer -> R5
	adds	r2,#1		// [1] shift pointer to source text buffer
	add	r5,r8		// [1] pointer to font line
	ldrb	r5,[r5]		// [2] load font sample -> R5

	// [3] load foreground color, XOR with background -> R6
	ldmia	r3!,{r6}	// [2] load foreground color from gradient buffer
	eors	r6,r4		// [1] XOR foreground color with background color

	// [2] prepare conversion table -> R5
	lsls	r5,#3		// [1] multiply font sample * 8
	add	r5,lr		// [1] add pointer to conversion table

	// [6] convert second 4 pixels (lower 4 bits)
	ldr	r7,[r5,#4]	// [2] load mask for lower 4 bits
	ands	r7,r6		// [1] mask foreground color
	eors	r7,r4		// [1] combine with background color
	stmia	r0!,{r7}	// [2] store second 4 pixels

	// shift X coordinate
	adds	r1,#4		// shift X coordinate

	// check end of segment
	ldr	r7,[sp,#36]	// load wrap width
	cmp	r1,r7		// end of segment?
	blo	1f
	movs	r1,#0		// reset X coordinate
	ldr	r2,[sp,#8]	// get base pointer to text data -> R2
	ldr	r3,[sp,#4]	// get base pointer to gradient array -> R3

	// shift remaining width
1:	ldr	r7,[sp,#12]	// get remaining width
	subs	r7,#4		// shift width
	str	r7,[sp,#12]	// save new width

	// prepare wrap width - start X -> R7
2:	ldr	r7,[sp,#36]	// load wrap width
	subs	r7,r1		// pixels remaining to end of segment

// ---- start outer loop, render one part of segment
// Outer loop variables (* prepared before outer loop):
//  R0 ... *pointer to destination data buffer
//  R1 ... number of characters to generate in one part of segment
//  R2 ... *pointer to source text buffer
//  R3 ... *pointer to gradient array
//  R4 ... *background color (expanded to 32-bit)
//  R5 ... (temporary)
//  R6 ... (temporary)
//  R7 ... *wrap width of this segment, later: temporary
//  R8 ... *pointer to font line
//  LR ... *pointer to conversion table
//  [SP+4] ... base pointer to gradient array
//  [SP+8] ... base pointer to text data (without X)
//  [SP+12] ... remaining width
//  [SP+36] ... wrap width

RenderGText_OutLoop:

	// limit wrap width by total width -> R7
	ldr	r6,[sp,#12]	// get remaining width
	cmp	r7,r6		// compare with wrap width
	bls	2f		// width is OK
	mov	r7,r6		// limit wrap width

	// check if remain whole characters
2:	cmp	r7,#8		// check number of remaining pixels
	bhs	5f		// enough characters remain

	// check if 1st part of last character remains
	cmp	r7,#4		// check 1st part of last character
	blo	3f		// all done

// ---- render 1st part of last character

RenderGText_Last:

	// [6] load font sample -> R5
	ldrb	r5,[r2,#0]	// [2] load character from source text buffer -> R5
	adds	r2,#1		// [1] shift pointer to source text buffer
	add	r5,r8		// [1] pointer to font line
	ldrb	r5,[r5]		// [2] load font sample -> R5

	// [3] load foreground color, XOR with background -> R6
	ldmia	r3!,{r6}	// [2] load foreground color from gradient buffer
	eors	r6,r4		// [1] XOR foreground color with background color

	// [2] prepare conversion table -> R5
	lsls	r5,#3		// [1] multiply font sample * 8
	add	r5,lr		// [1] add pointer to conversion table

	// [6] convert first 4 pixels (higher 4 bits)
	ldr	r1,[r5,#0]	// [2] load mask for higher 4 bits
	ands	r1,r6		// [1] mask foreground color
	eors	r1,r4		// [1] combine with background color
	stmia	r0!,{r1}	// [2] store first 4 pixels

	// check if continue with next segment
	ldr	r2,[sp,#8]	// get base pointer to text data -> R2
	ldr	r3,[sp,#4]	// get base pointer to gradient array -> R3
	cmp	r7,#4
	bhi	RenderGText_OutLoop

	// pop registers and return
3:	pop	{r4}
	mov	r8,r4
	pop	{r1-r7,pc}

// ---- prepare to render whole characters

	// prepare number of whole characters to render -> R1
5:	lsrs	r1,r7,#2	// shift to get number of characters*2
	lsls	r5,r1,#2	// shift back to get number of pixels, rounded down -> R5
	subs	r6,r5		// get remaining width
	str	r6,[sp,#12]	// save new remaining width
	subs	r1,#1		// number of characters*2 - 1

// ---- [28*N-1] start inner loop, render characters in one part of segment
// Inner loop variables (* prepared before inner loop):
//  R0 ... *pointer to destination data buffer
//  R1 ... *number of characters to generate*2 - 1 (loop counter)
//  R2 ... *pointer to source text buffer
//  R3 ... *pointer to gradient array
//  R4 ... *background color (expanded to 32-bit)
//  R5 ... (temporary)
//  R6 ... foreground color
//  R7 ... font sample
//  R8 ... *pointer to font line
//  LR ... *pointer to conversion table
//  [SP+4] ... base pointer to gradient array
//  [SP+8] ... base pointer to text data (without X)
//  [SP+12] ... remaining width
//  [SP+36] ... wrap width

RenderGText_InLoop:

	// [6] load font sample -> R7
	ldrb	r7,[r2,#0]	// [2] load character from source text buffer -> R7
	adds	r2,#1		// [1] shift pointer to source text buffer
	add	r7,r8		// [1] pointer to font line
	ldrb	r7,[r7]		// [2] load font sample -> R7

	// [3] load foreground color, XOR with background -> R6
	ldmia	r3!,{r6}	// [2] load foreground color from gradient buffer
	eors	r6,r4		// [1] XOR foreground color with background color

	// [2] prepare conversion table -> R7
	lsls	r7,#3		// [1] multiply font sample * 8
	add	r7,lr		// [1] add pointer to conversion table

	// [4] convert first 4 pixels (higher 4 bits) -> R5
	ldr	r5,[r7,#0]	// [2] load mask for higher 4 bits
	ands	r5,r6		// [1] mask foreground color
	eors	r5,r4		// [1] combine with background color

	// [3] load foreground color, XOR with background -> R6
	ldmia	r3!,{r6}	// [2] load foreground color from gradient buffer
	eors	r6,r4		// [1] XOR foreground color with background color

	// [7] convert and store second 4 pixels (lower 4 bits)
	ldr	r7,[r7,#4]	// [2] load mask for lower 4 bits
	ands	r7,r6		// [1] mask foreground color
	eors	r7,r4		// [1] combine with background color
	stmia	r0!,{r5,r7}	// [3] store 8 pixels

	// [2,3] loop counter
	subs	r1,#2		// [1] shift loop counter
	bhi	RenderGText_InLoop // [1,2] > 0, render next whole character

// ---- end inner loop, continue with last character, or start new part

	// continue to outer loop
	ldr	r7,[sp,#36]	// load wrap width
	beq	RenderGText_Last // render 1st half of last character
	ldr	r2,[sp,#8]	// get base pointer to text data -> R2
	ldr	r3,[sp,#4]	// get base pointer to gradient array -> R3
	b	RenderGText_OutLoop // go back to outer loop

	.align 2
RenderGText_Addr:
	.word	RenderTextMask
RenderGText_pSioBase:
	.word	SIO_BASE	// addres of SIO base
