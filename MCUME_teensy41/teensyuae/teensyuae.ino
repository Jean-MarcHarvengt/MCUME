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
    char * floppy1 = menuSelection();   
    if (action == ACTION_RUN1) {     
      emu_start(16666,nullptr);
      emu_Init(floppy1,floppy2);
      emu_Init2();       
    }  
    else if (action == ACTION_RUN2)  {
      strcpy(floppy2,menuSecondSelection());                        
    }      
    delay(20);
  }
  else {
    emu_Step();
    uint16_t bClick = 0; //emu_DebounceLocalKeys();
    emu_Input(bClick);     
  }
}

/*
void setup() {

#ifdef HAS_T4_VGA
#ifdef HIRES
  tft.begin(VGA_MODE_640x480);
#else
  tft.begin(VGA_MODE_320x240);
#endif
#else
  tft.begin();
#endif  
  
  emu_init(); 

  
  myTimer.begin(vblCount, 17000);  //to run every 16ms  
}


void loop(void) 
{
  static char floppy2[64]="";
  if (menuActive()) {
    uint16_t bClick = emu_DebounceLocalKeys();
    int action = handleMenu(bClick);
    char * floppy1 = menuSelection();
    if (action == ACTION_RUN1) {
      toggleMenu(false);
      vgaMode = false;   
      tft.fillScreenNoDma( RGBVAL16(0x00,0x00,0x00) );
      emu_start();
      emu_Init(floppy1,floppy2);      
      tft.startDMA(); 
      emu_Init2();      
    }    
    else if (action == ACTION_RUN2)  {
      strcpy(floppy2,menuSecondSelection());                        
    }         
    delay(20);
  }
  else {
    emu_Step();  
    uint16_t bClick = 0; //emu_DebounceLocalKeys();
    emu_Input(bClick);      
  }  
}
*/
