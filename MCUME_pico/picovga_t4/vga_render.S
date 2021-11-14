
// ****************************************************************************
//
//                              VGA render
//
// ****************************************************************************

#include "define.h"		// common definitions of C and ASM

	.syntax unified
	.section .time_critical.Render, "ax"
	.cpu cortex-m0plus
	.thumb			// use 16-bit instructions

.extern	pScreen			// sScreen* pScreen; // pointer to current video screen
.extern LineBuf0		// u8 LineBuf0[BLACK_MAX]; // line buffer with black color

// extern "C" u32* Render(u32* cbuf, u8* dbuf, int line, int pixnum);

// render scanline
//  cbuf ... control buffer
//  dbuf ... data buffer (pixel data)
//  line ... current scanline 0..
//  pixnum ... total pixels (must be multiple of 4)
// Returns new pointer to control buffer

.thumb_func
.global Render
Render:

	// push registers
	push	{r4-r7,lr}

	// prepare local variables
//  SP+0: input argument of render functions
//  SP+4: R0 control buffer
//  SP+8: R1 data buffer (pixel data)
//  SP+12: R2 current scanline 0..
//  SP+16: R3 total pixels
//  SP+20: R4
//  SP+24: R5
//  SP+28: R6
//  SP+32: R7
//  SP+36: LR

	sub	sp,#20
	str	r0,[sp,#4]	// control buffer
	str	r1,[sp,#8]	// data buffer
	str	r3,[sp,#16]	// total pixels

// ---- prepare pointer to current screen
// sScreen* s = pScreen;
// if (s != NULL) {

	// prepare pointer to current screen
	ldr	r4,Render_pScreenAddr // pointer to pointer to current video Screen (variable pScreen)
	ldr	r4,[r4,#0]	// pointer to current video Screen
	cmp	r4,#0		// is pointer valid?
	beq	Render_Clear	// pointer is not valid, clear rest of line (display is OFF)

// ---- find video strip with current scanline
// int stripnum = s->num;
// sStrip* t = &s->strip[0];
// for (; stripnum > 0; stripnum--) {

	// loop through video strips
	ldrh	r5,[r4,#SSCREEN_NUM] // u16 number of video strips
	tst	r5,r5		// check number of video strips
	beq	Render_Clear	// no video strips, return
	adds	r4,#SSCREEN_STRIP // pointer to first video strip

// R2 ... current scanline
// R4 ... pointer to video strip
// R5 ... counter of video strips

Render_StripLoop:

	// chek if current scanline has been found
	//  if (line < t->height) {
	ldrh	r3,[r4,#SSTRIP_HEIGHT] // u16 height of this video strip
	cmp	r2,r3		// check if current scanline fits into this video strip
	blo	Render_StripOK	// scanline < strip height, this strip is OK

	// subtract video strip height from scanline number (to be relative to start of strip)
	//  line -= t->height;
	subs	r2,r3		// subtract strip height from scanline number

	// next video strip
	//  t++;
	//  for (; stripnum > 0; stripnum--)
	adds	r4,#SSTRIP_SIZE	// shift pointer to next video strip
	subs	r5,#1		// counter of video strips
	bne	Render_StripLoop // next video strip
	b	Render_Clear	// video strip not found

// ---- process all video segments

Render_StripOK:

	// prepare first video segment
	//  sSegm* g = &t->seg[0];
	//  int segnum = t->num;
	//  for (; segnum > 0; segnum--) {
	str	r2,[sp,#12]	// save current scanline
	ldrh	r5,[r4,#SSTRIP_NUM] // u16 number of video segments
	tst	r5,r5		// check number of video segments
	beq	Render_Clear	// no video strips, return
	adds	r4,#SSTRIP_SEG	// pointer to first video segment

// R4 ... pointer to video segment
// R5 ... counter of video segments

Render_SegmLoop:

	// get number of remaining pixels
	ldr	r2,[sp,#16]	// get remaining pixels
	tst	r2,r2		// check number of pixels
	beq	Render_Clear	// end of scanline, stop rendering

	// get segment width -> R3
	//  int w = g->width;
	//  if (w > pixnum) w = pixnum;
	//  if (w > 0) {
	ldrh	r3,[r4,#SSEGM_WIDTH] // get segment width
	cmp	r3,r2		// check width
	blo	2f		// width is OK
	mov	r3,r2		// limit width by total width
2:	tst	r3,r3		// check width
	beq	Render_SegmNext	// this segment is invisible, skip it

	// update remaining pixels
	//  pixnum -= w;
	subs	r2,r3		// decrease remaining width
	str	r2,[sp,#16]	// store new remaining pixels

	// get Y coordinate -> R2
	//  int y = g->offy + line;
	ldrh	r2,[r4,#SSEGM_OFFY] // get offset at Y direction
	sxth	r2,r2		// expand to signed
	ldr	r1,[sp,#12]	// get current scanline
	add	r2,r1		// add Y offset and current scanline

	// double lines
	//  if (g->dbly) y /= 2;
	ldrb	r1,[r4,#SSEGM_DBLY] // get dbly flag
	tst	r1,r1		// is dbly flag set?
	beq	2f		// dbly flag not set
	asrs	r2,#1		// Y coordinate / 2

	// wrap Y coordinate
	//  int wy = g->wrapy;
	//  while (y < 0) y += wy;
	//  while (y >= wy) y -= wy;
2:	ldrh	r1,[r4,#SSEGM_WRAPY] // get wrapy
3:	subs	r2,r1		// subtract wrapy
	bpl	3b		// repeat
4:	adds	r2,r1		// add wrapy
	bmi	4b		// repeat

	// get X coordinate -> R1
	//  int x = g->offx;
6:	ldrh	r1,[r4,#SSEGM_OFFX] // get offset at X direction
	sxth	r1,r1		// expand to signed

	// wrap X coordinate
	//  int wx = g->wrapx;
	//  while (x < 0) x += wx;
	//  while (x >= wx) x -= wx;
	ldrh	r0,[r4,#SSEGM_WRAPX] // get wrapx
3:	subs	r1,r0		// subtract wrapx
	bpl	3b		// repeat
4:	adds	r1,r0		// add wrapx
	bmi	4b		// repeat

// ---- process 1st format group: GF_COLOR

	// get format -> R0
6:	ldrb	r0,[r4,#SSEGM_FORM] // get current format

	// serve format GF_COLOR
	tst	r0,r0		// format GF_COLOR ?
	bne	7f		// no

	//  u32 par = ((y & 1) == 0) ? g->par : g->par2
	lsrs	r2,#1		// check bit 0 of Y coordinate
	ldr	r1,[r4,#SSEGM_PAR] // get par for even line
	bcc	2f		// even line
	ldr	r1,[r4,#SSEGM_PAR2] // get par2 for odd line

	//  *cbuf++ = w/4; // number of pixels/4
2:	lsrs	r2,r3,#2	// width/4
	ldr	r6,[sp,#4]	// get pointer to control buffer
	stmia	r6!,{r2}	// store width/4

	//  *cbuf++ = (u32)dbuf; // pointer to data buffer
	ldr	r0,[sp,#8]	// get pointer to data buffer
	stmia	r6!,{r0}	// store pointer to data
	str	r6,[sp,#4]	// save new pointer to control buffer

	//  dbuf = RenderColor(dbuf, par, w/4);
	bl	RenderColor
	str	r0,[sp,#8]	// store new pointer to data buffer
	b	Render_SegmNext

// ---- process 2nd format group: using control buffer cbuf

	// prepare input argument video segment -> [SP+0]
7:	str	r4,[sp,#0]	// prepare 4th argument - current video segment

	// prepare function addres -> R7
	adr	r7,Render_FncAddr // get address of jump table
	lsls	r6,r0,#2	// format * 4
	ldr	r7,[r7,r6]	// load function address -> R7

	// check 2nd format group
	cmp	r0,#GF_GRP2MAX	// check 2nd format group
	bhi	2f		// > 2nd group

	//  cbuf = RenderGraph8(cbuf, x, y, w, g);
	ldr	r0,[sp,#4]	// get pointer to control buffer
	blx	r7		// call render function
	str	r0,[sp,#4]	// save new pointer to control buffer
	b	Render_SegmNext

// ---- process 3rd format group: using data buffer dbuf

	//  *cbuf++ = w/4; // number of pixels/4
2:	lsrs	r0,r3,#2	// width/4
	ldr	r6,[sp,#4]	// get pointer to control buffer
	stmia	r6!,{r0}	// store width/4

	//  *cbuf++ = (u32)dbuf; // pointer to data buffer
	ldr	r0,[sp,#8]	// get pointer to data buffer
	stmia	r6!,{r0}	// store pointer to data
	str	r6,[sp,#4]	// save new pointer to control buffer

	//  dbuf = RenderColor(dbuf, par, w/4);
	blx	r7		// call render function
	str	r0,[sp,#8]	// store new pointer to data buffer

Render_SegmNext:

	// next video segment
	adds	r4,#SSEGM_SIZE	// shift pointer to next video segment
	subs	r5,#1		// counter of video segments
	bne	Render_SegmLoop	// next video segment

// ---- clear rest of line, write pointer to control buffer

Render_Clear:

	// return current control buffer
	ldr	r0,[sp,#4]	// control buffer

	// check if some pixels left	
	ldr	r1,[sp,#16]	// number of remaining pixels
	lsrs	r1,#2		// number of pixels/4 (= number of 4-pixels)
	beq	9f		// no pixels left

	// write size and address to control buffer
	ldr	r2,Render_LineBuf0Addr	// data buffer with black color
	stmia	r0!,{r1,r2}	// write number of 4-pixels and pointer to data buffer to control buffer

	// pop registers and return (return control buffer in r0)
9:	add	sp,#20
	pop	{r4-r7,pc}

	.align 2

// pointer to pointer with current video screen
Render_pScreenAddr:
	.word	pScreen

// pointer to buffer with black color
Render_LineBuf0Addr:
	.word	LineBuf0

// poiners to render functions
Render_FncAddr:
	// 1st format group
	.word	RenderColor	// GF_COLOR simple color (par=color pattern 4-pixels even line, par2=color pattern 4-pixels odd line)

	// 2nd format group
	.word	RenderGraph8	// GF_GRAPH8 native 8-bit graphics (X1Y1R2G2B2) - fast, transfers "as is" to PIO 
	.word	RenderTile	// GF_TILE tiles
	.word	RenderTile2	// GF_TILE alternate tiles
	.word	RenderProgress	// GF_PROGRESS horizontal progress indicator
	.word	RenderGrad1	// render gradient with 1 line GF_GRAD1
	.word	RenderGrad2	// render gradient with 2 lines GF_GRAD2

	// 3rd format group
	.word	RenderGraph4	// GF_GRAPH4 4-bit graphics
	.word	RenderGraph2	// GF_GRAPH2 2-bit graphics
	.word	RenderGraph1	// GF_GRAPH1 1-bit graphics
	.word	RenderMText	// GF_MTEXT 8-pixel mono text
	.word	RenderAText	// GF_ATEXT 8-pixel attribute text, character + 2x4 bit attributes
	.word	RenderFText	// GF_FTEXT 8-pixel foreground color text, character + foreground color
	.word	RenderCText	// GF_CTEXT 8-pixel color text, character + background color + foreground color
	.word	RenderGText	// GF_GTEXT 8-pixel gradient text (par = pointer to 1-bit font, par2 = pointer to color array)
	.word	RenderDText	// GF_DTEXT 8-pixel double gradient text (par = pointer to 1-bit font, par2 = pointer to color array)
	.word	RenderLevel	// GF_LEVEL level graph
	.word	RenderLevelGrad	// GF_LEVELGRAD level gradient graph
	.word	RenderOscil	// GF_OSCIL oscilloscope pixel graph
	.word	RenderOscLine	// GF_OSCLINE oscilloscope line graph
	.word	RenderPlane2	// GF_PLANE2 4 colors on 2 graphic planes
	.word	RenderAttrib8	// GF_ATTRIB8 2x4 bit color attribute per 8x8 pixel sample
	.word	RenderGraph8Mat	// GF_GRAPH8MAT 8-bit graphics with 2D matrix transformation
	.word	RenderGraph8Persp // GF_GRAPH8PERSP 8-bit graphics with perspective projection
	.word	RenderTilePersp	// GF_TILEPERSP tiles with perspective
	.word	RenderTilePersp15 // GF_TILEPERSP15 tiles with perspective, 1.5 pixels
	.word	RenderTilePersp2 // GF_TILEPERSP2 tiles with perspective, double pixels
	.word	RenderTilePersp3 // GF_TILEPERSP3 tiles with perspective, triple pixels
	.word	RenderTilePersp4 // GF_TILEPERSP4 tiles with perspective, quadruple pixels
