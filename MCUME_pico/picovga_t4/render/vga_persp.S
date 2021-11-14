
// ****************************************************************************
//
//                       VGA render LAYERMODE_PERSP*
//
// ****************************************************************************
// img ... (const u8*) SLAYER_IMG image data
// par ... (const void*) SLAYER_PAR pointer to 6 matrix integer parameters m11,m12..m23
// horiz ... (s8) SLAYER_HORIZ horizon offset/4 (0=no perspecitve, <0 ceilling)
// xbits ... (u8) SLAYER_XBITS number of bits of image width
// ybits ... (u8) SLAYER_YBITS number of bits of image height
// w ... (u16) SLAYER_W destination width
// h ... (u16) SLAYER_H destination height

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

// extern "C" void RenderPersp(u8* dbuf, int y, sLayer* scr)

// render layers with transformatio matrix LAYERMODE_PERSP*
//  R0 ... dbuf pointer to data buffer
//  R1 ... y coordinate of scanline (relative in destination image)
//  R2 ... scr pointer to layer screen structure sLayer

.thumb_func
.global RenderPersp
RenderPersp:

	// push registers
	push	{r4-r7,lr}

// Stack content and input variables:
//  R0 dbuf pointer to data buffer
//  R1 Y coordinate of scanline
//  R2 scr pointer to layer screen structure sLayer
//  R3
//  SP+0: R4
//  SP+4: R5
//  SP+8: R6
//  SP+12: R7
//  SP+16: LR

// R0 ... pointer to destination data buffer
// R1 ... Y coordinate
// R2 ... sLayer

	// load horizon offset -> R4, check if use perspective
	ldr	r6,RenderPersp_pSioBase // get address of SIO base -> R6
	ldrh	r5,[r2,#SLAYER_H] // get destination height -> R5
	ldrb	r4,[r2,#SLAYER_HORIZ] // get horizon offset -> R4
	sxtb	r4,r4		// signed extension
	lsls	r4,#2		// horizon * 4, horizon = 0 ?
	bne	2f		// use perspective

	// not using perspective, start Y coordinate y0 = y - h/2 -> R12
	lsrs	r5,#1		// destination height/2 -> R5
	subs	r1,r5		// y - h/2 -> R1
	mov	r12,r1		// current coordinate Y0 = y - h/2 -> R12

	// prepare divide result to get 1<<FRACT
	movs	r5,#1		// R5 <- 1
	str	r5,[r6,#SIO_DIV_UDIVISOR_OFFSET] // divisor = 1
	lsls	r5,#FRACT	// constant 1<<FRACT -> R5
	str	r5,[r6,#SIO_DIV_UDIVIDEND_OFFSET] // dividend = FRACTMUL
	b	4f

	// using perspective, check ceilling mode
2:	bpl	3f		// horizon is not negative
	subs	r1,r5,r1	// negate, y = h - y
	subs	r1,#1		// y = h - 1 - y
	negs	r4,r4		// absolute value of horizon

	// prepare current coordinate Y0 = y - h -> R12
3:	subs	r7,r1,r5	// y - h = current Y coordinate -> R7
	mov	r12,r7		// store current coordinate Y0 -> R12

	// start calculating distance coefficient dist = FRACTMUL*h/(y + horiz)
	lsls	r5,#FRACT	// destination height * FRACTMUL -> R5
	str	r5,[r6,#SIO_DIV_UDIVIDEND_OFFSET] // store dividend, FRACTMUL*h
	adds	r1,r4		// horizon + y -> R2
	str	r1,[r6,#SIO_DIV_UDIVISOR_OFFSET] // store divisor, y + horiz

// R0 ... pointer to destination data buffer
// R2 ... sLayer
// R12 ... current coordinate Y0

	// prepare start coordinate X0 = -w/2 -> LR
4:	ldrh	r3,[r2,#SLAYER_W] // get destination width -> R3
	lsrs	r5,r3,#1	// width/2
	negs	r5,r5		// negate
	mov	lr,r5		// store start coordinate X0 -> LR

	// prepare number of 4-pixels (loop counter) -> R7
	lsrs	r7,r3,#2	// width/4 -> R7

	// get number of bits of image width "xbits" -> R1
	ldrb	r1,[r2,#SLAYER_XBITS] // number of bits of image width -> R1

	// get number of bits of image height "ybits" -> R4
	ldrb	r4,[r2,#SLAYER_YBITS] // number of bits of image height -> R4

	// prepare address of interpolator base -> R3
	ldr	r3,RenderPersp_Interp // get address of interpolator base -> R3

// R0 ... pointer to destination data buffer
// R1 ... number of bits of image width xbits
// R2 ... sLayer
// R3 ... interpolator base
// R4 ... number of bits of image height ybits
// R7 ... width/4
// LR ... start coordinate X0
// R12 ... current coordinate Y0

// ---- setup interpolator

	// set image base to base2
	ldr	r6,[r2,#SLAYER_IMG]	// load image base
	str	r6,[r3,#BASE2_OFFSET]	// set image base

	// set control word of lane 1 - add raw lane base back to accumulator, shift "FRACT-xbits", mask xbits...xbits+ybits-1
	ldr	r6,RenderPersp_Ctrl	// load control word
	subs	r6,r1			// FRACT - xbits (SIO_INTERP0_CTRL_LANE0_SHIFT_LSB = 0, no shift required)
	lsls	r5,r1,#SIO_INTERP1_CTRL_LANE0_MASK_LSB_LSB // shift xbits to mask LSB position -> R5
	orrs	r6,r5			// add xbits to control word
	subs	r1,#1			// xbits - 1 -> R1
	adds	r5,r1,r4		// xbits-1+ybits -> R5
	lsls	r5,#SIO_INTERP1_CTRL_LANE0_MASK_MSB_LSB // shift to MSB mask position 
	orrs	r6,r5			// add to control word
	str	r6,[r3,#CTRL_LANE1_OFFSET] // set control word of lane 1

// R0 ... pointer to destination data buffer
// R1 ... image width xbits-1
// R2 ... sLayer
// R3 ... interpolator base
// R7 ... width/4
// LR ... start coordinate X0
// R12 ... current coordinate Y0

	// set control word of lane 0 - add raw lane base back to accumulator, shift "FRACT", mask 0..xbits-1
	ldr	r6,RenderPersp_Ctrl	// load control word
	lsls	r1,#SIO_INTERP1_CTRL_LANE0_MASK_MSB_LSB // shift xbits-1 to mask MSB position
	orrs	r6,r1			// add to control word
	str	r6,[r3,#CTRL_LANE0_OFFSET] // set control word of lane 0

// R0 ... pointer to destination data buffer
// R2 ... sLayer
// R3 ... interpolator base
// R7 ... width/4
// LR ... start coordinate X0
// R12 ... current coordinate Y0

// ---- set matrix

	// get pointer to matrix -> R4
	ldr	r4,[r2,#SLAYER_PAR]	// get pointer to matrix -> R4

	// get distance coefficient dist -> R1
	ldr	r1,RenderPersp_pSioBase // get address of SIO base -> R1
	ldr	r1,[r1,#SIO_DIV_QUOTIENT_OFFSET] // get quotient-> R1, distance coefficient

// r4+0 ... m11
// r4+4 ... m12
// r4+8 ... m13
// r4+12 ... m21
// r4+16 ... m22
// r4+20 ... m23

	// set m11 -> R5 base0
	ldr	r5,[r4,#0]	// load m11
	muls	r5,r1		// m11*dist
	asrs	r5,#FRACT	// (m11*dist)>>FRACT
	str	r5,[r3,#BASE0_OFFSET] // set base0

	// set m21 -> R6 base1
	ldr	r6,[r4,#12]	// load m21
	muls	r6,r1		// m21*dist
	asrs	r6,#FRACT	// (m21*dist)>>FRACT
	str	r6,[r3,#BASE1_OFFSET] // set base1

// R0 ... pointer to destination data buffer
// R1 ... distance coefficient
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
	mov	lr,r1		// save distance coefficient -> LR
	ldr	r6,[r4,#4]	// load m12 -> R6
	muls	r1,r6		// m12*dist -> R1
	asrs	r1,#FRACT	// (m12*dist)>>FRACT -> R1
	mov	r6,r12		// load coordinate Y0 -> R6
	muls	r1,r6		// y0*m12 -> R1
	adds	r5,r1		// x0*m11 + y0*m12 -> R5
	ldr	r1,[r4,#8]	// load m13 -> R1
	adds	r5,r1		// x0*m11 + y0*m12 + m13 -> R5
	str	r5,[r3,#ACCUM0_OFFSET] // set accum0

// R0 ... pointer to destination data buffer
// R2 ... x0*m21
// R3 ... interpolator base
// R4 ... pointer to matrix
// R6 ... current coordinate Y0
// R7 ... width/4
// LR ... distance coefficient

	// set x0*m21 + y0*m22 + m23 -> accum1
	ldr	r1,[r4,#16]	// load m22 -> R1
	mov	r5,lr		// distance coefficient -> R5
	muls	r1,r5		// m22*dist
	asrs	r1,#FRACT	// (m22*dist)>>FRACT -> R1
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
// pointer to SIO base
RenderPersp_pSioBase:
	.word	SIO_BASE	// addres of SIO base

// pointer to Interp1 base
RenderPersp_Interp:
	.word	SIO_BASE+SIO_INTERP1_ACCUM0_OFFSET // addres of interpolator base

RenderPersp_Ctrl:		// lane control word
	.word	SIO_INTERP1_CTRL_LANE0_ADD_RAW_BITS | (FRACT<<SIO_INTERP1_CTRL_LANE0_SHIFT_LSB)
