#ifndef BASICTYPES_H
#define BASICTYPES_H

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long  uint32;

typedef signed char int8;
typedef signed short int16;
typedef signed long int32;


typedef unsigned char byte;
typedef unsigned short word;
typedef signed char offset;
typedef union
{
#ifdef LSB_FIRST
  struct { unsigned long l,h; } DW;
#else
  struct { unsigned long h,l; } DW;
#endif
//  unsigned long long LL;
} ullong;

typedef union
{
#ifdef LSB_FIRST
  struct { byte l,h,a,b; } B;
#else
  struct { byte b,a,h,l; } B;
#endif
  unsigned long DW;
} dpair;

typedef union
{
#ifdef LSB_FIRST
  struct { byte l,h; } B;
#else
  struct { byte h,l; } B;
#endif
  word W;
} pair;
//END Added from NES

#endif
