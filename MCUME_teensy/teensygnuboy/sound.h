

#ifndef __SOUND_H__
#define __SOUND_H__


struct sndchan
{
	int on;
	unsigned pos;
	int cnt, encnt, swcnt;
	int len, enlen, swlen;
	int swfreq;
	int freq;
	int envol, endir;
};


struct snd
{
	int rate;
	struct sndchan ch[4];
	byte wave[16];
};


extern struct snd snd;


extern void sound_reset(int rate);
extern void audio_play_sample(int16 *bufl, int16 *bufr, int length);




#endif


