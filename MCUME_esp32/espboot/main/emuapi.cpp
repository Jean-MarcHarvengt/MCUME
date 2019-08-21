#define KEYMAP_PRESENT 1

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

extern "C" {
  #include "emuapi.h"
  #include "iopins.h"
}

#include "ili9341_t3dma.h"
//#include "logo.h"
#include "bmpjoy.h"
#include "bmpvbar.h"

#include "esp_event.h"
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include <dirent.h>
#include <sys/stat.h>
#include <driver/adc.h>

#ifdef HAS_I2CKBD
#ifdef USE_WIRE
#include "Wire.h"
#else
#include <driver/i2c.h>
#define ACK_CHECK_EN          0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS         0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL               0x0              /*!< I2C ack value */
#define NACK_VAL              0x1              /*!< I2C nack value */
#endif

#define I2C_FREQ_HZ           400000           /*!< I2C master clock frequency */
static bool i2cKeyboardPresent = false;
#endif


extern ILI9341_t3DMA tft;

static char romspath[64];
static int calMinX=-1,calMinY=-1,calMaxX=-1,calMaxY=-1;
static sdmmc_card_t* card;
const uint16_t deflogo[] = {
  0x0000,0x0000
};
static const uint16_t * logo = deflogo;

#define CALIBRATION_FILE    "/sdcard/cal.cfg"

#define MAX_FILENAME_SIZE   28
#define MAX_MENULINES       (MKEY_L9)
#define TEXT_HEIGHT         16
#define TEXT_WIDTH          8
#define MENU_FILE_XOFFSET   (6*TEXT_WIDTH)
#define MENU_FILE_YOFFSET   (2*TEXT_HEIGHT)
#define MENU_FILE_W         (MAX_FILENAME_SIZE*TEXT_WIDTH)
#define MENU_FILE_H         (MAX_MENULINES*TEXT_HEIGHT)
#define MENU_FILE_FGCOLOR   RGBVAL16(0xff,0xff,0xff)
#define MENU_FILE_BGCOLOR   RGBVAL16(0x00,0x00,0x20)
#define MENU_JOYS_YOFFSET   (12*TEXT_HEIGHT)
#define MENU_VBAR_XOFFSET   (0*TEXT_WIDTH)
#define MENU_VBAR_YOFFSET   (MENU_FILE_YOFFSET)


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


const unsigned short menutouchareas[] = {
  TAREA_XY,MENU_FILE_XOFFSET,MENU_FILE_YOFFSET,
  TAREA_WH,MENU_FILE_W, TEXT_HEIGHT,
  TAREA_NEW_COL,TEXT_HEIGHT,TEXT_HEIGHT,TEXT_HEIGHT,TEXT_HEIGHT,TEXT_HEIGHT,TEXT_HEIGHT,TEXT_HEIGHT,TEXT_HEIGHT,TEXT_HEIGHT,
  
  TAREA_XY,MENU_VBAR_XOFFSET,MENU_VBAR_YOFFSET,
  TAREA_WH,32,48,
  TAREA_NEW_COL, 72,72,8,40,
    
  TAREA_END};
   
const unsigned short menutouchactions[] = {
  MKEY_L1,MKEY_L2,MKEY_L3,MKEY_L4,MKEY_L5,MKEY_L6,MKEY_L7,MKEY_L8,MKEY_L9,
  MKEY_UP,MKEY_DOWN,ACTION_NONE,MKEY_JOY}; 

  
static bool menuOn=true;
static bool callibrationOn=false;
static int callibrationStep=0;
static bool menuRedraw=true;
static int nbFiles=0;
static int curFile=0;
static int topFile=0;
static char selection[MAX_FILENAME_SIZE+1]="";
static uint8_t prev_zt=0; 

static int readNbFiles(void) {
  int totalFiles = 0;

  DIR* dir = opendir(romspath);
  while (true) {
    struct dirent* de = readdir(dir);
    if (!de) {
      // no more files
      break;
    }    
    if (de->d_type == DT_REG) {
      totalFiles++;
    }
    else if (de->d_type == DT_DIR) {
      if ( (strcmp(de->d_name,".")) && (strcmp(de->d_name,"..")) ) {
        totalFiles++;
      }
    }  
  }
  closedir(dir);
  printf("Directory read: %d files",totalFiles);
  return totalFiles;  
}

static char captureTouchZone(const unsigned short * areas, const unsigned short * actions, int *rx, int *ry, int *rw, int * rh) {
    uint16_t xt=0;
    uint16_t yt=0;
    uint16_t zt=0;
    bool hDir=true;  
  
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
  FILE * file = fopen(CALIBRATION_FILE, "rb");
  if (file) {
    fscanf(file,"%d %d %d %d\n",&calMinX,&calMinY,&calMaxX,&calMaxY);
    fclose(file);
    printf("Current callibration params: %d %d %d %d\n",calMinX,calMinY,calMaxX,calMaxY);                
  }
  else {
    printf("Callibration read error\n");
  }  
  tft.callibrateTouch(calMinX,calMinY,calMaxX,calMaxY);   
}

static void writeCallibration(void) 
{
  tft.callibrateTouch(calMinX,calMinY,calMaxX,calMaxY);
  FILE * file = fopen(CALIBRATION_FILE, "wb");
  if (file) {
    fprintf(file,"%d %d %d %d\n",calMinX,calMinY,calMaxX,calMaxY);
    fclose(file);
  }
  else {
    printf("Callibration write error\n");
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
          tft.drawTextNoDma(ILI9341_TFTREALWIDTH-8,0, "+", RGBVAL16(0xff,0x00,0x00), RGBVAL16(0xff,0xff,0xff), true);
          calMinX += xt;
          calMinY += yt;          
          break;
        case 1:
          callibrationStep++;
          tft.drawTextNoDma(ILI9341_TFTREALWIDTH-8,0, " ", RGBVAL16(0xff,0xff,0xff), RGBVAL16(0xff,0xff,0xff), true);
          tft.drawTextNoDma(ILI9341_TFTREALWIDTH-8,ILI9341_TFTREALHEIGHT-16, "+", RGBVAL16(0xff,0x00,0x00), RGBVAL16(0xff,0xff,0xff), true);
          calMaxX += xt;
          calMinY += yt;           
          break;
        case 2:
          callibrationStep++;
          tft.drawTextNoDma(ILI9341_TFTREALWIDTH-8,ILI9341_TFTREALHEIGHT-16, " ", RGBVAL16(0xff,0xff,0xff), RGBVAL16(0xff,0xff,0xff), true);
          tft.drawTextNoDma(0,ILI9341_TFTREALHEIGHT-16, "+", RGBVAL16(0xff,0x00,0x00), RGBVAL16(0xff,0xff,0xff), true);
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
          if ( (xt > (ILI9341_TFTREALWIDTH/4)) && (xt < (ILI9341_TFTREALWIDTH*3)/4) 
            && (yt > (ILI9341_TFTREALHEIGHT/4)) && (yt < (ILI9341_TFTREALHEIGHT*3)/4) ) {
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
      vTaskDelay(100 / portTICK_PERIOD_MS); 
    }  
  }
  else {
    prev_zt = 0;
  } 
  return 1; 
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
  
  struct stat st;
  bool newPathIsDir = false;
  if(stat(newpath,&st) == 0)
    if((st.st_mode & S_IFDIR) != 0)
      newPathIsDir = true;

  int rx=0,ry=0,rw=0,rh=0;
  char c = captureTouchZone(menutouchareas, menutouchactions, &rx,&ry,&rw,&rh);
  if ( ( (bClick & MASK_JOY2_BTN) || (bClick & MASK_KEY_USER1) )  && (newPathIsDir) ) {
      menuRedraw=true;
      strcpy(romspath,newpath);
      curFile = 0;
      nbFiles = readNbFiles();     
  }
  else if ( (c >= MKEY_L1) && (c <= MKEY_L9) ) {
    if ( (topFile+(int)c-1) <= (nbFiles-1)  )
    {
      curFile = topFile + (int)c -1;
      menuRedraw=true;
     //tft.drawRectNoDma( rx,ry,rw,rh, KEYBOARD_HIT_COLOR );
    }
  }
  else if ( (bClick & MASK_JOY2_BTN) ) {
      menuRedraw=true;
      action = ACTION_RUNTFT;       
  }
  else if (bClick & MASK_JOY2_UP) {
    if (curFile!=0) {
      menuRedraw=true;
      curFile--;
    }
  }
  else if ( (bClick & MASK_JOY2_RIGHT) || (c == MKEY_UP) ) {
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
  else if ( (bClick & MASK_JOY2_LEFT) || (c == MKEY_DOWN) ) {
    if ((curFile<(nbFiles-9)) && (nbFiles)) {
      curFile += 9;
      menuRedraw=true;
    }
    else if ((curFile<(nbFiles-1)) && (nbFiles)) {
      curFile++;
      menuRedraw=true;
    }
  }
  else if ( (bClick & MASK_KEY_USER1) || (c == MKEY_JOY) ) {
    emu_SwapJoysticks(0);
    menuRedraw=true;  
  }   

    
  if (menuRedraw && nbFiles) {
         
    int fileIndex = 0;
    DIR* dir = opendir(romspath);
    tft.drawRectNoDma(MENU_FILE_XOFFSET,MENU_FILE_YOFFSET, MENU_FILE_W, MENU_FILE_H, MENU_FILE_BGCOLOR);
//    if (curFile <= (MAX_MENULINES/2-1)) topFile=0;
//    else topFile=curFile-(MAX_MENULINES/2);
    if (curFile <= (MAX_MENULINES-1)) topFile=0;
    else topFile=curFile-(MAX_MENULINES/2);
    
    int i=0;
    while (i<MAX_MENULINES) {
      struct dirent* de = readdir(dir);
      if (!de) {
        break;
      }     
      if ( (de->d_type == DT_REG) || ((de->d_type == DT_DIR) && (strcmp(de->d_name,".")) && (strcmp(de->d_name,"..")) ) ) {
        if (fileIndex >= topFile) {              
          if ((i+topFile) < nbFiles ) {
            if ((i+topFile)==curFile) {
              tft.drawTextNoDma(MENU_FILE_XOFFSET,i*TEXT_HEIGHT+MENU_FILE_YOFFSET, de->d_name, RGBVAL16(0xff,0xff,0x00), RGBVAL16(0xff,0x00,0x00), true);
              strcpy(selection,de->d_name);            
            }
            else {
              tft.drawTextNoDma(MENU_FILE_XOFFSET,i*TEXT_HEIGHT+MENU_FILE_YOFFSET, de->d_name, MENU_FILE_FGCOLOR, MENU_FILE_BGCOLOR, true);      
            }
          }
          i++; 
        }
        fileIndex++;    
      }
    }
    closedir(dir);
    
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
  
#ifdef HAS_I2CKBD
#ifdef USE_WIRE
#else
static esp_err_t i2c_master_read_slave_reg(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t* data_rd, size_t size)
{
    if (size == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( i2c_addr << 1 ) | I2C_MASTER_READ, ACK_CHECK_EN);
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, (i2c_ack_type_t)ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, (i2c_ack_type_t)NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}
#endif
#endif


void emu_init(void)
{

  esp_err_t ret = 0;

  printf("mounting sd...\n");

  sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  host.max_freq_khz = 10000;
  //host.slot = HSPI_HOST;
  
  sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
  slot_config.gpio_miso = SPIN_NUM_MISO;
  slot_config.gpio_mosi = SPIN_NUM_MOSI;
  slot_config.gpio_sck  = SPIN_NUM_CLK;
  slot_config.gpio_cs = SPIN_NUM_CS;
  slot_config.dma_channel = 2;

 
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024
  };
  

  while((ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card)) != ESP_OK) {
    printf("retrying\n");
    vTaskDelay(500 / portTICK_PERIOD_MS);   
  }

  strcpy(romspath,"/sdcard/");
  strcat(romspath,ROMSDIR);
  printf("dir is : %s\n",romspath);

  nbFiles = readNbFiles(); 
  printf("SD initialized, files found: %d\n",nbFiles);

 
  tft.touchBegin();
  //uint16_t xt=0;
  //uint16_t yt=0;
  //uint16_t zt=0;  
  //tft.readRo(&xt,&yt,&zt);


  emu_InitJoysticks();
  readCallibration();
  
  if ((tft.isTouching()) || (emu_ReadKeys() & MASK_JOY2_BTN) ) {
    callibrationInit();
  } else  {
    toggleMenu(true);
  }

#ifdef HAS_I2CKBD
  uint8_t msg[7]={0,0,0,0,0,0,0};
  
#ifdef USE_WIRE
  Wire.begin(I2C_SDA_IO, I2C_SCL_IO);
  Wire.requestFrom(8, 7, I2C_FREQ_HZ);  // request 5 bytes from slave device #8 
  int i = 0;
  int hitindex=-1;
  while (Wire.available() && (i<7) ) { // slave may send less than requested
    uint8_t b = Wire.read(); // receive a byte
    if (b != 0xff) hitindex=i; 
    msg[i++] = b;        
  }  
#else
  int i2c_master_port = I2C_NUM_1;
  i2c_config_t conf;
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = I2C_SDA_IO;
  conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
  conf.scl_io_num = I2C_SCL_IO;
  conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
  conf.master.clk_speed = I2C_FREQ_HZ;
  i2c_param_config((i2c_port_t)i2c_master_port, &conf);
  if (i2c_driver_install((i2c_port_t)i2c_master_port, conf.mode,0, 0, 0) != ESP_OK)
    printf("I2C Failed initialized\n");
  
  if (i2c_master_read_slave_reg( I2C_NUM_1, 8, &msg[0], 7 ) != ESP_OK) 
      printf("I2C Failed \n");    
#endif  




  if ( (msg[0] == 0xff) && (msg[1] == 0xff) && 
       (msg[2] == 0xff) && (msg[3] == 0xff) && 
       (msg[4] == 0xff) && (msg[5] == 0xff) && (msg[6] == 0xff)) {
    i2cKeyboardPresent = true;
    printf("i2C keyboard found\n");            
  }
#endif 
}


void emu_printf(char * text)
{
  printf("%s\n",text);
}


void emu_printi(int val)
{
  printf("%d\n",val);
}

void * emu_Malloc(int size)
{
  void * retval =  malloc(size);
  if (!retval) {
    printf("failled to allocate %d\n",size);
  }
  else {
    printf("could allocate %d\n",size); 
  }
  
  return retval;
}

void emu_Free(void * pt)
{
  free(pt);
}


static FILE * lastfileOpened;


int emu_FileOpen(char * filename)
{
  int retval = 0;

  char filepath[80];
  strcpy(filepath, romspath);
  strcat(filepath, "/");
  strcat(filepath, filename);
  //printf("FileOpen...%s\n",filepath);
    
  lastfileOpened = fopen(filepath, "rb");
  if (lastfileOpened) {
    retval = 1;  
  }
  else {
    //printf("FileOpen failed\n");
  }
  return (retval);
}

int emu_FileRead(char * buf, int size)
{
  int retval = fread(buf, 1, size, lastfileOpened);
  if (retval != size) {
    printf("FileRead failed\n");
  }
  return (retval);     
}

unsigned char emu_FileGetc(void) {
  unsigned char c;
  int retval = fread(&c, 1, 1, lastfileOpened);
  if (retval != 1) {
    printf("emu_FileGetc failed\n");
  }  
  return c; 
}


void emu_FileClose(void)
{
  fclose(lastfileOpened);  
}

int emu_FileSize(char * filename) 
{
  int filesize=0;
  char filepath[80];
  strcpy(filepath, romspath);
  strcat(filepath, "/");
  strcat(filepath, filename);
  printf("FileSize...%s\n",filepath);

  FILE * file = fopen(filepath, "rb");
  if (file) {
    fseek(file, 0L, SEEK_END);
    filesize = ftell(file);
    //fseek(file, 0L, SEEK_SET);
    printf("filesize is...%d\n",filesize);    
    fclose(file);    
  }
 
  return(filesize);  
}

int emu_FileSeek(int seek) 
{
  fseek(lastfileOpened, seek, SEEK_SET);     
  return (seek);
}

int emu_LoadFile(char * filename, char * buf, int size)
{
  int filesize = 0;
    
  char filepath[80];
  strcpy(filepath, romspath);
  strcat(filepath, "/");
  strcat(filepath, filename);
  printf("LoadFile...%s\n",filepath);  

  filesize = emu_FileSize(filename);
  FILE * file = fopen(filepath, "rb");
  if (file) {
    if (size >= filesize)
    {
      if (fread(buf, 1, filesize, file) != filesize) {
        printf("File read failed\n");
      }        
    }
    fclose(file);
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
  printf("LoadFileSeek...%d bytes at %d from %s\n",size,seek,filepath); 

  FILE * file = fopen(filepath, "rb");
  if (file) {
    fseek(file, seek, SEEK_SET);       
    if (fread(buf, size, 1, file) != size) {
      printf("File read failed\n");
    }        
    fclose(file);
  }
  
  return(filesize);
}

static int keypadval=0; 
static bool joySwapped = false;
static uint16_t bLastState;
static int xRef;
static int yRef;

int emu_ReadAnalogJoyX(int min, int max) 
{
  int val; //adc1_get_raw((adc1_channel_t)PIN_JOY2_A1X);  
  adc2_get_raw((adc2_channel_t)PIN_JOY2_A1X, ADC_WIDTH_BIT_12,&val);
  //printf("refX:%d X:%d\n",xRef,val); 
  val = val-xRef;
  //val = ((val*140)/100);
  if ( (val > -xRef/4) && (val < xRef/4) ) val = 0;
#if INVX
  val = xRef-val;
#else
  val = val+xRef;
#endif  

  return (val*(max-min))/(xRef*2);
}

int emu_ReadAnalogJoyY(int min, int max) 
{
  int val; //= adc1_get_raw((adc1_channel_t)PIN_JOY2_A2Y);
  adc2_get_raw((adc2_channel_t)PIN_JOY2_A2Y, ADC_WIDTH_BIT_12,&val);
  //printf("refY:%d Y:%d\n",yRef,val); 
  val = val-yRef;
  //val = ((val*120)/100);
  if ( (val > -yRef/4) && (val < yRef/4) ) val = 0;
#if INVY
  val = yRef-val;
#else
  val = val+yRef;
#endif  
  return (val*(max-min))/(yRef*2);
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
  
  joysval |= ((gpio_get_level((gpio_num_t)PIN_JOY2_BTN) == 1) ? 0 : MASK_JOY2_BTN);

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

  if (joySwapped) {
    retval = ((j1 << 8) | j2);
  }
  else {
    retval = ((j2 << 8) | j1);
  }

  if (gpio_get_level((gpio_num_t)PIN_KEY_USER1) == 0 ) retval |= MASK_KEY_USER1;
  if (gpio_get_level((gpio_num_t)PIN_KEY_USER2) == 0 ) retval |= MASK_KEY_USER2;

  //printf("%d\n",retval);   
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
    uint8_t msg[7]; 
#ifdef USE_WIRE
    Wire.requestFrom(8, 7, I2C_FREQ_HZ);    // request 5 bytes from slave device #8 
    int i = 0;
    int hitindex=-1;
    while (Wire.available() && (i<7) ) { // slave may send less than requested
      uint8_t b = Wire.read(); // receive a byte
      if (b != 0xff) hitindex=i; 
      msg[i++] = b;        
    } 
#else
    if (i2c_master_read_slave_reg( I2C_NUM_1, 8, &msg[0], 7 ) != ESP_OK) 
      printf("I2C Failed \n");
    int hitindex=-1;
    int i = 0;
    while (i<7) {
      if (msg[i] != 0xff) hitindex=i;
      i++;
    }
#endif     
    //printf("I2C 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
    //  msg[0],msg[1],msg[2],msg[3],msg[4],msg[5],msg[6]);  
    if ((hitindex >=0 ) && (hitindex <=6 )) {
      unsigned short match = ((~msg[hitindex])&0x00FF) | (hitindex<<8);
      for (i=0; i<sizeof(i2ckeys); i++) {
        if (match == i2ckeys[i]) {
          //printf("I2C %d\n",keys[i]);          
          return (keys[i]);
        }
      }
    }    
  }
#endif
  return(retval);
}

void emu_InitJoysticks(void) {  
  gpio_set_direction((gpio_num_t)PIN_JOY2_BTN, GPIO_MODE_INPUT);
  gpio_set_pull_mode((gpio_num_t)PIN_JOY2_BTN, GPIO_PULLUP_ONLY);
  gpio_set_direction((gpio_num_t)PIN_KEY_USER1, GPIO_MODE_INPUT);
  gpio_set_pull_mode((gpio_num_t)PIN_KEY_USER1, GPIO_PULLUP_ONLY);
  gpio_set_direction((gpio_num_t)PIN_KEY_USER2, GPIO_MODE_INPUT);
  gpio_set_pull_mode((gpio_num_t)PIN_KEY_USER2, GPIO_PULLUP_ONLY);

  //adc1_config_channel_atten((adc1_channel_t)PIN_JOY2_A1X,ADC_ATTEN_DB_11);
  //adc1_config_channel_atten((adc1_channel_t)PIN_JOY2_A2Y,ADC_ATTEN_DB_11);
  adc2_config_channel_atten((adc2_channel_t)PIN_JOY2_A1X,ADC_ATTEN_DB_11);
  adc2_config_channel_atten((adc2_channel_t)PIN_JOY2_A2Y,ADC_ATTEN_DB_11);
  xRef=0; yRef=0;
  for (int i=0; i<10; i++) {
    int val;
    adc2_get_raw((adc2_channel_t)PIN_JOY2_A1X, ADC_WIDTH_BIT_12, &val);
    //val = adc1_get_raw((adc1_channel_t)PIN_JOY2_A1X);
    xRef += val;
    adc2_get_raw((adc2_channel_t)PIN_JOY2_A2Y,ADC_WIDTH_BIT_12, &val);
    //val = adc1_get_raw((adc1_channel_t)PIN_JOY2_A2Y); 
    yRef += val;
    vTaskDelay(20 / portTICK_PERIOD_MS); 
  }
  xRef /= 10;
  yRef /= 10;

  printf("refs: %d %d\n",xRef,yRef); 
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
#ifdef DMA_FULLgpio_get_level
            tft.begin();
            tft.refresh();
#endif                        
            //prev_zt = 0; 
            vkbActive = false;
            exitVkbd = false;
        }
        else {
#ifdef DMA_FULL          
            tft.stop();
            tft.begin();      
            tft.start();
#endif                       
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
              vTaskDelay(50 / portTICK_PERIOD_MS); 
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
    //logo = ;
    //keysw = ;      
  }
  else {
    //logo = ;
    //keysw = ;  
  }
  return 0;
}



static unsigned short palette16[PALETTE_SIZE];
static int fskip=0;

void emu_SetPaletteEntry(unsigned char r, unsigned char g, unsigned char b, int index)
{
  if (index<PALETTE_SIZE) {
    //printf("%d: %d %d %d\n", index, r,g,b);
    palette16[index] = RGBVAL16(r,g,b);    
  }
}

void emu_DrawVsync(void)
{
  //printf("sync %d\n",skip);  
  fskip += 1;
  fskip &= VID_FRAME_SKIP;
}

void emu_DrawLine(unsigned char * VBuf, int width, int height, int line) 
{
  if (fskip==0) {
    tft.writeLine(width,height,line, VBuf, palette16);
  }
}  

void emu_DrawScreen(unsigned char * VBuf, int width, int height, int stride) 
{  
  if (fskip==0) {
    tft.writeScreen(width,height-TFT_VBUFFER_YCROP,stride, VBuf+(TFT_VBUFFER_YCROP/2)*stride, palette16);
  }
}

int emu_FrameSkip(void)
{
  return fskip;
}

void * emu_LineBuffer(int line)
{
  return (void*)tft.getLineBuffer(line);
}

#ifdef HAS_SND
#include "AudioPlaySystem.h"
extern AudioPlaySystem audio;

void emu_sndInit() {
}

void emu_sndPlaySound(int chan, int volume, int freq)
{
  if (chan < 6) {
    audio.sound(chan, freq, volume); 
  } 
}

void emu_sndPlayBuzz(int size, int val) {
  //mymixer.buzz(size,val);  
}
#endif

