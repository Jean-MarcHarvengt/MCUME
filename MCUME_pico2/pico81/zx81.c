#include "z80.h"
//#include "Arduino.h"
#include "zx80rom.h"
#include "zx81rom.h"
#include "emuapi.h"
#include "common.h"
#include "AY8910.h"
#include "kbd.h"

#define MEMORYRAM_SIZE 0x10000

static AY8910 ay;
static byte memory[ MEMORYRAM_SIZE ];
static int unexpanded=0;
static int signal_int_flag=0;
unsigned char * zxmemory;
unsigned char *memptr[64];
int memattr[64];
int nmigen=0,hsyncgen=0,vsync=0;
int vsync_visuals=0;
int interrupted=0;
int ramsize=32; //32;
int zx80=0;
int autoload=1;

/* the keyboard state and other */
static byte keyboard[ 8 ] = {0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff};;
static byte XBuf[ WIDTH*8 ]; 


struct { unsigned char R,G,B; } Palette[2] = {
  {   0,   0,   0},
  { 255, 255, 255}
};


const byte map_qw[8][5] = {
    {25, 6,27,29,224},// vcxz<caps shift=Lshift>
    {10, 9, 7,22, 4}, // gfdsa
    {23,21, 8,26,20}, // trewq
    {34,33,32,31,30}, // 54321
    {35,36,37,38,39}, // 67890
    {28,24,12,18,19}, // yuiop
    {11,13,14,15,40}, // hjkl<enter>
    { 5,17,16,1,44},  // bnm. <space>
};

static char tapename[64]={0};
static const int kBuf[]={13,25,19,25,19,40}; //,21,40}; // LOAD "" (J shift p shift p, R ENTER) 
static const int tBuf[]={2,0,2,0,2,2};//200!,2,2};
static int kcount=0;
static int timeout=100;



unsigned int in(int h, int l)
{

  int ts=0;    /* additional cycles*256 */
  int tapezeromask=0x80;  /* = 0x80 if no tape noise (?) */

  if(!(l&4)) l=0xfb;
  if(!(l&1)) l=0xfe;

  switch(l)
  {
  //case 0xfb:
  //  return(printer_inout(0,0));
    
  case 0xfe:
    /* also disables hsync/vsync if nmi off
     * (yes, vsync requires nmi off too, Flight Simulation confirms this)
     */
    if(!nmigen)
    {
      hsyncgen=0;

      /* if vsync was on before, record position */
      if(!vsync)
        vsync_raise();
      vsync=1;

    }

    switch(h)
    {
        case 0xfe:  return(ts|(keyboard[0]^tapezeromask));
        case 0xfd:  return(ts|(keyboard[1]^tapezeromask));
        case 0xfb:  return(ts|(keyboard[2]^tapezeromask));
        case 0xf7:  return(ts|(keyboard[3]^tapezeromask));
        case 0xef:  return(ts|(keyboard[4]^tapezeromask));
        case 0xdf:  return(ts|(keyboard[5]^tapezeromask));
        case 0xbf:  return(ts|(keyboard[6]^tapezeromask));
        case 0x7f:  return(ts|(keyboard[7]^tapezeromask));
        default:
          {
            int i,mask,retval=0xff;
          
            /* some games (e.g. ZX Galaxians) do smart-arse things
            * like zero more than one bit. What we have to do to
            * support this is AND together any for which the corresponding
            * bit is zero.
            */
            for(i=0,mask=1;i<8;i++,mask<<=1)
              if(!(h&mask))
                retval&=keyboard[i];
            return(ts|(retval^tapezeromask));
          }
    }
    break;
  }

  return(ts|255);
}

unsigned int out(int h,int l,int a)

{
  /* either in from fe or out to ff takes one extra cycle;
   * experimentation strongly suggests not only that out to
   * ff takes one extra, but that *all* outs do.
   */
  int ts=1;  /* additional cycles */



  /* the examples in the manual (using DF/0F) and the
   * documented ports (CF/0F) don't match, so decode is
   * important for that.
   */
  if(!(l&0xf0))   /* not sure how many needed, so assume all 4 */
    l=0x0f;
  else
    if(!(l&0x20))   /* bit 5 low is common to DF and CF */
      l=0xdf;


  if(!(l&4)) l=0xfb;
  if(!(l&2)) l=0xfd;
  if(!(l&1)) l=0xfe;


  switch(l)
  {
    case 0x0f:    /* Zon X data */   
      WrData8910(&ay,a);
      break;
    case 0xdf:    /* Zon X reg. select */  
      WrCtrl8910(&ay,(a &0x0F));
      break;
  
    case 0xfb:
      return(ts/*|printer_inout(1,a)*/);
    case 0xfd:
      nmigen=0;
      if(vsync)
        vsync_lower();
      vsync=0;
      hsyncgen=1;
      break;
    case 0xfe:
      if(!zx80)
      {
        nmigen=1;
        break;
      }
      /* falls through, if zx80 */
    case 0xff:  /* XXX should *any* out turn off vsync? */
      /* fill screen gap since last raising of vsync */
      if(vsync)
        vsync_lower();
      vsync=0;
      hsyncgen=1;
      break;
  }

  return(ts);
}



void sighandler(int a)
{
  signal_int_flag=1;
}

void frame_pause(void)
{
  signal_int_flag=0;

  if(interrupted<2)
    interrupted=1;
}

void do_interrupt()
{
  /* being careful here not to screw up any pending reset... */
  if(interrupted==1)
    interrupted=0;
}

static int ik;
static int ihk;
static int iusbhk; // USB keyboard key
static int prevhk; // previous keyboard key

void z81_Input(int bClick) {
  ik  = emu_GetPad();
  ihk = emu_ReadI2CKeyboard();
}

void bitbufBlit(unsigned char * buf)
{
  emu_DrawVsync();  
  memset( XBuf, 1, WIDTH*8 ); 
  buf = buf + (ZX_VID_MARGIN*(ZX_VID_FULLWIDTH/8));
  int y,x,i;
  byte d;
  for(y=0;y<192;y++)
  {
    byte * src = buf + 4;
    for(x=0;x<32;x++)
    {
      byte * dst=&XBuf[(x<<3)+BORDER];
      d = *src++;
      for (i=0;i<8;i++)
      {
        if ( d & 128 )
        {
          *dst++=0;
        }
        else
        {
          *dst++=1;
        }
        d <<= 1;
      }       
    }
    emu_DrawLinePal16(&XBuf[0], WIDTH, HEIGHT, y);   
    buf += (ZX_VID_FULLWIDTH/8);
  }
}

#define INV_KEY 0

const short keyboardAsciiConv[] = // Ascii to Spectrum keys
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
/* 0x09 */ INV_KEY,   // tab
/* 0x0A */ INV_KEY,
/* 0x0B */ INV_KEY,
/* 0x0C */ INV_KEY,
/* 0x0D */ 40,        // enter
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
/* 0x1B */ INV_KEY,  // esc
/* 0x1C */ INV_KEY,
/* 0x1D */ INV_KEY,
/* 0x1E */ INV_KEY,
/* 0x1F */ INV_KEY,
/* 0x20 */ 44,       // space
/* 0x21 */ 30+64,    // ! exclamation mark
/* 0x22 */ 19+64,    // " double quote   
/* 0x23 */ INV_KEY,  // # dies
/* 0x24 */ 24+64,    // $ dollar
/* 0x25 */ INV_KEY,  // % percent
/* 0x26 */ INV_KEY,  // & ampercent
/* 0x27 */ INV_KEY,  // ' singlequote
/* 0x28 */ 12+64,    // ( bracket left
/* 0x29 */ 18+64,    // ) bracket right
/* 0x2A */ 5+64,     // * mult
/* 0x2B */ 14+64,    // + plus
/* 0x2C */ 1+64,     // , comma
/* 0x2D */ 13+64,    // - minus
/* 0x2E */ 1,        // . period
/* 0x2F */ 25+64,    // / slash
/* 0x30 */ 39,       // 0
/* 0x31 */ 30,       // 1
/* 0x32 */ 31,       // 2 
/* 0x33 */ 32,       // 3 
/* 0x34 */ 33,       // 4
/* 0x35 */ 34,       // 5
/* 0x36 */ 35,       // 6
/* 0x37 */ 36,       // 7
/* 0x38 */ 37,       // 8
/* 0x39 */ 38,       // 9
/* 0x3A */ 29+64,    // : colon
/* 0x3B */ 27+64,    // ; semi colon
/* 0x3C */ 17+64,    // <
/* 0x3D */ 15+64,    // = equal
/* 0x3E */ 16+64,    // >
/* 0x3F */ 6+64,     // ?
/* 0x40 */ INV_KEY,    // @
/* 0x41 */ 4,        // A
/* 0x42 */ 5,        // B
/* 0x43 */ 6,        // C
/* 0x44 */ 7,        // D
/* 0x45 */ 8,        // E
/* 0x46 */ 9,        // F
/* 0x47 */ 10,       // G
/* 0x48 */ 11,       // H
/* 0x49 */ 12,       // I
/* 0x4A */ 13,       // J
/* 0x4B */ 14,       // K
/* 0x4C */ 15,       // L
/* 0x4D */ 16,       // M
/* 0x4E */ 17,       // N
/* 0x4F */ 18,       // O
/* 0x50 */ 19,       // P
/* 0x51 */ 20,       // Q
/* 0x52 */ 21,       // R
/* 0x53 */ 22,       // S
/* 0x54 */ 23,       // T
/* 0x55 */ 24,       // U
/* 0x56 */ 25,       // V
/* 0x57 */ 26,       // W
/* 0x58 */ 27,       // X
/* 0x59 */ 28,       // Y
/* 0x5A */ 29,       // Z
/* 0x5B */ INV_KEY,  // square bracket open
/* 0x5C */ INV_KEY,  // baclslach
/* 0x5D */ INV_KEY,  // square braquet close
/* 0x5E */ INV_KEY,  // ^ circonflex
/* 0x5F */ INV_KEY,  // _ undescore
/* 0x60 */ INV_KEY,  // `backquote
/* 0x61 */ 4,        // a
/* 0x62 */ 5,        // b
/* 0x63 */ 6,        // c
/* 0x64 */ 7,        // d
/* 0x65 */ 8,        // e
/* 0x66 */ 9,        // f
/* 0x67 */ 10,       // g
/* 0x68 */ 11,       // h
/* 0x69 */ 12,       // i
/* 0x6A */ 13,       // j
/* 0x6B */ 14,       // k
/* 0x6C */ 15,       // l
/* 0x6D */ 16,       // m
/* 0x6E */ 17,       // n
/* 0x6F */ 18,       // o
/* 0x70 */ 19,       // p
/* 0x71 */ 20,       // q
/* 0x72 */ 21,       // r
/* 0x73 */ 22,       // s
/* 0x74 */ 23,       // t
/* 0x75 */ 24,       // u
/* 0x76 */ 25,       // v
/* 0x77 */ 26,       // w
/* 0x78 */ 27,       // x
/* 0x79 */ 28,       // y
/* 0x7A */ 29,       // z
/* 0x7B */ INV_KEY,  // curly bracket open
/* 0x7C */ INV_KEY,  // or
/* 0x7D */ INV_KEY,  // curly bracket close  
/* 0x7E */ INV_KEY,  // tilt
/* 0x7F */ 39+64,   // backspace

/* 0xC0 */ INV_KEY,
/* 0xC1 */ INV_KEY,
/* 0xC2 */ INV_KEY,   // F1
/* 0xC3 */ INV_KEY,   // F2
/* 0xC4 */ INV_KEY,   // F3
/* 0xC5 */ INV_KEY,   // F4
/* 0xC6 */ INV_KEY,   // F5
/* 0xC7 */ INV_KEY,   // F6
/* 0xC8 */ INV_KEY,   // F7
/* 0xC9 */ INV_KEY,   // F8
/* 0xCA */ INV_KEY,   // F9
/* 0xCB */ INV_KEY,   // F10
/* 0xCC */ INV_KEY,
/* 0xCD */ INV_KEY,
/* 0xCE */ INV_KEY,
/* 0xCF */ INV_KEY,
/* 0xD0 */ INV_KEY,
/* 0xD1 */ INV_KEY,
/* 0xD2 */ INV_KEY,
/* 0xD3 */ INV_KEY,
/* 0xD4 */ INV_KEY,   // DEL
/* 0xD5 */ INV_KEY,
/* 0xD6 */ INV_KEY,
/* 0xD7 */ 37+64,     // U
/* 0xD8 */ 34+64,     // L
/* 0xD9 */ 35+64,     // R
/* 0xDA */ 36+64,     // D
/* 0xDB */ INV_KEY,
/* 0xDC */ INV_KEY,
/* 0xDD */ INV_KEY,
/* 0xDE */ INV_KEY,
/* 0xDF */ INV_KEY
};

static void updateKeyboard (int hk)
{
  memset(keyboard, 0xff, sizeof(keyboard));    
  {
    int shift = hk;
    if (hk >=64) hk -= 64;    
    // scan all possibilities
    for (int j=0;j<8;j++) {
      for(int i=0;i<5;i++){
        if ( /*(k == map_qw[j][i]) ||*/ (hk == map_qw[j][i]) ) {
            keyboard[j] &= ~ (1<<(4-i));
        }   
      }  
    } 
    if (shift >=64) keyboard[0] &= ~ (1<<0);  // SHift      
  } 
}

/*
static void handleKeyBuf(void)
{
  if (timeout) {
    timeout--;
    if (timeout==0) {
      memset(keyboard, 0xff, sizeof(keyboard)); 
      emu_printf("key up");
    }
  }
  else {
    if (!(kcount == (sizeof(kBuf)/sizeof(int)))) {
      emu_printf("key dw");     
      timeout=tBuf[kcount];
      int k=kBuf[kcount++];
      // scan all possibilities
      for (int j=0;j<8;j++) {
        for(int i=0;i<5;i++){
          if ( (k == map_qw[j][i]) ) {
              keyboard[j] &= ~ (1<<(4-i));
          }   
        }  
      } 
      if (timeout == 0) {
        timeout=tBuf[kcount];
        int k=kBuf[kcount++];
        // scan all possibilities
        for (int j=0;j<8;j++) {
          for(int i=0;i<5;i++){
            if ( (k == map_qw[j][i]) ) {
                keyboard[j] &= ~ (1<<(4-i));
            }   
          }  
        }         
      }      
    }       
  }
}
*/


/* despite the name, this also works for the ZX80 :-) */
void reset81()
{
  interrupted=2;  /* will cause a reset */
  memset(zxmemory+0x4000,0,0xc000);  
}

void load_p(int a)
{
  emu_printf("loading...");
/*
  int got_ascii_already=0;
  if(zx80) {    
  }
  else
  {
    if(a>=32768)  
    {
      got_ascii_already=1;
      emu_printf("got ascii");
    } 
    if(!got_ascii_already)
    {
    }
  }
*/    
  emu_printf(tapename);
  int size = emu_FileSize(tapename);
  int f = emu_FileOpen(tapename, "r+b");  
  if ( !f ) {
    /* the partial snap will crash without a file, so reset */
    if(autoload)
      reset81(),autoload=0;
    return;
  }

  autoload=0;
  emu_FileRead(zxmemory + (zx80?0x4000:0x4009), size, f);
  emu_FileClose(f);

  if(zx80)
    store(0x400b,fetch(0x400b)+1);         
}

void save_p(int a)
{
  
}



void zx81hacks()
{
  /* patch save routine */
  zxmemory[0x2fc]=0xed; zxmemory[0x2fd]=0xfd;
  zxmemory[0x2fe]=0xc3; zxmemory[0x2ff]=0x07; zxmemory[0x300]=0x02;

  /* patch load routine */
  zxmemory[0x347]=0xeb;
  zxmemory[0x348]=0xed; zxmemory[0x349]=0xfc;
  zxmemory[0x34a]=0xc3; zxmemory[0x34b]=0x07; zxmemory[0x34c]=0x02;
}

void zx80hacks()
{
  /* patch save routine */
  zxmemory[0x1b6]=0xed; zxmemory[0x1b7]=0xfd;
  zxmemory[0x1b8]=0xc3; zxmemory[0x1b9]=0x83; zxmemory[0x1ba]=0x02;

  /* patch load routine */
  zxmemory[0x206]=0xed; zxmemory[0x207]=0xfc;
  zxmemory[0x208]=0xc3; zxmemory[0x209]=0x83; zxmemory[0x20a]=0x02;
}

static void initmem()
{
  int f;
  int count;

  if(zx80)
  {
    memset(zxmemory+0x1000,0,0xf000);
  }
  else
  {
    memset(zxmemory+0x2000,0,0xe000);
  }


  /* ROM setup */
  count=0;
  for(f=0;f<16;f++)
  {
    memattr[f]=memattr[32+f]=0;
    memptr[f]=memptr[32+f]=zxmemory+1024*count;
    count++;
    if(count>=(zx80?4:8)) count=0;
  }

  /* RAM setup */
  if(unexpanded)
    ramsize=1;
  count=0;
  for(f=16;f<32;f++)
  {
    memattr[f]=memattr[32+f]=1;
    memptr[f]=memptr[32+f]=zxmemory+1024*(16+count);
    count++;
    if(count>=ramsize) count=0;
  }


/* z81's ROM and RAM initialisation code is OK for <= 16K RAM but beyond
 * that it requires a little tweaking.
 * 
 * The following diagram shows the ZX81 + 8K ROM. The ZX80 version is
 * the same except that each 8K ROM region will contain two copies of
 * the 4K ROM.
 * 
 * RAM less than 16K is mirrored throughout the 16K region.
 * 
 * The ROM will only detect up to 8000h when setting RAMTOP, therefore
 * having more than 16K RAM will require RAMTOP to be set by the user
 * (or user program) to either 49152 for 32K or 65535 for 48/56K.
 * 
 *           1K to 16K       32K           48K           56K      Extra Info.
 * 
 *  65535  +----------+  +----------+  +----------+  +----------+ 
 * (FFFFh) | 16K RAM  |  | 16K RAM  |  | 16K RAM  |  | 16K RAM  | DFILE can be
 *         | mirrored |  | mirrored |  |          |  |          | wholly here.
 *         |          |  |          |  |          |  |          | 
 *         |          |  |          |  |          |  |          | BASIC variables
 *         |          |  |          |  |          |  |          | can go here.
 *  49152  +----------+  +----------+  +----------+  +----------+ 
 * (C000h) | 8K ROM   |  | 16K RAM  |  | 16K RAM  |  | 16K RAM  | BASIC program
 *         | mirrored |  |          |  |          |  |          | is restricted
 *  40960  +----------+  |          |  |          |  |          | to here.
 * (A000h) | 8K ROM   |  |          |  |          |  |          | 
 *         | mirrored |  |          |  |          |  |          | 
 *  32768  +----------+  +----------+  +----------+  +----------+ 
 * (8000h) | 16K RAM  |  | 16K RAM  |  | 16K RAM  |  | 16K RAM  | No machine code
 *         |          |  |          |  |          |  |          | beyond here.
 *         |          |  |          |  |          |  |          | 
 *         |          |  |          |  |          |  |          | DFILE can be
 *         |          |  |          |  |          |  |          | wholly here.
 *  16384  +----------+  +----------+  +----------+  +----------+ 
 * (4000h) | 8K ROM   |  | 8K ROM   |  | 8K ROM   |  | 8K RAM   | 
 *         | mirrored |  | mirrored |  | mirrored |  |          | 
 *   8192  +----------+  +----------+  +----------+  +----------+ 
 * (2000h) | 8K ROM   |  | 8K ROM   |  | 8K ROM   |  | 8K ROM   | 
 *         |          |  |          |  |          |  |          | 
 *      0  +----------+  +----------+  +----------+  +----------+ 
 */

  switch(ramsize)
  {
    case 56:
      for(f=8;f<16;f++)
      {
        memattr[f]=1;         /* It's now writable */
        memptr[f]=zxmemory+1024*f;
      }
    case 48:
      for(f=48;f<64;f++)
      {
        memattr[f]=1;
        memptr[f]=zxmemory+1024*f;
      }
    case 32:
      for(f=32;f<48;f++)
      {
        memattr[f]=1;
        memptr[f]=zxmemory+1024*f;
      }
      break;
  }

  if(zx80)
    zx80hacks();
  else
    zx81hacks();
}


void emu_KeyboardOnDown(int keymodifer, int key) {
  if (key <= 0x7f) iusbhk = key;
  else if (key == KBD_KEY_UP) iusbhk = 0xD7;  
  else if (key == KBD_KEY_LEFT) iusbhk = 0xD8;  
  else if (key == KBD_KEY_RIGHT) iusbhk = 0xD9;  
  else if (key == KBD_KEY_DOWN) iusbhk = 0xDA;  
  else if (key == KBD_KEY_BS) iusbhk = 0x7F;  
  else
    iusbhk = 0;
}

void emu_KeyboardOnUp(int keymodifer, int key) {
  iusbhk = 0;
}


void z81_Init(void) 
{
#if HAS_SND
  emu_sndInit(); 
#endif 
  
  /* Set up the palette */
  int J;
  for(J=0;J<2;J++)
    emu_SetPaletteEntry(Palette[J].R,Palette[J].G,Palette[J].B, J);


  emu_printf("Allocating RAM");
  if (zxmemory == 0) zxmemory = &memory[0]; //emu_Malloc(MEMORYRAM_SIZE);
  
  Reset8910(&ay,3500000,0);
  
  /* load rom with ghosting at 0x2000 */
  int siz=(zx80?4096:8192);  
  if(zx80)
  {
    memcpy( zxmemory + 0x0000, zx80rom, siz );    
  }
  else 
  {
    memcpy( zxmemory + 0x0000, zx81rom, siz );   
  }
  memcpy(zxmemory+siz,zxmemory,siz);
  if(zx80)
    memcpy(zxmemory+siz*2,zxmemory,siz*2);
    
  initmem();
 
  /* reset the keyboard state */
  memset( keyboard, 255, sizeof( keyboard ) );  

  ResetZ80();
 }


void z81_Step(void)
{
  ExecZ80();
  sighandler(0);

  int k  = ik;
  int hk = ihk;
  if (iusbhk) hk = keyboardAsciiConv[iusbhk];

  updateKeyboard(hk);

  Loop8910(&ay,20);
}

static int endsWith(const char * s, const char * suffix)
{
  int retval = 0;
  int len = strlen(s);
  int slen = strlen(suffix);
  if (len > slen ) {
    if (!strcmp(&s[len-slen], suffix)) {
      retval = 1;
    }
  }
   return (retval);  
}

void z81_Start(char * filename)
{
  char c;
  strncpy(tapename,filename,64);
  int f = emu_FileOpen(tapename, "r+b");
  if ( f ) {
    int fsize = emu_FileRead(&c, 1, f);
    if ( fsize == 0) { 
      autoload = 0;
      emu_printf("no autoload");
    }
    emu_FileClose(f);
  }

  //emu_setKeymap(1); 
  
  if ( (endsWith(filename, ".80")) || (endsWith(filename, ".o")) || (endsWith(filename, ".O")))  {
    zx80 = 1;
    ramsize = 48;
    //emu_setKeymap(0);    
  }
  else if (endsWith(filename, ".56") ) {
    ramsize = 56;
  }
}
