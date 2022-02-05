extern "C" { 
  #include "iopins.h"  
}


#include "tft_t_dma.h"
TFT_T_DMA tft = TFT_T_DMA(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO, TFT_TOUCH_CS, TFT_TOUCH_INT);

static IntervalTimer myTimer;
volatile boolean vbl=true;
static int skip=0;
static elapsedMicros tius;

static void vblCount() { 
  if (vbl) {
    vbl = false;
  } else {
    vbl = true;
  }
}

#ifdef HAS_SND

#include "AudioPlaySystem.h"

static AudioPlaySystem mymixer;


static void emu_sndInit() {
  Serial.println("sound init");  

  mymixer.begin_audio(256, mymixer.snd_Mixer);  
 // sgtl5000_1.enable();
 // sgtl5000_1.volume(0.6);
  mymixer.start();
}

static void emu_sndPlaySound(int chan, int volume, int freq)
{
  if (chan < 6) {
    mymixer.sound(chan, freq, volume); 
  }
  /*
  Serial.print(chan);
  Serial.print(":" );  
  Serial.print(volume);  
  Serial.print(":" );  
  Serial.println(freq); 
  */ 
}

static void emu_sndPlayBuzz(int size, int val) {
  mymixer.buzz(size,val); 
  //Serial.print((val==1)?1:0); 
  //Serial.print(":"); 
  //Serial.println(size); 
}
#endif

// ****************************************************
// the setup() method runs once, when the sketch starts
// ****************************************************
void setup() {
  tft.begin();
  tft.fillScreenNoDma( RGBVAL16(0x00,0x00,0x00) );
  tft.startDMA(); 
  myTimer.begin(vblCount, 20000);  //to run every 20ms    
#ifdef HAS_SND
  emu_sndInit();
#endif  
}


static uint8_t colcomponent = 0;
static uint8_t notedelay = 0;
static int notes[] = {
  440,
  466,
  494,
  523,
  554,
  587,
  622,
  659,
  698,
  740,
  784,
  831,
  880 };
static int note_pos;

// ****************************************************
// the loop() method runs continuously
// ****************************************************
void loop(void) 
{ 
    tft.fillScreen( RGBVAL16(colcomponent,0x00,0x00) );
    colcomponent += 1;
    colcomponent &= 0xff;
    volatile boolean vb=vbl;
    while (vbl==vb) {};
    notedelay += 1;
    notedelay &= 0x07;
    int note = notes[note_pos];
    emu_sndPlaySound(1, notedelay<<4, note);
    if ( !notedelay )
    {
      note_pos += 1;
      if (note_pos >= sizeof(notes)/sizeof(int) ) 
      { 
        note_pos = 0;
      }
    }
}
