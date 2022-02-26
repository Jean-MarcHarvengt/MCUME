#include "Z80.h"
#include "spectrum.rom.h"
#include "emuapi.h"
#include "zx_filetyp_z80.h"

#include "AY8910.h"

#define RAM_SIZE 0xC000 //0x20000 // 0xC000

#define WIDTH  320
#define HEIGHT 192

#define CYCLES_PER_FRAME 69888 //3500000/50

typedef struct {
  int port_ff;      // 0xff = emulate the port,  0x00 alwais 0xFF
  int ts_lebo;      // left border t states
  int ts_grap;      // graphic zone t states
  int ts_ribo;      // right border t states
  int ts_hore;      // horizontal retrace t states
  int ts_line;      // to speed the calc, the sum of 4 abobe
  int line_poin;    // lines in retraze post interrup
  int line_upbo;    // lines of upper border
  int line_grap;    // lines of graphic zone = 192
  int line_bobo;    // lines of bottom border
  int line_retr;    // lines of the retrace
  /*
  int TSTATES_PER_LINE;
  int TOP_BORDER_LINES;
  int SCANLINES;
  int BOTTOM_BORDER_LINES;
  int tstate_border_left;
  int tstate_graphic_zone;
  int tstate_border_right;
  int hw_model;
  int int_type;
  int videopage;
  int BANKM;
  int BANK678;
  */
} HWOptions ;

static HWOptions hwopt = { 0xFF, 24, 128, 24, 48, 224, 16, 48, 192, 48, 8 };
//224, 64, 192, 56, 24, 128, 72};

struct { unsigned char R,G,B; } Palette[16] = {
  {   0,   0,   0},
  {   0,   0, 205},
  { 205,   0,   0},
  { 205,   0, 205},
  {   0, 205,   0},
  {   0, 205, 205},
  { 205, 205,   0},
  { 212, 212, 212},
  {   0,   0,   0},
  {   0,   0, 255},
  { 255,   0,   0},
  { 255,   0, 255},
  {   0, 255,   0},
  {   0, 255, 255},
  { 255, 255,   0},
  { 255, 255, 255}
};

const byte map_qw[8][5] = {
    {25, 6,27,29,224}, // vcxz<caps shift=Lshift>
    {10, 9, 7,22, 4}, // gfdsa
    {23,21, 8,26,20}, // trewq
    {34,33,32,31,30}, // 54321
    {35,36,37,38,39}, // 67890
    {28,24,12,18,19}, // yuiop
    {11,13,14,15,40}, // hjkl<enter>
    { 5,17,16,225,44}, // bnm <symbshift=RSHift> <space>
};


static byte Z80_RAM[RAM_SIZE];                    // 48k RAM
static Z80 myCPU;
static byte * volatile VRAM=Z80_RAM;            // What will be displayed. Generally ZX VRAM, can be changed for alt screens.

//extern const byte rom_zx48_rom[];        // 16k ROM
static byte key_ram[8]={
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; // Keyboard buffer
byte out_ram;                            // Output (fe port)
static byte kempston_ram;                       // Kempston-Joystick Buffer

static int v_border=0;
static int h_border=32;
static int bordercolor=0;
static byte * XBuf=0; 

static int lastaudio=CYCLES_PER_FRAME;
static byte buzzer_val;

#define INV_KEY 0

const int16_t keyboardAsciiConv[] = // Ascii to Spectrum keys
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
/* 0x0A */ 40,        // enter
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
/* 0x1B */ INV_KEY,  // esc
/* 0x1C */ INV_KEY,
/* 0x1D */ INV_KEY,
/* 0x1E */ INV_KEY,
/* 0x1F */ INV_KEY,
/* 0x20 */ 44,       // space
/* 0x21 */ 30+64,    // ! exclamation mark
/* 0x22 */ 19+64,    // " double quote   
/* 0x23 */ 32+64,    // # dies
/* 0x24 */ 33+64,    // $ dollar
/* 0x25 */ 34+64,    // % percent
/* 0x26 */ 35+64,    // & ampercent
/* 0x27 */ INV_KEY,  // ' singlequote
/* 0x28 */ 37+64,    // ( bracket left
/* 0x29 */ 38+64,    // ) bracket right
/* 0x2A */ 5+64,     // * mult
/* 0x2B */ 14+64,    // + plus
/* 0x2C */ 17+64,    // , comma
/* 0x2D */ 13+64,    // - minus
/* 0x2E */ 16+64,    // . period
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
/* 0x3B */ 18+64,    // ; semi colon
/* 0x3C */ 21+64,    // <
/* 0x3D */ 15+64,    // = equal
/* 0x3E */ 23+64,    // >
/* 0x3F */ 6+64,     // ?
/* 0x40 */ 31+64,    // @
/* 0x41 */ 4+128,    // A
/* 0x42 */ 5+128,    // B
/* 0x43 */ 6+128,    // C
/* 0x44 */ 7+128,    // D
/* 0x45 */ 8+128,    // E
/* 0x46 */ 9+128,    // F
/* 0x47 */ 10+128,   // G
/* 0x48 */ 11+128,   // H
/* 0x49 */ 12+128,   // I
/* 0x4A */ 13+128,   // J
/* 0x4B */ 14+128,   // K
/* 0x4C */ 15+128,   // L
/* 0x4D */ 16+128,   // M
/* 0x4E */ 17+128,   // N
/* 0x4F */ 18+128,   // O
/* 0x50 */ 19+128,   // P
/* 0x51 */ 20+128,   // Q
/* 0x52 */ 21+128,   // R
/* 0x53 */ 22+128,   // S
/* 0x54 */ 23+128,   // T
/* 0x55 */ 24+128,   // U
/* 0x56 */ 25+128,   // V
/* 0x57 */ 26+128,   // W
/* 0x58 */ 27+128,   // X
/* 0x59 */ 28+128,   // Y
/* 0x5A */ 29+128,   // Z
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
/* 0x7F */ 39+128,   // backspace

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
/* 0xD7 */ 37+128,    // U
/* 0xD8 */ 34+128,    // L
/* 0xD9 */ 35+128,    // R
/* 0xDA */ 36+128,    // D
/* 0xDB */ INV_KEY,
/* 0xDC */ INV_KEY,
/* 0xDD */ INV_KEY,
/* 0xDE */ INV_KEY,
/* 0xDF */ INV_KEY
};


void displayscanline(int y, int f_flash)
{
  int x, row, col, dir_p, dir_a, pixeles, tinta, papel, atributos;

  row = y + v_border;    // 4 & 32 = graphical screen offset
  col = 0;              // 32+256+32=320  4+192+4=200  (res=320x200)

  for (x = 0; x < h_border; x++) {
    XBuf[col++] = bordercolor;
  }

  dir_p = ((y & 0xC0) << 5) + ((y & 0x07) << 8) + ((y & 0x38) << 2);
  dir_a = 0x1800 + (32 * (y >> 3));
  
  for (x = 0; x < 32; x++)
  {
    pixeles=  VRAM[dir_p++];
    atributos=VRAM[dir_a++];
    
    if (((atributos & 0x80) == 0) || (f_flash == 0))
    {
      tinta = (atributos & 0x07) + ((atributos & 0x40) >> 3);
      papel = (atributos & 0x78) >> 3;
    }
    else
    {
      papel = (atributos & 0x07) + ((atributos & 0x40) >> 3);
      tinta = (atributos & 0x78) >> 3;
    }
    XBuf[col++] = ((pixeles & 0x80) ? tinta : papel);
    XBuf[col++] = ((pixeles & 0x40) ? tinta : papel);
    XBuf[col++] = ((pixeles & 0x20) ? tinta : papel);
    XBuf[col++] = ((pixeles & 0x10) ? tinta : papel);
    XBuf[col++] = ((pixeles & 0x08) ? tinta : papel);
    XBuf[col++] = ((pixeles & 0x04) ? tinta : papel);
    XBuf[col++] = ((pixeles & 0x02) ? tinta : papel);
    XBuf[col++] = ((pixeles & 0x01) ? tinta : papel);
  }

  for (x = 0; x < h_border; x++) {
    XBuf[col++] = bordercolor;
  }
  
  emu_DrawLine(XBuf, WIDTH, HEIGHT, y);
}

static void displayScreen(void) {
  int y;
  static int f_flash = 1, f_flash2 = 0;
  f_flash2 = (f_flash2++) % 32;
  if (f_flash2 < 16)
    f_flash = 1;
  else
    f_flash = 0;
  
  for (y = 0; y < HEIGHT; y++)
    displayscanline (y, f_flash);

  emu_DrawVsync();    
}


static int ik;     // joypad key
static int ihk;    // I2C keyboard key
static int iusbhk; // USB keyboard key
static int prevhk; // previous keyboard key

static void InitKeyboard(void){
  memset(key_ram, 0xff, sizeof(key_ram));   
}

static void UpdateKeyboard (int asckey)
{
  int hk = keyboardAsciiConv[asckey]; 
  memset(key_ram, 0xff, sizeof(key_ram));    
  {
    //if (k & MASK_KEY_USER1) hk = 39;
    int shift = hk;
    if (hk >=128) hk -= 128;
    else if (hk >=64) hk -= 64;    
    // scan all possibilities
    for (int j=0;j<8;j++) {
      for(int i=0;i<5;i++){
        if ( /*(k == map_qw[j][i]) ||*/ (hk == map_qw[j][i]) ) {
            key_ram[j] &= ~ (1<<(4-i));
        }   
      }  
    } 
    if (shift >=128) key_ram[0] &= ~ (1<<0);  // SHift 
    else if (shift >=64) key_ram[7] &= ~ (1<<1);  // SHift symboles     
  } 
}


#define MAX_Z80SIZE RAM_SIZE


int endsWith(const char * s, const char * suffix)
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

void emu_KeyboardOnDown(int keymodifer, int key) {
  int keyCode = -1; //INV_KEY;
  if ((key >=0xc0) && (key <=0xdf)) {
    keyCode = ((key-0xc0) & 0x1f) + 0x80;
  }
  else {
    keyCode = key & 0x7f;
  }
  
  //emu_printh(key);
  //emu_printh(keyCode);
  
  if (keyCode != -1) {
    iusbhk = keyCode;
  }
}

void emu_KeyboardOnUp(int keymodifer, int key) {
  iusbhk = 0;
}

void spec_Start(char * filename) {
  memset(Z80_RAM, 0, RAM_SIZE);
  if ( (endsWith(filename, "SNA")) || (endsWith(filename, "sna")) ) {
    ZX_ReadFromFlash_SNA(&myCPU, filename); 
  } 
  else if ( (endsWith(filename, "Z80")) || (endsWith(filename, "z80")) ) {
    unsigned char * game = emu_Malloc(MAX_Z80SIZE);
    int size = emu_LoadFile(filename, game, MAX_Z80SIZE);  
    ZX_ReadFromFlash_Z80(&myCPU, game,size); 
    emu_Free(game);  
  }
#if HAS_SND
  emu_sndInit(); 
#endif  
}

void spec_Input(int click) {
  ik  = emu_GetPad();
  ihk = emu_ReadI2CKeyboard();   
}


static AY8910 ay;

void spec_Init(void) {
  int J;
  /* Set up the palette */
  for(J=0;J<16;J++)
    emu_SetPaletteEntry(Palette[J].R,Palette[J].G,Palette[J].B, J);
  
  InitKeyboard();
 
  Reset8910(&ay,3500000,0);

  
  if (XBuf == 0) XBuf = (byte *)emu_Malloc(WIDTH);
  VRAM = Z80_RAM;
  //memset(Z80_RAM, 0, RAM_SIZE);

  ResetZ80(&myCPU, CYCLES_PER_FRAME);
#if ALT_Z80CORE  
  myCPU.RAM = Z80_RAM; 
  Z80FlagTables();
#endif      
}



void spec_Step(void) {
// 32+256+32
//#define NBLINES (48+192+56+16)

  // Now run the emulator for all the real screen (192 lines)
  /*
  int scanl;
  for (scanl = 0; scanl < NBLINES; scanl++) {
    int ref=0;
    emu_resetus();
    ExecZ80(&myCPU,hwopt.ts_line);
    while (emu_us() < (20000/NBLINES)) { 
    }
  }
  */

  ExecZ80(&myCPU,CYCLES_PER_FRAME); // 3.5MHz ticks for 6 lines @ 30 kHz = 700 cycles
#if ALT_Z80CORE
#else
  IntZ80(&myCPU,INT_IRQ); // must be called every 20ms
#endif
  displayScreen();

  int k  = ik;
  int hk = ihk;
  if (iusbhk) hk = iusbhk;

  kempston_ram = 0x00;
  if (k & MASK_JOY2_BTN)
          kempston_ram |= 0x10; //Fire
  if (k & MASK_JOY2_UP)
          kempston_ram |= 0x8; //Up
  if (k & MASK_JOY2_DOWN)
          kempston_ram |= 0x4; //Down
  if (k & MASK_JOY2_RIGHT)
          kempston_ram |= 0x2; //Right
  if (k & MASK_JOY2_LEFT)
          kempston_ram |= 0x1; //Left

  UpdateKeyboard(hk);
    
  Loop8910(&ay,20);
}



#define BASERAM 0x4000


void WrZ80(register word Addr,register byte Value)
{
  if (Addr >= BASERAM)
    Z80_RAM[Addr-BASERAM]=Value;
}

byte RdZ80(register word Addr)
{
  if (Addr<BASERAM) 
    return rom_zx48_rom[Addr];
  else
    return Z80_RAM[Addr-BASERAM];
}



void buzz(int val, int currentTstates)
{
  //if (val==0) val = -1;
  //if (buzzer_val!=val)
  //if (val)   
  {  
    int sound_size = (currentTstates-lastaudio);
    if (sound_size < 0) sound_size += CYCLES_PER_FRAME;    
#if HAS_SND
    emu_sndPlayBuzz(sound_size,buzzer_val);  
#endif    
    //if (val)
    //  buzzer_val = 0;     
    //else
    //  buzzer_val = 1;
    buzzer_val = val;          
  } 
  lastaudio = currentTstates;
}

void OutZ80(register word Port,register byte Value)
{
  if ((Port & 0xC002) == 0xC000) {
    WrCtrl8910(&ay,(Value &0x0F));
  }  
  else if ((Port & 0xC002) == 0x8000) {
    WrData8910(&ay,Value);
  }    
  else if (!(Port & 0x01)) {
    bordercolor = (Value & 0x07);
    byte mic = (Value & 0x08);
    byte ear = (Value & 0x10);
    buzz(((/*mic|*/ear)?1:0), CYCLES_PER_FRAME-myCPU.ICount);
  }
  else if((Port&0xFF)==0xFE) {
    out_ram=Value; // update it
  }
}

byte InZ80(register word port)
{  
    if (port == 0xFFFD) {
      return (RdData8910(&ay));
    }
  
    if((port&0xFF)==0x1F) {
        // kempston RAM
        return kempston_ram;
    }

    if ((port&0xFF)==0xFE) {
        switch(port>>8) {
            case 0xFE : return key_ram[0]; break;
            case 0xFD : return key_ram[1]; break;
            case 0xFB : return key_ram[2]; break;
            case 0xF7 : return key_ram[3]; break;
            case 0xEF : return key_ram[4]; break;
            case 0xDF : return key_ram[5]; break;
            case 0xBF : return key_ram[6]; break;
            case 0x7F : return key_ram[7]; break;
        }
    } 

    if ((port & 0xFF) == 0xFF) {  
      if (hwopt.port_ff == 0xFF) {
       return 0xFF;        
      }
      else {
       //code = 1;
       //if (code == 0xFF) code = 0x00;
       return 1;
      }
    }    
    return 0xFF;
}


void PatchZ80(register Z80 *R)
{
  // nothing to do
}

/*
word LoopZ80(register Z80 *R)
{
  // no interrupt triggered
  return INT_NONE;
}
*/
