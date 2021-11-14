
// ****************************************************************************
//
//                              VGA render GF_GRAPH8MAT
//
// ****************************************************************************
// data ... image data
// par ... pointer to 6 matrix integer parameters m11,m12..m23 ((int)(m*FRACTMUL))
// par2 ... LOW=number of bits of image width, HIGH=number of bits of image height
//      image width must be max. 4096 (= 1<<FRACT); image with and height must be power of 2
// wrapy ... segment height

#include "../define.h"		// common definitions of C and ASM
#include "hardware/regs/sio.h"	// registers of hardware divider
#include "hardware/regs/addressmap.h" // SIO base address

#define ACCUM0_OFFSET		0
#define ACCUM1_OFFSET		4
#define BASE0_OFFSET		8
#define BASE1_OFFSET		12
#define BASE2_OFFSET		16
#define POP_LANE0_OFFSET	20
#define POP_LANE1_OFFSET	24
#define POP_FULL_OFFSET		28
#define PEEK_LANE0_OFFSET	32
#define PEEK_LANE1_OFFSET	36
#define PEEK_FULL_OFFSET	40
#define CTRL_LANE0_OFFSET	44
#define CTRL_LANE1_OFFSET	48
#define ACCUM0_ADD_OFFSET	52
#define ACCUM1_ADD_OFFSET	56
#define BASE_1AND0_OFFSET	60

	.syntax unified
	.section .time_critical.Render, "ax"
	.cpu cortex-m0plus
	.thumb			// use 16-bit instructions

// extern "C" u32* RenderGraph8Mat(u32* cbuf, int x, int y, int w, sSegm* segm);

// render 8-bit graphics GF_GRAPH8MAT, with 2D matrix transformation,
// using hardware interpolator inter1 (inter1 state is not saved during interrup)
//  R0 ... pointer to destination data buffer
//  R1 ... start X coordinate (not used)
//  R2 ... start Y coordinate (in graphics lines)
//  R3 ... width to display (must be multiple of 4)
//  [stack] ... segm video segment sSegm
// Output new pointer to data buffer.

.thumb_func
.global RenderGraph8Mat
RenderGraph8Mat:

// Input registers and stack:
//  R0 ... pointer to destination data buffer
//  R1 ... X coordinate (not used)
//  R2 ... Y coordinate
//  R3 ... remaining width
//  SP+0: R4
//  SP+4: R5
//  SP+8: R6
//  SP+12: R7
//  SP+16: LR
//  SP+20: video segment

	// push registers
	push	{r4-r7,lr}

// ---- prepare registers

	// prepare start coordinate X0 = -w/2 -> LR
	lsrs	r1,r3,#1	// width/2
	negs	r1,r1		// negate
	mov	lr,r1		// store start coordinate X0 -> LR

	// prepare number of 4-pixels (loop counter) -> R7
	lsrs	r7,r3,#2	// width/4 -> R7

	// get pointer to video segment -> R4
	ldr	r4,[sp,#20]	// load video segment -> R4

	// prepare current coordinate Y0 = -h/2 + y -> R12
	ldrh	r1,[r4,#SSEGM_WRAPY] // get segment height -> R1
	lsrs	r1,#1		// height/2
	negs	r1,r1		// negate
	adds	r1,r2		// add current Y coordinate
	mov	r12,r1		// store current coordinate Y0 -> R12

	// get number of bits of image width "xbits" -> R1
	ldrh	r1,[r4,#SSEGM_PAR2] // number of bits of image width -> R1

	// get number of bits of image height "ybits" -> R2
	ldrh	r2,[r4,#SSEGM_PAR2+2] // number of bits of image height -> R2

	// prepare address of interpolator base -> R3
	ldr	r3,RenderGraph8Mat_Interp // get address of interpolator base -> R3

// R0 ... pointer to data buffer
// R1 ... number of bits of image width xbits
// R2 ... number of bits of image height ybits
// R3 ... interpolator base
// R4 ... video segment
// R7 ... width/4
// LR ... start coordinate X0
// R12 ... current coordinate Y0

// ---- setup interpolator

	// set image base to base2
	ldr	r6,[r4,#SSEGM_DATA]	// load image base
	str	r6,[r3,#BASE2_OFFSET]	// set image base

	// set control word of lane 1 - add raw lane base back to accumulator, shift "FRACT-xbits", mask xbits...xbits+ybits-1
	ldr	r6,RenderGraph8Mat_Ctrl // load control word
	subs	r6,r1			// FRACT - xbits (SIO_INTERP0_CTRL_LANE0_SHIFT_LSB = 0, no shift required)
	lsls	r5,r1,#SIO_INTERP0_CTRL_LANE0_MASK_LSB_LSB // shift xbits to mask LSB position -> R5
	orrs	r6,r5			// add xbits to control word
	subs	r1,#1			// xbits - 1 -> R1
	adds	r5,r1,r2		// xbits-1+ybits -> R5
	lsls	r5,#SIO_INTERP0_CTRL_LANE0_MASK_MSB_LSB // shift to MSB mask position 
	orrs	r6,r5			// add to control word
	str	r6,[r3,#CTRL_LANE1_OFFSET] // set control word of lane 1

// R0 ... pointer to data buffer
// R1 ... image width xbits-1
// R3 ... interpolator base
// R4 ... video segment
// R7 ... width/4
// LR ... start coordinate X0
// R12 ... current coordinate Y0

	// set control word of lane 0 - add raw lane base back to accumulator, shift "FRACT", mask 0..xbits-1
	ldr	r6,RenderGraph8Mat_Ctrl // load control word
	lsls	r1,#SIO_INTERP0_CTRL_LANE0_MASK_MSB_LSB // shift xbits-1 to mask MSB position
	orrs	r6,r1			// add to control word
	str	r6,[r3,#CTRL_LANE0_OFFSET] // set control word of lane 0

// R0 ... pointer to data buffer
// R3 ... interpolator base
// R4 ... video segment
// R7 ... width/4
// LR ... start coordinate X0
// R12 ... current coordinate Y0

// ---- set matrix

	// get pointer to matrix -> R4
	ldr	r4,[r4,#SSEGM_PAR]	// get pointer to matrix -> R4

// r4+0 ... m11
// r4+4 ... m12
// r4+8 ... m13
// r4+12 ... m21
// r4+16 ... m22
// r4+20 ... m23

	// set m11 -> R5 base0
	ldr	r5,[r4,#0]	// load m11
	str	r5,[r3,#BASE0_OFFSET] // set base0

	// set m21 -> R6 base1
	ldr	r6,[r4,#12]	// load m21
	str	r6,[r3,#BASE1_OFFSET] // set base1

// R0 ... pointer to data buffer
// R3 ... interpolator base
// R4 ... pointer to matrix
// R5 ... m11
// R6 ... m21
// R7 ... width/4
// LR ... start coordinate X0
// R12 ... current coordinate Y0

	// set x0*m11 + y0*m12 + m13 -> accum0
	mov	r2,lr		// start coordinate X0 -> X2
	muls	r5,r2		// x0*m11 -> R5
	muls	r2,r6		// x0*m21 -> R2
	ldr	r1,[r4,#4]	// load m12 -> R1
	mov	r6,r12		// load coordinate Y0 -> R6
	muls	r1,r6		// y0*m12 -> R1
	adds	r5,r1		// x0*m11 + y0*m12 -> R5
	ldr	r1,[r4,#8]	// load m13 -> R1
	adds	r5,r1		// x0*m11 + y0*m12 + m13 -> R5
	str	r5,[r3,#ACCUM0_OFFSET] // set accum0

// R0 ... pointer to data buffer
// R2 ... x0*m21
// R3 ... interpolator base
// R4 ... pointer to matrix
// R6 ... current coordinate Y0
// R7 ... width/4

	// set x0*m21 + y0*m22 + m23 -> accum1
	ldr	r1,[r4,#16]	// load m22 -> R1
	muls	r1,r6		// y0*m22 -> R1
	adds	r2,r1		// x0*m21 + y0*m22 -> R2
	ldr	r1,[r4,#20]	// load m23 -> R1
	adds	r2,r1		// x0*m21 + y0*m22 + m23 -> R2
	str	r2,[r3,#ACCUM1_OFFSET] // set accum1

// ---- process odd 4-pixel

//  R0 ... pointer to destination data buffer
//  R3 ... interpolator base
//  R4 ... (temporary - get pointer to pixel)
//  R5 ... (temporary - load pixel)
//  R6 ... (temporary - pixel accumulator)
//  R7 ... width/4 (loop counter)

	lsrs	r7,#1		// width/4/2
	bcc	2f		// no odd 4-pixel

	// [3] load 1st pixel
	ldr	r4,[r3,#POP_FULL_OFFSET] // [1] get new value
	ldrb	r6,[r4,#0]	// [2] load pixel

	// [5] load 2nd pixel
	ldr	r4,[r3,#POP_FULL_OFFSET] // [1] get new value
	ldrb	r5,[r4,#0]	// [2] load pixel
	lsls	r5,#8		// [1] shift 1 byte left
	orrs	r6,r5		// [1] add pixel to accumulator

	// [5] load 3rd pixel
	ldr	r4,[r3,#POP_FULL_OFFSET] // [1] get new value
	ldrb	r5,[r4,#0]	// [2] load pixel
	lsls	r5,#16		// [1] shift 2 bytes left
	orrs	r6,r5		// [1] add pixel to accumulator

	// [5] load 4th pixel
	ldr	r4,[r3,#POP_FULL_OFFSET] // [1] get new value
	ldrb	r5,[r4,#0]	// [2] load pixel
	lsls	r5,#24		// [1] shift 3 bytes left
	orrs	r6,r5		// [1] add pixel to accumulator

	// [2] store 4 pixels
	stmia	r0!,{r6}	// [2] store 4 pixels

	// check number of remaining pixels
2:	tst	r7,r7		// check number of pixels
	beq	8f		// end

// ---- [42 per 8 pixels] inner loop
//  R0 ... pointer to destination data buffer
//  R1 ... (temporary - pixel accumulator 1)
//  R2 ... (temporary - pixel accumulator 2)
//  R3 ... interpolator base
//  R4 ... (temporary - get pointer to pixel, load pixel)
//  R7 ... width/8 (loop counter)

	// [3] load 1st pixel
6:	ldr	r4,[r3,#POP_FULL_OFFSET] // [1] get new value
	ldrb	r1,[r4,#0]	// [2] load pixel

	// [5] load 2nd pixel
	ldr	r4,[r3,#POP_FULL_OFFSET] // [1] get new value
	ldrb	r4,[r4,#0]	// [2] load pixel
	lsls	r4,#8		// [1] shift 1 byte left
	orrs	r1,r4		// [1] add pixel to accumulator

	// [5] load 3rd pixel
	ldr	r4,[r3,#POP_FULL_OFFSET] // [1] get new value
	ldrb	r4,[r4,#0]	// [2] load pixel
	lsls	r4,#16		// [1] shift 2 bytes left
	orrs	r1,r4		// [1] add pixel to accumulator

	// [5] load 4th pixel
	ldr	r4,[r3,#POP_FULL_OFFSET] // [1] get new value
	ldrb	r4,[r4,#0]	// [2] load pixel
	lsls	r4,#24		// [1] shift 3 bytes left
	orrs	r1,r4		// [1] add pixel to accumulator

	// [3] load 1st pixel
	ldr	r4,[r3,#POP_FULL_OFFSET] // [1] get new value
	ldrb	r2,[r4,#0]	// [2] load pixel

	// [5] load 2nd pixel
	ldr	r4,[r3,#POP_FULL_OFFSET] // [1] get new value
	ldrb	r4,[r4,#0]	// [2] load pixel
	lsls	r4,#8		// [1] shift 1 byte left
	orrs	r2,r4		// [1] add pixel to accumulator

	// [5] load 3rd pixel
	ldr	r4,[r3,#POP_FULL_OFFSET] // [1] get new value
	ldrb	r4,[r4,#0]	// [2] load pixel
	lsls	r4,#16		// [1] shift 2 bytes left
	orrs	r2,r4		// [1] add pixel to accumulator

	// [5] load 4th pixel
	ldr	r4,[r3,#POP_FULL_OFFSET] // [1] get new value
	ldrb	r4,[r4,#0]	// [2] load pixel
	lsls	r4,#24		// [1] shift 3 bytes left
	orrs	r2,r4		// [1] add pixel to accumulator

	// [3] store 8 pixels
	stmia	r0!,{r1,r2}	// [3] store 8 pixels

	// [2,3] loop counter
	subs	r7,#1		// [1] 8-pixel counter
	bne	6b		// [1,2] next 8-pixels

	// pop registers
8:	pop	{r4-r7,pc}

	.align 2
// pointer to Interp1 base
RenderGraph8Mat_Interp:
	.word	SIO_BASE+SIO_INTERP1_ACCUM0_OFFSET // addres of interpolator base

RenderGraph8Mat_Ctrl:		// lane control word
	.word	SIO_INTERP0_CTRL_LANE0_ADD_RAW_BITS | (FRACT<<SIO_INTERP0_CTRL_LANE0_SHIFT_LSB)
