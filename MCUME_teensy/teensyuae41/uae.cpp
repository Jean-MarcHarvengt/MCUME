#include <string.h>

#include "emuapi.h"
#include "tft_t_dma.h"
#include "iopins.h" 

extern "C" {
#include "dtypes.h"
#include "options.h"
#include "autoconf.h"
#include "memory.h"
#include "custom.h"
//#include "os.h"
//#include "newcpu.h"
#include "disk.h"
#include "xwin.h"
#include "gensound.h"
//#include "gui.h"
#include "events.h"
#include "sounddep/sound.h"
#include "keyboard.h"
void record_key (int);
void keybuf_init (void);
extern void serial_init(void);
extern void init_m68k(void);
extern void m68k_reset(void);
extern void m68k_go(int);
}

#define WIN_W                     TFT_WIDTH
#define WIN_H                     TFT_HEIGHT

//#define HAS_MEMDISK 1

int version = 100*UAEMAJOR + 10*UAEMINOR + UAEURSAMINOR;
struct uae_prefs changed_prefs;
struct uae_prefs currprefs = {
/* framerate */ 1,
/* illegal_mem */ 0,
/* no_xhair */ 0,
/* use_serial */ 0,
/* automount_uaedev */ 1,

/* fake_joystick */ 2 + (0<<8),
/* KbdLang keyboard_lang */ KBD_LANG_US,
/* allow_save */ 0,
/* emul_accuracy */ 2,
/* test_drawing_speed */ 0,

/* produce_sound */ 0,
/* sound_bits */ DEFAULT_SOUND_BITS,
/* sound_freq */ DEFAULT_SOUND_FREQ,
/* sound_minbsiz */ DEFAULT_SOUND_MINB,
/* sound_maxbsiz */ DEFAULT_SOUND_MAXB,

/* gfx_width */ WIN_W,
/* gfx_height */ WIN_H,
/* gfx_lores */ 1,
/* gfx_linedbl */ 0,
/* gfx_correct_aspect */ 0,
/* gfx_xcenter */ 40,
/* gfx_ycenter */ 40,
/* color_mode */ 0,
    
/* immediate_blits */ 0,
/* blits_32bit_enabled */ 0,
    
/* floppies */ { "df0.adf", "df1.adf", "df2.adf", "df3.adf" },
/* hardfile */ "hardfile", 
};

char df0[MAX_FILENAME]="df0.adf";
char df1[MAX_FILENAME]="df1.adf", df2[MAX_FILENAME]="df2.adf", df3[MAX_FILENAME]="df3.adf";

int quit_program = 0;
int buttonstate[3];
int lastmx, lastmy;
int newmousecounters;
xcolnr xcolors[4096];
struct vidbuf_description gfxvidinfo;
char romfile[MAX_FILENAME] = "./kick13.rom";
char prtname[MAX_FILENAME] = "lpr ";
char sername[MAX_FILENAME] = "";
char warning_buffer[256];
static char slinebuf[800];



#define MOUSE_STEP 4
static int prev_hk = 0;
static int hk = 0;
static int prev_key = 0;
static int k = 0;
static bool isMouse = true;

const int keyconv[] = 
{
  AK_1,AK_2,AK_3,AK_4,AK_5,AK_6,AK_7,AK_8,AK_9,AK_0,
  AK_Q,AK_W,AK_E,AK_R,AK_T,AK_Y,AK_U,AK_I,AK_O,AK_P,
  AK_A,AK_S,AK_D,AK_F,AK_G,AK_H,AK_J,AK_K,AK_L,AK_ENT,
  AK_Z,AK_C,AK_C,AK_V,AK_B,AK_N,AK_M,AK_COMMA,AK_BS,AK_SPC,
  AK_F1,AK_F2,AK_F3,AK_F4,AK_F5,AK_F6,AK_F7,AK_F8,AK_F9,AK_F10,  
};

void uae_Input(int bClick) {
  hk = emu_ReadI2CKeyboard();
  k = emu_ReadKeys(); 
  
  // Toggle mouse/joystick
  if (bClick & MASK_KEY_USER1) {
    if (isMouse) isMouse = false;
    else isMouse = true;
  }

  // Toggle keymap
  if (bClick & MASK_KEY_USER2) {
    if (isMouse) {
      emu_setKeymap(0);
    }
    else {
      disk_swap(df0,df1);
    }
  }   

  if (hk != prev_hk) {
    prev_hk = hk;
    if ( (hk != 0) && (hk != prev_key) ) {
      prev_key = hk;
      int iAmigaKeyCode = keyconv[hk-1];
      record_key(iAmigaKeyCode << 1);
    }     
  }
  if ( (hk == 0) && (prev_key) ) {
      int iAmigaKeyCode = keyconv[prev_key-1];
      record_key((iAmigaKeyCode << 1) | 1);      
      prev_key = 0;
  }           
  if (isMouse)
  {
    if (( k & MASK_JOY1_RIGHT) || ( k & MASK_JOY2_RIGHT)) {
      if ( lastmx < 640 ) {
        lastmx += MOUSE_STEP;
        newmousecounters = 1; 
      } 
    }
    else if (( k & MASK_JOY1_LEFT) || ( k & MASK_JOY2_LEFT)) {
      if ( lastmx >= MOUSE_STEP ) {
        lastmx -= MOUSE_STEP;
        newmousecounters = 1;       
      }
    }
    else if (( k & MASK_JOY1_UP) || ( k & MASK_JOY2_UP)) {
      if ( lastmy >= MOUSE_STEP ) {
        lastmy -= MOUSE_STEP;
        newmousecounters = 1;       
      }
    }
    else if (( k & MASK_JOY1_DOWN) || ( k & MASK_JOY2_DOWN)) {
      if ( lastmy < 480 ) {
        lastmy += MOUSE_STEP;
        newmousecounters = 1;        
      }
    } 

    buttonstate[0] = 0;
    if (k & MASK_JOY2_BTN) buttonstate[0] = 1;             
  }
}

void handle_events(void)
{
  uae_Input(emu_DebounceLocalKeys());
} 





extern "C" {

uae_u16 sndbuffer[sndbufsize];
uae_u16 * sndbufpt;
static uae_u32 *sndbuffer32=(uae_u32 *)sndbuffer;
static uae_u32 sndbufrdpt=sndbufsize/4;
static uae_u32 sndinc=0x100; // default read increment
#define sndbufrdmask ((sndbufsize/2-1)<<8)+0xff

void read_joystick(int nr, unsigned int *dir, int *button)
{
  int left = 0, right = 0, top = 0, bot = 0;
  *dir = 0;
  *button = 0;

  if (!isMouse)
  {
    if (k & MASK_JOY2_DOWN)  bot=1;
    if (k & MASK_JOY2_UP)    top=1;
    if (k & MASK_JOY2_LEFT)  left=1;
    if (k & MASK_JOY2_RIGHT) right=1;
    if (k & MASK_JOY2_BTN) *button |=0xFF;
    //buttonstate[0] = 0;
    //if (k & MASK_KEY_USER2) buttonstate[0] = 1;  
  
    if (left) top = !top;
    if (right) bot = !bot;
    *dir = bot | (right << 1) | (top << 8) | (left << 9);
  }  
}
  
void uae_reset (void)
{
  if (quit_program != 1 && quit_program != -1)
    quit_program = -2;
}

void gui_led(int led, int on){}
int needmousehack(void) { return 1;}
void LED(int on) {}
void write_log (const char *buf) { /*fprintf (stderr, buf); */ }

void flush_line(int y)
{
    if(y >= 0 && y < WIN_H) {
      emu_DrawLine16((unsigned short *)slinebuf, WIN_W , 1, y);
    }  
}

void flush_block(int ystart,int ystop){}

void flush_screen(int ystart,int ystop)
{
  uae_u32 loc = (sndbufpt-sndbuffer)/2;
  int delta = (sndbufrdpt>>8)-loc;
  if (delta < 0) delta = (sndbufrdpt>>8) + (sndbufsize/2)-loc;
  sndinc=((sndbufsize/4)<<8)/delta;
      
  emu_DrawVsync();    
}

}




#ifdef HAS_PSRAM
#include "psram_t.h"

PSRAM_T psram = PSRAM_T(PSRAM_CS, PSRAM_MOSI, PSRAM_SCLK, PSRAM_MISO);

extern "C" unsigned char read_rom(int address) {
  return (psram.psread(address)); 
}

extern "C" void  write_rom(int address, unsigned char val)  {
  psram.pswrite(address,val); 

}
#endif


void SND_Process(void *stream, int len) {  
    short * data = (short*)stream;
    len = len >> 1;
    for (int i=0;i<len;i++)
    {      
      uae_u32 s = sndbuffer32[sndbufrdpt>>8];  
      *data++ = (s >> 16);
      *data++ = (s & 0xffff);
      sndbufrdpt += sndinc;
      sndbufrdpt &= sndbufrdmask;
    }    
} 


int init_sound (void)
{
  currprefs.sound_maxbsiz = sndbufsize;
  sample_evtime = (long)maxhpos * maxvpos * 50 / currprefs.sound_freq;
  init_sound_table16 ();
  eventtab[ev_sample].handler = sample16_handler;
  int i;
  for (i=0;i<sndbufsize;i++) {
    sndbuffer[i] = 0;
  }
  sndbufpt = sndbuffer;
  sndbufrdpt=(sndbufsize/4)<<8;
  sound_available = 1;  
  return 1;
}


static int hddfound=0;

void uae_Init(void)
{
  emu_printf("Init");
  gfxvidinfo.rowbytes   = WIN_W*2;
  gfxvidinfo.pixbytes   = 2;
  gfxvidinfo.maxlinetoscr = WIN_W;
  gfxvidinfo.maxline = WIN_H;
  gfxvidinfo.linemem = slinebuf;
  gfxvidinfo.maxblocklines  = 0;
  
  buttonstate[0] = buttonstate[1] = buttonstate[2] = 0;
  lastmx = lastmy = 0;
  newmousecounters = 0;
  int i;
  // Palette
  for (int i = 0; i < 4096; i++) {
    int r = i >> 8;
    int g = (i >> 4) & 0xF;
    int b = i & 0xF;
    xcolors[i] = RGBVAL16(r<<4,g<<4,b<<4);
  } 



  quit_program = 2;

#ifdef HAS_HARDFILE
  rtarea_init ();
  hardfile_install ();
#endif

#ifdef SOUND_PRESENT
#ifdef HAS_SND  
  emu_sndInit();
#endif  
  init_sound();
  currprefs.produce_sound = 2;
#else
  currprefs.produce_sound = 0;
#endif

  keybuf_init();
#ifdef HAS_EXPANMEM  
  expansion_init ();
#endif  
  memory_init();
#ifdef HAS_FILESYS
  filesys_install();
#endif  
  custom_init();
  serial_init();
  init_m68k();
  //customreset();
  // JMH to fix hdd
  if (!hddfound) {
    DISK_init();
    disk_swap(df0,df1);    
  }
  emu_printf("cpu reset");
  m68k_reset();
  emu_printf("cpu go");
  m68k_go(1);
}

#ifdef HAS_MEMDISK

static void floppy_to_mem(char * floppy, int region) {
  int pos = FLOPPY_SIZE*region;
//  for (int i=pos; i<(pos+FLOPPY_SIZE); i++) {
//#ifdef HAS_PSRAM
//    write_rom(i,0);
//#else
//    disk_memWrite(i,0);
//#endif  
//  }
  int n;
  char buf[512];
  int size = 0;  
  if (emu_FileOpen(floppy)) {
    while ( (n = emu_FileRead(buf,512) ) ) {
      size += n;
      for (int i=0; i<n; i++) {
#ifdef HAS_PSRAM
        write_rom(pos++,buf[i]);
#else
        disk_memWrite(pos++,buf[i]);
#endif  
      }
      //emu_printi(size);            
      //emu_printi(n);          
    }
    emu_FileClose();
  } 
}
#endif

void uae_Start(char * floppy1, char * floppy2)
{
  emu_printf("Start");
  emu_printf(floppy1);  
  emu_printf(floppy2);  
#ifdef HAS_MEMDISK
#ifdef HAS_PSRAM
  psram.begin();
#endif 
  floppy_to_mem(floppy1, 0);
  floppy_to_mem(floppy2, 1); 
#else
  int strl = strlen(floppy1);
  if ( (strl >= 4) ) {
    emu_printf(&floppy1[strl-4]);    
    if ( (!strcmp(&floppy1[strl-4], ".hdf")) || (!strcmp(&floppy1[strl-4], ".HDF")) ) {
      emu_printf("file is hdd");
      strcpy(currprefs.hf0,floppy1);
      hddfound = 1;
    }
  }
  
  if (!hddfound) {
    strcpy(df0,floppy1);
    strcpy(df1,floppy2);  
  }

#endif  

  emu_printf("Start done");
}

void uae_Step(void) {
  /*
  JoyState = 0;
  if (k & MASK_JOY2_DOWN)  JoyState|=0x02;
  if (k & MASK_JOY2_UP)    JoyState|=0x01;
  if (k & MASK_JOY2_LEFT)  JoyState|=0x04;
  if (k & MASK_JOY2_RIGHT) JoyState|=0x08;
  if (k & MASK_JOY2_BTN) JoyState|=0x10; 
  if (k & MASK_KEY_USER2) JoyState|=0x20;  
*/
/*
  if (hk != 0) {
    emu_printh(hk);
    KeyMap[Keys[hk-1].Pos] &=~ Keys[hk-1].Mask;   
  }
  else  {
    memset(KeyMap,0xFF,16);
  }
*/

  //emu_DrawVsync();    
}











