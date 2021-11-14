
// ****************************************************************************
//
//                                 VGA videomodes
//
// ****************************************************************************

#include "include.h"

sVmode Vmode;	// videomode setup
sVgaCfg Cfg;	// required configuration
sCanvas Canvas; // canvas of draw box

// default 16-color palettes (EGA colors)
// - do not set "const", to stay in faster RAM
u8 DefPal16[16] = {
	CGACOL_0,	//	0	// 0x000000 black
	CGACOL_1,	//	2	// 0x0000AA dark blue
	CGACOL_2,	//	20	// 0x00B600 dark green
	CGACOL_3,	//	22	// 0x00B6AA dark cyan
	CGACOL_4,	//	160	// 0xB60000 dark red
	CGACOL_5,	//	162	// 0xB600AA dark magenta
	CGACOL_6,	//	168	// 0xB64900 brown
	CGACOL_7,	//	182	// 0xB6B6AA light gray

	CGACOL_8,	//	73	// 0x494955 dark gray
	CGACOL_9,	//	75	// 0x4949FF light blue
	CGACOL_10,	//	93	// 0x49FF55 light green
	CGACOL_11,	//	95	// 0x49FFFF light cyan
	CGACOL_12,	//	233	// 0xFF4955 light red
	CGACOL_13,	//	235	// 0xFF49FF light magenta
	CGACOL_14,	//	253	// 0xFFFF55 yellow
	CGACOL_15,	//	255	// 0xFFFFFF white
};

// 16-color palette translation table
u16 Pal16Trans[256];

/*
http://martin.hinner.info/vga/pal.html

VGA system (525 lines total):
time 0:
- line 1, 2: (2) vertical sync
- line 3..35: (33) dark
- line 36..515: (480) image lines 0..479
- line 516..525: (10) dark

PAL system (625 lines total):
time 0:
- line 1, 2: (2) vertical sync + vertical sync
- line 3: (1) vertical sync + half sync
- line 4, 5: (2) half sync + half sync
- line 6..23: (18) dark
- line 24..46: (23) dark image
time 46:
- line 47..286: (240) image lines odd 1, 3, 5 ... 479
- line 287..310: (24) dark image
- line 311..312: (2) half sync + half sync
- line 313: (1) half sync + vertical sync
vsync time 313 (vsync time 312.5):
- line 314..315: (2) vertical sync + vertical sync
- line 316..317: (2) half sync + half sync
- line 318..335: (18) dark
- line 336..358: (23) dark image
time 358 (45.5 from last vsync)
- line 359..598: (240) image lines even 0, 2, ... 478
- line 599..622: (24) dark image
- line 623..625: (3) half sync + half sync
time 625:

NTSC system (525 lines total):
time 0, even field:
- line 1..3: (3) vertical sync + vertical sync (6 serration pulses: 27.3 us low, 4.5 us high)
- line 4..6: (3) half sync + half sync (6 equalizing pulses: 2.3 us low, 29.5 us high)
- line 7..16: (10) dark (blanked video: 4.7 us low, 58.9 us high)
- line 17,18: (2) dark image
time 18:
- line 19..258: (240) image lines even 0, 2, ... 478
- line 259: (1) dark image
- line 260..262: (3) half sync + half sync (7 equalizing pulses)
- line 263: (1) half sync + vertical sync (6 serration pulses)
time 263 (vsync time 262.5):
- line 264,265: (2) vertical sync + vertical sync
- line 266: (1) vertical sync + half sync (5 equalizing pulses)
- line 267..268: (2) half sync + half sync
- line 269..279: (11) dark
- line 280..281: (2) dark image
time 281 (18.5 from last vsync)
- line 282..521: (240) image lines odd 1, 3, 5 ... 479
- line 522: (1) dark image
- line 523..525: (3) half sync + half sync
time 525:

*/

// === TV videomodes

// TV PAL interlaced 5:4 720x576 (4:3 768x576, 16:9 1024x576)
const sVideo VideoPAL = {
	// horizontal (horizontal frequency 15625 Hz, effective sync pulses 16000 Hz)
	.htot=   64.00000f,	// total scanline in [us]
	.hfront=  1.65000f,	// H front porch (after image, before HSYNC) in [us]
	.hsync=   4.70000f,	// H sync pulse in [us]
	.hback=   5.70000f,	// H back porch (after HSYNC, before image) in [us]
	.hfull=  47.36000f,	// H full visible in [us] (formally should be 51.95 us) 

	// vertical (vertical frequency 50 Hz)
	.vtot=625,		// total scanlines (both subframes)
	.vmax=576,		// maximal height

	// subframe 1
	.vsync1=5,		// V sync (half-)pulses on subframe 1
	.vpost1=5,		// V sync post half-pulses on subframe 1
	.vback1=18+23,		// V back porch (after VSYNC, before image) on subframe 1
	.vact1=240,		// active visible scanlines, subframe 1 (formally should be 288, 576 total)
	.vfront1=24,		// V front porch (after image, before VSYNC) on subframe 1
	.vpre1=5,		// V sync pre half-pulses on subframe 1

	// subframe 2 (ignored if not interlaced)
	.vsync2=5,		// V sync half-pulses on subframe 2
	.vpost2=4,		// V sync post half-pulses on subframe 2
	.vback2=18+23,		// V back porch (after VSYNC, before image) on subframe 2
	.vact2=240,		// active visible scanlines, subframe 2 (formally should be 288, 576 total)
	.vfront2=24,		// V front porch (after image, before VSYNC) on subframe 2
	.vpre2=6,		// V sync pre half-pulses on subframe 2

	// name
	.name = "PAL  ",	// video timing name (VIDEO_NAME_LEN characters + terminating 0)

	// flags
	.inter=True,		// interlaced (use subframes)
	.psync=False,		// positive synchronization
	.odd=True,		// first sub-frame is odd lines 1, 3, 5,... (PAL)
};

// TV PAL progressive 5:4 360x288 (4:3 384x288, 16:9 512x288)
const sVideo VideoPALp = {
	// horizontal (horizontal frequency 15625 Hz)
	.htot=   64.00000f,	// total scanline in [us]
	.hfront=  1.65000f,	// H front porch (after image, before HSYNC) in [us]
	.hsync=   4.70000f,	// H sync pulse in [us]
	.hback=   5.70000f,	// H back porch (after HSYNC, before image) in [us]
	.hfull=  47.36000f,	// H full visible in [us] (formally should be 51.95 us) 

	// vertical (vertical frequency 50 Hz)
	.vtot=312,		// total scanlines (both subframes)
	.vmax=288,		// maximal height

	// subframe 1
	.vsync1=2,		// V sync (half-)pulses on subframe 1
	.vpost1=0,		// V sync post half-pulses on subframe 1
	.vback1=18+23+2,	// V back porch (after VSYNC, before image) on subframe 1
	.vact1=240,		// active visible scanlines, subframe 1 (formally should be 288, 576 total)
	.vfront1=24+3,		// V front porch (after image, before VSYNC) on subframe 1
	.vpre1=0,		// V sync pre half-pulses on subframe 1

	// subframe 2 (ignored if not interlaced)
	.vsync2=0,		// V sync half-pulses on subframe 2
	.vpost2=0,		// V sync post half-pulses on subframe 2
	.vback2=0,		// V back porch (after VSYNC, before image) on subframe 2
	.vact2=0,		// active visible scanlines, subframe 2 (formally should be 288, 576 total)
	.vfront2=0,		// V front porch (after image, before VSYNC) on subframe 2
	.vpre2=0,		// V sync pre half-pulses on subframe 2

	// name
	.name = "PALp ",	// video timing name (VIDEO_NAME_LEN characters + terminating 0)

	// flags
	.inter=False,		// interlaced (use subframes)
	.psync=False,		// positive synchronization
	.odd=True,		// first sub-frame is odd lines 1, 3, 5,... (PAL)
};

// TV NTSC interlaced 4:3 640x480 (5:4 600x480, 16:9 848x480)
//   serration pulses (half vsync): 27.3 us low, 4.5 us high
//   equalizing pulses (half hsync): 2.3 us low, 29.5 us high
//   blanked video (hsync pulses): 4.7 us low, 58.9 us high
const sVideo VideoNTSC = {
	// horizontal (horizontal frequency 15734 Hz, effective sync pulses 16274 Hz)
	.htot=   63.55582f,	// total scanline in [us]
	.hfront=  1.50000f,	// H front porch (after image, before HSYNC) in [us]
	.hsync=   4.70000f,	// H sync pulse in [us]
	.hback=   4.50000f,	// H back porch (after HSYNC, before image) in [us]
	.hfull=  47.03130f,	// H full visible in [us]

	// vertical
	.vtot=525,		// total scanlines (both subframes)
	.vmax=480,		// maximal height

	// subframe 1
	.vsync1=6,		// V sync (half-)pulses on subframe 1
	.vpost1=6,		// V sync post half-pulses on subframe 1
	.vback1=10+2,		// V back porch (after VSYNC, before image) on subframe 1
	.vact1=240,		// active visible scanlines, subframe 1
	.vfront1=1,		// V front porch (after image, before VSYNC) on subframe 1
	.vpre1=7,		// V sync pre half-pulses on subframe 1

	// subframe 2 (ignored if not interlaced)
	.vsync2=6,		// V sync half-pulses on subframe 2
	.vpost2=5,		// V sync post half-pulses on subframe 2
	.vback2=11+2,		// V back porch (after VSYNC, before image) on subframe 2
	.vact2=240,		// active visible scanlines, subframe 2
	.vfront2=1,		// V front porch (after image, before VSYNC) on subframe 2
	.vpre2=6,		// V sync pre half-pulses on subframe 2

	// name
	.name = "NTSC ",	// video timing name (VIDEO_NAME_LEN characters + terminating 0)

	// flags
	.inter=True,		// interlaced (use subframes)
	.psync=False,		// positive synchronization
	.odd=False,		// first sub-frame is odd lines 1, 3, 5,... (PAL)
};

// TV NTSC progressive 4:3 320x240 (5:4 300x240, 16:9 424x240)
const sVideo VideoNTSCp = {
	// horizontal (horizontal frequency 15734 Hz)
	.htot=   63.55582f,	// total scanline in [us]
	.hfront=  1.50000f,	// H front porch (after image, before HSYNC) in [us]
	.hsync=   4.70000f,	// H sync pulse in [us]
	.hback=   4.50000f,	// H back porch (after HSYNC, before image) in [us]
	.hfull=  47.03130f,	// H full visible in [us]

	// vertical
	.vtot=262,		// total scanlines (both subframes)
	.vmax=240,		// maximal height

	// subframe 1
	.vsync1=3,		// V sync (half-)pulses on subframe 1
	.vpost1=0,		// V sync post half-pulses on subframe 1
	.vback1=10+2+3,		// V back porch (after VSYNC, before image) on subframe 1
	.vact1=240,		// active visible scanlines, subframe 1
	.vfront1=1+3,		// V front porch (after image, before VSYNC) on subframe 1
	.vpre1=0,		// V sync pre half-pulses on subframe 1

	// subframe 2 (ignored if not interlaced)
	.vsync2=0,		// V sync half-pulses on subframe 2
	.vpost2=0,		// V sync post half-pulses on subframe 2
	.vback2=0,		// V back porch (after VSYNC, before image) on subframe 2
	.vact2=0,		// active visible scanlines, subframe 2
	.vfront2=0,		// V front porch (after image, before VSYNC) on subframe 2
	.vpre2=6,		// V sync pre half-pulses on subframe 2

	// name
	.name = "NTSCp",	// video timing name (VIDEO_NAME_LEN characters + terminating 0)

	// flags
	.inter=False,		// interlaced (use subframes)
	.psync=False,		// positive synchronization
	.odd=False,		// first sub-frame is odd lines 1, 3, 5,... (PAL)
};

// === Monitor videomodes

// EGA 8:5 640x400 (5:4 500x400, 4:3 528x400, 16:9 704x400), vert. 70 Hz, hor. 31.4685 kHz, pixel clock 25.175 MHz
const sVideo VideoEGA = {
	// horizontal
	.htot=   31.77781f,	// total scanline in [us]
	.hfront=  0.63556f,	// H front porch (after image, before HSYNC) in [us]
	.hsync=   3.81334f,	// H sync pulse in [us]
	.hback=   1.90667f,	// H back porch (after HSYNC, before image) in [us]
	.hfull=  25.42224f,	// H full visible in [us]

	// vertical
	.vtot=449,		// total scanlines (both subframes)
	.vmax=400,		// maximal height

	// subframe 1
	.vsync1=2,		// V sync (half-)pulses on subframe 1
	.vpost1=0,		// V sync post half-pulses on subframe 1
	.vback1=35,		// V back porch (after VSYNC, before image) on subframe 1
	.vact1=400,		// active visible scanlines, subframe 1
	.vfront1=12,		// V front porch (after image, before VSYNC) on subframe 1
	.vpre1=0,		// V sync pre half-pulses on subframe 1

	// subframe 2 (ignored if not interlaced)
	.vsync2=0,		// V sync half-pulses on subframe 2
	.vpost2=0,		// V sync post half-pulses on subframe 2
	.vback2=0,		// V back porch (after VSYNC, before image) on subframe 2
	.vact2=0,		// active visible scanlines, subframe 2
	.vfront2=0,		// V front porch (after image, before VSYNC) on subframe 2
	.vpre2=0,		// V sync pre half-pulses on subframe 2

	// name
	.name = "EGA  ",	// video timing name (VIDEO_NAME_LEN characters + terminating 0)

	// flags
	.inter=False,		// interlaced (use subframes)
	.psync=False,		// positive synchronization
	.odd=False,		// first sub-frame is odd lines 1, 3, 5,... (PAL)
};

// VGA 4:3 640x480 (16:9 848x480), vert. 60 Hz, hor. 31.4685 kHz, pixel clock 25.175 MHz
const sVideo VideoVGA = {
	// horizontal
	.htot=   31.77781f,	// total scanline in [us] (800 pixels)
	.hfront=  0.63556f,	// H front porch (after image, before HSYNC) in [us] (16 pixels)
	.hsync=   3.81334f,	// H sync pulse in [us] (96 pixels)
	.hback=   1.90667f,	// H back porch (after HSYNC, before image) in [us] (48 pixels)
	.hfull=  25.42224f,	// H full visible in [us] (640 pixels)

	// vertical
	.vtot=525,		// total scanlines (both subframes)
	.vmax=480,		// maximal height

	// subframe 1
	.vsync1=2,		// V sync (half-)pulses on subframe 1
	.vpost1=0,		// V sync post half-pulses on subframe 1
	.vback1=33,		// V back porch (after VSYNC, before image) on subframe 1
	.vact1=480,		// active visible scanlines, subframe 1
	.vfront1=10,		// V front porch (after image, before VSYNC) on subframe 1
	.vpre1=0,		// V sync pre half-pulses on subframe 1

	// subframe 2 (ignored if not interlaced)
	.vsync2=0,		// V sync half-pulses on subframe 2
	.vpost2=0,		// V sync post half-pulses on subframe 2
	.vback2=0,		// V back porch (after VSYNC, before image) on subframe 2
	.vact2=0,		// active visible scanlines, subframe 2
	.vfront2=0,		// V front porch (after image, before VSYNC) on subframe 2
	.vpre2=0,		// V sync pre half-pulses on subframe 2

	// name
	.name = "VGA  ",	// video timing name (VIDEO_NAME_LEN characters + terminating 0)

	// flags
	.inter=False,		// interlaced (use subframes)
	.psync=False,		// positive synchronization
	.odd=False,		// first sub-frame is odd lines 1, 3, 5,... (PAL)
};

// SVGA 4:3 800x600 (16:9 1064x600), vert. 60 Hz, hor. 37.897 kHz, pixel clock 40 MHz
const sVideo VideoSVGA = {
	// horizontal
	.htot=   26.40000f,	// total scanline in [us] (1056 pixels)
	.hfront=  1.00000f,	// H front porch (after image, before HSYNC) in [us] (40 pixels)
	.hsync=   3.20000f,	// H sync pulse in [us] (128 pixels)
	.hback=   2.20000f,	// H back porch (after HSYNC, before image) in [us] (88 pixels)
	.hfull=  20.00000f,	// H full visible in [us] (800 pixels)

	// vertical
	.vtot=628,		// total scanlines (both subframes)
	.vmax=600,		// maximal height

	// subframe 1
	.vsync1=4,		// V sync (half-)pulses on subframe 1
	.vpost1=0,		// V sync post half-pulses on subframe 1
	.vback1=23,		// V back porch (after VSYNC, before image) on subframe 1
	.vact1=600,		// active visible scanlines, subframe 1
	.vfront1=1,		// V front porch (after image, before VSYNC) on subframe 1
	.vpre1=0,		// V sync pre half-pulses on subframe 1

	// subframe 2 (ignored if not interlaced)
	.vsync2=0,		// V sync half-pulses on subframe 2
	.vpost2=0,		// V sync post half-pulses on subframe 2
	.vback2=0,		// V back porch (after VSYNC, before image) on subframe 2
	.vact2=0,		// active visible scanlines, subframe 2
	.vfront2=0,		// V front porch (after image, before VSYNC) on subframe 2
	.vpre2=0,		// V sync pre half-pulses on subframe 2

	// name
	.name = "SVGA ",	// video timing name (VIDEO_NAME_LEN characters + terminating 0)

	// flags
	.inter=False,		// interlaced (use subframes)
	.psync=True,		// positive synchronization
	.odd=False,		// first sub-frame is odd lines 1, 3, 5,... (PAL)
};

// XGA 4:3 1024x768 (16:9 1360x768), vert. 60 Hz, hor. 48.36310 kHz, pixel clock 65 MHz
const sVideo VideoXGA = {
	// horizontal
	.htot=   20.67692f,	// total scanline in [us] (1344 pixels)
	.hfront=  0.36923f,	// H front porch (after image, before HSYNC) in [us] (24 pixels)
	.hsync=   2.09231f,	// H sync pulse in [us] (136 pixels)
	.hback=   2.46154f,	// H back porch (after HSYNC, before image) in [us] (160 pixels)
	.hfull=  15.75385f,	// H full visible in [us] (1024 pixels)

	// vertical
	.vtot=806,		// total scanlines (both subframes)
	.vmax=768,		// maximal height

	// subframe 1
	.vsync1=6,		// V sync (half-)pulses on subframe 1
	.vpost1=0,		// V sync post half-pulses on subframe 1
	.vback1=29,		// V back porch (after VSYNC, before image) on subframe 1
	.vact1=768,		// active visible scanlines, subframe 1
	.vfront1=3,		// V front porch (after image, before VSYNC) on subframe 1
	.vpre1=0,		// V sync pre half-pulses on subframe 1

	// subframe 2 (ignored if not interlaced)
	.vsync2=0,		// V sync half-pulses on subframe 2
	.vpost2=0,		// V sync post half-pulses on subframe 2
	.vback2=0,		// V back porch (after VSYNC, before image) on subframe 2
	.vact2=0,		// active visible scanlines, subframe 2
	.vfront2=0,		// V front porch (after image, before VSYNC) on subframe 2
	.vpre2=0,		// V sync pre half-pulses on subframe 2

	// name
	.name = "XGA  ",	// video timing name (VIDEO_NAME_LEN characters + terminating 0)

	// flags
	.inter=False,		// interlaced (use subframes)
	.psync=False,		// positive synchronization
	.odd=False,		// first sub-frame is odd lines 1, 3, 5,... (PAL)
};

// VESA 4:3 1152x864, vert. 60 Hz, hor. 53.697 kHz, pixel clock 81.62 MHz
const sVideo VideoVESA = {
	// horizontal
	.htot=   18.62289f,	// total scanline in [us] (1520 pixels)
	.hfront=  0.78412f,	// H front porch (after image, before HSYNC) in [us] (64 pixels)
	.hsync=   1.47023f,	// H sync pulse in [us] (120 pixels)
	.hback=   2.25435f,	// H back porch (after HSYNC, before image) in [us] (184 pixels)
	.hfull=  14.11419f,	// H full visible in [us] (1152 pixels)

	// vertical
	.vtot=895,		// total scanlines (both subframes)
	.vmax=864,		// maximal height

	// subframe 1
	.vsync1=3,		// V sync (half-)pulses on subframe 1
	.vpost1=0,		// V sync post half-pulses on subframe 1
	.vback1=27,		// V back porch (after VSYNC, before image) on subframe 1
	.vact1=864,		// active visible scanlines, subframe 1
	.vfront1=1,		// V front porch (after image, before VSYNC) on subframe 1
	.vpre1=0,		// V sync pre half-pulses on subframe 1

	// subframe 2 (ignored if not interlaced)
	.vsync2=0,		// V sync half-pulses on subframe 2
	.vpost2=0,		// V sync post half-pulses on subframe 2
	.vback2=0,		// V back porch (after VSYNC, before image) on subframe 2
	.vact2=0,		// active visible scanlines, subframe 2
	.vfront2=0,		// V front porch (after image, before VSYNC) on subframe 2
	.vpre2=0,		// V sync pre half-pulses on subframe 2

	// name
	.name = "VESA ",	// video timing name (VIDEO_NAME_LEN characters + terminating 0)

	// flags
	.inter=False,		// interlaced (use subframes)
	.psync=True,		// positive synchronization
	.odd=False,		// first sub-frame is odd lines 1, 3, 5,... (PAL)
};

// HD 4:3 1280x960, vert. 53 Hz, hor. 51.858 kHz, pixel clock 102.1 MHz
#define HD_SLOW 1.15f
const sVideo VideoHD = {
	// horizontal
	.htot=   16.76787f*HD_SLOW,	// total scanline in [us] (1712 pixels)
	.hfront=  0.78355f*HD_SLOW,	// H front porch (after image, before HSYNC) in [us] (80 pixels)
	.hsync=   1.33203f*HD_SLOW,	// H sync pulse in [us] (136 pixels)
	.hback=   2.11557f*HD_SLOW,	// H back porch (after HSYNC, before image) in [us] (216 pixels)
	.hfull=  12.53673f*HD_SLOW,	// H full visible in [us] (1280 pixels)

	// vertical
	.vtot=994-10,		// total scanlines (both subframes)
	.vmax=960,		// maximal height

	// subframe 1
	.vsync1=3,		// V sync (half-)pulses on subframe 1
	.vpost1=0,		// V sync post half-pulses on subframe 1
	.vback1=30-10,		// V back porch (after VSYNC, before image) on subframe 1
	.vact1=960,		// active visible scanlines, subframe 1
	.vfront1=1,		// V front porch (after image, before VSYNC) on subframe 1
	.vpre1=0,		// V sync pre half-pulses on subframe 1

	// subframe 2 (ignored if not interlaced)
	.vsync2=0,		// V sync half-pulses on subframe 2
	.vpost2=0,		// V sync post half-pulses on subframe 2
	.vback2=0,		// V back porch (after VSYNC, before image) on subframe 2
	.vact2=0,		// active visible scanlines, subframe 2
	.vfront2=0,		// V front porch (after image, before VSYNC) on subframe 2
	.vpre2=0,		// V sync pre half-pulses on subframe 2

	// name
	.name = "HD   ",	// video timing name (VIDEO_NAME_LEN characters + terminating 0)

	// flags
	.inter=False,		// interlaced (use subframes)
	.psync=False,		// positive synchronization
	.odd=False,		// first sub-frame is odd lines 1, 3, 5,... (PAL)
};


// Search PLL setup
//  reqkhz ... required output frequency in kHz
//  input ... PLL input frequency in kHz (default 12000, or use clock_get_hz(clk_ref)/1000)
//  vcomin ... minimal VCO frequency in kHz (default 400000)
//  vcomax ... maximal VCO frequency in kHz (default 1600000)
//  lowvco ... prefer low VCO (lower power but more jiter)
// outputs:
//  outkhz ... output achieved frequency in kHz (0=not found)
//  outvco ... output VCO frequency in kHz
//  outfbdiv ... output fbdiv (16..320)
//  outpd1 ... output postdiv1 (1..7)
//  outpd2 ... output postdiv2 (1..7)
// Returns true if precise frequency has been found, or near frequency used otherwise.
bool vcocalc(u32 reqkhz, u32 input, u32 vcomin, u32 vcomax, bool lowvco,
		u32* outkhz, u32* outvco, u16* outfbdiv, u8* outpd1, u8* outpd2)
{
	u32 khz, vco, margin;
	u16 fbdiv;
	u8 pd1, pd2;
	u32 margin_best = 100000;
	*outkhz = 0;

	// fbdiv loop
	fbdiv = lowvco ? 16 : 320;
	for (;;)
	{
		// get current vco
		vco = fbdiv * input;

		// check vco range
		if ((vco >= vcomin) && (vco <= vcomax))
		{
			// pd1 loop
			for (pd1 = 7; pd1 >= 1; pd1--)
			{
				// pd2 loop
				for (pd2 = pd1; pd2 >= 1; pd2--)
				{
					// current output frequency
					khz = vco / (pd1 * pd2);

					// check best frequency
					margin = abs((int)(khz - reqkhz));
					if (margin < margin_best)
					{
						margin_best = margin;
						*outkhz = khz;
						*outvco = vco;
						*outfbdiv = fbdiv;
						*outpd1 = pd1;
						*outpd2 = pd2;
					}
				}
			}
		}

		// shift fbdiv
		if (lowvco)
		{
			fbdiv++;
			if (fbdiv > 320) break;
		}
		else
		{
			fbdiv--;
			if (fbdiv < 16) break;
		}
	}

	// check precise frequency
	return (*outkhz == reqkhz) && (*outvco == *outkhz * *outpd1 * *outpd2);
}

// find sysclock setup (use set_sys_clock_pll to set sysclock)
//  reqkhz ... required frequency in kHz
// outputs:
//  outkhz ... output achieved frequency in kHz (0=not found)
//  outvco ... output VCO frequency in kHz
//  outfbdiv ... output fbdiv (16..320)
//  outpd1 ... output postdiv1 (1..7)
//  outpd2 ... output postdiv2 (1..7)
// Returns true if precise frequency has been found, or near frequency used otherwise.
bool FindSysClock(u32 reqkhz, u32* outkhz, u32* outvco, u16* outfbdiv, u8* outpd1, u8* outpd2)
{
	// get reference frequency in kHz (should be 12 MHz)
	u32 input = clock_get_hz(clk_ref)/1000;

	// find PLL setup
	return vcocalc(reqkhz, input, 400000, 1600000, false,  outkhz, outvco, outfbdiv, outpd1, outpd2);
}


// initialize default VGA configuration
void VgaCfgDef(sVgaCfg* cfg)
{
	cfg->width = 640;		// width in pixels
	cfg->height = 480;		// height in lines
	cfg->wfull = 0;			// width of full screen, corresponding to 'hfull' time (0=use 'width' parameter)
	cfg->video = &VideoVGA;		// used video timings
	cfg->freq = 250000; //120000;		// required minimal system frequency in kHz (real frequency can be higher)
	cfg->fmax = 270000;		// maximal system frequency in kHz (limit resolution if needed)
	cfg->mode[0] = LAYERMODE_BASE;	// modes of overlapped layers 0..3 LAYERMODE_* (LAYERMODE_BASE = layer is off)
	cfg->mode[1] = LAYERMODE_BASE;	// - mode of layer 0 is ignored (always use LAYERMODE_BASE)
	cfg->mode[2] = LAYERMODE_BASE;	// - all overlapped layers must use same layer program
	cfg->mode[3] = LAYERMODE_BASE;
	cfg->dbly = False;		// double in Y direction
	cfg->lockfreq = False;		// lock required frequency, do not change it
}

// debug print videomode setup
void VgaPrintCfg(const sVmode* vmode)
{
	printf("width=%u height=%u wfull=%u wmax=%u\n", vmode->width, vmode->height, vmode->wfull, vmode->wmax);
	printf("freq=%u vco=%u fbdiv=%u pd1=%u pd2=%u\n", vmode->freq, vmode->vco, vmode->fbdiv, vmode->pd1, vmode->pd2);
	printf("div=%u cpp=%u prog=%u mode=%u %u %u %u\n", vmode->div, vmode->cpp, vmode->prog, vmode->mode[0], vmode->mode[1], vmode->mode[2], vmode->mode[3]);
	printf("htot=%u hfront=%u hsync=%u hback=%u\n", vmode->htot, vmode->hfront, vmode->hsync, vmode->hback);
	printf("vtot=%u vmax=%u\n", vmode->vtot, vmode->vmax);
	printf("vsync1=%u vpost1=%u vback1=%u vact1=%u vfront1=%u vpre1=%u vfirst1=%u\n", vmode->vsync1, vmode->vpost1,
		vmode->vback1, vmode->vact1, vmode->vfront1, vmode->vpre1, vmode->vfirst1);
	printf("vsync2=%u vpost2=%u vback2=%u vact2=%u vfront2=%u vpre2=%u vfirst2=%u\n", vmode->vsync2, vmode->vpost2,
		vmode->vback2, vmode->vact2, vmode->vfront2, vmode->vpre2, vmode->vfirst2);
	printf("lockfreq=%u dbly=%u inter=%u psync=%u odd=%u\n", vmode->lockfreq, vmode->dbly, vmode->inter, vmode->psync, vmode->odd);
}

// calculate videomode setup
//   cfg ... required configuration
//   vmode ... destination videomode setup for driver
void VgaCfg(const sVgaCfg* cfg, sVmode* vmode)
{
	int i;

	// prepare layer program, copy layer modes
	u8 prog = LAYERMODE_BASE;
	vmode->mode[0] = prog;
	for (i = 1; i < LAYERS; i++)
	{
		if (cfg->mode[i] != LAYERMODE_BASE) prog = LayerMode[cfg->mode[i]].prog;
		vmode->mode[i] = cfg->mode[i];
	}
	vmode->prog = prog;

	// prepare minimal and maximal clocks per pixel
	int mincpp = LayerMode[LAYERMODE_BASE].mincpp;
	int maxcpp = LayerMode[LAYERMODE_BASE].maxcpp;
	int cpp;
	for (i = 1; i < LAYERS; i++)
	{
		cpp = LayerMode[cfg->mode[i]].mincpp;
		if (cpp > mincpp) mincpp = cpp;
		cpp = LayerMode[cfg->mode[i]].maxcpp;
		if (cpp < maxcpp) maxcpp = cpp;
	}

	// prepare full width
	int w = cfg->width; // required width
	int wfull = cfg->wfull;	// full width
	if (wfull == 0) wfull = w; // use required width as 100% width

	// prepare maximal active time and maximal pixels
	const sVideo* v = cfg->video;
	float hmax = v->htot - v->hfront - v->hsync - v->hback;
	float hfull = v->hfull;
	int wmax = (int)(wfull*hmax/hfull + 0.001f);

	// calculate cpp from required frequency (rounded down), limit minimal cpp
	u32 freq = cfg->freq;
	cpp = (int)(freq*hfull/1000/wfull + 0.1f);
	if (cpp < mincpp) cpp = mincpp;

	// recalculate frequency if not locked
	if (!cfg->lockfreq)
	{
		int freq2 = (int)(cpp*wfull*1000/hfull + 0.5f) + 200;
		if (freq2 < freq)
		{
			cpp++;
			freq2 = (int)(cpp*wfull*1000/hfull + 0.5f) + 200;
		}
		if (freq2 >= freq) freq = freq2;
		if (freq > cfg->fmax) freq = cfg->fmax;
	}

	// find sysclock setup (use set_sys_clock_pll to set sysclock)
	u32 vco;
	u16 fbdiv;
	u8 pd1, pd2;
	FindSysClock(freq, &freq, &vco, &fbdiv, &pd1, &pd2);

	vmode->freq = freq;
	vmode->vco = vco;
	vmode->fbdiv = fbdiv;
	vmode->pd1 = pd1;
	vmode->pd2 = pd2;

	// calculate divisor
	cpp = (int)(freq*hfull/1000/wfull + 0.2f);
	int div = 1;
	while (cpp > maxcpp)
	{
		div++;
		cpp = (int)(freq*hfull/1000/wfull/div + 0.2f);
	}

	vmode->div = div;
	vmode->cpp = cpp;

	// calculate new full resolution and max resolution
	wfull = (int)(freq*hfull/1000/cpp/div + 0.4f);
	wmax = (int)(freq*hmax/1000/cpp/div + 0.4f);

	// limit resolution
	if (w > wmax) w = wmax;
	w = ALIGN4(w);
	vmode->width = w; // active width
	vmode->wfull = wfull; // width of full screen (image should be full visible)
	vmode->wmax = wmax; // maximal width (can be > wfull)

	// horizontal timings
	int hwidth = w*cpp; // active width in state machine clocks
	int htot = (int)(freq*v->htot/1000/div + 0.5f);  // total state machine clocks per line
	int hsync = (int)(freq*v->hsync/1000/div + 0.5f); // H sync pulse in state machine clocks (min. 4)

	if (hsync < 4)
	{
		htot -= 4 - hsync;
		hsync = 4;
	}

	int hfront = (int)(freq*v->hfront/1000/div + 0.5f); // H front porch in state machine clocks (min. 2)
	int hback = (int)(freq*v->hback/1000/div + 0.5f); // H back porch in state machine clocks (min. 13)
	int d = htot - hfront - hsync - hback - hwidth; // difference
	hfront += d/2;
	hback += (d < 0) ? (d-1)/2 : (d+1)/2;

	if (hfront < 4)
	{
		hback -= 4 - hfront;
		hfront = 4;
	}

	if (hback < 13)
	{
		hfront -= 13 - hback;
		hback = 13;

		if (hfront < 2) hfront = 2;
	}

	htot = hfront + hsync + hback + hwidth; // total state machine clocks per line

	// interliced htot must be even (to enable split to half-sync)
	if (v->inter && ((htot & 1) != 0))
	{
		htot--;
		hfront++;
	}

	vmode->htot = (u16)htot; // total state machine clocks per line
	vmode->hfront = (u16)hfront; // H front porch in state machine clocks (min. 2)
	vmode->hsync = (u16)hsync; // H sync pulse in state machine clocks (min. 4)
	vmode->hback = (u16)hback; // H back porch in state machine clocks (min. 13)

	// vertical timings
	int h = cfg->height; // required height
	if (cfg->dbly) h *= 2; // use double lines
	vmode->vmax = v->vmax; // maximal height
	if (h > v->vmax) h = v->vmax; // limit height
	if (cfg->dbly) h &= ~1; // must be even number if double lines

	int vact1 = h;	// active lines in progress mode
	int vact2 = 0;
	if (v->inter) // interlaced
	{
		if (v->odd) // first frame is odd lines
		{
			vact1 = h/2;
			vact2 = (h+1)/2; // if even lines, even frame will have more lines
		}
		else
		{
			vact1 = (h+1)/2; // if even lines, even frame will have more lines
			vact2 = h/2;
		}
	}

	if (cfg->dbly) h /= 2; // return double lines to single lines
	vmode->height = h;

	// vertical timings
	vmode->vtot = v->vtot; // total scanlines

	vmode->vact1 = vact1; // active scanlines of 1st subframe
	int dh = vact1 - v->vact1; // difference
	vmode->vsync1 = v->vsync1; // V sync (half-)pulses on subframe 1
	vmode->vpost1 = v->vpost1; // V sync post (half-)pulses on subframe 1
	vmode->vback1 = v->vback1 - dh/2; // V back porch (after VSYNC, before image) on subframe 1
	vmode->vfront1 = v->vfront1 - ((dh < 0) ? (dh-1)/2 : (dh+1)/2); // V front porch (after image, before VSYNC) on subframe 1
	vmode->vpre1 = v->vpre1; // V sync pre (half-)pulses on subframe 1

	vmode->vact2 = vact2; // active scanlines of 2nd subframe
	dh = vact2 - v->vact2; // difference
	vmode->vsync2 = v->vsync2; // V sync half-pulses on subframe 2
	vmode->vpost2 = v->vpost2; // V sync post half-pulses on subframe 2
	vmode->vback2 = v->vback2 - dh/2; // V back porch (after VSYNC, before image) on subframe 2
	vmode->vfront2 = v->vfront2 - ((dh < 0) ? (dh-1)/2 : (dh+1)/2); // V front porch (after image, before VSYNC) on subframe 2
	vmode->vpre2 = v->vpre2; // V sync pre half-pulses on subframe 2

	// frequency
	vmode->hfreq = vmode->freq * 1000.0f / vmode->div / vmode->htot;
	vmode->vfreq = vmode->hfreq / vmode->vtot;

	// name
	vmode->name = v->name;	// video timing name

	// flags
	vmode->lockfreq = cfg->lockfreq; // lock current frequency, do not change it
	vmode->dbly = cfg->dbly; // double scanlines
	vmode->inter = v->inter; // interlaced (use sub-frames)
	vmode->psync = v->psync; // positive synchronization
	vmode->odd = v->odd; // first sub-frame is odd lines 1, 3, 5,... (PAL)

	// first active scanline
	if (v->inter)
	{
		// interlaced
		vmode->vfirst1 = (vmode->vsync1 + vmode->vpost1)/2 + vmode->vback1 + 1; 
		vmode->vfirst2 = vmode->vfirst1 + vmode->vact1 + vmode->vfront1 + 
			(vmode->vpre1 + vmode->vsync2 + vmode->vpost2)/2 + vmode->vback2;
	}
	else
	{
		// progressive
		vmode->vfirst1 = vmode->vsync1 + vmode->vback1 + 1;
		vmode->vfirst2 = 0;
	}
}

// timings
const sVideo* VideoResTab[DEV_MAX*RES_MAX] =
{
	// DEV_PAL
	&VideoPALp,	// RES_ZX = 0,	// 256x192
	&VideoPALp,	// RES_CGA,	// 320x200
	&VideoPALp,	// RES_QVGA,	// 320x240
	&VideoPAL,	// RES_EGA,	// 528x400
	&VideoPAL,	// RES_VGA,	// 640x480
	&VideoPAL,	// RES_SVGA,	// 800x600 (not for TV device)
	&VideoPAL,	// RES_XGA,	// 1024x768 (not for TV device)
	&VideoPAL,	// RES_HD,	// 1280x960 (not for TV device)

	// DEV_NTSC
	&VideoNTSCp,	// RES_ZX = 0,	// 256x192
	&VideoNTSCp,	// RES_CGA,	// 320x200
	&VideoNTSCp,	// RES_QVGA,	// 320x240
	&VideoNTSC,	// RES_EGA,	// 528x400
	&VideoNTSC,	// RES_VGA,	// 640x480
	&VideoNTSC,	// RES_SVGA,	// 800x600 (not for TV device)
	&VideoNTSC,	// RES_XGA,	// 1024x768 (not for TV device)
	&VideoNTSC,	// RES_HD,	// 1280x960 (not for TV device)

	// DEV_VGA
	&VideoEGA,	// RES_ZX = 0,	// 256x192
	&VideoVGA,	// RES_CGA,	// 320x200
	&VideoVGA,	// RES_QVGA,	// 320x240
	&VideoEGA,	// RES_EGA,	// 528x400
	&VideoVGA,	// RES_VGA,	// 640x480
	&VideoSVGA,	// RES_SVGA,	// 800x600 (not for TV device)
	&VideoXGA,	// RES_XGA,	// 1024x768 (not for TV device)
	&VideoHD,	// RES_HD,	// 1280x960 (not for TV device)
};

// required resolution width x height
const u16 VideoResReq[RES_MAX*2] =
{
	256, 192,	// RES_ZX = 0,	// 256x192
	320, 200,	// RES_CGA,	// 320x200
	320, 240,	// RES_QVGA,	// 320x240
	512, 400,	// RES_EGA,	// 512x400
	640, 480, 	// RES_VGA,	// 640x480
	800, 600, 	// RES_SVGA,	// 800x600 (not for TV device)
	1024, 768,	// RES_XGA,	// 1024x768 (not for TV device)
	1280, 960,	// RES_HD,	// 1280x960 (not for TV device)
};

// initialize videomode
//  dev ... device DEV_*
//  res ... resolution RES_*
//  form ... format FORM_*
//  buf ... pointer to frame buffer (must be aligned to 4-bites, use ALIGNED attribute)
//  buf2 ...pointer to additional buffer:
//	FORM_TILE: pointer to column of tiles 32x32 in 8-bit graphics
//	FORM_TEXT: pointer to font 8x16 or 8x8 (size 4 KB or 2 KB, ALIGNED attribute, should be in RAM)
//		- copy font to 4KB or 2 KB RAM buffer with ALIGNED attribute
//		- text uses color attributes PC_*
//	FORM_RLE: pointer to image rows (ALIGNED attribute, should be in RAM)
// JMH
const sVmode* Video(u8 dev, u8 res, u8 form, u8* buf, const void* buf2 /* = FontBoldB8x16 */)
{
	// stop VGA core
	// JMH
	//multicore_reset_core1();

	// run VGA core
	// JMH
	//multicore_launch_core1(VgaCore);

	// prepare timings structure
	if (dev >= DEV_MAX) dev = DEV_VGA;
	if (res >= RES_MAX) res = RES_MAX-1;
	if (form >= FORM_MAX) form = FORM_MAX-1;
	const sVideo* v = VideoResTab[dev*RES_MAX + res];

	// required resolution
	u16 w = VideoResReq[res*2];
	u16 h = VideoResReq[res*2+1];
	if (h > v->vmax) h = v->vmax;

	if ((form == FORM_TEXT8) || (form == FORM_MTEXT8))
	{
		w = w/8*8;
		h = h/8*8;
	}

	if ((form == FORM_TEXT16) || (form == FORM_MTEXT16))
	{
		w = w/8*8;
		h = h/16*16;
	}

	// setup videomode
	VgaCfgDef(&Cfg); // get default configuration
	Cfg.video = v; // video timings
	Cfg.width = w; // screen width
	Cfg.height = h; // screen height
	if (form == FORM_RLE) Cfg.mode[1] = LAYERMODE_RLE;
	Cfg.dbly = h <= v->vmax/2; // double scanlines
	VgaCfg(&Cfg, &Vmode); // calculate videomode setup

	// initialize base layer 0
	ScreenClear(pScreen);
	sStrip* t = ScreenAddStrip(pScreen, h);
	sSegm* g = ScreenAddSegm(t, w);
	switch (form)
	{
	case FORM_8BIT:	// 8-bit pixel graphics (up to EGA resolution)
		ScreenSegmGraph8(g, buf, w);
		Canvas.img = buf;
		Canvas.w = w;
		Canvas.h = h;
		Canvas.wb = w;
		Canvas.format = CANVAS_8;
		break;

	case FORM_4BIT:	// 4-bit pixel graphics (up to SVGA graphics)
		GenPal16Trans(Pal16Trans, DefPal16); // generate palette translation table
		ScreenSegmGraph4(g, buf, Pal16Trans, w/2);
		Canvas.img = buf;
		Canvas.w = w;
		Canvas.h = h;
		Canvas.wb = w/2;
		Canvas.format = CANVAS_4;
		break;

	case FORM_MONO:	// 1-bit pixel graphics
		ScreenSegmGraph1(g, buf, COL_BLACK, COL_WHITE, w/8);
		Canvas.img = buf;
		Canvas.w = w;
		Canvas.h = h;
		Canvas.wb = w/8;
		Canvas.format = CANVAS_1;
		break;

	case FORM_TILE8: // 8x8 tiles
		ScreenSegmTile(g, buf, buf2, 8, 8, (w+7)/8);
		break;

	case FORM_TILE12: // 12x12 tiles
		ScreenSegmTile(g, buf, buf2, 12, 12, (w+11)/12);
		break;

	case FORM_TILE16: // 16x16 tiles
		ScreenSegmTile(g, buf, buf2, 16, 16, (w+15)/16);
		break;

	case FORM_TILE24: // 24x24 tiles
		ScreenSegmTile(g, buf, buf2, 24, 24, (w+23)/24);
		break;

	case FORM_TILE32: // 32x32 tiles
		ScreenSegmTile(g, buf, buf2, 32, 32, (w+31)/32);
		break;

	case FORM_TILE48: // 48x48 tiles
		ScreenSegmTile(g, buf, buf2, 48, 48, (w+47)/48);
		break;

	case FORM_TILE64: // 64x64 tiles
		ScreenSegmTile(g, buf, buf2, 64, 64, (w+63)/64);
		break;

	case FORM_MTEXT8: // mono text with font 8x8
		ScreenSegmMText(g, buf, buf2, 8, COL_BLACK, COL_WHITE, w/8);
		break;

	case FORM_MTEXT16: // mono text with font 8x16
		ScreenSegmMText(g, buf, buf2, 16, COL_BLACK, COL_WHITE, w/8);
		break;

	case FORM_TEXT8: // attribute text with font 8x8
		ScreenSegmAText(g, buf, buf2, 8, DefPal16, w/8*2);
		break;

	case FORM_TEXT16: // attribute text with font 8x16
		ScreenSegmAText(g, buf, buf2, 16, DefPal16, w/8*2);
		break;

	case FORM_RLE:	// images with RLE compression (on overlapped layer 1)
		ScreenSegmColor(g, 0, 0);
		LayerSetup(1, buf, &Vmode, w, h, 0, buf2);
		LayerOn(1);
		break;
	}

	// initialize system clock
	set_sys_clock_pll(Vmode.vco*1000, Vmode.pd1, Vmode.pd2);



	// initialize videomode
	// JMH
	//VgaInitReq(&Vmode);

	return &Vmode;
}

