
#ifndef _DTYPES_H_
#define _DTYPES_H_

typedef unsigned char UBYTE;
typedef signed char BYTE;
typedef unsigned short UWORD;
typedef signed short WORD;
typedef unsigned int ULONG;
typedef signed int LONG;
typedef ULONG CPTR;

#undef REGPARAM
#ifndef REGPARAM
#define REGPARAM
#endif

#endif /* _DTYPES_H_ */

