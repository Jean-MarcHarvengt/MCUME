#ifndef EMUAPI_H
#define EMUAPI_H

#include "platform_config.h"

#define CUSTOM_SND  1
//#define TIMER_REND  1

#define EXTRA_HEAP  0x10000


// Title:     <                                        >
#define TITLE "           Genesis Emulator             "
#define ROMSDIR "/gen"

#define emu_Init(ROM) {gen_Init(); gen_Start(ROM);}
#define emu_Step(x) {gen_Step();}
#define emu_Input(x) {gen_Input(x);}

#define MAX_FILENAME_PATH    64
#define NB_FILE_HANDLER      4
#define PALETTE_SIZE         1
#define VID_FRAME_SKIP       0x0
#define TFT_VBUFFER_YCROP    0
#define SINGLELINE_RENDERING 1

#define R32(rgb) ((rgb>>16)&0xff) 
#define G32(rgb) ((rgb>>8)&0xff) 
#define B32(rgb) (rgb & 0xff) 

#define ACTION_NONE          0
#define ACTION_MAXKBDVAL     16
#define ACTION_EXITKBD       128
#define ACTION_RUN1          129
#define ACTION_RUN2          130
#define ACTION_RUN3          131

#ifdef KEYMAP_PRESENT

#define keylables_map0_0 (char *)"qwertyuiop\x1a"
#define keylables_map0_1 (char *)" asdfghjkl\x19"
#define keylables_map0_2 (char *)" zxcvbnm,.;/"
#define keylables_map0_3 (char *)" +\x10-"
const unsigned short key_map0[] = {
  'q','w','e','r','t','y','u','i','o','p',157,  //lowecase
  0,'a','s','d','f','g','h','j','k','l',0x0D,
  0,'z','x','c','v','b','n','m',',','.',';','/',
  145,157,29,17,
  0,'+',' ','-'
  };  

#define keylables_map1_0 (char *)"QWERTYUIOP@"
#define keylables_map1_1 (char *)" ASDFGHJKL\x19"
#define keylables_map1_2 (char *)" ZXCVBNM<>:?"
#define keylables_map1_3 (char *)" =\x10_"
const unsigned short key_map1[] = {
  'Q','W','E','R','T','Y','U','I','O','P','@',  //shift uppercase
  0,'A','S','D','F','G','H','J','K','L',0x0D,
  0,'Z','X','C','V','B','N','M','<','>',':','?',
  145,157,29,17,
  0,'=',' ','_'
  };  

#define keylables_map2_0 (char *)"!\"#$%^&*()@"
#define keylables_map2_1 (char *)" |\\[]{}    "
#define keylables_map2_2 (char *)"        <>:?"
#define keylables_map2_3 (char *)" =\x10_"
const unsigned short key_map2[] = {
  '!','"','#','$','%','^','&','*','(',')','@',  // shiftothers
  0, '|','\\','[',']','{','}','\'',0,0,0,
  0, 0,0,0,0,0,0,0,'<','>',':','?',
  0,0,0,0,
  0,'=',' ','_'
  };  

#define keylables_map3_0 (char *)"1234567890 "
#define keylables_map3_1 (char *)"           "
#define keylables_map3_2 (char *)"            "
#define keylables_map3_3 (char *)"    "

const unsigned short key_map3[] = {
  '1','2','3','4','5','6','7','8','9','0',0, // digit keys
  0, 0,0,0,0,0,0,0,0,0,0,
  0, 0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,
  0,0,' ',0
  };

#define keylables_map4_0 (char *)"\x11\x12\x13\x14\x15\x16\x17\x18   "
#define keylables_map4_1 (char *)"           "
#define keylables_map4_2 (char *)"            "
#define keylables_map4_3 (char *)"    "

const unsigned short key_map4[] = {
  133,134,135,136,137,138,139,140,0,0,0, // function keys
  0, 0,0,0,0,0,0,0,0,0,0,
  0, 0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,
  0,0,' ',0
  };

#define keylables_map5_0 (char *)"           "
#define keylables_map5_1 (char *)"           "
#define keylables_map5_2 (char *)"            "
#define keylables_map5_3 (char *)"    "

const unsigned short key_map5[] = {
  0,0,0,0,0,0,0,0,0,0,0, // extra keys
  0, 0,0,0,0,0,0,0,0,0,0,
  0, 0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,
  0,0,' ',0
  };

const unsigned short matkeys[] = {
  0x004,0x008,0x108,0x104,0x208,0x204,0x308,0x304,0x408,0x404,0x410, // row 1
  0x502,0x002,0x020,0x102,0x120,0x202,0x220,0x302,0x320,0x402,0x420, // row 2
  0x508,0x001,0x040,0x101,0x140,0x201,0x240,0x210,0x340,0x301,0x401,0x440, // row 3
  0x504,0x520,0x540,0x501, // UP LEFT RIGHT DOWN
  0x510,0x010,0x110,0x310, // row 4
  };
   
#endif



#define MASK_JOY2_RIGHT 0x0001
#define MASK_JOY2_LEFT  0x0002
#define MASK_JOY2_UP    0x0004
#define MASK_JOY2_DOWN  0x0008
#define MASK_JOY2_BTN   0x0010
#define MASK_KEY_USER1  0x0020
#define MASK_KEY_USER2  0x0040
#define MASK_KEY_USER3  0x0080
#define MASK_JOY1_RIGHT 0x0100
#define MASK_JOY1_LEFT  0x0200
#define MASK_JOY1_UP    0x0400
#define MASK_JOY1_DOWN  0x0800
#define MASK_JOY1_BTN   0x1000
#define MASK_KEY_USER4  0x2000


#ifdef __cplusplus  
extern "C" {
#endif

extern void emu_init(void);
extern void emu_start(void);
extern void emu_printf(const char * text);
extern void emu_printi(int val);
extern void emu_printh(int val);
extern void * emu_Malloc(unsigned int size);
extern void * emu_MallocI(unsigned int size);
extern void emu_Free(void * pt);

extern int emu_FileOpen(const char * filepath, const char * mode);
extern int emu_FileRead(void * buf, int size, int handler);
extern int emu_FileGetc(int handler);
extern int emu_FileSeek(int handler, int seek, int origin);
extern int emu_FileTell(int handler);
extern void emu_FileClose(int handler);

extern unsigned int emu_FileSize(const char * filepath);
extern unsigned int emu_LoadFile(const char * filepath, void * buf, int size);
extern unsigned int emu_LoadFileSeek(const char * filepath, void * buf, int size, int seek);

extern void emu_SetPaletteEntry(unsigned char r, unsigned char g, unsigned char b, int index);
extern void emu_DrawScreen(unsigned char * VBuf, int width, int height, int stride);
extern void emu_DrawLine(unsigned char * VBuf, int width, int height, int line);
extern void emu_DrawLine16(unsigned short * VBuf, int width, int height, int line);
extern void emu_DrawLine8(unsigned char * VBuf, int width, int height, int line);
extern void emu_CopyLine(int width, int height, int ysrc, int ydst);
extern void emu_DrawVsync(void);
extern int emu_FrameSkip(void);
extern void * emu_LineBuffer(int line);
extern void emu_tweakVideo(int shiftdelta, int numdelta, int denomdelta);

extern bool menuActive(void);
extern char * menuSelection(void);
extern char * menuSecondSelection(void);
extern void toggleMenu(bool on);
extern int  handleMenu(unsigned short bClick);

extern int handleOSKB(void);
extern void toggleOSKB(bool forceon);

extern void emu_InitJoysticks(void);
extern int emu_SwapJoysticks(int statusOnly);
extern unsigned short emu_DebounceLocalKeys(void);
extern int emu_ReadKeys(void);
extern int emu_GetPad(void);
extern int emu_GetMouse(int *x, int *y, int *buts);
extern int emu_MouseDetected(void);
extern int emu_KeyboardDetected(void);
extern int emu_ReadAnalogJoyX(int min, int max);
extern int emu_ReadAnalogJoyY(int min, int max);
extern int emu_ReadI2CKeyboard(void);
extern unsigned char emu_ReadI2CKeyboard2(int row);
extern void emu_KeyboardOnUp(int keymodifer, int key);
extern void emu_KeyboardOnDown(int keymodifer, int key);
extern void emu_MidiOnDataReceived(unsigned char data);

extern void emu_sndPlaySound(int chan, int volume, int freq);
extern void emu_sndPlayBuzz(int size, int val);
extern void emu_sndInit();
extern void emu_resetus(void);
extern int emu_us(void);

extern int emu_setKeymap(int index);

#ifdef __cplusplus
}
#endif

#endif
