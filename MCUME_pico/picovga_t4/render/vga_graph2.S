
// ****************************************************************************
//
//                              VGA render GF_GRAPH2
//
// ****************************************************************************

#include "../define.h"		// common definitions of C and ASM

	.syntax unified
	.section .time_critical.Render, "ax"
	.cpu cortex-m0plus
	.thumb			// use 16-bit instructions

// extern "C" u8* RenderGraph2(u8* dbuf, int x, int y, int w, sSegm* segm);

// render 2-bit palette graphics GF_GRAPH2
//  R0 ... destination data buffer
//  R1 ... start X coordinate (must be multiple of 4)
//  R2 ... start Y coordinate 
//  R3 ... width of this segment (must be multiple of 4)
//  segm ... video segment
// Output new dbuf pointer.
// 320 pixels takes 5 us on 151 MHz.

.thumb_func
.global RenderGraph2
RenderGraph2:

	// push registers
	push	{r3-r7,lr}

// Input registers and stack content:
//  R0 ... destination data buffer
//  R1 ... start X coordinate
//  R2 ... start Y coordinate
//  SP+0: R3 ... width to display (remaining width)
//  SP+4: R4
//  SP+8: R5
//  SP+12: R6
//  SP+16: R7
//  SP+20: LR
//  SP+24: video segment

	// get pointer to video segment -> R4
	ldr	r4,[sp,#24]	// load video segment -> R4

	// get wrap width -> R7
	ldrh	r7,[r4,#SSEGM_WRAPX] // get wrap width
	movs	r6,#3		// mask to align to 32-bit
	bics	r7,r6		// align wrap

	// align X coordinate to 32-bit -> R1
	bics	r1,r6

	// align remaining width -> [SP+0]
	bics	r3,r6
	str	r3,[sp,#0]	// save new width

	// base pointer to image data (without X) -> LR, R2
	ldrh	r5,[r4,#SSEGM_WB] // get pitch of rows
	muls	r2,r5		// Y * WB -> offset of row in image buffer
	ldr	r5,[r4,#SSEGM_DATA] // pointer to data
	add	r2,r5		// base address of image buffer
	mov	lr,r2		// save pointer to image buffer

	// prepare pointer to image data with X -> R2
	lsrs	r6,r1,#2	// convert X to character index (1 character is 4 pixels width)
	add	r2,r6		// add index, pointer to source image buffer -> R2

	// prepare pointer to palette translation table -> R3
	ldr	r3,[r4,#SSEGM_PAR] // get pointer to palette translation table -> R3

	// prepare wrap width - start X -> R6
	subs	r6,r7,r1	// pixels remaining to end of segment

// ---- start outer loop, render one part of segment
// Outer loop variables (* prepared before outer loop):
//  R0 ... *pointer to destination data buffer
//  R1 ... number of 4-pixels to generate in one part of segment
//  R2 ... *pointer to source image buffer
//  R3 ... *pointer to palette translation table
//  R4 ... (temporary)
//  R5 ... (temporary)
//  R6 ... part width
//  R7 ... *wrap width
//  LR ... *base pointer to image data (without X)
//  [SP+0] ... width to display

RenderGraph2_OutLoop:

	// limit wrap width by total width -> R7
	ldr	r4,[sp,#0]	// get remaining width
	cmp	r6,r4		// compare with wrap width
	bls	2f		// width is OK
	mov	r6,r4		// limit wrap width

	// check number of pixels
2:	cmp	r6,#4		// check number of remaining pixels
	bhs	5f		// enough characters remain

	// pop registers and return
	pop	{r3-r7,pc}

// ---- prepare to render whole characters

	// prepare number of 4-pixels to render -> R1
5:	lsrs	r1,r6,#2	// shift to get number of 4-pixels
	lsls	r6,r1,#2	// shift back to get number of pixels, rounded down -> R6
	subs	r4,r6		// get remaining width
	str	r4,[sp,#0]	// save new remaining width

// ---- generate odd pixel

	// [2,3] check odd pixel
	lsrs	r1,#1		// [1] check odd pixel
	bcc	RenderGraph2_InLoop // [1,2] odd pixel not set

	// [3] load image sample -> R4
	ldrb	r4,[r2,#0]	// [2] load image sample
	adds	r2,#1		// [1] increase pointer to image data

	// [5] write 4 pixels
	lsls	r4,#2		// [1] index*4
	ldr	r5,[r3,r4]	// [2] load colors
	stmia	r0!,{r5}	// [2] write pixels

	// [2,3] check end of data
	tst	r1,r1		// [1] check counter
	beq	RenderGraph2_EndLoop // [1,2] end

// ---- [17*N-1] start inner loop, render pixels in one part of segment
// Inner loop variables (* prepared before inner loop):
//  R0 ... *pointer to destination data buffer
//  R1 ... *number of 4-pixels to generate (loop counter)
//  R2 ... *pointer to source image buffer
//  R3 ... *pointer to palette translation table
//  R4 ... image sample
//  R5 ... output pixels
//  R6 ... output pixels
//  R7 ... *wrap width
//  LR ... *base pointer to image data (without X)

RenderGraph2_InLoop:

	// [2] load image sample -> R4
	ldrb	r4,[r2,#0]	// [2] load image sample

	// [3] prepare 4 pixels
	lsls	r4,#2		// [1] index*4
	ldr	r5,[r3,r4]	// [2] load colors

	// [3] load image sample -> R4
	ldrb	r4,[r2,#1]	// [2] load image sample
	adds	r2,#2		// [1] increase pointer to image data

	// [6] prepare and write next 4 pixels
	lsls	r4,#2		// [1] index*4
	ldr	r6,[r3,r4]	// [2] load colors
	stmia	r0!,{r5,r6}	// [3] write pixels

	// [2,3] loop counter
	subs	r1,#1		// [1] loop counter
	bne	RenderGraph2_InLoop // [1,2] next step

// ---- end inner loop, start new part

RenderGraph2_EndLoop:

	// continue to outer loop
	mov	r6,r7		// load wrap width -> R6
	mov	r2,lr		// get base pointer to text data -> R2
	b	RenderGraph2_OutLoop // go back to outer loop
