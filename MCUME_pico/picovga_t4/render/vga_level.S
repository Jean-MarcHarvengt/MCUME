
// ****************************************************************************
//
//                              VGA render GF_LEVEL
//
// ****************************************************************************

#include "../define.h"		// common definitions of C and ASM

	.syntax unified
	.section .time_critical.Render, "ax"
	.cpu cortex-m0plus
	.thumb			// use 16-bit instructions

// render font pixel mask
.extern	RenderTextMask		// u32 RenderTextMask[512];

// extern "C" u8* RenderLevel(u8* dbuf, int x, int y, int w, sSegm* segm);

// render level graph GF_LEVEL
//   dbuf ... destination data buffer
//   x ... start X coordinate (must be multiple of 4)
//   y ... start Y coordinate 
//   w ... width of this segment (must be multiple of 4)
//   segm ... video segment
// Output new dbuf pointer.
// 320 pixels takes 14 us on 151 MHz.

.thumb_func
.global RenderLevel
RenderLevel:

	// push registers
	push	{r1-r7,lr}

// Input registers and stack content:
//  R0 ... pointer to testination data buffer
//  SP+0: R1 start X coordinate (later: zero level)
//  SP+4: R2 start Y coordinate (later: base pointer to sample data)
//  SP+8: R3 width to display
//  SP+12: R4
//  SP+16: R5
//  SP+20: R6
//  SP+24: R7
//  SP+28: LR
//  SP+32: video segment (later: wrap width in X direction)

	// get pointer to video segment -> R4
	ldr	r4,[sp,#32]	// load video segment -> R4

	// get wrap width -> [SP+32]
	ldrh	r5,[r4,#SSEGM_WRAPX] // get wrap width
	movs	r7,#3		// mask to align to 32-bit
	bics	r5,r7		// align wrap
	str	r5,[sp,#32]	// save wrap width

	// align X coordinate to 32-bit -> R1
	bics	r1,r7

	// align remaining width -> [SP+8]
	bics	r3,r7
	str	r3,[sp,#8]	// save new width

	// current Y in direction from bottom to up -> R5
	ldrh	r5,[r4,#SSEGM_WRAPY] // get wrap height
	subs	r5,#1		// wrapy - 1
	subs	r5,r2		// subtract Y, get Y relative to bottom -> R5

	// get zero level -> [SP+0]
	ldrb	r3,[r4,#SSEGM_PAR2] // get zero level
	str	r3,[sp,#0]	// save zero level

	// base pointer to sample data (without X) -> [SP+4], R2
	ldr	r2,[r4,#SSEGM_DATA] // pointer to sample data
	str	r2,[sp,#4]	// save pointer to sample buffer

	// prepare pointer to sample data with X -> R2
	add	r2,r1		// pointer to source sample buffer -> R2

	// prepare foreground color, expand to 32-bit -> R6
	ldrb	r6,[r4,#SSEGM_PAR+1] // load foreground color
	lsls	r3,r6,#8	// [1] shift foreground color << 8
	orrs	r3,r6		// [1] color expanded to 16 bits
	lsls	r6,r3,#16	// [1] shift 16-bit color << 16
	orrs	r6,r3		// [1] color expanded to 32 bits

	// prepare background color, expand to 32 bits -> R4
	ldrb	r4,[r4,#SSEGM_PAR] // load background color
	lsls	r3,r4,#8	// shift background color << 8
	orrs	r3,r4		// color expanded to 16 bits
	lsls	r4,r3,#16	// shift 16-bit color << 16
	orrs	r4,r3		// color expanded to 32 bits

	// [1] XOR foreground and background color -> R6
	eors	r6,r4		// [1] XOR foreground color with background color

	// prepare pointer to conversion table -> LR
	ldr	r3,RenderLevel_Addr // get pointer to conversion table -> R5
	mov	lr,r3		// conversion table -> LR

	// prepare wrap width - start X -> R7
	ldr	r7,[sp,#32]	// load wrap width
	subs	r7,r1		// pixels remaining to end of segment

	// last 4-pixels
	cmp	r7,#4
	bhi	RenderLevel_OutLoop
	ldr	r7,[sp,#32]	// load wrap width
	b	RenderLevel_Last // render last 4-pixels of first segment

// ---- start outer loop, render one part of segment
// Outer loop variables (* prepared before outer loop):
//  R0 ... *pointer to destination data buffer
//  R1 ... number of 4-pixels to generate in one part of segment
//  R2 ... *pointer to source sample buffer
//  R3 ... remaining width, later: (temporary)
//  R4 ... *background color (expanded to 32-bit)
//  R5 ... *current line Y (in direction from bottom to up)
//  R6 ... *foreground color (expanded to 32-bit)
//  R7 ... *wrap width of this segment, later: (temporary)
//  LR ... *pointer to conversion table
//  [SP+0] ... *zero level
//  [SP+4] ... *base pointer to sample data (without X)
//  [SP+8] ... *remaining width
//  [SP+32] ... *wrap width

RenderLevel_OutLoop:

	// limit wrap width by total width -> R7
	ldr	r3,[sp,#8]	// get remaining width
	cmp	r7,r3		// compare with wrap width
	bls	2f		// width is OK
	mov	r7,r3		// limit wrap width

	// check number of pixels
2:	cmp	r7,#8		// check number of remaining pixels
	bhs	5f		// enough pixels remain to render 8-pixels

	// check last 4-pixels
	cmp	r7,#4		// check last 4-pixels
	blo	3f		// all done

// ---- render last 4 pixels

RenderLevel_Last:

	// check half of graph
	ldr	r3,[sp,#0]	// get zero level
	cmp	r5,r3		// check current line
	blo	RenderLevel_Last2 // bottom half of graph

// ---- top half

	// [1] clear sample accumulator
	movs	r1,#0		// [1] clear sample accumulator

	// [4] get sample 0
	ldrb	r3,[r2,#0]	// [2] get data sample -> R3
	cmp	r3,r5		// [1] compare sample with current line
	adcs	r1,r1		// [1] shift carry of comparison to accumulator -> R1
	
	// [4] get sample 1
	ldrb	r3,[r2,#1]	// [2] get data sample -> R3
	cmp	r3,r5		// [1] compare sample with current line
	adcs	r1,r1		// [1] shift carry of comparison to accumulator -> R1
	
	// [4] get sample 2
	ldrb	r3,[r2,#2]	// [2] get data sample -> R3
	cmp	r3,r5		// [1] compare sample with current line
	adcs	r1,r1		// [1] shift carry of comparison to accumulator -> R1
	
	// [4] get sample 3
	ldrb	r3,[r2,#3]	// [2] get data sample -> R3
	cmp	r3,r5		// [1] compare sample with current line
	adcs	r1,r1		// [1] shift carry of comparison to accumulator -> R1
	adds	r2,#4		// [1] shift pointer to source buffer

	// [2] prepare conversion table -> R1
	lsls	r1,#3		// [1] multiply sample * 8
	add	r1,lr		// [1] add pointer to conversion table

	// [7] convert 4 pixels (lower 4 bits)
	ldr	r1,[r1,#4]	// [2] load mask for lower 4 bits
	ands	r1,r6		// [1] mask foreground color
	eors	r1,r4		// [1] combine with background color
	stmia	r0!,{r1}	// [3] store 4 pixels

	b	7f	

// ---- bottom half

RenderLevel_Last2:

	// [1] clear sample accumulator
	movs	r1,#0		// [1] clear sample accumulator

	// [4] get sample 0
	ldrb	r3,[r2,#0]	// [2] get data sample -> R3
	cmp	r5,r3		// [1] compare sample with current line
	adcs	r1,r1		// [1] shift carry of comparison to accumulator -> R1
	
	// [4] get sample 1
	ldrb	r3,[r2,#1]	// [2] get data sample -> R3
	cmp	r5,r3		// [1] compare sample with current line
	adcs	r1,r1		// [1] shift carry of comparison to accumulator -> R1
	
	// [4] get sample 2
	ldrb	r3,[r2,#2]	// [2] get data sample -> R3
	cmp	r5,r3		// [1] compare sample with current line
	adcs	r1,r1		// [1] shift carry of comparison to accumulator -> R1
	
	// [4] get sample 3
	ldrb	r3,[r2,#3]	// [2] get data sample -> R3
	cmp	r5,r3		// [1] compare sample with current line
	adcs	r1,r1		// [1] shift carry of comparison to accumulator -> R1
	adds	r2,#4		// [1] shift pointer to source buffer

	// [2] prepare conversion table -> R1
	lsls	r1,#3		// [1] multiply sample * 8
	add	r1,lr		// [1] add pointer to conversion table

	// [7] convert 4 pixels (lower 4 bits)
	ldr	r1,[r1,#4]	// [2] load mask for lower 4 bits
	ands	r1,r6		// [1] mask foreground color
	eors	r1,r4		// [1] combine with background color
	stmia	r0!,{r1}	// [3] store 4 pixels


	// check if continue with next segment
7:	ldr	r2,[sp,#4]	// get base pointer to sample data -> R2
	cmp	r7,#4
	bhi	RenderLevel_OutLoop

	// pop registers and return
3:	pop	{r1-r7,pc}

// ---- prepare to render 8-pixels

	// prepare number of whole 4-pixels to render -> R1
5:	lsrs	r1,r7,#2	// shift width to get number of 4-pixels
	lsls	r7,r1,#2	// shift back to get number of pixels, rounded down -> R7
	subs	r3,r7		// get remaining width
	str	r3,[sp,#8]	// save new remaining width
	subs	r1,#1		// number of 4-pixels - 1

	// check half of graph
	ldr	r3,[sp,#0]	// get zero level
	cmp	r5,r3		// check current line
	blo	RenderLevel_InLoopBot // bottom half of graph

// ---- [50*N-1] start inner loop, render in one part of segment - top half of graph
// Inner loop variables (* prepared before inner loop):
//  R0 ... *pointer to destination data buffer
//  R1 ... *number of 4-pixels to generate*2 - 1 (loop counter)
//  R2 ... *pointer to source sample buffer
//  R3 ... sample
//  R4 ... *background color (expanded to 32-bit)
//  R5 ... *current line Y (in direction from bottom to up)
//  R6 ... *foreground color (expanded to 32-bit)
//  R7 ... sample accumulator, conversion table
//  LR ... *pointer to conversion table
//  [SP+0] ... *zero level
//  [SP+4] ... *base pointer to sample data (without X)
//  [SP+8] ... *remaining width
//  [SP+32] ... *wrap width

RenderLevel_InLoopTop: // render 8 pixels in one loop step, top half of graph

	// [1] clear sample accumulator
	movs	r7,#0		// [1] clear sample accumulator

	// [4] get sample 0
	ldrb	r3,[r2,#0]	// [2] get data sample -> R3
	cmp	r3,r5		// [1] compare sample with current line
	adcs	r7,r7		// [1] shift carry of comparison to accumulator -> R7
	
	// [4] get sample 1
	ldrb	r3,[r2,#1]	// [2] get data sample -> R3
	cmp	r3,r5		// [1] compare sample with current line
	adcs	r7,r7		// [1] shift carry of comparison to accumulator -> R7
	
	// [4] get sample 2
	ldrb	r3,[r2,#2]	// [2] get data sample -> R3
	cmp	r3,r5		// [1] compare sample with current line
	adcs	r7,r7		// [1] shift carry of comparison to accumulator -> R7
	
	// [4] get sample 3
	ldrb	r3,[r2,#3]	// [2] get data sample -> R3
	cmp	r3,r5		// [1] compare sample with current line
	adcs	r7,r7		// [1] shift carry of comparison to accumulator -> R7
	
	// [4] get sample 4
	ldrb	r3,[r2,#4]	// [2] get data sample -> R3
	cmp	r3,r5		// [1] compare sample with current line
	adcs	r7,r7		// [1] shift carry of comparison to accumulator -> R7
	
	// [4] get sample 5
	ldrb	r3,[r2,#5]	// [2] get data sample -> R3
	cmp	r3,r5		// [1] compare sample with current line
	adcs	r7,r7		// [1] shift carry of comparison to accumulator -> R7
	
	// [4] get sample 6
	ldrb	r3,[r2,#6]	// [2] get data sample -> R3
	cmp	r3,r5		// [1] compare sample with current line
	adcs	r7,r7		// [1] shift carry of comparison to accumulator -> R7
	
	// [5] get sample 7
	ldrb	r3,[r2,#7]	// [2] get data sample -> R3
	cmp	r3,r5		// [1] compare sample with current line
	adcs	r7,r7		// [1] shift carry of comparison to accumulator -> R7
	adds	r2,#8		// [1] shift pointer to source buffer

	// [2] prepare conversion table -> R7
	lsls	r7,#3		// [1] multiply sample * 8
	add	r7,lr		// [1] add pointer to conversion table

	// [4] convert first 4 pixels (higher 4 bits)
	ldr	r3,[r7,#0]	// [2] load mask for higher 4 bits
	ands	r3,r6		// [1] mask foreground color
	eors	r3,r4		// [1] combine with background color

	// [7] convert second 4 pixels (lower 4 bits)
	ldr	r7,[r7,#4]	// [2] load mask for lower 4 bits
	ands	r7,r6		// [1] mask foreground color
	eors	r7,r4		// [1] combine with background color
	stmia	r0!,{r3,r7}	// [3] store second 4 pixels

	// [2,3] loop counter
	subs	r1,#2		// [1] shift loop counter
	bhi	RenderLevel_InLoopTop // [1,2] > 0, render next whole 8-pixels

// ---- end inner loop, continue with last 4-pixels, or start new part

	// continue to outer loop
	ldr	r7,[sp,#32]	// load wrap width
8:	beq	RenderLevel_Last // render last 4-pixels
	ldr	r2,[sp,#4]	// get base pointer to sample data -> R2
	b	RenderLevel_OutLoop // go back to outer loop

// ---- [50*N-1] start inner loop, render in one part of segment - bottom half of graph
// Inner loop variables (* prepared before inner loop):
//  R0 ... *pointer to destination data buffer
//  R1 ... *number of 4-pixels to generate*2 - 1 (loop counter)
//  R2 ... *pointer to source sample buffer
//  R3 ... sample
//  R4 ... *background color (expanded to 32-bit)
//  R5 ... *current line Y (in direction from bottom to up)
//  R6 ... *foreground color (expanded to 32-bit)
//  R7 ... sample accumulator, conversion table
//  LR ... *pointer to conversion table
//  [SP+0] ... *zero level
//  [SP+4] ... *base pointer to sample data (without X)
//  [SP+8] ... *remaining width
//  [SP+32] ... *wrap width

RenderLevel_InLoopBot: // render 8 pixels in one loop step, bottom half of graph

	// [1] clear sample accumulator
	movs	r7,#0		// [1] clear sample accumulator

	// [4] get sample 0
	ldrb	r3,[r2,#0]	// [2] get data sample -> R3
	cmp	r5,r3		// [1] compare sample with current line
	adcs	r7,r7		// [1] shift carry of comparison to accumulator -> R7
	
	// [4] get sample 1
	ldrb	r3,[r2,#1]	// [2] get data sample -> R3
	cmp	r5,r3		// [1] compare sample with current line
	adcs	r7,r7		// [1] shift carry of comparison to accumulator -> R7
	
	// [4] get sample 2
	ldrb	r3,[r2,#2]	// [2] get data sample -> R3
	cmp	r5,r3		// [1] compare sample with current line
	adcs	r7,r7		// [1] shift carry of comparison to accumulator -> R7
	
	// [4] get sample 3
	ldrb	r3,[r2,#3]	// [2] get data sample -> R3
	cmp	r5,r3		// [1] compare sample with current line
	adcs	r7,r7		// [1] shift carry of comparison to accumulator -> R7
	
	// [4] get sample 4
	ldrb	r3,[r2,#4]	// [2] get data sample -> R3
	cmp	r5,r3		// [1] compare sample with current line
	adcs	r7,r7		// [1] shift carry of comparison to accumulator -> R7
	
	// [4] get sample 5
	ldrb	r3,[r2,#5]	// [2] get data sample -> R3
	cmp	r5,r3		// [1] compare sample with current line
	adcs	r7,r7		// [1] shift carry of comparison to accumulator -> R7
	
	// [4] get sample 6
	ldrb	r3,[r2,#6]	// [2] get data sample -> R3
	cmp	r5,r3		// [1] compare sample with current line
	adcs	r7,r7		// [1] shift carry of comparison to accumulator -> R7
	
	// [5] get sample 7
	ldrb	r3,[r2,#7]	// [2] get data sample -> R3
	cmp	r5,r3		// [1] compare sample with current line
	adcs	r7,r7		// [1] shift carry of comparison to accumulator -> R7
	adds	r2,#8		// [1] shift pointer to source buffer

	// [2] prepare conversion table -> R7
	lsls	r7,#3		// [1] multiply sample * 8
	add	r7,lr		// [1] add pointer to conversion table

	// [4] convert first 4 pixels (higher 4 bits)
	ldr	r3,[r7,#0]	// [2] load mask for higher 4 bits
	ands	r3,r6		// [1] mask foreground color
	eors	r3,r4		// [1] combine with background color

	// [7] convert second 4 pixels (lower 4 bits)
	ldr	r7,[r7,#4]	// [2] load mask for lower 4 bits
	ands	r7,r6		// [1] mask foreground color
	eors	r7,r4		// [1] combine with background color
	stmia	r0!,{r3,r7}	// [3] store second 4 pixels

	// [2,3] loop counter
	subs	r1,#2		// [1] shift loop counter
	bhi	RenderLevel_InLoopBot // [1,2] > 0, render next whole 8-pixels

// ---- end inner loop, continue with last 4-pixels, or start new part

	// continue to outer loop
	ldr	r7,[sp,#32]	// load wrap width
	beq	8b		// render last 4-pixels
	ldr	r2,[sp,#4]	// get base pointer to sample data -> R2
	b	RenderLevel_OutLoop // go back to outer loop

	.align 2
RenderLevel_Addr:
	.word	RenderTextMask
