#define KEYMAP_PRESENT 1

extern "C" {
  #include "emuapi.h"
  #include "iopins.h"
}

#ifdef HAS_T4_VGA
#include "vga_t_dma.h"
#else
#include "tft_t_dma.h"
#endif

#include "bmpjoy.h"
#include "bmpvbar.h"



#ifdef HAS_USBKEY
#include "USBHost_t36.h"  // Read this header first for key info
USBHost myusb;
USBHub hub1(myusb);
KeyboardController keyboard1(myusb);
USBHIDParser hid1(myusb);
MouseController mouse1(myusb);
#endif
static bool mouseDetected = false;
static bool keyboardDetected = false;
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


#define MAX_FILES           64
#define MAX_FILENAME_SIZE   24
#define MAX_MENULINES       9
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
static char shown_selection[MAX_FILENAME_SIZE+1]="";
static char files[MAX_FILES][MAX_FILENAME_SIZE];
static bool menuRedraw=true;

static int keyMap;
static const unsigned short * keys;

#ifdef TEECOMPUTER
static unsigned char keymatrix[6];
static int keymatrix_hitrow=-1;
static bool key_fn=false;
static bool key_alt=false;
static uint32_t keypress_t_ms=0;
static uint32_t last_t_ms=0;
static uint32_t hundred_ms_cnt=0;
static bool ledflash_toggle=false;
#endif

static boolean joySwapped = false;
static uint16_t bLastState;
static int xRef;
static int yRef;

static bool menuOn=true;


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





#ifdef PIN_JOY2_A1X
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
#endif

#ifdef PIN_JOY2_A2Y
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
#endif

static uint16_t readAnalogJoystick(void)
{
  uint16_t joysval = 0;

#ifdef PIN_JOY2_A1X
  int xReading = emu_ReadAnalogJoyX(0,256);
  if (xReading > 128) joysval |= MASK_JOY2_LEFT;
  else if (xReading < 128) joysval |= MASK_JOY2_RIGHT;
  
  int yReading = emu_ReadAnalogJoyY(0,256);
  if (yReading < 128) joysval |= MASK_JOY2_UP;
  else if (yReading > 128) joysval |= MASK_JOY2_DOWN;
#endif
  
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
  return(bLastState/*|((joySwapped?1:0)<<7)*/);
}

int emu_ReadKeys(void) 
{
  uint16_t retval;
  uint16_t j1 = readAnalogJoystick();
  uint16_t j2 = 0;
  
  // Second joystick
#ifdef PIN_JOY1_1
  if ( digitalRead(PIN_JOY1_1) == LOW ) j2 |= MASK_JOY2_UP;
#endif
#ifdef PIN_JOY1_2
  if ( digitalRead(PIN_JOY1_2) == LOW ) j2 |= MASK_JOY2_DOWN;
#endif
#ifdef PIN_JOY1_3
  if ( digitalRead(PIN_JOY1_3) == LOW ) j2 |= MASK_JOY2_RIGHT;
#endif
#ifdef PIN_JOY1_4
  if ( digitalRead(PIN_JOY1_4) == LOW ) j2 |= MASK_JOY2_LEFT;
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
  if (usbnavpad & MASK_KEY_USER1) retval |= MASK_KEY_USER1;
  if (usbnavpad & MASK_KEY_USER2) retval |= MASK_KEY_USER2;

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

#ifdef TEECOMPUTER
  keymatrix_hitrow = -1;
  unsigned char row;
  unsigned short cols[6]={KCOLOUT1,KCOLOUT2,KCOLOUT3,KCOLOUT4,KCOLOUT5,KCOLOUT6};
  for (int i=0;i<6;i++){
    pinMode(cols[i],OUTPUT);
    digitalWrite(cols[i], 0);
    row=0; 
    row |= (digitalRead(KROWIN1) ? 0 : 0x01);
    row |= (digitalRead(KROWIN1) ? 0 : 0x01);
    row |= (digitalRead(KROWIN1) ? 0 : 0x01);
    row |= (digitalRead(KROWIN1) ? 0 : 0x01);
    row |= (digitalRead(KROWIN2) ? 0 : 0x02);
    row |= (digitalRead(KROWIN3) ? 0 : 0x04);
    row |= (digitalRead(KROWIN4) ? 0 : 0x08);
    row |= (digitalRead(KROWIN5) ? 0 : 0x10);
    row |= (digitalRead(KROWIN6) ? 0 : 0x20);
    digitalWrite(cols[i], 1);
    pinMode(cols[i],INPUT_DISABLE);
    keymatrix[i]=row;
  }

  bool alt_pressed=false;
  if ( keymatrix[5] & 0x20 ) {alt_pressed=true; keymatrix[5] &= ~0x20;};

  for (int i=0;i<6;i++){
    row = keymatrix[i];
    if (row) keymatrix_hitrow=i;
  }

  //6,9,15,8,7,22
#if INVX
  if ( row & 0x2  ) retval |= MASK_JOY2_LEFT;
  if ( row & 0x1  ) retval |= MASK_JOY2_RIGHT;
#else
  if ( row & 0x1  ) retval |= MASK_JOY2_LEFT;
  if ( row & 0x2  ) retval |= MASK_JOY2_RIGHT;
#endif
#if INVY
  if ( row & 0x8  ) retval |= MASK_JOY2_DOWN;
  if ( row & 0x4  ) retval |= MASK_JOY2_UP;  
#else
  if ( row & 0x4  ) retval |= MASK_JOY2_DOWN;
  if ( row & 0x8  ) retval |= MASK_JOY2_UP;  
#endif
  if ( row & 0x10 ) retval |= MASK_JOY2_BTN;

  // Handle LED flash
  uint32_t time_ms=millis();
  if ((time_ms-last_t_ms) > 100) {
    last_t_ms = time_ms;
    if (ledflash_toggle == false) {
      ledflash_toggle = true;
    }
    else {
      ledflash_toggle = false;
    }  
  }  
 
  if ( alt_pressed ) {
    if (key_fn == false) 
    {
      // Release to Press transition
      if (hundred_ms_cnt == 0) {
        keypress_t_ms=time_ms;
        hundred_ms_cnt += 1; // 1
      }  
      else {
        hundred_ms_cnt += 1; // 2
        if (hundred_ms_cnt >= 2) 
        { 
          hundred_ms_cnt = 0; 
          if ( (time_ms-keypress_t_ms) < 500) 
          {
            if (key_alt == false) 
            {
              key_alt = true;
            }
            else 
            {
              key_alt = false;
            } 
          }
        }        
      }
    }
    else {
      // Keep press
      if (hundred_ms_cnt == 1) {
        if ((millis()-keypress_t_ms) > 1000) 
        {
          if (key_alt == false) 
          {
            key_alt = true;
          }
          else 
          {
            key_alt = false;
          } 
          hundred_ms_cnt = 0; 
        }
      } 
    } 
    key_fn = true;
  }
  else  {
    key_fn = false;    
  }

  // Handle LED
  if (key_alt == true) {
    digitalWrite(KLED, (ledflash_toggle?1:0));
  }
  else {
    if (key_fn == true) {
      digitalWrite(KLED, 1);
    }
    else {
      digitalWrite(KLED, 0);
    }     
  } 
 
  if ( key_fn ) retval |= MASK_KEY_USER2;
  if ( ( key_fn ) && (keymatrix[0] == 0x02 )) retval |= MASK_KEY_USER1;
#else

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
#endif
  
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
#ifdef TEECOMPUTER
  if (key_alt) {
    keys = (const unsigned short *)key_map3;
  }
  else if (key_fn) {
    keys = (const unsigned short *)key_map2;
  }
  else {
    keys = (const unsigned short *)key_map1;
  }
  if (keymatrix_hitrow >=0 ) {
    unsigned short match = ((unsigned short)keymatrix_hitrow<<8) | keymatrix[keymatrix_hitrow];  
    for (int i=0; i<sizeof(matkeys)/sizeof(unsigned short); i++) {
      if (match == matkeys[i]) {
        hundred_ms_cnt = 0;    
        return (keys[i]);
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

#ifdef PIN_JOY2_A1X
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
#endif

#ifdef TEECOMPUTER
  // keyboard LED
  pinMode(KLED,OUTPUT);
  digitalWrite(KLED,1);

  // Output (cols)
  pinMode(KCOLOUT1,OUTPUT);
  pinMode(KCOLOUT2,OUTPUT);
  pinMode(KCOLOUT3,OUTPUT);
  pinMode(KCOLOUT4,OUTPUT);
  pinMode(KCOLOUT5,OUTPUT);
  pinMode(KCOLOUT6,OUTPUT);
  digitalWrite(KCOLOUT1,1);
  digitalWrite(KCOLOUT2,1);
  digitalWrite(KCOLOUT3,1);
  digitalWrite(KCOLOUT4,1);
  digitalWrite(KCOLOUT5,1);
  digitalWrite(KCOLOUT6,1);

  // Input pins (rows)
  pinMode(KROWIN1, INPUT_PULLUP);
  pinMode(KROWIN2, INPUT_PULLUP);
  pinMode(KROWIN3, INPUT_PULLUP);
  pinMode(KROWIN4, INPUT_PULLUP);
  pinMode(KROWIN5, INPUT_PULLUP);
  pinMode(KROWIN6, INPUT_PULLUP); 
#endif
}


int emu_setKeymap(int index) {
  /*
  int xoff = 16;
  if (keyMap == 1) {  
    keyMap = 0;     
    keys = key_map1;
    tft.drawText(xoff, 0+16, " 1 I 2 I 3 I 4 I 5 I 6 I 7 I 8 I 9 I 10I", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), true);
    tft.drawText(xoff,16+16, "--- --- --- --- --- --- --- --- --- --- ", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), false);
    tft.drawText(xoff,24+16, " q I w I e I r I t I y I u I i I o I p I", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0x80), true);
    tft.drawText(xoff,40+16, "--- --- --- --- --- --- --- --- --- --- ", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0x80), false);
    tft.drawText(xoff,48+16, " a I s I d I f I g I h I j I k I l IRTNI", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), true);
    tft.drawText(xoff,64+16, "--- --- --- --- --- --- --- --- --- --- ", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), false);
    tft.drawText(xoff,72+16, " z I x I c I v I b I n I m I , IDELISPCI", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0x80), true);
    tft.drawText(xoff,88+16, "--- --- --- --- --- --- --- --- --- --- ", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0x80), false);
    tft.drawText(xoff,96+16, "mouse mode                              ", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0x00), false);
    delay(200);
  }
  else if (keyMap == 0) {
    keyMap = 1;     
    keys = key_map2;            
    tft.drawText(xoff, 0+16, "F1 IF2 IF3 IF4 IF5 IF6 IF7 IF8 IF9 IF10I", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), true);
    tft.drawText(xoff,16+16, "--- --- --- --- --- --- --- --- --- --- ", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), false);
    tft.drawText(xoff,24+16, "PRNIDELIDELI r I t I y I u I i I o I p I", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0x80), true);
    tft.drawText(xoff,40+16, "--- --- --- --- --- --- --- --- --- --- ", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0x80), false);
    tft.drawText(xoff,48+16, " a I s I d I f I g I h I j I k I l IRTNI", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), true);
    tft.drawText(xoff,64+16, "--- --- --- --- --- --- --- --- --- --- ", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), false);
    tft.drawText(xoff,72+16, " z I x I c I v I b I n I m I , IDELISPCI", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0x80), true);
    tft.drawText(xoff,88+16, "--- --- --- --- --- --- --- --- --- --- ", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0x80), false);
    tft.drawText(xoff,96+16, "joystick mode                           ", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0x00), false);
    delay(200);     
  }
  */
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
}

int handleMenu(uint16_t bClick)
{
  int action = ACTION_NONE;

  char newpath[80];
  strcpy(newpath, romspath);
  strcat(newpath, "/");
  strcat(newpath, selection);

  int rx=0,ry=0,rw=0,rh=0;

  if ( (bClick & MASK_JOY2_BTN) || (bClick & MASK_JOY1_BTN) ) {     
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
        action = ACTION_RUN1;               
      }
      menuRedraw=true;
  }
  else if ( bClick & MASK_KEY_USER1 ) {
      menuRedraw=true;
      strcpy(shown_selection,selection);
      action = ACTION_RUN2;    
  }
  else if ( bClick & MASK_KEY_USER2  ) {
      menuRedraw=true;  
      //action = ACTION_RUN3;    
      emu_SwapJoysticks(0);
  }  
  else if ( (bClick & MASK_JOY2_UP) || (bClick & MASK_JOY1_UP) ) {
    if (curFile!=0) {
      menuRedraw=true;
      curFile--;
    }
  }
  else if ( (bClick & MASK_JOY2_RIGHT) || (bClick & MASK_JOY1_RIGHT) ) {
    if ((curFile-9)>=0) {
      menuRedraw=true;
      curFile -= 9;
    } else if (curFile!=0) {
      menuRedraw=true;
      curFile--;
    }
  }  
  else if ( (bClick & MASK_JOY2_DOWN) || (bClick & MASK_JOY1_DOWN) )  {
    if ((curFile<(nbFiles-1)) && (nbFiles)) {
      curFile++;
      menuRedraw=true;
    }
  }
  else if ( (bClick & MASK_JOY2_LEFT) || (bClick & MASK_JOY1_LEFT) ) {
    if ((curFile<(nbFiles-9)) && (nbFiles)) {
      curFile += 9;
      menuRedraw=true;
    }
    else if ((curFile<(nbFiles-1)) && (nbFiles)) {
      curFile++;
      menuRedraw=true;
    }
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
//    tft.drawTextNoDma(48,MENU_JOYS_YOFFSET+8, "FLOPPY2:", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), false);
//    tft.drawRectNoDma(120,MENU_JOYS_YOFFSET+8, MENU_FILE_W, 8, RGBVAL16(0x00,0x00,0x00));
//    tft.drawTextNoDma(120,MENU_JOYS_YOFFSET+8, shown_selection, RGBVAL16(0xff,0xff,0xff), RGBVAL16(0x00,0x00,0x00), false);
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
  



int emu_GetMouse(int *x, int *y, int *buts) {
#ifdef HAS_USBKEY
  if (mouse1.available()) {
    *buts = mouse1.getButtons();
    *x = mouse1.getMouseX();
    *y = mouse1.getMouseY();
    mouse1.mouseDataClear();
    mouseDetected = true;
    return 1;
  }   
#endif
  return 0;
}

#ifdef HAS_USBKEY
void OnPress(auto key)
{
  keyboardDetected = true;
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
      case 32:
        usbnavpad |= MASK_KEY_USER1;
        break;
      case 9:
        usbnavpad |= MASK_KEY_USER2;
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
  keyboardDetected = true;
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
      case 32:
        usbnavpad &= ~MASK_KEY_USER1;
        break;
      case 9:
        usbnavpad &= ~MASK_KEY_USER2;
        break;
    }     
  }
  else
  {
    emu_KeyboardOnUp(keymodifier, key);  
  }
}
#endif

int emu_MouseDetected(void) {
  return (mouseDetected?1:0);
}

int emu_KeyboardDetected(void) {
  return (keyboardDetected?1:0); 
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
  
  toggleMenu(true);
}


void emu_start(void)
{
  usbnavpad = 0;
  
  //keys = key_map1;
  keyMap = 0;
}
