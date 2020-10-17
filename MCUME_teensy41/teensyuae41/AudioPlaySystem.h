#ifndef audioplaysystem_h_
#define audioplaysystem_h_

#ifdef HAS_SND

#include "platform_config.h"

//#undef HAS_T4_VGA // To force using Audio library!!

#ifndef HAS_T4_VGA

#include <Audio.h>

class AudioPlaySystem : public AudioStream
{
public:
  AudioPlaySystem(void) : AudioStream(0, NULL) { begin(); }
  void begin(void);
  void setSampleParameters(float clockfreq, float samplerate);
  void reset(void);
  void start(void);
  void stop(void);
  bool isPlaying(void);
  void sound(int C, int F, int V);    
  void buzz(int size, int val);
  void step(void);  
    
private:
  virtual void update(void);
  static void snd_Mixer(short *  stream, int len );   
};

#else
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
};

#endif

#endif

#endif
