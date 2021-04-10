
/*
 *   O2EM Freeware Odyssey2 / Videopac+ Emulator
 *
 *   Created by Daniel Boris <dboris@comcast.net>  (c) 1997,1998
 *
 *   Developed by Andre de la Rocha <adlroc@users.sourceforge.net>
 *
 *   http://o2em.sourceforge.net
 *
 *
 *
 *   O2 audio emulation
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "types.h"
#include "config.h"
#include "vmachine.h"
#include "audio.h"

//#define SAMPLE_RATE 44100
#define PERIOD1 11  //11
#define PERIOD2 44 //44

#define SOUND_BUFFER_LEN 1056

#define AUD_CTRL  0xAA
#define AUD_D0	  0xA7
#define AUD_D1	  0xA8
#define AUD_D2	  0xA9


int sound_IRQ;
//static short *stream=NULL;
//static AUDIOSTREAM *stream=NULL;
//FILE *sndlog=NULL;

static double flt_a=0.0, flt_b=0.0;
static unsigned char flt_prv = 0;


static void filter(unsigned char *buf, unsigned long len);


//void audio_process(unsigned char *buffer){
#ifdef GP32
void audio_process(unsigned short *buffer, int len)
#else
void audio_process(short *buffer, int len)
#endif
{
	unsigned long aud_data;
#ifdef GP32
	unsigned short s;
#else
	short s;
#endif
	int volume, re_circ, noise, enabled, intena, period, pnt, cnt, rndbit, pos;
	
	aud_data = (VDCwrite[AUD_D2] | (VDCwrite[AUD_D1] << 8) | (VDCwrite[AUD_D0] << 16));

	intena = VDCwrite[0xA0] & 0x04;

	pnt = cnt = 0;

	noise = VDCwrite[AUD_CTRL] & 0x10;
	enabled = VDCwrite[AUD_CTRL] & 0x80;
	rndbit = (enabled && noise) ? (rand()%2) : 0;

	while (pnt < len/*SOUND_BUFFER_LEN*/) {
		pos = (tweakedaudio) ? (pnt/3) : (MAXLINES-1);
		volume = AudioVector[pos] & 0x0F;
		enabled = AudioVector[pos] & 0x80;
		period = (AudioVector[pos] & 0x20) ? PERIOD1 : PERIOD2;
		re_circ = AudioVector[pos] & 0x40;
		
#ifdef GP32
		s= ( (enabled) ? ((aud_data & 0x01)^rndbit) * (0x10 * volume) : 0) << 8;
#else
		s= (( (enabled) ? ((aud_data & 0x01)^rndbit) * (0x10 * volume) : 0) - 128) << 8 ;
#endif
		*buffer++ = s; 
		*buffer++ = s;
		pnt++;
		cnt++;

		if (cnt >= period) {
			cnt=0;
			aud_data = (re_circ) ? ((aud_data >> 1) | ((aud_data & 1) << 23)) : (aud_data >>= 1);
			rndbit = (enabled && noise) ? (rand()%2) : 0;

			if (enabled && intena && (!sound_IRQ)) {
				sound_IRQ = 1;
				ext_IRQ();
			}		
		}
	}
	
//	if (app_data.filter) filter(buffer, SOUND_BUFFER_LEN);	
}


void update_audio(void) {
//	unsigned char *p;
	if (app_data.sound_en) {
//		p = (unsigned char *)get_audio_stream_buffer(stream);
//		if (p) {
//			audio_process(p);
//			if (sndlog) fwrite(p,1,SOUND_BUFFER_LEN,sndlog);
//			free_audio_stream_buffer(stream);
//		}
   	}
}


void init_audio(void) {
//	int i;

	sound_IRQ=0;		
//	set_volume(255,255);
	init_sound_stream();

//	sndlog = NULL;
}


void init_sound_stream(void){
	int vol;
	if (app_data.sound_en){
		if (app_data.filter)
			vol = (255*app_data.svolume)/100;
		else
			vol = (255*app_data.svolume)/200;
//		stream = play_audio_stream(SOUND_BUFFER_LEN,8,0,SAMPLE_RATE,vol,128);
//		if (!stream) {
//			printf("Error creating audio stream!\n");
//			app_data.sound_en=0;
//		}
		flt_a = flt_b = 0.0;
		flt_prv = 0;
	}
}


void mute_audio(void){
	if (app_data.sound_en /*&& stream*/){
//		stop_audio_stream(stream);
//		stream=NULL;
	}
}


void close_audio(void) {
	if (app_data.sound_en /*&& stream*/) {
//		stop_audio_stream(stream);
	}
	app_data.sound_en=0;
}


static void filter(unsigned char *buffer, unsigned long len){    
	static unsigned char buf[SOUND_BUFFER_LEN];
	int t;
	unsigned long i;
	if (len>SOUND_BUFFER_LEN) return;
	memcpy(buf,buffer,len);	
	for (i=0; i<len; i++){
		t = (i==0)?(buf[0]-flt_prv):(buf[i]-buf[i-1]);
		if (t) flt_b = (double)t;
		flt_a += flt_b/4.0 - flt_a/80.0;
		flt_b -= flt_b/4.0;
		if ((flt_a>255.0)||(flt_a<-255.0)) flt_a=0.0;
		buffer[i] = (unsigned char)((flt_a+255.0)/2.0);
	}
	flt_prv = buf[len-1];
}


