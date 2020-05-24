 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * disk support
  *
  * (c) 1995 Bernd Schmidt
  */

#define FLOPPY_SIZE 0x100000

extern void DISK_init(void);
extern void DISK_select(UBYTE data);
extern UBYTE DISK_status(void);
extern void DISK_GetData(UWORD *mfm,UWORD *byt);
extern void DISK_Index(void);
extern void DISK_InitWrite(void);
extern void DISK_WriteData(void);
extern void disk_eject(int num);
extern int disk_empty(int num);
extern void disk_insert(int num, char *name);
extern void disk_swap();

void disk_memWrite(int pos, unsigned char val);

extern int indexpulse;
extern UWORD* mfmwrite;
