#define KEYMAP_PRESENT 1

extern "C" {
  #include "emuapi.h"
  #include "iopins.h"
}

#ifdef HAS_T4_VGA
#include "vga_t_dma.h"
const uint16_t deflogo[] = {
  0,0
};
static const uint16_t * logo = deflogo;
#else
#include "tft_t_dma.h"
#include "logo.h"
#endif

#include "bmpjoy.h"
#include "bmpvbar.h"
#ifdef OLD_LAYOUT    
#include "bmpvga.h"
#include "bmptft.h"
#endif


#ifdef HAS_I2CKBD
#include <Wire.h>
#endif

#ifdef HAS_USBKEY
#include "USBHost_t36.h"  // Read this header first for key info
USBHost myusb;
USBHub hub1(myusb);
KeyboardController keyboard1(myusb);
#endif
static uint8_t usbnavpad=0;

#ifdef USE_SDFS
#include "uSDFS.h"
static FATFS fatfs;
static FIL file; 
#else

#ifdef USE_SDFAT
#include "SdFat.h"
#include "sdios.h"

#ifndef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SS;

#else  // SDCARD_SS_PIN
// Assume built-in SD is used.
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif  // SDCARD_SS_PIN

#define SD_CONFIG SdioConfig(FIFO_SDIO)

// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI)
#endif  // HAS_SDIO_CLASS
//------------------------------------------------------------------------------

#if SD_FAT_TYPE == 0
#elif SD_FAT_TYPE == 1
#define SdFat SdFat32
#define File File32
#elif SD_FAT_TYPE == 2
#define SdFat SdExFat
#define File ExFile
#elif SD_FAT_TYPE == 3
#define SdFat SdFs
#define File FsFile
#endif  // SD_FAT_TYPE
static SdFat SD;
#else
#include <SD.h>
#endif
static File file;
#endif

#define CALIBRATION_FILE    "/cal.cfg"

#define MKEY_L1             1
#define MKEY_L2             2
#define MKEY_L3             3
#define MKEY_L4             4
#define MKEY_L5             5
#define MKEY_L6             6
#define MKEY_L7             7
#define MKEY_L8             8
#define MKEY_L9             9
#define MKEY_UP             20
#define MKEY_DOWN           21
#define MKEY_JOY            22
#define MKEY_TFT            23
#define MKEY_VGA            24

#define MAX_FILES           64
#define MAX_FILENAME_SIZE   24
#define MAX_MENULINES       (MKEY_L9)
#define TEXT_HEIGHT         16
#define TEXT_WIDTH          8
#define MENU_FILE_XOFFSET   (6*TEXT_WIDTH)
#define MENU_FILE_YOFFSET   (2*TEXT_HEIGHT)
#define MENU_FILE_W         (MAX_FILENAME_SIZE*TEXT_WIDTH)
#define MENU_FILE_H         (MAX_MENULINES*TEXT_HEIGHT)
#define MENU_FILE_BGCOLOR   RGBVAL16(0x00,0x00,0x40)
#define MENU_JOYS_YOFFSET   (12*TEXT_HEIGHT)
#define MENU_VBAR_XOFFSET   (0*TEXT_WIDTH)
#define MENU_VBAR_YOFFSET   (MENU_FILE_YOFFSET)

#define MENU_TFT_XOFFSET    (MENU_FILE_XOFFSET+MENU_FILE_W+8)
#define MENU_TFT_YOFFSET    (MENU_VBAR_YOFFSET+32)
#define MENU_VGA_XOFFSET    (MENU_FILE_XOFFSET+MENU_FILE_W+8)
#define MENU_VGA_YOFFSET    (MENU_VBAR_YOFFSET+MENU_FILE_H-32-37)

extern TFT_T_DMA tft;

static char romspath[64];
static int nbFiles=0;
static int curFile=0;
static int topFile=0;
static char selection[MAX_FILENAME_SIZE+1]="";
static char files[MAX_FILES][MAX_FILENAME_SIZE];
static bool menuRedraw=true;

static int calMinX=-1,calMinY=-1,calMaxX=-1,calMaxY=-1;
static bool i2cKeyboardPresent = false;
static unsigned short * keys;
static int keyMap;

const unsigned short menutouchareas[] = {
  TAREA_XY,MENU_FILE_XOFFSET,MENU_FILE_YOFFSET,
  TAREA_WH,MENU_FILE_W, TEXT_HEIGHT,
  TAREA_NEW_COL,TEXT_HEIGHT,TEXT_HEIGHT,TEXT_HEIGHT,TEXT_HEIGHT,TEXT_HEIGHT,TEXT_HEIGHT,TEXT_HEIGHT,TEXT_HEIGHT,TEXT_HEIGHT,
  
  TAREA_XY,MENU_VBAR_XOFFSET,MENU_VBAR_YOFFSET,
  TAREA_WH,32,48,
  TAREA_NEW_COL, 72,72,8,40,

  TAREA_XY,MENU_TFT_XOFFSET,MENU_TFT_YOFFSET,
  TAREA_WH,32,37,
  TAREA_NEW_COL, 38,38,
    
  TAREA_END};
   
const unsigned short menutouchactions[] = {
  MKEY_L1,MKEY_L2,MKEY_L3,MKEY_L4,MKEY_L5,MKEY_L6,MKEY_L7,MKEY_L8,MKEY_L9,
  MKEY_UP,MKEY_DOWN,ACTION_NONE,MKEY_JOY,
  MKEY_TFT,MKEY_VGA}; 


static int keypadval=0; 
static boolean joySwapped = false;
static uint16_t bLastState;
static int xRef;
static int yRef;
static uint8_t prev_zt=0; 

static bool menuOn=true;
static bool callibrationOn=false;
static int callibrationStep=0;


static char captureTouchZone(const unsigned short * areas, const unsigned short * actions, int *rx, int *ry, int *rw, int * rh) {
    uint16_t xt=0;
    uint16_t yt=0;
    uint16_t zt=0;
    boolean hDir=true;  
  
    if (tft.isTouching())
    {
        if (prev_zt == 0) {
            prev_zt =1;
            tft.readCal(&xt,&yt,&zt);
            if (zt<1000) {
              prev_zt=0; 
              return ACTION_NONE;
            }   
            int i=0;
            int k=0;
            int y2=0, y1=0;
            int x2=0, x1=0;
            int x=KEYBOARD_X,y=KEYBOARD_Y;
            int w=TAREA_W_DEF,h=TAREA_H_DEF;
            uint8_t s;
            while ( (s=areas[i++]) != TAREA_END ) {
                if (s == TAREA_XY) {
                    x = areas[i++];
                    y = areas[i++];                    
                    x2 = x;
                    y2 = y;  
                }
                else if (s == TAREA_WH) {
                    w = areas[i++];
                    h = areas[i++];
                }                     
                else if (s == TAREA_NEW_ROW) {
                  hDir = true;
                  y1 = y2;
                  y2 = y1 + h;
                  x2 = x;
                }  
                else if (s == TAREA_NEW_COL) {
                  hDir = false;
                  x1 = x2;
                  x2 = x1 + w;
                  y2 = y;                  
                }
                else { 
                    if (hDir) {
                      x1 = x2;
                      x2 = x1+s;                                                            
                    } else {
                      y1 = y2;
                      y2 = y1+s;                      
                    }
                    if ( (yt >= y1) && (yt < y2) && (xt >= x1) && (xt < x2)  ) {
                        *rx = x1;
                        *ry = y1;
                        *rw = x2-x1;
                        *rh = y2-y1;
                        return (actions[k]);  
                    }
                    k++;
                }                
            }
        } 
        prev_zt =1; 
    } else {
        prev_zt=0; 
    } 
  
    return ACTION_NONE;   
} 








void emu_printf(char * text)
{
  Serial.println(text);
}

void emu_printf(int val)
{
  Serial.println(val);
}

void emu_printi(int val)
{
  Serial.println(val,HEX);
}

void emu_printh(int val)
{
  Serial.println(val,HEX);
}

static int malbufpt = 0;
static char malbuf[EXTRA_HEAP];

void * emu_Malloc(int size)
{
  void * retval =  malloc(size);
  if (!retval) {
    emu_printf("failled to allocate");
    emu_printf(size);
    emu_printf("fallback");
    if ( (malbufpt+size) < sizeof(malbuf) ) {
      retval = (void *)&malbuf[malbufpt];
      malbufpt += size;      
    }
    else {
      emu_printf("failure to allocate");
    }
  }
  else {
    emu_printf("could allocate dynamic ");
    emu_printf(size);    
  }
  
  return retval;
}

void * emu_MallocI(int size)
{
  void * retval =  NULL; 

  if ( (malbufpt+size) < sizeof(malbuf) ) {
    retval = (void *)&malbuf[malbufpt];
    malbufpt += size;
    emu_printf("could allocate static ");
    emu_printf(size);          
  }
  else {
    emu_printf("failure to allocate");
  }

  return retval;
}
void emu_Free(void * pt)
{
  free(pt);
}





int emu_ReadAnalogJoyX(int min, int max) 
{
  int val = analogRead(PIN_JOY2_A1X);
#if INVX
  val = 4095 - val;
#endif
  val = val-xRef;
  val = ((val*140)/100);
  if ( (val > -512) && (val < 512) ) val = 0;
  val = val+2048;
  return (val*(max-min))/4096;
}

int emu_ReadAnalogJoyY(int min, int max) 
{
  int val = analogRead(PIN_JOY2_A2Y);
#if INVY
  val = 4095 - val;
#endif
  val = val-yRef;
  val = ((val*120)/100);
  if ( (val > -512) && (val < 512) ) val = 0;
  //val = (val*(max-min))/4096;
  val = val+2048;
  //return val+(max-min)/2;
  return (val*(max-min))/4096;
}


static uint16_t readAnalogJoystick(void)
{
  uint16_t joysval = 0;

  int xReading = emu_ReadAnalogJoyX(0,256);
  if (xReading > 128) joysval |= MASK_JOY2_LEFT;
  else if (xReading < 128) joysval |= MASK_JOY2_RIGHT;
  
  int yReading = emu_ReadAnalogJoyY(0,256);
  if (yReading < 128) joysval |= MASK_JOY2_UP;
  else if (yReading > 128) joysval |= MASK_JOY2_DOWN;
  
#ifdef PIN_JOY2_BTN  
  joysval |= (digitalRead(PIN_JOY2_BTN) == HIGH ? 0 : MASK_JOY2_BTN);
#endif
  return (joysval);     
}


int emu_SwapJoysticks(int statusOnly) {
  if (!statusOnly) {
    if (joySwapped) {
      joySwapped = false;
    }
    else {
      joySwapped = true;
    }
  }
  return(joySwapped?1:0);
}

int emu_GetPad(void) 
{
  return(keypadval/*|((joySwapped?1:0)<<7)*/);
}

int emu_ReadKeys(void) 
{
  uint16_t retval;
  uint16_t j1 = readAnalogJoystick();
  uint16_t j2 = 0;
  
  // Second joystick
#if INVY
#ifdef PIN_JOY1_1
  if ( digitalRead(PIN_JOY1_1) == LOW ) j2 |= MASK_JOY2_DOWN;
#endif
#ifdef PIN_JOY1_2
  if ( digitalRead(PIN_JOY1_2) == LOW ) j2 |= MASK_JOY2_UP;
#endif
#else
#ifdef PIN_JOY1_1
  if ( digitalRead(PIN_JOY1_1) == LOW ) j2 |= MASK_JOY2_UP;
#endif
#ifdef PIN_JOY1_2
  if ( digitalRead(PIN_JOY1_2) == LOW ) j2 |= MASK_JOY2_DOWN;
#endif
#endif
#if INVX
#ifdef PIN_JOY1_3
  if ( digitalRead(PIN_JOY1_3) == LOW ) j2 |= MASK_JOY2_LEFT;
#endif
#ifdef PIN_JOY1_4
  if ( digitalRead(PIN_JOY1_4) == LOW ) j2 |= MASK_JOY2_RIGHT;
#endif
#else
#ifdef PIN_JOY1_3
  if ( digitalRead(PIN_JOY1_3) == LOW ) j2 |= MASK_JOY2_RIGHT;
#endif
#ifdef PIN_JOY1_4
  if ( digitalRead(PIN_JOY1_4) == LOW ) j2 |= MASK_JOY2_LEFT;
#endif
#endif
#ifdef PIN_JOY1_BTN
  if ( digitalRead(PIN_JOY1_BTN) == LOW ) j2 |= MASK_JOY2_BTN;
#endif
  if (joySwapped) {
    retval = ((j1 << 8) | j2);
  }
  else {
    retval = ((j2 << 8) | j1);
  }

  if (usbnavpad & MASK_JOY2_UP) retval |= MASK_JOY2_UP;
  if (usbnavpad & MASK_JOY2_DOWN) retval |= MASK_JOY2_DOWN;
  if (usbnavpad & MASK_JOY2_LEFT) retval |= MASK_JOY2_LEFT;
  if (usbnavpad & MASK_JOY2_RIGHT) retval |= MASK_JOY2_RIGHT;
  if (usbnavpad & MASK_JOY2_BTN) retval |= MASK_JOY2_BTN;

#ifdef PIN_KEY_USER1 
  if ( digitalRead(PIN_KEY_USER1) == LOW ) retval |= MASK_KEY_USER1;
#endif
#ifdef PIN_KEY_USER2 
  if ( digitalRead(PIN_KEY_USER2) == LOW ) retval |= MASK_KEY_USER2;
#endif
#ifdef PIN_KEY_USER3 
  if ( digitalRead(PIN_KEY_USER3) == LOW ) retval |= MASK_KEY_USER3;
#endif
#ifdef PIN_KEY_USER4 
  if ( digitalRead(PIN_KEY_USER4) == LOW ) retval |= MASK_KEY_USER4;
#endif

//  Serial.println(retval,HEX);

  if ( ((retval & (MASK_KEY_USER1+MASK_KEY_USER2)) == (MASK_KEY_USER1+MASK_KEY_USER2))
     || (retval & MASK_KEY_USER4 ) )
  {  
// Reset procedure T3.X and T4.0 by Frank Boesing !!   
#if defined(__IMXRT1052__) || defined(__IMXRT1062__)    
    uint32_t tmp = SNVS_LPCR; // save control register
    
    SNVS_LPSR |= 1;
    
    // disable alarm
    SNVS_LPCR &= ~0x02;
    while (SNVS_LPCR & 0x02);
    
    __disable_irq();
    //get Time:
    uint32_t lsb, msb;
    do {
      msb = SNVS_LPSRTCMR;
      lsb = SNVS_LPSRTCLR;
    } while ( (SNVS_LPSRTCLR != lsb) | (SNVS_LPSRTCMR != msb) );
    uint32_t secs = (msb << 17) | (lsb >> 15);
    
    //set alarm
    secs += 2;
    SNVS_LPTAR = secs;
    while (SNVS_LPTAR != secs);
    
    SNVS_LPCR = tmp | 0x02; // restore control register and set alarm
    while (!(SNVS_LPCR & 0x02));
    
    SNVS_LPCR |= (1 << 6); // turn off power
    while (1) asm("wfi");  
#else
    *(volatile uint32_t *)0xE000ED0C = 0x5FA0004;
    while (true) {
      ;
    } 
#endif 
  }
  
  return (retval);
}

unsigned short emu_DebounceLocalKeys(void)
{
  uint16_t bCurState = emu_ReadKeys();
  uint16_t bClick = bCurState & ~bLastState;
  bLastState = bCurState;

  return (bClick);
}

int emu_ReadI2CKeyboard(void) {
  int retval=0;
#ifdef HAS_I2CKBD
  if (i2cKeyboardPresent) {
    byte msg[7]={0,0,0,0,0,0,0};
    Wire.requestFrom(8, 7);    // request 7 bytes from slave device #8 
    int i = 0;
    int hitindex=-1;
    while (Wire.available() && (i<7) ) { // slave may send less than requested
      byte b = Wire.read(); // receive a byte
      if (b != 0xff) hitindex=i; 
      msg[i++] = b;        
    }
    
    if (hitindex >=0 ) {
      /*
      Serial.println(msg[0], BIN);
      Serial.println(msg[1], BIN);
      Serial.println(msg[2], BIN);
      Serial.println(msg[3], BIN);
      Serial.println(msg[4], BIN);
      Serial.println(msg[5], BIN);
      Serial.println(msg[6], BIN);
      */
      unsigned short match = (~msg[hitindex])&0x00FF | (hitindex<<8);
      //Serial.println(match,HEX);  
      for (i=0; i<sizeof(i2ckeys); i++) {
        if (match == i2ckeys[i]) {
          //Serial.println((int)keys[i]);      
          return (keys[i]);
        }
      }
    }    
  }
#endif
  return(retval);
}

void emu_InitJoysticks(void) { 

  // Second Joystick   
#ifdef PIN_JOY1_1
  pinMode(PIN_JOY1_1, INPUT_PULLUP);
#endif  
#ifdef PIN_JOY1_2
  pinMode(PIN_JOY1_2, INPUT_PULLUP);
#endif  
#ifdef PIN_JOY1_3
  pinMode(PIN_JOY1_3, INPUT_PULLUP);
#endif  
#ifdef PIN_JOY1_4
  pinMode(PIN_JOY1_4, INPUT_PULLUP);
#endif  
#ifdef PIN_JOY1_BTN
  pinMode(PIN_JOY1_BTN, INPUT_PULLUP);
#endif  

#ifdef PIN_KEY_USER1
  pinMode(PIN_KEY_USER1, INPUT_PULLUP);
#endif  
#ifdef PIN_KEY_USER2
  pinMode(PIN_KEY_USER2, INPUT_PULLUP);
#endif  
#ifdef PIN_KEY_USER3
  pinMode(PIN_KEY_USER3, INPUT_PULLUP);
#endif  
#ifdef PIN_KEY_USER4
  pinMode(PIN_KEY_USER4, INPUT_PULLUP);
#endif  
#ifdef PIN_JOY2_BTN
  pinMode(PIN_JOY2_BTN, INPUT_PULLUP);
#endif  
  analogReadResolution(12);
  xRef=0; yRef=0;
  for (int i=0; i<10; i++) {
    xRef += analogRead(PIN_JOY2_A1X);
    yRef += analogRead(PIN_JOY2_A2Y);  
    delay(20);
  }

#if INVX
  xRef = 4095 -xRef/10;
#else
  xRef /= 10;
#endif
#if INVY
  yRef = 4095 -yRef/10;
#else
  yRef /= 10;
#endif   
}



static bool vkbKeepOn = false;
static bool vkbActive = false;
static bool vkeyRefresh=false;
static bool exitVkbd = false;
static uint8_t keyPressCount=0; 


bool virtualkeyboardIsActive(void) {
    return (vkbActive);
}

void toggleVirtualkeyboard(bool keepOn) {
       
    if (keepOn) {      
        tft.drawSpriteNoDma(0,0,logo);
        //prev_zt = 0;
        vkbKeepOn = true;
        vkbActive = true;
        exitVkbd = false;  
    }
    else {
        vkbKeepOn = false;
        if ( (vkbActive) /*|| (exitVkbd)*/ ) {
            tft.fillScreenNoDma( RGBVAL16(0x00,0x00,0x00) );
            tft.startDMA();                     
            //prev_zt = 0; 
            vkbActive = false;
            exitVkbd = false;
        }
        else {         
            tft.stopDMA();                  
            tft.drawSpriteNoDma(0,0,logo);           
            //prev_zt = 0;
            vkbActive = true;
            exitVkbd = false;
        }
    }   
}

 
void handleVirtualkeyboard() {
  int rx=0,ry=0,rw=0,rh=0;

    if (keyPressCount == 0) {
      keypadval = 0;      
    } else {
      keyPressCount--;
    }
    
    if ( (!virtualkeyboardIsActive()) && (tft.isTouching()) && (!keyPressCount) ) {
        toggleVirtualkeyboard(false);
        return;
    }
    
    if ( ( (vkbKeepOn) || (virtualkeyboardIsActive())  )  ) {
        char c = captureTouchZone(keysw, keys, &rx,&ry,&rw,&rh);
        if (c) {

            tft.drawRectNoDma( rx,ry,rw,rh, KEYBOARD_HIT_COLOR );
            if ( (c >=1) && (c <= ACTION_MAXKBDVAL) ) {
            
              keypadval = c;
              keyPressCount = 10;
              delay(50);
              vkeyRefresh = true;
              exitVkbd = true;
            }
            else if (c == ACTION_EXITKBD) {
              vkeyRefresh = true;
              exitVkbd = true;  
            }
        }   
     }    
     
    if (vkeyRefresh) {
        vkeyRefresh = false;
        tft.drawSpriteNoDma(0,0,logo, rx, ry, rw, rh);
    }  
         
    if ( (exitVkbd) && (vkbActive) ) {      
        if (!vkbKeepOn) {             
            toggleVirtualkeyboard(false);
        }
        else {         
            toggleVirtualkeyboard(true);           
        } 
    }
}

int emu_setKeymap(int index) {
}




static int readNbFiles(void) {
  int totalFiles = 0;

#ifdef USE_SDFS
  DIR dir;
  FILINFO entry; 
  f_opendir(&dir, romspath);  
  while ( (true) && (totalFiles<MAX_FILES) ) {
    f_readdir(&dir, &entry);
    if (!entry.fname[0]) {
      // no more files
      break;
    }
    char * filename = entry.fname;
    Serial.println(filename);     
    if ( !(entry.fattrib & AM_DIR) ) {
      strncpy(&files[totalFiles][0], filename, MAX_FILENAME_SIZE-1);
      totalFiles++;
    }
    else {
      if ( (strcmp(filename,".")) && (strcmp(filename,"..")) ) {
        strncpy(&files[totalFiles][0], filename, MAX_FILENAME_SIZE-1);
        totalFiles++;
      }
    }  
  } 
  f_closedir(&dir);
#else
  File entry;    
  file = SD.open(romspath);
  while ( (true) && (totalFiles<MAX_FILES) ) {
    entry = file.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
#ifdef USE_SDFAT
  char filename[MAX_FILENAME_SIZE];
  entry.getName(&filename[0], MAX_FILENAME_SIZE); 
#else
  char * filename = entry.name();
#endif 
    Serial.println(filename); 
    if (!entry.isDirectory())  {
      strncpy(&files[totalFiles][0], filename, MAX_FILENAME_SIZE-1);
      totalFiles++;
    }
    else {
      if ( (strcmp(filename,".")) && (strcmp(filename,"..")) ) {
        strncpy(&files[totalFiles][0], filename, MAX_FILENAME_SIZE-1);
        totalFiles++;
      }
    }  
    entry.close();
  }
  file.close();
#endif  
  return totalFiles;  
}  



void backgroundMenu(void) {
    menuRedraw=true;  
    tft.fillScreenNoDma(RGBVAL16(0x00,0x00,0x00));
    tft.drawTextNoDma(0,0, TITLE, RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), true);  
#ifndef HAS_T4_VGA
    tft.drawSpriteNoDma(MENU_VBAR_XOFFSET,MENU_VBAR_YOFFSET,bmpvbar);
#endif    
#ifdef OLD_LAYOUT    
    tft.drawSpriteNoDma(MENU_TFT_XOFFSET,MENU_TFT_YOFFSET,bmptft);
    tft.drawSpriteNoDma(MENU_VGA_XOFFSET,MENU_VGA_YOFFSET,bmpvga);
#endif      
}

int handleMenu(uint16_t bClick)
{
  int action = ACTION_NONE;

  char newpath[80];
  strcpy(newpath, romspath);
  strcat(newpath, "/");
  strcat(newpath, selection);

  int rx=0,ry=0,rw=0,rh=0;
  char c = 0; //captureTouchZone(menutouchareas, menutouchactions, &rx,&ry,&rw,&rh);
 
  if ( (bClick & MASK_JOY2_BTN) || (c == MKEY_TFT) ) {     
      emu_printf(newpath);
#ifdef USE_SDFS
      FILINFO entry;
      f_stat(newpath, &entry);
      if ( (entry.fattrib & AM_DIR) ) {
#else
      File file = SD.open(newpath);
      if (file.isDirectory())  {
#endif      
        strcpy(romspath,newpath);
        curFile = 0;
        nbFiles = readNbFiles();             
      }
      else {
        action = ACTION_RUNTFT;               
      }
      menuRedraw=true;
  }
  else if ( (bClick & MASK_KEY_USER1) || (c == MKEY_VGA) ) {
      menuRedraw=true;
      action = ACTION_RUNVGA;    
  }
  else if ( (c >= MKEY_L1) && (c <= MKEY_L9) ) {
    if ( (topFile+(int)c-1) <= (nbFiles-1)  )
    {
      curFile = topFile + (int)c -1;
      menuRedraw=true;
     //tft.drawRectNoDma( rx,ry,rw,rh, KEYBOARD_HIT_COLOR );
    }
  }
  else if (bClick & MASK_JOY2_UP) {
    if (curFile!=0) {
      menuRedraw=true;
      curFile--;
    }
  }
  else if ( (bClick & MASK_JOY2_RIGHT) || (bClick & MASK_JOY1_RIGHT) || (c == MKEY_UP) ) {
    if ((curFile-9)>=0) {
      menuRedraw=true;
      curFile -= 9;
    } else if (curFile!=0) {
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
  else if ( (bClick & MASK_JOY2_LEFT) || (bClick & MASK_JOY1_LEFT) || (c == MKEY_DOWN) ) {
    if ((curFile<(nbFiles-9)) && (nbFiles)) {
      curFile += 9;
      menuRedraw=true;
    }
    else if ((curFile<(nbFiles-1)) && (nbFiles)) {
      curFile++;
      menuRedraw=true;
    }
  }
  else if ( (bClick & MASK_KEY_USER2) || (c == MKEY_JOY) ) {
    emu_SwapJoysticks(0);
    menuRedraw=true;  
  }   
    
  if (menuRedraw && nbFiles) {
    int fileIndex = 0;
    tft.drawRectNoDma(MENU_FILE_XOFFSET,MENU_FILE_YOFFSET, MENU_FILE_W, MENU_FILE_H, MENU_FILE_BGCOLOR);
//    if (curFile <= (MAX_MENULINES/2-1)) topFile=0;
//    else topFile=curFile-(MAX_MENULINES/2);
    if (curFile <= (MAX_MENULINES-1)) topFile=0;
    else topFile=curFile-(MAX_MENULINES/2);

    //Serial.print("curfile: ");
    //Serial.println(curFile);
    //Serial.print("topFile: ");
    //Serial.println(topFile);
    
    int i=0;
    while (i<MAX_MENULINES) {
      if (fileIndex>=nbFiles) {
          // no more files
          break;
      }
      char * filename = &files[fileIndex][0];    
      if (fileIndex >= topFile) {              
        if ((i+topFile) < nbFiles ) {
          if ((i+topFile)==curFile) {
            tft.drawTextNoDma(MENU_FILE_XOFFSET,i*TEXT_HEIGHT+MENU_FILE_YOFFSET, filename, RGBVAL16(0xff,0xff,0x00), RGBVAL16(0xff,0x00,0x00), true);
            strcpy(selection,filename);            
          }
          else {
            tft.drawTextNoDma(MENU_FILE_XOFFSET,i*TEXT_HEIGHT+MENU_FILE_YOFFSET, filename, RGBVAL16(0xff,0xff,0xff), MENU_FILE_BGCOLOR, true);      
          }
        }
        i++; 
      }
      fileIndex++;    
    }

#ifndef HAS_T4_VGA
    tft.drawSpriteNoDma(0,MENU_JOYS_YOFFSET,(uint16_t*)bmpjoy);
#endif      
    tft.drawTextNoDma(48,MENU_JOYS_YOFFSET+8, (emu_SwapJoysticks(1)?(char*)"SWAP=1":(char*)"SWAP=0"), RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), false);
    menuRedraw=false;     
  }

  return (action);  
}

bool menuActive(void) 
{
  return (menuOn);
}

void toggleMenu(bool on) {
  if (on) {
    callibrationOn=false;
    menuOn=true;
    backgroundMenu();
  } else {
    menuOn = false;    
  }
}

char * menuSelection(void)
{
  return (selection);  
}
  



static void callibrationInit(void) 
{
  callibrationOn=true;
  menuOn=false;
  callibrationStep = 0;
  calMinX=0,calMinY=0,calMaxX=0,calMaxY=0;
  tft.fillScreenNoDma(RGBVAL16(0xff,0xff,0xff));
  tft.drawTextNoDma(0,100, "          Callibration process:", RGBVAL16(0x00,0x00,0x00), RGBVAL16(0xff,0xff,0xff), true);
  tft.drawTextNoDma(0,116, "     Hit the red cross at each corner", RGBVAL16(0x00,0x00,0x00), RGBVAL16(0xff,0xff,0xff), true);
  tft.drawTextNoDma(0,0, "+", RGBVAL16(0xff,0x00,0x00), RGBVAL16(0xff,0xff,0xff), true);
  prev_zt = 1;  
}


static void readCallibration(void) 
{
  char fileBuffer[64];

#ifdef USE_SDFS
  FIL file;
  int retval;          
  if( !(f_open(&file, CALIBRATION_FILE, FA_READ)) ) {
    if( !(f_read (&file, fileBuffer, 64, &retval)) ) {
      if (retval == 64) { 
        sscanf(fileBuffer,"%d %d %d %d", &calMinX,&calMinY,&calMaxX,&calMaxY); 
      }
    }
    f_close(&file);    
  }
#else
  File file = SD.open(CALIBRATION_FILE, O_READ);  
  if (file) {
    if ( file.read(fileBuffer, 64) ) {
      sscanf(fileBuffer,"%d %d %d %d", &calMinX,&calMinY,&calMaxX,&calMaxY);
    }
    file.close();
    Serial.println("Current callibration params:");
    Serial.println(calMinX);
    Serial.println(calMinY);
    Serial.println(calMaxX);
    Serial.println(calMaxY);                 
  }
#endif  
  else {
    Serial.println("Callibration read error");
  }  
  tft.callibrateTouch(calMinX,calMinY,calMaxX,calMaxY);   
}

static void writeCallibration(void) 
{
  tft.callibrateTouch(calMinX,calMinY,calMaxX,calMaxY);

#ifdef USE_SDFS
  FIL file; 
  int retval;       
  if( !(f_open(&file, CALIBRATION_FILE, FA_WRITE)) ) {
    //if( !(f_read (&file, fileBuffer, 64, &retval)) ) {
    //  if (retval == 64) { 
    //    sscanf(fileBuffer,"%d %d %d %d", &calMinX,&calMinY,&calMaxX,&calMaxY); 
    //  }
    //}
    f_close(&file);    
  }
#else
  File file = SD.open(CALIBRATION_FILE, O_WRITE | O_CREAT | O_TRUNC);
  if (file) {
    file.print(calMinX);
    file.print(" ");
    file.print(calMinY);
    file.print(" ");
    file.print(calMaxX);
    file.print(" ");
    file.println(calMaxY);
    file.close();
  }
#endif  
  else {
    Serial.println("Callibration write error");
  }  
}


bool callibrationActive(void) 
{
  return (callibrationOn);
}



int handleCallibration(uint16_t bClick) {
  uint16_t xt=0;
  uint16_t yt=0;
  uint16_t zt=0;  
  if (tft.isTouching()) {
    if (prev_zt == 0) {
      prev_zt = 1;
      tft.readRaw(&xt,&yt,&zt);
      if (zt < 1000) {
        return 0;
      }
      switch (callibrationStep) 
      {
        case 0:
          callibrationStep++;
          tft.drawTextNoDma(0,0, " ", RGBVAL16(0xff,0xff,0xff), RGBVAL16(0xff,0xff,0xff), true);
          tft.drawTextNoDma(TFT_REALWIDTH-8,0, "+", RGBVAL16(0xff,0x00,0x00), RGBVAL16(0xff,0xff,0xff), true);
          calMinX += xt;
          calMinY += yt;          
          break;
        case 1:
          callibrationStep++;
          tft.drawTextNoDma(TFT_REALWIDTH-8,0, " ", RGBVAL16(0xff,0xff,0xff), RGBVAL16(0xff,0xff,0xff), true);
          tft.drawTextNoDma(TFT_REALWIDTH-8,TFT_REALHEIGHT-16, "+", RGBVAL16(0xff,0x00,0x00), RGBVAL16(0xff,0xff,0xff), true);
          calMaxX += xt;
          calMinY += yt;           
          break;
        case 2:
          callibrationStep++;
          tft.drawTextNoDma(TFT_REALWIDTH-8,TFT_REALHEIGHT-16, " ", RGBVAL16(0xff,0xff,0xff), RGBVAL16(0xff,0xff,0xff), true);
          tft.drawTextNoDma(0,TFT_REALHEIGHT-16, "+", RGBVAL16(0xff,0x00,0x00), RGBVAL16(0xff,0xff,0xff), true);
          calMaxX += xt;
          calMaxY += yt;
          break;
        case 3:
          tft.fillScreenNoDma(RGBVAL16(0xff,0xff,0xff));
          tft.drawTextNoDma(0,100, "          Callibration done!", RGBVAL16(0x00,0x00,0x00), RGBVAL16(0xff,0xff,0xff), true);
          tft.drawTextNoDma(0,116, "        (Click center to exit)", RGBVAL16(0xff,0x00,0x00), RGBVAL16(0xff,0xff,0xff), true);           
          callibrationStep++;
          calMinX += xt;
          calMaxY += yt;       
          break;                 
        case 4:
          //Serial.println(xt);
          //Serial.println(yt);
          if ( (xt > (TFT_REALWIDTH/4)) && (xt < (TFT_REALWIDTH*3)/4) 
            && (yt > (TFT_REALHEIGHT/4)) && (yt < (TFT_REALHEIGHT*3)/4) ) {
            calMinX /= 2;
            calMinY /= 2;
            calMaxX /= 2;
            calMaxY /= 2;
            writeCallibration();                       
            toggleMenu(true);
          }
          else {
            callibrationInit();              
          }
          break; 
                           
      }
      delay(100);
    }  
  }
  else {
    prev_zt = 0;
  }  
}





int emu_FileOpen(char * filename)
{
  int retval = 0;

  char filepath[80];
  strcpy(filepath, romspath);
  strcat(filepath, "/");
  strcat(filepath, filename);
  emu_printf("FileOpen...");
  emu_printf(filepath);
#ifdef USE_SDFS
  if( !(f_open(&file, filepath, FA_READ)) ) {
#else    
  if ((file = SD.open(filepath, O_READ))) {
#endif
    retval = 1;  
  }
  else {
    emu_printf("FileOpen failed");
  }
  return (retval);
}

int emu_FileRead(char * buf, int size)
{
  unsigned char buffer[256];
  
  int remaining = size;
  int byteread = 0;
  int retval=0; 
  if (size < 256) {
#ifdef USE_SDFS
    if( !(f_read (&file, buffer, size, &retval)) )
#else
    retval = file.read(buffer, size);
#endif
   if (retval>0) {
      memcpy(buf,buffer,retval);
      byteread += retval;   
   }   
  }
  else {
    while (remaining>0) {
#ifdef USE_SDFS
      if( !(f_read (&file, buffer, 256, &retval)) )
      //f_read (&file, buffer, 256, &retval);
#else
      retval = file.read(buffer, 256);
#endif
      if (retval>0) {
        //emu_printi(retval);
        memcpy(buf,buffer,retval);
        buf += retval;
        byteread += retval;     
        remaining -= retval;
      }
      else {
        break;
      }
    }    
  }

  return byteread; 
}

unsigned char emu_FileGetc(void) {
  unsigned char c;
#ifdef USE_SDFS
  int retval=0;
  if( !(f_read (&file, &c, 1, &retval)) )
#else
  int retval = file.read(&c, 1);
#endif  
  if (retval != 1) {
    emu_printf("emu_FileGetc failed");
  }  
  return c; 
}


void emu_FileClose(void)
{
#ifdef USE_SDFS
  f_close(&file); 
#else
  file.close();  
#endif  
}

int emu_FileSize(char * filename) 
{
  int filesize=0;
  char filepath[80];
  strcpy(filepath, romspath);
  strcat(filepath, "/");
  strcat(filepath, filename);
  emu_printf("FileSize...");
  emu_printf(filepath);
#ifdef USE_SDFS
  FILINFO entry;
  f_stat(filepath, &entry);
  filesize = entry.fsize; 
#else
  if ((file = SD.open(filepath, O_READ))) 
  {
    emu_printf("filesize is...");
    filesize = file.size(); 
    emu_printf(filesize);
    file.close();    
  }
#endif
 
  return(filesize);    
}

int emu_FileSeek(int seek) 
{
#ifdef USE_SDFS
  f_lseek(&file, seek);
#else
  file.seek(seek);
#endif
  return (seek);
}

int emu_FileTell(void) 
{
#ifdef USE_SDFS
  return (f_tell(&file));
#else
  return (50);
#endif
}


int emu_LoadFile(char * filename, char * buf, int size)
{
  int filesize = 0;
    
  char filepath[80];
  strcpy(filepath, romspath);
  strcat(filepath, "/");
  strcat(filepath, filename);
  emu_printf("LoadFile...");
  emu_printf(filepath);
#ifdef USE_SDFS
  if( !(f_open(&file, filepath, FA_READ)) ) {
    filesize = f_size(&file);
    emu_printf(filesize);
    if (size >= filesize)
    {
      int retval=0;
      if( (f_read (&file, buf, filesize, &retval)) ) {
        emu_printf("File read failed");        
      }
    }
    f_close(&file);
  }
#else
  if ((file = SD.open(filepath, O_READ))) 
  {
    filesize = file.size(); 
    emu_printf(filesize);
    
    if (size >= filesize)
    {
      if (emu_FileRead(buf, filesize) != filesize) 
      {
        emu_printf("File read failed");
      }        
    }
    file.close();
  }
#endif  
  
  return(filesize);
}

int emu_LoadFileSeek(char * filename, char * buf, int size, int seek)
{
  int filesize = 0;
    
  char filepath[80];
  strcpy(filepath, romspath);
  strcat(filepath, "/");
  strcat(filepath, filename);
  emu_printf("LoadFileSeek...");
  emu_printf(filepath);
#ifdef USE_SDFS
  if( !(f_open(&file, filepath, FA_READ)) ) {
    f_lseek(&file, seek);
    emu_printf(size);
    if (size >= filesize)
    {
      int retval=0;
      if( (!f_read (&file, buf, size, &retval)) ) 
      if (retval != size)
      {
        emu_printf("File read failed");      
      }
    }
    f_close(&file);
  }
#else
  if ((file = SD.open(filepath, O_READ))) 
  {
    file.seek(seek);
    emu_printf(size);
    if (file.read(buf, size) != size) {
      emu_printf("File read failed");
    }        
    file.close();
  }
#endif  
  
  return(filesize);
}

const uint32_t FILE_SIZE_MB = 8;
const uint32_t FILE_SIZE = 1000000UL*FILE_SIZE_MB;
#ifdef USE_SDFS
static FIL tempfile; 
#else
static File tempfile;
#endif

void emu_FileTempInit(void) 
{
#ifdef USE_SDFS
 if( (f_open(&tempfile, SDFSDEV "/bench.dat", FA_READ| FA_WRITE )) ) {
#else
#ifdef USE_SDFAT
  if (!tempfile.open("/bench.dat", O_RDWR /*| O_CREAT | O_TRUNC*/)) {
#else
  if ((tempfile = SD.open("/bench.dat", O_RDWR))) {
#endif
#endif
    Serial.println("psram open failed");
  }
/*
  else {
    uint8_t buf[256];
    Serial.println("psram creating");    
    file = SD.open("/bench.dat", O_RDWR | O_CREAT | O_TRUNC);
    if (file) {
      Serial.println("psram opened");    
      file.truncate(0);

      if (!file.preAllocate(FILE_SIZE)) {
        Serial.println("allocate failed");  
      }      
      for(int i=0;i<FILE_SIZE/256;i++){
        file.write(buf, 256);
        Serial.println(i);
        
      }
      Serial.println("psram created");
    }
    
    //file.close();
  }
*/  
#ifdef USE_SDFS
  FILINFO entry;
  f_stat(SDFSDEV "/bench.dat", &entry);
  int fs=entry.fsize;
#else
  int fs=tempfile.size();
#endif
  Serial.print("psram size is ");
  Serial.println(fs); 
}

void emu_FileTempRead(int addr, unsigned char * val, int n) 
{
#ifdef USE_SDFS
  f_lseek(&file, addr);
  int retval=0;
  f_read (&file, val, n, &retval);
#else
  tempfile.seek(addr);
  tempfile.read(val, n);
#endif    
}

void emu_FileTempWrite(int addr, unsigned char val) 
{
  uint8_t v=val;
#ifdef USE_SDFS
  f_lseek(&tempfile, addr);
  int retval=0;
  f_write (&tempfile, &v, 1, &retval);
#else
  tempfile.seek(addr);
  tempfile.write(&v, 1);
#endif   
}

#ifdef HAS_USBKEY
void OnPress(auto key)
{
  uint8_t keymodifier = keyboard1.getModifiers();

  if(keymodifier == 0x40){
    // ALTGR Key modifier FR Keyboard
    switch (key) {
#ifdef LAYOUT_FRENCH
      case 233 : key = '~' ; break;
      case  34 : key = '#' ; break;
      case  39 : key = '{' ; break;
      case  40 : key = '[' ; break;
      case  45 : key = '|' ; break;
      case 232 : key = '`' ; break;
      case  95 : key = 92  ; break;
      case 231 : key = '^' ; break;
      case 224 : key = '@' ; break;
      case  41 : key = ']' ; break;
      case  61 : key = '}' ; break;
#endif  
#ifdef LAYOUT_FRENCH_BELGIAN
      case  38 : key = '|' ; break; //1
      case 233 : key = '@' ; break; //2
      case  34 : key = '#' ; break; //3
      case 167 : key = '^' ; break; //6
      case 231 : key = '{' ; break; //9
      case 224 : key = '}' ; break; //0
      case  36 : key = ']' ; break; //$
      case  61 : key = '~' ; break; //=
#endif  
    }
  }  

  if (menuActive())
  {
    switch (key)  
    {
      case 217:
        usbnavpad |= MASK_JOY2_DOWN;
        break;
      case 218:
        usbnavpad |= MASK_JOY2_UP;
        break;
      case 216:
        usbnavpad |= MASK_JOY2_LEFT;
        break;
      case 215:
        usbnavpad |= MASK_JOY2_RIGHT;
        break;
      case 10:
        usbnavpad |= MASK_JOY2_BTN;
        break;
    }     
  }
  else
  {
    emu_KeyboardOnDown(keymodifier, key);
  }
}

void OnRelease(int key)
{
  uint8_t keymodifier = keyboard1.getModifiers();

  if(keymodifier == 0x40){
    // ALTGR Key modifier FR Keyboard
    switch (key) {
#ifdef LAYOUT_FRENCH
      case 233 : key = '~' ; break;
      case  34 : key = '#' ; break;
      case  39 : key = '{' ; break;
      case  40 : key = '[' ; break;
      case  45 : key = '|' ; break;
      case 232 : key = '`' ; break;
      case  95 : key = 92  ; break;
      case 231 : key = '^' ; break;
      case 224 : key = '@' ; break;
      case  41 : key = ']' ; break;
      case  61 : key = '}' ; break;
#endif  
#ifdef LAYOUT_FRENCH_BELGIAN
      case  38 : key = '|' ; break; //1
      case 233 : key = '@' ; break; //2
      case  34 : key = '#' ; break; //3
      case 167 : key = '^' ; break; //6
      case 231 : key = '{' ; break; //9
      case 224 : key = '}' ; break; //0
      case  36 : key = ']' ; break; //$
      case  61 : key = '~' ; break; //=
#endif  
    }
  }

  if (menuActive())
  {
    switch (key)  
    {
      case 217:
        usbnavpad &= ~MASK_JOY2_DOWN;
        break;
      case 218:
        usbnavpad &= ~MASK_JOY2_UP;
        break;
      case 216:
        usbnavpad &= ~MASK_JOY2_LEFT;
        break;
      case 215:
        usbnavpad &= ~MASK_JOY2_RIGHT;
        break;
      case 10:
        usbnavpad &= ~MASK_JOY2_BTN;
        break;
    }     
  }
  else
  {
    emu_KeyboardOnUp(keymodifier, key);  
  }
}
#endif

void emu_init(void)
{
  Serial.begin(115200);

#ifdef HAS_USBKEY
  myusb.begin();
  keyboard1.attachPress(OnPress);
  keyboard1.attachRelease(OnRelease);
#endif

#ifdef USE_SDFS
  strcpy(romspath,SDFSDEV);
  strcat(romspath,ROMSDIR);    
  FRESULT rc;
  if((rc = f_mount (&fatfs, romspath, 1))) {
    emu_printf("Fail mounting SDFS");  
  }
  else { 
    rc = f_chdrive(ROMSDIR);
  }
#else  
#ifdef USE_SDFAT
  while (!SD.begin(SD_CONFIG))
#else
  while (!SD.begin(SD_CS))
#endif
  {
    Serial.println("SD begin failed, retrying...");
    delay(1000);    
  }
  strcpy(romspath,ROMSDIR);
#endif

  nbFiles = readNbFiles(); 

  Serial.print("SD initialized, files found: ");
  Serial.println(nbFiles);

  
  emu_InitJoysticks();
#ifdef SWAP_JOYSTICK
  joySwapped = true;   
#else
  joySwapped = false;   
#endif  
  readCallibration();
 
  if ((tft.isTouching()) || (emu_ReadKeys() & MASK_JOY2_BTN) ) {
    callibrationInit();
  } 
  else  
  {
    toggleMenu(true);
  }
}


void emu_start(void)
{
#ifdef HAS_I2CKBD
  byte msg[7]={0,0,0,0,0,0,0};
  Wire.begin(); // join i2c bus SDA2/SCL2
  Wire.requestFrom(8, 7);    // request 7 bytes from slave device #8 
  int i = 0;
  while (Wire.available() && (i<7) ) { // slave may send less than requested
    byte b = Wire.read(); // receive a byte
    msg[i++] = b;        
  }
  /*     
  Serial.println(msg[0], BIN);
  Serial.println(msg[1], BIN);
  Serial.println(msg[2], BIN);
  Serial.println(msg[3], BIN);
  Serial.println(msg[4], BIN);
  Serial.println(msg[5], BIN);
  Serial.println(msg[6], BIN);
  */  
  if ( (msg[0] == 0xff) && (msg[1] == 0xff) && 
       (msg[2] == 0xff) && (msg[3] == 0xff) && 
       (msg[4] == 0xff) && (msg[5] == 0xff) && (msg[6] == 0xff)) {
    i2cKeyboardPresent = true;
    Serial.println("i2C keyboard found");            
  }
#endif

  usbnavpad = 0;
  
  keys = key_map1;
  keyMap = 0;
}
