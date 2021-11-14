
// ****************************************************************************
//
//                                 VGA layers
//
// ****************************************************************************

#ifndef _VGA_LAYER_H
#define _VGA_LAYER_H

// base layer commands
#define VGADARK(num,col) (((u32)(vga_offset_dark+BASE_OFFSET)<<27) | ((u32)(num)<<8) | (u32)(col)) // assemble control word of "dark" command
#define VGACMD(jmp,num) (((u32)(jmp)<<27) | (u32)(num)) // assemble control word

// --- overlapped layer init word (delay: use number of offset pixels * Vmode.cpp, num: number of pixels)

// init word of key color layer LAYERPROG_KEY
#define VGAKEY(delay,num,col) (((u32)((delay)+1)<<19) | ((u32)(col)<<11) | (u32)((num)-1))

// init word of mono layer LAYERPROG_MONO
#define VGAMONO(delay,num,col) (((u32)((delay)+0)<<20) | ((u32)(col)<<12) | ((u32)((num)-1)<<1) | B0)

// init word of color layer LAYERPROG_MONO
#define VGACOLOR(delay,num) (((u32)((delay)+2)<<20) | ((u32)0xff<<12) | ((u32)((num)-1)<<1) | 0)

// init word of black color layer LAYERPROG_BLACK
#define VGABLACK(delay,num) (((u32)((delay)+3)<<16) | (u32)((num)-1))

// init word of white color layer LAYERPROG_WHITE
#define VGAWHITE(delay,num) (((u32)((delay)+3)<<16) | (u32)((num)-1))

// init word of RLE layer LAYERPROG_RLE
#define VGARLE(delay) ((delay)+1)

// swap bytes of command
#define BYTESWAP(n) ((((n)&0xff)<<24)|(((n)&0xff00)<<8)|(((n)&0xff0000)>>8)|(((n)&0xff000000)>>24))

// align to multiple of 4
#define ALIGN4(x) ((x) & ~3)

// layer program descriptor
typedef struct {
	const u16*		ins;		// pointer to program instructions (NULL=layers is OFF)
	const struct pio_program* prg;		// pointer to program descriptor
	u8			length;		// program length (number of instructions)
	u8			wrap_target;	// offset of wrap target
	u8			wrap;		// offset of wrap end
	u8			idle;		// offset of idle
	u8			entry;		// offset of entry
	u8			maxidle;	// max. offset of idle to detect end of job
	u8			extranum;	// number of extra offsets
	u8			extra[2*16];	// extra offsets, pairs: offset, CPP-correction
} sLayerProg;

// layer program descriptors
extern const sLayerProg LayerProg[LAYERPROG_NUM];

// current layer program of overlapped layers
extern u8 LayerProgInx;		// index of current layer program (LAYERPROG_*, LAYERPROG_BASE = overlapped layers are OFF)
extern sLayerProg CurLayerProg;	// copy of current layer program

// layer mode descriptor
typedef struct {
	u8	prog;		// layer program (LAYERPROG_*)
	u8	mincpp;		// minimal clock cycles per pixel
	u8	maxcpp;		// maximal clock cycles per pixel
} sLayerMode;

// layer mode descriptors
extern const sLayerMode LayerMode[LAYERMODE_NUM];

// current layer mode of layers
extern u8 LayerModeInx[LAYERS];	// index of current layer mode (LAYERMODE_*)
extern sLayerMode CurLayerMode[LAYERS]; // copy of current layer mode

// layer screen descriptor (on change update SLAYER_* in define.h)
typedef struct {
	const u8*	img;	// pointer to image in current layer format, or sprite list
	const void*	par;	// additional parameter (RLE index table, integer transformation matrix)
	u32		init;	// init word sent on start of scanline (start X coordinate)
	u32		keycol;	// key color
	u16		trans;	// trans count
	s16		x;	// start X coordinate
	s16		y;	// start Y coordinate
	u16		w;	// width in pixels
	u16		h;	// height
	u16		wb;	// image width in bytes (pitch of lines)
	u8		mode;	// layer mode
	s8		horiz;	// horizon of perspective projection/4 (only with LAYERMODE_PERSP* modes, 0=no perspecitve, <0 ceilling)
	u8		xbits;	// number of bits of width of source image (only with LAYERMODE_PERSP* modes)
	u8		ybits;	// number of bits of height of source image (only with LAYERMODE_PERSP* modes)
	u16		spritenum; // number of sprites
	Bool		on;	// layer is ON
	u8		cpp;	// current clock pulses per pixel (used to calculate X coordinate)
} sLayer;

// sprite (on change update SSPRITE_* in define.h)
typedef struct {
	u8*	img;	// SSPRITE_IMG pointer to image data
	u8*	x0;	// SSPRITE_X0 pointer to array of start of lines, or fast sprite start of lines/4
	u8*	w0;	// SSPRITE_W0 pointer to array of length of lines, or fast sprite length of lines/4
	u32	keycol;	// SSPRITE_KEYCOL key color
	s16	x;	// SSPRITE_X sprite X-coordinate on the screen
	s16	y;	// SSPRITE_Y sprite Y-coordinate on the screen
	u16	w;	// SSPRITE_W sprite width (slow sprite: max. width 255)
	u16	h;	// SSPRITE_H sprite height
	u16	wb;	// SSPRITE_WB sprite pitch (number of bytes between lines)
	u16	res;	// ...reserved, structure align
} sSprite;

// current layer screens
extern sLayer LayerScreen[LAYERS]; // layer screens

extern u8 LayerMask;	// mask of active layers

// index of first pin of layer (base layer should stay VGA_GPIO_FIRST)
extern u8 LayerFirstPin[LAYERS_MAX];

// number of pins of overlapped layer (base layer should stay VGA_GPIO_OUTNUM)
extern u8 LayerNumPin[LAYERS_MAX];

// set overlapped layer 1..3 ON
void LayerOn(u8 inx);

// set overlapped layer 1..3 OFF
void LayerOff(u8 inx);

// set coordinate X of overlapped layer
void LayerSetX(u8 inx, s16 x);

// set coordinate Y of overlapped layer
void LayerSetY(u8 inx, s16 y);

// set width of image of overlapped layer
//   Uses auto pitch wb (full line). Set custom wb after calling this function.
void LayerSetW(u8 inx, u16 w);

// set height of image of overlapped layer
void LayerSetH(u8 inx, u16 h);

// setup overlapped layer 1..3 (not for sprites and not for perspective mode)
//  inx ... layer index 1..3
//  img ... pointer to image data
//  vmode ... pointer to initialized video configuration
//  w ... image width in pixels (must be multiple of 4)
//  h ... image height
//  col ... key color (needed for LAYERMODE_KEY and LAYERMODE_MONO layer mode)
//  par ... additional data (RLE index table, integer transformation matrix)
// Use these functions after layer setup: LayerSetX, LayerSetY, LayerOn
void LayerSetup(u8 inx, const u8* img, const sVmode* vmode, u16 w, u16 h, u8 col = 0, const void* par = NULL);

// setup overlapped layer 1..3 for LAYERMODE_PERSP* modes
//  inx ... layer index 1..3
//  img ... pointer to source image data (image width and height must be power of 2)
//  vmode ... pointer to initialized video configuration
//  w ... destination image width in pixels (must be multiple of 4)
//  h ... destination image height
//  xbits ... number of bits of width of source image
//  ybits ... number of bits of height of source image
//  horiz ... horizon of perspective projection/4 (0=no perspecitve, <0 ceilling)
//  mat ... integer transformation matrix
//  col ... key color (needed for LAYERMODE_PERSPKEY layer mode)
// Use these functions after layer setup: LayerSetX, LayerSetY, LayerOn
void LayerPerspSetup(u8 inx, const u8* img, const sVmode* vmode, u16 w, u16 h, u8 xbits, u8 ybits,
	s8 horiz, const int* mat, u8 col = 0);

// setup overlapped layer 1..3 for LAYERMODE_SPRITE* and LAYERMODE_FASTSPRITE* modes
//  inx ... layer index 1..3
//  sprite ... pointer to list of sprites (array of pointers to sprites; sorted by X on LAYERMODE_FASTSPRITE* modes)
//  spritenum ... number of sprites in the list (to turn sprite off, you can set its coordinate Y out of the screen)
//  vmode ... pointer to initialized video configuration
//  x ... start coordinate X of area with sprites
//  y ... start coordinate Y of area with sprites
//  w ... width of area with sprites (must be multiple of 4)
//  h ... height of area with sprites
//  col ... key color (needed for LAYERMODE_SPRITEKEY and LAYERMODE_FASTSPRITEKEY layer mode)
// Use functions LayerOn after layer setup.
void LayerSpriteSetup(u8 inx, sSprite** sprite, u16 spritenum, const sVmode* vmode,
	s16 x, s16 y, u16 w, u16 h, u8 col = 0);

// prepare array of start and length of lines (detects transparent pixels)
//  img ... image
//  x0 ... array of start of lines
//  w0 ... array of length of lines
//  w ... sprite width (slow sprite: max. width 255)
//  h ... sprite height
//  wb ... sprite pitch (bytes between lines)
//  col ... key color
//  fast ... fast sprite, divide start and length of line by 4
void SpritePrepLines(const u8* img, u8* x0, u8* w0, u16 w, u16 h, u16 wb, u8 col, Bool fast);

// sort fast sprite list by X coordinate
void SortSprite(sSprite** list, int num);

#endif // _VGA_LAYER_H
