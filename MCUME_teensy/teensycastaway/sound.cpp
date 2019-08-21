#include "dcastaway.h"

#ifndef NO_SOUND

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<Arduino.h>

#include "st.h"
#include "mem.h"
#include "m68k_intrf.h"
#include "sound.h"

#define LONGLONG long long

#define ENVELOPE_PERIOD(Fine,Coarse)  (((unsigned long)Coarse)<<8) + (unsigned long)Fine
#define NOISE_PERIOD(Freq)            ((((unsigned long)Freq)&0x1f)<<11)
#define TONE_PERIOD(Fine,Coarse)      ((((unsigned long)Coarse)&0x0f)<<8) + (unsigned long)Fine
#define MIXTABLE_SIZE    (256*8)        /* Large table, so don't overflow */
#define TONEFREQ_SHIFT   28             /* 4.28 fixed point */
#define NOISEFREQ_SHIFT  28             /* 4.28 fixed point */
#define ENVFREQ_SHIFT    16             /* 16.16 fixed */

#define SAMPLES_BUFFER_SIZE  1024
/* Number of generated samples per frame (eg. 44Khz=882) : */
#define SAMPLES_PER_FRAME  ((SOUND_FREQ*2)/50) //((SOUND_FREQ+35)/50)
/* Frequency of generated samples: */
#define SAMPLES_FREQ   (SOUND_FREQ)
#define YM_FREQ        (2000000/SAMPLES_FREQ)      /* YM Frequency 2Mhz */

/* Original wave samples */
/* Shape x Length(repeat 3rd/4th entries) */
#include "tab_EnvelopeShapeValues.h"
//static const int EnvelopeShapeValues[16*1024] = { 1,2 };                        
/* Use table to convert from (A+B+C) to clipped 'unsigned char' for sound buffer */
/* -ve and +ve range */
#include "tab_MixTable.h"
//static const char MixTable[MIXTABLE_SIZE] = { 1,2 };                             
static const char *pMixTable = &MixTable[MIXTABLE_SIZE/2];            /* Signed index into above */
/* LogTable */
#include "tab_LogTable.h"
//static const int LogTable[256] = { 1,2 }; 
#include "tab_LogTable16.h"
//static const int LogTable16[16] = { 1,2 }; 
static const int *pEnvelopeLogTable = &LogTable[128];

/* Current sample for this time period */
static int Envelope[SAMPLES_BUFFER_SIZE];   
static int Noise[SAMPLES_BUFFER_SIZE];
char MixBuffer[MIXBUFFER_SIZE];



/* Frequency and time period samples */
static unsigned long ChannelFreq[3], EnvelopeFreq, NoiseFreq;   /* Current frequency of each channel A,B,C,Envelope and Noise */
static int ChannelAmpDecayTime[3];                              /* Store counter to show if amplitude is changed to generate 'samples' */
/* Output channel data */
static int Channel_A_Buffer[SAMPLES_BUFFER_SIZE],Channel_B_Buffer[SAMPLES_BUFFER_SIZE],Channel_C_Buffer[SAMPLES_BUFFER_SIZE];
static int ActiveSndBufIdx;                                     /* Current working index into above mix buffer */
static int nSamplesToGenerate;                                  /* How many samples are needed for this time-frame */

/* global values */
bool bWriteEnvelopeFreq;                                        /* Did write to register '13' - causes frequency reset */
bool bWriteChannelAAmp, bWriteChannelBAmp, bWriteChannelCAmp;   /* Did write to amplitude registers? */
bool bEnvelopeFreqFlag;                                         /* As above, but cleared each frame for YM saving */
/* Buffer to store circular samples */
int nGeneratedSamples;                                          /* Generated samples since audio buffer update */
int SoundCycles;


/*-----------------------------------------------------------------------*/
/* Envelope shape table */
typedef struct
{
  int WaveStart[4], WaveDelta[4];
} ENVSHAPE;


/* Square wave look up table */
PROGMEM static const int SquareWave[16] = { 127,127,127,127,127,127,127,127, -128,-128,-128,-128,-128,-128,-128,-128 };


static long RandomNum;

static __inline__ long Misc_NextLongRand(long Seed)
{
  unsigned long Lo, Hi;

  Lo = 16807 * (long)(Seed & 0xffff);
  Hi = 16807 * (long)((unsigned long)Seed >> 16);
  Lo += (Hi & 0x7fff) << 16;
  if (Lo > 2147483647L) {
    Lo &= 2147483647L;
    ++Lo;
  }
  Lo += Hi >> 15;
  if (Lo > 2147483647L) {
    Lo &= 2147483647L;
    ++Lo;
  }
  return((long)Lo);
}

static __inline__ long Misc_GetRandom(void)
{
  RandomNum = Misc_NextLongRand(RandomNum);
  if (!RandomNum)
  {
    RandomNum++;
    return 0;
  }
  return(RandomNum);
}


/*-----------------------------------------------------------------------*/
/*
  Init sound tables and envelopes
*/
void Sound_Init(void)
{
  Sound_Reset();
}


/*-----------------------------------------------------------------------*/
/*
  Reset the sound emulation
*/
void Sound_Reset(void)
{
  int i;

  Sound_ClearMixBuffer();       /* Clear buffer */

  /* Clear cycle counts, buffer index and register '13' flags */
  SoundCycles = 0;
  bEnvelopeFreqFlag = FALSE;
  bWriteEnvelopeFreq = FALSE;
  bWriteChannelAAmp = bWriteChannelBAmp = bWriteChannelCAmp = FALSE;

  /* Lock audio system before accessing variables that are also use by the callback function! */
  Audio_Lock();
  CompleteSndBufIdx = 0;
  ActiveSndBufIdx =  (SOUND_BUFFER_SIZE + SAMPLES_PER_FRAME) % MIXBUFFER_SIZE;
  nGeneratedSamples = 0;
  Audio_Unlock();

  /* Clear frequency counter */
  for(i=0; i<3; i++)
  {
    ChannelFreq[i] =
    ChannelAmpDecayTime[i] = 0;
  }
  EnvelopeFreq = NoiseFreq = 0;
}


/*-----------------------------------------------------------------------*/
/*
  Clear mixer buffer, where samples are stored ready to pass to sound player
*/
void Sound_ClearMixBuffer(void)
{
  Audio_Lock();

  Memory_Clear(MixBuffer, MIXBUFFER_SIZE);      /* Clear buffer */

  Audio_Unlock();
}


/*-----------------------------------------------------------------------*/
/*
  Find how many samples to generate and store in 'nSamplesToGenerate'
  Also update 'SoundCycles' to store how many we actually did so generates set amount each frame
*/
PROGMEM static void Sound_SetSamplesPassed(void)
{
  int nSampleCycles;
  int nSamplesPerFrame;
  int Dec=1;

  /* Check how many cycles have passed, as we use this to help find out if we are playing sample data */

  /* First, add decay to channel amplitude variables */
  if (SoundCycles>(CYCLES_PER_FRAME/4))
    Dec = 16;                            /* Been long time between sound writes, must be normal tone sound */

  if (!bWriteChannelAAmp)                /* Not written to amplitude, decay value */
  {
    ChannelAmpDecayTime[0]-=Dec;
    if (ChannelAmpDecayTime[0]<0)  ChannelAmpDecayTime[0] = 0;
  }
  if (!bWriteChannelBAmp)
  {
    ChannelAmpDecayTime[1]-=Dec;
    if (ChannelAmpDecayTime[1]<0)  ChannelAmpDecayTime[1] = 0;
  }
  if (!bWriteChannelCAmp)
  {
    ChannelAmpDecayTime[2]-=Dec;
    if (ChannelAmpDecayTime[2]<0)  ChannelAmpDecayTime[2] = 0;
  }

  /* 160256 cycles per VBL, 44Khz = 882 samples per VBL */
  /* 882/160256 samples per clock cycle */
  nSamplesPerFrame = SAMPLES_PER_FRAME;
#if 0  /* Use floats for calculation */
  nSamplesToGenerate = (int)( (float)SoundCycles * ((float)nSamplesPerFrame/(float)CYCLES_PER_FRAME) );
  if (nSamplesToGenerate > nSamplesPerFrame)
    nSamplesToGenerate = nSamplesPerFrame;

  nSampleCycles = (int)( (float)nSamplesToGenerate / ((float)nSamplesPerFrame/(float)CYCLES_PER_FRAME) );
  SoundCycles -= nSampleCycles;
#else  /* Use integers for calculation - both of these calculations should fit into 32-bit int */
  nSamplesToGenerate = SoundCycles * nSamplesPerFrame / CYCLES_PER_FRAME;
//printf("nSamplesToGenerate=%i , SoundCycles=%i\n",nSamplesToGenerate,SoundCycles);
  if (nSamplesToGenerate > nSamplesPerFrame)
    nSamplesToGenerate = nSamplesPerFrame;

  nSampleCycles = nSamplesToGenerate * CYCLES_PER_FRAME / nSamplesPerFrame;
  SoundCycles -= nSampleCycles;
#endif
}


/*-----------------------------------------------------------------------*/
/*
  Generate envelope wave for this time-frame
*/
static void Sound_GenerateEnvelope(unsigned char EnvShape, unsigned char Fine, unsigned char Coarse)
{
  const int *pEnvelopeValues;
  unsigned long EnvelopePeriod,EnvelopeFreqDelta;
  int i;

  /* Find envelope details */
  if (bWriteEnvelopeFreq)
    EnvelopeFreq = 0;
  pEnvelopeValues = &EnvelopeShapeValues[ (EnvShape&0x0f)*1024 ];          /* Envelope shape values */
  EnvelopePeriod = ENVELOPE_PERIOD((unsigned long)Fine,(unsigned long)Coarse);

  if (EnvelopePeriod==0)                                                   /* Handle div by zero */
    EnvelopeFreqDelta = 0;
  else
    EnvelopeFreqDelta = ((LONGLONG)YM_FREQ<<ENVFREQ_SHIFT) / (EnvelopePeriod);  /* 16.16 fixed point */

  /* Create envelope from current shape and frequency */
  for(i=0; i<nSamplesToGenerate; i++)
  {
    Envelope[i] = pEnvelopeValues[EnvelopeFreq>>ENVFREQ_SHIFT];           /* Store envelope wave, already applied 'log' function */
    EnvelopeFreq += EnvelopeFreqDelta;
    if (EnvelopeFreq&0xfe000000)
      EnvelopeFreq = 0x02000000 | (EnvelopeFreq&0x01ffffff);              /* Keep in range 512-1024 once past 511! */
  }
}


/*-----------------------------------------------------------------------*/
/*
  Generate nosie for this time-frame
*/
static void Sound_GenerateNoise(unsigned char MixerControl, unsigned char NoiseGen)
{
  int NoiseValue;
  unsigned long NoisePeriod,NoiseFreqDelta;
  int i;

  NoisePeriod = NOISE_PERIOD((unsigned long)NoiseGen);

  if (NoisePeriod==0)                                            /* Handle div by zero */
    NoiseFreqDelta = 0;
  else
    NoiseFreqDelta = (((LONGLONG)YM_FREQ)<<NOISEFREQ_SHIFT) / NoisePeriod;  /* 4.28 fixed point */

  /* Generate noise samples */
  for(i=0; i<nSamplesToGenerate; i++)
  {
    NoiseValue = (unsigned int)Misc_GetRandom()%96;              /* Get random value */
    if (SquareWave[NoiseFreq>>NOISEFREQ_SHIFT]<=0)               /* Add to square wave at given frequency */
      NoiseValue = -NoiseValue;

    Noise[i] = NoiseValue;
    NoiseFreq += NoiseFreqDelta;
  }
}


/*-----------------------------------------------------------------------*/
/*
  Generate channel of samples for this time-frame
*/
static void Sound_GenerateChannel(int *pBuffer, unsigned char ToneFine, unsigned char ToneCoarse,unsigned char Amplitude,unsigned char MixerControl,unsigned long *pChannelFreq,int MixMask)
{   
  int *pNoise = Noise, *pEnvelope = Envelope;
  unsigned long ToneFreq=*pChannelFreq;
  unsigned long TonePeriod;
  unsigned long ToneFreqDelta;
  int i,Amp,Mix;
  int ToneOutput,NoiseOutput,MixerOutput,EnvelopeOutput,AmplitudeOutput;

  TonePeriod = TONE_PERIOD((unsigned long)ToneFine,(unsigned long)ToneCoarse);
  /* Find frequency of channel */
  if (TonePeriod==0)
    ToneFreqDelta = 0;                                  /* Handle div by zero */
  else
    ToneFreqDelta = (((LONGLONG)YM_FREQ)<<TONEFREQ_SHIFT) / TonePeriod;    /* 4.28 fixed point */
  Amp = LogTable16[(Amplitude&0x0f)];
  Mix = (MixerControl>>MixMask)&9;                      /* Read I/O Mixer */

  /* Check if we are trying to play a 'sample' - we need to up the volume on these as they tend to be rather quiet */
  if ((Amplitude&0x10)==0)                /* Fixed level amplitude? */
  {
    ChannelAmpDecayTime[MixMask]++;       /* Increment counter to find out if we are playing samples... */
    if (ChannelAmpDecayTime[MixMask]>16)
      ChannelAmpDecayTime[MixMask] = 16;  /* And limit */
  }

  for(i=0; i<nSamplesToGenerate; i++)
  {
    /* Output from Tone Generator(0-255) */
    ToneOutput = SquareWave[ToneFreq>>TONEFREQ_SHIFT];

    /* Output from Noise Generator(0-255) */
    NoiseOutput = *pNoise++; 
    /* Output from Mixer(combines Tone+Noise) */
    switch (Mix) {
      case 0:    /* Has Noise and Tone */
        MixerOutput = NoiseOutput+ToneOutput;
        break;
      case 1:    /* Has Noise */
        MixerOutput = NoiseOutput;
        break;
      case 8:    /* Has Tone */
        MixerOutput = ToneOutput;
        break;

      default:  /* This is used to emulate samples - should give no output, but ST gives set tone!!?? */
        /* MixerControl gets set to give a continuous tone and then then Amplitude */
        /* of channels A,B and C get changed with all other registers in the PSG */
        /* staying as zero's. This produces the sounds from Quartet, Speech, NoiseTracker etc...! */
        MixerOutput = 127;
    }

    EnvelopeOutput = pEnvelopeLogTable[*pEnvelope++];

    if ((Amplitude&0x10)==0)
    {
      AmplitudeOutput = Amp;          /* Fixed level amplitude */

      /* As with most emulators, sample playback is always 'quiet'. We check to see if */
      /* the amplitude of a channel is repeatedly changing and when this is detected we */
      /* scale the volume accordingly */
      if (ChannelAmpDecayTime[MixMask]>8)
        AmplitudeOutput <<= 1;        /* Scale up by a factor of 2 */
    }
    else
      AmplitudeOutput = EnvelopeOutput;

    *pBuffer++ = (MixerOutput*AmplitudeOutput)>>8;

    ToneFreq+=ToneFreqDelta;
  }

  /* Store back incremented frequency, for next call */
  *pChannelFreq = ToneFreq;
}


/*-----------------------------------------------------------------------*/
/*
  Generate samples for all channels during this time-frame
*/
static void Sound_GenerateSamples(void)
{
  int *pChannelA=Channel_A_Buffer, *pChannelB=Channel_B_Buffer, *pChannelC=Channel_C_Buffer;
  int i;

  /* Anything to do? */
  if (nSamplesToGenerate>0)
  {
    /* Generate envelope/noise samples for this time */
    Sound_GenerateEnvelope(psg[PSG_REG_ENV_SHAPE],psg[PSG_REG_ENV_FINE],psg[PSG_REG_ENV_COARSE]);
    Sound_GenerateNoise(psg[PSG_REG_MIXER_CONTROL],psg[PSG_REG_NOISE_GENERATOR]);

    /* Generate 3 channels, store to separate buffer so can mix/clip */
    Sound_GenerateChannel(pChannelA,psg[PSG_REG_CHANNEL_A_FINE],psg[PSG_REG_CHANNEL_A_COARSE],psg[PSG_REG_CHANNEL_A_AMP],psg[PSG_REG_MIXER_CONTROL],&ChannelFreq[0],0); 
    Sound_GenerateChannel(pChannelB,psg[PSG_REG_CHANNEL_B_FINE],psg[PSG_REG_CHANNEL_B_COARSE],psg[PSG_REG_CHANNEL_B_AMP],psg[PSG_REG_MIXER_CONTROL],&ChannelFreq[1],1);
    Sound_GenerateChannel(pChannelC,psg[PSG_REG_CHANNEL_C_FINE],psg[PSG_REG_CHANNEL_C_COARSE],psg[PSG_REG_CHANNEL_C_AMP],psg[PSG_REG_MIXER_CONTROL],&ChannelFreq[2],2);

    /* Mix channels together, using table to clip and also convert to 'unsigned char' */
    for(i=0; i<nSamplesToGenerate; i++)
      MixBuffer[(i+ActiveSndBufIdx)%MIXBUFFER_SIZE] = pMixTable[(*pChannelA++) + (*pChannelB++) + (*pChannelC++)];

    ActiveSndBufIdx = (ActiveSndBufIdx + nSamplesToGenerate) % MIXBUFFER_SIZE;
    nGeneratedSamples += nSamplesToGenerate;

    /* Reset the write to register '13' flag */
    bWriteEnvelopeFreq = FALSE;
    /* And amplitude write flags */
    bWriteChannelAAmp = bWriteChannelBAmp = bWriteChannelCAmp = FALSE;
  }
}


/*-----------------------------------------------------------------------*/
/*
  This is called to built samples up until this clock cycle
*/
void Sound_Update(void)
{
  int OldSndBufIdx = ActiveSndBufIdx;


  /* Find how many to generate */
  Sound_SetSamplesPassed();
  /* And generate */
  Sound_GenerateSamples();


}


/*-----------------------------------------------------------------------*/
/*
  On each VBL (50fps) complete samples.
*/
void Sound_Update_VBL(void)
{
  //Sound_Update();

  /* Clear write to register '13', used for YM file saving */
  bEnvelopeFreqFlag = FALSE;
}


/*-----------------------------------------------------------------------*/
/*
  This is called from the audio callback function to create enough samples
  to fill the current sound buffer.
*/
void Sound_UpdateFromAudioCallBack(void)
{
  /* If there are already enough samples or if we are recording, we should
   * not generate more samples here! */
  if(nGeneratedSamples >= SOUND_BUFFER_SIZE)
    return;

  nSamplesToGenerate = SOUND_BUFFER_SIZE - nGeneratedSamples;

  Sound_GenerateSamples();
}

void Sound_UpdateFromCallBack16(short *pBuffer, int len)
{
  int *pChannelA=Channel_A_Buffer, *pChannelB=Channel_B_Buffer, *pChannelC=Channel_C_Buffer;
  int i;
  nSamplesToGenerate = len;

  /* Generate envelope/noise samples for this time */
  Sound_GenerateEnvelope(psg[PSG_REG_ENV_SHAPE],psg[PSG_REG_ENV_FINE],psg[PSG_REG_ENV_COARSE]);
  Sound_GenerateNoise(psg[PSG_REG_MIXER_CONTROL],psg[PSG_REG_NOISE_GENERATOR]);

  /* Generate 3 channels, store to separate buffer so can mix/clip */
  Sound_GenerateChannel(pChannelA,psg[PSG_REG_CHANNEL_A_FINE],psg[PSG_REG_CHANNEL_A_COARSE],psg[PSG_REG_CHANNEL_A_AMP],psg[PSG_REG_MIXER_CONTROL],&ChannelFreq[0],0); 
  Sound_GenerateChannel(pChannelB,psg[PSG_REG_CHANNEL_B_FINE],psg[PSG_REG_CHANNEL_B_COARSE],psg[PSG_REG_CHANNEL_B_AMP],psg[PSG_REG_MIXER_CONTROL],&ChannelFreq[1],1);
  Sound_GenerateChannel(pChannelC,psg[PSG_REG_CHANNEL_C_FINE],psg[PSG_REG_CHANNEL_C_COARSE],psg[PSG_REG_CHANNEL_C_AMP],psg[PSG_REG_MIXER_CONTROL],&ChannelFreq[2],2);

  /* Mix channels together, using table to clip and also convert to 'unsigned char' */
  for(i=0; i<len; i++) {
    short s = ((*pChannelA++) +(*pChannelB++) + (*pChannelC++))<<4; 
    *pBuffer++ = s; 
    *pBuffer++ = s;
    //char s = pMixTable[(*pChannelA++) + (*pChannelB++) + (*pChannelC++)];
    //*pBuffer++ = ((short)s << 8);
    //*pBuffer++ = ((short)s << 8);
  }

  /* Reset the write to register '13' flag */
  bWriteEnvelopeFreq = FALSE;
  /* And amplitude write flags */
  bWriteChannelAAmp = bWriteChannelBAmp = bWriteChannelCAmp = FALSE;
}

#else
#warning NO_SOUND
#warning NO_SOUND
#warning NO_SOUND
#warning NO_SOUND
#warning NO_SOUND
#warning NO_SOUND
#endif
