
// ****************************************************************************
//
//                              VGA render GF_TILEPERSP
//
// ****************************************************************************
// data ... tile map
// par ... column of tile images
// par2 ... pointer to 6 matrix integer parameters m11,m12..m23 ((int)(m*FRACTMUL))
// par3 ... LOW8=number of bits of tile width and height, HIGH8=horizon offset
// wb ... LOW8=number of bits of tile map width, HIGH8=number of bits of tile map height
// wrapy ... segment height

#include "../define.h"		// common definitions of C and ASM
#include "hardware/regs/sio.h"	// registers of hardware divider
#include "hardware/regs/addressmap.h" // SIO base address

#define ACCUM0_OFFSET0		0
#define ACCUM1_OFFSET0		4
#define BASE0_OFFSET0		8
#define BASE1_OFFSET0		12
#define BASE2_OFFSET0		16
#define POP_LANE0_OFFSET0	20
#define POP_LANE1_OFFSET0	24
#define POP_FULL_OFFSET0	28
#define PEEK_LANE0_OFFSET0	32
#define PEEK_LANE1_OFFSET0	36
#define PEEK_FULL_OFFSET0	40
#define CTRL_LANE0_OFFSET0	44
#define CTRL_LANE1_OFFSET0	48
#define ACCUM0_ADD_OFFSET0	52
#define ACCUM1_ADD_OFFSET0	56
#define BASE_1AND0_OFFSET0	60

#define ACCUM0_OFFSET1		64
#define ACCUM1_OFFSET1		68
#define BASE0_OFFSET1		72
#define BASE1_OFFSET1		76
#define BASE2_OFFSET1		80
#define POP_LANE0_OFFSET1	84
#define POP_LANE1_OFFSET1	88
#define POP_FULL_OFFSET1	92
#define PEEK_LANE0_OFFSET1	96
#define PEEK_LANE1_OFFSET1	100
#define PEEK_FULL_OFFSET1	104
#define CTRL_LANE0_OFFSET1	108
#define CTRL_LANE1_OFFSET1	112
#define ACCUM0_ADD_OFFSET1	116
#define ACCUM1_ADD_OFFSET1	120
#define BASE_1AND0_OFFSET1	124

	.syntax unified
	.section .time_critical.Render, "ax"
	.cpu cortex-m0plus
	.thumb			// use 16-bit instructions

// extern "C" u32* RenderTilePersp(u32* cbuf, int x, int y, int w, sSegm* segm);

// render tiles with perspective GF_TILEPERSP
// using hardware interpolator inter0 and inter1 (their state is not saved during interrup)
//  R0 ... pointer to destination data buffer
//  R1 ... start X coordinate (not used)
//  R2 ... start Y coordinate (in graphics lines)
//  R3 ... width to display (must be multiple of 4)
//  [stack] ... segm video segment sSegm
// Output new pointer to data buffer.
// 320 pixels takes ?? us on 151 MHz.

.thumb_func
.global RenderTilePersp
RenderTilePersp:

// Input registers and stack:
//  R0 ... pointer to destination data buffer
//  R1 ... X coordinate (not used)
//  R2 ... Y coordinate
//  SP+0: R3 ... remaining width
//  SP+4: R4
//  SP+8: R5
//  SP+12: R6
//  SP+16: R7
//  SP+20: LR
//  SP+24: video segment

	// push registers
	push	{r3-r7,lr}

// ---- prepare registers

	// get pointer to video segment -> R4
	ldr	r4,[sp,#24]	// load video segment -> R4

// R0 ... pointer to data buffer
// R2 ... Y coordinate
// R3 ... remaining width
// R4 ... video segment

	// load horizon offset -> R1, check if use perspective
	ldr	r6,RenderTilePersp_pSioBase // get address of SIO base -> R6
	ldrh	r5,[r4,#SSEGM_WRAPY] // get segment height -> R5
	ldrb	r1,[r4,#SSEGM_PAR3+1] // get horizon offset -> R1
	sxtb	r1,r1		// signed extension
	lsls	r1,#2		// horizon * 4, horizon = 0 ?
	bne	2f		// use perspective

	// not using perspective, start Y coordinate y0 = y - h/2 -> R12
	lsrs	r5,#1		// segment height/2 -> R5
	subs	r2,r5		// y - h/2 -> R2
	mov	r12,r2		// current coordinate Y0 = y - h/2 -> R12

	// prepare divide result to get 1<<FRACT
	movs	r5,#1		// R5 <- 1
	str	r5,[r6,#SIO_DIV_UDIVISOR_OFFSET] // divisor = 1
	lsls	r5,#FRACT	// constant 1<<FRACT -> R5
	str	r5,[r6,#SIO_DIV_UDIVIDEND_OFFSET] // dividend = FRACTMUL
	b	4f

	// using perspective, check ceilling mode
2:	bpl	3f		// horizon is not negative
	subs	r2,r5,r2	// negate, y = h - y
	subs	r2,#1		// y = h - 1 - y
	negs	r1,r1		// absolute value of horizon

	// prepare current coordinate Y0 = y - h -> R12
3:	subs	r7,r2,r5	// y - h = current Y coordinate -> R7
	mov	r12,r7		// store current coordinate Y0 -> R12

	// start calculating distance coefficient dist = FRACTMUL*h/(y + horiz)
	lsls	r5,#FRACT	// segment height * FRACTMUL -> R5
	str	r5,[r6,#SIO_DIV_UDIVIDEND_OFFSET] // store dividend, FRACTMUL*h
	adds	r2,r1		// horizon + y -> R2
	str	r2,[r6,#SIO_DIV_UDIVISOR_OFFSET] // store divisor, y + horiz

// R0 ... pointer to data buffer
// R3 ... remaining width
// R4 ... video segment
// R12 ... current coordinate Y0

	// prepare start coordinate X0 = -w/2 -> LR
4:	lsrs	r5,r3,#1	// width/2
	negs	r5,r5		// negate
	mov	lr,r5		// store start coordinate X0 -> LR

	// prepare number of 4-pixels (loop counter) -> R7
	lsrs	r7,r3,#2	// width/4 -> R7

	// prepare address of interpolator 0 base -> R3
	ldr	r3,RenderTilePersp_Interp // get address of interpolator 0 base -> R3

// R0 ... pointer to data buffer
// R3 ... interpolator base
// R4 ... video segment
// R7 ... width/4
// LR ... start coordinate X0
// R12 ... current coordinate Y0

// ---- setup interpolator 0 to get tile index

	// set tile map base to base2
	ldr	r6,[r4,#SSEGM_DATA]	// load tile map base
	str	r6,[r3,#BASE2_OFFSET0]	// set tile map base

	// set control word of lane 0: shift=FRACT+tilebits, mask=0..mapwbits-1
	ldr	r6,RenderTilePersp_Ctrl // load control word
	ldrb	r1,[r4,#SSEGM_PAR3]	// get tile width and height -> R1
	str	r1,[sp,#0]		// save tile size -> [SP+0]
	adds	r6,r1			// FRACT + tilebits (SIO_INTERP0_CTRL_LANE0_SHIFT_LSB = 0, no shift required)
	ldrb	r2,[r4,#SSEGM_WB]	// number of bits of tile map width mapwbits -> R2
	subs	r5,r2,#1		// mapwbits - 1
	lsls	r5,#SIO_INTERP0_CTRL_LANE0_MASK_MSB_LSB // shift to mask MSB position
	orrs	r6,r5			// add to control word
	str	r6,[r3,#CTRL_LANE0_OFFSET0] // set control word of lane 0

	// set control word of lane 1: shift=FRACT+tilebits-mapwbits,
	//  mask=mapwbits..mapwbits+maphbits-1
	subs	r6,r2			// FRACT + tilebits - mapwbits
	lsls	r2,#SIO_INTERP0_CTRL_LANE0_MASK_LSB_LSB // shift mapwbits to mask LSB position
	orrs	r6,r2			// add mapwbits to control word
	ldrb	r2,[r4,#SSEGM_WB+1]	// number of bits of tile map height maphbits -> R2
	lsls	r2,#SIO_INTERP0_CTRL_LANE0_MASK_MSB_LSB // shift maphbits to mask MSB position
	adds	r6,r2			// add to control word
	str	r6,[r3,#CTRL_LANE1_OFFSET0] // set control word of lane 1

// ---- setup interpolator 1 to get pixel index

	// set tile image to base2
	ldr	r6,[r4,#SSEGM_PAR]	// load tile image base
	str	r6,[r3,#BASE2_OFFSET1]	// set tile image base

	// set control word of lane 0: shift=FRACT, mask=0..tilebits-1
	ldr	r6,RenderTilePersp_Ctrl // load control word
	subs	r5,r1,#1		// tilebits - 1
	lsls	r5,#SIO_INTERP1_CTRL_LANE0_MASK_MSB_LSB // shift to mask MSB position
	orrs	r6,r5			// add to control word
	str	r6,[r3,#CTRL_LANE0_OFFSET1] // set control word of lane 0

	// set control word of lane 1: shift=FRACT-tilebits, mask=tilebits..tilebits*2-1
	subs	r6,r1			// FRACT - tilebits
	lsls	r5,r1,#SIO_INTERP1_CTRL_LANE0_MASK_LSB_LSB // shift to mask LSB position
	orrs	r6,r5			// add tilebits to control word
	lsls	r1,#SIO_INTERP1_CTRL_LANE0_MASK_MSB_LSB // shift tilebits to mask MSB position
	adds	r6,r1			// add to control word
	str	r6,[r3,#CTRL_LANE1_OFFSET1] // set control word of lane 1

// R0 ... pointer to data buffer
// R3 ... interpolator base
// R4 ... video segment
// R7 ... width/4
// LR ... start coordinate X0
// R12 ... current coordinate Y0
// [SP+0] ... number of bits of tile width and height

// ---- set matrix

	// get pointer to matrix -> R4
	ldr	r4,[r4,#SSEGM_PAR2]	// get pointer to matrix -> R4

	// get distance coefficient dist -> R1
	ldr	r1,RenderTilePersp_pSioBase // get address of SIO base -> R1
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
	str	r5,[r3,#BASE0_OFFSET0] // set base0
	str	r5,[r3,#BASE0_OFFSET1] // set base0

	// set m21 -> R6 base1
	ldr	r6,[r4,#12]	// load m21
	muls	r6,r1		// m21*dist
	asrs	r6,#FRACT	// (m21*dist)>>FRACT
	str	r6,[r3,#BASE1_OFFSET0] // set base1
	str	r6,[r3,#BASE1_OFFSET1] // set base1

// R0 ... pointer to data buffer
// R1 ... distance coefficient
// R3 ... interpolator base
// R4 ... pointer to matrix
// R5 ... m11
// R6 ... m21
// R7 ... width/4
// LR ... start coordinate X0
// R12 ... current coordinate Y0
// [SP+0] ... number of bits of tile width and height

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
	str	r5,[r3,#ACCUM0_OFFSET0] // set accum0
	str	r5,[r3,#ACCUM0_OFFSET1] // set accum0

// R0 ... pointer to data buffer
// R2 ... x0*m21
// R3 ... interpolator base
// R4 ... pointer to matrix
// R6 ... current coordinate Y0
// R7 ... width/4
// LR ... distance coefficient
// [SP+0] ... number of bits of tile width and height

	// set x0*m21 + y0*m22 + m23 -> accum1
	ldr	r1,[r4,#16]	// load m22 -> R1
	mov	r5,lr		// distance coefficient -> R5
	muls	r1,r5		// m22*dist
	asrs	r1,#FRACT	// (m22*dist)>>FRACT -> R1
	muls	r1,r6		// y0*m22 -> R1
	adds	r2,r1		// x0*m21 + y0*m22 -> R2
	ldr	r1,[r4,#20]	// load m23 -> R1
	adds	r2,r1		// x0*m21 + y0*m22 + m23 -> R2
	str	r2,[r3,#ACCUM1_OFFSET0] // set accum1
	str	r2,[r3,#ACCUM1_OFFSET1] // set accum1

// ---- process odd 4-pixel

	// prepare tile bits * 2
	ldr	r6,[sp,#0]	// get tile bits
	lsls	r6,#1		// tile bits * 2

//  R0 ... pointer to destination data buffer
//  R1 ... (temporary - pixel accumulator 1)
//  R2 ... (temporary - pixel accumulator 2)
//  R3 ... interpolator base
//  R4 ... (temporary - get pointer to tile map, load tile index)
//  R5 ... (temporary - get pointer to pixel, load pixel)
//  R6 ... tilebits*2
//  R7 ... width/4 (loop counter)
// [SP+0] ... number of bits of tile width and height

	// check odd 4-pixels
	lsrs	r7,#1		// width/4/2
	bcc	2f		// no odd 4-pixel

	// [7] load 1st pixel
	ldr	r4,[r3,#POP_FULL_OFFSET0] // [1] get pointer to tile map
	ldrb	r4,[r4,#0]	// [2] load tile index
	lsls	r4,r6		// [1] tile index * tile size
	ldr	r5,[r3,#POP_FULL_OFFSET1] // [1] get pointer to tile image
	ldrb	r1,[r5,r4]	// [2] load pixel

	// [9] load 2nd pixel
	ldr	r4,[r3,#POP_FULL_OFFSET0] // [1] get pointer to tile map
	ldrb	r4,[r4,#0]	// [2] load tile index
	lsls	r4,r6		// [1] tile index * tile size
	ldr	r5,[r3,#POP_FULL_OFFSET1] // [1] get pointer to tile image
	ldrb	r4,[r5,r4]	// [2] load pixel
	lsls	r4,#8		// [1] shift 1 byte left
	orrs	r1,r4		// [1] add pixel to accumulator

	// [9] load 3rd pixel
	ldr	r4,[r3,#POP_FULL_OFFSET0] // [1] get pointer to tile map
	ldrb	r4,[r4,#0]	// [2] load tile index
	lsls	r4,r6		// [1] tile index * tile size
	ldr	r5,[r3,#POP_FULL_OFFSET1] // [1] get pointer to tile image
	ldrb	r4,[r5,r4]	// [2] load pixel
	lsls	r4,#16		// [1] shift 2 bytes left
	orrs	r1,r4		// [1] add pixel to accumulator

	// [9] load 4th pixel
	ldr	r4,[r3,#POP_FULL_OFFSET0] // [1] get pointer to tile map
	ldrb	r4,[r4,#0]	// [2] load tile index
	lsls	r4,r6		// [1] tile index * tile size
	ldr	r5,[r3,#POP_FULL_OFFSET1] // [1] get pointer to tile image
	ldrb	r4,[r5,r4]	// [2] load pixel
	lsls	r4,#24		// [1] shift 3 bytes left
	orrs	r1,r4		// [1] add pixel to accumulator

	// [2] store 4 pixels
	stmia	r0!,{r1}	// [2] store 4 pixels

	// check number of remaining pixels
2:	tst	r7,r7		// check number of pixels
	beq	8f		// end

// ---- [74 per 8 pixels] inner loop
//  R0 ... pointer to destination data buffer
//  R1 ... (temporary - pixel accumulator 1)
//  R2 ... (temporary - pixel accumulator 2)
//  R3 ... interpolator base
//  R4 ... (temporary - get pointer to tile map, load tile index)
//  R5 ... (temporary - get pointer to pixel, load pixel)
//  R6 ... tilebits*2
//  R7 ... width/8 (loop counter)

	// [7] load 1st pixel
6:	ldr	r4,[r3,#POP_FULL_OFFSET0] // [1] get pointer to tile map
	ldrb	r4,[r4,#0]	// [2] load tile index
	lsls	r4,r6		// [1] tile index * tile size
	ldr	r5,[r3,#POP_FULL_OFFSET1] // [1] get pointer to tile image
	ldrb	r1,[r5,r4]	// [2] load pixel

	// [9] load 2nd pixel
	ldr	r4,[r3,#POP_FULL_OFFSET0] // [1] get pointer to tile map
	ldrb	r4,[r4,#0]	// [2] load tile index
	lsls	r4,r6		// [1] tile index * tile size
	ldr	r5,[r3,#POP_FULL_OFFSET1] // [1] get pointer to tile image
	ldrb	r4,[r5,r4]	// [2] load pixel
	lsls	r4,#8		// [1] shift 1 byte left
	orrs	r1,r4		// [1] add pixel to accumulator

	// [9] load 3rd pixel
	ldr	r4,[r3,#POP_FULL_OFFSET0] // [1] get pointer to tile map
	ldrb	r4,[r4,#0]	// [2] load tile index
	lsls	r4,r6		// [1] tile index * tile size
	ldr	r5,[r3,#POP_FULL_OFFSET1] // [1] get pointer to tile image
	ldrb	r4,[r5,r4]	// [2] load pixel
	lsls	r4,#16		// [1] shift 2 bytes left
	orrs	r1,r4		// [1] add pixel to accumulator

	// [9] load 4th pixel
	ldr	r4,[r3,#POP_FULL_OFFSET0] // [1] get pointer to tile map
	ldrb	r4,[r4,#0]	// [2] load tile index
	lsls	r4,r6		// [1] tile index * tile size
	ldr	r5,[r3,#POP_FULL_OFFSET1] // [1] get pointer to tile image
	ldrb	r4,[r5,r4]	// [2] load pixel
	lsls	r4,#24		// [1] shift 3 bytes left
	orrs	r1,r4		// [1] add pixel to accumulator

	// [7] load 1st pixel
	ldr	r4,[r3,#POP_FULL_OFFSET0] // [1] get pointer to tile map
	ldrb	r4,[r4,#0]	// [2] load tile index
	lsls	r4,r6		// [1] tile index * tile size
	ldr	r5,[r3,#POP_FULL_OFFSET1] // [1] get pointer to tile image
	ldrb	r2,[r5,r4]	// [2] load pixel

	// [9] load 2nd pixel
	ldr	r4,[r3,#POP_FULL_OFFSET0] // [1] get pointer to tile map
	ldrb	r4,[r4,#0]	// [2] load tile index
	lsls	r4,r6		// [1] tile index * tile size
	ldr	r5,[r3,#POP_FULL_OFFSET1] // [1] get pointer to tile image
	ldrb	r4,[r5,r4]	// [2] load pixel
	lsls	r4,#8		// [1] shift 1 byte left
	orrs	r2,r4		// [1] add pixel to accumulator

	// [9] load 3rd pixel
	ldr	r4,[r3,#POP_FULL_OFFSET0] // [1] get pointer to tile map
	ldrb	r4,[r4,#0]	// [2] load tile index
	lsls	r4,r6		// [1] tile index * tile size
	ldr	r5,[r3,#POP_FULL_OFFSET1] // [1] get pointer to tile image
	ldrb	r4,[r5,r4]	// [2] load pixel
	lsls	r4,#16		// [1] shift 2 bytes left
	orrs	r2,r4		// [1] add pixel to accumulator

	// [9] load 4th pixel
	ldr	r4,[r3,#POP_FULL_OFFSET0] // [1] get pointer to tile map
	ldrb	r4,[r4,#0]	// [2] load tile index
	lsls	r4,r6		// [1] tile index * tile size
	ldr	r5,[r3,#POP_FULL_OFFSET1] // [1] get pointer to tile image
	ldrb	r4,[r5,r4]	// [2] load pixel
	lsls	r4,#24		// [1] shift 3 bytes left
	orrs	r2,r4		// [1] add pixel to accumulator

	// [3] store 8 pixels
	stmia	r0!,{r1,r2}	// [3] store 8 pixels

	// [2,3] loop counter
	subs	r7,#1		// [1] 8-pixel counter
	bne	6b		// [1,2] next 8-pixels

	// pop registers
8:	pop	{r3-r7,pc}

	.align 2
// pointer to SIO base
RenderTilePersp_pSioBase:
	.word	SIO_BASE	// addres of SIO base

// pointer to Interp0 base
RenderTilePersp_Interp:
	.word	SIO_BASE+SIO_INTERP0_ACCUM0_OFFSET // addres of interpolator 0 base

RenderTilePersp_Ctrl:		// lane control word
	.word	SIO_INTERP0_CTRL_LANE0_ADD_RAW_BITS | (FRACT<<SIO_INTERP0_CTRL_LANE0_SHIFT_LSB)
