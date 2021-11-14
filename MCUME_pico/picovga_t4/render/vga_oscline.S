
// ****************************************************************************
//
//                              VGA render GF_OSCLINE
//
// ****************************************************************************

#include "../define.h"		// common definitions of C and ASM

	.syntax unified
	.section .time_critical.Render, "ax"
	.cpu cortex-m0plus
	.thumb			// use 16-bit instructions

// render font pixel mask
.extern	RenderTextMask		// u32 RenderTextMask[512];

// extern "C" u8* RenderOscLine(u8* dbuf, int x, int y, int w, sSegm* segm);

// render oscilloscope graph GF_OSCLINE
//   dbuf ... destination data buffer
//   x ... start X coordinate (must be multiple of 4)
//   y ... start Y coordinate 
//   w ... width of this segment (must be multiple of 4)
//   segm ... video segment
// Output new dbuf pointer.
// 320 pixels takes 21.5 us on 151 MHz.

.thumb_func
.global RenderOscLine
RenderOscLine:

	// push registers
	push	{r2-r7,lr}

// Input registers and stack content:
//  R0 ... pointer to testination data buffer
//  R1 ... start X coordinate
//  SP+0: R2 start Y coordinate (later: base pointer to sample data)
//  SP+4: R3 width to display
//  SP+8: R4
//  SP+12: R5
//  SP+16: R6
//  SP+20: R7
//  SP+24: LR
//  SP+28: video segment (later: wrap width in X direction)

	// get pointer to video segment -> R4
	ldr	r4,[sp,#28]	// load video segment -> R4

	// get wrap width/2 -> [SP+28]
	ldrh	r5,[r4,#SSEGM_WRAPX] // get wrap width
	lsrs	r5,#1		// wrap width / 2
	str	r5,[sp,#28]	// save wrap width

	// X coordinate/2 -> R1
	lsrs	r1,#1

	// remaining width/2 -> [SP+4]
	lsrs	r3,#1
	str	r3,[sp,#4]	// save new width

	// current Y in direction from bottom to up -> LR
	ldrh	r5,[r4,#SSEGM_WRAPY] // get wrap height
	subs	r5,#1		// wrapy - 1
	subs	r5,r2		// subtract Y, get Y relative to bottom -> R5
	mov	lr,r5

	// base pointer to sample data (without X) -> [SP+0], R2
	ldr	r2,[r4,#SSEGM_DATA] // pointer to sample data
	str	r2,[sp,#0]	// save pointer to sample buffer

	// prepare pointer to sample data with X -> R2
	add	r2,r1		// pointer to source sample buffer -> R2

	// prepare foreground color -> R6
	ldrb	r6,[r4,#SSEGM_PAR+1] // load foreground color
	lsls	r7,r6,#8
	orrs	r6,r7

	// prepare background color -> R4
	ldrb	r4,[r4,#SSEGM_PAR] // load background color
	lsls	r7,r4,#8
	orrs	r4,r7

	// prepare wrap width - start X -> R1
	ldr	r7,[sp,#28]	// load wrap width
	subs	r1,r7,r1	// pixels remaining to end of segment

// ---- start outer loop, render one part of segment
// Outer loop variables (* prepared before outer loop):
//  R0 ... *pointer to destination data buffer
//  R1 ... *wrap width of this segment, later: number of pixels to generate in one part of segment
//  R2 ... *pointer to source sample buffer
//  R3 ... remaining width, later: (temporary)
//  R4 ... *background color
//  R5 ... (temporary)
//  R6 ... *foreground color
//  R7 ... (temporary)
//  LR ... *current line Y (in direction from bottom to up)
//  [SP+0] ... *base pointer to sample data (without X)
//  [SP+4] ... *remaining width
//  [SP+28] ... *wrap width

RenderOscLine_OutLoop:

	// limit wrap width by total width -> R1
	ldr	r3,[sp,#4]	// get remaining width
	cmp	r1,r3		// compare with wrap width
	bls	2f		// width is OK
	mov	r1,r3		// limit wrap width

	// check number of pixels
2:	cmp	r1,#0		// check number of remaining pixels
	beq	RenderOscLine_Stop // stop
	subs	r3,r1		// get remaining width
	str	r3,[sp,#4]	// save new remaining width

// ---- start inner loop, render in one part of segment
// Inner loop variables (* prepared before inner loop):
//  R0 ... *pointer to destination data buffer
//  R1 ... *number of pixels to generate (loop counter)
//  R2 ... *pointer to source sample buffer
//  R3 ... sample
//  R4 ... *background color
//  R5 ... previous sample
//  R6 ... *foreground color
//  R7 ... current color
//  LR ... *current line Y (in direction from bottom to up)
//  [SP+0] ... *base pointer to sample data (without X)
//  [SP+4] ... *remaining width
//  [SP+28] ... *wrap width

	ldrb	r5,[r2,#0]	// [2] prepare previous sample -> R5

RenderOscLine_InLoop: // render 8 pixels in one loop step, top half of graph

	// [3] get sample
	ldrb	r3,[r2,#0]	// [2] get data sample -> R3
	adds	r2,#1		// [1] increment pointer

	// [1] preset to background color
	mov	r7,r4		// [1] preset to background color

	// [3..8] (sample > previous sample) AND (sample > line) AND (line > previous sample) - display pixel
	cmp	r3,lr		// [1] compare sample with line
	beq	4f		// [1,2] (sample == line), true, display pixel everytime
	blo	2f		// [1,2] (sample < line), false
	cmp	r3,r5		// [1] compare sample with previous sample
	bls	2f		// [1,2] (sample <= previous), false
	cmp	lr,r5		// [1] compare line with previous sample
	bhi	4f		// [1,2] (line > previous), true

	// [3..7] (sample < previous sample) AND (sample < line) AND (line < previous sample) - display pixel
2:	cmp	r3,r5		// [1] compare sample with previous sample
	bhs	6f		// [1,2] (sample >= previous), false
	cmp	r3,lr		// [1] compare sample with line
	bhs	6f		// [1,2] (sample >= line), false
	cmp	lr,r5		// [1] compare line with previous sample
	bhs	6f		// [1,2] (line >= previous), false

	// [1] use foreground color
4:	mov	r7,r6		// [1] use foreground color

	// [3] write 2 pixels
6:	strh	r7,[r0,#0]	// [2] write pixel
	adds	r0,#2		// [1] increment pointer

	// [1] save previous sample
	mov	r5,r3		// [1]

	// [2,3] loop counter
	subs	r1,#1		// [1] shift loop counter
	bne	RenderOscLine_InLoop // [1,2] render next pixel

// ---- end inner loop, start new part

	// continue to outer loop
	ldr	r1,[sp,#28]	// load wrap width
	ldr	r2,[sp,#0]	// get base pointer to sample data -> R2
	b	RenderOscLine_OutLoop // go back to outer loop

RenderOscLine_Stop:

	// pop registers and return
	pop	{r2-r7,pc}

	.align 2
RenderOscLine_Addr:
	.word	RenderTextMask
