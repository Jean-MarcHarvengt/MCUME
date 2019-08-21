#ifndef DCASTAWAY_SOUND_H
#define DCASTAWAY_SOUND_H

#ifndef NO_SOUND

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "config.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MUSIC_VOLUME 64
#ifdef DINGOO
#define SOUND_FREQ 16000
#else
#define SOUND_FREQ 11025 //22050
#endif

#define SOUND_BUFFER_SIZE 1024

void Sound_Reset(void);
void Sound_ClearMixBuffer(void);
void Audio_EnableAudio(bool bEnable);

enum
{
  FREQ_11Khz,
  FREQ_22Khz,
  FREQ_44Khz
};

#define MIXBUFFER_SIZE    8192
#define SCREEN_START_CYCLE  96          /* Cycle first normal pixel appears on */
#define SCANLINES_PER_FRAME 313         /* Number of scan lines per frame */
#define CYCLES_PER_LINE     512         /* Cycles per horiztonal line scan */
#define CYCLES_VBL_IN       (SCREEN_START_HBL*CYCLES_PER_LINE)     /* ((28+64)*CYCLES_PER_LINE) */
#define CYCLES_PER_FRAME    (SCANLINES_PER_FRAME*CYCLES_PER_LINE)  /* Cycles per VBL @ 50fps = 160256 */
#define CYCLES_PER_SEC      (CYCLES_PER_FRAME*50) /* Cycles per second */
#define CYCLES_ENDLINE      (64+320+88+40)        /* DE(Display Enable) */
#define CYCLES_HBL          (CYCLES_PER_LINE+96)  /* Cycles for first HBL - very inaccurate on ST */
#define CYCLES_DEBUGGER     3000        /* Check debugger every 'x' cycles */

enum {
  PSG_REG_CHANNEL_A_FINE,     // 0x0000
  PSG_REG_CHANNEL_A_COARSE,   // 0x0001
  PSG_REG_CHANNEL_B_FINE,     // 0x0010
  PSG_REG_CHANNEL_B_COARSE,   // 0x0011
  PSG_REG_CHANNEL_C_FINE,     // 0x0100
  PSG_REG_CHANNEL_C_COARSE,   // 0x0101
  PSG_REG_NOISE_GENERATOR,    // 0x0110
  PSG_REG_MIXER_CONTROL,      // 0x0111
  PSG_REG_CHANNEL_A_AMP,      // 0x1000
  PSG_REG_CHANNEL_B_AMP,      // 0x1001
  PSG_REG_CHANNEL_C_AMP,      // 0x1010
  PSG_REG_ENV_FINE,           // 0x1011
  PSG_REG_ENV_COARSE,         // 0x1100
  PSG_REG_ENV_SHAPE,          // 0x1101
  PSG_REG_IO_PORTA,           // 0x1110
  PSG_REG_IO_PORTB            // 0x1111
};

#define NUM_PSG_SOUND_REGISTERS    14  // Number of register, not including IO ports

extern bool bSoundWorking;                /* Is sound OK */
extern volatile bool bPlayingBuffer;     /* Is playing buffer? */
extern int OutputAudioFreqIndex;    /* Playback rate (11Khz,22Khz or 44Khz) */
extern int SoundBufferSize;               /* Size of sound buffer */
extern int CompleteSndBufIdx;                    /* Replay-index into MixBuffer */

extern int SoundPlayBackFrequencies[];


extern bool bWriteEnvelopeFreq,bWriteChannelAAmp,bWriteChannelBAmp,bWriteChannelCAmp;
extern bool bEnvelopeFreqFlag;
extern char MixBuffer[MIXBUFFER_SIZE];
extern int SoundCycles;
extern int nGeneratedSamples;

static __inline__ int Misc_LimitInt(int Value, int MinRange, int MaxRange)
{
  if (Value<MinRange)
    Value = MinRange;
  else if (Value>MaxRange)
    Value = MaxRange;

  return(Value);
}

#define Memory_Clear(A,B) (memset(A,0,B))


#define Audio_Lock()
#define Audio_Unlock()

void Sound_UpdateFromCallBack16(short *pBuffer, int len);

void Sound_Update_VBL(void);
void Sound_UpdateFromAudioCallBack(void);
void Sound_Init(void);
void Sound_Update(void);
void audio_init(void);
void audio_stop(void);
void audio_init_music(void);
void audio_play_wait(void);
void audio_play_click(void);
void audio_play_file(void);
void audio_play_main(void);
void audio_play_error(void);
void audio_play_goodbye(void);
void audio_play_save(void);

#else
/* NO SOUND */
#define Sound_Update_VBL() 
#define Sound_Update()
#define audio_init_music()
#define audio_play_wait()
#define audio_play_click()
#define audio_play_file()
#define audio_play_main()
#define audio_play_error()
#define audio_play_goodbye()
#define audio_play_save()
#define audio_init()
#define audio_stop()

#endif

#endif
