#include "Z80.h"
#include "spectrum.rom.h"
#include "emuapi.h"
#include "zx_filetyp_z80.h"

#include "AY8910.h"


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

static byte Z80_RAM[0xC000];                    // 48k RAM
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



static void InitKeyboard(void){
  memset(key_ram, 0xff, sizeof(key_ram));   
}

static void UpdateKeyboard (void)
{
  int nb_keys=0;
  int k = emu_GetPad();
  int hk = emu_ReadI2CKeyboard();  
  if ( (k == 0) && (hk == 0) )  {
    memset(key_ram, 0xff, sizeof(key_ram));    
  }
  else {
    // scan all possibilities
    for (int j=0;j<8;j++) {
      for(int i=0;i<5;i++){
        if ( (k == map_qw[j][i]) || (hk == map_qw[j][i]) ) {
            key_ram[j] &= ~ (1<<(4-i));
            nb_keys++;
        }   
      }  
    }    
  } 
}


#define MAX_Z80SIZE 49152


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

void spec_Start(char * filename) {
  memset(Z80_RAM, 0, 0xC000);
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
  memset(Z80_RAM, 0, sizeof(Z80_RAM));

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

  int k=emu_ReadKeys();
  
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


  UpdateKeyboard();
    
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
