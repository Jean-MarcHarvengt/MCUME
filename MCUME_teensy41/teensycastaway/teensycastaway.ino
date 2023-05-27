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
  emu_init(SUPPORT_HIRES);
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
static char floppy2[MAX_FILENAME_PATH]="";
  if (menuActive()) {
    uint16_t bClick = emu_DebounceLocalKeys();
    int action = handleMenu(bClick);
    char * floppy1 = menuSelection();
    static char floppy2[MAX_FILENAME_PATH]="";
    if (action == ACTION_RUN1) {     
      emu_start(20000,vbl);
      emu_Init(floppy1,floppy2, (((bClick&MASK_KEY_USER2)&&(emu_IsVgaHires()))?1:0) );
    }
    else if (action == ACTION_RUN2) {
      strcpy(floppy2,menuSecondSelection());                        
    }    
    delay(20);
  }
  else {
    emu_Step();          
  }  
}
