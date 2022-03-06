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

#ifdef HAS_USBKEY
#include "USBHost_t36.h"  // Read this header first for key info
USBHost myusb;
USBHub hub1(myusb);
KeyboardController keyboard1(myusb);
USBHIDParser hid1(myusb);
MouseController mouse1(myusb);
MIDIDevice midi1(myusb);
#endif

static bool emu_writeConfig(void);
static bool emu_readConfig(void);
static bool emu_eraseConfig(void);

static bool mouseDetected = false;
static bool keyboardDetected = false;
static uint8_t usbnavpad=0;

#include <SD.h>
static File file;

#define MAX_FILES           64
#define AUTORUN_FILENAME    "autorun.txt"

#define MAX_FILENAME_SIZE   34
#define MAX_MENULINES       9
#define TEXT_HEIGHT         16
#define TEXT_WIDTH          8
#define MENU_FILE_XOFFSET   (2*TEXT_WIDTH)
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

static int nbFiles=0;
static int curFile=0;
static int topFile=0;
static char selection[MAX_FILENAME_PATH]="";
static char second_selection[MAX_FILENAME_PATH]="";
static char files[MAX_FILES][MAX_FILENAME_SIZE];
static char selected_filename[MAX_FILENAME_SIZE]=""; 
static char second_selected_filename[MAX_FILENAME_SIZE]="";
static bool menuRedraw=true;
static bool autorun=false;

static const unsigned short * keys;
#ifdef TEECOMPUTER
static unsigned char keymatrix[6];
static int keymatrix_hitrow=-1;
static uint32_t keypress_t_ms=0;
static uint32_t last_t_ms=0;
static uint32_t hundred_ms_cnt=0;
static bool ledflash_toggle=false;
#endif
static bool key_extmode=false;
static bool key_sh=false;
static bool key_fn=false;

static boolean joySwapped = false;
static uint16_t bLastState;
#ifdef PIN_JOY2_A1X
static int xRef;
static int yRef;
#endif
static bool menuOn=true;


/********************************
 * Generic output and malloc
********************************/ 
void emu_printf(const char * text)
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

void * emu_Malloc(unsigned int size)
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

void * emu_MallocI(unsigned int size)
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

/********************************
 * Input and keyboard
********************************/ 
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
    row |= (digitalRead(KROWIN2) ? 0 : 0x02);
    row |= (digitalRead(KROWIN3) ? 0 : 0x04);
    row |= (digitalRead(KROWIN4) ? 0 : 0x08);
    row |= (digitalRead(KROWIN5) ? 0 : 0x10);
    row |= (digitalRead(KROWIN6) ? 0 : 0x20);
    row |= (digitalRead(KROWIN7) ? 0 : 0x40);
    digitalWrite(cols[i], 1);
    pinMode(cols[i],INPUT_DISABLE);
    keymatrix[i]=row;
  }

  bool fn_pressed=false;
  if ( keymatrix[5] & 0x08 ) {fn_pressed=true; keymatrix[5] &= ~0x08;};
  
  bool sh_pressed=false;
  if ( keymatrix[5] & 0x10 ) {sh_pressed=true; keymatrix[5] &= ~0x10;};

  for (int i=0;i<6;i++){
    row = keymatrix[i];
    if (row) keymatrix_hitrow=i;
  }

  //6,9,15,8,7,22
#if INVX
  if ( row & 0x40  ) retval |= MASK_JOY2_LEFT;
  if ( row & 0x20  ) retval |= MASK_JOY2_RIGHT;
#else
  if ( row & 0x20  ) retval |= MASK_JOY2_LEFT;
  if ( row & 0x40  ) retval |= MASK_JOY2_RIGHT;
#endif
#if INVY
  if ( row & 0x4  ) retval |= MASK_JOY2_DOWN;
  if ( row & 0x1  ) retval |= MASK_JOY2_UP;  
#else
  if ( row & 0x1  ) retval |= MASK_JOY2_DOWN;
  if ( row & 0x4  ) retval |= MASK_JOY2_UP;  
#endif
  if ( row & 0x02 ) retval |= MASK_JOY2_BTN;

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

  if ( sh_pressed ) {
      key_sh = true;      
  }
  else {
    key_sh = false;
    if ( fn_pressed ) {
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
              if (key_extmode == false) 
              {
                key_extmode = true;
              }
              else 
              {
                key_extmode = false;
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
            if (key_extmode == false) 
            {
              key_extmode = true;
            }
            else 
            {
              key_extmode = false;
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
  }
  
  // Handle LED
  if (key_extmode == true) {
    digitalWrite(KLED, (ledflash_toggle?1:0));
  }
  else {
    if ( (key_fn == true) || (key_sh == true) ) {
      digitalWrite(KLED, 1);
    }
    else {
      digitalWrite(KLED, 0);
    }     
  } 
 
  if ( key_fn ) retval |= MASK_KEY_USER2;
  if ( ( key_fn ) && (keymatrix[4] == 0x10 )) retval |= MASK_KEY_USER1;

  if ( (key_fn) && (key_sh) )
#else
  if ( ((retval & (MASK_KEY_USER1+MASK_KEY_USER2)) == (MASK_KEY_USER1+MASK_KEY_USER2))
     || (retval & MASK_KEY_USER4 ) )
#endif
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
#ifdef TEECOMPUTER
  if (key_extmode) {
    if (key_fn) {
      keys = (const unsigned short *)key_map5; // fn-extra
    }
    else if (key_sh) {
      keys = (const unsigned short *)key_map4; // shift-functionkeys
    }
    else {
      keys = (const unsigned short *)key_map3; // def-digitkeys
    }
  }
  else {
    if (key_fn) {
      keys = (const unsigned short *)key_map2; // fn-shiftothers 
    }
    else if (key_sh) {
      keys = (const unsigned short *)key_map1; // shift-uppercase
    }
    else {
      keys = (const unsigned short *)key_map0; // def-lowercase
    }
  }

  
  if (keymatrix_hitrow >=0 ) {
    unsigned short match = ((unsigned short)keymatrix_hitrow<<8) | keymatrix[keymatrix_hitrow];  
    for (unsigned int i=0; i<sizeof(matkeys)/sizeof(unsigned short); i++) {
      if (match == matkeys[i]) {
        hundred_ms_cnt = 0;    
        return (keys[i]);
      }
    }
  }
#endif
  return(retval);
}

unsigned char emu_ReadI2CKeyboard2(int row) {
  int retval=0;
#ifdef TEECOMPUTER
  retval = keymatrix[row];
#endif
  return retval;
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
  pinMode(KROWIN7, INPUT_PULLUP); 
#endif
}


int emu_setKeymap(int index) {
  return 0;
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

#ifdef HAS_USBKEY
static unsigned char midiBuffer[16];
static unsigned char midiLastCmd=0;
static int midiDataCnt=0;
static int midiCmdNbParam=0;
#endif

void emu_MidiOnDataReceived(unsigned char value) {

#ifdef HAS_USBKEY
//Serial.println(value, HEX);    
//10000000 = 128 = note off
//10010000 = 144 = note on
//10100000 = 160 = aftertouch
//10110000 = 176 = continuous controller
//11000000 = 192 = patch change
//11010000 = 208 = channel pressure
//11100000 = 224 = pitch bend
//11110000 = 240 = non-musical commands
  if (value >= 128) {
    midiDataCnt = 0;
    midiLastCmd = value;
    switch (value & 0xF0) {
      case 128: // 0x80
        midiCmdNbParam = 2;
        //Serial.print("note off: ");      
        //Serial.println(value&0xf);            
        break;
      case 144: //0x90
        midiCmdNbParam = 2;
        //Serial.print("note on: ");      
        //Serial.println(value&0xf);            
        break;
      case 160: //0xA0
        midiCmdNbParam = 2;
        //Serial.print("aftertouch: "); // rare     
        //Serial.println(value&0xf);            
        break;
      case 176: //0xB0
        midiCmdNbParam = 2;
        //Serial.print("continuous controller: ");      
        //Serial.println(value&0xf);            
        break;
      case 192: //0xC0
        midiCmdNbParam = 1;
        //Serial.print("patch change: "); //some      
        //Serial.println(value&0xf);            
        break;
      case 208: //0xD0
        midiCmdNbParam = 1;
        Serial.print("channel pressure: ");      
        Serial.println(value&0xf);            
        break;
      case 224: //0xE0
        midiCmdNbParam = 2;
        //Serial.print("pitch bend: ");      
        //Serial.println(value&0xf);            
        break;
      case 240: //0xF0
        // non-musical commands     
        switch (value) {
          case 0xF0:
            //Serial.println("NI: System Exclusive"); 
            break;
          case 0xF1:
            //Serial.println("NI: System Common - MIDI Time Code Quarter Frame"); 
            break;
          case 0xF2:
             midiCmdNbParam = 2;
             break;
          case 0xF3:
            //Serial.println("NI: System Common - Song Select"); 
            break;
          case 0xF6:
            //Serial.println("NI: System Common - Tune Request"); 
            break;
          case 0xF8:
            //Serial.println("System Real Time - Timing Clock");
            midi1.sendRealTime(value, 0); 
            break;
          case 0xFA:
            //Serial.println("System Real Time - Start"); 
            midi1.sendRealTime(value, 0); 
            break;
          case 0xFB:
            //Serial.println("System Real Time - Continue"); 
            midi1.sendRealTime(value, 0); 
            break;
          case 0xFC:
            //Serial.println("System Real Time - Stop");
            midi1.sendRealTime(value, 0); 
            break;
          case 0xFE:
            //Serial.println("System Real Time - Active Sensing"); 
            midi1.sendRealTime(value, 0); 
            break;
          case 0xFF:
            //Serial.println("System Real Time - System Reset"); 
            midi1.sendRealTime(value, 0); 
            break;
        }
        //SystemExclusive       = 0xF0, // System Exclusive
        //TimeCodeQuarterFrame  = 0xF1, // System Common - MIDI Time Code Quarter Frame
        //SongPosition          = 0xF2, // System Common - Song Position Pointer
        //SongSelect            = 0xF3, // System Common - Song Select
        //TuneRequest           = 0xF6, // System Common - Tune Request
        //Clock                 = 0xF8, // System Real Time - Timing Clock
        //Start                 = 0xFA, // System Real Time - Start
        //Continue              = 0xFB, // System Real Time - Continue
        //Stop                  = 0xFC, // System Real Time - Stop
        //ActiveSensing         = 0xFE, // System Real Time - Active Sensing
        //SystemReset           = 0xFF, // System Real Time - System Reset                   
        break;
      default:
        Serial.print("??: ");      
        Serial.println(value&0xf);            
        break;      
    }          
  }
  else {
    if (midiDataCnt<16) midiBuffer[midiDataCnt++] = value ;
    if ( (midiLastCmd & 0xF0) == 240) {
      if (midiLastCmd == 0xF2) {
        if (midiDataCnt == midiCmdNbParam) {
          //Serial.println("System Common - Song Position Pointer"); 
          midi1.sendSongPosition(((int)midiBuffer[1]<<7)+(int)midiBuffer[0], 0);          
        }
      }
      else {
        Serial.println(value);
      }
    }
    else if (midiDataCnt == midiCmdNbParam) {
      unsigned char chan = (midiLastCmd&0xf)+1;
      //Serial.print("ch ");    
      //Serial.println(chan);    
      switch (midiLastCmd & 0xF0) {
        case 128: //0x80
          //Serial.print("note off: ");      
          midi1.sendNoteOff(midiBuffer[0], midiBuffer[1], chan);
          break;
        case 144: //0x90
          //Serial.print("note on: ");      
          midi1.sendNoteOn(midiBuffer[0], midiBuffer[1], chan);
          break;
        case 160: //0xA0
          //Serial.print("aftertouch: ");      
          midi1.sendPolyPressure(midiBuffer[0], midiBuffer[1], chan);
          break;
        case 176: //0xB0
          //Serial.print("continuous controller: ");      
          midi1.sendControlChange(midiBuffer[0], midiBuffer[1], chan);           
          break;
        case 192: //0xC0
          //Serial.print("patch change: ");      
          midi1.sendProgramChange(midiBuffer[0], chan);           
          break;
        case 208: //0xD0
          //Serial.print("channel pressure: ");      
          midi1.sendAfterTouch(midiBuffer[0], chan);                 
          break;
        case 224: //0xE0
          //Serial.print("pitch bend: ");          
          midi1.sendPitchBend((((int)midiBuffer[1]<<7)+(int)midiBuffer[0])-8192, chan);                  
          break;
        default:
          Serial.print("??: ");               
          break;      
      }         
    }
  }
#endif  
}

/********************************
 * Menu file loader UI
********************************/ 
static int readNbFiles(void) {
  int totalFiles = 0;

  File entry;    
  file = SD.open(selection);
  while ( (true) && (totalFiles<MAX_FILES) ) {
    entry = file.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
  const char * filename = entry.name();
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



void backgroundMenu(void) {
    menuRedraw=true;  
    tft.fillScreenNoDma(RGBVAL16(0x00,0x00,0x00));
    tft.drawTextNoDma(0,0, TITLE, RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), true);         
}

int handleMenu(uint16_t bClick)
{
  if (autorun) {
      return (ACTION_RUN1);                    
  }  
  
  int action = ACTION_NONE;
  if ( (bClick & MASK_JOY2_BTN) || (bClick & MASK_JOY1_BTN) ) {     
      char newpath[MAX_FILENAME_PATH];
      strcpy(newpath, selection);
      strcat(newpath, "/");
      strcat(newpath, selected_filename);
      strcpy(selection,newpath);
      emu_printf("new filepath is");
      emu_printf(selection);
      File file = SD.open(selection);
      if (file.isDirectory())  {
        curFile = 0;
        nbFiles = readNbFiles();             
      }
      else {
        action = ACTION_RUN1;
#ifdef TEECOMPUTER
        if (key_extmode) {
          emu_writeConfig();
        }
#endif                       
      }
      menuRedraw=true;
  }
  else if ( bClick & MASK_KEY_USER1 ) {
      menuRedraw=true;
      strcpy(second_selected_filename,selected_filename); 
      strcpy(second_selection, selection);
      strcat(second_selection, "/");
      strcat(second_selection, second_selected_filename);      
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
            strcpy(selected_filename,filename);            
          }
          else {
            tft.drawTextNoDma(MENU_FILE_XOFFSET,i*TEXT_HEIGHT+MENU_FILE_YOFFSET, filename, RGBVAL16(0xff,0xff,0xff), MENU_FILE_BGCOLOR, true);      
          }
        }
        i++; 
      }
      fileIndex++;    
    }

    
//    tft.drawTextNoDma(48,MENU_JOYS_YOFFSET+8, (emu_SwapJoysticks(1)?(char*)"SWAP=1":(char*)"SWAP=0"), RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), false);
    tft.drawTextNoDma(48,MENU_JOYS_YOFFSET+8, "FLOPPY2:", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), false);
    tft.drawRectNoDma(120,MENU_JOYS_YOFFSET+8, MENU_FILE_W, 8, RGBVAL16(0x00,0x00,0x00));
    tft.drawTextNoDma(120,MENU_JOYS_YOFFSET+8, second_selected_filename, RGBVAL16(0xff,0xff,0xff), RGBVAL16(0x00,0x00,0x00), false);
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

char * menuSecondSelection(void)
{
  return (second_selection);  
}


/********************************
 * OSKB handling
********************************/   
static bool oskbOn = false;
static int cxpos = 0;
static int cypos = 0;
static uint16_t oskbBLastState = 0;
#define OSKBHEIGHT 4
#define OSKBXOFF   16
#define OSKBYOFF   0

static void lineOSKB(int xoff, bool bottom, char * str, int row)
{
  char c[4] = {' ',0,' ',0};
  const char * cpt = str;
  int i=0;
  int fb_width,fb_height;
  tft.get_frame_buffer_size(&fb_width, &fb_height);
  int ypos = (bottom?(fb_height-2*8):0);
  int line = row + (bottom?2:0);
  while ((c[1] = *cpt++))
  {
    uint16_t bg;
    if (row&1) bg = (i&1)?RGBVAL16(0xff,0xff,0xff):RGBVAL16(0xe0,0xe0,0xe0);
    else bg = (i&1)?RGBVAL16(0xe0,0xe0,0xe0):RGBVAL16(0xff,0xff,0xff);
    if ( (cxpos == i) && (cypos == line) ) bg = RGBVAL16(0x00,0xff,0xff);
    tft.drawTextNoDma(OSKBXOFF+xoff+24*i,OSKBYOFF+ypos+8*row , &c[0], RGBVAL16(0x00,0x00,0x00), bg, false);
    i++;
  } 
}

static int linelenOSKB() {
  if (cypos == 0) return strlen(keylables_map0_0);
  else if (cypos == 1) return strlen(keylables_map0_1);
  else if (cypos == 2) return strlen(keylables_map0_2);
  else return strlen(keylables_map0_3);
}

static void drawOSKB() {
  if (key_extmode) {
    if (key_fn) {
      lineOSKB(16,false, keylables_map5_0,  0);
      lineOSKB(8, false, keylables_map5_1,  1);
      lineOSKB(0, true, keylables_map5_2,  0);
      lineOSKB(96,true, keylables_map5_3,  1);
    }
    else if (key_sh) {
      lineOSKB(16,false, keylables_map4_0,  0);
      lineOSKB(8, false, keylables_map4_1,  1);
      lineOSKB(0, true, keylables_map4_2,  0);
      lineOSKB(96,true, keylables_map4_3,  1);
    }
    else {
      lineOSKB(16,false, keylables_map3_0,  0);
      lineOSKB(8, false, keylables_map3_1,  1);
      lineOSKB(0, true, keylables_map3_2,  0);
      lineOSKB(96,true, keylables_map3_3,  1);
    }
  }
  else {
    if (key_fn) {
      lineOSKB(16,false, keylables_map2_0,  0);
      lineOSKB(8, false, keylables_map2_1,  1);
      lineOSKB(0, true, keylables_map2_2,  0);
      lineOSKB(96,true, keylables_map2_3,  1);
    }
    else if (key_sh) {
      lineOSKB(16,false, keylables_map1_0,  0);
      lineOSKB(8, false, keylables_map1_1,  1);
      lineOSKB(0, true, keylables_map1_2,  0);
      lineOSKB(96,true, keylables_map1_3,  1);
    }
    else {
      lineOSKB(16,false, keylables_map0_0,  0);
      lineOSKB(8, false, keylables_map0_1,  1);
      lineOSKB(0, true, keylables_map0_2,  0);
      lineOSKB(96,true, keylables_map0_3,  1);
    }
  }  
}

int handleOSKB(void) { 
  int retval = 0;
  if (oskbOn) {
    uint16_t bClick = bLastState & ~oskbBLastState;
    oskbBLastState = bLastState;
    bool updated = true;
    if (bClick & MASK_KEY_USER1)
    { 
    }    
    else if (bClick & MASK_JOY2_RIGHT)
    {  
      cxpos++;
      if (cxpos >= linelenOSKB()) cxpos = 0;
    }
    else if (bClick & MASK_JOY2_LEFT)
    {  
      cxpos--;
      if (cxpos < 0) cxpos = linelenOSKB()-1;
    }
    else if (bClick & MASK_JOY2_DOWN)
    {  
      cypos++;
      if (cypos >= OSKBHEIGHT) cypos = 0;
      if (cxpos >= linelenOSKB()) cxpos = linelenOSKB()-1;
    }
    else if (bClick & MASK_JOY2_UP)
    {  
      cypos--;
      if (cypos < 0) cypos = OSKBHEIGHT-1;
      if (cxpos >= linelenOSKB()) cxpos = linelenOSKB()-1;
    }
    else if (oskbBLastState & MASK_JOY2_BTN)
    {  
      //if (retval) { toggleOSKB(false); updated=false; };
    }
    else {
      updated=false;
    }    
    /*if (updated)*/ drawOSKB();
  }
  return retval; 
}

void toggleOSKB(bool forceon) {
  if (forceon) {
    oskbOn = true;
    drawOSKB();
  }
  else {
    if (oskbOn) {
      oskbOn = false;
    }
    else {
      oskbOn = true;
      drawOSKB();
    }
  }
}



/********************************
 * File IO
********************************/ 
static File file_handlers[NB_FILE_HANDLER];

static void FileHandlersInit(void) {
  for (int i=0; i<NB_FILE_HANDLER;i++) {
    file_handlers[i]=file;
  }
}

static int getFreeFileHandler(void) {
  for (int i=0; i<NB_FILE_HANDLER; i++) {
    if (!file_handlers[i] != file) return i;
  }
  emu_printf("No free file handler");
  return -1;
}

static File getFileHandler(int handler) {
  return (file_handlers[handler-1]);
}

//#define HCFH 1

int emu_FileOpen(const char * filepath, const char * mode)
{
//  emu_printf("FileOpen...");
//  emu_printf(filepath);
#ifdef HCFH  
  int retval = 0;
  if ((file = SD.open(filepath, O_READ))) {
    retval = 1;  
  }
  else {
    emu_printf("FileOpen failed");
  }
  return (retval);
#else
  int retval = 0;
  int handler = getFreeFileHandler();
  if (handler >= 0) {
    if ((file_handlers[handler] = SD.open(filepath, O_READ))) {
 //     emu_printi(handler+1);
      retval = handler+1;  
    }
    else {
      file_handlers[handler] = file;
      emu_printf("FileOpen failed");
    }
  }
  return (retval);
#endif
}

int emu_FileRead(void * buf, int size, int handler)
{
//  emu_printf("FileRead");
//  emu_printi(handler);
#ifdef HCFH  
  return (file.read(buf, size));
#else
  return (getFileHandler(handler).read(buf, size));
#endif
}

int emu_FileGetc(int handler) {
//  emu_printf("FileGetc");
//  emu_printi(handler);
#ifdef HCFH  
  unsigned char c;
  int retval = file.read(&c, 1);
  if (retval != 1) {
    emu_printf("emu_FileGetc failed");
  }  
  return (int)c; 
#else
  unsigned char c;
  int retval = getFileHandler(handler).read(&c, 1);
  if (retval != 1) {
    emu_printf("emu_FileGetc failed");
  }  
  return (int)c; 
#endif
}

int emu_FileSeek(int handler, int seek, int origin)
{
//  emu_printf("FileSeek");
//  emu_printi(handler);
//  emu_printi(seek);
#ifdef HCFH  
  file.seek(seek);
  return (seek);
#else
  getFileHandler(handler).seek(seek);
  return (seek);
#endif
}

int emu_FileTell(int handler) {
//  emu_printf("FileTell");
//  emu_printi(handler);
#ifdef HCFH  
  return (50);
#else
  File file = getFileHandler(handler);
  return (emu_FileSize((char*)file.name()));
#endif
}


void emu_FileClose(int handler)
{
//  emu_printf("FileClose");
//  emu_printi(handler);
#ifdef HCFH  
  file.close();  
#else
  getFileHandler(handler).close();  
  file_handlers[handler-1] = file;
#endif
}


static File lofile;

unsigned int emu_FileSize(const char * filepath) 
{
  unsigned int filesize=0;
  emu_printf(filepath);
  if ((lofile = SD.open(filepath, O_READ))) 
  {
    emu_printf("filesize is...");
    filesize = lofile.size(); 
    emu_printf(filesize);
    lofile.close();    
  }
  return(filesize);
}

unsigned int emu_LoadFile(const char * filepath, void * buf, int size)
{ 
  unsigned int filesize = 0;
  emu_printf("LoadFile...");
  emu_printf(filepath);
  if ((lofile = SD.open(filepath, O_READ))) 
  {
    filesize = lofile.size(); 
    emu_printf(filesize);
    if ((unsigned int)size >= filesize)
    {
      if (lofile.read(buf, filesize) != filesize) 
      {
        emu_printf("File read failed");
      }        
    }
    lofile.close();
  }
  return(filesize);
}

unsigned int emu_LoadFileSeek(const char * filepath, void * buf, int size, int seek)
{  
  unsigned int filesize = 0;
  emu_printf("LoadFileSeek...");
  emu_printf(filepath);
  if ((lofile = SD.open(filepath, O_READ))) 
  {
    lofile.seek(seek);
    emu_printf(size);
    if (lofile.read(buf, size) != (unsigned int)size) {
      emu_printf("File read failed");
    }        
    lofile.close();
  }
  return(filesize);
}

static bool emu_writeConfig(void)
{
  bool retval = false;
  if ((lofile = SD.open(ROMSDIR "/" AUTORUN_FILENAME, O_CREAT | O_WRITE)))
  {
    if (lofile.write(selection, strlen(selection)) != strlen(selection)) {
      emu_printf("Config write failed");
    }
    else {
      retval = true;
    }
    lofile.close();
  }
  return retval;
}

static bool emu_readConfig(void)
{
  bool retval = false;

  if ((lofile = SD.open(ROMSDIR "/" AUTORUN_FILENAME, O_READ)))
  {
    unsigned int filesize = lofile.size();
    unsigned int sizeread = lofile.read(selection, filesize);
    if (sizeread != filesize) {
      emu_printf("Config read failed");
    }
    else {
      if (sizeread == filesize)
      {
        selection[filesize]=0;
        retval = true;
      }
    }
    lofile.close();
  }
  return retval;
}

static bool emu_eraseConfig(void)
{
  SD.remove (ROMSDIR "/" AUTORUN_FILENAME);
}

/********************************
 * File IO compatibility
********************************/ 
#ifdef HAS_EXTFF

#include "ff.h"

typedef struct {
  File f;
  int offset;
  int size;
  int used;
} FileDesc;

typedef int * FIL;



#define NO_MMAP_HANDLES 32

static FileDesc fds[NO_MMAP_HANDLES];
static int nextHandle=0;

static void freeHandle(int h) {
  fds[h].used = 0;
}


static int getFreeHandle() {
  int n=NO_MMAP_HANDLES;
  while (fds[nextHandle].used!=0 && n!=0) {
    nextHandle++;
    if (nextHandle==NO_MMAP_HANDLES) nextHandle=0;
    n-1;
  }
  if (n==0) {
    emu_printf("getFreeHandle error"); 
    return;
  }
  
  int r=nextHandle;
    fds[r].used = 1;  
  nextHandle++;
  if (nextHandle==NO_MMAP_HANDLES) nextHandle=0;

  return r;
}

FRESULT f_open (FIL* fp, const char * path, unsigned char mode)
{
  emu_printf("fopen");
  emu_printf((char*)path);
  int i=getFreeHandle();
  emu_printf(i);
  fds[i].f = SD.open(path, O_READ);
  *fp = i;
  if  (fds[i].f) {
    fds[i].size = fds[i].f.size();
    emu_printi(fds[i].size);
    return(FR_OK);
  } 
  emu_printf("error");
  freeHandle(fds[i].f);
  return(FR_NO_FILE);
}

FRESULT f_close (FIL* fp)
{
  int i = *fp;
  emu_printf("fclose");
  emu_printi(i);
  fds[i].f.close();  
  freeHandle(i);  
  return(FR_OK);
}

FRESULT f_read (FIL* fp, void* buff, unsigned int  btr, unsigned int * br)
{
  int i = *fp;

  if (btr < 64738) {
    int nr = fds[i].f.read(buff, btr);
    //emu_printf("fread");
    //emu_printi(btr);  
    //emu_printi(nr);    
    *br = nr;
    if (nr <= 0) 
      return(FR_DISK_ERR);
    else
      return(FR_OK);
  }

  unsigned char buffer[256];
  
  int remaining = btr;
  int byteread = 0;
  int retval=0; 

    while (remaining>0) {
      if (remaining < 256)
        retval = fds[i].f.read(buffer, remaining);      
      else
        retval = fds[i].f.read(buffer, 256);
      if (retval>0) {
        memcpy(buff,buffer,retval);
        buff += retval;
        byteread += retval;     
        remaining -= retval;
      }
      else {
        break;
      }
    }    
    *br = byteread;
    //emu_printi(byteread);         
    if (byteread <= 0) 
      return(FR_DISK_ERR);
    else
      return(FR_OK);
}

FRESULT f_readn (FIL* fp, void* buff, unsigned int btr, unsigned int * br)
{
  return(f_read (fp, buff, btr, br));
}

FRESULT f_write (FIL* fp, const void* buff, unsigned int  btw, unsigned int * bw)
{
  return(FR_OK);
}
FRESULT f_writen (FIL* fp, const void* buff, unsigned int btw, unsigned int * bw)
{
  return(FR_OK);
}
FRESULT f_lseek (FIL* fp, unsigned long ofs)
{
  int i = *fp;
  //emu_printf("fseek");
  //emu_printi(ofs);  
  fds[i].f.seek(ofs);
  return(FR_OK);
}


FRESULT f_unlink (const char * path)
{
  return(FR_OK);
}
FRESULT f_rename (const char * path_old, const  char * path_new)
{
  return(FR_OK);
}
FRESULT f_stat (const char * path, FILINFO* fno)
{
  return(FR_OK);
}

unsigned long f_tell (FIL * fp)
{
  int i = *fp;
  emu_printf("ftell");
  return(fds[i].size);  
  //return(fds[i].f.ftell());
}

unsigned long f_size (FIL * fp)
{
  int i = *fp;
  emu_printf("fsize");
  emu_printi(fds[i].size); 
  return(fds[i].size);
}

FRESULT f_mkdir (const char* path)
{
  return(FR_OK);
}  
#endif


/********************************
 * Initialization
********************************/ 
void emu_init(void)
{
  Serial.begin(115200);

#ifdef HAS_USBKEY
  myusb.begin();
  keyboard1.attachPress(OnPress);
  keyboard1.attachRelease(OnRelease);
#endif

  while (!SD.begin(SD_CS))
  {
    Serial.println("SD begin failed, retrying...");
    delay(1000);    
  }
  strcpy(selection,ROMSDIR);
  
  FileHandlersInit();
  
  nbFiles = readNbFiles(); 

  Serial.print("SD initialized, files found: ");
  Serial.println(nbFiles);

  emu_InitJoysticks();
#ifdef SWAP_JOYSTICK
  joySwapped = true;   
#else
  joySwapped = false;   
#endif

#ifdef TEECOMPUTER
#ifndef HAS_T4_VGA
    tft.flipscreen(false);
#endif
#endif
  int keypressed = emu_ReadKeys();
  if (keypressed & MASK_JOY2_DOWN) {
    tft.fillScreenNoDma( RGBVAL16(0xff,0x00,0x00) );
    tft.drawTextNoDma(64,48,    (char*)" AUTURUN file erased", RGBVAL16(0xff,0xff,0x00), RGBVAL16(0xff,0x00,0x00), true);
    emu_eraseConfig();
    delay(1000);
  }
  else {
    if (emu_readConfig()) {
      autorun = true;
    }
  }  

  toggleMenu(true);
}


void emu_start(void)
{
  usbnavpad = 0;
}
