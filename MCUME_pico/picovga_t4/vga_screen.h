
// ****************************************************************************
//
//                               VGA screen layout
//
// ****************************************************************************

#ifndef _VGA_SCREEN_H
#define _VGA_SCREEN_H

// video segment (on change update SSEGM_* in define.h)
typedef struct {
	u16	width;	// SSEGM_WIDTH width of this video segment in pixels (must be multiple of 4, 0=inactive segment)
	u16	wb;	// SSEGM_WB pitch - number of bytes between lines
	s16	offx;	// SSEGM_OFFX display offset at X direction (must be multiple of 4)
	s16	offy;	// SSEGM_OFFY display offset at Y direction
	u16	wrapx;	// SSEGM_WRAPX wrap width in X direction (number of pixels, must be multiply of 4 and > 0)
			//	text modes: wrapx must be multiply of 8
	u16	wrapy;	// SSEGM_WRAPY wrap width in Y direction (number of lines, cannot be 0)
	const void* data; // SSEGM_DATA pointer to video buffer with image data
	u8	form;	// SSEGM_FORM graphics format GF_*
	bool	dbly;	// SSEGM_DBLY double Y (2 scanlines per 1 image line)
	u16	par3;	// SSEGM_PAR3 parameter 3
	u32	par;	// SSEGM_PAR parameter 1
	u32	par2;	// SSEGM_PAR2 parameter 2
} sSegm;

// video strip (on change update SSTRIP_* in define.h)
typedef struct {
	u16	height;		// SSTRIP_HEIGHT height of this strip in number of scanlines
	u16	num;		// SSTRIP_NUM number of video segments
	sSegm	seg[SEGMAX];	// SSTRIP_SEG list of video segments
} sStrip;

// video screen (on change update SSCREEN_* in define.h)
typedef struct {
	u16	num;		// SSCREEN_NUM number of video strips
	u16	backup;		// SSCREEN_BACKUP backup number of video strips during display OFF
	sStrip	strip[STRIPMAX]; // SSCREEN_STRIP list of video strips
} sScreen;

// current video screen
extern sScreen Screen;		// default video screen
extern sScreen* pScreen;	// pointer to current video screen

// clear screen (set 0 strips, does not modify sprites)
void ScreenClear(sScreen* s);

// add empty strip to the screen (returns pointer to the strip)
sStrip* ScreenAddStrip(sScreen* s, int height);

// add empty segment to video strip (returns pointer to the segment and initialises is to defaults)
sSegm* ScreenAddSegm(sStrip* strip, int width);

// set video segment to simple color format GF_COLOR
//  col1 = color pattern 4-pixels even line (use macro MULTICOL)
//  col2 = color pattern 4-pixels odd line (use macro MULTICOL)
void ScreenSegmColor(sSegm* segm, u32 col1, u32 col2);

// set video segment to gradient with 1 line
//   data = pointer to data buffer with gradient
//   wb = pitch - length of buffer
// To scroll gradient, set virtual dimension wrapx, then shift offx
void ScreenSegmGrad1(sSegm* segm, const void* data, int wb);

// set video segment to gradient with 2 lines
//   data = pointer to data buffer with gradient
//   wb = pitch - lenght of buffer
// To scroll gradient, set virtual dimension wrapx, then shift offx
void ScreenSegmGrad2(sSegm* segm, const void* data, int wb);

// set video segment to native 8-bit graphics (R3G3B2)
//   data = pointer to data buffer
//   wb = pitch - number of bytes between lines
// To scroll image, set virtual dimension wrapx and wrapy, then shift offx and offy.
void ScreenSegmGraph8(sSegm* segm, const void* data, int wb);

// generate 16-color palette translation table
//  trans = pointer to destination palette translation table (u16 trans[256])
//  pal = pointer to source palette of 16 colors (u8 pal[16])
void GenPal16Trans(u16* trans, const u8* pal);

// set video segment to 4-bit palette graphics
//   data = pointer to data buffer
//   trans = pointer to 16-color palette translation table (generated with GenPal16Trans function)
//   wb = pitch - number of bytes between lines
// To scroll image, set virtual dimension wrapx and wrapy, then shift offx and offy.
void ScreenSegmGraph4(sSegm* segm, const void* data, const void* trans, int wb);

// generate palette 4 translation table for function ScreenSegmGraph2
//  trans = pointer to destination palette translation table (u32 trans[256])
//  pal = pointer to source palette of 4 colors (u8 pal[4])
void GenPal4Trans(u32* trans, const u8* pal);

// set video segment to 2-bit palette graphics
//   data = pointer to data buffer
//   trans = pointer to 4-color palette translation table (generated with GenPal4Trans function)
//   wb = pitch - number of bytes between lines
// To scroll image, set virtual dimension wrapx and wrapy, then shift offx and offy.
void ScreenSegmGraph2(sSegm* segm, const void* data, const void* trans, int wb);

// set video segment to 1-bit palette graphics
//   data = pointer to data buffer
//   bg = background color
//   fg = foreground color
//   wb = pitch - number of bytes between lines
// To scroll image, set virtual dimension wrapx and wrapy, then shift offx and offy.
void ScreenSegmGraph1(sSegm* segm, const void* data, u8 bg, u8 fg, int wb);

// set video segment to 8-pixel mono text
//   data = pointer to text buffer
//   font = pointer to 1-bit font of 256 characters of width 8 (total width of image 2048 pixels)
//   fontheight = font height
//   bg = background color
//   fg = foreground color
//   wb = pitch - number of bytes between text lines
void ScreenSegmMText(sSegm* segm, const void* data, const void* font, u16 fontheight, u8 bg, u8 fg, int wb);

// set video segment to 8-pixel attribute text
//   data = pointer to text buffer (character + 2x4 bit attributes)
//   font = pointer to 1-bit font of 256 characters of width 8 (total width of image 2048 pixels)
//   fontheight = font height
//   pal = pointer to palette of 16 colors
//   wb = pitch - number of bytes between text lines
void ScreenSegmAText(sSegm* segm, const void* data, const void* font, u16 fontheight, const void* pal, int wb);

// set video segment to 8-pixel foreground color text
//   data = pointer to text buffer (character + foreground color)
//   font = pointer to 1-bit font of 256 characters of width 8 (total width of image 2048 pixels)
//   fontheight = font height
//   bg = background color
//   wb = pitch - number of bytes between text lines
void ScreenSegmFText(sSegm* segm, const void* data, const void* font, u16 fontheight, u8 bg, int wb);

// set video segment to 8-pixel color text
//   data = pointer to text buffer (character + background color + foreground color)
//   font = pointer to 1-bit font of 256 characters of width 8 (total width of image 2048 pixels)
//   fontheight = font height
//   wb = pitch - number of bytes between text lines
void ScreenSegmCText(sSegm* segm, const void* data, const void* font, u16 fontheight, int wb);

// set video segment to 8-pixel gradient color text
//   data = pointer to text buffer (character + foreground color)
//   font = pointer to 1-bit font of 256 characters of width 8 (total width of image 2048 pixels)
//   fontheight = font height
//   bg = background color
//   grad = pointer to array of gradient colors
//   wb = pitch - number of bytes between text lines
void ScreenSegmGText(sSegm* segm, const void* data, const void* font, u8 fontheight, u8 bg, const void* grad, int wb);

// set video segment to 8-pixel double gradient color text
//   data = pointer to text buffer (character + foreground color)
//   font = pointer to 1-bit font of 256 characters of width 8 (total width of image 2048 pixels)
//   fontheight = font height
//   bg = background color
//   grad = pointer to array of gradient colors
//   wb = pitch - number of bytes between text lines
void ScreenSegmDText(sSegm* segm, const void* data, const void* font, u8 fontheight, u8 bg, const void* grad, int wb);

// set video segment to tiles
//   data = pointer to tile map buffer (with tile indices)
//   tiles = pointer to 1 column of tiles, 1 pixel = 8 bits
//   w = tile width (must be multiple of 4)
//   h = tile height
//   wb = pitch - number of bytes between tile map rows
void ScreenSegmTile(sSegm* segm, const void* data, const void* tiles, int w, int h, int wb);

// set video segment to alternate tiles
//   data = pointer to tile map buffer (with tile indices)
//   tiles = pointer to 1 row of tiles, 1 pixel = 8 bits
//   w = tile width (must be multiple of 4)
//   h = tile height
//   tilewb = tile width bytes (usually tile width * number of tiles)
//   wb = pitch - number of bytes between tile map rows
void ScreenSegmTile2(sSegm* segm, const void* data, const void* tiles, int w, int h, int tilewb, int wb);

// set video segment to level graph GF_LEVEL
//   data = pointer to buffer with line samples 0..255
//   zero = Y zero level
//   bg = background color
//   fg = foreground color
void ScreenSegmLevel(sSegm* segm, const void* data, u8 zero, u8 bg, u8 fg);

// set video segment to leve gradient graph GF_LEVELGRAD
//   data = pointer to buffer with values 0..255 of 4-pixels in rows
//   sample1 = scanline sample < data
//   sample2 = scanline sample >= data
void ScreenSegmLevelGrad(sSegm* segm, const void* data, const void* sample1, const void* sample2);

// set video segment to oscilloscope 1-pixel graph GF_OSCIL
//   data = pointer to buffer with line samples 0..255
//   bg = background color
//   fg = foreground color
//   pixh = height of pixels - 1
void ScreenSegmOscil(sSegm* segm, const void* data, u8 bg, u8 fg, int pixh);

// set video segment to oscilloscope line graph GF_OSCLINE
//   data = pointer to buffer with line samples 0..255
//   bg = background color
//   fg = foreground color
void ScreenSegmOscLine(sSegm* segm, const void* data, u8 bg, u8 fg);

// generate palette 4-color translation table for function ScreenSegmPlane2
//  trans = pointer to destination palette translation table (u32 trans[256])
//  pal = pointer to source palette of 4 colors (u8 pal[4])
void GenPal4Plane(u32* trans, const u8* pal);

// set video segment to 4-color on 2-planes graphics
//   data = pointer to data buffer
//   plane = offset of 2nd graphics plane (in bytes), size of one graphics plane
//   trans = pointer to 4-color palette translation table (generated with GenPal4Plane function)
//   wb = pitch - number of bytes between lines
// To scroll image, set virtual dimension wrapx and wrapy, then shift offx and offy.
void ScreenSegmPlane2(sSegm* segm, const void* data, int plane, const void* trans, int wb);

// set video segment to 2x4 bit color attribute per 8x8 pixel sample graphics
//   data = pointer to data buffer with mono pixels
//   attr = pointer to color attributes
//   pal = pointer to 16-color palette table
//   wb = pitch - number of bytes between lines
// To scroll image, set virtual dimension wrapx and wrapy, then shift offx and offy.
void ScreenSegmAttrib8(sSegm* segm, const void* data, const void* attr, const u8* pal, int wb);

// set video segment to horizontal progress indicator GF_PROGRESS
//   data = pointer to buffer with values 0..255 of 4-pixels in rows
//   sample1 = scanline sample < data
//   sample2 = scanline sample >= data
void ScreenSegmProgress(sSegm* segm, const void* data, const void* sample1, const void* sample2);

// set video segment to 8-bit graphics with 2D matrix transformation
//   data = pointer to image data (width and height of image must be power of 2)
//   mat = pointer to array of 6 matrix integer parameters m11, m12...m23 (exported with ExportInt function)
//   xbits = number of bits of image width (image width must be power of 2 and must be = pitch width bytes)
//   ybits = number of bits of image height (image height must be power of 2)
// Use default settings of parameters: offx = 0, offy = 0, wrapx = segment width, wrapy = segment height
void ScreenSegmGraph8Mat(sSegm* segm, const void* data, const int* mat, u16 xbits, u16 ybits);

// set video segment to 8-bit graphics with perspective projection
//   data = pointer to image data (width and height of image must be power of 2)
//   mat = pointer to array of 6 matrix integer parameters m11, m12...m23 (exported with ExportInt function)
//   xbits = number of bits of image width (image width must be power of 2 and must be = pitch width bytes)
//   ybits = number of bits of image height (image height must be power of 2)
//   horiz = horizon offset
// Use default settings of parameters: offx = 0, offy = 0, wrapx = segment width, wrapy = segment height
void ScreenSegmGraph8Persp(sSegm* segm, const void* data, const int* mat, u16 xbits, u16 ybits, u16 horiz);

// set video segment to tiles with perspective
//   map = pointer to tile map with tile indices (width and height must be power of 2)
//   tiles = pointer to 1 column of square tiles, 1 pixel = 8 bits (width and height must be power of 2)
//   mat = pointer to array of 6 matrix integer parameters m11, m12...m23 (exported with ExportInt function)
//   mapwbits = number of bits of tile map width
//   maphbits = number of bits of tile map height
//   tilebits = number of bits of tile width and height
//   horizon = horizon offset/4 (0=do not use perspective projection, <0=vertical flip to display ceiling)
// Use default settings of parameters: offx = 0, offy = 0, wrapx = segment width, wrapy = segment height
void ScreenSegmTilePersp(sSegm* segm, const u8* map, const u8* tiles, const int* mat, 
	u8 mapwbits, u8 maphbits, u8 tilebits, s8 horizon);

// set video segment to tiles with perspective, 1.5 pixels
//   map = pointer to tile map with tile indices (width and height must be power of 2)
//   tiles = pointer to 1 column of square tiles, 1 pixel = 8 bits (width and height must be power of 2)
//   mat = pointer to array of 6 matrix integer parameters m11, m12...m23 (exported with ExportInt function)
//   mapwbits = number of bits of tile map width
//   maphbits = number of bits of tile map height
//   tilebits = number of bits of tile width and height
//   horizon = horizon offset/4 (0=do not use perspective projection, <0=vertical flip to display ceiling)
// Use default settings of parameters: offx = 0, offy = 0, wrapx = segment width, wrapy = segment height
void ScreenSegmTilePersp15(sSegm* segm, const u8* map, const u8* tiles, const int* mat, 
	u8 mapwbits, u8 maphbits, u8 tilebits, s8 horizon);

// set video segment to tiles with perspective, double pixels
//   map = pointer to tile map with tile indices (width and height must be power of 2)
//   tiles = pointer to 1 column of square tiles, 1 pixel = 8 bits (width and height must be power of 2)
//   mat = pointer to array of 6 matrix integer parameters m11, m12...m23 (exported with ExportInt function)
//   mapwbits = number of bits of tile map width
//   maphbits = number of bits of tile map height
//   tilebits = number of bits of tile width and height
//   horizon = horizon offset/4 (0=do not use perspective projection, <0=vertical flip to display ceiling)
// Use default settings of parameters: offx = 0, offy = 0, wrapx = segment width, wrapy = segment height
void ScreenSegmTilePersp2(sSegm* segm, const u8* map, const u8* tiles, const int* mat, 
	u8 mapwbits, u8 maphbits, u8 tilebits, s8 horizon);

// set video segment to tiles with perspective, triple pixels
//   map = pointer to tile map with tile indices (width and height must be power of 2)
//   tiles = pointer to 1 column of square tiles, 1 pixel = 8 bits (width and height must be power of 2)
//   mat = pointer to array of 6 matrix integer parameters m11, m12...m23 (exported with ExportInt function)
//   mapwbits = number of bits of tile map width
//   maphbits = number of bits of tile map height
//   tilebits = number of bits of tile width and height
//   horizon = horizon offset/4 (0=do not use perspective projection, <0=vertical flip to display ceiling)
// Use default settings of parameters: offx = 0, offy = 0, wrapx = segment width, wrapy = segment height
void ScreenSegmTilePersp3(sSegm* segm, const u8* map, const u8* tiles, const int* mat, 
	u8 mapwbits, u8 maphbits, u8 tilebits, s8 horizon);

// set video segment to tiles with perspective, quadruple pixels
//   map = pointer to tile map with tile indices (width and height must be power of 2)
//   tiles = pointer to 1 column of square tiles, 1 pixel = 8 bits (width and height must be power of 2)
//   mat = pointer to array of 6 matrix integer parameters m11, m12...m23 (exported with ExportInt function)
//   mapwbits = number of bits of tile map width
//   maphbits = number of bits of tile map height
//   tilebits = number of bits of tile width and height
//   horizon = horizon offset/4 (0=do not use perspective projection, <0=vertical flip to display ceiling)
// Use default settings of parameters: offx = 0, offy = 0, wrapx = segment width, wrapy = segment height
void ScreenSegmTilePersp4(sSegm* segm, const u8* map, const u8* tiles, const int* mat, 
	u8 mapwbits, u8 maphbits, u8 tilebits, s8 horizon);

#endif // _VGA_SCREEN_H
