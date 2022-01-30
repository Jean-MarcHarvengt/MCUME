#include <string.h>

#include "emuapi.h"

#include "dcastaway.h"
#include "st.h"
#include "mem.h"
#include "m68k_intrf.h"
#include "xram.h"
#include "iopins.h"

#ifdef HAS_T4_VGA 
#include "vga_t_dma.h"
#endif

#ifndef NO_SOUND  
#include "sound.h"
#endif

#include "tossw14.h"

int8 *membase;
int8 *rombase;

static int hbl = 0;
static int hsync = 0;
static int vsyncpend = 0, hsyncpend = 0;

int exmousex=160,exmousey=100;
int CompleteSndBufIdx; 
int waitstate=0;
int maybe_border=0;
unsigned cyclenext=512;
unsigned vid_adr_cycleyet=0;
static unsigned char vid_cycles_pal[1024];
unsigned char *vid_cycle=(unsigned char *)&vid_cycles_pal;
extern unsigned char fdc_motor;

#define XRES 320
#define YRES 200

#ifdef HAS_T4_VGA 
#define DOUBLE_BUFFERING 1

#ifdef DOUBLE_BUFFERING
#ifdef HIRES
static vga_pixel line[XRES*2];
#else
static vga_pixel line[XRES];
#endif
#else
#ifdef HIRES
static unsigned short line[XRES*2];
#else
static unsigned short line[XRES];
#endif
#endif
#else
static unsigned short line[XRES];
#endif

#define PALMULT8(x)  ((x)<<5)
#define RGBVAL16(r,g,b)  ( (((r>>3)&0x1f)<<11) | (((g>>2)&0x3f)<<5) | (((b>>3)&0x1f)<<0) )

#ifndef DOUBLE_BUFFERING
void Redraw16 ( int row, int vid_adr ) 
{
  static unsigned short palmap [ 16 ];
  register unsigned short *line_i;
  line_i = (unsigned short *)(&membase[vid_adr]);
  register unsigned short *line_o= &line[0];
  if (vid_flag) {
    unsigned char i, r, g, b;
    for (i = 0; i < 16; i++) {
      b = PALMULT8 ( (vid_col[i] & 0x7) );
      g = PALMULT8 ( ((vid_col[i] >> 4) & 0x7) );
      r = PALMULT8 ( ((vid_col[i] >> 8) & 0x7) );
      palmap [ i ] = RGBVAL16(r,g,b); 
    }
    vid_flag=0;
  }
  register int col;
  register int bit;
  for (col=0; col<20; col++) {
    register unsigned short pl0=*line_i++,pl1=*line_i++,pl2=*line_i++,pl3=*line_i++;
    for (bit=15;bit>=0;bit--) {
      int ind = (pl0 >> bit) & 0x1;
      ind += ((pl1 >> bit) & 0x1)<<1;
      ind += ((pl2 >> bit) & 0x1)<<2;
      ind += ((pl3 >> bit) & 0x1)<<3;
      *line_o++ = palmap [ ind ]; 
    }
  }
#ifdef HIRES
  emu_DrawLine16(&line[0], XRES, YRES*2, row*2);  
  emu_DrawLine16(&line[0], XRES, YRES*2, row*2+1);  
#else
  emu_DrawLine16(&line[0], XRES, YRES, row);
#endif  
}

void Redraw16_med ( int row, int vid_adr )
{
  static unsigned short palmap [ 4 ];
  register unsigned short *line_i;
  line_i = (unsigned short *)(&membase[vid_adr]);
  register unsigned short *line_o= &line[0];
  if (vid_flag) {
    unsigned char i, r, g, b;
    for (i = 0; i < 4; i++) {
      b = PALMULT8 ( (vid_col[i] & 0x7) );
      g = PALMULT8 ( ((vid_col[i] >> 4) & 0x7) );
      r = PALMULT8 ( ((vid_col[i] >> 8) & 0x7) );
      palmap [ i ] = RGBVAL16(r,g,b); 
    }
    vid_flag=0;
  }
  register int col;
  register int bit;
  for (col=0; col<40; col++) {
    register unsigned short pl0=*line_i++,pl1=*line_i++;
    for (bit=15;bit>=0;bit--) {
      int ind = (pl0 >> bit) & 0x1;
      ind += ((pl1 >> bit) & 0x1)<<1;
#ifdef HIRES
      *line_o++ = palmap [ ind ]; 
#else
      if (bit & 0x01)
        *line_o++ = palmap [ ind ]; 
#endif
    }
  }
#ifdef HIRES
  emu_DrawLine16(&line[0], XRES*2, YRES*2, row*2);  
  emu_DrawLine16(&line[0], XRES*2, YRES*2, row*2+1);  
#else
  emu_DrawLine16(&line[0], XRES, YRES, row);
#endif  
}

void Redraw16_hi ( int row, int vid_adr ) 
{
#ifdef HIRES
  static unsigned short palmap [ 2 ];
  register unsigned short *line_i;
  line_i = (unsigned short *)(&membase[vid_adr]);
  register unsigned short *line_o= &line[0];
  if (vid_flag) {
    palmap [ 0 ] = RGBVAL16(0xff,0xff,0xff); 
    palmap [ 1 ] = RGBVAL16(0x00,0x00,0x00); 
    vid_flag=0;
  }
  register int col;
  register int bit;
  for (col=0; col<40; col++) {
    register unsigned short pl0=*line_i++;
    for (bit=15;bit>=0;bit--) {
      int ind = (pl0 >> bit) & 0x1;
      *line_o++ = palmap [ ind ]; 
    }
  }
  emu_DrawLine16(&line[0], XRES*2, YRES*2, row*2);  
  line_o= &line[0];   
  for (col=0; col<40; col++) {
    register unsigned short pl0=*line_i++;
    for (bit=15;bit>=0;bit--) {
      int ind = (pl0 >> bit) & 0x1;
      *line_o++ = palmap [ ind ]; 
    }
  }
  emu_DrawLine16(&line[0], XRES*2, YRES*2, row*2+1);  
#endif    
}
#endif


static uint8 disk0[256];
static uint8 disk1[256];


void ast_Init(void)
{
  emu_printf("Allocating RAM");
  membase = (int8*)xram_MemPool();

  for (int i=0; i<MEMSIZE;i++)
    membase[i]=0; 
        
  rombase = (int8*)&tos[0]-ROMBASE;   
  (void) memcpy (membase, &tos[0], 8);

  extern unsigned char * disc[2];
  disc [ 0 ] = (uint8*) &disk0[0];
  disc [ 1 ] = (uint8*) &disk1[0];
}

static int mouse_x = XRES/2;
static int mouse_y = 100;
static int prev_mouse_x = XRES/2;
static int prev_mouse_y = 100;

static int prev_key = 0; 
static int prev_j = 0; 
static int prev_mouseb1 = 0;
static int prev_mouseb2 = 0;
static bool isMouse = true;
static int joynum = 1;
static int hk = 0;
static int prev_hk = 0;
static int k = 0;
static int prev_k = 0;


#define INV_KEY 0

const int16_t keyboardAsciiConv[] = // QWERTY Keyboard
{
/* 0x00 */ INV_KEY,
/* 0x01 */ INV_KEY,
/* 0x02 */ INV_KEY,
/* 0x03 */ INV_KEY,
/* 0x04 */ INV_KEY,
/* 0x05 */ INV_KEY,
/* 0x06 */ INV_KEY,
/* 0x07 */ INV_KEY,
/* 0x08 */ INV_KEY,
/* 0x09 */ 0x0f,     // tab
/* 0x0A */ 0x1C,     // enter
/* 0x0B */ INV_KEY,
/* 0x0C */ INV_KEY,
/* 0x0D */ INV_KEY,
/* 0x0E */ INV_KEY,
/* 0x0F */ INV_KEY,
/* 0x10 */ INV_KEY,
/* 0x11 */ INV_KEY,
/* 0x12 */ INV_KEY,
/* 0x13 */ INV_KEY,
/* 0x14 */ INV_KEY,
/* 0x15 */ INV_KEY,
/* 0x16 */ INV_KEY,
/* 0x17 */ INV_KEY,
/* 0x18 */ INV_KEY,
/* 0x19 */ INV_KEY,
/* 0x1A */ INV_KEY,
/* 0x1B */ 0x01,     // esc
/* 0x1C */ INV_KEY,
/* 0x1D */ INV_KEY,
/* 0x1E */ INV_KEY,
/* 0x1F */ INV_KEY,
/* 0x20 */ 0x39,     // space
/* 0x21 */ INV_KEY,
/* 0x22 */ INV_KEY,
/* 0x23 */ INV_KEY,
/* 0x24 */ INV_KEY,
/* 0x25 */ INV_KEY,
/* 0x26 */ 43,       // ampercent (backslash)
/* 0x27 */ 40,       // single quote
/* 0x28 */ 26,       // bracket left
/* 0x29 */ 27,       // bracket right
/* 0x2A */ INV_KEY,  // mult *
/* 0x2B */ INV_KEY,
/* 0x2C */ 51,       // comma ,
/* 0x2D */ 12,       // minus -
/* 0x2E */ 52,       // period .
/* 0x2F */ 53,       // slash /
/* 0x30 */ 0x0B,     // 0
/* 0x31 */ 0x02,
/* 0x32 */ 0x03,
/* 0x33 */ 0x04,
/* 0x34 */ 0x05,
/* 0x35 */ 0x06,
/* 0x36 */ 0x07,
/* 0x37 */ 0x08,
/* 0x38 */ 0x09,
/* 0x39 */ 0x0A,     // 9
/* 0x3A */ INV_KEY,  // colon :
/* 0x3B */ 39,       // semi colon ;
/* 0x3C */ INV_KEY,
/* 0x3D */ 13,       // equal =
/* 0x3E */ INV_KEY,
/* 0x3F */ INV_KEY,
/* 0x40 */ INV_KEY,
/* 0x41 */ INV_KEY,
/* 0x42 */ INV_KEY,
/* 0x43 */ INV_KEY,
/* 0x44 */ INV_KEY,
/* 0x45 */ INV_KEY,
/* 0x46 */ INV_KEY,
/* 0x47 */ INV_KEY,
/* 0x48 */ INV_KEY,
/* 0x49 */ INV_KEY,
/* 0x4A */ INV_KEY,
/* 0x4B */ INV_KEY,
/* 0x4C */ INV_KEY,
/* 0x4D */ INV_KEY,
/* 0x4E */ INV_KEY,
/* 0x4F */ INV_KEY,
/* 0x50 */ INV_KEY,
/* 0x51 */ INV_KEY,
/* 0x52 */ INV_KEY,
/* 0x53 */ INV_KEY,
/* 0x54 */ INV_KEY,
/* 0x55 */ INV_KEY,
/* 0x56 */ INV_KEY,
/* 0x57 */ INV_KEY,
/* 0x58 */ INV_KEY,
/* 0x59 */ INV_KEY,
/* 0x5A */ INV_KEY,
/* 0x5B */ INV_KEY,
/* 0x5C */ INV_KEY,
/* 0x5D */ INV_KEY,
/* 0x5E */ INV_KEY,
/* 0x5F */ INV_KEY,
/* 0x60 */ INV_KEY,
/* 0x61 */ 0x1E,    // A
/* 0x62 */ 0x30,    // B
/* 0x63 */ 0x2E,    // C
/* 0x64 */ 0x20,    // D
/* 0x65 */ 0x12,    // E
/* 0x66 */ 0x21,    // F
/* 0x67 */ 0x22,    // G
/* 0x68 */ 0x23,    // H
/* 0x69 */ 0x17,    // I
/* 0x6A */ 0x24,    // J
/* 0x6B */ 0x25,    // K
/* 0x6C */ 0x26,    // L
/* 0x6D */ 0x32,    // M
/* 0x6E */ 0x31,    // N
/* 0x6F */ 0x18,    // O
/* 0x70 */ 0x19,    // P
/* 0x71 */ 0x10,    // Q
/* 0x72 */ 0x13,    // R
/* 0x73 */ 0x1F,    // S
/* 0x74 */ 0x14,    // T
/* 0x75 */ 0x16,    // U
/* 0x76 */ 0x2F,    // V
/* 0x77 */ 0x11,    // W
/* 0x78 */ 0x2D,    // X
/* 0x79 */ 0x15,    // Y
/* 0x7A */ 0x2C,    // Z
/* 0x7B */ INV_KEY,
/* 0x7C */ INV_KEY,
/* 0x7D */ INV_KEY,
/* 0x7E */ INV_KEY,
/* 0x7F */ 0x0E     // backspace
};


const int16_t keyboardSpecialConv[] = // Functions and other keys
{
/* 0xC0 */ INV_KEY,
/* 0xC1 */ INV_KEY,
/* 0xC2 */ 0x3b,   // F1
/* 0xC3 */ 0x3c,   // F2
/* 0xC4 */ 0x3d,   // F3
/* 0xC5 */ 0x3e,   // F4
/* 0xC6 */ 0x3f,   // F5
/* 0xC7 */ 0x40,   // F6
/* 0xC8 */ 0x41,   // F7
/* 0xC9 */ 0x42,   // F8
/* 0xCA */ 0x43,   // F9
/* 0xCB */ 0x44,   // F10
/* 0xCC */ INV_KEY,
/* 0xCD */ INV_KEY,
/* 0xCE */ INV_KEY,
/* 0xCF */ INV_KEY,
/* 0xD0 */ INV_KEY,
/* 0xD1 */ INV_KEY,
/* 0xD2 */ INV_KEY,
/* 0xD3 */ INV_KEY,
/* 0xD4 */ INV_KEY, // DEL
/* 0xD5 */ INV_KEY,
/* 0xD6 */ INV_KEY,
/* 0xD7 */ INV_KEY,
/* 0xD8 */ INV_KEY,
/* 0xD9 */ INV_KEY,
/* 0xDA */ INV_KEY,
/* 0xDB */ INV_KEY,
/* 0xDC */ INV_KEY,
/* 0xDD */ INV_KEY,
/* 0xDE */ INV_KEY,
/* 0xDF */ INV_KEY
};



void emu_KeyboardOnDown(int keymodifer, int key) {
  //emu_printi(key);
  int keyCode = INV_KEY;
  if ((key >=0xc0) && (key <=0xdf)) {
    keyCode = keyboardSpecialConv[(key-0xc0) & 0x1f];    
  }
  else {
    keyCode = keyboardAsciiConv[key & 0x7f];
  }  
  //emu_printi(keyCode);

  IkbdKeyPress ( keyCode );
}

void emu_KeyboardOnUp(int keymodifer, int key) {
  int keyCode = INV_KEY;
  if ((key >=0xc0) && (key <=0xdf)) {
    keyCode = keyboardSpecialConv[(key-0xc0) & 0x1f];    
  }
  else {
    keyCode = keyboardAsciiConv[key & 0x7f];
  }     
  IkbdKeyRelease ( keyCode );
}


extern void ast_Input(int click) {
  hk = emu_ReadI2CKeyboard();
  k = emu_ReadKeys();   
}

static void do_events(void) 
{
  int bClick = k & ~prev_k;
  prev_k = k;
  int  mouseb1 = prev_mouseb1;
  int  mouseb2 = prev_mouseb2;  
  
  // Toggle keymap + mouse/joystick
  if (bClick & MASK_KEY_USER2) {
    if (isMouse) isMouse = false;
    else isMouse = true;
#ifndef HAS_T4_VGA     
    emu_setKeymap(0);
#endif  
  }
  

  // force joystick mode if mouse detected
  if (emu_MouseDetected() ) isMouse = false;
  
  if (hk != prev_hk) {
    prev_hk = hk;
    if ( (hk != 0) && (hk != prev_key) ) {     
      prev_key = hk;
      int keyCode = INV_KEY;
      if ((prev_key >=0xc0) && (prev_key <=0xdf)) {
        keyCode = keyboardSpecialConv[(prev_key-0xc0) & 0x1f];    
      }
      else {
        keyCode = keyboardAsciiConv[prev_key & 0x7f];
      }  
      IkbdKeyPress ( keyCode );
    }     
  }
  if ( (hk == 0) && (prev_key) ) {
      int keyCode = INV_KEY;
      if ((prev_key >=0xc0) && (prev_key <=0xdf)) {
        keyCode = keyboardSpecialConv[(prev_key-0xc0) & 0x1f];    
      }
      else {
        keyCode = keyboardAsciiConv[prev_key & 0x7f];
      }  
      IkbdKeyRelease ( keyCode /*| 0x80*/ );
      prev_key = 0;
  }

  int buts,dx,dy;
  int mouseEvent = emu_GetMouse(&dx,&dy,&buts);
  if (mouseEvent){
    mouse_x += dx;
#ifdef HIRES
    if (vid_shiftmode==COL2) {
      if ( mouse_x >= XRES*2 ) {
        mouse_x = XRES*2-1;
      }      
    }
    else  
#endif    
    if ( mouse_x >= XRES ) {
      mouse_x = XRES-1;
    } 
    else if ( mouse_x < 0 ) {
      mouse_x = 0;     
    }
    mouse_y += dy;
    if ( mouse_y >= YRES ) {
      mouse_y = YRES-1;
    } 
    else if ( mouse_y < 0 ) {
      mouse_y = 0;      
    }
    
    if (buts & 0x1) {
      mouseb1 |=1; 
    }
    else {
      mouseb1 &=~1;       
    }
    if (buts & 0x2) {
      mouseb2 |=1;
    }
    else {
      mouseb2 &=~1;     
    }
  }

  if (!isMouse)
  {
    int j = 0;     
    if (( k & MASK_JOY1_RIGHT) || ( k & MASK_JOY2_RIGHT)) {
      j |= 0x08;
    }
    if (( k & MASK_JOY1_LEFT) || ( k & MASK_JOY2_LEFT)) {
      j |= 0x04;
    }
    if (( k & MASK_JOY1_UP) || ( k & MASK_JOY2_UP)) {
      j |= 0x01;
    }
    if (( k & MASK_JOY1_DOWN) || ( k & MASK_JOY2_DOWN)) {
      j |= 0x02;
    }  
    if (( k & MASK_JOY1_BTN) || ( k & MASK_JOY2_BTN)) {
      j |= 0x80;
    }
    if (j != prev_j) {
      IkbdJoystickChange(joynum,j);
      prev_j = j;
    }
  }
  else {
    if (( k & MASK_JOY1_RIGHT) || ( k & MASK_JOY2_RIGHT)) {
      mouse_x += 1;
#ifdef HIRES
      if (vid_shiftmode==COL2) {
        if ( mouse_x >= XRES*2 ) {
          mouse_x = XRES*2-1;
        }      
      }
      else  
#endif    
      if ( mouse_x >= XRES ) {
        mouse_x = XRES-1;
      } 
    }
    else if (( k & MASK_JOY1_LEFT) || ( k & MASK_JOY2_LEFT)) {
      mouse_x -= 1;       
      if ( mouse_x < 0 ) {
        mouse_x = 0;       
      }
    }
    else if (( k & MASK_JOY1_UP) || ( k & MASK_JOY2_UP)) {
      mouse_y -= 1;       
      if ( mouse_y < 0 ) {
        mouse_y = 0;
      }
    }
    else if (( k & MASK_JOY1_DOWN) || ( k & MASK_JOY2_DOWN)) {
      mouse_y += 1;       
      if ( mouse_y >= YRES ) {
         mouse_y = YRES-1;
     }
    }  
    
    if (( k & MASK_JOY1_BTN) || ( k & MASK_JOY2_BTN)) {
      mouseb1 |= 1;
    }
    else  {
      mouseb1 &=~1;  
    }
    if (( k & MASK_KEY_USER1)) {
      mouseb2 |= 1;
    }
    else  {
      mouseb2 &=~1;  
    }    
  }

  if ( (prev_mouse_x != mouse_x) | (prev_mouse_y != mouse_y) ) {
      IkbdMouseMotion ( mouse_x, mouse_y );
      IkbdLoop(); 
      prev_mouse_x = mouse_x;
      prev_mouse_y = mouse_y;
  }


  if ( (mouseb1 != prev_mouseb1) ){
    if (mouseb1) {
      IkbdMousePress(2);
      //emu_printf("b1p");
    }
    else {
      IkbdMouseRelease(2);
      //emu_printf("b1r");
    }
    IkbdLoop();
    prev_mouseb1 = mouseb1;  
  }    
  if ( (mouseb2 != prev_mouseb2) ){
    if (mouseb2) {
      IkbdMousePress(1);
      //emu_printf("b2p");
    }
    else {
      IkbdMouseRelease(1);
      //emu_printf("b2r");
    }
    IkbdLoop();
    prev_mouseb2 = mouseb2;  
  }    
}     

#ifdef DOUBLE_BUFFERING
static unsigned short lines[200*80];
#ifdef HAS_T4_VGA   
static unsigned char palettes[200*16];
#else
static unsigned short palettes[200*16];
#endif
static unsigned char modes[200];

static void renderScreen(void) {
  register int col;
  register int bit;  
  int row;
  for (row=0; row<200; row++) {
    int mode= modes[row];
    unsigned short * line_i = &lines[80*row];
#ifdef HAS_T4_VGA 
    vga_pixel * palmap = &palettes[row*16];
    register vga_pixel *line_o= (unsigned char *)&line[0];    
#else
    unsigned short * palmap = &palettes[row*16];
    register unsigned short *line_o= &line[0];    
#endif    
    if (mode==MONO) {
#ifdef HIRES
      for (col=0; col<40; col++) {
        register unsigned short pl0=*line_i++;
        for (bit=15;bit>=0;bit--) {
          int ind = (pl0 >> bit) & 0x1;
          *line_o++ = palmap [ ind ]; 
        }
      }
      emu_DrawLine8((vga_pixel*)&line[0], XRES*2, YRES*2, row*2); 
      line_o= (vga_pixel *)&line[0];   
      for (col=0; col<40; col++) {
        register unsigned short pl0=*line_i++;
        for (bit=15;bit>=0;bit--) {
          int ind = (pl0 >> bit) & 0x1;
          *line_o++ = palmap [ ind ]; 
        }
      }
      emu_DrawLine8((unsigned char*)&line[0], XRES*2, YRES*2, row*2+1); 
#endif
    }
    else if (mode==COL2) {        
      for (col=0; col<40; col++) {
        register unsigned short pl0=*line_i++,pl1=*line_i++;
        for (bit=15;bit>=0;bit--) {
          int ind = (pl0 >> bit) & 0x1;
          ind += ((pl1 >> bit) & 0x1)<<1;
#ifdef HIRES
          *line_o++ = palmap [ ind ]; 
#else
          if (bit & 0x01)
            *line_o++ = palmap [ ind ]; 
#endif
        }
      }
#ifdef HAS_T4_VGA 
#ifdef HIRES
      emu_DrawLine8((unsigned char*)&line[0], XRES*2, YRES*2, row*2); 
      emu_CopyLine(XRES*2, YRES*2, row*2, row*2+1);       
#else
      emu_DrawLine8((unsigned char*)&line[0], XRES, YRES, row);     
#endif     
#else
      emu_DrawLine16(&line[0], XRES, YRES, row);
#endif 
    }
    else { // COL4
      for (col=0; col<20; col++) {
        register unsigned short pl0=*line_i++,pl1=*line_i++,pl2=*line_i++,pl3=*line_i++;
        for (bit=15;bit>=0;bit--) {
          int ind = (pl0 >> bit) & 0x1;
          ind += ((pl1 >> bit) & 0x1)<<1;
          ind += ((pl2 >> bit) & 0x1)<<2;
          ind += ((pl3 >> bit) & 0x1)<<3;
          *line_o++ = palmap [ ind ]; 
        }
      }
#ifdef HAS_T4_VGA 
#ifdef HIRES
      emu_DrawLine8((unsigned char*)&line[0], XRES, YRES*2, row*2);
      emu_CopyLine(XRES*2, YRES*2, row*2, row*2+1);       
#else
      emu_DrawLine8((unsigned char*)&line[0], XRES, YRES, row);     
#endif     
#else
      emu_DrawLine16(&line[0], XRES, YRES, row);
#endif 
    }
  }
}


static copyLine(int row, int vid_adr, int mode) 
{
#ifdef HAS_T4_VGA 
  vga_pixel * palmap=&palettes[row*16];
#else
  unsigned short * palmap=&palettes[row*16];
#endif  
  unsigned char i, r, g, b;   
  modes[row] = mode;
  if (vid_flag) {
    vid_flag=0;
  } 
  if (mode==MONO) { 
 #ifdef HAS_T4_VGA 
    palmap [ 0 ] = VGA_RGB(0xff,0xff,0xff); 
    palmap [ 1 ] = VGA_RGB(0x00,0x00,0x00);      
 #else
    palmap [ 0 ] = RGBVAL16(0xff,0xff,0xff); 
    palmap [ 1 ] = RGBVAL16(0x00,0x00,0x00);      
 #endif    
  }
  else if (mode==COL2) { 
    for (i = 0; i < 4; i++) {
      b = PALMULT8 ( (vid_col[i] & 0x7) );
      g = PALMULT8 ( ((vid_col[i] >> 4) & 0x7) );
      r = PALMULT8 ( ((vid_col[i] >> 8) & 0x7) );
 #ifdef HAS_T4_VGA 
     palmap [ i ] = VGA_RGB(r,g,b); 
 #else
     palmap [ i ] = RGBVAL16(r,g,b); 
 #endif
    }     
  }
  else {    
    for (i = 0; i < 16; i++) {
      b = PALMULT8 ( (vid_col[i] & 0x7) );
      g = PALMULT8 ( ((vid_col[i] >> 4) & 0x7) );
      r = PALMULT8 ( ((vid_col[i] >> 8) & 0x7) );
 #ifdef HAS_T4_VGA 
     palmap [ i ] = VGA_RGB(r,g,b); 
 #else
     palmap [ i ] = RGBVAL16(r,g,b); 
 #endif
    }
  }
  memcpy(&lines[row*80], &membase[vid_adr], 160);
}
#endif


void ast_Step(void)
{
  int delay_fdc_motor=0;
  unsigned long cycleco=0;
  unsigned long oldpend,newpend;
  hsync=0;
  hbl=0;
  vsyncpend=0;
  hsyncpend=0;

  int running = 1;
 /* Event loop */
  while (running) 
  {
      cycleco=cpu_loop(cyclenext);
      cycleco+=waitstate;
      waitstate=0;
#ifndef NO_SOUND
      SoundCycles+=cycleco;
#endif            
      //MFP timer A delay mode
      if (mfp_ascale>1) {
          mfp_acount-=mfp_ascale*cycleco;
          if (mfp_acount<=0) {
            do {mfp_acount+=mfp_tadr;} while (mfp_acount<=0);
            oldpend=mfp_ipra; newpend=(oldpend|0x20)&mfp_iera;
            if (newpend!=oldpend) {mfp_ipra=newpend;

            }
          }
  #ifdef USE_SHORT_SLICE
          cyclenext=4+(mfp_acount/mfp_ascale);
  #endif
      }
  #ifdef USE_SHORT_SLICE
      else
          cyclenext=512;
  #endif
      //MFP timer B delay mode
      if (mfp_bscale>1) {
          mfp_bcount-=mfp_bscale*cycleco;
          if (mfp_bcount<=0) {
            do {mfp_bcount+=mfp_tbdr;} while (mfp_bcount<=0);
              oldpend=mfp_ipra; newpend=(oldpend|0x1)&mfp_iera;
              if (newpend!=oldpend) {mfp_ipra=newpend;
              }
          }
  #ifdef USE_SHORT_SLICE
          {
              int n=4+(mfp_bcount/mfp_bscale);
              if (n<cyclenext)
                  cyclenext=n;
          }
  #endif
      }
      //MFP timer C delay mode
      if (mfp_cscale>1) {
          mfp_ccount-=mfp_cscale*cycleco;
          if (mfp_ccount<=0) {
            do {mfp_ccount+=mfp_tcdr;} while (mfp_ccount<=0);
              oldpend=mfp_iprb; newpend=(oldpend|0x20)&mfp_ierb;
              if (newpend!=oldpend) {mfp_iprb=newpend;
              }
          }
  #ifdef USE_SHORT_SLICE
          {
              int n=4+(mfp_ccount/mfp_cscale);
              if (n<cyclenext)
                  cyclenext=n;
          }
  #endif
      }
      //MFP timer D delay mode
      if (mfp_dscale>1) {
          mfp_dcount-=mfp_dscale*cycleco;
          if (mfp_dcount<=0) {
            do {mfp_dcount+=mfp_tddr;} while (mfp_dcount<=0);
              oldpend=mfp_iprb; newpend=(oldpend|0x10)&mfp_ierb;
              if (newpend!=oldpend) {mfp_iprb=newpend;
              }
          }
  #ifdef USE_SHORT_SLICE
          {
              int n=4+(mfp_dcount/mfp_dscale);
              if (n<cyclenext)
                  cyclenext=n;
          }
  #endif
      }
    
      
      vid_adr+=(cycleco)&(~3);

      //Count hbls
      hsync-=cycleco;

    if (hsync<=0) {
      hbl++; hsync+=512;
      //Generate hbl interrupt
      Interrupt(AUTOINT2, 2);
      

      //Trigger event counters for visible scan-lines
      if (hbl<64) {
        vid_adr=(vid_baseh<<16)+(vid_basem<<8);
      }
      else if (hbl<264) {
        if ( (hbl == 132 )) {
//        if ( (hbl % (end_visible_screen/2) ) == 0) {
            //CPUEvent();
            //timer_tick = 1; 
          }
        //Do IO every 64 hbls
        if (!(hbl&63)) {
          //Generate FDC interrupt in mfp?
          if (!(mfp_gpip & 0x20)) {
            mfp_iprb |= 0x80;
            mfp_iprb &= mfp_ierb;           
          } 
          //Generate ACIA interrupt in mfp?
          IkbdWriteBuffer();
          if (!(mfp_gpip & 0x10)) {
            mfp_iprb |= 0x40;
            mfp_iprb &= mfp_ierb;           
          }
        }


        if (emu_FrameSkip() == 0 ) {
          //Update video address and draw screen line
          vid_adr=(vid_baseh<<16)+(vid_basem<<8)+(hbl-64)*160;
#ifdef DOUBLE_BUFFERING
          copyLine(hbl - 64, vid_adr,vid_shiftmode); 
#else          
          
          if (vid_shiftmode==MONO) {
            Redraw16_hi( hbl - 64, vid_adr );
          }  
          else if (vid_shiftmode==COL2) {
            Redraw16_med( hbl - 64, vid_adr );
          } else {
            Redraw16( hbl - 64, vid_adr );
          }  
#endif         
        }

        //Timer-A event count mode
        if (mfp_ascale==1) {
          mfp_acount-=1048576;
          if (mfp_acount<=0) {
            mfp_acount=mfp_tadr;
            oldpend=mfp_ipra; newpend=(oldpend|0x20)&mfp_iera;
            if (newpend!=oldpend) {mfp_ipra=newpend;
            }
          }
        }
        //Timer-B event count mode
        if (mfp_bscale==1) {
          mfp_bcount-=1048576;
          if (mfp_bcount<=0) {
            mfp_bcount=mfp_tbdr;
            oldpend=mfp_ipra; newpend=(oldpend|0x1)&mfp_iera;
            if (newpend!=oldpend) {mfp_ipra=newpend;
            }
          }
        }
      }


      //Vertical blank?
      else if (hbl>=313)
      {
#ifdef DOUBLE_BUFFERING        
        if (vid_shiftmode==MONO) {
          emu_DrawWaitLine(440);
          emu_tweakVideo(1,0,0);
        }  
        else if (vid_shiftmode==COL2) {
          emu_DrawWaitLine(440);
          emu_tweakVideo(1,0,0);          
        } else {
          emu_DrawWaitLine(440);
          emu_tweakVideo(0,0,0);          
        }
        renderScreen();        
#else
        emu_DrawVsync();
#endif        
        do_events();
#ifndef NO_SOUND  
        Sound_Update_VBL();
#endif        
        running=0;  

             
        hbl=0;
        //Generate vsync interrupt
        Interrupt(AUTOINT4, 4);
        //Do fdc spinup
        if (fdc_motor){
          if (delay_fdc_motor>150) {
            fdc_status &= ~0x80;
            delay_fdc_motor=0;
            fdc_motor=0;
          }
          else delay_fdc_motor++;
        }
      }
    }


    //Recalculate interrupts?
    {
      if (6>GetI()) {
        //Mfp interrupt
        {
          int number;
          uint16 imr, ipr, isr, irr;
          int in_request;
          //Find in_request and in_service
          imr = (mfp_imra<<8)+mfp_imrb;
          ipr = (mfp_ipra<<8)+mfp_iprb;
          irr = imr & ipr;
          isr = (mfp_isra<<8) + mfp_isrb;
          //in_request higher than in_service?
          if (irr>isr) {
            //Find highest set bit
            for (in_request = 15; in_request > 0; in_request--) {
              if (irr & 0x8000) break;
              irr <<= 1;
            }
            isr = 1 << in_request;
            //Set interrupt in service bits in MFP
            if (mfp_ivr & 0x8) {
              mfp_isra |= isr >> 8;
              mfp_isrb |= isr;
            }else{
              mfp_isra &= (~isr) >> 8;
              mfp_isrb &= ~isr;
            }
            //Clear interrupt pending bits in MFP
            mfp_ipra &= ~(isr >> 8);
            mfp_iprb &= ~isr;
            //Pass interrupt to cpu
            number = in_request | (mfp_ivr & 0xf0);
            Interrupt(number, 6);
          }
        }
      }     
    }
  }
}



// disk IO mapped to File

int disk_Size(char * filename) {
  return emu_FileSize(filename);
}

int disk_Open(char * filename) {   
  return emu_FileOpen(filename,"a+r");
}

int disk_Read(char * buf, int size, int handler) {    
  return emu_FileRead(buf, size, handler);
}

int disk_Seek(int seek, int handler) { 
  return emu_FileSeek(handler, seek, 0);
}

void ast_Start(char * floppy1, char * floppy2, int mode)
{
  if (mode) display_mode = MONO;
  
  emu_printf("init started");
  strncpy (disk[0].name, floppy1, sizeof(disk[0].name));
  strncpy (disk[1].name, floppy2, sizeof(disk[1].name));

  initialize_memmap();
  FDCInit(0);
  FDCInit(1);
  //FDCeject(0);
  //FDCeject(1);
  IkbdReset(); 
  IOInit();   
#ifdef HAS_SND
  emu_sndInit();
#endif 
#ifndef NO_SOUND
#ifdef HAS_SND 
  Sound_Init(); 
#endif  
#endif   

  HWReset(); /* CPU Reset */

  emu_printf("init done");
}

void SND_Process(void *stream, int len) {
#ifndef NO_SOUND
#ifdef HAS_SND 
  Sound_UpdateFromCallBack16((short *)stream, len);
#endif  
#endif
} 
