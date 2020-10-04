#include "emuapi.h"

#ifdef HAS_SND

#include "AudioPlaySystem.h"
#include <Arduino.h>
#define SAMPLERATE AUDIO_SAMPLE_RATE_EXACT
#define CLOCKFREQ 985248

#ifndef CUSTOM_SND 
PROGMEM static const short square[]={
32767,32767,32767,32767,
32767,32767,32767,32767,
32767,32767,32767,32767,
32767,32767,32767,32767,
32767,32767,32767,32767,
32767,32767,32767,32767,
32767,32767,32767,32767,
32767,32767,32767,32767,
-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,
};

PROGMEM const short noise[] {
-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,32767,-32767,
-32767,-32767,32767,-32767,-32767,-32767,32767,-32767,-32767,-32767,32767,-32767,-32767,-32767,32767,-32767,
-32767,-32767,32767,-32767,-32767,-32767,32767,-32767,-32767,-32767,32767,-32767,-32767,32767,32767,-32767,
-32767,-32767,32767,-32767,-32767,32767,32767,-32767,-32767,-32767,32767,-32767,-32767,32767,32767,-32767,
-32767,-32767,32767,-32767,-32767,32767,32767,-32767,-32767,-32767,32767,-32767,32767,32767,32767,-32767,
32767,-32767,32767,-32767,-32767,32767,32767,-32767,-32767,-32767,32767,-32767,32767,32767,32767,-32767,
32767,-32767,32767,-32767,-32767,32767,32767,-32767,-32767,-32767,32767,32767,32767,32767,32767,-32767,
32767,-32767,32767,-32767,-32767,32767,32767,-32767,-32767,-32767,32767,32767,32767,32767,32767,-32767,
32767,-32767,32767,-32767,-32767,32767,32767,-32767,-32767,-32767,-32767,32767,32767,32767,-32767,-32767,
32767,-32767,-32767,-32767,-32767,32767,-32767,-32767,-32767,-32767,32767,32767,32767,32767,32767,-32767,
32767,-32767,32767,-32767,-32767,32767,32767,-32767,-32767,32767,-32767,32767,32767,32767,-32767,-32767,
32767,32767,-32767,-32767,-32767,32767,-32767,-32767,-32767,-32767,32767,32767,32767,32767,32767,-32767,
32767,-32767,32767,-32767,-32767,32767,32767,-32767,32767,32767,-32767,32767,-32767,32767,-32767,-32767,
32767,32767,-32767,-32767,-32767,32767,-32767,-32767,-32767,-32767,32767,32767,32767,32767,32767,-32767,
32767,-32767,32767,-32767,-32767,32767,32767,32767,32767,32767,-32767,32767,-32767,32767,-32767,-32767,
};

#define NOISEBSIZE 0x100

typedef struct
{
  unsigned int spos;
  unsigned int sinc;
  unsigned int vol;
} Channel;

static Channel chan[6] = {
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0} };
  
#endif

volatile bool playing = false;




PROGMEM static void snd_Reset(void)
{
#ifndef CUSTOM_SND 
  chan[0].vol = 0;
  chan[1].vol = 0;
  chan[2].vol = 0;
  chan[3].vol = 0;
  chan[4].vol = 0;
  chan[5].vol = 0;
  chan[0].sinc = 0;
  chan[1].sinc = 0;
  chan[2].sinc = 0;
  chan[3].sinc = 0;
  chan[4].sinc = 0;
  chan[5].sinc = 0;
#endif
}


#ifdef CUSTOM_SND 
void SND_Process(void *sndbuffer, int sndn);
#endif

PROGMEM static void snd_Mixer(short *  stream, int len )
{
  if (playing) 
  {
#ifdef CUSTOM_SND 
    SND_Process((void*)stream, len);
#else
    int i;
    long s;  
    len = len >> 1; 
     
    short v0=chan[0].vol;
    short v1=chan[1].vol;
    short v2=chan[2].vol;
    short v3=chan[3].vol;
    short v4=chan[4].vol;
    short v5=chan[5].vol;
    for (i=0;i<len;i++)
    {
      s =((v0*square[(chan[0].spos>>8)&0x3f])>>11);
      s+=((v1*square[(chan[1].spos>>8)&0x3f])>>11);
      s+=((v2*square[(chan[2].spos>>8)&0x3f])>>11);
      s+=((v3*noise[(chan[3].spos>>8)&(NOISEBSIZE-1)])>>11);
      s+=((v4*noise[(chan[4].spos>>8)&(NOISEBSIZE-1)])>>11);
      s+=((v5*noise[(chan[5].spos>>8)&(NOISEBSIZE-1)])>>11);         
      *stream++ = (short)(s);
      *stream++ = (short)(s);
      chan[0].spos += chan[0].sinc;
      chan[1].spos += chan[1].sinc;
      chan[2].spos += chan[2].sinc;
      chan[3].spos += chan[3].sinc;  
      chan[4].spos += chan[4].sinc;  
      chan[5].spos += chan[5].sinc;  
    }
#endif         
  }
}
  
PROGMEM void AudioPlaySystem::begin(void)
{
  //emu_printf("AudioPlaySystem constructor");
	this->reset();
	setSampleParameters(CLOCKFREQ, SAMPLERATE);
}

PROGMEM void AudioPlaySystem::start(void)
{
  //emu_printf("allocating sound buf");
  playing = true;  
}

PROGMEM void AudioPlaySystem::setSampleParameters(float clockfreq, float samplerate) {
}

PROGMEM void AudioPlaySystem::reset(void)
{
	snd_Reset();
}

PROGMEM void AudioPlaySystem::stop(void)
{
	//__disable_irq();
	playing = false;	
	//__enable_irq();
}

PROGMEM bool AudioPlaySystem::isPlaying(void) 
{ 
  return playing; 
}

PROGMEM void AudioPlaySystem::update(void) {
	audio_block_t *block;

	// only update if we're playing
	if (!playing) return;

	// allocate the audio blocks to transmit
	block = allocate();
	if (block == NULL) return;

  snd_Mixer((short*)block->data,AUDIO_BLOCK_SAMPLES);
  //memset( (void*)block->data, 0, AUDIO_BLOCK_SAMPLES*2);

	transmit(block);
	release(block);
}

PROGMEM void AudioPlaySystem::sound(int C, int F, int V) {
#ifndef CUSTOM_SND 
  if (C < 6) {
    chan[C].vol = V;
    chan[C].sinc = F>>1; 
  }
#endif  
}

PROGMEM void AudioPlaySystem::step(void) {
}
#endif

