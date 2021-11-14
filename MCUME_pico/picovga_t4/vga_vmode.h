
// ****************************************************************************
//
//                                 VGA videomodes
//
// ****************************************************************************

#ifndef _VGA_VMODE_H
#define _VGA_VMODE_H

#define VIDEO_NAME_LEN	5	// length of video timing name

// video timings
typedef struct {
	// horizontal
	float	htot;		// total scanline in [us]
	float	hfront;		// H front porch (after image, before HSYNC) in [us]
	float	hsync;		// H sync pulse in [us]
	float	hback;		// H back porch (after HSYNC, before image) in [us]
	float	hfull;		// H full visible in [us] (corresponding to 'wfull' pixels)

	// vertical
	u16	vtot;		// total scanlines (both subframes)
	u16	vmax;		// maximal height

	// subframe 1
	u16	vsync1;		// V sync (half-)pulses on subframe 1
	u16	vpost1;		// V sync post half-pulses on subframe 1
	u16	vback1;		// V back porch (after VSYNC, before image) on subframe 1
	u16	vact1;		// active visible scanlines, subframe 1
	u16	vfront1;	// V front porch (after image, before VSYNC) on subframe 1
	u16	vpre1;		// V sync pre half-pulses on subframe 1

	// subframe 2 (ignored if not interlaced)
	u16	vsync2;		// V sync half-pulses on subframe 2
	u16	vpost2;		// V sync post half-pulses on subframe 2
	u16	vback2;		// V back porch (after VSYNC, before image) on subframe 2
	u16	vact2;		// active visible scanlines, subframe 2
	u16	vfront2;	// V front porch (after image, before VSYNC) on subframe 2
	u16	vpre2;		// V sync pre half-pulses on subframe 2

	// name
	const char* name;	// video timing name (VIDEO_NAME_LEN characters + terminating 0)

	// flags
	bool	inter;		// interlaced (use subframes)
	bool	psync;		// positive synchronization
	bool	odd;		// first sub-frame is odd lines 1, 3, 5,... (PAL)
} sVideo;

// === TV videomodes

// TV PAL interlaced 5:4 720x576 (4:3 768x576, 16:9 1024x576)
extern const sVideo VideoPAL;

// TV PAL progressive 5:4 360x288 (4:3 384x288, 16:9 512x288)
extern const sVideo VideoPALp;

// TV NTSC interlaced 4:3 640x480 (5:4 600x480, 16:9 848x480)
extern const sVideo VideoNTSC;

// TV NTSC progressive 4:3 320x240 (5:4 300x240, 16:9 424x240)
extern const sVideo VideoNTSCp;

// === Monitor videomodes

// EGA 8:5 640x400 (5:4 500x400, 4:3 528x400, 16:9 704x400), vert. 70 Hz, hor. 31.4685 kHz, pixel clock 25.175 MHz
extern const sVideo VideoEGA;

// VGA 4:3 640x480 (16:9 848x480), vert. 60 Hz, hor. 31.4685 kHz, pixel clock 25.175 MHz
extern const sVideo VideoVGA;

// SVGA 4:3 800x600 (16:9 1064x600), vert. 60 Hz, hor. 37.897 kHz, pixel clock 40 MHz
extern const sVideo VideoSVGA;

// XGA 4:3 1024x768 (16:9 1360x768), vert. 60 Hz, hor. 48.36310 kHz, pixel clock 65 MHz
extern const sVideo VideoXGA;

// VESA 4:3 1152x864, vert. 60 Hz, hor. 53.697 kHz, pixel clock 81.62 MHz
extern const sVideo VideoVESA;

// HD 4:3 1280x960, vert. 53 Hz, hor. 51.858 kHz, pixel clock 102.1 MHz
extern const sVideo VideoHD;

// required configuration to initialize VGA output
typedef struct {
	u16	width;			// width in pixels
	u16	height;			// height in lines
	u16	wfull;			// width of full screen, corresponding to 'hfull' time (0=use 'width' parameter)
	const sVideo*	video;		// used video timings
	u32	freq;			// required minimal system frequency in kHz (real frequency can be higher)
	u32	fmax;			// maximal system frequency in kHz (limit resolution if needed)
	u8	mode[LAYERS_MAX];	// modes of overlapped layers 0..3 LAYERMODE_* (LAYERMODE_BASE = layer is off)
					//  - mode of layer 0 is ignored (always use LAYERMODE_BASE)
					//  - all overlapped layers must use same layer program
	bool	dbly;			// double in Y direction
	bool	lockfreq;		// lock required frequency, do not change it
} sVgaCfg;

// videomode table - used to setup video driver
typedef struct {
	// screen resolution
	u16	width;		// width in pixels
	u16	height; 	// height in lines
	u16	wfull;		// width of full screen (corresponding to 'hfull' time)
	u16	wmax;		// maximal width (corresponding to 'hmax' time)

	// setup PLL system clock
	u32	freq;		// system clock frequency in kHz
	u32	vco;		// VCO frequency in kHz
	u16	fbdiv;		// fbdiv PLL divider
	u8	pd1;		// postdiv1
	u8	pd2;		// postdiv2

	// setup PIO state machine	
	u16	div;		// divide base state machine clock
	u16	cpp;		// state machine clocks per pixel
	u8	prog;		// layer program LAYERPROG_*
	u8	mode[LAYERS_MAX]; // mode of layer 0..3 LAYERMODE_* (LAYERMODE_BASE = layer is off or base layer)

	// horizontal timings
	u16	htot;		// total state machine clocks per line
	u16	hfront;		// H front porch in state machine clocks (min. 2)
	u16	hsync;		// H sync pulse in state machine clocks (min. 4)
	u16	hback;		// H back porch in state machine clocks (min. 13)
	float	hfreq;		// horizontal frequency in [Hz]

	// vertical timings
	u16	vtot;		// total scanlines (both sub-frames)
	u16	vmax;		// maximal height
	float	vfreq;		// vertical frequency in [Hz]

	// subframe 1
	u16	vsync1;		// V sync (half-)pulses on subframe 1
	u16	vpost1;		// V sync post (half-)pulses on subframe 1
	u16	vback1;		// V back porch (after VSYNC, before image) on subframe 1
	u16	vact1;		// active visible scanlines, subframe 1
	u16	vfront1;	// V front porch (after image, before VSYNC) on subframe 1
	u16	vpre1;		// V sync pre (half-)pulses on subframe 1
	u16	vfirst1;	// first active scanline, subframe 1

	// subframe 2 (ignored if not interlaced)
	u16	vsync2;		// V sync half-pulses on subframe 2
	u16	vpost2;		// V sync post half-pulses on subframe 2
	u16	vback2;		// V back porch (after VSYNC, before image) on subframe 2
	u16	vact2;		// active visible scanlines, subframe 2
	u16	vfront2;	// V front porch (after image, before VSYNC) on subframe 2
	u16	vpre2;		// V sync pre half-pulses on subframe 2
	u16	vfirst2;	// first active scanline, subframe 2

	// name
	const char* name;	// video timing name (VIDEO_NAME_LEN characters + terminating 0)

	// flags
	bool	lockfreq;	// lock current frequency, do not change it
	bool	dbly;		// double scanlines
	bool	inter;		// interlaced (use sub-frames)
	bool	psync;		// positive synchronization
	bool	odd;		// first sub-frame is odd lines 1, 3, 5,... (PAL)
} sVmode;

// output device
enum {
	DEV_PAL = 0,	// PAL TV
	DEV_NTSC,	// NTSC TV
	DEV_VGA,	// VGA monitor

	DEV_MAX
};

// preset videomode resolution
enum {
	RES_ZX = 0,	// 256x192
	RES_CGA,	// 320x200
	RES_QVGA,	// 320x240
	RES_EGA,	// 512x400
	RES_VGA,	// 640x480
	RES_SVGA,	// 800x600 (not for TV device)
	RES_XGA,	// 1024x768 (not for TV device)
	RES_HD,		// 1280x960 (not for TV device)

	RES_MAX
};

// graphics formats
enum {
	FORM_8BIT = 0,	// 8-bit pixel graphics (up to EGA resolution)
	FORM_4BIT,	// 4-bit pixel graphics (up to SVGA graphics)
	FORM_MONO,	// 1-bit pixel graphics
	FORM_TILE8,	// 8x8 tiles
	FORM_TILE12,	// 12x12 tiles
	FORM_TILE16,	// 16x16 tiles
	FORM_TILE24,	// 24x24 tiles
	FORM_TILE32,	// 32x32 tiles
	FORM_TILE48,	// 48x48 tiles
	FORM_TILE64,	// 64x64 tiles
	FORM_MTEXT8,	// mono text with font 8x8
	FORM_MTEXT16,	// mono text with font 8x16
	FORM_TEXT8,	// attribute text with font 8x8
	FORM_TEXT16,	// attribute text with font 8x16
	FORM_RLE,	// images with RLE compression (on overlapped layer 1)

	FORM_MAX
};

extern sVmode Vmode;	// videomode setup
extern sVgaCfg Cfg;	// required configuration
extern sCanvas Canvas; // canvas of draw box

// 16-color palette translation table
extern u16 Pal16Trans[256];

// initialize default VGA configuration
void VgaCfgDef(sVgaCfg* cfg);

// debug print videomode setup
void VgaPrintCfg(const sVmode* vmode);

// calculate videomode setup
//   cfg ... required configuration
//   vmode ... destination videomode setup for driver
void VgaCfg(const sVgaCfg* cfg, sVmode* vmode);

// initialize videomode
//  dev ... device DEV_*
//  res ... resolution RES_*
//  form ... format FORM_*
//  buf ... pointer to frame buffer (must be aligned to 4-bites, use ALIGNED attribute)
//  buf2 ...pointer to additional buffer:
//	FORM_TILE*: pointer to column of tiles 32x32 in 8-bit graphics
//	FORM_TEXT: pointer to font 8x16 or 8x8 (size 4 KB or 2 KB, ALIGNED attribute, should be in RAM)
//		- copy font to 4KB or 2 KB RAM buffer with ALIGNED attribute
//		- text uses color attributes PC_*
//	FORM_RLE: pointer to image rows (ALIGNED attribute, should be in RAM)
// JMH
const sVmode* Video(u8 dev, u8 res, u8 form, u8* buf, const void* buf2 = NULL);

#endif // _VGA_VMODE_H
