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
#include "LibFC14/fc14audiodecoder.h"
#include "loader.h"
#endif

#include <rom/rtc.h>
#include <string.h>
#include <math.h>
#include "mcume.h"

#define RTC_SLOW_MEM ((uint32_t*) 0x50000000)       /*!< RTC slow memory, 8k size */

#define ULP_DATA_OFFSET     36

_Static_assert(ULP_DATA_OFFSET < CONFIG_ULP_COPROC_RESERVE_MEM/4 - 6,
        "ULP_DATA_OFFSET is set too high, or CONFIG_ULP_COPROC_RESERVE_MEM is not sufficient");

static inline uint16_t ulp_data_read(size_t offset)
{
    return RTC_SLOW_MEM[ULP_DATA_OFFSET + offset] & 0xffff;
}
static inline void ulp_data_write(size_t offset, uint16_t value)
{
    RTC_SLOW_MEM[ULP_DATA_OFFSET + offset] = value;
}


ILI9341_t3DMA tft = ILI9341_t3DMA(PIN_NUM_CS, PIN_NUM_DC, -1, PIN_NUM_MOSI, PIN_NUM_CLK, PIN_NUM_MISO, TPIN_NUM_CS, TPIN_NUM_IRQ);
#ifdef HAS_SND
AudioPlaySystem audio;
static void *decoder = nullptr;  
#endif


#define MAX_FILENAME_SIZE   18
#define MAX_MENULINES       7
#define TEXT_HEIGHT         16
#define TEXT_WIDTH          8
#define MENU_FILE_XOFFSET   (11*TEXT_WIDTH)
#define MENU_FILE_YOFFSET   (7*TEXT_HEIGHT)
#define MENU_FILE_W         (MAX_FILENAME_SIZE*TEXT_WIDTH)
#define MENU_FILE_H         (MAX_MENULINES*TEXT_HEIGHT)
#define MENU_FILE_FGCOLOR   RGBVAL16(0xff,0xff,0xff)
#define MENU_FILE_BGCOLOR   RGBVAL16(0x00,0x00,0x10)


#define NB_APPS 8

static bool menuRedraw=false;
static int nbFiles=NB_APPS;
static int curFile=0;
static int topFile=0;
static int xOffLogo=0;
static int swipeAngle=0;

static char * apps[NB_APPS] = {
"       Zx81       ",
"   Zx Spectrum    ",
"    Atari 800     ",
"       C64        ",
"    Atari 2600    ",
//"     Odyssey      ",
"       NES        ",
"   Colecovision   ",
"    Atari 5200    "
};


static void initBootMenu(void) {
  menuRedraw=true;  
  tft.fillScreenNoDma(RGBVAL16(0x00,0x00,0x00));
  //tft.drawSpriteNoDma(30,10,(uint16_t*)logo);
  //tft.drawTextNoDma(0,0, TITLE, RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), true);  
}



static int handleBootMenu(unsigned short bClick)
{
  int action = -1;
  xOffLogo = 16*sin((2*3.14*swipeAngle)/256)+30;
  swipeAngle = (swipeAngle + 2)&0xff;
//printf("xOffLogo %d %d\n",xOffLogo,swipeAngle);
  tft.drawSpriteNoDma(xOffLogo,10,(uint16_t*)logo);

  if (bClick & MASK_JOY2_UP) {
    if (curFile!=0) {
      menuRedraw=true;
      curFile--;
    }
  } 
  else if (bClick & MASK_JOY2_DOWN)  {
    if ((curFile<(nbFiles-1)) && (nbFiles)) {
      curFile++;
      menuRedraw=true;
    }
  }   
  else if (bClick & MASK_JOY2_BTN)  {
    action = curFile;
  }
 
  if (menuRedraw && nbFiles) {
    //printf("update\n");
    int fileIndex = 0;
    int appcnt=0;
    tft.drawRectNoDma(MENU_FILE_XOFFSET,MENU_FILE_YOFFSET, MENU_FILE_W, MENU_FILE_H, MENU_FILE_BGCOLOR);
    if (curFile <= (MAX_MENULINES/2-1)) topFile=0;
    else topFile=curFile-(MAX_MENULINES/2);
    //if (curFile <= (MAX_MENULINES-1)) topFile=0;
    //else topFile=curFile-(MAX_MENULINES/2);
    
    int i=0;
    while (i<MAX_MENULINES) {
      if (appcnt == (NB_APPS)) {
        break;
      }     
      char * title=apps[appcnt++];
      if (fileIndex >= topFile) {              
        if ((i+topFile) < nbFiles ) {
          int dx = (MAX_FILENAME_SIZE-strlen(title))*TEXT_WIDTH/2;
          if ((i+topFile)==curFile) {
            tft.drawTextNoDma(MENU_FILE_XOFFSET+dx,i*TEXT_HEIGHT+MENU_FILE_YOFFSET, title, RGBVAL16(0xff,0xff,0x00), RGBVAL16(0xff,0x00,0x00), true);          
          }
          else {
            tft.drawTextNoDma(MENU_FILE_XOFFSET+dx,i*TEXT_HEIGHT+MENU_FILE_YOFFSET, title, MENU_FILE_FGCOLOR, MENU_FILE_BGCOLOR, true);      
          }
        }
        i++; 
      }
      fileIndex++;    
    }
    
    
    menuRedraw=false;     
  }


  return (action);  
}


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
      //emu_Input(bClick);
    }
#ifdef HAS_SND      
    audio.step();
#endif  
    vTaskDelay(20 / portTICK_PERIOD_MS);
  } 
}


static void main_step() {
  uint16_t bClick = emu_DebounceLocalKeys();
  int action = handleBootMenu(bClick);
  if (action >= 0) {  
    printf("launching\n");
    ulp_data_write(0, action); 
    esp_restart();       
  }
#ifdef HAS_SND      
  audio.step();
#endif     
  //vTaskDelay(20 / portTICK_PERIOD_MS); 
}




void setup(void)
{
  printf("App selector\n");   
	
  tft.begin();
	tft.flipscreen(true);  
	tft.start();
	tft.refresh();
 
  emu_InitJoysticks();
  initBootMenu();

  xTaskCreatePinnedToCore(spi_task, "spithread", 4096, NULL, 1, NULL, 0);
  //vTaskPrioritySet(NULL, tskIDLE_PRIORITY+1);     

#ifdef HAS_SND      
  audio.begin();
  audio.start();
  bool haveModule = false;  
  decoder = fc14dec_new();
  haveModule = fc14dec_init(decoder,(void*)musym,sizeof(musym));
  if ( !haveModule ) {
    fc14dec_delete(decoder);
    printf("FC module not supported\n");
  }
  else {
    printf("FC music loaded\n");
    fc14dec_mixer_init(decoder,22050,16,1,0 /*32767*/);

  }     
#endif  
}

void loop(void)
{
  unsigned long t = esp_timer_get_time();
  main_step();
  //printf("%d\n",(int)((esp_timer_get_time()-t)/1000));  
} 


void SND_Process( void* stream, int len) {
#ifdef HAS_SND      
  fc14dec_buffer_fill(decoder,stream,len*2);  
#endif  
}

