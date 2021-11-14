
// ****************************************************************************
//
//                       VGA render LAYERMODE_FASTSPRITE*
//
// ****************************************************************************

#include "../define.h"		// common definitions of C and ASM

	.syntax unified
	.section .time_critical.Render, "ax"
	.cpu cortex-m0plus
	.thumb			// use 16-bit instructions

// extern "C" u32* RenderFastSprite(u32* cbuf, int y, sLayer* scr, u8* buf)

// render layers with fast sprites LAYERMODE_FASTSPRITE*
//  R0 ... cbuf pointer to control buffer
//  R1 ... y coordinate of scanline
//  R2 ... scr pointer to layer screen structure sLayer
//  R3 ... buf pointer to destination data buffer with transparent color
// Output new pointer to control buffer.

.thumb_func
.global RenderFastSprite
RenderFastSprite:

	// push registers
	push	{r1-r7,lr}

// Stack content and input variables:
//  R0 cbuf pointer to control buffer
//  SP+0: R1 Y coordinate of scanline
//  SP+4: R2 scr pointer to layer screen structure sLayer, later: num number of sprites
//  SP+8: R3 buf pointer to data buffer with transparent color
//  SP+12: R4
//  SP+16: R5
//  SP+20: R6
//  SP+24: R7
//  SP+28: LR

// Variables:
//  R0 ... pointer to destination control buffer
//  R1 ... X0 absolute coordinate counted from start
//  R2 ... W layer screen width
//  R3 ... s pointer to current sprite, later: absolute X coordinate of start of sprite
//  R4 ... Y2 coordinate relative to sprite base, later: s->img[Y2*WB] address of sprite line
//  R5 ... relative X2 coordinate of sprite segment
//  R6 ... W2 width of sprite segment
//  R7 ... (temporary)
//  LR ... spr pointer to list of sprites
//  [SP+0] ... (R1) Y coordinate of scanline
//  [SP+4] ... (R2) num number of sprites (loop counter)
//  [SP+8] ... (R3) buf pointer to data buffer with transparent color

	// load pointer to list of sprites -> LR
	ldr	r7,[r2,#SLAYER_IMG]
	mov	lr,r7

	// load number of sprites -> [SP+4]
	ldrh	r7,[r2,#SLAYER_SPRITENUM]
	str	r7,[sp,#4]

	// load screen width -> R2
	ldrh	r2,[r2,#SLAYER_W]

	// reset absolute coordinate X0 -> R1
	movs	r1,#0		// R1 <- 0

	// count number of sprites, end if num = 0
2:	ldr	r7,[sp,#4]	// get number of sprites
	subs	r7,#1		// decrement number of sprites
	blo	8f		// no other sprites
	str	r7,[sp,#4]	// save new number of sprites

	// get pointer to next sprite -> R3
	mov	r7,lr		// pointer to list of sprites -> R7
	ldmia	r7!,{r3}	// pointer to sprite -> R3
	mov	lr,r7		// save new pointer to list of sprites -> LR

	// prepare Y2 coordinate relative to sprite base -> R4
	ldrh	r7,[r3,#SSPRITE_Y] // get Y coordinate of the sprite -> R7
	sxth	r7,r7		// signed extend Y2
	ldr	r4,[sp,#0]	// Y coordinate of scanline -> R4
	subs	r4,r7		// relative coordinate Y2 = Y - s->y

	// check if Y2 coordinate is valid
	bmi	2b		// Y2 < 0, go next sprite
	ldrh	r7,[r3,#SSPRITE_H] // get sprite height
	cmp	r4,r7		// check sprite height
	bge	2b		// Y2 >= s->h, go next sprite

	// get relative start X2 coordinate of this line segment -> R5
	ldr	r7,[r3,#SSPRITE_X0] // get table of X0 of lines
	ldrb	r5,[r7,r4]	// get X2 coordinate -> R5
	lsls	r5,#2		// convert X2 coordinate to byte offset

	// get width W2 of this line segment -> R6
	ldr	r7,[r3,#SSPRITE_W0] // get table of W0 of lines
	ldrb	r6,[r7,r4]	// get W2 width -> R6
	lsls	r6,#2		// convert W2 width to bytes

	// get address of sprite line s->img[Y2*s->wb] -> R4
	ldrh	r7,[r3,#SSPRITE_WB] // get sprite pitch w->wb
	muls	r4,r4,r7	// sprite offset Y2*s->wb
	ldr	r7,[r3,#SSPRITE_IMG] // get sprite image
	add	r4,r7		// line address -> R4
	
	// get absolute X coordinate of start of line -> R3
	ldrh	r3,[r3,#SSPRITE_X] // get sprite X coordinate -> R3
	sxth	r3,r3		// signed extend X
	adds	r3,r3,r5	// s->X + X2, X coordinate of start of line -> R3

	// check if sprite coordinate X lies below current X0 coordinate
	subs	r7,r1,r3	// difference X0 - X -> R7
	ble	3f		// X0 <= X, sprite does not lie below current X0

	// sprite correction
	adds	r5,r7		// X2 += X0 - X
	subs	r6,r7		// W2 -= X0 - X
	mov	r3,r1		// X = X0

	// check line length W2
3:	subs	r7,r2,r3	// W - X -> R7
	cmp	r6,r7		// compare W2 with W - X
	ble	4f		// W2 <= W - X, length is OK
	mov	r6,r7		// limit segment width W2 -> R6

	// align to word
4:	movs	r7,#3		// mask to word
	bics	r3,r7		// align X
	bics	r5,r7		// align X2
	bics	r6,r7		// align W2
	ble	2b		// no W2 left (W2 <= 0)

	// decode space before sprite
	subs	r7,r3,r1	// X - X0 -> R7
	ble	5f		// no space left before sprite
	lsrs	r7,#2		// number of words (X - X0)/4
	stmia	r0!,{r7}	// write number of words
	ldr	r7,[sp,#8]	// pointer to data buffer -> R7
	stmia	r0!,{r7}	// write address
	mov	r1,r3		// shift X0	

	// write sprite line
5:	adds	r7,r4,r5	// address of pixel &s->img[y2*s->wb+x2] -> R7
	lsrs	r4,r6,#2	// W2/4 line length -> R4
	stmia	r0!,{r4,r7}	// write sprite length and address
	adds	r1,r6		// add X0 += W2
	b	2b		// next sprite

	// clear rest of scanline
8:	subs	r2,r1		// subtract W - X0
	bls	9f		// no pixels left
	lsrs	r2,#2		// (W - X0)/4
	ldr	r3,[sp,#8]	// pointer to data buffer -> R3
	stmia	r0!,{r2,r3}	// write number of pixels and address

	// pop registers and return
9:	pop	{r1-r7,pc}
