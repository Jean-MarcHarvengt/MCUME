#define PROGMEM

#include "atari800.h"
#include <string.h>
#include "memory.h"
#include "cpu.h"
#include "atari.h"
#include "akey.h"
#include "pokey.h"
//#include "romatariosa.h"
#include "romatariosb.h"
//#include "romatarixl.h"
#include "antic.h"
#include "gtia.h"
#include "pia.h"
#include "colours.h"
#include "emuapi.h"
#ifdef HAS_SND
#include "pokeysnd.h"
#endif
#include "kbd.h"

// Controllers
typedef struct
{ 
  // All values are 1 or 0, or perhaps not...
  int left;
  int right;
  int up;
  int down;
  int trig;

} CONTROLLER;


// global variables
unsigned char mem[MEMORY_SIZE];
unsigned char * memory=mem;
int tv_mode = TV_PAL;
UBYTE INPUT_key_consol;


// local variables
static CONTROLLER cont1, cont2;
static int INPUT_key_code = AKEY_NONE;
static int INPUT_key_shift = 0;

/* MEMORY MAP                   INDEX  (0 is invalid)
* 0-3FFF    RAM       (read/write)
* 4000-7FFF RAM       (read/write)
* 8000-9FFF BASIC     right CART 8K
* A000-BFFF BASIC     left  CART 8K
* D000-D0FF GTIA regs
* D200-D2FF POKEY regs
* D300-D3FF PIA  regs
* D400-D4FF ANTIC regs
* D800-FFFF OS ROM (ro)
* E000    I/O expansion
*/


// memory CPU read (load) handler
uint8 Atari_GetByte(uint16 addr)
{
  if (addr < MEMORY_SIZE) { // MAPPER_RAM
      return(memory[addr]);
  }
  else if ( (addr >= 0xD000) && (addr < 0xD100) ) {  // MAPPER_GTIA
      return GTIA_GetByte(addr,1);
  } 
  else if ( (addr >= 0xD200) && (addr < 0xD300) ) {  // MAPPER_POKEY
      return POKEY_GetByte(addr,1);       
  } 
  else if ( (addr >= 0xD300) && (addr < 0xD400) ) {  // MAPPER_PIA
      return PIA_GetByte(addr);       
  } 
  else if ( (addr >= 0xD400) && (addr < 0xD500) ) {  // MAPPER_ANTIC
      return ANTIC_GetByte(addr,1);
  }
  else if ((addr >= 0xD800) && (addr < 0x10000)) {  // MAPPER_ROM (bios)
      //return(memory[addr]);
      return(romos[addr-0xD800]);
  }

  //case MAPPER_IOEXP:  // I/O exp read
  //  return IOEXPread(addr);

  return 0xff;
}

// memory CPU write (store) handler
void Atari_PutByte(uint16 addr, uint8 byte)
{
  if (addr < 0x8000) { // MAPPER_RAM
      memory[addr] = byte;
  }
  else if (addr < 0xC000) {  // MAPPER_ROM
  }
  else if ( (addr >= 0xD000) && (addr < 0xD100) ) {  // MAPPER_GTIA
      GTIA_PutByte(addr, byte);    
  } 
  else if ( (addr >= 0xD200) && (addr < 0xD300)  ) {  // MAPPER_POKEY
      POKEY_PutByte(addr, byte);                     
  } 
  else if ( (addr >= 0xD300) && (addr < 0xD400)  ) {  // MAPPER_PIA
      PIA_PutByte(addr, byte);                     
  } 
  else if ( (addr >= 0xD400) && (addr < 0xD500) ) {  // MAPPER_ANTIC
      ANTIC_PutByte(addr, byte);
  }
  
  //case MAPPER_IOEXP:  // I/O exp write
  //  IOEXPwrite(addr, byte);
}


static void INPUT_Frame(void)
{
  int i;
  static int last_key_code = AKEY_NONE;
  static int last_key_break = 0;

  if (cont1.trig) GTIA_TRIG[0]=0; else GTIA_TRIG[0]=1;


  i = (INPUT_key_code == AKEY_BREAK);
  if (i && !last_key_break) {
    if (POKEY_IRQEN & 0x80) {
      POKEY_IRQST &= ~0x80;
      CPU_GenerateIRQ();
    }
  }
  last_key_break = i;
  POKEY_SKSTAT |= 0xc;
  if (INPUT_key_shift)
    POKEY_SKSTAT &= ~8;

  if (INPUT_key_code < 0) {
    //if (CASSETTE_press_space) {
    //  INPUT_key_code = AKEY_SPACE;
    //  CASSETTE_press_space = 0;
    //}
    //else {
      last_key_code = AKEY_NONE;
    //}
  }  
  if ((INPUT_key_code&~0x17) == AKEY_SHFTCTRL) {
    INPUT_key_code = AKEY_NONE;
  }
  if (INPUT_key_code >= 0) {    
//emu_printi(INPUT_key_code);     
    POKEY_SKSTAT &= ~4;
    if ((INPUT_key_code ^ last_key_code) & ~AKEY_SHFTCTRL) {
    /* ignore if only shift or control has changed its state */
      last_key_code = INPUT_key_code;
      POKEY_KBCODE = (UBYTE) INPUT_key_code;
      if (POKEY_IRQEN & 0x40) {
        if (POKEY_IRQST & 0x40) {       
          POKEY_IRQST &= ~0x40;
          CPU_GenerateIRQ();
        }
        else {
          /* keyboard over-run */
          POKEY_SKSTAT &= ~0x40;
          /* assert(CPU_IRQ != 0); */
        }
      }
    }
  }  
}

// check keyboard and set kbcode on VBI (not for A800)
void INPUT_Scanline(void)
{
//  if (cont1.trig) GTIA_TRIG[0]=0; else GTIA_TRIG[0]=1;
}


static void load_CART(char * cartname) 
{
  int flen = emu_FileSize(cartname);
  int f=emu_FileOpen(cartname, "r+b");
  if (flen < 16384) {
    emu_printf("8k");
    for(int i=0; i<flen; i++) memory[0xA000 + i] = emu_FileGetc(f);    
  }
  else {
    emu_printf("16k");    
    for(int i=0; i<flen; i++) memory[0x8000 + i] = emu_FileGetc(f);
  }

  emu_FileClose(f); 
}

  


void add_esc(UWORD address, UBYTE esc_code)
{
  memory[address++] = 0xf2; /* ESC */
  memory[address++] = esc_code; /* ESC CODE */
  memory[address] = 0x60;   /* RTS */
}

int Atari_PORT(int num)
{
  int nibble_0 = 0xff;
  int nibble_1 = 0xff;

  if (num == 0)
  {  
    if (cont1.left) nibble_0 &= ~0x04;   
    if (cont1.right) nibble_0 &= ~0x08;   
    if (cont1.up) nibble_0 &= ~0x01;   
    if (cont1.down) nibble_0 &= ~0x02;   
    if (cont2.left) nibble_1 &= ~0x04;   
    if (cont2.right) nibble_1 &= ~0x08;   
    if (cont2.up) nibble_1 &= ~0x01;   
    if (cont2.down) nibble_1 &= ~0x02;   
  }
  return (nibble_1 << 4) | nibble_0;
}

static void Initialise(void)
{
  int i;

  emu_printf("Initialising ...");

  // Set up memory area
  memset(memory, 0, MEMORY_SIZE);
  memset(memory+0x8000, 0xff, 0x4000);

  // init controllers
  memset(&cont1, 0, sizeof(CONTROLLER));
  memset(&cont2, 0, sizeof(CONTROLLER));
  

  // Load bios to address 0xF800
  //for (i = 0; i < 0x2800; i++)
  //{
  //  memory[0xD800 + i] = romos[i];
  //}  
  //add_esc(0xe459, ESC_SIOV);  
}



#define INV_KEY 255

const unsigned char keyboardAsciiConv[] = // Ascii to ATARI keys
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
/* 0x09 */ 0x2C,      // tab
/* 0x0A */ INV_KEY,
/* 0x0B */ INV_KEY,
/* 0x0C */ INV_KEY,
/* 0x0D */ 0x0C,      // enter
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
/* 0x20 */ 0x21,     // space
/* 0x21 */ 95,       // ! exclamation mark
/* 0x22 */ 94,       // " double quote   
/* 0x23 */ 90,       // # dies
/* 0x24 */ 88,       // $ dollar
/* 0x25 */ 93,       // % percent
/* 0x26 */ 91,       // & ampercent
/* 0x27 */ INV_KEY,  // ' singlequote
/* 0x28 */ 112,      // ( bracket left
/* 0x29 */ 114,      // ) bracket right
/* 0x2A */ 0x07,     // * mult
/* 0x2B */ 0x06,     // + plus
/* 0x2C */ 0x20,     // , comma
/* 0x2D */ 0x0E,     // - minus
/* 0x2E */ 0x22,     // . period
/* 0x2F */ 0x26,     // / slash
/* 0x30 */ 0x32,     // 0
/* 0x31 */ 0x1F,     // 1
/* 0x32 */ 0x1E,     // 2 
/* 0x33 */ 0x1A,     // 3 
/* 0x34 */ 0x18,     // 4
/* 0x35 */ 0x1D,     // 5
/* 0x36 */ 0x1B,     // 6
/* 0x37 */ 0x33,     // 7
/* 0x38 */ 0x35,     // 8
/* 0x39 */ 0x30,     // 9
/* 0x3A */ 66,       // : colon
/* 0x3B */ 0x02,     // ; semi colon
/* 0x3C */ 0x36,     // <
/* 0x3D */ 0x0F,     // = equal
/* 0x3E */ 0x37,     // >
/* 0x3F */ 102,      // ?
/* 0x40 */ 117,      // @
/* 0x41 */ INV_KEY,  // A
/* 0x42 */ INV_KEY,  // B
/* 0x43 */ INV_KEY,  // C
/* 0x44 */ INV_KEY,  // D
/* 0x45 */ INV_KEY,  // E
/* 0x46 */ INV_KEY,  // F
/* 0x47 */ INV_KEY,  // G
/* 0x48 */ INV_KEY,  // H
/* 0x49 */ INV_KEY,  // I
/* 0x4A */ INV_KEY,  // J
/* 0x4B */ INV_KEY,  // K
/* 0x4C */ INV_KEY,  // L
/* 0x4D */ INV_KEY,  // M
/* 0x4E */ INV_KEY,  // N
/* 0x4F */ INV_KEY,  // O
/* 0x50 */ INV_KEY,  // P
/* 0x51 */ INV_KEY,  // Q
/* 0x52 */ INV_KEY,  // R
/* 0x53 */ INV_KEY,  // S
/* 0x54 */ INV_KEY,  // T
/* 0x55 */ INV_KEY,  // U
/* 0x56 */ INV_KEY,  // V
/* 0x57 */ INV_KEY,  // W
/* 0x58 */ INV_KEY,  // X
/* 0x59 */ INV_KEY,  // Y
/* 0x5A */ INV_KEY,  // Z
/* 0x5B */ INV_KEY,  // square bracket open
/* 0x5C */ INV_KEY,  // backslach
/* 0x5D */ INV_KEY,  // square braquet close
/* 0x5E */ INV_KEY,  // ^ circonflex
/* 0x5F */ INV_KEY,  // _ undescore
/* 0x60 */ INV_KEY,  // `backquote
/* 0x61 */ 0x3F,     // a
/* 0x62 */ 0x15,     // b
/* 0x63 */ 0x12,     // c
/* 0x64 */ 0x3A,     // d
/* 0x65 */ 0x2A,     // e
/* 0x66 */ 0x38,     // f
/* 0x67 */ 0x3D,     // g
/* 0x68 */ 0x39,     // h
/* 0x69 */ 0x0D,     // i
/* 0x6A */ 0x01,     // j
/* 0x6B */ 0x05,     // k
/* 0x6C */ 0x00,     // l
/* 0x6D */ 0x25,     // m
/* 0x6E */ 0x23,     // n
/* 0x6F */ 0x08,     // o
/* 0x70 */ 0x0A,     // p
/* 0x71 */ 0x2F,     // q
/* 0x72 */ 0x28,     // r
/* 0x73 */ 0x3E,     // s
/* 0x74 */ 0x2D,     // t
/* 0x75 */ 0x0B,     // u
/* 0x76 */ 0x10,     // v
/* 0x77 */ 0x2E,     // w
/* 0x78 */ 0x16,     // x
/* 0x79 */ 0x2B,     // y
/* 0x7A */ 0x17,     // z
/* 0x7B */ INV_KEY,  // curly bracket open
/* 0x7C */ INV_KEY,  // or
/* 0x7D */ INV_KEY,  // curly bracket close  
/* 0x7E */ INV_KEY,  // tilt
/* 0x7F */ AKEY_BACKSPACE,     // backspace

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
/* 0xD7 */ AKEY_UP,   // U
/* 0xD8 */ AKEY_LEFT, // L
/* 0xD9 */ AKEY_RIGHT,// R
/* 0xDA */ AKEY_DOWN, // D
/* 0xDB */ INV_KEY,
/* 0xDC */ INV_KEY,
/* 0xDD */ INV_KEY,
/* 0xDE */ INV_KEY,
/* 0xDF */ INV_KEY
};


static int ik;     // joypad key
static int ihk;    // I2C keyboard key
static int iusbhk; // USB keyboard key

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

void at8_Input(int click) {
  ik  = emu_GetPad();
  ihk = emu_ReadI2CKeyboard();   
}

#define R32(rgb) ((rgb>>16)&0xff) 
#define G32(rgb) ((rgb>>8)&0xff) 
#define B32(rgb) (rgb & 0xff) 

void at8_Init(void)
{
   int i;

  // Palette
  for (i = 0; i < PALETTE_SIZE; i++)
  {
    emu_SetPaletteEntry(R32(colourtable[i]), G32(colourtable[i]), B32(colourtable[i]), i);
  }

#ifdef HAS_SND
  emu_sndInit();
  POKEYSND_Init(POKEYSND_FREQ_17_APPROX, 44100, 1, POKEYSND_BIT16);
#endif

  emu_printf("Allocating RAM");
  if (memory == NULL) memory = emu_Malloc(MEMORY_SIZE);

  Initialise();
}


void at8_Step(void)
{
  int k  = ik;
  int hk = ihk;
  if (iusbhk) hk = iusbhk;
    
  CONTROLLER * which;
  
  if (k & 0x8000) which=&cont2;
  else which=&cont1;

  // Start
  if (k & MASK_KEY_USER1)
    INPUT_key_consol &= ~0x01;
  else
    INPUT_key_consol |= 0x01;   

  // Select
  if (k & MASK_KEY_USER2)
    INPUT_key_consol &= ~0x02;
  else
    INPUT_key_consol |= 0x02;   

  // OPTION
  if (k & MASK_KEY_USER3)
    INPUT_key_consol &= ~0x04;
  else
    INPUT_key_consol |= 0x04;   

  if (hk != 0) {
    int key = keyboardAsciiConv[hk];
    if (key != INV_KEY) INPUT_key_code = key;  
  }
  else {
    INPUT_key_code = AKEY_NONE;    
  }
  
  // Keyboard
  INPUT_Frame();

  // Joystick side button, trigger and directions
  if ( (k & MASK_JOY2_BTN) || (k & MASK_JOY1_BTN) )
      which->trig = 1;    
  else
      which->trig = 0;
  if ( (k & MASK_JOY2_DOWN) || (k & MASK_JOY1_DOWN) )
      which->down = 1;
  else
      which->down = 0;
  if ( (k & MASK_JOY2_UP) || (k & MASK_JOY1_UP) )
      which->up = 1;
  else
      which->up = 0;
  if ( (k & MASK_JOY2_LEFT) || (k & MASK_JOY1_LEFT) ) 
      which->right = 1;
  else
      which->right = 0;
  if ( (k & MASK_JOY2_RIGHT) || (k & MASK_JOY1_RIGHT) )
      which->left = 1;
  else
      which->left = 0; 

  GTIA_Frame();
  ANTIC_Frame(1); 
  emu_DrawVsync();
  POKEY_Frame();
}



void at8_Start(char * cartname)
{
  load_CART(cartname);

  emu_printf("antic");
  ANTIC_Initialise();
  emu_printf("gtia");  
  GTIA_Initialise();
  emu_printf("pia");
  PIA_Initialise();  
  emu_printf("pokey");
  POKEY_Initialise();
  PORTA = 0x00;
  ANTIC_NMIEN = 0x00;
  ANTIC_NMIST = 0x00;
  memory[0x244] = 1;  
  GTIA_TRIG[0]=1;
  GTIA_TRIG[1]=1;
  GTIA_TRIG[2]=1;
  GTIA_TRIG[3]=1;
  emu_printf("6502 reset");
  CPU_Reset();
    
  emu_printf("init done");   
}
