#ifndef audioplaysystem_h_
#define audioplaysystem_h_

#ifdef HAS_SND

#include "platform_config.h"

class AudioPlaySystem
{
public:
  AudioPlaySystem(void) { };
  void begin(void);
  void setSampleParameters(float clockfreq, float samplerate);
  void reset(void);
  void start(void);
  void stop(void);
  bool isPlaying(void);
  void sound(int C, int F, int V);    
  void buzz(int size, int val);
  void step(void);
  static void snd_Mixer(short *  stream, int len );  
#ifndef HAS_T4_VGA
  void begin_audio(int samplesize, void (*callback)(short * stream, int len));
  void end_audio();
  static void AUDIO_isr(void);  
  static void SOFTWARE_isr(void);  
#endif

};


#endif

#endif
