// ****************************************************************************
//
//                                 VGA videomodes
//
// file derived from the PicoVGA project
// https://github.com/Panda381/PicoVGA
// by Miroslav Nemecek
//
// ****************************************************************************

#ifndef _VGA_VMODE_H
#define _VGA_VMODE_H

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

	// frame
	u16	vsync;		// V sync (half-)pulses
	u16	vpost;		// V sync post half-pulses
	u16	vback;		// V back porch (after VSYNC, before image)
	u16	vact;		// active visible scanlines
	u16	vfront;	// V front porch (after image, before VSYNC)
	u16	vpre;		// V sync pre half-pulses

	bool	psync;		// positive synchronization
} sVideo;


// === Monitor videomodes

// EGA 8:5 640x400 (5:4 500x400, 4:3 528x400, 16:9 704x400), vert. 70 Hz, hor. 31.4685 kHz, pixel clock 25.175 MHz
extern const sVideo VideoEGA;
// VGA 4:3 640x480 (16:9 848x480), vert. 60 Hz, hor. 31.4685 kHz, pixel clock 25.175 MHz
extern const sVideo VideoVGA;



// required configuration to initialize VGA output
typedef struct {
	u16	width;			// width in pixels
	u16	height;			// height in lines
	u16	wfull;			// width of full screen, corresponding to 'hfull' time (0=use 'width' parameter)
	const sVideo*	video;		// used video timings
	u32	freq;			// required minimal system frequency in kHz (real frequency can be higher)
	u32	fmax;			// maximal system frequency in kHz (limit resolution if needed)
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

	// frame
	u16	vsync;		// V sync (half-)pulses
	u16	vpost;		// V sync post (half-)pulses
	u16	vback;		// V back porch (after VSYNC, before image)
	u16	vact;		// active visible scanlines
	u16	vfront;	// V front porch (after image, before VSYNC)
	u16	vpre;		// V sync pre (half-)pulses
	u16	vfirst;	// first active scanline

	// flags
	bool	lockfreq;	// lock current frequency, do not change it
	bool	dbly;		// double scanlines
	bool	psync;		// positive synchronization
} sVmode;

// output device
enum {
	DEV_VGA=0,	// VGA monitor

	DEV_MAX
};

// preset videomode resolution
enum {
	RES_ZX = 0,	// 256x192
	RES_CGA,	// 320x200
	RES_QVGA,	// 320x240
	RES_EGA,	// 512x400
	RES_VGA,	// 640x480

	RES_MAX
};


extern sVmode Vmode;	// videomode setup
extern sVgaCfg Cfg;	// required configuration

// initialize default VGA configuration
void VgaCfgDef(sVgaCfg* cfg);

// calculate videomode setup
//   cfg ... required configuration
//   vmode ... destination videomode setup for driver
void VgaCfg(const sVgaCfg* cfg, sVmode* vmode);

// initialize videomode
//  dev ... device DEV_*
//  res ... resolution RES_*
const sVmode* Video(u8 dev, u8 res);

#endif // _VGA_VMODE_H
