#ifndef EMUAPI_H
#define EMUAPI_H

#include "platform_config.h"
#include "emucfg.h"

#define ACTION_NONE     0
#define ACTION_RUN      1

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

#define RGBVAL16(r,g,b)  ( (((r>>3)&0x1f)<<11) | (((g>>2)&0x3f)<<5) | (((b>>3)&0x1f)<<0) )

#ifdef __cplusplus
extern "C"
{
#endif
extern void emu_init(void);
extern void emu_start(void);
extern void emu_resetSD(void);

extern void emu_printf(const char * text);
extern void emu_printi(int val);
extern void * emu_Malloc(int size);
extern void emu_Free(void * pt);

extern int emu_FileOpen(const char * filepath, const char * mode);
extern int emu_FileRead(void * buf, int size, int handler);
extern int emu_FileGetc(int handler);
extern int emu_FileSeek(int handler, int seek, int origin);
extern int emu_FileTell(int handler);
extern void emu_FileClose(int handler);

extern unsigned int emu_FileSize(const char * filepath);
extern unsigned int emu_LoadFile(const char * filepath, void * buf, int size);

extern void emu_SetPaletteEntry(unsigned char r, unsigned char g, unsigned char b, int index);
extern void emu_DrawLinePal16(unsigned char * VBuf, int width, int height, int line);
extern void emu_DrawLine16(unsigned short * VBuf, int width, int height, int line);
extern void emu_DrawScreenPal16(unsigned char * VBuf, int width, int height, int stride);
extern void emu_DrawVsync(void);
extern int emu_FrameSkip(void);
extern int emu_IsVga(void);
//extern void emu_DrawLine(unsigned char * VBuf, int width, int height, int line);
//extern void * emu_LineBuffer(int line);

extern void emu_InitJoysticks(void);
extern int emu_SwapJoysticks(int statusOnly);
extern unsigned short emu_DebounceLocalKeys(void);
extern int emu_ReadKeys(void);
extern int emu_GetPad(void);
extern int emu_ReadAnalogJoyX(int min, int max);
extern int emu_ReadAnalogJoyY(int min, int max);
extern int emu_ReadI2CKeyboard(void);
extern unsigned char emu_ReadI2CKeyboard2(int row);
extern void emu_KeyboardOnUp(int keymodifer, int key);
extern void emu_KeyboardOnDown(int keymodifer, int key);
extern int emu_oskbActive(void);

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
#ifdef __cplusplus
}
#endif


#endif
