#include "emuapi.h"  
#include "iopins.h"  

#include "t4_dsp.h"
extern T4_DSP tft;

// ****************************************************
// the setup() method runs once, when the sketch starts
// ****************************************************
void setup() {
  emu_init();
  emu_start(20000,nullptr);  
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
    emu_DrawVsync();
    notedelay += 1;
    notedelay &= 0x07;
    int note = notes[note_pos];
#ifdef HAS_SND    
    emu_sndPlaySound(1, notedelay<<4, note);
#endif    
    if ( !notedelay )
    {
      note_pos += 1;
      if (note_pos >= sizeof(notes)/sizeof(int) ) 
      { 
        note_pos = 0;
      }
    }
}
