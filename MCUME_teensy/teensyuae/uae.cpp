#include <string.h>

#include "emuapi.h"
#include "tft_t_dma.h"
#include "iopins.h" 

extern "C" {
#include "dtypes.h"

#include "memory.h"
#include "custom.h"
#include "newcpu.h"
#include "disk.h"
#include "keybuf.h"
#include "xwin.h"
#include "gui.h"
#include "events.h"
#include "keyboard.h"
}



#define MAX_FILENAME 2

int framerate = 1;
int dont_want_aspect = 1;
int use_debugger = 0;
int use_slow_mem = 0;
int use_gfxlib = 0;
int use_xhair = 0;
int use_lores = 1;
int automount_uaedev = 1;
int produce_sound = 0;
int fake_joystick = 0;
int screen_res = 0;
int color_mode = 0;
ULONG fastmem_size = 0;

int quit_program = 0;
int buttonstate[3];
int lastmx, lastmy;
int newmousecounters;
char prtname[MAX_FILENAME] = "lpr ";

xcolnr xcolors[4096];
struct vidbuf_description gfxvidinfo;

CPTR audlc[4], audpt[4];
UWORD audvol[4], audper[4], audlen[4], audwlen[4];
int audwper[4];
UWORD auddat[4];
int audsnum[4];

int audst[4];



#define MOUSE_STEP 2
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

void uae_Input(int click) {
  hk = emu_ReadI2CKeyboard();
  k = emu_ReadKeys(); 

  // Toggle mouse/joystick
  if (k & MASK_KEY_USER1) {
    if (isMouse) isMouse = false;
    else isMouse = true;
  }
  // Toggle keymap
  if (k & MASK_KEY_USER2) {
    emu_setKeymap(0);
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
      if ( lastmx < 320 ) {
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
      if ( lastmy < 240 ) {
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
  uae_Input(0);
    
} 

extern "C" void read_joystick (UWORD *dir, int *button);
void read_joystick(UWORD *dir, int *button)
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

int debuggable(void)
{
  return 1;
}

int needmousehack(void)
{
  return 1;
}

//static short sound_table[256][64];
static int smplcnt = 0;
static long count = 0;
/* The buffer is too large... */
//static UWORD buffer[44100], *bufpt;
static UBYTE snddata[4];
static int frq_divisor = (22050 / 50);

/*
static void init_sound_table16(void)
{
    int i,j;
    
    for (i = 0; i < 256; i++)
      for (j = 0; j < 64; j++)
        sound_table[i][j] = j * (BYTE)i;
}
*/

static void channel_reload (int c)
{
    audst[c] = 1;
    audpt[c] = audlc[c];
    audwper[c] = 0;
    audwlen[c] = audlen[c];
    audsnum[c] = 1;    
}

extern "C" void do_sound (void);
void do_sound (void)
{
  smplcnt -= 227;
  while (smplcnt < 0) {
    int i;
    smplcnt += 80;
  
    for(i = 0; i < 4; i++) 
    {
      if (dmaen (1<<i)) {
        if (audst[i] == 0) {    
          /* DMA was turned on for this channel */
          channel_reload (i);
          continue;
        }
  
        if (audwper[i] <= 0) {
          audwper[i] += audper[i];
          if (audst[i] == 1) {
            /*  Starting a sample, cause interrupt */
            put_word (0xDFF09C, 0x8000 | (0x80 << i));
            audst[i] = 2;
          }
          audsnum[i] ^= 1;
          if (audsnum[i] == 0) {
            auddat[i] = get_word (audpt[i]);
            audpt[i] += 2;
            audwlen[i]--;
            if (audwlen[i] == 0) {
              channel_reload (i);
            }
          }
        }
        if (adkcon & (0x11 << i)) {
          snddata[i] = 0;
          audsnum[i] ^= 2;
          audsnum[i] |= 1;
          if (i < 3) {
            if (adkcon & (0x11 << i)) {
              if (audsnum[i] & 2)
                audvol[i+1] = auddat[i];
              else
                audper[i+1] = auddat[i];
            } 
            else 
              if (adkcon & (1 << i)) 
              {
                audvol[i+1] = auddat[i];
              } 
              else 
              {
                audper[i+1] = auddat[i];
              }
          }
        } 
        else 
        {
          snddata[i] = audsnum[i] & 1 ? auddat[i] : auddat[i] >> 8;
        }
        audwper[i] -= 80;   
      } 
      else 
        audst[i] = snddata[i] = 0;
    }
    
    if ((count++ % frq_divisor) == 0) {
      /*
      long size;
      *bufpt++ = (sound_table[snddata[0]][audvol[0]] 
      + sound_table[snddata[1]][audvol[1]]
      + sound_table[snddata[2]][audvol[2]] 
      + sound_table[snddata[3]][audvol[3]];
      size = (char *)bufpt - (char *)buffer;
      if (size >= sndbufsize) {
        if (AFGetTime(ac) > aftime)
          aftime = AFGetTime(ac);
        AFPlaySamples(ac, aftime, size, (unsigned char*) buffer);
        aftime += size / 2;
        bufpt = buffer;
      }
      */
    }
  } 
}

void gui_led(int led, int on)
{
}

void LED(int on)
{
}

void flush_line(int y)
{
}

void flush_block(int ystart,int ystop)
{
}

void flush_screen(int ystart,int ystop)
{
  emu_DrawVsync();    
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

char df0[MAX_FILENAME]="",df1[MAX_FILENAME]="", df2[MAX_FILENAME]="", df3[MAX_FILENAME]="";

void uae_Init(void)
{
  emu_printf("Init");
  gfxvidinfo.pixbytes   = 2;
  
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
  keybuf_init();
  memory_init();
  custom_init();
  DISK_init();
  MC68000_reset();
  MC68000_run();
  produce_sound = 1;
#ifdef HAS_SND  
  emu_sndInit();
//#ifdef SOUND_PRESENT
  //init_sound_table16();
//#else
//  produce_sound = 0;
//#endif   
#endif  
}





void uae_Start(char * floppy)
{
  emu_printf("Start");

#ifdef HAS_PSRAM
  psram.begin();
  int offset = 0;
  int pos = 0;
  int n;
  int i;
  char buf[512];
  int size = 0;  
  if (emu_FileOpen(floppy)) {
    while ( (n = emu_FileRead(buf,512) ) ) {
      size += n;
      for (int i=0; i<n; i++) {
        write_rom(pos++,buf[i]);
      }
      //emu_printi(size);            
      //emu_printi(n);          
    }
    emu_FileClose();
  }
emu_printi(size);  
#else
  strcpy(df0,floppy);
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






void SND_Process(void *stream, int len) {
  //psg_update(stream, 0, len); 
  //mixaudio(stream, len); 
} 





