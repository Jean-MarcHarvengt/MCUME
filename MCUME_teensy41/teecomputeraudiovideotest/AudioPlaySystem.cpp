#include "platform_config.h"

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


static void snd_Reset(void)
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
//extern "C" {
void SND_Process(void *sndbuffer, int sndn);
//}
#endif


FASTRUN void AudioPlaySystem::snd_Mixer(short *  stream, int len )
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
  
void AudioPlaySystem::begin(void)
{
  this->reset();
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
  //__disable_irq();
  playing = false;  
  //__enable_irq();
}

bool AudioPlaySystem::isPlaying(void) 
{ 
  return playing; 
}



void AudioPlaySystem::sound(int C, int F, int V) {
#ifndef CUSTOM_SND 
  if (C < 6) {
    chan[C].vol = V;
    chan[C].sinc = F>>1; 
  }
#endif  
}

void AudioPlaySystem::step(void) {
}


#ifndef HAS_T4_VGA
/*******************************************************************
 Experimental I2S interrupt based sound driver for PCM51xx !!!
*******************************************************************/

FLASHMEM static void set_audioClock(int nfact, int32_t nmult, uint32_t ndiv, bool force) // sets PLL4
{
  if (!force && (CCM_ANALOG_PLL_AUDIO & CCM_ANALOG_PLL_AUDIO_ENABLE)) return;

  CCM_ANALOG_PLL_AUDIO = CCM_ANALOG_PLL_AUDIO_BYPASS | CCM_ANALOG_PLL_AUDIO_ENABLE
           | CCM_ANALOG_PLL_AUDIO_POST_DIV_SELECT(2) // 2: 1/4; 1: 1/2; 0: 1/1
           | CCM_ANALOG_PLL_AUDIO_DIV_SELECT(nfact);

  CCM_ANALOG_PLL_AUDIO_NUM   = nmult & CCM_ANALOG_PLL_AUDIO_NUM_MASK;
  CCM_ANALOG_PLL_AUDIO_DENOM = ndiv & CCM_ANALOG_PLL_AUDIO_DENOM_MASK;
  
  CCM_ANALOG_PLL_AUDIO &= ~CCM_ANALOG_PLL_AUDIO_POWERDOWN;//Switch on PLL
  while (!(CCM_ANALOG_PLL_AUDIO & CCM_ANALOG_PLL_AUDIO_LOCK)) {}; //Wait for pll-lock
  
  const int div_post_pll = 1; // other values: 2,4
  CCM_ANALOG_MISC2 &= ~(CCM_ANALOG_MISC2_DIV_MSB | CCM_ANALOG_MISC2_DIV_LSB);
  if(div_post_pll>1) CCM_ANALOG_MISC2 |= CCM_ANALOG_MISC2_DIV_LSB;
  if(div_post_pll>3) CCM_ANALOG_MISC2 |= CCM_ANALOG_MISC2_DIV_MSB;
  
  CCM_ANALOG_PLL_AUDIO &= ~CCM_ANALOG_PLL_AUDIO_BYPASS;//Disable Bypass
}

#define AUDIO_SAMPLE_RATE_EXACT  11025.0 //44117.64706 //11025.0 //22050.0 //44117.64706 //31778.0

FLASHMEM static void config_sai1()
{
  CCM_CCGR5 |= CCM_CCGR5_SAI1(CCM_CCGR_ON);
  double fs = AUDIO_SAMPLE_RATE_EXACT;
  // PLL between 27*24 = 648MHz und 54*24=1296MHz
  int n1 = 4; //SAI prescaler 4 => (n1*n2) = multiple of 4
  int n2 = 1 + (24000000 * 27) / (fs * 256 * n1);
  double C = (fs * 256 * n1 * n2) / 24000000;
  int c0 = C;
  int c2 = 10000;
  int c1 = C * c2 - (c0 * c2);

  set_audioClock(c0, c1, c2, true);
  // clear SAI1_CLK register locations
  CCM_CSCMR1 = (CCM_CSCMR1 & ~(CCM_CSCMR1_SAI1_CLK_SEL_MASK))
               | CCM_CSCMR1_SAI1_CLK_SEL(2); // &0x03 // (0,1,2): PLL3PFD0, PLL5, PLL4

  n1 = n1 / 2; //Double Speed for TDM

  CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI1_CLK_PRED_MASK | CCM_CS1CDR_SAI1_CLK_PODF_MASK))
               | CCM_CS1CDR_SAI1_CLK_PRED(n1 - 1) // &0x07
               | CCM_CS1CDR_SAI1_CLK_PODF(n2 - 1); // &0x3f

  IOMUXC_GPR_GPR1 = (IOMUXC_GPR_GPR1 & ~(IOMUXC_GPR_GPR1_SAI1_MCLK1_SEL_MASK))
                    | (IOMUXC_GPR_GPR1_SAI1_MCLK_DIR | IOMUXC_GPR_GPR1_SAI1_MCLK1_SEL(0));  //Select MCLK


  // configure transmitter
  int rsync = 0;
  int tsync = 1;

  I2S1_TMR = 0;
  I2S1_TCR1 = I2S_TCR1_RFW(1);
  I2S1_TCR2 = I2S_TCR2_SYNC(tsync) | I2S_TCR2_BCP // sync=0; tx is async;
        | (I2S_TCR2_BCD | I2S_TCR2_DIV((1)) | I2S_TCR2_MSEL(1));
  I2S1_TCR3 = I2S_TCR3_TCE;
  I2S1_TCR4 = I2S_TCR4_FRSZ((2-1)) | I2S_TCR4_SYWD((32-1)) | I2S_TCR4_MF
        | I2S_TCR4_FSD | I2S_TCR4_FSE | I2S_TCR4_FSP;
  I2S1_TCR5 = I2S_TCR5_WNW((32-1)) | I2S_TCR5_W0W((32-1)) | I2S_TCR5_FBT((32-1));


  I2S1_RMR = 0;
  I2S1_RCR1 = I2S_RCR1_RFW(1);
  I2S1_RCR2 = I2S_RCR2_SYNC(rsync) | I2S_RCR2_BCP  // sync=0; rx is async;
        | (I2S_RCR2_BCD | I2S_RCR2_DIV((1)) | I2S_RCR2_MSEL(1));
  I2S1_RCR3 = I2S_RCR3_RCE;
  I2S1_RCR4 = I2S_RCR4_FRSZ((2-1)) | I2S_RCR4_SYWD((32-1)) | I2S_RCR4_MF
        | I2S_RCR4_FSE | I2S_RCR4_FSP | I2S_RCR4_FSD;
  I2S1_RCR5 = I2S_RCR5_WNW((32-1)) | I2S_RCR5_W0W((32-1)) | I2S_RCR5_FBT((32-1));

  //CORE_PIN23_CONFIG = 3;  // MCLK
  CORE_PIN21_CONFIG = 3;  // RX_BCLK
  CORE_PIN20_CONFIG = 3;  // RX_SYNC
  CORE_PIN7_CONFIG  = 3;  // TX_DATA0
  I2S1_RCSR |= I2S_RCSR_RE | I2S_RCSR_BCE;
  I2S1_TCSR = I2S_TCSR_TE | I2S_TCSR_BCE  | I2S_TCSR_FRDE ;//<-- not using DMA */;
}



//DMAMEM __attribute__((aligned(32))) static uint32_t i2s_tx[1024];

static bool fillfirsthalf = true;
static uint16_t cnt = 0;
static uint16_t sampleBufferSize = 0;

static void (*fillsamples)(short * stream, int len) = nullptr;

static uint32_t * i2s_tx_buffer __attribute__((aligned(32)));
static uint16_t * i2s_tx_buffer16;
static uint16_t * txreg = (uint16_t *)((uint32_t)&I2S1_TDR0 + 2);


FASTRUN void AudioPlaySystem::AUDIO_isr() {
  
  *txreg = i2s_tx_buffer16[cnt]; 
  cnt = cnt + 1;
  cnt = cnt & (sampleBufferSize*2-1);

  if (cnt == 0) {
    fillfirsthalf = false;
    NVIC_SET_PENDING(IRQ_SOFTWARE);
  } 
  else if (cnt == sampleBufferSize) {
    fillfirsthalf = true;
    NVIC_SET_PENDING(IRQ_SOFTWARE);
  }
/*
  I2S1_TDR0 = i2s_tx_buffer[cnt]; 
  cnt = cnt + 1;
  cnt = cnt & (sampleBufferSize-1);
  if (cnt == 0) {
    fillfirsthalf = false;
    NVIC_SET_PENDING(IRQ_SOFTWARE);
  } 
  else if (cnt == sampleBufferSize/2) {
    fillfirsthalf = true;
    NVIC_SET_PENDING(IRQ_SOFTWARE);
  }
*/
}

FASTRUN void AudioPlaySystem::SOFTWARE_isr() {
  //Serial.println("x");
  if (fillfirsthalf) {
    fillsamples((short *)i2s_tx_buffer, sampleBufferSize);
    arm_dcache_flush_delete((void*)i2s_tx_buffer, (sampleBufferSize/2)*sizeof(uint32_t));
  }  
  else { 
    fillsamples((short *)&i2s_tx_buffer[sampleBufferSize/2], sampleBufferSize);
    arm_dcache_flush_delete((void*)&i2s_tx_buffer[sampleBufferSize/2], (sampleBufferSize/2)*sizeof(uint32_t));
  }
}

// display VGA image
FLASHMEM void AudioPlaySystem::begin_audio(int samplesize, void (*callback)(short * stream, int len))
{
  fillsamples = callback;
  i2s_tx_buffer =  (uint32_t*)malloc(samplesize*sizeof(uint32_t)); //&i2s_tx[0];

  if (i2s_tx_buffer == NULL) {
    Serial.println("could not allocate audio samples");
    return;  
  }
  memset((void*)i2s_tx_buffer,0, samplesize*sizeof(uint32_t));
  arm_dcache_flush_delete((void*)i2s_tx_buffer, samplesize*sizeof(uint32_t));
  i2s_tx_buffer16 = (uint16_t*)i2s_tx_buffer;

  sampleBufferSize = samplesize;

  config_sai1();
  attachInterruptVector(IRQ_SAI1, AUDIO_isr);
  NVIC_ENABLE_IRQ(IRQ_SAI1);
  NVIC_SET_PRIORITY(IRQ_QTIMER3, 0);  // 0 highest priority, 255 = lowest priority 
  NVIC_SET_PRIORITY(IRQ_SAI1, 127);
  attachInterruptVector(IRQ_SOFTWARE, SOFTWARE_isr);
  NVIC_SET_PRIORITY(IRQ_SOFTWARE, 208);
  NVIC_ENABLE_IRQ(IRQ_SOFTWARE);

  I2S1_TCSR |= 1<<8;  // start generating TX FIFO interrupts

  Serial.print("Audio sample buffer = ");
  Serial.println(samplesize);
}
 
FLASHMEM void AudioPlaySystem::end_audio()
{
  if (i2s_tx_buffer != NULL) {
    free(i2s_tx_buffer);
  }
}

#endif
#endif
