#ifndef X2600_SOUND_H
#define X2600_SOUND_H

/* $Id: sound.h,v 1.4 1996/09/04 22:37:48 ahornby Exp $ */

extern void
sound_init(void);

extern void
sound_close(void);

extern void 
sound_freq(int channel, BYTE freq);

extern void
sound_volume(int channel, BYTE vol);

//extern void
//	  Update_tia_sound(0x15 + channel, value);

//sound_waveform(int channel, BYTE value);

extern void
sound_update(void);

#endif
