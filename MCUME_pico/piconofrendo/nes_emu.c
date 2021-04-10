
#include "emuapi.h"

#include <string.h>
#include <stdint.h>
#include <stdio.h>

//Nes stuff wants to define this as well...
#undef false
#undef true
#undef bool

#include "noftypes.h"


#include "nes.h"
#include "nofrendo.h"
#include "osd.h"
#include "nesinput.h"
#include "event.h"
#include "nofconfig.h"

#define  DEFAULT_WIDTH        256
#define  DEFAULT_HEIGHT       NES_VISIBLE_HEIGHT


char configfilename[]="na";
char romname[64];

/* This is os-specific part of main() */
int osd_main(int argc, char *argv[])
{
   config.filename = configfilename;

   return main_loop(romname, system_autodetect);
}

/* File system interface */
void osd_fullname(char *fullname, const char *shortname)
{
   strncpy(fullname, shortname, 64);
}

/* This gives filenames for storage of saves */
char *osd_newextension(char *string, char *ext)
{
   return string;
}

/* This gives filenames for storage of PCX snapshots */
int osd_makesnapname(char *filename, int len)
{
   return -1;
}

#if HAS_SND
static void (*audio_callback)(void *buffer, int length) = NULL;

void SND_Process( void * stream, int len )
{
  if (audio_callback != NULL) {
 	  audio_callback(stream,len);
  }
}
#endif

void osd_setsound(void (*playfunc)(void *buffer, int length))
{
   //Indicates we should call playfunc() to get more data.
#if HAS_SND
   audio_callback = playfunc;
#endif
}

static void osd_stopsound(void)
{
#if HAS_SND
   audio_callback = NULL;
#endif
}


static int osd_init_sound(void)
{
#if HAS_SND
	audio_callback = NULL;
#endif

	return 0;
}

void osd_getsoundinfo(sndinfo_t *info)
{
   info->sample_rate = 22050;
   info->bps = 16;
}


void osd_getinput(void)
{
	const int ev[16]={
			event_joypad1_select,0,0,event_joypad1_start,event_joypad1_up,event_joypad1_right,event_joypad1_down,event_joypad1_left,
			0,0,0,0,event_soft_reset,event_joypad1_a,event_joypad1_b,event_hard_reset
		};

  	int j=emu_ReadKeys();
  	int hk = emu_ReadI2CKeyboard();  
  	int b=0xffff;

  if ( (j & MASK_KEY_USER2) ) // B
    b &= ~0x4000;
  if ( (j & MASK_KEY_USER3) || (hk == 2) ) // SELECT
    b &= ~0x0001;
  if ( (j & MASK_KEY_USER1) || (hk == 3) ) // START
    b &= ~0x0008;
  if (j & MASK_JOY2_UP)
    b &= ~0x0010;
  if (j & MASK_JOY2_LEFT)
    b &= ~0x0020;
  if (j & MASK_JOY2_DOWN)
    b &= ~0x0040;
  if (j & MASK_JOY2_RIGHT)
    b &= ~0x0080;
  if (j & MASK_JOY2_BTN)  // A
    b &= ~0x2000;


	static int oldb=0xffff;

	int chg=b^oldb;
	int x;
	oldb=b;
	event_t evh;
	for (x=0; x<16; x++) {
		if (chg&1) {
			evh=event_get(ev[x]);
			if (evh) evh((b&1)?INP_STATE_BREAK:INP_STATE_MAKE);
		}
		chg>>=1;
		b>>=1;
	}
}


void osd_getmouse(int *x, int *y, int *button)
{
}

/* this is at the bottom, to eliminate warnings */
void osd_shutdown()
{
}

int osd_init()
{
	return 0;
}


/*
** Video
*/
static int init(int width, int height)
{
	return 0;
}

static void shutdown(void)
{
}

static int set_mode(int width, int height)
{
   return 0;
}

static bitmap_t *myBitmap=NULL;
static void set_palette(rgb_t *pal)
{
  int i;
  for (i = 0; i < PALETTE_SIZE; i++)
  {
   	emu_SetPaletteEntry(pal[i].r, pal[i].g, pal[i].b,i);
  }

}

/* clear all frames to a particular color */
static void clear(uint8 color)
{
}

/* acquire the directbuffer for writing */
static char fb[1]; //dummy
static bitmap_t *lock_write(void)
{
   if (myBitmap == NULL)
   	myBitmap = bmp_createhw((uint8*)fb, DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_WIDTH*2);

   return myBitmap;
}

/* release the resource */
static void free_write(int num_dirties, rect_t *dirty_rects)
{
   //bmp_destroy(&myBitmap);
}


static void custom_blit(bitmap_t *bmp, int num_dirties, rect_t *dirty_rects) {
}

static viddriver_t sdlDriver =
{
   "Simple DirectMedia Layer",         /* name */
   init,          /* init */
   shutdown,      /* shutdown */
   set_mode,      /* set_mode */
   set_palette,   /* set_palette */
   clear,         /* clear */
   lock_write,    /* lock_write */
   free_write,    /* free_write */
   custom_blit,   /* custom_blit */
   false          /* invalidate flag */
};

void osd_getvideoinfo(vidinfo_t *info)
{
   info->default_width = DEFAULT_WIDTH;
   info->default_height = DEFAULT_HEIGHT;
   info->driver = &sdlDriver;
}

void osd_togglefullscreen(int code)
{
}

//Seemingly, this will be called only once. Should call func with a freq of frequency,
int osd_installtimer(int frequency, void *func, int funcsize, void *counter, int countersize)
{
   return 0;
}

char* romdata=NULL;

char *osd_getromdata() {

    return (char*)romdata;
}

void emu_KeyboardOnDown(int keymodifer, int key) {
}

void emu_KeyboardOnUp(int keymodifer, int key) {
}

void nes_Init(void)
{
#if HAS_SND
  emu_sndInit();
#endif
  nofrendo_main(0, NULL); 
}

void nes_Step(void)
{
	nes_step(emu_FrameSkip());	
	//delay(20);
	emu_DrawVsync();	
}

void nes_Start(char * filename)
{
	strcpy(romname,filename);
	int romsize = emu_FileSize(filename);	
	romdata = (char*)emu_Malloc(romsize);
	if (romdata)
	{
		if (emu_FileOpen(filename)) {
			if (emu_FileRead((char*)romdata, romsize) != romsize ) {
			  	romdata = NULL;
			}
			emu_FileClose();
		} else {
			romdata = NULL;
		}  
	}
}
