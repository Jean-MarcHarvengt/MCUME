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



#ifdef PICOMPUTER
static const unsigned short * keys;
static unsigned char keymatrix[6];
static int keymatrix_hitrow=-1;
static bool key_fn=false;
static bool key_alt=false;
static uint32_t keypress_t_ms=0;
static uint32_t last_t_ms=0;
static uint32_t hundred_ms_cnt=0;
static bool ledflash_toggle=false;
#endif
static int keyMap;

static bool joySwapped = false;
static uint16_t bLastState;
static int xRef;
static int yRef;
static uint8_t usbnavpad=0;


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







/********************************
 * Input and keyboard
********************************/ 
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
  // First joystick
#if INVY
#ifdef PIN_JOY2_1
  if ( !gpio_get(PIN_JOY2_1) ) joysval |= MASK_JOY2_DOWN;
#endif
#ifdef PIN_JOY2_2
  if ( !gpio_get(PIN_JOY2_2) ) joysval |= MASK_JOY2_UP;
#endif
#else
#ifdef PIN_JOY2_1
  if ( !gpio_get(PIN_JOY2_1) ) joysval |= MASK_JOY2_UP;
#endif
#ifdef PIN_JOY2_2
  if ( !gpio_get(PIN_JOY2_2) ) joysval |= MASK_JOY2_DOWN;
#endif
#endif
#if INVX
#ifdef PIN_JOY2_3
  if ( !gpio_get(PIN_JOY2_3) ) joysval |= MASK_JOY2_LEFT;
#endif
#ifdef PIN_JOY2_4
  if ( !gpio_get(PIN_JOY2_4) ) joysval |= MASK_JOY2_RIGHT;
#endif
#else
#ifdef PIN_JOY2_3
  if ( !gpio_get(PIN_JOY2_3) ) joysval |= MASK_JOY2_RIGHT;
#endif
#ifdef PIN_JOY2_4
  if ( !gpio_get(PIN_JOY2_4) ) joysval |= MASK_JOY2_LEFT;
#endif
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
  return(bLastState/*|((joySwapped?1:0)<<7)*/);
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
//    gpio_set_dir(cols[i], GPIO_OUT);
    gpio_put(cols[i], 0);
#ifdef SWAP_ALT_DEL
    sleep_us(1);
    //__asm volatile ("nop\n"); // 4-8ns
#endif    
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
    gpio_put(cols[i], 1);
//    gpio_set_dir(cols[i], GPIO_IN);   
    keymatrix[i]=row;
  }

#ifdef SWAP_ALT_DEL
  // Swap ALT and DEL  
  unsigned char alt = keymatrix[0] & 0x02;
  unsigned char del = keymatrix[5] & 0x20;
  keymatrix[0] &= ~0x02;
  keymatrix[5] &= ~0x20;
  if (alt) keymatrix[5] |= 0x20;
  if (del) keymatrix[0] |= 0x02;
#endif

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
  uint32_t time_ms=to_ms_since_boot (get_absolute_time());
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
        if ((to_ms_since_boot (get_absolute_time())-keypress_t_ms) > 1000) 
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
    gpio_put(KLED, (ledflash_toggle?1:0));
  }
  else {
    if (key_fn == true) {
      gpio_put(KLED, 1);
    }
    else {
      gpio_put(KLED, 0);
    }     
  } 
 
  if ( key_fn ) retval |= MASK_KEY_USER2;
  if ( ( key_fn ) && (keymatrix[0] == 0x02 )) retval |= MASK_KEY_USER1;
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
    //if ( (match == 0x002 )  ) return 0; // shift or fn
    //if (match < 0x100 ) match = match & ~0x002; // ignore shift key
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

unsigned char emu_ReadI2CKeyboard2(int row) {
  int retval=0;
#ifdef PICOMPUTER
  retval = keymatrix[row];
#endif
  return retval;
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

  // User keys   
#ifdef PIN_KEY_USER1
  gpio_set_pulls(PIN_KEY_USER1,true,false);
  gpio_set_dir(PIN_KEY_USER1,GPIO_IN);  
#endif  
#ifdef PIN_KEY_USER2
  gpio_set_dir(PIN_KEY_USER2,GPIO_IN);
  gpio_set_pulls(PIN_KEY_USER2,true,false);
#endif  
#ifdef PIN_KEY_USER3
  gpio_set_pulls(PIN_KEY_USER3,true,false);
  gpio_set_dir(PIN_KEY_USER3,GPIO_IN);  
#endif  
#ifdef PIN_KEY_USER4
  gpio_set_pulls(PIN_KEY_USER4,true,false);
  gpio_set_dir(PIN_KEY_USER4,GPIO_IN);  
#endif  

  // First Joystick   
#ifdef PIN_JOY2_1
  gpio_set_pulls(PIN_JOY2_1,true,false);
  gpio_set_dir(PIN_JOY2_1,GPIO_IN);
  gpio_set_input_enabled(PIN_JOY2_1, true); // Force ADC as digital input        
#endif  
#ifdef PIN_JOY2_2
  gpio_set_pulls(PIN_JOY2_2,true,false);
  gpio_set_dir(PIN_JOY2_2,GPIO_IN);  
  gpio_set_input_enabled(PIN_JOY2_2, true);  // Force ADC as digital input       
#endif  
#ifdef PIN_JOY2_3
  gpio_set_pulls(PIN_JOY2_3,true,false);
  gpio_set_dir(PIN_JOY2_3,GPIO_IN);  
  gpio_set_input_enabled(PIN_JOY2_3, true);  // Force ADC as digital input        
#endif  
#ifdef PIN_JOY2_4
  gpio_set_pulls(PIN_JOY2_4,true,false);
  gpio_set_dir(PIN_JOY2_4,GPIO_IN);  
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
  // keyboard LED
  gpio_init(KLED);
  gpio_set_dir(KLED, GPIO_OUT);
  gpio_put(KLED, 1);

  // Output (rows)
  gpio_init(1);
  gpio_init(2);
  gpio_init(3);
  gpio_init(4);
  gpio_init(5);
  gpio_init(14);
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
  
/*
  gpio_put(1, 0);
  gpio_put(2, 0);
  gpio_put(3, 0);
  gpio_put(4, 0);
  gpio_put(5, 0);
  gpio_put(14, 0);
  gpio_set_pulls(1,true,true);
  gpio_set_pulls(2,true,true);
  gpio_set_pulls(3,true,true);
  gpio_set_pulls(4,true,true);
  gpio_set_pulls(5,true,true);
  gpio_set_pulls(14,true,true);
  gpio_set_dir(1, GPIO_IN); 
  gpio_set_dir(2, GPIO_IN); 
  gpio_set_dir(3, GPIO_IN); 
  gpio_set_dir(4, GPIO_IN); 
  gpio_set_dir(5, GPIO_IN); 
  gpio_set_dir(14, GPIO_IN); 
*/

  // Input pins (cols)
  gpio_init(6);
  gpio_init(9);
  gpio_init(15);
  gpio_init(8);
  gpio_init(7);
  gpio_init(22);
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



void emu_init(void)
{  
  emu_InitJoysticks();
#ifdef SWAP_JOYSTICK
  joySwapped = true;   
#else
  joySwapped = false;   
#endif  

#ifdef PICOMPUTER
  // Flip screen if UP pressed
  if (emu_ReadKeys() & MASK_JOY2_UP)
  {
#ifdef PICOMPUTERMAX
    tft.flipscreen(true);
#else
    tft.flipscreen(true);
#endif
  }
  else 
  {
#ifdef PICOMPUTERMAX
    tft.flipscreen(false);
#else
    tft.flipscreen(false);
#endif
  }
#endif
}


void emu_start(void)
{

  usbnavpad = 0;

  keyMap = 0;
}
