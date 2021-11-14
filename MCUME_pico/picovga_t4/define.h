
// ****************************************************************************
//                                 
//                     VGA common definitions of C and ASM
//
// ****************************************************************************

#include "vga_config.h"		// VGA configuration

#define LAYERS_MAX	4	// max. number of layers (should be 4)

#define BLACK_MAX	MAXX	// size of buffer with black color (used to clear rest of unused line)

// VGA PIO program
#define BASE_OFFSET	17	// offset of base layer program
#define LAYER_OFFSET	0	// offset of overlapped layer program

// layer program
#define LAYERPROG_BASE	0	// program of base layer (overlapped layers are OFF)
#define LAYERPROG_KEY	1	// layer with key color
#define LAYERPROG_BLACK	2	// layer with black key color
#define LAYERPROG_WHITE	3	// layer with white key color
#define LAYERPROG_MONO	4	// layer with mono pattern or simple color
#define LAYERPROG_RLE	5	// layer with RLE compression

#define LAYERPROG_NUM	6	// number of layer programs

// layer mode (CPP = clock cycles per pixel)
//	Control buffer: 16 bytes
//	Data buffer: 4 bytes
// fast sprites can be up Control buffer: width*2 bytes
// sprites Data buffer: width bytes
#define LAYERMODE_BASE		0	// base layer
#define LAYERMODE_KEY		1	// layer with key color
#define LAYERMODE_BLACK		2	// layer with black key color
#define LAYERMODE_WHITE		3	// layer with white key color
#define LAYERMODE_MONO		4	// layer with mono pattern
#define LAYERMODE_COLOR		5	// layer with simple color
#define LAYERMODE_RLE		6	// layer with RLE compression
#define LAYERMODE_SPRITEKEY	7	// layer with sprites with key color
#define LAYERMODE_SPRITEBLACK	8	// layer with sprites with black key color
#define LAYERMODE_SPRITEWHITE	9	// layer with sprites with white key color
#define LAYERMODE_FASTSPRITEKEY	10	// layer with fast sprites with key color
#define LAYERMODE_FASTSPRITEBLACK 11	// layer with fast sprites with black key color
#define LAYERMODE_FASTSPRITEWHITE 12	// layer with fast sprites with white key color
#define LAYERMODE_PERSPKEY	13	// layer with key color and image with transformation matrix
#define LAYERMODE_PERSPBLACK	14	// layer with black key color and image with transformation matrix
#define LAYERMODE_PERSPWHITE	15	// layer with white key color and image with transformation matrix
#define LAYERMODE_PERSP2KEY	16	// layer with key color and double-pixel image with transformation matrix
#define LAYERMODE_PERSP2BLACK	17	// layer with black key color and double-pixel image with transformation matrix
#define LAYERMODE_PERSP2WHITE	18	// layer with white key color and double-pixel image with transformation matrix

#define LAYERMODE_NUM	19	// number of overlapped layer modes

// Structure of sprite sSprite (on change update structure sSprite in vga_layer.h)
#define SSPRITE_IMG	0	// u8*	img;	// pointer to image data
#define SSPRITE_X0	4	// u8*	x0;	// pointer to pixel offset of start of lines/4 (used with fast sprites)
#define SSPRITE_W0	8	// u8*	w0;	// pointer to pixel length of length of lines/4 (used with fast sprites)
#define SSPRITE_KEYCOL	12	// u32	keycol;	// key color
#define SSPRITE_X	16	// s16	x;	// sprite X-coordinate on the screen
#define SSPRITE_Y	18	// s16	y;	// sprite Y-coordinate on the screen
#define SSPRITE_W	20	// u16	w;	// sprite width
#define SSPRITE_H	22	// u16	h;	// sprite height
#define SSPRITE_WB	24	// u16	wb;	// sprite pitch (number of bytes between lines)
				// u16	res;	// ...reserved, structure align
#define	SSPRITE_SIZE	28	// size of sSprite structure

// Structure of layer screen sLayer (on change update structure sLayer in vga_layer.h)
#define SLAYER_IMG	0	// const u8*	img;	// pointer to image in current layer format, or sprite list
#define SLAYER_PAR	4	// const void* par;	// additional parameter (RLE index table, transformation matrix)
#define SLAYER_INIT	8	// u32 	init;	// init word sent on start of scanline
#define SLAYER_KEYCOL	12	// u32	keycol;	// key color
#define SLAYER_TRANS	16	// u16	trans;	// trans count
#define SLAYER_X	18	// s16	x;	// start X coordinate
#define SLAYER_Y	20	// s16	y;	// start Y coordinate
#define SLAYER_W	22	// u16	w;	// width in pixels
#define SLAYER_H	24	// u16	h;	// height
#define SLAYER_WB	26	// u16	wb;	// image width in bytes (pitch of lines)
#define SLAYER_MODE	28	// u8	mode;	// layer mode
#define SLAYER_HORIZ	29	// s8	horiz;	// horizon of perspective projection/4 (only with LAYERMODE_PERSP* modes, 0=no perspecitve, <0 ceilling)
#define SLAYER_XBITS	30	// u8	xbits;	// number of bits of width of source image (only with LAYERMODE_PERSP* modes)
#define SLAYER_YBITS	31	// u8	ybits;	// number of bits of height of source image (only with LAYERMODE_PERSP* modes)
#define SLAYER_SPRITENUM 32	// u16	spritenum; // number of sprites
#define SLAYER_ON	34	// Bool on;	// layer is ON
#define SLAYER_CPP	35	// u8	cpp;	// current clock pulses per pixel (used to calculate X coordinate)
#define SLAYER_SIZE	36	// size of sLayer structure

// Structure of video segment sSegm (on change update structure sSegm in vga_screen.h)
#define SSEGM_WIDTH	0	// u16	width;	// width of this video segment in pixels (must be multiple of 4, 0=inactive segment)
#define SSEGM_WB	2	// u16	wb;	// pitch - number of bytes between lines
#define SSEGM_OFFX	4	// s16	offx;	// display offset at X direction (must be multiple of 4)
#define SSEGM_OFFY	6	// s16	offy;	// display offset at Y direction
#define SSEGM_WRAPX	8	// u16	wrapx;	// wrap width in X direction (number of pixels, must be multiply of 4 and > 0)
				//	text modes: wrapx must be multiply of 8
#define SSEGM_WRAPY	10	// u16	wrapy;	// wrap width in Y direction (number of lines, cannot be 0)
#define SSEGM_DATA	12	// const void* data; // pointer to video buffer with image data
#define SSEGM_FORM	16	// u8	form;	// graphics format GF_*
#define SSEGM_DBLY	17	// bool dbly;	// double Y (2 scanlines per 1 image line)
#define SSEGM_PAR3	18	// u16	par3;	// SSEGM_PAR3 parameter 3
#define SSEGM_PAR	20	// u32	par;	// parameter 1: color, pointer to palettes, tile source, font
#define SSEGM_PAR2	24	// u32	par2;	// parameter 2
#define SSEGM_SIZE	28	// size of sSegm structure

// Structure of video strip sStrip (on change update structure sStrip in vga_screen.h)
#define SSTRIP_HEIGHT	0	// u16	height;		// height of this strip in number of scanlines
#define SSTRIP_NUM	2	// u16	num;		// number of video segments
#define SSTRIP_SEG	4	// sSegm	seg[SEGMAX];
#define SSTRIP_SIZE	(4+SSEGM_SIZE*SEGMAX) // size of sStrip structure (= 4 + 28*8 = 228 bytes)

// Structure of video screen sScreen (on change update structure sScreen in vga_screen.h)
#define SSCREEN_NUM	0	// u16	num;		// number of video strips
#define SSCREEN_BACKUP	2	// u16	num_backup;	// backup number of video strips during display OFF
#define SSCREEN_STRIP	4	// sStrip	strip[STRIPMAX]; // list of video strips
#define SSCREEN_SIZE	(4+SSTRIP_SIZE*STRIPMAX) // size of sScreen structure (= 4 + 228*8 = 1828 bytes)

// --- graphics formats
// There are 3 groups of formats - separated due internal reasons, do not mix them.

// 1st group of formats - rendered specially
#define GF_COLOR	0	// simple color (par=color pattern 4-pixels even line, par2=color pattern 4-pixels odd line)
//	Data buffer: width bytes (320 pixels: 320 bytes)
//	Control buffer: 8 bytes

// 2nd group of formats - rendering into control buffer cbuf
#define GF_GRAPH8	1	// native 8-bit graphics (X1Y1R2G2B2) - fast, transfers "as is" to PIO 
				//	(num = number of pixels/4 = number of bytes/4)
//	Control buffer: 8 bytes (320 pixels: 8 bytes)
#define GF_TILE		2	// tiles (par = tile table with one column of tiles,
				//	par2 = tile height, par3 = tile width as multiple of 4)
//	Control buffer: width/tile width*8 bytes (320 pixels of 32x32: 80 bytes)
#define GF_TILE2	3	// alternate tiles (par = tile table with one row of tiles,
				//	par2 = LOW tile height, HIGH tile width bytes,
				//	par3 = tile width as multiple of 4)
//	Control buffer: width/tile width*8 bytes (320 pixels of 32x32: 80 bytes)
#define GF_PROGRESS	4	// horizontal progress indicator (data = values 0..255 of 4-pixels in rows,
				//    par = scanline gradient < data, par2 = scanline gradient >= data)
//	Control buffer: 16 bytes
#define GF_GRAD1	5	// gradient with 1 line
//	Control buffer: 8 bytes (320 pixels: 8 bytes)
#define GF_GRAD2	6	// gradient with 2 lines
//	Control buffer: 8 bytes (320 pixels: 8 bytes)

#define GF_GRP2MIN	GF_GRAPH8	// 2nd group minimal format
#define GF_GRP2MAX	GF_GRAD2	// 2nd group maximal format

// 3rd group of formats - rendering into data buffer dbuf
//	Control buffer: 8 bytes
//	Data buffer: width bytes
#define GF_GRAPH4	7	// 4-bit graphics (num = number of pixels/4 = number of bytes/2;
				//	par = pointer to 16-color palette translation table)
#define GF_GRAPH2	8	// 2-bit graphics (num = number of pixels/4 = number of bytes,
				//	par = pointer to 4-color palette translation table)
#define GF_GRAPH1	9	// 1-bit graphics (num = number of pixels/8 = number of bytes,
				//	par = 2 colors of palettes)
#define GF_MTEXT	10	// 8-pixel mono text (num = number of characters, font is 8-bit width,
				//	par = pointer to 1-bit font, par2 = 2 colors of palettes)
#define GF_ATEXT	11	// 8-pixel attribute text, character + 2x4 bit attributes 
				//      (num = number of characters, font is 8-bit width,
				//	par = pointer to 1-bit font, par2 = pointer to 16 colors of palettes)
#define GF_FTEXT	12	// 8-pixel foreground color text, character + foreground color
				//      (num = number of characters, font is 8-bit width,
				//	par = pointer to 1-bit font, par2 = background color)
#define GF_CTEXT	13	// 8-pixel color text, character + background color + foreground color
				//      (num = number of characters, font is 8-bit width,
				//	par = pointer to 1-bit font)
#define GF_GTEXT	14	// 8-pixel gradient text (par = pointer to 1-bit font, par2 = pointer to color array)
#define GF_DTEXT	15	// 8-pixel double gradient text (par = pointer to 1-bit font, par2 = pointer to color array)
#define GF_LEVEL	16	// level graph (data=samples 0..255, par = 2 colors of palettes, par2 = Y zero level 0..255)
#define GF_LEVELGRAD	17	// level gradient graph (data = samples 0..255, par = scanline gradient < data, par2 = scanline gradient >= data)
#define GF_OSCIL	18	// oscilloscope pixel graph (data=samples 0..255, par = 2 colors of palettes, par2 = height of pixels - 1)
#define GF_OSCLINE	19	// oscilloscope line graph (data=samples 0..255, par = 2 colors of palettes)
#define GF_PLANE2	20	// 4 colors on 2 graphic planes (data=graphic, par=offset of 2nd graphic plane,
				//	par2 = pointer to 4-color palette translation table)
#define GF_ATTRIB8	21	// 2x4 bit color attribute per 8x8 pixel sample (data=mono graphic, par=offset of color attributes,
				//	par2 = pointer to 16-color palette table)
#define GF_GRAPH8MAT	22	// 8-bit graphics with 2D matrix transformation, using hardware interpolator inter1 (inter1 state is not saved during interrup)
				//	(data=image, par=pointer to 6 matrix integer parameters m11,m12..m23 ((int)(m*FRACTMUL)),
				//	par2 LOW=number of bits of image width, par2 HIGH=number of bits of image height)
#define GF_GRAPH8PERSP	23	// 8-bit graphics with perspective, using hardware interpolator inter1 (inter1 state is not saved during interrup)
				//	(data=image, par=pointer to 6 matrix integer parameters m11,m12..m23 ((int)(m*FRACTMUL)),
				//	par2 LOW=number of bits of image width, par2 HIGH=number of bits of image height,
				//	par3=horizon offset)
#define GF_TILEPERSP	24	// tiles with perspective, using hardware interpolators inter0 and inter1 (their state is not saved during interrup)
				//	(data=tile map, par=one column of tiles, par2=pointer to integer matrix,
				//	wb LOW=number of bits of map width, wb HIGH=number of bits of map height,
				//	par3 LOW=number of bits of tile size, par3 HIGH=horizon offset/4 or 0=no perspective or <0=ceilling,
				//	wrapy=segment height)
#define GF_TILEPERSP15	25	// tiles with perspective, 1.5 pixels (parameters as GF_TILEPERSP)
#define GF_TILEPERSP2	26	// tiles with perspective, double pixels (parameters as GF_TILEPERSP)
#define GF_TILEPERSP3	27	// tiles with perspective, triple pixels (parameters as GF_TILEPERSP)
#define GF_TILEPERSP4	28	// tiles with perspective, quadruple pixels (parameters as GF_TILEPERSP)

#define GF_GRP3MIN	GF_GRAPH4	// 3rd group minimal format
#define GF_GRP3MAX	GF_TILEPERSP4	// 3rd group maximal format


#define FRACT		12	// number of bits of fractional part of fractint number (use max. 13, min. 8)
#define FRACTMUL (1<<FRACT)
