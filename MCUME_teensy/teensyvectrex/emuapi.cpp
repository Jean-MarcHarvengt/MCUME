#define KEYMAP_PRESENT 1

//extern "C" {
  #include "emuapi.h"
  #include "iopins.h"
//}

#include "tft_t_dma.h"
#include "logo.h"
#include "bmpjoy.h"
#include "bmpvbar.h"
#include "bmpvga.h"
#include "bmptft.h"

#ifndef SD_CS
#define USE_SDFAT 1
#endif

#ifdef HAS_I2CKBD
#include <Wire.h>
#endif

#ifdef USE_SDFAT
#include <SdFat.h>
static SdFatSdio SD;
#else
#include <SD.h>
#endif

extern TFT_T_DMA tft;
static File file;
static char romspath[64];
static int16_t calMinX=-1,calMinY=-1,calMaxX=-1,calMaxY=-1;
static bool i2cKeyboardPresent = false;
//const uint16_t deflogo[] = {
//  0x0000,0x0000
//};
//static const uint16_t * logo = deflogo;

#define CALIBRATION_FILE    "/cal.cfg"

#define MAX_FILES           32
#define MAX_FILENAME_SIZE   28
#define MAX_MENULINES       (MKEY_L9)
#define TEXT_HEIGHT         16
#define TEXT_WIDTH          8
#define MENU_FILE_XOFFSET   (6*TEXT_WIDTH)
#define MENU_FILE_YOFFSET   (2*TEXT_HEIGHT)
#define MENU_FILE_W         (MAX_FILENAME_SIZE*TEXT_WIDTH)
#define MENU_FILE_H         (MAX_MENULINES*TEXT_HEIGHT)
#define MENU_FILE_BGCOLOR   RGBVAL16(0x00,0x00,0x20)
#define MENU_JOYS_YOFFSET   (12*TEXT_HEIGHT)
#define MENU_VBAR_XOFFSET   (0*TEXT_WIDTH)
#define MENU_VBAR_YOFFSET   (MENU_FILE_YOFFSET)

#define MENU_TFT_XOFFSET    (MENU_FILE_XOFFSET+MENU_FILE_W+8)
#define MENU_TFT_YOFFSET    (MENU_VBAR_YOFFSET+32)
#define MENU_VGA_XOFFSET    (MENU_FILE_XOFFSET+MENU_FILE_W+8)
#define MENU_VGA_YOFFSET    (MENU_VBAR_YOFFSET+MENU_FILE_H-32-37)


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

  
static bool menuOn=true;
static bool callibrationOn=false;
static int callibrationStep=0;
static bool menuRedraw=true;
static int nbFiles=0;
static int curFile=0;
static int topFile=0;
static char selection[MAX_FILENAME_SIZE+1]="";
static uint8_t prev_zt=0; 


static char files[MAX_FILES][MAX_FILENAME_SIZE];

static int readNbFiles(void) {
  int totalFiles = 0;
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
  return totalFiles;  
}  

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

void toggleMenu(bool on) {
  if (on) {
    callibrationOn=false;
    menuOn=true;
    menuRedraw=true;  
    tft.fillScreenNoDma(RGBVAL16(0x00,0x00,0x00));
    tft.drawTextNoDma(0,0, TITLE, RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), true);  
    tft.drawSpriteNoDma(MENU_VBAR_XOFFSET,MENU_VBAR_YOFFSET,(uint16_t*)bmpvbar);
    tft.drawSpriteNoDma(MENU_TFT_XOFFSET,MENU_TFT_YOFFSET,(uint16_t*)bmptft);
    tft.drawSpriteNoDma(MENU_VGA_XOFFSET,MENU_VGA_YOFFSET,(uint16_t*)bmpvga);
  } else {
    menuOn = false;    
  }
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
  else {
    Serial.println("Callibration read error");
  }  
  tft.callibrateTouch(calMinX,calMinY,calMaxX,calMaxY);   
}

static void writeCallibration(void) 
{
  tft.callibrateTouch(calMinX,calMinY,calMaxX,calMaxY);
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




bool menuActive(void) 
{
  return (menuOn);
}

int handleMenu(uint16_t bClick)
{
  int action = ACTION_NONE;

  char newpath[80];
  strcpy(newpath, romspath);
  strcat(newpath, "/");
  strcat(newpath, selection);

  int rx=0,ry=0,rw=0,rh=0;
  char c = captureTouchZone(menutouchareas, menutouchactions, &rx,&ry,&rw,&rh);
 
  if ( (bClick & MASK_JOY2_BTN) || (c == MKEY_TFT) ) {
      File file = SD.open(newpath);
      emu_printf(newpath);
      if (file.isDirectory())  {
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
            tft.drawTextNoDma(MENU_FILE_XOFFSET,i*TEXT_HEIGHT+MENU_FILE_YOFFSET, filename, 0xFFFF, 0x0000, true);      
          }
        }
        i++; 
      }
      fileIndex++;    
    }

    tft.drawSpriteNoDma(0,MENU_JOYS_YOFFSET,(uint16_t*)bmpjoy);  
    tft.drawTextNoDma(48,MENU_JOYS_YOFFSET+8, (emu_SwapJoysticks(1)?(char*)"SWAP=1":(char*)"SWAP=0"), RGBVAL16(0x00,0xff,0xff), RGBVAL16(0xff,0x00,0x00), false);
    menuRedraw=false;     
  }

  return (action);  
}

char * menuSelection(void)
{
  return (selection);  
}
  

 

void emu_init(void)
{
  Serial.begin(115200);
  //while (!Serial) {}
#ifdef USE_SDFAT
  if (!SD.begin()) {
#else
  if (!SD.begin(SD_CS)) {
#endif    
    emu_printf("SdFat.begin() failed");
  }
  strcpy(romspath,ROMSDIR);
  nbFiles = readNbFiles(); 

  Serial.print("SD initialized, files found: ");
  Serial.println(nbFiles);

  emu_InitJoysticks();
  readCallibration();
 
  if ((tft.isTouching()) || (emu_ReadKeys() & MASK_JOY2_BTN) ) {
    callibrationInit();
  } 
  else  
  {
    toggleMenu(true);
  }

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
  Serial.println(val);
}

void * emu_Malloc(int size)
{
  void * retval =  malloc(size);
  if (!retval) {
    emu_printf("failled to allocate ");
    emu_printf(size);
  }
  else {
    emu_printf("could allocate ");
    emu_printf(size);    
  }
  
  return retval;
}

void emu_Free(void * pt)
{
  free(pt);
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
    
  if ((file = SD.open(filepath, O_READ))) {
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
  while (remaining >= 256) {
    int retval = file.read(buffer, 256);
    if (retval>0) {
      memcpy(buf,buffer,retval);
      buf += retval;
      byteread += retval;     
      remaining -= retval;
    }
  }
  if (remaining) {
    int retval = file.read(buffer, remaining);
    if (retval>0) {
      memcpy(buf,buffer,retval);
      byteread += retval;
    }
  }    
  return byteread; 
}

unsigned char emu_FileGetc(void) {
  unsigned char c;
  int retval = file.read(&c, 1);
  if (retval != 1) {
    emu_printf("emu_FileGetc failed");
  }  
  return c; 
}


void emu_FileClose(void)
{
  file.close();  
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

  if ((file = SD.open(filepath, O_READ))) 
  {
    emu_printf("filesize is...");
    filesize = file.size(); 
    emu_printf(filesize);
    file.close();    
  }
 
  return(filesize);    
}

int emu_FileSeek(int seek) 
{
  file.seek(seek);
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
  
  if ((file = SD.open(filepath, O_READ))) 
  {
    file.seek(seek);
    emu_printf(size);
    if (file.read(buf, size) != size) {
      emu_printf("File read failed");
    }        
    file.close();
  }
  
  return(filesize);
}

static int keypadval=0; 
static boolean joySwapped = false;
static uint16_t bLastState;
static int xRef;
static int yRef;


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
  return(keypadval|((joySwapped?1:0)<<7));
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

  //Serial.println(retval,HEX);

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
        tft.drawSpriteNoDma(0,0,(uint16_t*)logo);
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
            tft.drawSpriteNoDma(0,0,(uint16_t*)logo);           
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
        tft.drawSpriteNoDma(0,0,(uint16_t*)logo, rx, ry, rw, rh);
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
  if (index) {   
  }
  else {
  }
}



