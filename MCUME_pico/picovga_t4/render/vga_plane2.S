
// ****************************************************************************
//
//                              VGA render GF_PLANE2
//
// ****************************************************************************

#include "../define.h"		// common definitions of C and ASM

	.syntax unified
	.section .time_critical.Render, "ax"
	.cpu cortex-m0plus
	.thumb			// use 16-bit instructions

// extern "C" u8* RenderPlane2(u8* dbuf, int x, int y, int w, sSegm* segm);

// render 2-bit palette graphics GF_GRAPH2
//  R0 ... destination data buffer
//  R1 ... start X coordinate (must be multiple of 4)
//  R2 ... start Y coordinate 
//  R3 ... width of this segment (must be multiple of 4)
//  segm ... video segment
// Output new dbuf pointer.
// 320 pixels takes 7.3 us on 151 MHz.

.thumb_func
.global RenderPlane2
RenderPlane2:

	// push registers
	push	{r3-r7,lr}

// Input registers and stack content:
//  R0 ... destination data buffer
//  R1 ... start X coordinate
//  R2 ... start Y coordinate
//  SP+0: R3 width to display (remaining width)
//  SP+4: R4
//  SP+8: R5
//  SP+12: R6
//  SP+16: R7
//  SP+20: LR
//  SP+24: video segment (later: wrap width in X direction)

	// get pointer to video segment -> R4
	ldr	r4,[sp,#24]	// load video segment -> R4

	// get wrap width -> [SP+24]
	ldrh	r7,[r4,#SSEGM_WRAPX] // get wrap width
	movs	r6,#3		// mask to align to 32-bit
	bics	r7,r6		// align wrap
	str	r7,[sp,#24]	// save wrap width

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
	lsrs	r6,r1,#3	// convert X to 8-pixel offset
	add	r2,r6		// pointer to source image buffer -> R2

	// prepare size of one plane -> R3
	ldr	r3,[r4,#SSEGM_PAR] // get size of one plane -> R3

	// prepare pointer to palette translation table -> R7
	ldr	r7,[r4,#SSEGM_PAR2] // get pointer to palette translation table -> R7

// ---- render 2nd half of first 8-pixel
//  R0 ... pointer to destination data buffer
//  R1 ... start X coordinate
//  R2 ... pointer to source image data
//  R3 ... size of one plane (= offset of plane 1 from plane 0)
//  R4 ... (temporary)
//  R5 ... (temporary)
//  R6 ... (temporary)
//  R7 ... *pointer to palette translation table
//  LR ... *base pointer to image data (without X)
//  [SP+0] ... *remaining width
//  [SP+24] ... *wrap width

	// check bit 2 of X coordinate - check if image starts with 2nd half of first 8-pixel
	lsls	r5,r1,#29	// check bit 2 of X coordinate
	bpl	2f		// bit 2 not set, starting even 4-pixels

	// [5] load samples -> R5, R6
	ldrb	r5,[r2,#0]	// [2] load sample from plane 1
	ldrb	r6,[r2,r3]	// [2] load sample from plane 2
	adds	r2,#1		// [1] increase pointer

	// [5] compose samples LOW -> R5
	lsls	r6,#28		// [1] isolate low 4 bits from sample 2
	lsrs	r6,#22		// [1] shift to bit position 6
	lsls	r5,#28		// [1] isolate low 4 bit from sample 1
	lsrs	r5,#26		// [1] shift to bit position 2
	orrs	r5,r6		// [1] compose samples

	// [5] write pixels
	ldr	r5,[r7,r5]	// [2] load colors
	stmia	r0!,{r5}	// [3] write pixels

	// shift X coordinate
	adds	r1,#4		// shift X coordinate

	// check end of segment
	ldr	r6,[sp,#24]	// load wrap width
	cmp	r1,r6		// X=end of segment?
	blo	1f
	movs	r1,#0		// reset X coordinate
	mov	r2,lr		// get base pointer to image data -> R2

	// shift remaining width
1:	ldr	r6,[sp,#0]	// get remaining width
	subs	r6,#4		// shift width
	str	r6,[sp,#0]	// save new width

	// prepare wrap width - start X -> R6
2:	ldr	r6,[sp,#24]	// load wrap width
	subs	r6,r1		// pixels remaining to end of segment

// ---- start outer loop, render one part of segment
// Outer loop variables (* prepared before outer loop):
//  R0 ... *pointer to destination data buffer
//  R1 ... number of 4-pixels - 1 to generate in one part of segment
//  R2 ... *pointer to source image data
//  R3 ... *size of one plane (= offset of plane 1 from plane 0)
//  R4 ... (temporary)
//  R5 ... (temporary)
//  R6 ... part width
//  R7 ... *pointer to palette translation table
//  LR ... *base pointer to image data (without X)
//  [SP+0] ... *remaining width
//  [SP+24] ... *wrap width

RenderPlane2_OutLoop:

	// limit wrap width by total width -> R7
	ldr	r4,[sp,#0]	// get remaining width
	cmp	r6,r4		// compare with wrap width
	bls	2f		// width is OK
	mov	r6,r4		// limit wrap width

	// check number of pixels
2:	cmp	r6,#8		// check number of remaining pixels
	bhs	5f		// enough 8-pixels remain

	// check if 1st part of last 8-pixel remains
	cmp	r6,#4		// check number of pixels
	blo	3f		// all done

// ---- render 1st part of last 8-pixel

RenderPlane2_Last:

	// [5] load samples -> R5, R4
	ldrb	r5,[r2,#0]	// [2] load sample from plane 1
	ldrb	r4,[r2,r3]	// [2] load sample from plane 2
	adds	r2,#1		// [1] increase pointer

	// [5] compose samples HIGH -> R4
	lsrs	r4,#4		// [1] isolate high 4 bits from sample 2
	lsls	r4,#8		// [1] shift left
	orrs	r4,r5		// [1] compose sample 2 with sample 1
	lsrs	r4,#4		// [1] isolate high 4 bits from sample 1
	lsls	r4,#2		// [1] 2 shifts to get index*4

	// [4] write pixels
	ldr	r4,[r7,r4]	// [2] load colors
	stmia	r0!,{r4}	// [2] write pixels

	// check if continue with next segment
	mov	r2,lr		// get base pointer to image data -> R2
	cmp	r6,#4
	bhi	RenderPlane2_OutLoop

	// pop registers and return
3:	pop	{r3-r7,pc}

// ---- prepare to render whole 8-pixels

	// prepare number of 4-pixels to render -> R1
5:	lsrs	r1,r6,#2	// shift to get number of 4-pixels
	lsls	r6,r1,#2	// shift back to get number of pixels, rounded down -> R6
	subs	r4,r6		// get remaining width
	str	r4,[sp,#0]	// save new remaining width
	subs	r1,#1		// number of 4-pixels - 1

// ---- [25*N-1] start inner loop, render whole 8-pixels in one part of segment
// Inner loop variables (* prepared before inner loop):
//  R0 ... *pointer to destination data buffer
//  R1 ... *number of 4-pixels - 1 to generate (loop counter)
//  R2 ... *pointer to source image data
//  R3 ... *size of one plane (= offset of plane 1 from plane 0)
//  R4 ... output sample
//  R5 ... sample from plane 1
//  R6 ... sample from plane 2
//  R7 ... *pointer to palette translation table
//  LR ... *base pointer to image data (without X)
//  [SP+0] ... *remaining width
//  [SP+24] ... *wrap width

RenderPlane2_InLoop:

	// [5] load samples -> R5, R6
	ldrb	r5,[r2,#0]	// [2] load sample from plane 1
	ldrb	r6,[r2,r3]	// [2] load sample from plane 2
	adds	r2,#1		// [1] increase pointer

	// [5] compose samples HIGH -> R4
	lsrs	r4,r6,#4	// [1] isolate high 4 bits from sample 2
	lsls	r4,#8		// [1] shift left
	orrs	r4,r5		// [1] compose sample 2 with sample 1
	lsrs	r4,#4		// [1] isolate high 4 bits from sample 1
	lsls	r4,#2		// [1] 2 shifts to get index*4

	// [2] prepare first 4 pixels
	ldr	r4,[r7,r4]	// [2] load colors

	// [5] compose samples LOW -> R5
	lsls	r6,#28		// [1] isolate low 4 bits from sample 2
	lsrs	r6,#22		// [1] shift to bit position 6
	lsls	r5,#28		// [1] isolate low 4 bit from sample 1
	lsrs	r5,#26		// [1] shift to bit position 2
	orrs	r5,r6		// [1] compose samples

	// [5] write pixels
	ldr	r5,[r7,r5]	// [2] load colors
	stmia	r0!,{r4,r5}	// [3] write pixels

	// [2,3] loop counter
	subs	r1,#2		// [1] loop counter
	bhi	RenderPlane2_InLoop // [1,2] > 0, next step

// ---- end inner loop

RenderPlane2_EndLoop:

	// continue to outer loop
	ldr	r6,[sp,#24]	// load wrap width -> R6
	beq	RenderPlane2_Last // render 1st half of last 8-pixels
	mov	r2,lr		// get base pointer to image data -> R2
	b	RenderPlane2_OutLoop // go back to outer loop
