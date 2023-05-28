#include "emuapi.h"  
#include "iopins.h"  

#include "t4_dsp.h"
extern T4_DSP tft;

// ****************************************************
// the setup() method runs once, when the sketch starts
// ****************************************************
void setup() {
  emu_init(SUPPORT_HIRES);
}

// ****************************************************
// the loop() method runs continuously
// ****************************************************
static char floppy2[64]="";
void loop(void) 
{
  if (menuActive()) {
    uint16_t bClick = emu_DebounceLocalKeys();
    int action = handleMenu(bClick);
    char * image = menuSelection();   
    if (action == ACTION_RUN1) {     
      emu_start(16666,nullptr);
      emu_Init(image);    
    }     
    delay(20);
  }
  else {
    uint16_t bClick = emu_DebounceLocalKeys();
    emu_Input(bClick);     
    emu_Step();    
  }
}
