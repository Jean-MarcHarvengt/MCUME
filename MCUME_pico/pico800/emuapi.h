#ifndef EMUAPI_H
#define EMUAPI_H

#include "platform_config.h"

#define CUSTOM_SND  1
//#define TIMER_REND  1
#define EXTRA_HEAP  0x10

// Title:     <                        >
#define TITLE "    Atari 800 Emulator"
#define ROMSDIR "800"

#define emu_Init(ROM) {at8_Init(); at8_Start(ROM);}
#define emu_Step(x) {at8_Step();}
#define emu_Input(x) {}

#define PALETTE_SIZE         256
#define VID_FRAME_SKIP       0x0
#define TFT_VBUFFER_YCROP    0
#define SINGLELINE_RENDERING 1

#define R32(rgb) ((rgb>>16)&0xff) 
#define G32(rgb) ((rgb>>8)&0xff) 
#define B32(rgb) (rgb & 0xff) 

#define ACTION_NONE          0
#define ACTION_MAXKBDVAL     0X40
#define ACTION_EXITKBD       127
#define ACTION_RUNTFT        126
#define ACTION_RUNVGA        125


#ifdef KEYMAP_PRESENT
#ifdef PICOMPUTER
const unsigned short key_map1[] = {
  //0x1F+1,0x1F  ,0x1A+1,0x18+1,0x1D+1,0x1B+1,0x33+1,0x35+1,0x30+1,0x32+1,0x34+1, // Digits
  0x2F+1,0x2F  ,0x2A+1,0x28+1,0x2D+1,0x2B+1,0x0B+1,0x0D+1,0x08+1,0x0A+1,0x34+1, 
  0x2C+1,0x3F+1,0x3F  ,0x3A+1,0x38+1,0x3D+1,0x39+1,0x01+1,0x05+1,0x00+1,0x0C+1, 
  0,     0x17+1,0x16+1,0x12+1,0x10+1,0x15+1,0x23+1,0x25+1,0x20+1, 0x21+1,
  0,0,0,0
  };  

const unsigned short key_map2[] = {
  0x1F+1,0x1F,0x1A+1,0x18+1,0x1D+1,0x1B+1,0x33+1,0x35+1,0x30+1,0x32+1,0x34+1, // Digits
  0x2C+1,0x07+1,0x22+1,0x26+1,0x02+1,0x06+1,0x36+1,0x37+1,0x0F+1,0x0E + 1,0x06+1, // various 
  //0x07+1=*, 0x22+1=.,0x26+1=/, 0x02+1=;, 0x06+1=+, 0x36+1=<, 0x37+1=>, 0x0F+1==, 0x0E+1=-, 0x06+1=+
  0, 0,0,0,0,0,3,4,19,20,
  0,0,0,0
  };  

const unsigned short matkeys[] = {
  0x020,0x120,0x220,0x320,0x420,0x408,0x308,0x208,0x108,0x008,0x520, // row 1
  0x510,0x010,0x110,0x210,0x310,0x410,0x401,0x301,0x201,0x101,0x001, // row 2
  0x002,0x102,0x202,0x302,0x402,0x404,0x304,0x204,0x104,0x004, // row 3
  0x508,0x501,0x502,0x504 }; // cursor keys
#endif 
#endif

/* Pokey code   
KEY_A  = 63
KEY_S  = 62
KEY_G  = 61
KEY_Cap  = 60
KEY_D  = 58
KEY_H  = 57
KEY_F  = 56
KEY_Great = 55
KEY_Less = 54
KEY_8  = 53
KEY_BSp  = 52
KEY_7  = 51
KEY_0  = 50
KEY_9  = 48
KEY_Q  = 47
KEY_W  = 46
KEY_T  = 45
KEY_Tab  = 44
KEY_Y  = 43
KEY_E  = 42
KEY_R  = 40
KEY_Inv  = 39
KEY_Slash = 38
KEY_M  = 37
KEY_N  = 35
KEY_Dot  = 34
KEY_Spa  = 33
KEY_Comma = 32
KEY_1  = 31
KEY_2  = 30
KEY_5  = 29
KEY_Esc  = 28
KEY_6  = 27
KEY_3  = 26
KEY_4  = 24
KEY_Z  = 23
KEY_X  = 22
KEY_B  = 21
KEY_F4  = 20
KEY_F3  = 19
KEY_C  = 18
KEY_Hlp  = 17
KEY_V  = 16
KEY_Equal = 15
KEY_Minus = 14
KEY_I  = 13
KEY_Ret  = 12
KEY_U  = 11
KEY_P  = 10
KEY_O  = 8
KEY_Aster = 7
KEY_plus = 6
KEY_K  = 5
KEY_F2  = 4
KEY_F1  = 3
KEY_Semi = 2
KEY_J  = 1
KEY_L  = 0
*/

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



extern void emu_init(void);
extern void emu_start(void);
extern void emu_resetSD(void);

extern void emu_printf(char * text);
extern void emu_printi(int val);
extern void * emu_Malloc(int size);
extern void emu_Free(void * pt);

extern int emu_FileOpen(char * filename);
extern int emu_FileRead(char * buf, int size);
extern unsigned char emu_FileGetc(void);
extern int emu_FileSeek(int seek);
extern void emu_FileClose(void);
extern int emu_FileSize(char * filename);
extern int emu_LoadFile(char * filename, char * buf, int size);
extern int emu_LoadFileSeek(char * filename, char * buf, int size, int seek);
extern void emu_FileTempInit(void); 
extern void emu_FileTempRead(int addr, unsigned char * val, int n); 
extern void emu_FileTempWrite(int addr, unsigned char val); 

extern void emu_SetPaletteEntry(unsigned char r, unsigned char g, unsigned char b, int index);
extern void emu_DrawScreen(unsigned char * VBuf, int width, int height, int stride);
extern void emu_DrawLine(unsigned char * VBuf, int width, int height, int line);
extern void emu_DrawLine8(unsigned char * VBuf, int width, int height, int line);
extern void emu_DrawLine16(unsigned short * VBuf, int width, int height, int line);
extern void emu_DrawVsync(void);
extern int emu_FrameSkip(void);
extern void * emu_LineBuffer(int line);

extern void emu_InitJoysticks(void);
extern int emu_SwapJoysticks(int statusOnly);
extern unsigned short emu_DebounceLocalKeys(void);
extern int emu_ReadKeys(void);
extern int emu_GetPad(void);
extern int emu_ReadAnalogJoyX(int min, int max);
extern int emu_ReadAnalogJoyY(int min, int max);
extern int emu_ReadI2CKeyboard(void);
extern void emu_KeyboardOnUp(int keymodifer, int key);
extern void emu_KeyboardOnDown(int keymodifer, int key);

extern void emu_sndPlaySound(int chan, int volume, int freq);
extern void emu_sndPlayBuzz(int size, int val);
extern void emu_sndInit();
extern void emu_resetus(void);
extern int emu_us(void);

extern int emu_setKeymap(int index);

extern void emu_FileTempInit(void); 
extern void emu_FileTempRead(int addr, unsigned char * val, int n); 
extern void emu_FileTempWrite(int addr, unsigned char val); 
extern void emu_printh(int val);


#endif
