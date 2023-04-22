#ifndef EMUCFG_H
#define EMUCFG_H

#define PALETTE_SIZE         256
#define VID_FRAME_SKIP       0x0
#define TFT_VBUFFER_YCROP    0
#define SINGLELINE_RENDERING 1
#define CUSTOM_SND           1
//#define TIMER_REND           1
#define EXTRA_HEAP           0x10
#define FILEBROWSER

// Title:     <                        >
#define TITLE "    C64 Emulator "
#define ROMSDIR "c64"

#define emu_Init(ROM) {c64_Start(ROM); c64_Init(); }
#define emu_Step(x) { c64_Step(); }
#define emu_Input(x) { c64_Input(x); }

#ifdef KEYMAP_PRESENT
   
#ifdef PICOZX
const unsigned short key_map1[] = {
  '1','2','3','4','5','6','7','8','9','0',
  'Q','W','E','R','T','Y','U','I','O','P',
  'A','S','D','F','G','H','J','K','L',0x0D,
  0,'Z','X','C','V','B','N','M',0,' ',
  0,0,0,0,157,29 // L R
  };  

const unsigned short key_map2[] = {
  '!','@','#','$','%','&','\'','(',')','_',
  0,0,0,'<','>',0,0,0,';','"',
  0,0,0,0,0,'^','-','+','=',0x0D,
  0,':',0,'?','\/','*',',','.',0,' ',
  145,157,29,17,0,0 // U L R D
  };  

const unsigned short key_map3[] = {
  133,134,135,136,137,138,139,140,0,0, // function keys
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  145,157,29,17,0,0 // U L R D
  };

const unsigned short matkeys[] = {
  0x104,0x101,0x108,0x102,0x110,0x120,0x140,0x204,0x201,0x208, // row 1
  0x202,0x210,0x220,0x240,0x304,0x301,0x308,0x302,0x310,0x320, // row 2
  0x340,0x404,0x401,0x408,0x402,0x410,0x420,0x440,0x504,0x501, // row 3
  0x020,0x508,0x502,0x510,0x520,0x540,0x604,0x601,0x040,0x608, // row 4
  0x008,0x001,0x002,0x010,0x620,0x640 }; // cursor keys up, left, right, down, <<<, >>>

#else   
#define keylables_map1_0 (char *)"qwertyuiop\x1a"
#define keylables_map1_1 (char *)" asdfghjkl\x19"
#define keylables_map1_2 (char *)" zxcvbnm.\x10 "
const unsigned short key_map1[] = {
  'Q','W','E','R','T','Y','U','I','O','P',157,
  0,'A','S','D','F','G','H','J','K','L',0x0D,
  0,'Z','X','C','V','B','N','M','.',' ',
  0,0,0,0
  };  

#define keylables_map2_0 (char *)"1234567890 "
#define keylables_map2_1 (char *)" !@#$%+&*- "
#define keylables_map2_2 (char *)" ()?/\"<>,: "
const unsigned short key_map2[] = {
  '1','2','3','4','5','6','7','8','9','0',0,
  0, '!','@','#','$','%','+','&','*','-','\/',
  0, '(',')','?','\/','"','<','>',',',':',
  145,157,29,17 //U L R D
  };  

#define keylables_map3_0 (char *)"\x11\x12\x13\x14\x15\x16\x17\x18   "
#define keylables_map3_1 (char *)"           "
#define keylables_map3_2 (char *)"        ;= "
const unsigned short key_map3[] = {
  133,134,135,136,137,138,139,140,0,0,0, // function keys
  0, 0,0,0,0,0,0,0,0,0,0,
  0, 0,0,0,0,0,0,0,';','=',
  0,0,0,0
  };

const unsigned short matkeys[] = {
  0x020,0x120,0x220,0x320,0x420,0x408,0x308,0x208,0x108,0x008,0x002, // row 1
  0x510,0x010,0x110,0x210,0x310,0x410,0x401,0x301,0x201,0x101,0x001, // row 2
  0x520,0x102,0x202,0x302,0x402,0x404,0x304,0x204,0x104,0x004, // row 3
  0x508,0x501,0x502,0x504 }; // cursor keys
#endif 
#endif

#endif
