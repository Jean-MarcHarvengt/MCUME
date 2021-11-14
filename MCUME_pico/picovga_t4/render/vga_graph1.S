
// ****************************************************************************
//
//                              VGA render GF_GRAPH1
//
// ****************************************************************************

#include "../define.h"		// common definitions of C and ASM

	.syntax unified
	.section .time_critical.Render, "ax"
	.cpu cortex-m0plus
	.thumb			// use 16-bit instructions

// render font pixel mask
.extern	RenderTextMask		// u32 RenderTextMask[512];

// extern "C" u8* RenderGraph1(u8* dbuf, int x, int y, int w, sSegm* segm);

// render 1-bit palette graphics GF_GRAPH1
//   dbuf ... destination data buffer
//   x ... start X coordinate (must be multiple of 4)
//   y ... start Y coordinate 
//   w ... width of this segment (must be multiple of 4)
//   segm ... video segment
// Output new dbuf pointer.
// 320 pixels takes 6 us on 151 MHz.

.thumb_func
.global RenderGraph1
RenderGraph1:

	// push registers
	push	{r3-r7,lr}

// Input registers and stack content:
//  R0 ... destination data buffer
//  R1 ... start X coordinate
//  R2 ... start Y coordinate
//  SP+0: R3 width to display
//  SP+4: R4
//  SP+8: R5
//  SP+12: R6
//  SP+16: R7
//  SP+20: LR
//  SP+24: video segment (later: wrap width in X direction)

	// get pointer to video segment -> R4
	ldr	r4,[sp,#24]	// load video segment -> R4

	// get wrap width -> [SP+24]
	ldrh	r5,[r4,#SSEGM_WRAPX] // get wrap width
	movs	r7,#3		// mask to align to 32-bit
	bics	r5,r7		// align wrap
	str	r5,[sp,#24]	// save wrap width

	// align X coordinate to 32-bit -> R1
	bics	r1,r7

	// align remaining width -> [SP+0]
	bics	r3,r7
	str	r3,[sp,#0]	// save new width

	// base pointer to image data (without X) -> LR
	ldrh	r5,[r4,#SSEGM_WB] // get pitch of lines
	muls	r2,r5		// Y * WB -> offset of row in text buffer
	ldr	r5,[r4,#SSEGM_DATA] // pointer to data
	add	r2,r5		// base address of text buffer
	mov	lr,r2		// save pointer to text buffer

	// prepare pointer to image data with X -> R2
	lsrs	r2,r1,#3	// convert X to character index (1 character is 8 pixels width)
	add	r2,lr		// pointer to source text buffer -> R2

	// prepare foreground color, expand to 32-bit -> R6
	ldrb	r6,[r4,#SSEGM_PAR+1] // load foreground color
	lsls	r7,r6,#8	// [1] shift foreground color << 8
	orrs	r7,r6		// [1] color expanded to 16 bits
	lsls	r6,r7,#16	// [1] shift 16-bit color << 16
	orrs	r6,r7		// [1] color expanded to 32 bits

	// prepare background color, expand to 32 bits -> R4
	ldrb	r4,[r4,#SSEGM_PAR] // load background color
	lsls	r5,r4,#8	// shift background color << 8
	orrs	r5,r4		// color expanded to 16 bits
	lsls	r4,r5,#16	// shift 16-bit color << 16
	orrs	r4,r5		// color expanded to 32 bits

	// [1] XOR foreground and background color -> R6
	eors	r6,r4		// [1] XOR foreground color with background color

	// prepare pointer to conversion table -> R3
	ldr	r3,RenderGraph1_Addr // get pointer to conversion table -> R3

// ---- render 2nd half of first character
//  R0 ... pointer to destination data buffer
//  R1 ... start X coordinate
//  R2 ... current pointer to image buffer
//  R3 ... pointer to conversion table
//  R4 ... background color (expanded to 32-bit)
//  R5 ... (temporary)
//  R6 ... foreground color (expanded to 32-bit)
//  R7 ... (temporary)
//  LR ... base pointer to image data (without X)
//  [SP+0] ... remaining width
//  [SP+24] ... wrap width

	// check bit 2 of X coordinate - check if image starts with 2nd half of first character
	lsls	r5,r1,#29	// check bit 2 of X coordinate
	bpl	2f		// bit 2 not set, starting even 4-pixels

	// [3] load image sample -> R5
	ldrb	r5,[r2,#0]	// [2] load image sample -> R5
	adds	r2,#1		// [1] shift pointer to image buffer

	// [2] prepare conversion table -> R5
	lsls	r5,#3		// [1] multiply image sample * 8
	add	r5,r3		// [1] add pointer to conversion table

	// [6] convert second 4 pixels (lower 4 bits)
	ldr	r7,[r5,#4]	// [2] load mask for lower 4 bits
	ands	r7,r6		// [1] mask foreground color
	eors	r7,r4		// [1] combine with background color
	stmia	r0!,{r7}	// [2] store second 4 pixels

	// shift X coordinate
	adds	r1,#4		// shift X coordinate

	// check end of segment
	ldr	r7,[sp,#24]	// load wrap width
	cmp	r1,r7		// X=end of segment?
	blo	1f
	movs	r1,#0		// reset X coordinate
	mov	r2,lr		// get base pointer to image data -> R2

	// shift remaining width
1:	ldr	r7,[sp,#0]	// get remaining width
	subs	r7,#4		// shift width
	str	r7,[sp,#0]	// save new width

	// prepare wrap width - start X -> R7
2:	ldr	r7,[sp,#24]	// load wrap width
	subs	r7,r1		// pixels remaining to end of segment

// ---- start outer loop, render one part of segment
// Outer loop variables (* prepared before outer loop):
//  R0 ... *pointer to destination data buffer
//  R1 ... number of characters to generate in one part of segment
//  R2 ... *current pointer to image buffer
//  R3 ... *pointer to conversion table
//  R4 ... *background color (expanded to 32-bit)
//  R5 ... (temporary)
//  R6 ... *foreground color (expanded to 32-bit)
//  R7 ... *wrap width of this segment, later: temporary
//  LR ... *base pointer to image data (without X)
//  [SP+0] ... *remaining width
//  [SP+24] ... *wrap width

RenderGraph1_OutLoop:

	// limit wrap width by total width -> R7
	ldr	r5,[sp,#0]	// get remaining width
	cmp	r7,r5		// compare with wrap width
	bls	2f		// width is OK
	mov	r7,r5		// limit wrap width

	// check if remain whole characters
2:	cmp	r7,#8		// check number of remaining pixels
	bhs	5f		// enough characters remain

	// check if 1st part of last character remains
	cmp	r7,#4		// check 1st part of last character
	blo	3f		// all done

// ---- render 1st part of last character

RenderGraph1_Last:

	// [3] load image sample -> R5
	ldrb	r5,[r2,#0]	// [2] load image sample -> R5
	adds	r2,#1		// [1] shift pointer to image buffer

	// [2] prepare conversion table -> R5
	lsls	r5,#3		// [1] multiply image sample * 8
	add	r5,r3		// [1] add pointer to conversion table

	// [6] convert first 4 pixels (higher 4 bits)
	ldr	r1,[r5,#0]	// [2] load mask for higher 4 bits
	ands	r1,r6		// [1] mask foreground color
	eors	r1,r4		// [1] combine with background color
	stmia	r0!,{r1}	// [2] store first 4 pixels

	// check if continue with next segment
	mov	r2,lr		// get base pointer to image data -> R2
	cmp	r7,#4
	bhi	RenderGraph1_OutLoop

	// pop registers and return
3:	pop	{r3-r7,pc}

// ---- prepare to render whole characters

	// prepare number of whole characters to render -> R1
5:	lsrs	r1,r7,#2	// shift to get number of characters*2
	lsls	r7,r1,#2	// shift back to get number of pixels, rounded down -> R7
	subs	r5,r7		// get remaining width
	str	r5,[sp,#0]	// save new remaining width
	subs	r1,#1		// number of characters*2 - 1

// ---- [20*N-1] start inner loop, render characters in one part of segment
// Inner loop variables (* prepared before inner loop):
//  R0 ... *pointer to destination data buffer
//  R1 ... *number of characters to generate*2 - 1 (loop counter)
//  R2 ... *current pointer to image buffer
//  R3 ... *pointer to conversion table
//  R4 ... *background color (expanded to 32-bit)
//  R5 ... font sample
//  R6 ... *foreground color (expanded to 32-bit)
//  R7 ... (temporary)
//  LR ... *base pointer to image data (without X)

RenderGraph1_InLoop:

	// [3] load image sample -> R5
	ldrb	r5,[r2,#0]	// [2] load image sample -> R5
	adds	r2,#1		// [1] shift pointer to image buffer

	// [2] prepare conversion table -> R5
	lsls	r5,#3		// [1] multiply image sample * 8
	add	r5,r3		// [1] add pointer to conversion table

	// [6] convert first 4 pixels (higher 4 bits)
	ldr	r7,[r5,#0]	// [2] load mask for higher 4 bits
	ands	r7,r6		// [1] mask foreground color
	eors	r7,r4		// [1] combine with background color
	stmia	r0!,{r7}	// [2] store first 4 pixels

	// [6] convert second 4 pixels (lower 4 bits)
	ldr	r7,[r5,#4]	// [2] load mask for lower 4 bits
	ands	r7,r6		// [1] mask foreground color
	eors	r7,r4		// [1] combine with background color
	stmia	r0!,{r7}	// [2] store second 4 pixels

	// [2,3] loop counter
	subs	r1,#2		// [1] shift loop counter
	bhi	RenderGraph1_InLoop // [1,2] > 0, render next whole character

// ---- end inner loop, continue with last character, or start new part

	// continue to outer loop
	ldr	r7,[sp,#24]	// load wrap width
	beq	RenderGraph1_Last // render 1st half of last character
	mov	r2,lr		// get base pointer to image data -> R2
	b	RenderGraph1_OutLoop // go back to outer loop

	.align 2
RenderGraph1_Addr:
	.word	RenderTextMask
