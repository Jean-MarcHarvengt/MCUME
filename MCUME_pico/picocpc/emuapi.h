#ifndef EMUAPI_H
#define EMUAPI_H

#include "platform_config.h"

#define EXTRA_HEAP  0x10

/* Title */
#define TITLE "    Amstrad CPC Emulator"
#define ROMSDIR "roms"

#define emu_Init(ROM) {cpc_Init(); cpc_Start(ROM);}
#define emu_Step(x) {cpc_Step();}
#define emu_Input(x) {cpc_Input(x);}

#define PALETTE_SIZE        27
#define VID_FRAME_SKIP      0x0
#define TFT_VBUFFER_YCROP   0

#define ACTION_NONE         0
#define ACTION_MAXKBDVAL    225
#define ACTION_EXITKBD      128
#define ACTION_RUNTFT       129
#define ACTION_RUNVGA       130

#ifdef KEYMAP_PRESENT
// TODO add key mappings
#endif

// here are joystick mask mappings in emuapi.h for picospeccy

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
#define MASK_OSKB       0x8000

extern void emu_init(void);
extern void emu_start(void);
extern void emu_resetSD(void);

extern void emu_printf(const char* text);
extern void emu_printi(int val);
extern void* emu_Malloc(int size);
extern void emu_Free(void* pt);

extern int emu_FileOpen(const char* filepath, const char* mode);
extern int emu_FileRead(void* buf, int size, int handler);
extern int emu_FileGetc(int handler);
extern int emu_FileSeek(int handler, int seek, int origin);
extern int emu_FileTell(int handler);
extern void emu_FileClose(int handler);

extern unsigned int emu_FileSize(const char* filepath);
extern unsigned int emu_LoadFile(const char* filepath, void* buf, int size);

extern void emu_SetPaletteEntry(unsigned char r, unsigned char g, unsigned char b, int index);
extern void emu_DrawPixel(unsigned char * VBuf, int x, int y);
extern void emu_DrawScreen(unsigned char* VBuf, int width, int height, int stride);
extern void emu_DrawLine(unsigned char* VBuf, int width, int height, int line);
extern void emu_DrawLine8(unsigned char* VBuf, int width, int height, int line);
extern void emu_DrawLine16(unsigned char* Vbuf, int width, int height, int line);
extern void emu_DrawVsync(void);
extern int emu_FrameSkip(void);
extern void* emu_LineBuffer(int line);
#define RGBVAL32(r,g,b)  ( (r<<16) | (g<<8) | b)
#define RGBVAL16(r,g,b)  ( (((r>>3)&0x1f)<<11) | (((g>>2)&0x3f)<<5) | (((b>>3)&0x1f)<<0) )
#define RGBVAL8(r,g,b)   ( (((r>>5)&0x07)<<5) | (((g>>5)&0x07)<<2) | (((b>>6)&0x3)<<0) )
#define R16(rgb) ((rgb>>8)&0xf8) 
#define G16(rgb) ((rgb>>3)&0xfc) 
#define B16(rgb) ((rgb<<3)&0xf8) 
extern void emu_drawText(unsigned short x, unsigned short y, const char* text, unsigned short fgcolor, unsigned short bgcolor, unsigned short doublesize);

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