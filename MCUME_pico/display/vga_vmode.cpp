// ****************************************************************************
//
//                                 VGA videomodes
//
// file derived from the PicoVGA project
// https://github.com/Panda381/PicoVGA
// by Miroslav Nemecek
//
// ****************************************************************************

#include "include.h"

sVmode Vmode;	// videomode setup
sVgaCfg Cfg;	// required configuration

/*
http://martin.hinner.info/vga/pal.html

VGA system (525 lines total):
time 0:
- line 1, 2: (2) vertical sync
- line 3..35: (33) dark
- line 36..515: (480) image lines 0..479
- line 516..525: (10) dark
*/



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

	// frame
	.vsync=2,		// V sync (half-)pulses
	.vpost=0,		// V sync post half-pulses
	.vback=35,		// V back porch (after VSYNC, before image)
	.vact=400,		// active visible scanlines
	.vfront=12,	// V front porch (after image, before VSYNC)
	.vpre=0,		// V sync pre half-pulses

	// flags
	.psync=False,		// positive synchronization
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

	// frame
	.vsync=2,		// V sync (half-)pulses
	.vpost=0,		// V sync post half-pulses
	.vback=33,		// V back porch (after VSYNC, before image)
	.vact=480,		// active visible scanlines
	.vfront=10,		// V front porch (after image, before VSYNC)
	.vpre=0,		// V sync pre half-pulses

	// flags
	.psync=False,		// positive synchronization
};


// timings
const sVideo* VideoResTab[DEV_MAX*RES_MAX] =
{
	// DEV_VGA
	&VideoEGA,	// RES_ZX = 0,	// 256x192
	&VideoVGA,	// RES_CGA,	// 320x200
	&VideoVGA,	// RES_QVGA,	// 320x240
	&VideoEGA,	// RES_EGA,	// 528x400
	&VideoVGA,	// RES_VGA,	// 640x480
};

// required resolution width x height
const u16 VideoResReq[RES_MAX*2] =
{
	256, 192,	// RES_ZX = 0,	// 256x192
	320, 200,	// RES_CGA,	// 320x200
	320, 240,	// RES_QVGA,	// 320x240
	512, 400,	// RES_EGA,	// 512x400
	640, 480, 	// RES_VGA,	// 640x480
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
	uint freq = clock_get_hz(clk_sys)/1000;
	cfg->freq = freq;       // required minimal system frequency in kHz (real frequency can be higher)
	cfg->fmax = 270000;		// maximal system frequency in kHz (limit resolution if needed)
	cfg->dbly = False;		// double in Y direction
	cfg->lockfreq = False;		// lock required frequency, do not change it
}



// calculate videomode setup
//   cfg ... required configuration
//   vmode ... destination videomode setup for driver
void VgaCfg(const sVgaCfg* cfg, sVmode* vmode)
{
	// prepare minimal and maximal clocks per pixel
	int mincpp = 2;
	int maxcpp = 17;

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
	int cpp = (int)(freq*hfull/1000/wfull + 0.1f);
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

	int vact = h;	// active lines in progress mode

	if (cfg->dbly) h /= 2; // return double lines to single lines
	vmode->height = h;

	// vertical timings
	vmode->vtot = v->vtot; // total scanlines

	vmode->vact = vact; // active scanlines
	int dh = vact - v->vact; // difference
	vmode->vsync = v->vsync; // V sync (half-)pulses
	vmode->vpost = v->vpost; // V sync post (half-)pulses
	vmode->vback = v->vback - dh/2; // V back porch (after VSYNC, before image)
	vmode->vfront = v->vfront - ((dh < 0) ? (dh-1)/2 : (dh+1)/2); // V front porch (after image, before VSYNC)
	vmode->vpre = v->vpre; // V sync pre (half-)pulses

	// frequency
	vmode->hfreq = vmode->freq * 1000.0f / vmode->div / vmode->htot;
	vmode->vfreq = vmode->hfreq / vmode->vtot;

	// flags
	vmode->lockfreq = cfg->lockfreq; // lock current frequency, do not change it
	vmode->dbly = cfg->dbly; // double scanlines
	vmode->psync = v->psync; // positive synchronization

	// first active scanline
	vmode->vfirst = vmode->vsync + vmode->vback + 1;
}



// initialize videomode
//  dev ... device DEV_*
//  res ... resolution RES_*
const sVmode* Video(u8 dev, u8 res)
{
	// prepare timings structure
	if (dev >= DEV_MAX) dev = DEV_VGA;
	if (res >= RES_MAX) res = RES_MAX-1;
	const sVideo* v = VideoResTab[dev*RES_MAX + res];

	// required resolution
	u16 w = VideoResReq[res*2];
	u16 h = VideoResReq[res*2+1];
	if (h > v->vmax) h = v->vmax;

	// setup videomode
	VgaCfgDef(&Cfg); // get default configuration
	Cfg.video = v; // video timings
	Cfg.width = w; // screen width
	Cfg.height = h; // screen height
	Cfg.dbly = h <= v->vmax/2; // double scanlines
	VgaCfg(&Cfg, &Vmode); // calculate videomode setup

	// initialize system clock
	set_sys_clock_pll(Vmode.vco*1000, Vmode.pd1, Vmode.pd2);

	return &Vmode;
}

