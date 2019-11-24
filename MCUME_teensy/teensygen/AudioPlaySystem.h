#ifndef audioplaysystem_h_
#define audioplaysystem_h_

#ifdef HAS_SND

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
};
#endif

#endif

