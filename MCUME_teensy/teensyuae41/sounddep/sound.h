 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * Support for Linux/USS sound
  * 
  * Copyright 1997 Bernd Schmidt
  */


#define sndbufsize 8192

extern int sound_fd;
extern uae_u16 sndbuffer[];
extern uae_u16 *sndbufpt;


static __inline__ void check_sound_buffers (void)
{
    if (sndbufpt - sndbuffer >= sndbufsize) {
	   //write (sound_fd, sndbuffer, sndbufsize);
	   sndbufpt = sndbuffer;
    }
}


#define PUT_SOUND_BYTE(b) do { *(uae_u8 *)sndbufpt = b; sndbufpt = (uae_u16 *)(((uae_u8 *)sndbufpt) + 1); } while (0)
#define PUT_SOUND_WORD(b) do { *(uae_u16 *)sndbufpt = b; sndbufpt = (uae_u16 *)(((uae_u8 *)sndbufpt) + 2); } while (0)
#define SOUND16_BASE_VAL 0
#define SOUND8_BASE_VAL 128

#define DEFAULT_SOUND_MAXB 8192
#define DEFAULT_SOUND_MINB 8192
#define DEFAULT_SOUND_BITS 16
#define DEFAULT_SOUND_FREQ 44100

#undef DONT_WANT_SOUND
