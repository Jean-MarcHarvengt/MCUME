
// ****************************************************************************
//
//                              VGA sprites
//
// ****************************************************************************
// Takes 100 bytes

#include "define.h"		// common definitions of C and ASM

	.syntax unified
	.section .time_critical.BlitKey, "ax"
	.cpu cortex-m0plus
	.thumb			// use 16-bit instructions

// [6,7] blit macro (4 instructions, 8 bytes)
.macro blitkey n
	ldrb	r4,[r1,#\n]	// [2] load 1 pixel
	cmp	r4,r3		// [1] is it transparent color?
	beq	2f		// [1,2] pixel is transparent
	strb	r4,[r0,#\n]	// [2] write 1 pixel
2:
.endm

// blit scanline using key color
//  dst ... destination buffer
//  src ... source buffer
//  w ... width
//  key ... key color
//extern "C" void BlitKey(u8* dst, u8* src, int w, u8 key);

.thumb_func
.global BlitKey
BlitKey:

	// push registers
	push	{r4,lr}

// Registers:
//  R0 ... destination buffer
//  R1 ... source buffer
//  R2 ... width counter
//  R3 ... key color
//  R4 ... (temporary)

	// save start of destination buffer
	mov	lr,r0		// start buffer

	// get number of pixels aligned to 8 bytes
	lsrs	r4,r2,#3	// number of pixels / 8
	lsls	r4,#3		// number of pixels aligned to 8 bytes down -> R4
	eors	r2,r4		// number of pixels last 3 bits (modulo 8)

	// shift pointers to last 8-byte group
	add	r0,r4		// shift destination pointer to the end
	add	r1,r4		// shift source pointer to the end

	// jump to blit rest of pixels in last 8-byte group
	adr	r4,3f		// get address of label '3:' (must be word aligned)
	lsls	r2,#3		// *8, convert number of pixels to offset of blit macro (1 macro is 8 bytes long)
	subs	r4,r2		// subtract offset of first valid blit macro
	adds	r4,#1		// set bit 0 - flag to use thumb instructions
	bx	r4		// jump into loop

// ---- [53..61 per loop] blend pixels, speed 6.625..7.625 clock cycles per pixel

.align 2 // address of label '3:' must be word aligned (32 bits)

	// [2] shift pointers 8 bytes down
1:	subs	r0,#8		// [1] shift destination pointer by 8 bytes down
	subs	r1,#8		// [1] shift source pointer by 8 bytes down

	// [48..56] blit 8 pixels (32 instructions)
	blitkey 7		// [6,7] blit pixel 7
	blitkey 6		// [6,7] blit pixel 6
	blitkey 5		// [6,7] blit pixel 5
	blitkey 4		// [6,7] blit pixel 4
	blitkey 3		// [6,7] blit pixel 3
	blitkey 2		// [6,7] blit pixel 2
	blitkey 1		// [6,7] blit pixel 1
	blitkey 0		// [6,7] blit pixel 0

// this address must be word aligned

	// [2,3] next 8 pixels
3:	cmp	r0,lr		// [1] start address reached?
	bhi	1b		// [1,2] not start address yet

	// pop registers and return from function
9:	pop	{r4,pc}
