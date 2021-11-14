
// ****************************************************************************
//
//                              VGA render GF_TILE2
//
// ****************************************************************************
// u16	par3;	// SSEGM_PAR3 tile width (must be multiple of 4)
// u32	par;	// SSEGM_PAR tile table with one column of tiles
// u32	par2;	// SSEGM_PAR2 LOW tile height, HIGH tile width bytes

#include "../define.h"		// common definitions of C and ASM
#include "hardware/regs/sio.h"	// registers of hardware divider
#include "hardware/regs/addressmap.h" // SIO base address

	.syntax unified
	.section .time_critical.Render, "ax"
	.cpu cortex-m0plus
	.thumb			// use 16-bit instructions

// extern "C" u32* RenderTile2(u32* cbuf, int x, int y, int w, sSegm* segm);

// render tiles GF_TILE2
//   cbuf ... destination control buffer
//   x ... start X coordinate (must be multiple of 4)
//   y ... start Y coordinate 
//   w ... width of this segment (must be multiple of 4)
//   segm ... video segment
// Output new cbuf pointer.
// 320 pixels takes on 151 MHz: tiles 8x8 3.5 us, tile 16x16 2 us, tiles 32x32 1.3 us, tiles 64x64 0.9 us.

.thumb_func
.global RenderTile2
RenderTile2:

	// push registers
	push	{r2-r7,lr}

// Input registers and stack content:
//  R0 ... destination control buffer
//  R1 ... X coordinate
//  SP+0: R2 ... Y coordinate
//  SP+4: R3 ... width to display
//  SP+8: R4
//  SP+12: R5
//  SP+16: R6
//  SP+20: R7
//  SP+24: LR
//  SP+28: video segment

	// get pointer to video segment -> R4
	ldr	r4,[sp,#28]	// load video segment -> R4

//  R0 ... pointer to destination control buffer
//  R1 ... X coordinate
//  R2 ... Y coordinate
//  R3 ... remaining width
//  R4 ... sSegm*

	// start divide Y/tile_height
	ldr	r5,RenderTile_pSioBase // get address of SIO base -> R5
	str	r2,[r5,#SIO_DIV_UDIVIDEND_OFFSET] // store dividend, Y coordinate
	ldrh	r2,[r4,#SSEGM_PAR2] // tile height -> R2
	str	r2,[r5,#SIO_DIV_UDIVISOR_OFFSET] // store divisor, tile height

// - now we must wait at least 8 clock cycles to get result of division

//  R0 ... pointer to destination control buffer
//  R1 ... X coordinate
//  R3 ... remaining width
//  R4 ... sSegm*
//  R5 ... SIO_BASE

	// [6] get wrap width -> [SP+0]
	ldrh	r7,[r4,#SSEGM_WRAPX] // [2] get wrap width
	movs	r6,#3		// [1] mask to align to 32-bit
	bics	r7,r6		// [1] align wrap
	str	r7,[sp,#0]	// [2] save wrap width

//  R0 ... pointer to destination control buffer
//  R1 ... X coordinate
//  R3 ... remaining width
//  R4 ... sSegm*
//  R5 ... SIO_BASE
//  R6 ... align mask #3
//  [SP+0] ... wrap width

	// [1] align X coordinate to 32-bit -> R1
	bics	r1,r6		// [1] align X

//  R0 ... pointer to destination control buffer
//  R1 ... X coordinate
//  R3 ... remaining width
//  R4 ... sSegm*
//  R5 ... SIO_BASE
//  R6 ... align mask #3
//  [SP+0] ... wrap width

	// [3] align remaining width -> [SP+4]
	bics	r3,r6		// [1] align width
	str	r3,[sp,#4]	// [2] store aligned width to [SP+4]

//  R0 ... pointer to destination control buffer
//  R1 ... X coordinate
//  R4 ... sSegm*
//  R5 ... SIO_BASE
//  [SP+0] ... wrap width
//  [SP+4] ... remaining width

	// [2] prepare tile width -> R3
	ldrh	r3,[r4,#SSEGM_PAR3] // [2] get tile width -> R3

//  R0 ... pointer to destination control buffer
//  R1 ... X coordinate
//  R3 ... tile width
//  R4 ... sSegm*
//  R5 ... SIO_BASE
//  [SP+0] ... wrap width
//  [SP+4] ... remaining width

	// load result of division Y/tile_height -> R6 Y relative at row, R7 Y row
	//  Note: QUOTIENT must be read last
	ldr	r6,[r5,#SIO_DIV_REMAINDER_OFFSET] // get remainder of result -> R6, Y coordinate relative to current row
	ldr	r7,[r5,#SIO_DIV_QUOTIENT_OFFSET] // get quotient-> R7, index of row

//  R0 ... pointer to destination control buffer
//  R1 ... X coordinate
//  R3 ... tile width
//  R4 ... sSegm*
//  R5 ... SIO_BASE
//  R6 ... Y relative at row
//  R7 ... Y row index
//  [SP+0] ... wrap width
//  [SP+4] ... remaining width

	// start divide X/tile_width
	str	r1,[r5,#SIO_DIV_UDIVIDEND_OFFSET] // store dividend, X coordinate
	str	r3,[r5,#SIO_DIV_UDIVISOR_OFFSET] // store divisor, tile width

// - now we must wait at least 8 clock cycles to get result of division

//  R0 ... pointer to destination control buffer
//  R1 ... X coordinate
//  R3 ... tile width
//  R4 ... sSegm*
//  R5 ... SIO_BASE
//  R6 ... Y relative at row
//  R7 ... Y row index
//  [SP+0] ... wrap width
//  [SP+4] ... remaining width

	// [7] base pointer to source data buffer (without X) -> LR, R7
	ldrh	r2,[r4,#SSEGM_WB] // [2] get pitch of rows -> R2
	muls	r7,r2		// [1] pitch * row (Y * WB) -> offset of row in data buffer
	ldr	r2,[r4,#SSEGM_DATA] // [2] pointer to data -> R2
	adds	r7,r2		// [1] base address of data buffer
	mov	lr,r7		// [1] save base address

//  R0 ... pointer to destination control buffer
//  R1 ... X coordinate
//  R3 ... tile width 
//  R4 ... sSegm*
//  R5 ... SIO_BASE
//  R6 ... Y relative at row
//  R7 ... base address of data buffer (without X)
//  LR ... base address of data buffer (without X)
//  [SP+0] ... wrap width
//  [SP+4] ... remaining width

	// [6] tile base address -> R4
	ldrh	r2,[r4,#SSEGM_PAR2+2] // [2] tile width bytes -> R2
	muls	r6,r2		// [1] tile width bytes * Y relative to row -> tile line offset R6
	ldr	r4,[r4,#SSEGM_PAR] // [2] pointer to tiles
	adds	r4,r6		// [1] tile base address -> R4

//  R0 ... pointer to destination control buffer
//  R1 ... X coordinate
//  R3 ... tile width
//  R4 ... tile base address
//  R5 ... SIO_BASE
//  R7 ... base address of data buffer (without X)
//  LR ... base address of data buffer (without X)
//  [SP+0] ... wrap width
//  [SP+4] ... remaining width

	// load result of division X/tile_width -> R6 X pixel relative, R5 tile position
	//  Note: QUOTIENT must be read last
	ldr	r6,[r5,#SIO_DIV_REMAINDER_OFFSET] // get remainder of result -> R6, X pixel relative in tile
	ldr	r5,[r5,#SIO_DIV_QUOTIENT_OFFSET] // get quotient-> R5, tile position

//  R0 ... pointer to destination control buffer
//  R1 ... X coordinate
//  R3 ... tile width
//  R4 ... tile base address
//  R5 ... tile position
//  R6 ... X pixel relative in tile
//  R7 ... base address of data buffer (without X)
//  LR ... base address of data buffer (without X)
//  [SP+0] ... wrap width
//  [SP+4] ... remaining width

	// prepare current pointer to source data buffer with X -> R7
	adds	r7,r5		// tile source address -> R7

//  R0 ... pointer to destination control buffer
//  R1 ... X coordinate
//  R3 ... tile width
//  R4 ... tile base address
//  R6 ... X pixel relative in tile
//  R7 ... pointer to source data buffer (with X)
//  LR ... base address of data buffer (without X)
//  [SP+0] ... wrap width
//  [SP+4] ... remaining width

// ---- render rest of first tile

	// check if X is tile-aligned
	tst	r6,r6		// check tile align
	beq	2f		// X is tile aligned

	// shift X coordinate
	subs	r5,r3,r6	// pixels remain in current tile -> R5
	adds	r1,r5		// shift X coordinate (align to next tile)

	// shift remaining width
	ldr	r2,[sp,#4]	// get remaining width
	subs	r2,r5		// shift width
	str	r2,[sp,#4]	// store remaining width

	// write number of 4-pixels
	lsrs	r5,#2		// number of 4-pixels
	stmia	r0!,{r5}	// save width

	// load tile index -> R2
	ldrb	r2,[r7,#0]	// [2] load tile index
	adds	r7,#1		// [1] increase tile address

	// write tile addres
	muls	r2,r3		// tile index * tile width = tile offset
	add	r2,r4		// [1] add tile base address
	add	r2,r6		// [1] shift to tile start
	stmia	r0!,{r2}	// [3] save pointer

	// check end of segment
	ldr	r2,[sp,#0]	// get wrap width
	cmp	r1,r2		// check end of segment
	blo	2f		// not end of segment
	movs	r1,#0		// reset X coordinate
	mov	r7,lr		// get base pointer to tile data

	// prepare wrap width - start X -> R5
2:	ldr	r2,[sp,#0]	// get wrap width
	subs	r5,r2,r1	// pixels remaining to end of segment
	ldr	r2,[sp,#4]	// total remaining width -> R3

// ---- start outer loop, render one part of segment
//  R0 ... pointer to destination control buffer
//  R2 ... total remaining width
//  R3 ... tile width
//  R4 ... tile base address
//  R5 ... wrap width of this segment
//  R7 ... pointer to source data buffer
//  LR ... base address of data buffer (without X)
//  [SP+0] ... wrap width

RenderTile_OutLoop:

	// limit wrap width by total width -> R5
	cmp	r5,r2		// compare wrap width with total width
	bls	2f		// width is OK
	mov	r5,r2		// limit wrap width

	// check if remain whole tile
2:	cmp	r5,r3		// check number of remaining pixels
	bhs	5f		// remain whole tiles

	// check if start of last tile remains
	cmp	r5,#4		// check start of last tile
	blo	3f		// all done
	mov	r1,r5		// width to render

// ---- render start of last tile
//  R0 ... pointer to destination control buffer
//  R1 ... width to render in this segment
//  R2 ... total remaining width
//  R3 ... tile width
//  R4 ... tile base address
//  R5 ... wrap width of this segment
//  R7 ... pointer to source data buffer (with X)
//  LR ... base address of data buffer (without X)
//  [SP+0] ... wrap width

RenderTile_Last:

	// save width
	lsrs	r6,r1,#2	// number of 4-pixels
	stmia	r0!,{r6}	// save width

	// load tile index -> R6
	ldrb	r6,[r7,#0]	// [2] load tile index
	adds	r7,#1		// [1] increase tile index

	// save tile addres
	muls	r6,r3		// multiply tile index * tile width
	add	r6,r4		// [1] add tile base address
	stmia	r0!,{r6}	// [3] save pointer

	// check if continue with next segment
	mov	r7,lr		// get base pointer to tile data
	cmp	r5,r3		// whole tile remains?
	bhs	RenderTile_OutLoop // render next segment

	// pop registers and return
3:	pop	{r2-r7,pc}

// ---- prepare to render whole tiles
//  R0 ... pointer to destination control buffer
//  R2 ... total remaining width
//  R3 ... tile width
//  R4 ... tile base address
//  R5 ... width of this segment
//  R7 ... pointer to source data buffer (with X)
//  LR ... base address of data buffer (without X)
//  [SP+0] ... wrap width

	// prepare number of 4-pixels to render -> R1
5:  	lsrs	r1,r5,#2	// shift to get number of tiles in multiply of 4-pixels -> R1
	lsls	r5,r1,#2	// shift back to get number of pixels, rounded down -> R5
	subs	r2,r5		// update remaining width -> R2

	lsrs	r5,r3,#2	// tile width/4 -> R5
	subs	r1,r5		// number of 4-pixels - width/4
	adds	r1,#1		// number of 4-pixels - (width/4-1)

// ---- [11*N-1] start inner loop, render in one part of segment
//  R0 ... pointer to destination control buffer
//  R1 ... number of 4-pixels to generate - 1 (loop counter)
//  R2 ... total remaining width
//  R3 ... tile width
//  R4 ... tile base address
//  R5 ... tile width/4
//  R7 ... pointer to source data buffer (with X)
//  LR ... base address of data buffer (without X)
//  [SP+0] ... wrap width

RenderTile_InLoop:

	// [3] load tile index -> R6
	ldrb	r6,[r7,#0]	// [2] load tile index
	adds	r7,#1		// [1] increase tile index

	// [2] get tile addres
	muls	r6,r3		// [1] multiply tile index * tile width
	add	r6,r4		// [1] add tile base address
	
	// [3] save control block
	stmia	r0!,{r5,r6}	// [3] save width and pointer

	// [2,3] loop
	subs	r1,r5		// [1] shift loop counter, subtract tile width/4
	bhi	RenderTile_InLoop // [1,2] > 0, render next whole tile

// ---- end inner loop, continue with last tile, or start new part

	// continue to outer loop
	adds	r1,r5		// return size of last tile
	subs	r1,#1		// add "tile size/4 - 1"
	ldr	r5,[sp,#0]	// load wrap width -> R5
	lsls	r1,#2		// convert back to pixels
	bne	RenderTile_Last // render start of last tile
	mov	r7,lr		// get base pointer to tile data -> R7
	b	RenderTile_OutLoop // go back to outer loop

	.align 2
// pointer to SIO base
RenderTile_pSioBase:
	.word	SIO_BASE	// addres of SIO base
