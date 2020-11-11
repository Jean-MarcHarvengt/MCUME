
#ifndef _DTYPES_H_
#define _DTYPES_H_

typedef unsigned char UBYTE;
typedef signed char BYTE;
typedef unsigned short UWORD;
typedef signed short WORD;
typedef unsigned int ULONG;
typedef signed int LONG;
typedef ULONG CPTR;

typedef unsigned char uae_u8;
typedef signed char uae_s8;
typedef unsigned short uae_u16;
typedef signed short uae_s16;
typedef unsigned int uae_u32;
typedef signed int uae_s32;
typedef ULONG uaecptr;


#undef REGPARAM
#ifndef REGPARAM
#define REGPARAM
#endif

#undef REGPARAM2
#ifndef REGPARAM2
#define REGPARAM2
#endif


#define SOUND_PRESENT 1
#define M68K_SPEED 8
//#define HAS_BOGOMEM 1
//#define HAS_MEMDISK  1
#define HAS_EXPANMEM 1
#define HAS_HARDFILE 1  
#define HAS_FILESYS 1  

#endif /* _DTYPES_H_ */
