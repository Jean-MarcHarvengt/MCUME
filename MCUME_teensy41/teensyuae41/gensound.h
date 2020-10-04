 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Prototypes for general sound related functions
  * This use to be called sound.h, but that causes confusion
  *
  * Copyright 1997 Bernd Schmidt
  */

extern int sound_available;

/* Determine if we can produce any sound at all. */
extern int setup_sound (void);

extern int init_sound (void);
extern void close_sound (void);

extern void sample16_handler (void);
extern void sample8_handler (void);
extern void sample_ulaw_handler (void);
extern void init_sound_table16(void);
extern void init_sound_table8(void);

