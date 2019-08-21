#include "go.h"

extern "C" {
  #include "emuapi.h"
  #include "iopins.h"
}

#include "esp_event.h"

#include "keyboard_osd.h"
#include "ili9341_t3dma.h"
#ifdef HAS_SND
#include "AudioPlaySystem.h"
#endif

#include "Oddemu.h"


ILI9341_t3DMA tft = ILI9341_t3DMA(PIN_NUM_CS, PIN_NUM_DC, -1, PIN_NUM_MOSI, PIN_NUM_CLK, PIN_NUM_MISO, TPIN_NUM_CS, TPIN_NUM_IRQ);
#ifdef HAS_SND
AudioPlaySystem audio;
#endif


static void spi_task(void *args)
{
  while(true) {
    tft.refresh();
  } 
}

static void input_task(void *args)
{
  while(true) {
    if ((emu_ReadKeys() & (MASK_KEY_USER1+MASK_KEY_USER2)) == (MASK_KEY_USER1+MASK_KEY_USER2)) {  
      printf("rebooting\n");
      esp_restart();    
    }

    uint16_t bClick = emu_DebounceLocalKeys();
    if (bClick & MASK_KEY_USER2) { 
      printf("%d\n",emu_SwapJoysticks(1)); 
      emu_SwapJoysticks(0);
    }
    else {
      emu_Input(bClick);
    }
#ifdef HAS_SND      
    audio.step();
#endif  
    vTaskDelay(20 / portTICK_PERIOD_MS);
  } 
}

static void main_step() {
  if (menuActive()) {
    uint16_t bClick = emu_DebounceLocalKeys();
    int action = handleMenu(bClick);
    char * filename = menuSelection();
    if (action == ACTION_RUNTFT) {
#ifdef HAS_SND      
      audio.begin();
      audio.start();
#endif                 
      toggleMenu(false); 
      tft.fillScreenNoDma( RGBVAL16(0x00,0x00,0x00) );
      xTaskCreatePinnedToCore(input_task, "inputthread", 4096, NULL, 2, NULL, 0);
      emu_Init(filename);        
    }       
    //vTaskDelay(20 / portTICK_PERIOD_MS); 
  }
  else {         
    emu_Step();     
  }
}


void setup(void)
{
  printf("Starting emulator\n");
  
	tft.begin();
	tft.flipscreen(true);  
	tft.start();
	tft.refresh();
 
	emu_init(); 

  xTaskCreatePinnedToCore(spi_task, "spithread", 4096, NULL, 1, NULL, 0);
  //vTaskPrioritySet(NULL, tskIDLE_PRIORITY+1);     
}

void loop(void)
{
  unsigned long t = esp_timer_get_time();
  main_step();
  //printf("%d\n",(int)((esp_timer_get_time()-t)/1000));  
} 

