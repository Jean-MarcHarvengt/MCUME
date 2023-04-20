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
#define emu_Step(x) {z81_Step();}
#define emu_Input(x) {z81_Input(x);}

#ifdef KEYMAP_PRESENT
   
#ifdef PICOZX
const unsigned short key_map1[] = {
  30,31,32,33,34,35,36,37,38,39,
  20,26, 8,21,23,28,24,12,18,19,
   4,22, 7, 9,10,11,13,14,15,40,
   0,29,27, 6,25, 5,17,16, 0,44,
  0,0,0,0,0,0
  };  

const unsigned short key_map2[] = {
  0,0,0,24+64,34+64,35+64,36+64,37+64,18+64,0,
  21+64,23+64,28+64,17+64,16+64,31+64,26+64,0,0,19+64,
  0,0,0,12+64,18+64,0,13+64,14+64,15+64,40+64,
  0,29+64,27+64, 6+64,25+64, 5+64,1+64,1,0,44+64,
  
  0,0,0,0,0,0
  }; 
  // To be mapped
  //30+64,0,32+64,33+64,0,0,0,0,38+64,39+64,
  //20+64,0, 8+64,0,0,0,0,0,0,0,
  //4+64,22+64, 7+64, 9+64,10+64,11+64,0,0,0,0,
  //0,    0,0, 0,0, 0,0,0, 0,0,
  //0,0,0,0,0,0
  //};  

const unsigned short key_map3[] = {
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0
  };

const unsigned short matkeys[] = {
  0x104,0x101,0x108,0x102,0x110,0x120,0x140,0x204,0x201,0x208, // row 1
  0x202,0x210,0x220,0x240,0x304,0x301,0x308,0x302,0x310,0x320, // row 2
  0x340,0x404,0x401,0x408,0x402,0x410,0x420,0x440,0x504,0x501, // row 3
  0x020,0x508,0x502,0x510,0x520,0x540,0x604,0x601,0x040,0x608, // row 4
  0x008,0x001,0x002,0x010,0x620,0x640 }; // cursor keys up, left, right, down, <<<, >>>

#else
/*
    {25, 6,27,29,224},// vcxz<caps shift=Lshift>
    {10, 9, 7,22, 4}, // gfdsa
    {23,21, 8,26,20}, // trewq
    {34,33,32,31,30}, // 54321
    {35,36,37,38,39}, // 67890
    {28,24,12,18,19}, // yuiop
    {11,13,14,15,40}, // hjkl<enter>
    { 5,17,16,1,44},  // bnm. <space>
*/

#define keylables_map1_0 (char *)"qwertyuiop\x1a"
#define keylables_map1_1 (char *)" asdfghjkl\x19"
#define keylables_map1_2 (char *)" zxcvbnm.\x10 "
const unsigned short key_map1[] = {
  20,26,8,21,23,28,24,12,18,19,39+64,
  0, 4, 22, 7,9, 10,11,13,14,15,40,
  0,29,27,6,25,5,17,16,1,44, 
  0,0,0,0   //up,left,right,down
  };  

#define keylables_map2_0 (char *)"1234567890="
#define keylables_map2_1 (char *)"    $ + *- "
#define keylables_map2_2 (char *)" ()?/\"<>,: "
const unsigned short key_map2[] = {
  30,31,32,33,34,35,36,37,38,39,15+64,
  0, 0,0,0,24+64,0,14+64,0,5+64,13+64,0,
  0, 12+64,18+64,6+64,25+64,19+64,17+64,16+64,1+64,29+64,
  36+64,34+64,37+64,35+64   //up,left,right,down
  };  

#define keylables_map3_0 (char *)"           "
#define keylables_map3_1 (char *)"           "
#define keylables_map3_2 (char *)"        ;  "
const unsigned short key_map3[] = {
  0, 0,0,0,0,0,0,0,0,0,0, // Upper case
  0, 0,0,0,0,0,0,0,0,0,0,
  0, 0,0,0,0,0,0,0,27+64,0,
  36+64,34+64,37+64,35+64   //up,left,right,down
  };

const unsigned short matkeys[] = {
  0x020,0x120,0x220,0x320,0x420,0x408,0x308,0x208,0x108,0x008,0x002, // row 1
  0x510,0x010,0x110,0x210,0x310,0x410,0x401,0x301,0x201,0x101,0x001, // row 2
  0x520,0x102,0x202,0x302,0x402,0x404,0x304,0x204,0x104,0x004, // row 3
  0x508,0x501,0x502,0x504 }; // cursor keys
#endif 
#endif

#endif
