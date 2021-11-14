
// ****************************************************************************
//
//                              VGA render GF_COLOR
//
// ****************************************************************************

#include "../define.h"		// common definitions of C and ASM

	.syntax unified
	.section .time_critical.Render, "ax"
	.cpu cortex-m0plus
	.thumb			// use 16-bit instructions

// extern "C" u8* RenderColor(u8* dbuf, u32 color, int w);

// render color GF_COLOR
//  R0 ... pointer to destination data buffer
//  R1 ... color pattern 4-pixels
//  R2 ... width of this segment as multiply of 4 pixels (=width in pixels/4)
// Output new pointer to destination data buffer.
// 320 pixels takes 1.1 us on 151 MHz.
//   - using only small transfer (24 pixels per loop) takes 1.22 us
//   - using only single transfer (4 pixels per loop) takes 2.91 us
//   - memset takes 1.42 us

.thumb_func
.global RenderColor
RenderColor:

// fill memory buffer with u32 words
//  buf ... data buffer, must be 32-bit aligned
//  data ... data word to store
//  num ... number of 32-bit words (= number of bytes/4)
// Returns new destination address.
// extern "C" u32* MemSet4(u32* buf, u32 data, int num);

.thumb_func
.global MemSet4
MemSet4:

	// push registers
	push	{r4,r5,r6,r7,lr}

	// duplicate color pattern
	mov	r3,r1
	mov	r4,r1
	mov	r5,r1
	mov	r6,r1
	mov	r7,r1

	// go to big transfer
	b	3f

// ---- [38 per loop] big transfer 120 pixels, speed 0.317 clk per pixel

	// [38] store 30 words (=120 pixels)
2:	stmia	r0!,{r1,r3,r4,r5,r6,r7} // [7] 6 words, 24 pixels
	stmia	r0!,{r1,r3,r4,r5,r6,r7} // [7] 6 words, 24 pixels
	stmia	r0!,{r1,r3,r4,r5,r6,r7} // [7] 6 words, 24 pixels
	stmia	r0!,{r1,r3,r4,r5,r6,r7} // [7] 6 words, 24 pixels
	stmia	r0!,{r1,r3,r4,r5,r6,r7} // [7] 6 words, 24 pixels
3:	subs	r2,#30		// [1] decrement number of words
	bge	2b		// [1,2] loop next 30 words
	adds	r2,#30		// [1] restore

	// go to small transfer
	b	6f

// ---- [10 per loop] small transfer 24 pixels, speed 0.417 clk per pixel

	// [8] store 6 words (=24 pixels)
4:	stmia	r0!,{r1,r3,r4,r5,r6,r7} // [7] 6 words, 24 pixels
6:	subs	r2,#6		// [1] decrement number of words
	bge	4b		// [1,2] loop next 6 words
	adds	r2,#6		// [1] restore

	// go to single transfer
	b	8f

// ---- [5 per loop] single transfer 4 pixels, speed 1.25 clk per pixel

	// [4,5] store 1 word (=4 pixels)
7:	stmia	r0!,{r1}	// [2] 1 word, 4 pixels
8:	subs	r2,#1		// [1] loop counter
	bge	7b		// [1,2] next word

	// pop registers
	pop	{r4,r5,r6,r7,pc}
