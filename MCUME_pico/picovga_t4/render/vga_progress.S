
// ****************************************************************************
//
//                              VGA render GF_PROGRESS
//
// ****************************************************************************

#include "../define.h"		// common definitions of C and ASM

	.syntax unified
	.section .time_critical.Render, "ax"
	.cpu cortex-m0plus
	.thumb			// use 16-bit instructions

// extern "C" u32* RenderProgress(u32* cbuf, int x, int y, int w, sSegm* segm);

// render horizontal progress indicator GF_PROGRESS
//  R0 ... pointer to control buffer
//  R1 ... start X coordinate (in pixels, must be multiple of 4)
//  R2 ... start Y coordinate (in graphics lines)
//  R3 ... width to display (must be multiple of 4 and > 0)
//  [stack] ... segm video segment sSegm
// Output new pointer to control buffer.
// 320 pixels takes 0.5 us on 151 MHz.

.thumb_func
.global RenderProgress
RenderProgress:

	// push registers
	push	{r4-r7,lr}

// Stack content:
//  SP+0: R4
//  SP+4: R5
//  SP+8: R6
//  SP+12: R7
//  SP+16: LR
//  SP+20: video segment

// Variables:
//  R0 ... pointer to control buffer
//  R1 ... X coordinate/4
//  R2 ... data sample
//  R3 ... remaining width
//  R4 ... gradient buffer 1
//  R5 ... gradient buffer 2
//  R6 ... (temporary)
//  R7 ... current wrap width
//  LR ... wrap width

	// get pointer to video segment -> R4
	ldr	r4,[sp,#20]	// load video segment -> R4

	// prepare X coordinate/4 -> R1
	lsrs	r1,#2		// X coordinate/4 -> R1

	// load data sample -> R2
	ldr	r5,[r4,#SSEGM_DATA] // pointer to data
	ldrb	r2,[r5,r2]	// load data sample -> R2

	// prepare remaining width/4 -> R3
	lsrs	r3,#2		// width/4 -> R3

	// get wrap width/4 -> LR
	ldrh	r7,[r4,#SSEGM_WRAPX] // get wrap width
	lsrs	r7,#2		// wrap width/4 -> R7
	mov	lr,r7

	// prepare gradient buffers -> R4, R5
	ldr	r5,[r4,#SSEGM_PAR2] // gradient buffer 2 -> R5
	ldr	r4,[r4,#SSEGM_PAR] // gradient buffer 1 -> R4

	// check remaining width
2:	tst	r3,r3		// check remaining width
	beq	9f		// end of data

	// prepare wrap width - start X -> R7
	mov	r7,lr		// wrap width
	subs	r7,r1		// pixels remaining to end of segment

	// limit wrap width by total width -> R7
	cmp	r7,r3		// compare with wrap width
	bls	4f		// width is OK
	mov	r7,r3		// limit wrap width

	// decrease remaining width
4:	subs	r3,r7		// subtract from remaining width

	// first part visible if x < data
	cmp	r1,r2
	bhs	6f		// x >= data

	// width of this part
	subs	r6,r2,r1	// width <- data - x

	// limit width
	cmp	r6,r7		// check width
	bls	5f		// width is OK
	mov	r6,r7		// limit width
5:	subs	r7,r6		// decrease width

	// save control block with 1st part
5:	stm	r0!,{r6}	// write width
	adds	r6,r4,r1	// gradient address at offset x
	stm	r0!,{r6}	// write address
	mov	r1,r2		// X <- data

	// check if some width remain
6:	tst	r7,r7		// check with of this part
	beq	7f		// end of segment

	// save control block width 2nd part
	stm	r0!,{r7}	// write width
	adds	r6,r5,r1	// gradient address at offset x
	stm	r0!,{r6}	// write address

	// reset X
7:	movs	r1,#0		// reset X
	b	2b		// next segment

	// pop registers and return
9:	pop	{r4-r7,pc}
