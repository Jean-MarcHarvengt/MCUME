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
#ifndef SN76489_H
#define SN76489_H

#define PSG_BASE     131072    /* Base frequency for SN76489 */

#define PSG_ASYNC    0         /* Asynchronous emulation     */
#define PSG_SYNC     1         /* Synchronous emulation mode */
#define PSG_FLUSH    2         /* Flush buffers only         */

typedef struct
{
  int Channel,Freq[4],Volume[4],Sync;
  unsigned char NoiseMode,Buf,Changed;
  void (*Sound)(int,int,int);
} SN76489;

/** Reset76489() *********************************************/
/** Reset the sound chip. The user has to provide a pointer **/
/** to a function Sound(Channel,Freq,Volume) used to make   **/
/** actual sound.                                           **/
/*************************************************************/
void Reset76489(register SN76489 *D,void (*Sound)(int,int,int));

/** Sync76489() **********************************************/
/** Flush all accumulated changes by issuing Sound() calls, **/
/** and set the synchronization on/off. The second argument **/
/** should be PSG_SYNC, PSG_ASYNC to set/reset sync, or     **/
/** PSG_FLUSH to leave sync mode as it is.                  **/
/*************************************************************/
void Sync76489(register SN76489 *D,register unsigned char Sync);

/** Write76489() *********************************************/
/** Call this function to output a value V into the sound   **/
/** chip.                                                   **/
/*************************************************************/
void Write76489(register SN76489 *D,register unsigned char V);

#endif /* SN76489_H */
