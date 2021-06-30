#define KEYMAP_PRESENT 1

#define PROGMEM

#include "pico.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>
#include <string.h>

extern "C" {
  #include "emuapi.h"
  #include "iopins.h"
}

#ifdef USE_VGA
#include "vga_t_dma.h"
#else
#include "tft_t_dma.h"
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
static char files[MAX_FILES][MAX_FILENAME_SIZE];
static bool menuRedraw=true;

#ifdef PICOMPUTER
static unsigned short * keys;
static unsigned char keymatrix[6];
static int keymatrix_hitrow=-1;
static bool key_fn=false;
#endif
static int keyMap;
   
static int keypadval=0; 
static bool joySwapped = false;
static uint16_t bLastState;
static int xRef;
static int yRef;
static uint8_t usbnavpad=0;

static bool menuOn=true;




void emu_printf(char * text)
{
  printf("%s\n",text);
}

void emu_printf(int val)
{
  printf("%d\n",val);
}

void emu_printi(int val)
{
  printf("%d\n",val);
}

void emu_printh(int val)
{
  printf("0x%.8\n",val);
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
  adc_select_input(0);
  int val = adc_read();
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
  adc_select_input(1);  
  int val = adc_read();
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
#ifdef PIN_JOY2_A1X
  int xReading = emu_ReadAnalogJoyX(0,256);
  if (xReading > 128) joysval |= MASK_JOY2_LEFT;
  else if (xReading < 128) joysval |= MASK_JOY2_RIGHT;
  
  int yReading = emu_ReadAnalogJoyY(0,256);
  if (yReading < 128) joysval |= MASK_JOY2_UP;
  else if (yReading > 128) joysval |= MASK_JOY2_DOWN;
#endif  
#ifdef PIN_JOY2_BTN
  joysval |= (gpio_get(PIN_JOY2_BTN) ? 0 : MASK_JOY2_BTN);
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
  if ( !gpio_get(PIN_JOY1_1) ) j2 |= MASK_JOY2_DOWN;
#endif
#ifdef PIN_JOY1_2
  if ( !gpio_get(PIN_JOY1_2) ) j2 |= MASK_JOY2_UP;
#endif
#else
#ifdef PIN_JOY1_1
  if ( !gpio_get(PIN_JOY1_1) ) j2 |= MASK_JOY2_UP;
#endif
#ifdef PIN_JOY1_2
  if ( !gpio_get(PIN_JOY1_2) ) j2 |= MASK_JOY2_DOWN;
#endif
#endif
#if INVX
#ifdef PIN_JOY1_3
  if ( !gpio_get(PIN_JOY1_3) ) j2 |= MASK_JOY2_LEFT;
#endif
#ifdef PIN_JOY1_4
  if ( !gpio_get(PIN_JOY1_4) ) j2 |= MASK_JOY2_RIGHT;
#endif
#else
#ifdef PIN_JOY1_3
  if ( !gpio_get(PIN_JOY1_3) ) j2 |= MASK_JOY2_RIGHT;
#endif
#ifdef PIN_JOY1_4
  if ( !gpio_get(PIN_JOY1_4) ) j2 |= MASK_JOY2_LEFT;
#endif
#endif
#ifdef PIN_JOY1_BTN
  if ( !gpio_get(PIN_JOY1_BTN) ) j2 |= MASK_JOY2_BTN;
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
  if ( !gpio_get(PIN_KEY_USER1) ) retval |= MASK_KEY_USER1;
#endif
#ifdef PIN_KEY_USER2 
  if ( !gpio_get(PIN_KEY_USER2) ) retval |= MASK_KEY_USER2;
#endif
#ifdef PIN_KEY_USER3 
  if ( !gpio_get(PIN_KEY_USER3) ) retval |= MASK_KEY_USER3;
#endif
#ifdef PIN_KEY_USER4 
  if ( !gpio_get(PIN_KEY_USER4) ) retval |= MASK_KEY_USER4;
#endif

#ifdef PICOMPUTER
  keymatrix_hitrow = -1;
  unsigned char row;
  unsigned short cols[6]={1,2,3,4,5,14};
  for (int i=0;i<6;i++){

    gpio_put(cols[i], 0);
    row=0;
    
    row |= (gpio_get(9) ? 0 : 0x01);
    row |= (gpio_get(9) ? 0 : 0x01);
    row |= (gpio_get(9) ? 0 : 0x01);
    row |= (gpio_get(9) ? 0 : 0x01);
    row |= (gpio_get(8) ? 0 : 0x02);
    row |= (gpio_get(6) ? 0 : 0x04);
    row |= (gpio_get(15) ? 0 : 0x08);
    row |= (gpio_get(7) ? 0 : 0x10);
    row |= (gpio_get(22) ? 0 : 0x20);
    /*
    if ( !gpio_get(9) ) row |= 0x01;
    if ( !gpio_get(9) ) row |= 0x01;
    if ( !gpio_get(9) ) row |= 0x01;
    if ( !gpio_get(9) ) row |= 0x01;
    if ( !gpio_get(8) ) row |= 0x02;
    if ( !gpio_get(6) ) row |= 0x04;
    if ( !gpio_get(15) ) row |= 0x08;  
    if ( !gpio_get(7) ) row |= 0x10;
    if ( !gpio_get(22) ) row |= 0x20;
    */
    keymatrix[i]=row;
    if (row) keymatrix_hitrow=i;
    gpio_put(cols[i], 1);
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
  if ( row & 0x20 ) retval |= MASK_KEY_USER1;
#endif

  //Serial.println(retval,HEX);

  if ( ((retval & (MASK_KEY_USER1+MASK_KEY_USER2)) == (MASK_KEY_USER1+MASK_KEY_USER2))
     || (retval & MASK_KEY_USER4 ) )
  {  
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
#ifdef PICOMPUTER
  if ( keymatrix[0] & 0x02 ) { 
    key_fn = true;
  }  
  if (key_fn) {
    keys = (unsigned short *)key_map2;    
  }
  else {
    keys = (unsigned short *)key_map1;    
  }
  if (keymatrix_hitrow >=0 ) {
    unsigned short match = ((unsigned short)keymatrix_hitrow<<8) | keymatrix[keymatrix_hitrow];  
    for (int i=0; i<sizeof(matkeys); i++) {
      if (match == matkeys[i]) {    
        key_fn = false;  
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
  gpio_set_pulls(PIN_JOY1_1,true,false);
  gpio_set_dir(PIN_JOY1_1,GPIO_IN);  
#endif  
#ifdef PIN_JOY1_2
  gpio_set_pulls(PIN_JOY1_2,true,false);
  gpio_set_dir(PIN_JOY1_2,GPIO_IN);  
#endif  
#ifdef PIN_JOY1_3
  gpio_set_pulls(PIN_JOY1_3,true,false);
  gpio_set_dir(PIN_JOY1_3,GPIO_IN);  
#endif  
#ifdef PIN_JOY1_4
  gpio_set_pulls(PIN_JOY1_4,true,false);
  gpio_set_dir(PIN_JOY1_4,GPIO_IN);  
#endif  
#ifdef PIN_JOY1_BTN
  gpio_set_pulls(PIN_JOY1_BTN,true,false);
  gpio_set_dir(PIN_JOY1_BTN,GPIO_IN);  
#endif  

#ifdef PIN_KEY_USER1
  gpio_set_pulls(PIN_KEY_USER1,true,false);
  gpio_set_dir(PIN_KEY_USER1,GPIO_IN);  
#endif  
#ifdef PIN_KEY_USER2
  gpio_set_pulls(PIN_KEY_USER2,true,false);
  gpio_set_dir(PIN_KEY_USER2,GPIO_IN);  
#endif  
#ifdef PIN_KEY_USER3
  gpio_set_pulls(PIN_KEY_USER3,true,false);
  gpio_set_dir(PIN_KEY_USER3,GPIO_IN);  
#endif  
#ifdef PIN_KEY_USER4
  gpio_set_pulls(PIN_KEY_USER4,true,false);
  gpio_set_dir(PIN_KEY_USER4,GPIO_IN);  
#endif  
#ifdef PIN_JOY2_BTN
  gpio_set_pulls(PIN_JOY2_BTN,true,false);
  gpio_set_dir(PIN_JOY2_BTN,GPIO_IN);  
#endif  

#ifdef PIN_JOY2_A1X
  adc_init(); 
  adc_gpio_init(PIN_JOY2_A1X);
  adc_gpio_init(PIN_JOY2_A2Y);
  xRef=0; yRef=0;
  for (int i=0; i<10; i++) {
    adc_select_input(0);  
    xRef += adc_read();
    adc_select_input(1);  
    yRef += adc_read();
    sleep_ms(20);
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

#ifdef PICOMPUTER
  gpio_init(1);
  gpio_init(2);
  gpio_init(3);
  gpio_init(4);
  gpio_init(5);
  gpio_init(14);

  gpio_init(6);
  gpio_init(9);
  gpio_init(15);
  gpio_init(8);
  gpio_init(7);
  gpio_init(22);
  gpio_set_dir(1, GPIO_OUT); 
  gpio_set_dir(2, GPIO_OUT); 
  gpio_set_dir(3, GPIO_OUT); 
  gpio_set_dir(4, GPIO_OUT); 
  gpio_set_dir(5, GPIO_OUT); 
  gpio_set_dir(14, GPIO_OUT); 
  gpio_put(1, 1);
  gpio_put(2, 1);
  gpio_put(3, 1);
  gpio_put(4, 1);
  gpio_put(5, 1);
  gpio_put(14, 1);
  gpio_set_pulls(6,true,false);
  gpio_set_dir(6,GPIO_IN);  
  gpio_set_pulls(9,true,false);
  gpio_set_dir(9,GPIO_IN);  
  gpio_set_pulls(15,true,false);
  gpio_set_dir(15,GPIO_IN);  
  gpio_set_pulls(8,true,false);
  gpio_set_dir(8,GPIO_IN);  
  gpio_set_pulls(7,true,false);
  gpio_set_dir(7,GPIO_IN);  
  gpio_set_pulls(22,true,false);
  gpio_set_dir(22,GPIO_IN);  
#endif
}




int emu_setKeymap(int index) {
}


#include "ff.h"
static FATFS fatfs;
static FIL file; 
extern "C" int sd_init_driver(void);

static int readNbFiles(char * rootdir) {
  int totalFiles = 0;

  DIR dir;
  FILINFO entry;
  FRESULT fr = f_findfirst(&dir, &entry, rootdir, "*");
  while ( (fr == FR_OK) && (entry.fname[0]) && (totalFiles<MAX_FILES) ) {  
    if (!entry.fname[0]) {
      // no more files
      break;
    }
    char * filename = entry.fname;   
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
    fr = f_findnext(&dir, &entry);  
  } 
  f_closedir(&dir);

  return totalFiles;  
}  



void backgroundMenu(void) {
    menuRedraw=true;  
    tft.fillScreenNoDma(RGBVAL16(0x00,0x00,0x00));
    tft.drawTextNoDma(0,0, TITLE, RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), true);           
}

int handleMenu(uint16_t bClick)
{
  int action = ACTION_NONE;

  char newpath[80];
  strcpy(newpath, romspath);
  strcat(newpath, "/");
  strcat(newpath, selection);
  if ( (bClick & MASK_JOY2_BTN) ) {     
    emu_printf(newpath);
    DIR dir;
    FILINFO entry;
    FRESULT fr;
    fr = f_findfirst(&dir, &entry, romspath, selection);
    if ( (fr == FR_OK) && (entry.fattrib & AM_DIR) ) {
        printf("we enter %s/%s\n", romspath, selection);     
        strcpy(romspath,newpath);
        curFile = 0;
        nbFiles = readNbFiles(newpath);             
    }
    else
    {
      action = ACTION_RUNTFT;               
    }
    menuRedraw=true;
  }
  else if ( (bClick & MASK_KEY_USER1) ) {
      menuRedraw=true;
      action = ACTION_RUNVGA;    
  }
  else if (bClick & MASK_JOY2_UP) {
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
  else if (bClick & MASK_JOY2_DOWN)  {
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
  else if ( (bClick & MASK_KEY_USER2) ) {
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
    menuOn = true;
    backgroundMenu();
  } else {
    menuOn = false;    
  }
}

char * menuSelection(void)
{
  return (selection);  
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
  if( !(f_open(&file, filepath, FA_READ)) ) {
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
  unsigned int retval=0; 
  if (size < 256) {
    if( !(f_read (&file, buffer, size, &retval)) )
    if (retval>0) {
      memcpy(buf,buffer,retval);
      byteread += retval;   
    }   
  }
  else {
    while (remaining>0) {
      if( !(f_read (&file, buffer, 256, &retval)) )
      //f_read (&file, buffer, 256, &retval);
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
  unsigned int retval=0;
  if( !(f_read (&file, &c, 1, &retval)) )
  if (retval != 1) {
    emu_printf("emu_FileGetc failed");
  }  
  return c; 
}


void emu_FileClose(void)
{
  f_close(&file); 
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
  FILINFO entry;
  f_stat(filepath, &entry);
  filesize = entry.fsize; 
  return(filesize);    
}

int emu_FileSeek(int seek) 
{
  f_lseek(&file, seek);
  return (seek);
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
  if( !(f_open(&file, filepath, FA_READ)) ) {
    filesize = f_size(&file);
    emu_printf(filesize);
    if (size >= filesize)
    {
      unsigned int retval=0;
      if( (f_read (&file, buf, filesize, &retval)) ) {
        emu_printf("File read failed");        
      }
    }
    f_close(&file);
  }
 
  return(filesize);
}

#ifdef SDIO


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
#endif



void emu_init(void)
{
  sd_init_driver(); 
  FRESULT fr = f_mount(&fatfs, "0:", 1);    
  //emu_FileSystemInit();

  strcpy(romspath,ROMSDIR);
  nbFiles = readNbFiles(romspath); 

  emu_printf("SD initialized, files found: ");
  emu_printi(nbFiles);
  
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

  keyMap = 0;
}
