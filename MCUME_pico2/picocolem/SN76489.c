/** EMULib Emulation Library *********************************/
/**                                                         **/
/**                        SN76489.c                        **/
/**                                                         **/
/** This file contains emulation for the SN76489 sound chip **/
/** produced by Intel.                                      **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1996-1998                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/

#include "SN76489.h"

/** Reset76489() *********************************************/
/** Reset the sound chip. The user has to provide a pointer **/
/** to a function Sound(Channel,Freq,Volume) used to make   **/
/** actual sound.                                           **/
/*************************************************************/
void Reset76489(SN76489 *D,void (*Sound)(int,int,int))
{
  register int J;

  for(J=0;J<4;J++) D->Volume[J]=D->Freq[J]=0;
  D->NoiseMode=D->Buf=D->Changed=0x00;
  D->Sync=PSG_ASYNC;D->Sound=Sound;
}

/** Sync76489() **********************************************/
/** Flush all accumulated changes by issuing Sound() calls, **/
/** and set the synchronization on/off. The second argument **/
/** should be PSG_SYNC, PSG_ASYNC to set/reset sync, or     **/
/** PSG_FLUSH to leave sync mode as it is.                  **/
/*************************************************************/
void Sync76489(SN76489 *D,unsigned char Sync)
{
  register int J,I;

  if(D->Sync&&D->Changed)
  {
    for(J=0,I=1;J<4;J++,I<<=1)
      if(D->Changed&I)
        D->Sound(J,D->Freq[J],D->Volume[J]);
  }
  D->Changed=0x00;
  if(Sync!=PSG_FLUSH) D->Sync=Sync;
}

/** Write76489() *********************************************/
/** Call this function to output a value V into the sound   **/
/** chip.                                                   **/
/*************************************************************/
void Write76489(SN76489 *D,unsigned char V)
{
  register unsigned char N,J;
  register long L;

  switch(V&0xF0)
  {
    case 0xE0:
      J=V&0x03;
      if(J==D->NoiseMode) return;
      switch(J)
      {
        case 0: D->Freq[3]=20000;break;
        case 1: D->Freq[3]=10000;break;
        case 2: D->Freq[3]=5000;break;
        case 3: D->Freq[3]=D->Freq[2];break;
      }
      N=3;break;
    case 0x80: case 0xA0: case 0xC0:
      D->Buf=V;return;
    case 0x90: case 0xB0: case 0xD0: case 0xF0:
      N=(V-0x90)>>5;
      J=(~V&0x0F)*16;
      if(J==D->Volume[N]) return;
      D->Volume[N]=J;
      break;
    default:
      if(!(D->Buf&0xC0)) return;
      N=(D->Buf-0x80)>>5;
      L=PSG_BASE/((V&0x3F)*16+(D->Buf&0x0F)+1);
      if(L>15000) L=0;
      if(L==D->Freq[N]) return;
      if((N==2)&&(D->NoiseMode==3))
      {
        D->Freq[3]=L;
        if(D->Sync) D->Changed|=0x08;
        else D->Sound(3,D->Freq[3],D->Volume[3]);
      }
      D->Freq[N]=L;
      break;
  }

  if(D->Sync) D->Changed|=1<<N;
  else D->Sound(N,D->Freq[N],D->Volume[N]);
}
