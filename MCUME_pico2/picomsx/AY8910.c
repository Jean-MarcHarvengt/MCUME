/** EMULib Emulation Library *********************************/
/**                                                         **/
/**                         AY8910.c                        **/
/**                                                         **/
/** This file contains emulation for the AY8910 sound chip  **/
/** produced by General Instruments, Yamaha, etc. See       **/
/** AY8910.h for declarations.                              **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1996-2005                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/

#include "AY8910.h"
#include "Sound.h"
#include <string.h>

/** Reset8910() **********************************************/
/** Reset the sound chip and use sound channels from the    **/
/** one given in First.                                     **/
/*************************************************************/
void Reset8910(register AY8910 *D,int First)
{
  static byte RegInit[16] =
  {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFD,
    0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00
  };
  int J;

  for(J=0;J<AY8910_CHANNELS;J++) D->Freq[J]=D->Volume[J]=0;
  memcpy(D->R,RegInit,sizeof(D->R));
  D->Phase[0]=D->Phase[1]=D->Phase[2]=0;
  D->First   = First;
  D->Sync    = AY8910_ASYNC;
  D->Changed = 0x00;
  D->EPeriod = 0;
  D->ECount  = 0;
  D->Latch   = 0x00;

  /* Set sound types */
  SetSound(0+First,SND_MELODIC);
  SetSound(1+First,SND_MELODIC);
  SetSound(2+First,SND_MELODIC);
  SetSound(3+First,SND_NOISE);
  SetSound(4+First,SND_NOISE);
  SetSound(5+First,SND_NOISE);
}

/** WrCtrl8910() *********************************************/
/** Write a value V to the PSG Control Port.                **/
/*************************************************************/
void WrCtrl8910(AY8910 *D,byte V)
{
  D->Latch=V&0x0F;
}

/** WrData8910() *********************************************/
/** Write a value V to the PSG Data Port.                   **/
/*************************************************************/
void WrData8910(AY8910 *D,byte V)
{
  Write8910(D,D->Latch,V);
}

/** RdData8910() *********************************************/
/** Read a value from the PSG Data Port.                    **/
/*************************************************************/
byte RdData8910(AY8910 *D)
{
  return(D->R[D->Latch]);
}

/** Write8910() **********************************************/
/** Call this function to output a value V into the sound   **/
/** chip.                                                   **/
/*************************************************************/
void Write8910(register AY8910 *D,register byte R,register byte V)
{
  register int J,I;

  /* Exit if no change */
  if((R>15)||((V==D->R[R])&&(R<8)&&(R>10))) return;

  switch(R)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      /* Write value */
      D->R[R]=V;
      /* Exit if the channel is silenced */
      if(D->R[7]&(1<<(R>>1))) return;
      /* Go to the first register in the pair */
      R&=0xFE;
      /* Compute frequency */
      J=((int)(D->R[R+1]&0x0F)<<8)+D->R[R];
      /* Compute channel number */
      R>>=1;
      /* Assign frequency */
      D->Freq[R]=AY8910_BASE/(J+1);
      /* Compute changed channels mask */
      D->Changed|=1<<R;
      break;

    case 6:
      /* Write value */
      D->R[6]=V;
      /* Exit if noise channels are silenced */
      if(!(~D->R[7]&0x38)) return;
      /* Compute and assign noise frequency */
      J=AY8910_BASE/((V&0x1F)+1);
      if(!(D->R[7]&0x08)) D->Freq[3]=J;
      if(!(D->R[7]&0x10)) D->Freq[4]=J;
      if(!(D->R[7]&0x20)) D->Freq[5]=J;
      /* Compute changed channels mask */
      D->Changed|=0x38&~D->R[7];
      break;

    case 7:
      /* Find changed channels */
      R=(V^D->R[7])&0x3F;
      D->Changed|=R;
      /* Write value */
      D->R[7]=V;
      /* Update frequencies */
      for(J=0;R&&(J<AY8910_CHANNELS);J++,R>>=1,V>>=1)
        if(R&1)
        {
          if(V&1) D->Freq[J]=0;
          else
          {
            I=J<3? (((int)(D->R[J*2+1]&0x0F)<<8)+D->R[J*2]):(D->R[6]&0x1F);
            D->Freq[J]=AY8910_BASE/(I+1);
          }
        }
      break;
      
    case 8:
    case 9:
    case 10:
      /* Write value */
      D->R[R]=V;
      /* Compute channel number */
      R-=8;
      /* Compute and assign new volume */
      D->Volume[R+3]=D->Volume[R]=
        V&0x10? (V&0x04? 0:255):255*(V&0x0F)/15;
      /* Start/stop envelope */
      D->Phase[R]=V&0x10? 1:0;
      /* Compute changed channels mask */
      D->Changed|=(0x09<<R)&~D->R[7];
      break;

    case 11:
    case 12:
      /* Write value */
      D->R[R]=V;
      /* Compute frequency */
      J=((int)D->R[12]<<8)+D->R[11]+1;
      D->EPeriod=1000*(J<<4)/AY8910_BASE;
      D->ECount=0;
      /* No channels changed */
      return;

    case 13:
    case 14:
    case 15:
      /* Write value */
      D->R[R]=V;
      /* No channels changed */
      return;

    default:
      /* Wrong register, do nothing */
      return;
  }

  /* For asynchronous mode, make Sound() calls right away */
  if(!D->Sync&&D->Changed) Sync8910(D,AY8910_FLUSH);
}

/** Loop8910() ***********************************************/
/** Call this function periodically to update volume        **/
/** envelopes. Use mS to pass the time since the last call  **/
/** of Loop8910() in milliseconds.                          **/
/*************************************************************/
void Loop8910(register AY8910 *D,int mS)
{
  register int J,I,Step;

  /* Exit if no envelope running */
  if(!D->EPeriod) return;

  /* Count milliseconds */
  D->ECount+=mS;
  if(D->ECount<D->EPeriod) return;

  /* By how much do we change volume? */
  Step=(D->ECount<<8)/D->EPeriod;

  /* Count the remaining milliseconds */
  D->ECount%=D->EPeriod;

  for(J=0;J<3;J++)
    if(D->Phase[J]&&(D->R[J+8]&0x10))
      switch(D->R[13]&0x0F)
      {
        case 0:
        case 1:
        case 2:
        case 3:
        case 9:
          I=D->Volume[J]-Step;
          D->Volume[J+3]=D->Volume[J]=I=I>=0? I:0;
          if(!I) D->Phase[J]=0;
          D->Changed|=(0x09<<J)&~D->R[7];
          break;
        case 4:
        case 5:
        case 6:
        case 7:
          I=D->Volume[J]+Step;
          D->Volume[J+3]=D->Volume[J]=I=I<256? I:0;
          if(!I) D->Phase[J]=0;
          D->Changed|=(0x09<<J)&~D->R[7];
          break;
        case 8:
          I=D->Volume[J]-Step;
          D->Volume[J+3]=D->Volume[J]=I>=0? I:255;
          D->Changed|=(0x09<<J)&~D->R[7];
          break;
        case 10:
          if(D->Phase[J]==2)
          {
            I=D->Volume[J]+Step;
            D->Volume[J+3]=D->Volume[J]=I=I<256? I:255;
            if(I==255) D->Phase[J]=1;
          }
          else
          {
            I=D->Volume[J]-Step;
            D->Volume[J+3]=D->Volume[J]=I=I>=0? I:0;
            if(!I) D->Phase[J]=2;
          }
          D->Changed|=(0x09<<J)&~D->R[7];
          break;
        case 11:
          I=D->Volume[J]-Step;
          D->Volume[J+3]=D->Volume[J]=I=I>=0? I:255;
          if(I==255) D->Phase[J]=0;
          D->Changed|=(0x09<<J)&~D->R[7];
          break;
        case 12:
          I=D->Volume[J]+Step;
          D->Volume[J+3]=D->Volume[J]=I<256? I:0;
          D->Changed|=(0x09<<J)&~D->R[7];
          break; 
        case 13:
        case 15:
          I=D->Volume[J]+Step;
          D->Volume[J+3]=D->Volume[J]=I<256? I:255;
          if(I==255) D->Phase[J]=0;
          D->Changed|=(0x09<<J)&~D->R[7];
          break;
        case 14:
          if(D->Phase[J]==2)
          {
            I=D->Volume[J]-Step;
            D->Volume[J+3]=D->Volume[J]=I=I>=0? I:0;
            if(!I) D->Phase[J]=1;
          }
          else
          {
            I=D->Volume[J]+Step;
            D->Volume[J+3]=D->Volume[J]=I=I<256? I:255;
            if(I==255) D->Phase[J]=2;
          }
          D->Changed|=(0x09<<J)&~D->R[7];
          break;
      }

  /* For asynchronous mode, make Sound() calls right away */
  if(!D->Sync&&D->Changed) Sync8910(D,AY8910_FLUSH);
}

/** Sync8910() ***********************************************/
/** Flush all accumulated changes by issuing Sound() calls  **/
/** and set the synchronization on/off. The second argument **/
/** should be AY8910_SYNC/AY8910_ASYNC to set/reset sync,   **/
/** or AY8910_FLUSH to leave sync mode as it is. To emulate **/
/** noise channels with MIDI drums, OR second argument with **/
/** AY8910_DRUMS.                                           **/
/*************************************************************/
void Sync8910(register AY8910 *D,register byte Sync)
{
  register int J,I;

  /* Hit MIDI drums for noise channels, if requested */
  if(Sync&AY8910_DRUMS)
  {
    Sync&=~AY8910_DRUMS;
    J=0;
    if(D->Volume[3]&&D->Freq[3]) J+=D->Volume[3];
    if(D->Volume[4]&&D->Freq[4]) J+=D->Volume[4];
    if(D->Volume[5]&&D->Freq[5]) J+=D->Volume[5];
    if(J) Drum(DRM_MIDI|28,(J+2)/3);
  }

  if(Sync!=AY8910_FLUSH) D->Sync=Sync;

  for(J=0,I=D->Changed;I&&(J<AY8910_CHANNELS);J++,I>>=1)
    if(I&1) Sound(J+D->First,D->Freq[J],D->Volume[J]);

  D->Changed=0x00;
}
