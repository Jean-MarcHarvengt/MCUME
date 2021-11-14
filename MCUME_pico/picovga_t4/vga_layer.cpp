
// ****************************************************************************
//
//                                 VGA layers
//
// ****************************************************************************

#include "include.h"

// layer program descriptors
const sLayerProg LayerProg[LAYERPROG_NUM] = {

	// LAYERPROG_BASE base layer
	{
		.ins=vga_program_instructions,		// pointer to program instructions
		.prg=&vga_program,			// pointer to program descriptor
		.length=vga_program.length,		// program length (number of instructions)
		.wrap_target=vga_wrap_target,		// offset of wrap target
		.wrap=vga_wrap,				// offset of wrap end
		.idle=vga_offset_entry,			// offset of idle
		.entry=vga_offset_entry,		// offset of entry
		.maxidle=2,				// max. offset of idle to detect end of job
		.extranum=2,				// number of extra offsets
		.extra={				// extra offsets, pairs: offset, CPP-correction
			vga_offset_extra1, 2,
			vga_offset_extra2, 2,
		},
	},

	// LAYERPROG_KEY layer with key color
	{
		.ins=keylayer_program_instructions,	// pointer to program instructions
		.prg=&keylayer_program,			// pointer to program descriptor
		.length=keylayer_program.length,	// program length (number of instructions)
		.wrap_target=keylayer_wrap_target,	// offset of wrap target
		.wrap=keylayer_wrap,			// offset of wrap end
		.idle=keylayer_offset_idle,		// offset of idle
		.entry=keylayer_offset_entry,		// offset of entry
		.maxidle=2,				// max. offset of idle to detect end of job
		.extranum=1,				// number of extra offsets
		.extra={				// extra offsets, pairs: offset, CPP-correction
			keylayer_offset_extra1, 6,
		},
	},

	// LAYERPROG_BLACK layer with black key color
	{
		.ins=blacklayer_program_instructions,	// pointer to program instructions
		.prg=&blacklayer_program,		// pointer to program descriptor
		.length=blacklayer_program.length,	// program length (number of instructions)
		.wrap_target=blacklayer_wrap_target,	// offset of wrap target
		.wrap=blacklayer_wrap,			// offset of wrap end
		.idle=blacklayer_offset_idle,		// offset of idle
		.entry=blacklayer_offset_entry,		// offset of entry
		.maxidle=2,				// max. offset of idle to detect end of job
		.extranum=2,				// number of extra offsets
		.extra={				// extra offsets, pairs: offset, CPP-correction
			blacklayer_offset_extra1, 4,
			blacklayer_offset_extra2, 3,
		},
	},

	// LAYERPROG_WHITE layer with white key color
	{
		.ins=whitelayer_program_instructions,	// pointer to program instructions
		.prg=&whitelayer_program,		// pointer to program descriptor
		.length=whitelayer_program.length,	// program length (number of instructions)
		.wrap_target=whitelayer_wrap_target,	// offset of wrap target
		.wrap=whitelayer_wrap,			// offset of wrap end
		.idle=whitelayer_offset_idle,		// offset of idle
		.entry=whitelayer_offset_entry,		// offset of entry
		.maxidle=2,				// max. offset of idle to detect end of job
		.extranum=1,				// number of extra offsets
		.extra={				// extra offsets, pairs: offset, CPP-correction
			whitelayer_offset_extra1, 4,
		},
	},

	// LAYERPROG_MONO layer with mono pattern or simple color
	{
		.ins=monolayer_program_instructions,	// pointer to program instructions
		.prg=&monolayer_program,		// pointer to program descriptor
		.length=monolayer_program.length,	// program length (number of instructions)
		.wrap_target=monolayer_wrap_target,	// offset of wrap target
		.wrap=monolayer_wrap,			// offset of wrap end
		.idle=monolayer_offset_idle,		// offset of idle
		.entry=monolayer_offset_entry,		// offset of entry
		.maxidle=2,				// max. offset of idle to detect end of job
		.extranum=2,				// number of extra offsets
		.extra={				// extra offsets, pairs: offset, CPP-correction
			monolayer_offset_extra1, 4,
			monolayer_offset_extra2, 2,
		},
	},

	// LAYERPROG_RLE layer with RLE compression
	{
		.ins=rlelayer_program_instructions,	// pointer to program instructions
		.prg=&rlelayer_program,			// pointer to program descriptor
		.length=rlelayer_program.length,	// program length (number of instructions)
		.wrap_target=rlelayer_wrap_target,	// offset of wrap target
		.wrap=rlelayer_wrap,			// offset of wrap end
		.idle=rlelayer_offset_idle,		// offset of idle
		.entry=rlelayer_offset_entry,		// offset of entry
		.maxidle=2,				// max. offset of idle to detect end of job
		.extranum=7,				// number of extra offsets
		.extra={				// extra offsets, pairs: offset, CPP-correction
			rlelayer_offset_extra1, 1,
			rlelayer_offset_extra2, 3,
			rlelayer_offset_extra3, 2,
			rlelayer_offset_extra4, 2,
			rlelayer_offset_extra5, 3,
			rlelayer_offset_extra6, 2,
			rlelayer_offset_extra7, 3,
		},
	},

};

// current layer program of overlapped layers
u8 LayerProgInx;		// index of current layer program (LAYERPROG_*)
sLayerProg CurLayerProg;	// copy of current layer program

// layer mode descriptors
const sLayerMode LayerMode[LAYERMODE_NUM] = {

	// LAYERMODE_BASE base layer
	{
		.prog=LAYERPROG_BASE,	// layer program (LAYERPROG_*)
		.mincpp=2,		// minimal clock cycles per pixel
		.maxcpp=17,		// maximal clock cycles per pixel
	},

	// LAYERMODE_KEY layers with key color
	{
		.prog=LAYERPROG_KEY,	// layer program (LAYERPROG_*)
		.mincpp=6,		// minimal clock cycles per pixel
		.maxcpp=37,		// maximal clock cycles per pixel
	},

	// LAYERMODE_BLACK layers with black key color
	{
		.prog=LAYERPROG_BLACK,	// layer program (LAYERPROG_*)
		.mincpp=4,		// minimal clock cycles per pixel
		.maxcpp=34,		// maximal clock cycles per pixel
	},

	// LAYERMODE_WHITE layers with white key color
	{
		.prog=LAYERPROG_WHITE,	// layer program (LAYERPROG_*)
		.mincpp=4,		// minimal clock cycles per pixel
		.maxcpp=35,		// maximal clock cycles per pixel
	},

	// LAYERMODE_MONO layers with mono pattern
	{
		.prog=LAYERPROG_MONO,	// layer program (LAYERPROG_*)
		.mincpp=4,		// minimal clock cycles per pixel
		.maxcpp=35,		// maximal clock cycles per pixel
	},

	// LAYERMODE_COLOR layers with simple color
	{
		.prog=LAYERPROG_MONO,	// layer program (LAYERPROG_*)
		.mincpp=2,		// minimal clock cycles per pixel
		.maxcpp=33,		// maximal clock cycles per pixel
	},

	// LAYERMODE_RLE layers with RLE compression
	{
		.prog=LAYERPROG_RLE,	// layer program (LAYERPROG_*)
		.mincpp=3,		// minimal clock cycles per pixel
		.maxcpp=32,		// maximal clock cycles per pixel
	},

	// LAYERMODE_SPRITEKEY layers with sprites with key color
	{
		.prog=LAYERPROG_KEY,	// layer program (LAYERPROG_*)
		.mincpp=6,		// minimal clock cycles per pixel
		.maxcpp=37,		// maximal clock cycles per pixel
	},

	// LAYERMODE_SPRITEBLACK layers with sprites with black key color
	{
		.prog=LAYERPROG_BLACK,	// layer program (LAYERPROG_*)
		.mincpp=4,		// minimal clock cycles per pixel
		.maxcpp=34,		// maximal clock cycles per pixel
	},

	// LAYERMODE_SPRITEWHITE layers with sprites with white key color
	{
		.prog=LAYERPROG_WHITE,	// layer program (LAYERPROG_*)
		.mincpp=4,		// minimal clock cycles per pixel
		.maxcpp=35,		// maximal clock cycles per pixel
	},

	// LAYERMODE_FASTSPRITEKEY layers with fast sprites with key color
	{
		.prog=LAYERPROG_KEY,	// layer program (LAYERPROG_*)
		.mincpp=6,		// minimal clock cycles per pixel
		.maxcpp=37,		// maximal clock cycles per pixel
	},

	// LAYERMODE_FASTSPRITEBLACK layers with fast sprites with black key color
	{
		.prog=LAYERPROG_BLACK,	// layer program (LAYERPROG_*)
		.mincpp=4,		// minimal clock cycles per pixel
		.maxcpp=34,		// maximal clock cycles per pixel
	},

	// LAYERMODE_FASTSPRITEWHITE layers with fast sprites with white key color
	{
		.prog=LAYERPROG_WHITE,	// layer program (LAYERPROG_*)
		.mincpp=4,		// minimal clock cycles per pixel
		.maxcpp=35,		// maximal clock cycles per pixel
	},

	// LAYERMODE_PERSPKEY layer with key color and image with transformation matrix
	{
		.prog=LAYERPROG_KEY,	// layer program (LAYERPROG_*)
		.mincpp=6,		// minimal clock cycles per pixel
		.maxcpp=37,		// maximal clock cycles per pixel
	},

	// LAYERMODE_PERSPBLACK layer with black key color and image with transformation matrix
	{
		.prog=LAYERPROG_BLACK,	// layer program (LAYERPROG_*)
		.mincpp=4,		// minimal clock cycles per pixel
		.maxcpp=34,		// maximal clock cycles per pixel
	},

	// LAYERMODE_PERSPWHITE layer with white key color and image with transformation matrix
	{
		.prog=LAYERPROG_WHITE,	// layer program (LAYERPROG_*)
		.mincpp=4,		// minimal clock cycles per pixel
		.maxcpp=35,		// maximal clock cycles per pixel
	},

	// LAYERMODE_PERSP2KEY layer with key color and double pixel image with transformation matrix
	{
		.prog=LAYERPROG_KEY,	// layer program (LAYERPROG_*)
		.mincpp=6,		// minimal clock cycles per pixel
		.maxcpp=37,		// maximal clock cycles per pixel
	},

	// LAYERMODE_PERSP2BLACK layer with black key color and double pixel image with transformation matrix
	{
		.prog=LAYERPROG_BLACK,	// layer program (LAYERPROG_*)
		.mincpp=4,		// minimal clock cycles per pixel
		.maxcpp=34,		// maximal clock cycles per pixel
	},

	// LAYERMODE_PERSP2WHITE layer with white key color and double pixel image with transformation matrix
	{
		.prog=LAYERPROG_WHITE,	// layer program (LAYERPROG_*)
		.mincpp=4,		// minimal clock cycles per pixel
		.maxcpp=35,		// maximal clock cycles per pixel
	},
};

// current layer mode of layers
u8 LayerModeInx[LAYERS];	// index of current layer mode (LAYERMODE_*)
sLayerMode CurLayerMode[LAYERS]; // copy of current layer mode

// current layer screens
sLayer LayerScreen[LAYERS]; // layer screens

u8 LayerMask;	// mask of active layers

// index of first pin of layer (base layer should stay VGA_GPIO_FIRST)
u8 LayerFirstPin[LAYERS_MAX] = { VGA_GPIO_FIRST, VGA_GPIO_FIRST, VGA_GPIO_FIRST, VGA_GPIO_FIRST};

// number of pins of overlapped layer (base layer should stay VGA_GPIO_OUTNUM)
u8 LayerNumPin[LAYERS_MAX] = { VGA_GPIO_OUTNUM, VGA_GPIO_OUTNUM, VGA_GPIO_OUTNUM, VGA_GPIO_OUTNUM};

// set overlapped layer 1..3 ON
void LayerOn(u8 inx)
{
	__dmb();
	LayerScreen[inx].on = True;
	__dmb();
}

// set overlapped layer 1..3 OFF
void LayerOff(u8 inx)
{
	__dmb();
	LayerScreen[inx].on = False;
	__dmb();
}

// set coordinate X of overlapped layer
void LayerSetX(u8 inx, s16 x)
{
	sLayer* lay = &LayerScreen[inx];
	s32 cppx = lay->cpp*x; // initial delay
	if (cppx < 0) cppx = 0;
	u32 w = lay->w; // image width
	u32 init = 0; // init word

	// prepare init word
	switch (lay->mode)
	{
	case LAYERMODE_PERSP2KEY: // layer with key color and double pixel image with transformation matrix
	case LAYERMODE_PERSPKEY: // layer with key color and image with transformation matrix
	case LAYERMODE_FASTSPRITEKEY: // layer with fast sprites with key color
	case LAYERMODE_SPRITEKEY: // layer with sprites with key color
	case LAYERMODE_KEY: // layer with key color
		init = VGAKEY(cppx, w, (lay->keycol & 0xff));
		break;

	case LAYERMODE_PERSP2BLACK: // layer with black key color and double pixel image with transformation matrix
	case LAYERMODE_PERSPBLACK: // layer with black key color and image with transformation matrix
	case LAYERMODE_FASTSPRITEBLACK: // layer with fast sprites with black key color
	case LAYERMODE_SPRITEBLACK: // layer with sprites with black key color
	case LAYERMODE_BLACK: // layer with black key color
		init = VGABLACK(cppx, w);
		break;

	case LAYERMODE_PERSP2WHITE: // layer with white key color and double pixel image with transformation matrix
	case LAYERMODE_PERSPWHITE: // layer with white key color and image with transformation matrix
	case LAYERMODE_FASTSPRITEWHITE: // layer with fast sprites with white key color
	case LAYERMODE_SPRITEWHITE: // layer with sprites with white key color
	case LAYERMODE_WHITE: // layer with white key color
		init = VGAWHITE(cppx, w);
		break;

	case LAYERMODE_MONO: // layer with mono pattern
		init = VGAMONO(cppx, w, (lay->keycol & 0xff));
		break;

	case LAYERMODE_COLOR: // layer with simple color
		init = VGACOLOR(cppx, w);
		break;

	case LAYERMODE_RLE: // layer with RLE compression
		init = VGARLE(cppx);
		break;
	}
	lay->init = init; // init word
	lay->x = x; // start X coordinate
}

// set coordinate Y of overlapped layer
void LayerSetY(u8 inx, s16 y)
{
	sLayer* lay = &LayerScreen[inx];
	lay->y = y;
}

// set width of image of overlapped layer
//   Uses auto pitch wb (full line). Set custom wb after calling this function.
void LayerSetW(u8 inx, u16 w)
{
	sLayer* lay = &LayerScreen[inx];
	lay->w = w; // image width
	Bool mono = (lay->mode == LAYERMODE_MONO);
	lay->trans = mono ? (((w/8)+3)/4) : (w/4); // transfer count
	lay->wb = mono ? (w/8) : w; // width bytes
	LayerSetX(inx, lay->x); // update init word
}

// set height of image of overlapped layer
void LayerSetH(u8 inx, u16 h)
{
	sLayer* lay = &LayerScreen[inx];
	lay->h = h;
}

// setup overlapped layer 1..3 (not for sprites and not for perspective mode)
//  inx ... layer index 1..3
//  img ... pointer to image data
//  vmode ... pointer to initialized video configuration
//  w ... image width in pixels (must be multiple of 4)
//  h ... image height
//  col ... key color (needed for LAYERMODE_KEY and LAYERMODE_MONO layer mode)
//  par ... additional data (RLE index table, integer transformation matrix)
// Use these functions after layer setup: LayerSetX, LayerSetY, LayerOn
void LayerSetup(u8 inx, const u8* img, const sVmode* vmode, u16 w, u16 h, u8 col /* = 0 */, const void* par /* = NULL */)
{
	LayerOff(inx); // set layer OFF
	sLayer* lay = &LayerScreen[inx]; // get pointer to layer
	lay->img = img; // pointer to image data
	lay->par = par; // additional parameter
	lay->keycol = col | ((u16)col << 8) | ((u32)col << 16) | ((u32)col << 24); // key color
	lay->x = 0; // X coordinate
	lay->y = 0; // Y coordinate
	lay->h = h; // height of image
	lay->spritenum = 0; // number of sprites
	lay->cpp = vmode->cpp; // save clocks per pixel
	lay->mode = vmode->mode[inx]; // layer mode
	LayerSetW(inx, w); // set width of image, update parameters init, trans and wb
}

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
	s8 horiz, const int* mat, u8 col /* = 0 */)
{
	LayerSetup(inx, img, vmode, w, h, col, mat);
	sLayer* lay = &LayerScreen[inx]; // get pointer to layer
	lay->xbits = xbits;
	lay->ybits = ybits;
	lay->horiz = horiz;
}

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
	s16 x, s16 y, u16 w, u16 h, u8 col /* = 0 */)
{
	LayerSetup(inx, (const u8*)sprite, vmode, w, h, col);
	LayerSetX(inx, x);
	LayerSetY(inx, y);
	sLayer* lay = &LayerScreen[inx]; // get pointer to layer
	lay->spritenum = spritenum;	
}

// prepare array of start and length of lines (detects transparent pixels)
//  img ... image
//  x0 ... array of start of lines
//  w0 ... array of length of lines
//  w ... sprite width (slow sprite: max. width 255)
//  h ... sprite height
//  wb ... sprite pitch (bytes between lines)
//  col ... key color
//  fast ... fast sprite, divide start and length of line by 4
void SpritePrepLines(const u8* img, u8* x0, u8* w0, u16 w, u16 h, u16 wb, u8 col, Bool fast)
{
	int x1, x2, w2, y;
	const u8* d;

	// loop through lines
	for (y = 0; y < h; y++)
	{
		// find start of line
		d = &img[y*wb];
		for (x1 = 0; x1 < w; x1++)
		{
			if (*d != col) break;
			d++;
		}

		// find end of line
		d = &img[y*wb + w - 1];
		for (x2 = w; x2 > x1; x2--)
		{
			if (*d != col) break;
			d--;
		}

		// prepare start and length
		w2 = x2 - x1;
		if (fast)
		{
			w2 += ((x2 + 3) & ~3) - x2;
			x1 /= 4;
			w2 = (w2 + 3)/4;
		}
		if (x1 > 255) x1 = 255;
		if (w2 > 255) w2 = 255;

		// store start and length
		*x0++ = x1;
		*w0++ = w2;
	}
}

// sort fast sprite list by X coordinate
void SortSprite(sSprite** list, int num)
{
	int i;
	sSprite* s;
	sSprite* s2;
	for (i = 0; i < num-1; i++)
	{
		s = list[i];
		s2 = list[i+1];
		if (s->x > s2->x)
		{
			list[i] = s2;
			list[i+1] = s;
			if (i > 0) i -= 2;
		}
	}
}
