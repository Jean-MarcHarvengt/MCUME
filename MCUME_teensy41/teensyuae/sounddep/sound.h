 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * Support for Linux/USS sound
  * 
  * Copyright 1997 Bernd Schmidt
  */


#define sndbufsize 4096 //1024 //8192 //2048 //2048

extern int sound_fd;
extern uae_u16 sndbuffer[];
extern uae_u32 sndbufpt;


static __inline__ void check_sound_buffers (void)
{
    if (sndbufpt >= sndbufsize) {
	   //write (sound_fd, sndbuffer, sndbufsize);
	   sndbufpt = 0;
    }
}


#define PUT_SOUND_WORD(b) do { sndbuffer[sndbufpt] = b; sndbufpt +=1; } while (0)
#define SOUND16_BASE_VAL 0

#define DEFAULT_SOUND_MAXB 8192
#define DEFAULT_SOUND_MINB 8192
#define DEFAULT_SOUND_BITS 16
#define DEFAULT_SOUND_FREQ 44100

#undef DONT_WANT_SOUND
