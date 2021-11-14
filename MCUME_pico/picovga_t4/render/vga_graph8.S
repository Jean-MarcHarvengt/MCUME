
// ****************************************************************************
//
//                              VGA render GF_GRAPH8
//
// ****************************************************************************

#include "../define.h"		// common definitions of C and ASM

	.syntax unified
	.section .time_critical.Render, "ax"
	.cpu cortex-m0plus
	.thumb			// use 16-bit instructions

// extern "C" u32* RenderGrad1(u32* cbuf, int x, int y, int w, sSegm* segm);

// render gradient with 1 line GF_GRAD1
//  R0 ... pointer to control buffer
//  R1 ... start X coordinate (in pixels, must be multiple of 4)
//  R2 ... start Y coordinate (in graphics lines), will be ignored and substituted with 0
//  R3 ... width to display (must be multiple of 4 and > 0)
//  [stack] ... segm video segment sSegm
// Output new pointer to control buffer.
// 320 pixels takes 0.45 us on 151 MHz.

.thumb_func
.global RenderGrad1
RenderGrad1:
	movs	r2,#0


// extern "C" u32* RenderGrad2(u32* cbuf, int x, int y, int w, sSegm* segm);

// render gradient with 2 lines GF_GRAD2
//  R0 ... pointer to control buffer
//  R1 ... start X coordinate (in pixels, must be multiple of 4)
//  R2 ... start Y coordinate (in graphics lines), will be masked to values 0 and 1
//  R3 ... width to display (must be multiple of 4 and > 0)
//  [stack] ... segm video segment sSegm
// Output new pointer to control buffer.
// 320 pixels takes 0.45 us on 151 MHz.

.thumb_func
.global RenderGrad2
RenderGrad2:
	lsls	r2,#31
	lsrs	r2,#31


// extern "C" u32* RenderGraph8(u32* cbuf, int x, int y, int w, sSegm* segm);

// render native 8-bit graphics GF_GRAPH8
//  R0 ... pointer to control buffer
//  R1 ... start X coordinate (in pixels, must be multiple of 4)
//  R2 ... start Y coordinate (in graphics lines)
//  R3 ... width to display (must be multiple of 4 and > 0)
//  [stack] ... segm video segment sSegm
// Output new pointer to control buffer.
// 320 pixels takes 0.45 us on 151 MHz.

.thumb_func
.global RenderGraph8
RenderGraph8:

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
//  R1 ... X coordinate, later: width of one segment
//  R2 ... Y coordinate, later: current pointer to data buffer
//  R3 ... remaining width
//  R4 ... base pointer to data buffer
//  R5 ... (temporary)
//  R6 ... (temporary)
//  R7 ... wrap width

	// get pointer to video segment -> R4
	ldr	r4,[sp,#20]	// load video segment -> R4

	// get wrap width -> R7
	ldrh	r7,[r4,#SSEGM_WRAPX] // get wrap width
	movs	r6,#3		// mask to align to 32-bit
	bics	r7,r6		// align wrap

	// align X coordinate to 32-bit -> R1
	bics	r1,r6

	// align remaining width -> R3
	bics	r3,r6

	// base pointer to data buffer (without X) -> R4
	ldrh	r5,[r4,#SSEGM_WB] // get pitch of rows
	muls	r2,r5		// Y * WB -> offset in data buffer
	ldr	r5,[r4,#SSEGM_DATA] // pointer to data
	adds	r4,r2,r5	// base address of data buffer -> R4

	// prepare current pointer to image data with X -> R2
	adds	r2,r4,r1	// pointer to source data buffer -> R2

	// prepare wrap width - start X -> R1
	subs	r1,r7,r1	// pixels remaining to end of segment

	// check remaining width
2:	tst	r3,r3		// check remaining width
	beq	6f		// end of data

	// limit wrap width by total width -> R1
	cmp	r1,r3		// compare with wrap width
	bls	4f		// width is OK
	mov	r1,r3		// limit wrap width

	// decrease remaining width
4:	subs	r3,r1		// subtract from remaining width
	
	// save control block
	lsrs	r1,#2		// width / 4
	stm	r0!,{r1,r2}	// save width and pointer to control block

	// continue to next loop
	mov	r1,r7		// load wrap width -> R1
	mov	r2,r4		// get base pointer to text data -> R2
	b	2b		// go next loop

	// pop registers and return
6:	pop	{r4-r7,pc}
