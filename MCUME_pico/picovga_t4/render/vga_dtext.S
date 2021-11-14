
// ****************************************************************************
//
//                              VGA render GF_DTEXT
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

// extern "C" u8* RenderDText(u8* dbuf, int x, int y, int w, sSegm* segm)

// render 8-pixel double gradient color text GF_DTEXT
//  R0 ... destination data buffer
//  R1 ... start X coordinate (in pixels, must be multiple of 4)
//  R2 ... start Y coordinate (in graphics lines)
//  R3 ... width to display (must be multiple of 4 and > 0)
//  [stack] ... segm video segment sSegm
// Output new pointer to destination data buffer.
// 320 pixels takes  us on 151 MHz.

.thumb_func
.global RenderDText
RenderDText:

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

	// push registers
	push	{r1-r7,lr}
	mov	r4,r8
	push	{r4}

	// get pointer to video segment -> R4
	ldr	r4,[sp,#36]	// load video segment -> R4

	// start divide Y/font height
	ldr	r6,RenderDText_pSioBase // get address of SIO base -> R6
	str	r2,[r6,#SIO_DIV_UDIVIDEND_OFFSET] // store dividend, Y coordinate
	ldrb	r2,[r4,#SSEGM_PAR3+1] // font height -> R2
	str	r2,[r6,#SIO_DIV_UDIVISOR_OFFSET] // store divisor, font height

// - now we must wait at least 8 clock cycles to get result of division

	// [6] get wrap width -> [SP+36]
	ldrh	r5,[r4,#SSEGM_WRAPX] // [2] get wrap width
	movs	r7,#3		// [1] mask to align to 32-bit
	bics	r5,r7		// [1] align wrap
	str	r5,[sp,#36]	// [1] save wrap width

	// [1] align X coordinate to 32-bit
	bics	r1,r7		// [1]

	// [3] align remaining width
	bics	r3,r7		// [1]
	str	r3,[sp,#12]	// [2] save new width

	// load result of division Y/font_height -> R6 Y relative at row, R7 Y row
	//  Note: QUOTIENT must be read last
	ldr	r5,[r6,#SIO_DIV_REMAINDER_OFFSET] // get remainder of result -> R5, Y coordinate relative to current row
	ldr	r2,[r6,#SIO_DIV_QUOTIENT_OFFSET] // get quotient-> R2, index of row

	// pointer to font line -> R3
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
	lsrs	r6,r1,#3	// convert X to gradient index
	lsls	r6,#2		// round to 4-bytes
	add	r3,r6		// pointer to source gradient array
	lsrs	r6,r1,#4	// convert X to character index (1 character is 16 pixels width)
	add	r2,r6		// pointer to source text buffer -> R2

	// prepare background color, expand to 32 bits -> R4
	ldrb	r4,[r4,#SSEGM_PAR3] // load background color
	lsls	r5,r4,#8	// shift background color << 8
	orrs	r5,r4		// color expanded to 16 bits
	lsls	r4,r5,#16	// shift 16-bit color << 16
	orrs	r4,r5		// color expanded to 32 bits

	// prepare pointer to conversion table -> LR
	ldr	r5,RenderDText_Addr // get pointer to conversion table -> R5
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

	// check if X is aligned
	lsls	r6,r1,#(32-4)	// check if X is aligned
	beq	2f		// X not aligned

	// shift X coordinate
	lsrs	r5,r6,#(32-4)	// [1] X pixel offset in last character -> R5
	movs	r6,16		// character width
	subs	r6,r5		// pixels remain
	adds	r1,r6		// shift X coordinate (align to next character)
	ldr	r7,[sp,#12]
	subs	r7,r6		// shift width
	str	r7,[sp,#12]

	push	{r1}

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

	// [4] convert first 4 pixels (higher 4 bits) -> R1
	ldr	r1,[r7,#0]	// [2] load mask for higher 4 bits
	ands	r1,r6		// [1] mask foreground color
	eors	r1,r4		// [1] combine with background color

	cmp	r5,#4		// check start position of X
	bhi	3f		// > 4

	// [20] store 8 pixels
	lsrs	r1,#16		// [1]
	strb	r1,[r0,#0]	// [2]
	strb	r1,[r0,#1]	// [2]
	lsrs	r1,#8		// [1]
	strb	r1,[r0,#2]	// [2]
	strb	r1,[r0,#3]	// [2]
	adds	r0,#4		// [1]

	// [3] load foreground color, XOR with background -> R6
3:	ldmia	r3!,{r6}	// [2] load foreground color from gradient buffer
	eors	r6,r4		// [1] XOR foreground color with background color

	// [4] convert second 4 pixels (lower 4 bits)
	ldr	r1,[r7,#4]	// [2] load mask for lower 4 bits
	ands	r1,r6		// [1] mask foreground color
	eors	r1,r4		// [1] combine with background color

	// store 8 pixels
	cmp	r5,#8		// check start position of X
	bhi	4f		// > 8

	strb	r1,[r0,#0]	// [2]
	strb	r1,[r0,#1]	// [2]
	lsrs	r1,#8		// [1]
	strb	r1,[r0,#2]	// [2]
	strb	r1,[r0,#3]	// [2]
	lsls	r1,#8
	adds	r0,#4

4:	lsrs	r1,#16		// [1]
	strb	r1,[r0,#0]	// [2]
	strb	r1,[r0,#1]	// [2]
	lsrs	r1,#8		// [1]
	strb	r1,[r0,#2]	// [2]
	strb	r1,[r0,#3]	// [2]
	adds	r0,#4		// [1]

	pop	{r1}

	// check end of segment
	ldr	r7,[sp,#36]	// load wrap width
	cmp	r1,r7		// end of segment?
	blo	2f
	movs	r1,#0		// reset X coordinate
	ldr	r2,[sp,#8]	// get base pointer to text data -> R2
	ldr	r3,[sp,#4]	// get base pointer to gradient array -> R3

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

RenderDText_OutLoop:

	// limit wrap width by total width -> R7
	ldr	r6,[sp,#12]	// get remaining width
	cmp	r7,r6		// compare with wrap width
	bls	2f		// width is OK
	mov	r7,r6		// limit wrap width

	// check if remain whole characters
2:	cmp	r7,#16		// check number of remaining pixels
	bhs	5f		// enough characters remain

	// check if 1st part of last character remains
	cmp	r7,#4		// check 1st part of last character
	blo	3f		// all done
	mov	r1,r7		// width to render

// ---- render 1st part of last character

RenderDText_Last:

	push	{r7}

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

	// [20] store 8 pixels
	strb	r5,[r0,#0]	// [2]
	strb	r5,[r0,#1]	// [2]
	lsrs	r5,#8		// [1]
	strb	r5,[r0,#2]	// [2]
	strb	r5,[r0,#3]	// [2]
	adds	r0,#4		// [1]

	cmp	r1,#4
	bls	4f

	lsrs	r5,#8		// [1]
	strb	r5,[r0,#0]	// [2]
	strb	r5,[r0,#1]	// [2]
	lsrs	r5,#8		// [1]
	strb	r5,[r0,#2]	// [2]
	strb	r5,[r0,#3]	// [2]
	adds	r0,#4		// [1]

	cmp	r1,#8
	bls	4f

	// [3] load foreground color, XOR with background -> R6
	ldmia	r3!,{r6}	// [2] load foreground color from gradient buffer
	eors	r6,r4		// [1] XOR foreground color with background color

	// [4] convert second 4 pixels (lower 4 bits)
	ldr	r7,[r7,#4]	// [2] load mask for lower 4 bits
	ands	r7,r6		// [1] mask foreground color
	eors	r7,r4		// [1] combine with background color

	// [20] store 8 pixels
	strb	r7,[r0,#0]	// [2]
	strb	r7,[r0,#1]	// [2]
	lsrs	r7,#8		// [1]
	strb	r7,[r0,#2]	// [2]
	strb	r7,[r0,#3]	// [2]
	adds	r0,#4		// [1]

	// check if continue with next segment
4:	pop	{r7}

	ldr	r2,[sp,#8]	// get base pointer to text data -> R2
	ldr	r3,[sp,#4]	// get base pointer to gradient array -> R3
	cmp	r7,#16
	bhs	RenderDText_OutLoop

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
	subs	r1,#3		// number of characters*2 - 3

// ---- [65*N-1] start inner loop, render characters in one part of segment
// Inner loop variables (* prepared before inner loop):
//  R0 ... *pointer to destination data buffer
//  R1 ... *number of characters to generate*2 - 3 (loop counter)
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

RenderDText_InLoop:

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

	// [20] store 8 pixels
	strb	r5,[r0,#0]	// [2]
	strb	r5,[r0,#1]	// [2]
	lsrs	r5,#8		// [1]
	strb	r5,[r0,#2]	// [2]
	strb	r5,[r0,#3]	// [2]
	lsrs	r5,#8		// [1]
	strb	r5,[r0,#4]	// [2]
	strb	r5,[r0,#5]	// [2]
	lsrs	r5,#8		// [1]
	strb	r5,[r0,#6]	// [2]
	strb	r5,[r0,#7]	// [2]
	adds	r0,#8		// [1]

	// [3] load foreground color, XOR with background -> R6
	ldmia	r3!,{r6}	// [2] load foreground color from gradient buffer
	eors	r6,r4		// [1] XOR foreground color with background color

	// [4] convert second 4 pixels (lower 4 bits)
	ldr	r7,[r7,#4]	// [2] load mask for lower 4 bits
	ands	r7,r6		// [1] mask foreground color
	eors	r7,r4		// [1] combine with background color

	// [20] store 8 pixels
	strb	r7,[r0,#0]	// [2]
	strb	r7,[r0,#1]	// [2]
	lsrs	r7,#8		// [1]
	strb	r7,[r0,#2]	// [2]
	strb	r7,[r0,#3]	// [2]
	lsrs	r7,#8		// [1]
	strb	r7,[r0,#4]	// [2]
	strb	r7,[r0,#5]	// [2]
	lsrs	r7,#8		// [1]
	strb	r7,[r0,#6]	// [2]
	strb	r7,[r0,#7]	// [2]
	adds	r0,#8		// [1]

	// [2,3] loop counter
	subs	r1,#4		// [1] shift loop counter
	bhi	RenderDText_InLoop // [1,2] > 0, render next whole character

// ---- end inner loop, continue with last character, or start new part

	// continue to outer loop
	ldr	r7,[sp,#36]	// load wrap width
	adds	r1,#3		// return size of last tile
	lsls	r1,#2		// convert back to pixels
	bne	RenderDText_Last // render 1st half of last character
	ldr	r2,[sp,#8]	// get base pointer to text data -> R2
	ldr	r3,[sp,#4]	// get base pointer to gradient array -> R3
	b	RenderDText_OutLoop // go back to outer loop

	.align 2
RenderDText_Addr:
	.word	RenderTextMask
RenderDText_pSioBase:
	.word	SIO_BASE	// addres of SIO base
