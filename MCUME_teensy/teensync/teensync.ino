#include "iopins.h"  
#include "emuapi.h"  

#include "tft_t_dma.h"
#include "bmpvbar.h"
//#define USE_SD 1
#ifdef USE_SD
#include <SD.h>
#endif
#include "uSDFS.h"


// Title:     <                                        >
#define TITLE "       Teensy Norton Commander          "

#define MAX_PATHNAME_SIZE   128
#define MAX_FILES           32
#define MIN_FILENAME_SIZE   16
#define MAX_FILENAME_SIZE   32
#define MAX_MENULINES       ((TEXT_HEIGHT == 16)?10:20)
#define TEXT_HEIGHT         8
#define TEXT_WIDTH          8
#define MENU_FILE_LXOFFSET  0
#define MENU_FILE_RXOFFSET  (TFT_WIDTH/2+2*8)
#define MENU_FILE_YOFFSET   (3*TEXT_HEIGHT)
#define MENU_FILE_W         (MIN_FILENAME_SIZE*TEXT_WIDTH)
#define MENU_FILE_H         (MAX_MENULINES*TEXT_HEIGHT)
#define MENU_FILE_SBGCOLOR  RGBVAL16(0x00,0x00,0xA0)
#define MENU_FILE_UBGCOLOR  RGBVAL16(0x00,0x00,0x50)
#define MENU_VBAR_XOFFSET   (TFT_WIDTH/2-3*8)
#define MENU_VBAR_YOFFSET   (MENU_FILE_YOFFSET)

#define LEFT_PANEL          0
#define RIGHT_PANEL         1

#define CHUNK_SIZE          1024


TFT_T_DMA tft = TFT_T_DMA(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO, TFT_TOUCH_CS, TFT_TOUCH_INT);

static int SelectedPanel = LEFT_PANEL; 

#ifdef USE_SD
static char RIGHTpath[MAX_PATHNAME_SIZE]="";
#else
const char RIGHTpath[MAX_PATHNAME_SIZE]={'1',':',0};
#endif
static int  RIGHTnbFiles=0;
static int  RIGHTcurFile=0;
static int  RIGHTtopFile=0;
static char RIGHTselection[MAX_FILENAME_SIZE+1]="";
static char RIGHTfiles[MAX_FILES][MAX_FILENAME_SIZE];

const char LEFTpath[MAX_PATHNAME_SIZE]={'2',':',0};
static FATFS fatfs;
static int  LEFTnbFiles=0;
static int  LEFTcurFile=0;
static int  LEFTtopFile=0;
static char LEFTselection[MAX_FILENAME_SIZE+1]={0};
static char LEFTfiles[MAX_FILES][MAX_FILENAME_SIZE];

static int RIGHTreadNbFiles(void) {
  int totalFiles = 0;
#ifdef USE_SD  
  File dir;
  File entry;    
  dir = SD.open(RIGHTpath);
#else
  DIR dir;
  FILINFO entry; 
  f_opendir(&dir, RIGHTpath); 
#endif  
  while ( (true) && (totalFiles<MAX_FILES) ) {
#ifdef USE_SD   
    entry = dir.openNextFile();
    if (!entry) {
#else
    f_readdir(&dir, &entry);
    if (!entry.fname[0]) {      
#endif
      // no more files
      break;
    }

#ifdef USE_SD
    char * filename = entry.name();
    //Serial.println(filename); 
    if (!entry.isDirectory())  {
#else
    char * filename = entry.fname;
    //Serial.print(filename);
    if ( !(entry.fattrib & AM_DIR) ) {
#endif
      strncpy(&RIGHTfiles[totalFiles][0], filename, MAX_FILENAME_SIZE-1);
      totalFiles++;
    }
    else {
      if ( (strcmp(filename,".")) && (strcmp(filename,"..")) ) {
        strncpy(&RIGHTfiles[totalFiles][0], filename, MAX_FILENAME_SIZE-1);
        totalFiles++;
      }
    }  
#ifdef USE_SD    
    entry.close();
#endif    
  }
#ifdef USE_SD  
  dir.close();
#endif  
  return totalFiles;  
}  

static int LEFTreadNbFiles(void) {
  int totalFiles = 0;
  DIR dir;
  FILINFO entry; 
  f_opendir(&dir, LEFTpath);  
  while ( (true) && (totalFiles<MAX_FILES) ) {
    f_readdir(&dir, &entry);
    if (!entry.fname[0]) {
      // no more files
      break;
    }
    char * filename = entry.fname;
    //Serial.print(filename);
    if ( !(entry.fattrib & AM_DIR) ) {
      strncpy(&LEFTfiles[totalFiles][0], filename, MAX_FILENAME_SIZE-1);
      totalFiles++;
    }
    else {
      if ( (strcmp(filename,".")) && (strcmp(filename,"..")) ) {
        strncpy(&LEFTfiles[totalFiles][0], filename, MAX_FILENAME_SIZE-1);
        totalFiles++;
      }
    }  
  }
  return totalFiles;  
}  

static int RIGHThandleMenu(uint16_t bClick, uint16_t bgColor, bool menuRedraw)
{
  char newpath[MAX_PATHNAME_SIZE];
  strcpy(newpath, RIGHTpath);
  strcat(newpath, "/");
  strcat(newpath, RIGHTselection);
  int retval = 0;
  if (bClick & MASK_JOY2_BTN)  {
#ifdef USE_SD  
      File file = SD.open(newpath);
      if (file.isDirectory())  {
#else
      FILINFO entry; 
      f_stat(newpath, &entry);
      if ( (entry.fattrib & AM_DIR) ) {
#endif
        strcpy(RIGHTpath,newpath);
        RIGHTcurFile = 0;
        RIGHTnbFiles = RIGHTreadNbFiles();             
      }
      else {
        retval = 1;               
      }
      menuRedraw=true;
  }
  else if (bClick & MASK_JOY2_UP) {
    if (RIGHTcurFile!=0) {
      menuRedraw=true;
      RIGHTcurFile--;
    }
  }
  else if (bClick & MASK_JOY2_DOWN)  {
    if ((RIGHTcurFile<(RIGHTnbFiles-1)) && (RIGHTnbFiles)) {
      RIGHTcurFile++;
      menuRedraw=true;
    }
  }

  if (menuRedraw && RIGHTnbFiles) {
    int fileIndex = 0;
    tft.drawRectNoDma(MENU_FILE_RXOFFSET,MENU_FILE_YOFFSET, MENU_FILE_W, MENU_FILE_H, bgColor);
    if (RIGHTcurFile <= (MAX_MENULINES/2-1)) RIGHTtopFile=0;
    else RIGHTtopFile=RIGHTcurFile-(MAX_MENULINES/2);
//    if (RIGHTcurFile <= (MAX_MENULINES-1)) RIGHTtopFile=0;
//    else RIGHTtopFile=RIGHTcurFile-(MAX_MENULINES/2);

    int i=0;
    while (i<MAX_MENULINES) {
      if (fileIndex>=RIGHTnbFiles) {
          // no more files
          break;
      }
      char filename[MAX_FILENAME_SIZE];
      strncpy(filename,&RIGHTfiles[fileIndex][0],MAX_FILENAME_SIZE);    
      if (fileIndex >= RIGHTtopFile) {              
        if ((i+RIGHTtopFile) < RIGHTnbFiles ) {
          if ((i+RIGHTtopFile)==RIGHTcurFile) {
            strcpy(RIGHTselection,filename);
            if (strlen(filename) > MIN_FILENAME_SIZE) {filename[MIN_FILENAME_SIZE] = 0;}
            tft.drawTextNoDma(MENU_FILE_RXOFFSET,i*TEXT_HEIGHT+MENU_FILE_YOFFSET, filename, RGBVAL16(0xff,0xff,0x00), RGBVAL16(0xff,0x00,0x00), (TEXT_HEIGHT == 16));           
            strcpy(newpath, RIGHTpath);
            strcat(newpath, "/");
            strcat(newpath, RIGHTselection);
            tft.drawRectNoDma(0,MENU_FILE_YOFFSET+MENU_FILE_H, TFT_WIDTH, 16, RGBVAL16(0x00,0x80,0x00));
#ifdef USE_SD           
            File file = SD.open(newpath);
            int fsize =  (int)file.size();       
            if ( file.isDirectory() ) {
#else
            FILINFO entry;
            f_stat(newpath, &entry);
            int fsize = (int)entry.fsize;            
            if ( (entry.fattrib & AM_DIR) ) {
#endif
              tft.drawTextNoDma(0,MENU_FILE_YOFFSET+MENU_FILE_H, "Dir", RGBVAL16(0xff,0xff,0xff), RGBVAL16(0x00,0x80,0x00), true);
            }
            else {
              tft.drawTextNoDma(0,MENU_FILE_YOFFSET+MENU_FILE_H, "File", RGBVAL16(0xff,0xff,0xff), RGBVAL16(0x00,0x80,0x00), true);
              tft.drawTextNoDma(5*8,MENU_FILE_YOFFSET+MENU_FILE_H, String(fsize).c_str(), RGBVAL16(0xff,0xff,0xff), RGBVAL16(0x00,0x80,0x00), true);
            }
 
          }
          else {
            if (strlen(filename) > MIN_FILENAME_SIZE) {filename[MIN_FILENAME_SIZE] = 0;}
            tft.drawTextNoDma(MENU_FILE_RXOFFSET,i*TEXT_HEIGHT+MENU_FILE_YOFFSET, filename, RGBVAL16(0xff,0xff,0xff), bgColor, (TEXT_HEIGHT == 16));      
          }
        }
        i++; 
      }
      fileIndex++;    
    }  
  }

  return(retval);  
}



static int LEFThandleMenu(uint16_t bClick, uint16_t bgColor, bool menuRedraw)
{  
  char newpath[MAX_PATHNAME_SIZE];
  strcpy(newpath, LEFTpath);
  strcat(newpath, "/");
  strcat(newpath, LEFTselection);
  int retval = 0;
  if (bClick & MASK_JOY2_BTN) {
      FILINFO entry;
      f_stat(newpath, &entry);
      if ( (entry.fattrib & AM_DIR) ) {
        strcpy(LEFTpath,newpath);        
        LEFTcurFile = 0;
        LEFTnbFiles = LEFTreadNbFiles();             
      }
      else {
        retval = 1;               
      }
      menuRedraw=true;
  }
  else if (bClick & MASK_JOY2_UP) {
    if (LEFTcurFile!=0) {
      menuRedraw=true;
      LEFTcurFile--;
    }
  }
  else if (bClick & MASK_JOY2_DOWN)  {
    if ((LEFTcurFile<(LEFTnbFiles-1)) && (LEFTnbFiles)) {
      LEFTcurFile++;
      menuRedraw=true;
    }
  }

  if (menuRedraw && LEFTnbFiles) {
    int fileIndex = 0;
    tft.drawRectNoDma(MENU_FILE_LXOFFSET,MENU_FILE_YOFFSET, MENU_FILE_W, MENU_FILE_H, bgColor);
    if (LEFTcurFile <= (MAX_MENULINES/2-1)) LEFTtopFile=0;
    else LEFTtopFile=LEFTcurFile-(MAX_MENULINES/2);
//    if (LEFTcurFile <= (MAX_MENULINES-1)) LEFTtopFile=0;
//    else LEFTtopFile=LEFTcurFile-(MAX_MENULINES/2);

    int i=0;
    while (i<MAX_MENULINES) {
      if (fileIndex>=LEFTnbFiles) {
          // no more files
          break;
      }   
      char filename[MAX_FILENAME_SIZE];
      strncpy(filename,&LEFTfiles[fileIndex][0],MAX_FILENAME_SIZE);      
      if (fileIndex >= LEFTtopFile) {              
        if ((i+LEFTtopFile) < LEFTnbFiles ) {
          if ((i+LEFTtopFile)==LEFTcurFile) {
            strcpy(LEFTselection,filename);
            if (strlen(filename) > MIN_FILENAME_SIZE) {filename[MIN_FILENAME_SIZE] = 0;}
            tft.drawTextNoDma(MENU_FILE_LXOFFSET,i*TEXT_HEIGHT+MENU_FILE_YOFFSET, filename, RGBVAL16(0xff,0xff,0x00), RGBVAL16(0xff,0x00,0x00), (TEXT_HEIGHT == 16));
            strcpy(newpath, LEFTpath);
            strcat(newpath, "/");
            strcat(newpath, LEFTselection);           
            tft.drawRectNoDma(0,MENU_FILE_YOFFSET+MENU_FILE_H, TFT_WIDTH, 16, RGBVAL16(0x00,0x80,0x00));
            FILINFO entry;
            f_stat(newpath, &entry);
            int fsize = (int)entry.fsize;
            if ( (entry.fattrib & AM_DIR) ) {
              tft.drawTextNoDma(0,MENU_FILE_YOFFSET+MENU_FILE_H, "Dir", RGBVAL16(0xff,0xff,0xff), RGBVAL16(0x00,0x80,0x00), true);
            }
            else {
              tft.drawTextNoDma(0,MENU_FILE_YOFFSET+MENU_FILE_H, "File", RGBVAL16(0xff,0xff,0xff), RGBVAL16(0x00,0x80,0x00), true);
              tft.drawTextNoDma(5*8,MENU_FILE_YOFFSET+MENU_FILE_H, String(fsize).c_str(), RGBVAL16(0xff,0xff,0xff), RGBVAL16(0x00,0x80,0x00), true);
            }
          }
          else {
            if (strlen(filename) > MIN_FILENAME_SIZE) {filename[MIN_FILENAME_SIZE] = 0;}
            tft.drawTextNoDma(MENU_FILE_LXOFFSET,i*TEXT_HEIGHT+MENU_FILE_YOFFSET, filename, RGBVAL16(0xff,0xff,0xff), bgColor, (TEXT_HEIGHT == 16));      
          }
        }
        i++; 
      }
      fileIndex++;    
    }   
  }

  return(retval);  
}

// ****************************************************
// the setup() method runs once, when the sketch starts
// ****************************************************
void setup() {
  tft.begin();

  emu_init();
  Serial.begin(115200);
  //while (!Serial) {}

Serial.println("go");
  // mount FS
#ifdef USE_SD  
  if (!SD.begin(SD_CS)) {  
    emu_printf("SdFat.begin() for SD failed");
  }
#else
  if(f_mount (&fatfs, RIGHTpath, 1)) { 
    emu_printf("f_mount() for RIGHT MSC failed");
  }
#endif  
Serial.println("sd");
  if(f_mount (&fatfs, LEFTpath, 1)) { 
    emu_printf("f_mount() for LEFT MSC failed");
  }
Serial.println("usb");
  
  // Read root dir
  RIGHTnbFiles  = RIGHTreadNbFiles(); 
  LEFTnbFiles = LEFTreadNbFiles();

  // Init display
  tft.fillScreenNoDma(RGBVAL16(0x00,0x00,0x00));
  tft.drawTextNoDma(0,0, TITLE, RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), true);  
  tft.drawSpriteNoDma(MENU_VBAR_XOFFSET,MENU_VBAR_YOFFSET,(uint16_t*)bmpvbar);
  LEFThandleMenu(0,MENU_FILE_SBGCOLOR,true);
  RIGHThandleMenu(0,MENU_FILE_UBGCOLOR,true);
}



// ****************************************************
// the loop() method runs continuously
// ****************************************************
void loop(void) 
{
  uint16_t bClick = emu_DebounceLocalKeys();
  if (bClick & MASK_JOY2_LEFT) {
    SelectedPanel = LEFT_PANEL;
    LEFThandleMenu(0,MENU_FILE_SBGCOLOR,true); 
    RIGHThandleMenu(0,MENU_FILE_UBGCOLOR,true);    
  }
  else if (bClick & MASK_JOY2_RIGHT) {
      SelectedPanel = RIGHT_PANEL; 
      LEFThandleMenu(0,MENU_FILE_UBGCOLOR,true);
      RIGHThandleMenu(0,MENU_FILE_SBGCOLOR,true);    
  }
    
  if (SelectedPanel == LEFT_PANEL) {
    int action = LEFThandleMenu(bClick,MENU_FILE_SBGCOLOR,false);
    if (action) 
    {
      char srcpath[MAX_PATHNAME_SIZE];
      strcpy(srcpath, LEFTpath);
      strcat(srcpath, "/");
      strcat(srcpath, LEFTselection);
      FILINFO entry;
      f_stat(srcpath, &entry);
      int filesize = (int)entry.fsize;
      
      char dstpath[MAX_PATHNAME_SIZE];
      strcpy(dstpath, RIGHTpath);
      strcat(dstpath, "/");
      strcat(dstpath, LEFTselection);        
      emu_printf(srcpath);
      emu_printf(dstpath);
      if (filesize) {     
#ifdef XXX        
        File file = SD.open(&dstpath[1], O_WRITE);
        if (file) {
          FIL fil;          
          if(!(f_open(&fil, srcpath, FA_READ)) {
            int max=ILI9341_TFTWIDTH;
            tft.drawRectNoDma(0,MENU_FILE_YOFFSET+MENU_FILE_H+16, ILI9341_TFTWIDTH, 16, RGBVAL16(0x00,0x00,0x00));
            unsigned char buffer[CHUNK_SIZE];
            int remaining = filesize;
            int byteread = 0; 
            while (remaining >= CHUNK_SIZE) {
              //int retval = file.read(buffer, CHUNK_SIZE);
              unsigned int retval;
              f_read (&fil, buffer, CHUNK_SIZE, &retval);
              if (retval>0) {
                //memcpy(buf,buffer,retval);
                //buf += retval;
                byteread += retval;     
                remaining -= retval;
                tft.drawRectNoDma(0,MENU_FILE_YOFFSET+MENU_FILE_H+16, (ILI9341_TFTWIDTH*byteread)/filesize, 16, RGBVAL16(0xff,0x00,0x00));          
              }
            }
            if (remaining) {
              //int retval = file.read(buffer, remaining);
              unsigned int retval;
              f_read (&fil, buffer, remaining, &retval);
              if (retval>0) {
                //memcpy(buf,buffer,retval);
                byteread += retval;
                tft.drawRectNoDma(0,MENU_FILE_YOFFSET+MENU_FILE_H+16, (ILI9341_TFTWIDTH*byteread)/filesize, 16, RGBVAL16(0xff,0x00,0x00));
              }
            }           
           
            f_close(&fil);
            RIGHTcurFile = 0;
            RIGHTnbFiles = RIGHTreadNbFiles();
            RIGHThandleMenu(0,MENU_FILE_UBGCOLOR,true); 
            LEFThandleMenu(0,MENU_FILE_SBGCOLOR,true); 
          }
          file.close(); 
        }
        else {
          emu_printf("failed opening RIGHT");
        }
#endif        
      }  
    }
  }
  else {
   RIGHThandleMenu(bClick,MENU_FILE_SBGCOLOR,false);
  }
       
  delay(20);  
}








