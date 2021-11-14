
// ****************************************************************************
//
//                                   Canvas
//
// ****************************************************************************

#ifndef _CANVAS_H
#define _CANVAS_H

#define DRAW_HWINTER	1	// 1=use hardware interpolator to draw images

// canvas format
//   Note: do not use enum, symbols could not be used by the preprocessor
#define CANVAS_8	0	// 8-bit pixels
#define CANVAS_4	1	// 4-bit pixels
#define CANVAS_2	2	// 2-bit pixels
#define CANVAS_1	3	// 1-bit pixels
#define CANVAS_PLANE2	4	// 4 colors on 2 planes
#define CANVAS_ATTRIB8	5	// 2x4 bit color attributes per 8x8 pixel sample
				//  draw functions:	bit 0..3 = draw color
				//			bit 4 = draw color is background color

// canvas descriptor
typedef struct {
	u8*	img;	// image data
	u8*	img2;	// image data 2 (2nd plane of CANVAS_PLANE2, attributes of CANVAS_ATTRIB8)
	int	w;	// width
	int	h;	// height
	int	wb;	// pitch (bytes between lines)
	u8	format;	// canvas format CANVAS_*
} sCanvas;

// Draw rectangle
void DrawRect(sCanvas* canvas, int x, int y, int w, int h, u8 col);

// Draw frame
void DrawFrame(sCanvas* canvas, int x, int y, int w, int h, u8 col);

// clear canvas (fill with black color)
void DrawClear(sCanvas* canvas);

// Draw point
void DrawPoint(sCanvas* canvas, int x, int y, u8 col);

// Draw line
void DrawLine(sCanvas* canvas, int x1, int y1, int x2, int y2, u8 col);

// Draw filled circle
//  x0, y0 ... coordinate of center
//  r ... radius
//  col ... color
//     col with CANVAS_ATTRIB8 format: bit 0..3 = draw color, bit 4 = draw color is background color
//  mask ... mask of used octants (0xff = 255 = draw whole circle)
//         . B2|B1 .
//       B3 .  |  . B0
//       ------o------
//       B4 .  |  . B7
//         . B5|B6 .
void DrawFillCircle(sCanvas* canvas, int x0, int y0, int r, u8 col, u8 mask=0xff);

// Draw circle
//  x0, y0 ... coordinate of center
//  r ... radius
//  col ... color
//     col with CANVAS_ATTRIB8 format: bit 0..3 = draw color, bit 4 = draw color is background color
//  mask ... mask of used octants (0xff = 255 = draw whole circle)
//         . B2|B1 .
//       B3 .  |  . B0
//       ------o------
//       B4 .  |  . B7
//         . B5|B6 .
void DrawCircle(sCanvas* canvas, int x0, int y0, int r, u8 col, u8 mask=0xff);

// Draw text (transparent background)
//   font = pointer to 1-bit font
void DrawText(sCanvas* canvas, const char* text, int x, int y, u8 col,
	const void* font, int fontheight=8, int scalex=1, int scaley=1);

// Draw text with background
//   font = pointer to 1-bit font
void DrawTextBg(sCanvas* canvas, const char* text, int x, int y, u8 col, u8 bgcol,
	const void* font, int fontheight=8, int scalex=1, int scaley=1);

// Draw image
void DrawImg(sCanvas* canvas, sCanvas* src, int xd, int yd, int xs, int ys, int w, int h);

// Draw image with transparency (source and destination must have same format, col = transparency key color)
//  CANVAS_ATTRIB8 format replaced by DrawImg function
void DrawBlit(sCanvas* canvas, sCanvas* src, int xd, int yd, int xs, int ys, int w, int h, u8 col);

// DrawImgMat mode
enum {
	DRAWIMG_WRAP,		// wrap image
	DRAWIMG_NOBORDER,	// no border (transparent border)
	DRAWIMG_CLAMP,		// clamp image (use last pixel as border)
	DRAWING_COLOR,		// color border
	DRAWIMG_TRANSP,		// transparent image with key color
	DRAWIMG_PERSP,		// perspective floor
};

// draw 8-bit image with 2D transformation matrix
//  canvas ... destination canvas
//  src ... source canvas with image
//  x ... destination coordinate X
//  y ... destination coordinate Y
//  w ... destination width
//  h ... destination height
//  m ... transformation matrix (should be prepared using PrepDrawImg or PrepDrawPersp function)
//  mode ... draw mode DRAWIMG_*
//  color ... key or border color
// Note to wrap and perspective mode: Width and height of source image must be power of 2!
void DrawImgMat(sCanvas* canvas, const sCanvas* src, int x, int y, int w, int h,
	const class cMat2Df* m, u8 mode, u8 color);

// draw tile map using perspective projection
//  canvas ... destination canvas
//  src ... source canvas with column of 8-bit square tiles (width = tile size, must be power of 2)
//  map ... byte map of tile indices
//  mapwbits ... number of bits of map width (number of tiles; width must be power of 2)
//  maphbits ... number of bits of map height (number of tiles; height must be power of 2)
//  tilebits ... number of bits of tile size (e.g. 5 = tile 32x32 pixel)
//  x ... destination coordinate X
//  y ... destination coordinate Y
//  w ... destination width
//  h ... destination height
//  mat ... transformation matrix (should be prepared using PrepDrawPersp function)
//  horizon ... horizon offset (0=do not use perspective projection)
void DrawTileMap(sCanvas* canvas, const sCanvas* src, const u8* map, int mapwbits, int maphbits,
	int tilebits, int x, int y, int w, int h, const cMat2Df* mat, u8 horizon);

// draw image line interpolated
//  canvas = destination canvas (8-bit pixel format)
//  src = source canvas (source image in 8-bit pixel format)
//  xd,yd = destination coordinates
//  xs,ys = source coordinates
//  wd = destination width
//  ws = source width
// Overflow in X direction is not checked!
void DrawImgLine(sCanvas* canvas, sCanvas* src, int xd, int yd, int xs, int ys, int wd, int ws);

#endif // _CANVAS_H
