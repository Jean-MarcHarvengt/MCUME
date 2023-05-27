extern "C" {
  #include "iopins.h"  
  #include "emuapi.h"  
}
#include "t4_dsp.h"
extern T4_DSP tft;

// ****************************************************
// the setup() method runs once, when the sketch starts
// ****************************************************
void setup() {
  emu_init();
}

static void vbl(void) {
    uint16_t bClick = emu_DebounceLocalKeys();
    emu_Input(bClick);
}

// ****************************************************
// the loop() method runs continuously
// ****************************************************
void loop(void) 
{
  if (menuActive()) {
    uint16_t bClick = emu_DebounceLocalKeys();
    int action = handleMenu(bClick);
    char * filename = menuSelection();    
    if (action == ACTION_RUN1) {     
      emu_start(16666,nullptr);
      emu_Init(filename);       
    }    
    delay(20);
  }
  else {
      uint16_t bClick = emu_DebounceLocalKeys();
      emu_Input(bClick);  
      emu_Step();              
  }  
}
