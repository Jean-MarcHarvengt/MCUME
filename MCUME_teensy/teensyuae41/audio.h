 /* 
  * UAE - The Un*x Amiga Emulator
  *
  * Sound emulation stuff
  *
  * Copyright 1995, 1996, 1997 Bernd Schmidt
  */

extern struct audio_channel_data {
    uaecptr lc, pt, dmaen;
    int data_written, snum, state, intreq2, wper, wlen;
    uae_u16 dat, nextdat, vol, per, len;
    int current_sample;
} audio_channel[4];

#ifndef DONT_WANT_SOUND
extern void aud0_handler(void);
extern void aud1_handler(void);
extern void aud2_handler(void);
extern void aud3_handler(void);
#endif

extern void AUDxDAT(int nr, uae_u16 value);

