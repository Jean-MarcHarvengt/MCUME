#ifndef __AUDIO_H
#define __AUDIO_H

void update_audio(void);
void init_audio(void);
void close_audio(void);
void init_sound_stream(void);
void mute_audio(void);
#ifdef GP32
void audio_process(unsigned short *buffer, int len);
#else
void audio_process(short *buffer, int len);
#endif
extern int sound_IRQ;

#endif


