
// ****************************************************************************
//
//                       VGA render LAYERMODE_SPRITE*
//
// ****************************************************************************

#include "../define.h"		// common definitions of C and ASM

	.syntax unified
	.section .time_critical.Render, "ax"
	.cpu cortex-m0plus
	.thumb			// use 16-bit instructions

// extern "C" void RenderSprite(u8* dbuf, int y, sLayer* scr)

// render layers with sprites LAYERMODE_SPRITE*
//  R0 ... dbuf pointer to data buffer
//  R1 ... y coordinate of scanline
//  R2 ... scr pointer to layer screen structure sLayer

.thumb_func
.global RenderSprite
RenderSprite:

	// push registers
	push	{r4-r7,lr}

// Stack content and input variables:
//  R0 dbuf pointer to data buffer
//  R1 Y coordinate of scanline
//  R2 scr pointer to layer screen structure sLayer, later: num number of sprites
//  R3
//  SP+0: R4
//  SP+4: R5
//  SP+8: R6
//  SP+12: R7
//  SP+16: LR

// Variables:
//  R0 ... dbuf pointer to data buffer, later: dbuf[x] destination address
//  R1 ... Y coordinate of scanline, later: Y2 coordinate relative to sprite base, later: s->img[Y2*WB+X2] address of sprite line
//  R2 ... num number of sprites (loop counter), later: W2 width of sprite segment
//  R3 ... s pointer to current sprite, later: col key color
//  R4 ... (temporary), later: absolute X coordinate of start of line
//  R5 ... relative X2 coordinate of sprite segment
//  R6 ... W layer screen width
//  R7 ... spr pointer to list of sprites
//  LR

	// load pointer to list of sprites -> R7
	ldr	r7,[r2,#SLAYER_IMG]

	// load screen width -> R6
	ldrh	r6,[r2,#SLAYER_W]

	// load number of sprites -> R2
	ldrh	r2,[r2,#SLAYER_SPRITENUM]

	// count number of sprites, end if num = 0
2:	subs	r2,#1		// decrement number of sprites
	blo	9f		// no other sprites

//  R0 ... dbuf pointer to data buffer
//  R1 ... Y coordinate of scanline
//  R2 ... num number of sprites (loop counter)
//  R3 ...
//  R4 ... 
//  R5 ... 
//  R6 ... W layer screen width
//  R7 ... spr pointer to list of sprites

	// push registers
	push	{r0-r2}		// push resiters R0..R2

	// get pointer to next sprite -> R3
	ldmia	r7!,{r3}	// pointer to sprite -> R3
//  R3 ... s pointer to current sprite

	// prepare Y2 coordinate relative to sprite base -> R1
	ldrh	r4,[r3,#SSPRITE_Y] // get Y coordinate of the sprite -> R4
	sxth	r4,r4		// signed extend Y2
	subs	r1,r1,r4	// relative coordinate Y2 = Y - s->y
//  R1 ... Y2 coordinate relative to sprite base

	// check if Y2 coordinate is valid
	bmi	8f		// Y2 < 0, go next sprite
	ldrh	r4,[r3,#SSPRITE_H] // get sprite height
	cmp	r1,r4		// check sprite height
	bge	8f		// Y2 >= s->h, go next sprite

	// prepare relative start X2 coordinate of this line segment -> R5
	ldr	r4,[r3,#SSPRITE_X0] // get table of X0 of lines
	ldrb	r5,[r4,r1]	// get X2 coordinate s->x0[y2] -> R5
	// lsls	r5,#2		// convert X2 coordinate to byte offset
//  R5 ... relative X2 coordinate of sprite segment

	// get width W2 of this line segment -> R2
	ldr	r4,[r3,#SSPRITE_W0] // get table of W0 of lines
	ldrb	r2,[r4,r1]	// get W2 width s->w0[y2] -> R2
	// lsls	r2,#2		// convert W2 width to bytes
//  R2 ... W2 width of sprite segment

	// get address of sprite line s->img[Y2*s->wb] -> R1
	ldrh	r4,[r3,#SSPRITE_WB] // get sprite pitch w->wb
	muls	r1,r1,r4	// sprite offset Y2*s->wb
	ldr	r4,[r3,#SSPRITE_IMG] // get sprite image
	add	r1,r4		// line address -> R1
//  R1 ... s->img[Y2*WB] address of sprite line
	
	// get absolute X coordinate of start of line -> R4
	ldrh	r4,[r3,#SSPRITE_X] // get sprite X coordinate -> R4
	sxth	r4,r4		// signed extend X
//  R4 ... absolute X coordinate of start of line

	// get key color -> R3
	ldrb	r3,[r3,#SSPRITE_KEYCOL] // get key color -> R3
//  R3 ... col key color

	// check if X coordinate >= 0
	adds	r4,r4,r5	// s->X + X2, X coordinate of start of line -> R4
	bpl	3f		// X >= 0, sprite does not lie below start

	// sprite correction
	subs	r5,r4		// X2 -= X
	adds	r2,r4		// W2 += X
	movs	r4,#0		// X = 0

	// shift source address -> R1
3:	adds	r1,r5		// add X2
//  R1 ... s->img[Y2*WB+X2] address of sprite line
//  R5 ... 

	// check line length W2
	subs	r5,r6,r4	// W - X -> R5
	cmp	r2,r5		// compare W2 with W - X
	ble	4f		// W2 <= W - X, length is OK
	mov	r2,r5		// limit segment width W2 -> R2

	// check width W2
4:	tst	r2,r2		// check W2
	ble	8f		// no W2 left (W2 <= 0)

	// shift destination address
	adds	r0,r4

//  R0 ... dbuf pointer to data buffer
//  R1 ... s->img[Y2*WB+X2] address of sprite line
//  R2 ... W2 width of sprite segment
//  R3 ... col key color
//  R4 ... 
//  R5 ... 
//  R6 ... W layer screen width
//  R7 ... spr pointer to list of sprites

	// blit sprite line
	bl	BlitKey		// blit sprite line

	// pop registers and continue loop
8:	pop	{r0-r2}		// pop registers R0..R2
	b	2b		// continue loop

	// pop registers and return
9:	pop	{r4-r7,pc}
