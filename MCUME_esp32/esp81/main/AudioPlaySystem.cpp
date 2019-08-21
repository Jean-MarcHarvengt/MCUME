extern "C" {
#include "emuapi.h"  
}

#ifdef HAS_SND
#include "AudioPlaySystem.h"
#include "esp_system.h"

#define USE_I2S 1

#ifdef USE_I2S
#include "esp_event.h"
#include "driver/i2s.h"
#include "freertos/queue.h"
#include "string.h"
#define I2S_NUM         ((i2s_port_t)0)
//static QueueHandle_t queue;
#else
#include "esp32-hal-timer.h"
#include "esp32-hal-dac.h"
static int32_t LastPlayPos=0;
volatile int32_t NextPlayPos=0;
volatile uint8_t DacPin;   
uint16_t LastDacValue;
hw_timer_t * timer = NULL;
#endif 

volatile uint16_t *Buffer;
volatile uint16_t BufferSize;


static const short square[]={
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

const short noise[] {
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

volatile bool playing = false;


static Channel chan[6] = {
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0} };


static void snd_Reset(void)
{
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
}

#ifdef CUSTOM_SND 
//extern "C" {
void SND_Process(void *sndbuffer, int sndn);
//}
#endif


static void snd_Mixer16(uint16_t *  stream, int len )
{
  if (playing) 
  {
#ifdef CUSTOM_SND 
    SND_Process((void*)stream, len);
#else
    int i;
    long s;  
    //len = len >> 1; 
     
    short v0=chan[0].vol;
    short v1=chan[1].vol;
    short v2=chan[2].vol;
    short v3=chan[3].vol;
    short v4=chan[4].vol;
    short v5=chan[5].vol;
    for (i=0;i<len;i++)
    {
      s = ( v0*(square[(chan[0].spos>>8)&0x3f]) );
      s+= ( v1*(square[(chan[1].spos>>8)&0x3f]) );
      s+= ( v2*(square[(chan[2].spos>>8)&0x3f]) );
      s+= ( v3*(noise[(chan[3].spos>>8)&(NOISEBSIZE-1)]) );
      s+= ( v4*(noise[(chan[4].spos>>8)&(NOISEBSIZE-1)]) );
      s+= ( v5*(noise[(chan[5].spos>>8)&(NOISEBSIZE-1)]) );         
      *stream++ = int16_t((s>>11));      
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

#ifdef USE_I2S
#else
void IRAM_ATTR onTimer() 
{ 
	// Sound playing code, plays whatever's in the buffer continuously. Big change from previous versions
	if(LastDacValue!=Buffer[NextPlayPos])		// Send value to DAC only of changed since last value else no need
	{
		// value to DAC has changed, send to actual hardware, else we just leave setting as is as it's not changed
		LastDacValue=Buffer[NextPlayPos];
		dacWrite(DacPin,uint8_t((LastDacValue>>8)+127));			// write out the data
	}
	Buffer[NextPlayPos]=0;						// Reset this buffer byte back to silence
	NextPlayPos++;								// Move play pos to next byte in buffer
	if(NextPlayPos==BufferSize)					// If gone past end of buffer, 
		NextPlayPos=0;							// set back to beginning
}  

#endif

void AudioPlaySystem::begin(void)
{
#ifdef USE_I2S
	Buffer = (uint16_t *)malloc(DEFAULT_SAMPLESIZE*4); //16bits, L+R
	uint16_t * dst=(uint16_t *)Buffer;
	for (int i=0; i<DEFAULT_SAMPLESIZE; i++) {
	  *dst++=32767;          
    *dst++=32767;          
	};

  i2s_config_t i2s_config;
  i2s_config.mode = (i2s_mode_t)(I2S_MODE_DAC_BUILT_IN|I2S_MODE_TX|I2S_MODE_MASTER);
  i2s_config.sample_rate=DEFAULT_SAMPLERATE;
  i2s_config.bits_per_sample=I2S_BITS_PER_SAMPLE_16BIT;
  i2s_config.communication_format=I2S_COMM_FORMAT_I2S_MSB;
  i2s_config.dma_buf_count = 2;
  i2s_config.dma_buf_len = DEFAULT_SAMPLESIZE;
  i2s_config.use_apll = false;
  i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
	//i2s_driver_install(I2S_NUM, &i2s_config, 4, &queue);
  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM, NULL);
  i2s_set_dac_mode(I2S_DAC_CHANNEL_LEFT_EN); 
	//I2S enables *both* DAC channels; we only need DAC1.
	//ToDo: still needed now I2S supports set_dac_mode?
	//CLEAR_PERI_REG_MASK(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_DAC_XPD_FORCE_M);
	//CLEAR_PERI_REG_MASK(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_XPD_DAC_M);
#else
	BufferSize = DEFAULT_SAMPLESIZE;
	Buffer=(volatile uint16_t *)malloc(BufferSize*2);
	volatile uint16_t * dst=Buffer;
	for (int i=0; i<BufferSize; i++) {
	  *dst++=0;          
	};	

	DacPin=25;								// set dac pin to use
	LastDacValue=0;									// set to mid  point
	dacWrite(DacPin,LastDacValue);					// Set speaker to mid point, stops click at start of first sound
	// Set up interrupt routine
	timer = timerBegin(0, 80, true);        // use timer 0, pre-scaler is 80 (divide by 8000), count up
	timerAttachInterrupt(timer, &onTimer, true); // P3= edge triggered
	timerAlarmWrite(timer, 45, true);       // will trigger 22050 times per sec (443 per 20 ms=22050/50)
	timerAlarmEnable(timer);   	  
#endif  
}

void AudioPlaySystem::start(void)
{ 
  playing = true;  
}

void AudioPlaySystem::setSampleParameters(float clockfreq, float samplerate) {
}

void AudioPlaySystem::reset(void)
{
	snd_Reset();
}

void AudioPlaySystem::stop(void)
{
	playing = false;
#ifdef USE_I2S  	
  i2s_driver_uninstall(I2S_NUM); //stop & destroy i2s driver
#else
#endif   
}

bool AudioPlaySystem::isPlaying(void) 
{ 
  return playing; 
}


void AudioPlaySystem::sound(int C, int F, int V) {
  if (C < 6) {
    //printf("play %d %d %d\n",C,F,V);  

    chan[C].vol = V;
    chan[C].sinc = F>>1; 
  }
}

void AudioPlaySystem::step(void) 
{
#ifdef USE_I2S
  int left=DEFAULT_SAMPLERATE/50;

  while(left) {
    int n=DEFAULT_SAMPLESIZE;
    if (n>left) n=left;
    snd_Mixer16((uint16_t*)Buffer, n);
    //16 bit mono -> 16 bit r+l
    for (int i=n-1; i>=0; i--) {
      Buffer[i*2+1]=Buffer[i]+32767;
      Buffer[i*2]=Buffer[i]+32767;
    }
    i2s_write_bytes(I2S_NUM, (const void*)Buffer, n*4, portMAX_DELAY);
    left-=n;
  }
#else

  int32_t CurPos=NextPlayPos;
  int32_t samples;
  if (CurPos > LastPlayPos) {
  	snd_Mixer16((uint16_t *)&Buffer[LastPlayPos], CurPos-LastPlayPos);
    samples = CurPos-LastPlayPos;
  }
  else {
  	snd_Mixer16((uint16_t *)&Buffer[LastPlayPos], BufferSize-LastPlayPos);
  	snd_Mixer16((uint16_t *)&Buffer[0], CurPos);
    samples = BufferSize-LastPlayPos;
    samples += CurPos;
  }
  LastPlayPos = CurPos;
  //printf("sam %d\n",bytes);
#endif    
}
#endif


