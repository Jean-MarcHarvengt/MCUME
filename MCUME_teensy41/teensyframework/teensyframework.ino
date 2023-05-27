extern "C" {
  #include "emuapi.h"  
  #include "iopins.h"  
}

#include "t4_dsp.h"
extern T4_DSP tft;

// ****************************************************
// the setup() method runs once, when the sketch starts
// ****************************************************
void setup() {
  emu_init();
}

static unsigned char col = 0; 
// ****************************************************
// the loop() method runs continuously
// ****************************************************
void loop(void) 
{
    char * filename;  
#ifdef FILEBROWSER
    while (true) {      
        if (menuActive()) {
            uint16_t bClick = emu_DebounceLocalKeys();
            int action = handleMenu(bClick);
            filename = menuSelection();   
            if (action == ACTION_RUN1) {
              break;    
            }
            delay(20);
        }
    }
#endif    
    emu_start(20000,nullptr);
    emu_Init(filename);  
    while (true) {
        uint16_t bClick = emu_DebounceLocalKeys();
        emu_Input(bClick);  
        emu_Step();
        tft.fillScreen(RGBVAL16(col++,0x00,0x00));
        emu_DrawVsync();        
    }
}
