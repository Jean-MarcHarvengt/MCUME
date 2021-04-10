#ifndef __MEMORY__
#define __MEMORY__

extern unsigned char * memory;

#define  MEMORY_GetByte(addr)   (Atari_GetByte(addr))
#define  MEMORY_PutByte(addr,byte)  Atari_PutByte(addr,byte)

#define  MEMORY_dGetByte(addr)   (Atari_GetByte(addr))
#define  MEMORY_dPutByte(addr,byte)  Atari_PutByte(addr,byte)
#define MEMORY_dGetWord(x)       (Atari_GetByte(x) | (Atari_GetByte((x) + 1) << 8))
#define MEMORY_dPutWord(x,y)     (Atari_PutByte(x,(UBYTE) (y)) , Atari_PutByte(x + 1,(UBYTE) ((y) >> 8)) 
/* faster versions of dGetWord and dPutWord for even addresses */
/* TODO: guarantee that memory is UWORD-aligned and use UWORD access */
#define MEMORY_dGetWordAligned(x)    MEMORY_dGetWord(x)
#define MEMORY_dPutWordAligned(x,y)  MEMORY_dPutWord(x,y)

#endif
