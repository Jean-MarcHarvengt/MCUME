/** fMSX: portable MSX emulator ******************************/
/**                                                         **/
/**                          Boot.h                         **/
/**                                                         **/
/** This file contains MSX boot sector image used to create **/
/** new disk images during FORMAT operation.                **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994-2003                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/
const unsigned char BootBlock[] =
{
  0xEB,0xFE,0x90,0x56,0x46,0x42,0x2D,0x31,0x39,0x38,0x39,0x00,0x02,0x02,0x01,0x00,
  0x02,0x70,0x00,0xA0,0x05,0xF9,0x03,0x00,0x09,0x00,0x02,0x00,0x00,0x00,0xD0,0xED,
  0x53,0x58,0xC0,0x32,0xC2,0xC0,0x36,0x55,0x23,0x36,0xC0,0x31,0x1F,0xF5,0x11,0x9D,
  0xC0,0x0E,0x0F,0xCD,0x7D,0xF3,0x3C,0x28,0x28,0x11,0x00,0x01,0x0E,0x1A,0xCD,0x7D,
  0xF3,0x21,0x01,0x00,0x22,0xAB,0xC0,0x21,0x00,0x3F,0x11,0x9D,0xC0,0x0E,0x27,0xCD,
  0x7D,0xF3,0xC3,0x00,0x01,0x57,0xC0,0xCD,0x00,0x00,0x79,0xE6,0xFE,0xFE,0x02,0x20,
  0x07,0x3A,0xC2,0xC0,0xA7,0xCA,0x22,0x40,0x11,0x77,0xC0,0x0E,0x09,0xCD,0x7D,0xF3,
  0x0E,0x07,0xCD,0x7D,0xF3,0x18,0xB4,0x42,0x6F,0x6F,0x74,0x20,0x65,0x72,0x72,0x6F,
  0x72,0x0D,0x0A,0x50,0x72,0x65,0x73,0x73,0x20,0x61,0x6E,0x79,0x20,0x6B,0x65,0x79,
  0x20,0x66,0x6F,0x72,0x20,0x72,0x65,0x74,0x72,0x79,0x0D,0x0A,0x24,0x00,0x4D,0x53,
  0x58,0x44,0x4F,0x53,0x20,0x20,0x53,0x59,0x53,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF3,0x2A,
  0x51,0xF3,0x11,0x00,0x01,0x19,0x01,0x00,0x01,0x11,0x00,0xC1,0xED,0xB0,0x3A,0xEE,
  0xC0,0x47,0x11,0xEF,0xC0,0x21,0x00,0x00,0xCD,0x51,0x52,0xF3,0x76,0xC9,0x18,0x64,
  0x3A,0xAF,0x80,0xF9,0xCA,0x6D,0x48,0xD3,0xA5,0x0C,0x8C,0x2F,0x9C,0xCB,0xE9,0x89,
  0xD2,0x00,0x32,0x26,0x40,0x94,0x61,0x19,0x20,0xE6,0x80,0x6D,0x8A,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
