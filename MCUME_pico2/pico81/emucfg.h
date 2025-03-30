#ifndef EMUCFG_H
#define EMUCFG_H

#define PALETTE_SIZE         2
#define VID_FRAME_SKIP       0x0
#define TFT_VBUFFER_YCROP    0
#define SINGLELINE_RENDERING 1
//#define CUSTOM_SND           1
//#define TIMER_REND           1
#define EXTRA_HEAP           0x10
#define FILEBROWSER

// Title:     <                        >
#define TITLE "    ZX81/ZX80 Emulator"
#define ROMSDIR "z81"

#define emu_Init(ROM) {z81_Start(ROM); z81_Init(); }
#define emu_Step() {z81_Step();}
#define emu_Input(x) {z81_Input(x);}

#ifdef KEYMAP_PRESENT
   
#ifdef PICO2ZX
const unsigned short key_map1[] = {
  30,31,32,33,34,35,36,37,38,39,39+64,
  20,26, 8,21,23,28,24,12,18,19,
   4,22, 7, 9,10,11,13,14,15,40,
   0,29,27, 6,25, 5,17,16, 1,44,
   0, 0, 0, 0, 0, 0 
  };  

const unsigned short key_map2[] = {
  30+64,31+64,32+64,33+64,34+64,35+64,36+64,37+64,38+64,39+64,39+64,
  20+64,26+64, 8+64,21+64,23+64,28+64,24+64,12+64,18+64,19+64,
   4+64,22+64, 7+64, 9+64,10+64,11+64,13+64,14+64,15+64,40+64,
   0+64,29+64,27+64, 6+64,25+64, 5+64,17+64,16+64, 1+64,44+64,
   0, 0, 0, 0, 0, 0
  };

const unsigned short key_map3[] = {
  30+64,31+64,32+64,33+64,34+64,35+64,36+64,37+64,38+64,39+64,39+64,
  20+64,26+64, 8+64,21+64,23+64,28+64,24+64,12+64,18+64,19+64,
   4+64,22+64, 7+64, 9+64,10+64,11+64,13+64,14+64,15+64,40+64,
   0+64,29+64,27+64, 6+64,25+64, 5+64,17+64,16+64, 1+64,44+64,
   0, 0, 0, 0, 0, 0
  };

const unsigned short matkeys[] = {
  0x401,0x301,0x201,0x101,0x001,0x008,0x108,0x208,0x308,0x408,0x580, // row 1
  0x402,0x302,0x202,0x102,0x002,0x010,0x110,0x210,0x310,0x410, // row 2
  0x404,0x304,0x204,0x104,0x004,0x040,0x140,0x240,0x340,0x440, // row 3
  0x420,0x320,0x220,0x120,0x020,0x080,0x180,0x280,0x380,0x480, // row 4
  0x520,0x540,0x510,0x508,0x502,0x504 }; // cursor keys up, left, right, down, <<<, >>>
#endif 
#endif

#endif
