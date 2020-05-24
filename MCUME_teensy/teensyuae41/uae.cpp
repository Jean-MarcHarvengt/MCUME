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
//#include "events.h"
#include "keyboard.h"
}

//#define HAS_MEMDISK 1

#define MAX_FILENAME 64

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

int quit_program = 0;
int buttonstate[3];
int lastmx, lastmy;
int newmousecounters;
char prtname[MAX_FILENAME] = "lpr ";

xcolnr xcolors[4096];
struct vidbuf_description gfxvidinfo;



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
      disk_swap();
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
  uae_Input(emu_DebounceLocalKeys());
    
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


CPTR audlc[4], audpt[4];
UWORD audvol[4], audper[4], audlen[4], audwlen[4];
int audwper[4];
UWORD auddat[4];
int audsnum[4];
int audst[4];

static short sound_table[256][64];
static UBYTE snddata[4];
#define SNDBUFSIZE 8192
static short buffer[SNDBUFSIZE];
static int wrpos=0;
static int rdpos = 0;
static int wrcount=0;
static int wrsam=0x3ab;
static int rdcount=0;
static int rdsam=0x1c0;
#define NUM rdsam
#define DEN wrsam


static void init_sound(void)
{
  audst[0] = audst[1] = audst[2] = audst[3] = 0;
  rdpos = 0;
  wrpos = SNDBUFSIZE/2;
  //wrpos = (wrpos*NUM);
    
  int i,j;
  for (i = 0; i < 256; i++)
    for (j = 0; j < 64; j++)
      sound_table[i][j] = j * (i-127);
}


static void channel_reload (int c)
{
    audst[c] = 1;
    audpt[c] = audlc[c];
    audwper[c] = 0;
    audwlen[c] = audlen[c];
    audsnum[c] = 1;    
}

extern "C" 
{
void do_sound (void)
{
  int smplcnt = -227;
  while (smplcnt < 0) 
  {
    smplcnt += 80;
    int i;
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
    
    //short s = snddata[0]*audvol[0]
    // + snddata[1]*audvol[1] 
    // + snddata[2]*audvol[2] 
    // + snddata[3]*audvol[3];
    
    int s = (sound_table[snddata[0]][audvol[0]] 
    + sound_table[snddata[1]][audvol[1]]
    + sound_table[snddata[2]][audvol[2]] 
    + sound_table[snddata[3]][audvol[3]]);
    buffer[wrpos++]=s>>2;
    wrpos &= (SNDBUFSIZE-1);
    wrcount++;
    /*
    int k = wrpos/DEN;
    buffer[k] = s>>1;
    wrpos += NUM;
    int l = wrpos/DEN;
    if (l >= SNDBUFSIZE) {
      wrpos = 0;
      wrpos = (l-SNDBUFSIZE)*NUM;
    }
    //if (k != l)
      wrcount++;
    */  
  } 
}
void do_sound_vsync (void)
{
  wrsam = wrcount;
  wrcount=0;
  rdsam = rdcount;
  rdcount=0;
  //emu_printf("wr");  
  //emu_printi(wrsam);  
  //emu_printf("rd");  
  //emu_printi(rdsam);  
}
}

void SND_Process(void *stream, int len) {
    short * data = (short*)stream;
    len = len >> 1; 
    for (int i=0;i<len;i++)
    {      
      short s = buffer[rdpos/rdsam];  
      *data++ = s;
      *data++ = s;
      rdpos += wrsam; 
      //rdpos &= (SNDBUFSIZE-1);
      if (rdpos/rdsam >= SNDBUFSIZE) rdpos = (rdpos/rdsam)-SNDBUFSIZE;
      rdcount++;
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
//char df0[MAX_FILENAME]="Workbench V1.3.ADF";
char df0[MAX_FILENAME]="";
char df1[MAX_FILENAME]="", df2[MAX_FILENAME]="df2.adf", df3[MAX_FILENAME]="df3.adf";

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
#ifdef HAS_SND  
  emu_sndInit();
//#ifdef SOUND_PRESENT
  produce_sound = 1;
  init_sound();
//#else
//  produce_sound = 0;
//#endif   
#endif  
  MC68000_reset();
  MC68000_run();
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
  strcpy(df0,floppy1);
  strcpy(df1,floppy2);
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











