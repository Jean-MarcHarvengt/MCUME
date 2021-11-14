
// ****************************************************************************
//
//                                 VGA output
//
// ****************************************************************************

#ifndef _VGA_H
#define _VGA_H

// scanline type
#define LINE_VSYNC	0	// long vertical sync
#define LINE_VVSYNC	1	// short vertical + vertical sync
#define LINE_VHSYNC	2	// short vertical + horizontal sync
#define LINE_HHSYNC	3	// short horizontal + horizontal sync
#define LINE_HVSYNC	4	// short horizontal + vertical sync
#define LINE_DARK	5	// dark line
#define LINE_IMG	6	// progressive image 0, 1, 2,...
#define LINE_IMGEVEN1	7	// interlaced image even 0, 2, 4,..., 1st subframe
#define LINE_IMGEVEN2	8	// interlaced image even 0, 2, 4,..., 2nd subframe
#define LINE_IMGODD1	9	// interlaced image odd 1, 3, 5,..., 1st subframe
#define LINE_IMGODD2	10	// interlaced image odd 1, 3, 5,..., 2nd subframe

extern u8 ScanlineType[MAXLINE];

extern int DispDev;	// current display device
extern sVmode CurVmode; // copy of current videomode table
//extern int LayerMode;	// current layer mode (LAYERMODE_*)
extern volatile int ScanLine;	// current scan line 1...
extern volatile u32 Frame;	// frame counter
extern volatile int BufInx;	// current buffer set (0..1)
extern volatile Bool VSync;	// current scan line is vsync or dark

// line buffers
extern ALIGNED u8	LineBuf1[DBUF_MAX]; // scanline 1 image data
extern ALIGNED u8	LineBuf2[DBUF_MAX]; // scanline 2 image data
extern int	LineBufSize[LAYERS_MAX]; // size of data buffers
extern u32	LineBufHsBp[4];		// HSYNC ... back porch-1 ... IRQ command ... image command
extern u32	LineBufFp;		// front porch+1
extern u32	LineBufDark[2];		// HSYNC ... dark line
extern u32	LineBufSync[10];	// vertical synchronization
				//  interlaced (5x half scanlines):
				//	2x half synchronization (HSYNC pulse/2 ... line dark/2)
				//	2x vertical synchronization (invert line dark/2 ... invert HSYNC pulse)
				//	1x half synchronization (HSYNC pulse/2 ... line dark/2)
				// progressive: 1x scanline with vertical synchronization (invert line dark ... invert HSYNC pulse)

extern	ALIGNED u8	LineBuf0[BLACK_MAX]; // line buffer with black color (used to clear rest of scanline)

// control buffers
extern u32	CtrlBuf1[CBUF_MAX];	// control pairs: u32 count, read address (must be terminated with [0,0])
extern u32	CtrlBuf2[CBUF_MAX];	// control pairs: u32 count, read address (must be terminated with [0,0])

extern int	CtrlBufSize[LAYERS_MAX]; // size of control buffers

// render font pixel mask
extern u32 RenderTextMask[512];

// fill memory buffer with u32 words
//  buf ... data buffer, must be 32-bit aligned
//  data ... data word to store
//  num ... number of 32-bit words (= number of bytes/4)
// Returns new destination address.
extern "C" u32* MemSet4(u32* buf, u32 data, int num);

// blit scanline using key color
//  dst ... destination buffer
//  src ... source buffer
//  w ... width
//  key ... key color
extern "C" void BlitKey(u8* dst, u8* src, int w, u8 key);

// render layers with sprites LAYERMODE_SPRITE*
//  dbuf ... pointer to data buffer
//  y ... coordinate of scanline
//  scr ... pointer to layer screen structure sLayer
extern "C" void RenderSprite(u8* dbuf, int y, sLayer* scr);

// render layers with fast sprites LAYERMODE_FASTSPRITE*
//  cbuf ... pointer to control buffer
//  y ... coordinate of scanline
//  scr ... pointer to layer screen structure sLayer
//  buf ... pointer to destination data buffer with transparent color
// Output new pointer to control buffer.
extern "C" u32* RenderFastSprite(u32* cbuf, int y, sLayer* scr, u8* buf);

// render layers with transformation matrix LAYERMODE_PERSP*
//  R0 ... dbuf pointer to data buffer
//  R1 ... y coordinate of scanline (relative in destination image)
//  R2 ... scr pointer to layer screen structure sLayer
extern "C" void RenderPersp(u8* dbuf, int y, sLayer* scr);

// render layers double pixel with transformation matrix LAYERMODE_PERSP2*
//  R0 ... dbuf pointer to data buffer
//  R1 ... y coordinate of scanline (relative in destination image)
//  R2 ... scr pointer to layer screen structure sLayer
extern "C" void RenderPersp2(u8* dbuf, int y, sLayer* scr);

// render scanline
//  cbuf ... control buffer
//  dbuf ... data buffer (pixel data)
//  line ... current line 0..
//  pixnum ... total pixels (must be multiple of 4)
// Returns new pointer to control buffer
extern "C" u32* Render(u32* cbuf, u8* dbuf, int line, int pixnum);

// initialize scanline type table
void ScanlineTypeInit(const sVmode* v);

// print table if scanline types
void ScanlineTypePrint(const u8* scan, int lines);

// initialize videomode (returns False on bad configuration)
// - All layer modes must use same layer program (LAYERMODE_BASE = overlapped layers are OFF)
void VgaInit(const sVmode* vmode); //, u8 layer1mode=LAYERMODE_BASE, u8 layer2mode=LAYERMODE_BASE, u8 layer3mode=LAYERMODE_BASE);

// VGA core
void VgaCore();

// request to initialize VGA videomode, NULL=only stop driver (wait to initialization completes)
void VgaInitReq(const sVmode* vmode);

// execute core 1 remote function
void Core1Exec(void (*fnc)());

// check if core 1 is busy (executing remote function)
Bool Core1Busy();

// wait if core 1 is busy (executing remote function)
void Core1Wait();

// wait for VSync scanline
void WaitVSync();

#endif // _VGA_H
