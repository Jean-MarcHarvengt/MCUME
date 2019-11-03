/*
  Based on C64 ILI9341 dma driver from Frank Bösing, 2017
*/

#include "TFT_T_DMA.h"
#include "font8x8.h"


#define SPICLOCK 144e6 //Just a number..max speed
#ifdef ILI9341
#define SPI_MODE SPI_MODE0
#endif
#ifdef ST7789
#define SPI_MODE SPI_MODE2
#endif

// touch
#define SPI_SETTING         SPISettings(2500000, MSBFIRST, SPI_MODE)
#define XPT2046_CFG_START   _BV(7)
#define XPT2046_CFG_MUX(v)  ((v&0b111) << (4))
#define XPT2046_CFG_8BIT    _BV(3)
#define XPT2046_CFG_12BIT   (0)
#define XPT2046_CFG_SER     _BV(2)
#define XPT2046_CFG_DFR     (0)
#define XPT2046_CFG_PWR(v)  ((v&0b11))
#define XPT2046_MUX_Y       0b101
#define XPT2046_MUX_X       0b001
#define XPT2046_MUX_Z1      0b011
#define XPT2046_MUX_Z2      0b100


#ifdef TFT_STATICFB
static uint16_t fb0[LINES_PER_BLOCK*TFT_WIDTH];
static uint16_t fb1[LINES_PER_BLOCK*TFT_WIDTH];
static uint16_t fb2[LINES_PER_BLOCK*TFT_WIDTH];
static uint16_t fb3[(TFT_HEIGHT-3*LINES_PER_BLOCK)*TFT_WIDTH];
static uint16_t * blocks[NR_OF_BLOCK]={fb0,fb1,fb2,fb3};
#else
static uint16_t * blocks[NR_OF_BLOCK];
#endif


static DMASetting dmasettings[SCREEN_DMA_NUM_SETTINGS];
static DMAChannel dmatx;//(false);
static volatile uint8_t rstop = 0;
static volatile bool cancelled = false;
static volatile uint8_t curTransfer = 0;
static uint8_t nbTransfer = 0;


PROGMEM static const uint8_t init_commands[] = {
#ifdef ILI9341  
  4, 0xEF, 0x03, 0x80, 0x02,
  4, 0xCF, 0x00, 0XC1, 0X30,
  5, 0xED, 0x64, 0x03, 0X12, 0X81,
  4, 0xE8, 0x85, 0x00, 0x78,
  6, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,
  2, 0xF7, 0x20,
  3, 0xEA, 0x00, 0x00,
  2, ILI9341_PWCTR1, 0x23, // Power control
  2, ILI9341_PWCTR2, 0x10, // Power control
  3, ILI9341_VMCTR1, 0x3e, 0x28, // VCM control
  2, ILI9341_VMCTR2, 0x86, // VCM control2
  2, ILI9341_MADCTL, 0x48, // Memory Access Control
  2, ILI9341_PIXFMT, 0x55,
  3, ILI9341_FRMCTR1, 0x00, 0x18,
  4, ILI9341_DFUNCTR, 0x08, 0x82, 0x27, // Display Function Control
  2, 0xF2, 0x00, // Gamma Function Disable
  2, ILI9341_GAMMASET, 0x01, // Gamma curve selected
  16, ILI9341_GMCTRP1, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08,
  0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
  16, ILI9341_GMCTRN1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07,
  0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma
//  3, 0xb1, 0x00, 0x1f, // FrameRate Control 61Hz
  3, 0xb1, 0x00, 0x10, // FrameRate Control 119Hz
  2, ILI9341_MADCTL, ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR,
  0
#endif
#ifdef ST7789
#define DELAY 0x80
    9,                       // 9 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
      150,                     //    150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
      255,                    //     255 = 500 ms delay
    ST7735_COLMOD , 1+DELAY,  //  3: Set color mode, 1 arg + delay:
      0x55,                   //     16-bit color
      10,                     //     10 ms delay
    ST7735_MADCTL , 1      ,  //  4: Memory access ctrl (directions), 1 arg:
      0x08,                   //     Row addr/col addr, bottom to top refresh
    ST7735_CASET  , 4      ,  //  5: Column addr set, 4 args, no delay:
      0x00, 
      0x00,                   //     XSTART = 0
      0x00, 
      240,                    //      XEND = 240
    ST7735_RASET  , 4      ,  // 6: Row addr set, 4 args, no delay:
      0x00, 
      0x00,                   //     YSTART = 0
      320>>8, 
      320 & 0xFF,             //      YEND = 320
    ST7735_INVON ,   DELAY,   // 7: hack
      10,
    ST7735_NORON  ,   DELAY,  // 8: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,   DELAY,  // 9: Main screen turn on, no args, w/delay
    255
#endif       
};


static void dmaInterrupt() {
  dmatx.clearInterrupt();
  curTransfer++;
  if (curTransfer >= nbTransfer) {
    curTransfer = 0;
    if (cancelled) {
        dmatx.disable();
        rstop = 1;
    }
  }
#if defined(__IMXRT1052__) || defined(__IMXRT1062__)    
  arm_dcache_flush(blocks[curTransfer], LINES_PER_BLOCK*TFT_WIDTH*2);
#endif  
}

static void setDmaStruct() {
  uint32_t remaining = TFT_HEIGHT*TFT_WIDTH*2;
  int i=0;
  uint16_t col=RGBVAL16(0x00,0x00,0x00);;
  while (remaining > 0) {
    uint16_t * fb = blocks[i];
    int32_t len = (remaining >= (LINES_PER_BLOCK*TFT_WIDTH*2)?LINES_PER_BLOCK*TFT_WIDTH*2:remaining);
#ifdef TFT_DEBUG        
    Serial.println((unsigned long)blocks[i]);    
    Serial.println(remaining);
#endif        
    switch (i) {
      case 0:
        if (fb == 0) fb = (uint16_t*)((int)malloc(len+64)&0xffffffe0);
        //fb=&fb0[0];
#ifdef TFT_DEBUG        
        col = RGBVAL16(0x00,0xff,0x00);
#endif        
        break;
      case 1:
        if (fb == 0) fb = (uint16_t*)((int)malloc(len+64)&0xffffffe0);
        //fb=&fb1[0];
#ifdef TFT_DEBUG        
        col = RGBVAL16(0x00,0xff,0xff);
#endif        
        break;
      case 2:
        if (fb == 0) fb = (uint16_t*)((int)malloc(len+64)&0xffffffe0);
        //fb=&fb2[0];
#ifdef TFT_DEBUG        
        col = RGBVAL16(0x00,0x00,0xff);
#endif        
        break;
      case 3:
        if (fb == 0) fb = (uint16_t*)((int)malloc(len+64)&0xffffffe0);
        //fb=&fb3[0];
#ifdef TFT_DEBUG        
        col = RGBVAL16(0xff,0x00,0xff);
#endif        
        break;
    }
    blocks[i] = fb;
    if (blocks[i] == 0) {
      Serial.print("ILI9341 allocaltion failed for block ");
      Serial.println(i);
      delay(10000);    
    }
       
    for (int j=0;j<len/2;j++) fb[j]=col;

#if defined(__IMXRT1052__) || defined(__IMXRT1062__)    
    dmasettings[i].sourceBuffer(fb, len);
    dmasettings[i].destination((uint8_t &)  LPSPI4_TDR);
    dmasettings[i].TCD->ATTR_DST = 1;
#else
    dmasettings[i].sourceBuffer(fb, len);
    dmasettings[i].destination((uint8_t &)  SPI0_PUSHR);
    dmasettings[i].TCD->ATTR_DST = 1;
#endif       
    dmasettings[i].replaceSettingsOnCompletion(dmasettings[i+1]);
    dmasettings[i].interruptAtCompletion();
    remaining -= len;
    i++;
  }
  dmasettings[i-1].replaceSettingsOnCompletion(dmasettings[0]);
  nbTransfer = i;
#ifdef TFT_DEBUG        
  Serial.println(nbTransfer);
#endif  
}


TFT_T_DMA::TFT_T_DMA(uint8_t cs, uint8_t dc, uint8_t rst, uint8_t mosi, uint8_t sclk, uint8_t miso,  uint8_t touch_cs,  uint8_t touch_irq)
{
  _cs   = cs;
  _dc   = dc;
  _rst  = rst;
  _mosi = mosi;
  _sclk = sclk;
  _miso = miso;
  pinMode(_dc, OUTPUT);
  pinMode(_cs, OUTPUT); 
  digitalWrite(_cs, 1);
  digitalWrite(_dc, 1);
  if ( (touch_cs != 255) && (touch_irq != 255) ) {
    _touch_irq = touch_irq;
    _touch_cs = touch_cs;
    pinMode(_touch_cs, OUTPUT);
    pinMode(touch_irq, INPUT_PULLUP);  
    digitalWrite(_touch_cs, 1);    
  }
}


void TFT_T_DMA::setArea(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2) {
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  digitalWrite(_cs, 0);

  digitalWrite(_dc, 0);
  SPI.transfer(TFT_CASET);
  digitalWrite(_dc, 1);
  SPI.transfer16(x1);
  SPI.transfer16(x2);

  digitalWrite(_dc, 0);
  SPI.transfer(TFT_PASET);
  digitalWrite(_dc, 1);
  SPI.transfer16(y1);
  SPI.transfer16(y2);

  digitalWrite(_dc, 0);
  SPI.transfer(TFT_RAMWR);
  digitalWrite(_dc, 1);
  
  digitalWrite(_cs, 1);
  SPI.endTransaction();  
}


void TFT_T_DMA::begin(void) {
  SPI.setMOSI(_mosi);
  SPI.setMISO(_miso);
  SPI.setSCK(_sclk);
  SPI.begin();
      
  // Initialize display
  if (_rst < 255) { // toggle RST low to reset
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(5);
    digitalWrite(_rst, LOW);
    delay(20);
    digitalWrite(_rst, HIGH);
    delay(120);
  }
  
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  const uint8_t *addr = init_commands;
  digitalWrite(_cs, 0);
#ifdef ILI9341 
  while (1) {
    uint8_t count = *addr++;
    if (count-- == 0) break;

    digitalWrite(_dc, 0);
    SPI.transfer(*addr++);

    while (count-- > 0) {
      digitalWrite(_dc, 1);
      SPI.transfer(*addr++);
    }
  }
  
  digitalWrite(_dc, 0);
  SPI.transfer(ILI9341_SLPOUT);
  digitalWrite(_dc, 1);
  digitalWrite(_cs, 1);
  SPI.endTransaction();

  digitalWrite(_dc, 1);
  digitalWrite(_cs, 1);
  SPI.endTransaction();

  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  digitalWrite(_dc, 0);
  digitalWrite(_cs, 0);
  SPI.transfer(ILI9341_DISPON);
  digitalWrite(_dc, 1);
  digitalWrite(_cs, 1);
  SPI.endTransaction();
#endif
#ifdef ST7789 
  uint8_t  numCommands, numArgs;
  uint16_t ms;
  numCommands = *addr++;    // Number of commands to follow
  while(numCommands--) {        // For each command...
    digitalWrite(_dc, 0);
    SPI.transfer(*addr++);    
    numArgs  = *addr++; //   Number of args to follow
    ms       = numArgs & DELAY;   //   If hibit set, delay follows args
    numArgs &= ~DELAY;      //   Mask out delay bit
    while(numArgs > 1) {      //   For each argument...
      digitalWrite(_dc, 1);
      SPI.transfer(*addr++);
      numArgs--;
    }

    if (numArgs)  {
      digitalWrite(_dc, 1);
      SPI.transfer(*addr++);
    }
    if(ms) {
      ms = *addr++; // Read post-command delay time (ms)
      if(ms == 255) ms = 500;   // If 255, delay for 500 ms
      SPI.endTransaction();
      digitalWrite(_dc, 1);
      digitalWrite(_cs, 1);
      delay(ms);
      //beginSPITransaction();
    }
  }
#endif

  setArea(0, 0, TFT_REALWIDTH-1, TFT_REALHEIGHT-1);  

  cancelled = false; 
#ifdef FLIP_SCREEN          
  flipscreen(true);           
#else
  flipscreen(false);  
#endif            
};


  
void TFT_T_DMA::flipscreen(bool flip)
{
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  digitalWrite(_dc, 0);
  digitalWrite(_cs, 0);
  SPI.transfer(TFT_MADCTL);
  digitalWrite(_dc, 1);
  if (flip) {
    flipped=true;
#ifdef ILI9341         
    SPI.transfer(ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
#endif
#ifdef ST7789
    //SPI.transfer(ST77XX_MADCTL_RGB);
    SPI.transfer(ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB);
#endif    
  }
  else {
    flipped=false;
#ifdef ILI9341        
    SPI.transfer(ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
#endif
#ifdef ST7789
    SPI.transfer(ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB);
#endif    
  }
  digitalWrite(_cs, 1);  
  SPI.endTransaction();
}

boolean TFT_T_DMA::isflipped(void)
{
  return(flipped);
}
  

#define PRREG(x) Serial.print(#x" 0x"); Serial.println(x,HEX)


void TFT_T_DMA::startDMA(void) {
  curTransfer = 0;  
  rstop = 0;     
  //dmatx.begin(true);
  dmatx.attachInterrupt(dmaInterrupt);
  setDmaStruct();
  setArea((TFT_REALWIDTH-TFT_WIDTH)/2, (TFT_REALHEIGHT-TFT_HEIGHT)/2, (TFT_REALWIDTH-TFT_WIDTH)/2 + TFT_WIDTH-1, (TFT_REALHEIGHT-TFT_HEIGHT)/2+TFT_HEIGHT-1);  
  fillScreen(RGBVAL16(0x00,0x00,0x00));
  
  digitalWrite(_cs, HIGH);
  SPI.begin();
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
#if defined(__IMXRT1052__) || defined(__IMXRT1062__)

#ifdef TFT_DEBUG          
  PRREG(LPSPI4_CCR);
  PRREG(LPSPI4_TCR);
  PRREG(LPSPI4_FCR);
  Serial.printf("SPI CLOCK %d CCR freq %.1f MHz\n", SPICLOCK, 528. / 7 / ((0xff & LPSPI4_CCR) + 2));
#endif
  LPSPI4_CR &= ~LPSPI_CR_MEN;//disable LPSPI:
  LPSPI4_CFGR1 |= LPSPI_CFGR1_NOSTALL; //prevent stall from RX
  LPSPI4_TCR = 15; // Framesize 16 Bits
  //LPSPI4_FCR = 0; // Fifo Watermark
  LPSPI4_DER = LPSPI_DER_TDDE; //TX DMA Request Enable
  LPSPI4_CR |= LPSPI_CR_MEN; //enable LPSPI:
  dmatx.triggerAtHardwareEvent( DMAMUX_SOURCE_LPSPI4_TX );
#else  
  SPI0_RSER |= SPI_RSER_TFFF_DIRS | SPI_RSER_TFFF_RE;  // Set ILI_DMA Interrupt Request Select and Enable register
  SPI0_MCR &= ~SPI_MCR_HALT;  //Start transfers.
  SPI0_CTAR0 = SPI0_CTAR1;
  (*(volatile uint16_t *)((int)&SPI0_PUSHR + 2)) = (SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT) >> 16; //Enable 16 Bit Transfers + Continue-Bit
  dmatx.triggerAtHardwareEvent(DMAMUX_SOURCE_SPI0_TX );
#endif
  dmatx = dmasettings[0];
  digitalWrite(_cs, 0); 
  digitalWrite(_dc, 0);
  SPI.transfer(TFT_RAMWR); 
  digitalWrite(_dc, 1);      
  dmatx.enable();
}


void TFT_T_DMA::stopDMA(void) {
  rstop = 0;
  wait();
  delay(50);
  cancelled = false;  
  dmatx.detachInterrupt();
  fillScreen(RGBVAL16(0x00,0x00,0x00));
  SPI.end();
#ifdef ST7789  
  begin();
#endif
#ifdef ILI9341 
  SPI.begin();
  digitalWrite(_cs, 0);
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));  
  SPI.endTransaction();
  digitalWrite(_cs, 1);
  digitalWrite(_dc, 1); 
#endif    
  setArea(0, 0, TFT_REALWIDTH-1, TFT_REALHEIGHT-1);    
}

void TFT_T_DMA::wait(void) {
  rstop = 1;
  unsigned long m = millis(); 
  cancelled = true; 
  while (!rstop)  {
    if ((millis() - m) > 100) break;
    delay(10);
    asm volatile("wfi");
  };
  rstop = 0;
}


/***********************************************************************************************
    Touch functions
 ***********************************************************************************************/
/* Code based on ...
 *
 * @file XPT2046.cpp
 * @date 19.02.2016
 * @author Markus Sattler
 *
 * Copyright (c) 2015 Markus Sattler. All rights reserved.
 * This file is part of the XPT2046 driver for Arduino.
 */

#define ADC_MAX                 0x0fff  

void TFT_T_DMA::enableTouchIrq() 
{
    SPI.beginTransaction(SPI_SETTING);
    digitalWrite(_touch_cs, LOW);
    const uint8_t buf[4] = { (XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_Y)), 0x00, 0x00, 0x00 };
    SPI.transfer((void*)&buf[0],3);   
    digitalWrite(_touch_cs, HIGH);
    SPI.endTransaction();
}

//Default callibration for non flipped
#define TX_MIN 30
#define TY_MIN 20
#define TX_MAX 300
#define TY_MAX 220

//Default callibration for flipped
#define TFX_MIN 20
#define TFY_MIN 25
#define TFX_MAX 288
#define TFY_MAX 221

static uint16_t txMin;
static uint16_t tyMin;
static uint16_t txMax;
static uint16_t tyMax;

 
void TFT_T_DMA::callibrateTouch(uint16_t xMin,uint16_t yMin,uint16_t xMax,uint16_t yMax)  {
  if ( (xMin >= 0) && (yMin >= 0) && (xMax < 320) && (yMax < 200) ) {
      txMin = xMin;
      tyMin = yMin;
      txMax = xMax;
      tyMax = yMax;     
  }
  else {
    if (flipped) {
      txMin = TFX_MIN;
      tyMin = TFY_MIN;
      txMax = TFX_MAX;
      tyMax = TFY_MAX;              
    }
    else {
      txMin = TX_MIN;
      tyMin = TY_MIN;
      txMax = TX_MAX;
      tyMax = TY_MAX;      
    }
  }
}


void TFT_T_DMA::readRaw(uint16_t * oX, uint16_t * oY, uint16_t * oZ) {
  if ( TOUCH_ENABLED() ) {
    uint16_t x = 0;
    uint16_t y = 0;
    uint16_t z1 = 0;
    uint16_t z2 = 0;
    uint8_t i = 0;
    int16_t xraw=0, yraw=0;
  
    SPI.beginTransaction(SPI_SETTING);
    digitalWrite(_touch_cs, LOW);
  
    for(; i < 15; i++) {
      // SPI requirer 32bit aliment
      uint8_t buf[12] = {
        (XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_Y) | XPT2046_CFG_PWR(3)), 0x00, 0x00,
        (XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_X) | XPT2046_CFG_PWR(3)), 0x00, 0x00,
        (XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_Z1)| XPT2046_CFG_PWR(3)), 0x00, 0x00,
        (XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_Z2)| XPT2046_CFG_PWR(3)), 0x00, 0x00
      };
      SPI.transfer(&buf[0], &buf[0], 12);
      y += (buf[1] << 8 | buf[2])>>3;
      x += (buf[4] << 8 | buf[5])>>3;
      z1 += (buf[7] << 8 | buf[8])>>3;
      z2 += (buf[10] << 8 | buf[11])>>3;
    }
    enableTouchIrq();
  
    if(i == 0) {
        *oX = 0;
        *oY = 0;
        *oZ = 0;
    }
    else {
        x /= i;
        y /= i;
        z1 /= i;
        z2 /= i;
    }
  
    digitalWrite(_touch_cs, HIGH);
    SPI.endTransaction();
    int z = z1 + ADC_MAX - z2;
    if (flipped) {
      xraw = x;
      yraw = y;
    } else {
      xraw = ADC_MAX - x;
      yraw = ADC_MAX - y;
    }
    xraw=(xraw*TFT_REALWIDTH)/(ADC_MAX+1);
    yraw=(yraw*TFT_REALHEIGHT)/(ADC_MAX+1);
  
    *oX = xraw;
    *oY = yraw;
    *oZ = z;
  }
  else 
  {
    *oX = 0;
    *oY = 0;
    *oZ = 0;    
  }
}

void TFT_T_DMA::readCal(uint16_t * oX, uint16_t * oY, uint16_t * oZ) {
  readRaw(oX,oY,oZ);
  // callibrate ...
  if(*oX >= txMin) *oX = ((*oX - txMin)*TFT_REALWIDTH)/(txMax-txMin);
  if(*oY >= tyMin) *oY = ((*oY - tyMin)*TFT_REALHEIGHT)/(tyMax-tyMin);
  //Serial.print(*oX);
  //Serial.print(" ");
  //Serial.println(*oY);
}


/***********************************************************************************************
    No DMA functions
 ***********************************************************************************************/
void TFT_T_DMA::fillScreenNoDma(uint16_t color) {
  setArea(0, 0, TFT_REALWIDTH-1, TFT_REALHEIGHT-1);  
  
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  digitalWrite(_cs, 0);
  digitalWrite(_dc, 0);
  SPI.transfer(TFT_RAMWR);
  int i,j;
  for (j=0; j<TFT_REALHEIGHT; j++)
  {
    for (i=0; i<TFT_REALWIDTH; i++) {
      digitalWrite(_dc, 1);
      SPI.transfer16(color);     
    }
  }
#ifdef ILI9341  
  digitalWrite(_dc, 0);
  SPI.transfer(ILI9341_SLPOUT);
  digitalWrite(_dc, 1);
#endif
  digitalWrite(_cs, 1);
  SPI.endTransaction();  
  
  setArea(0, 0, (TFT_REALWIDTH-1), (TFT_REALHEIGHT-1));  
}


void TFT_T_DMA::writeScreenNoDma(const uint16_t *pcolors) {
  setArea(0, 0, TFT_WIDTH-1, TFT_HEIGHT-1);  
  
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  digitalWrite(_cs, 0);
  digitalWrite(_dc, 0);
  SPI.transfer(TFT_RAMWR);
  int i,j;
  for (j=0; j<240; j++)
  {
    for (i=0; i<TFT_WIDTH; i++) {
      digitalWrite(_dc, 1);
      SPI.transfer16(*pcolors++);     
    }
  }
#ifdef ILI9341  
  digitalWrite(_dc, 0);
  SPI.transfer(ILI9341_SLPOUT);
  digitalWrite(_dc, 1);
#endif
  digitalWrite(_cs, 1);
  SPI.endTransaction();  
  
  setArea(0, 0, (TFT_REALWIDTH-1), (TFT_REALHEIGHT-1));  
}

void TFT_T_DMA::drawSpriteNoDma(int16_t x, int16_t y, const uint16_t *bitmap) {
    drawSpriteNoDma(x,y,bitmap, 0,0,0,0);
}

void TFT_T_DMA::drawSpriteNoDma(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t arx, uint16_t ary, uint16_t arw, uint16_t arh)
{
  int bmp_offx = 0;
  int bmp_offy = 0;
  uint16_t *bmp_ptr;
    
  int w =*bitmap++;
  int h = *bitmap++;
//Serial.println(w);
//Serial.println(h);

  if ( (arw == 0) || (arh == 0) ) {
    // no crop window
    arx = x;
    ary = y;
    arw = w;
    arh = h;
  }
  else {
    if ( (x>(arx+arw)) || ((x+w)<arx) || (y>(ary+arh)) || ((y+h)<ary)   ) {
      return;
    }
    
    // crop area
    if ( (x > arx) && (x<(arx+arw)) ) { 
      arw = arw - (x-arx);
      arx = arx + (x-arx);
    } else {
      bmp_offx = arx;
    }
    if ( ((x+w) > arx) && ((x+w)<(arx+arw)) ) {
      arw -= (arx+arw-x-w);
    }  
    if ( (y > ary) && (y<(ary+arh)) ) {
      arh = arh - (y-ary);
      ary = ary + (y-ary);
    } else {
      bmp_offy = ary;
    }
    if ( ((y+h) > ary) && ((y+h)<(ary+arh)) ) {
      arh -= (ary+arh-y-h);
    }     
  }

  setArea(arx, ary, arx+arw-1, ary+arh-1);  
  
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  digitalWrite(_cs, 0);
  digitalWrite(_dc, 0);
  SPI.transfer(TFT_RAMWR);      

  bitmap = bitmap + bmp_offy*w + bmp_offx;
  for (int row=0;row<arh; row++)
  {
    bmp_ptr = (uint16_t*)bitmap;
    for (int col=0;col<arw; col++)
    {
        uint16_t color = *bmp_ptr++;
        digitalWrite(_dc, 1);
        SPI.transfer16(color);             
    } 
    bitmap +=  w;
  }
#ifdef ILI9341  
  digitalWrite(_dc, 0);
  SPI.transfer(ILI9341_SLPOUT);
  digitalWrite(_dc, 1);
#endif
  digitalWrite(_cs, 1);
  SPI.endTransaction();   
  setArea(0, 0, TFT_REALWIDTH-1, TFT_REALHEIGHT-1);  
}

void TFT_T_DMA::drawTextNoDma(int16_t x, int16_t y, const char * text, uint16_t fgcolor, uint16_t bgcolor, bool doublesize) {
  uint16_t c;
  while ((c = *text++)) {
    const unsigned char * charpt=&font8x8[c][0];

    setArea(x,y,x+7,y+(doublesize?15:7));
  
    //SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
    digitalWrite(_cs, 0);
    //digitalWrite(_dc, 0);
    //SPI.transfer(TFT_RAMWR);

    digitalWrite(_dc, 1);
    for (int i=0;i<8;i++)
    {
      unsigned char bits;
      if (doublesize) {
        bits = *charpt;     
        digitalWrite(_dc, 1);
        if (bits&0x01) SPI.transfer16(fgcolor);
        else SPI.transfer16(bgcolor);
        bits = bits >> 1;     
        if (bits&0x01) SPI.transfer16(fgcolor);
        else SPI.transfer16(bgcolor);
        bits = bits >> 1;     
        if (bits&0x01) SPI.transfer16(fgcolor);
        else SPI.transfer16(bgcolor);
        bits = bits >> 1;     
        if (bits&0x01) SPI.transfer16(fgcolor);
        else SPI.transfer16(bgcolor);
        bits = bits >> 1;     
        if (bits&0x01) SPI.transfer16(fgcolor);
        else SPI.transfer16(bgcolor);
        bits = bits >> 1;     
        if (bits&0x01) SPI.transfer16(fgcolor);
        else SPI.transfer16(bgcolor);
        bits = bits >> 1;     
        if (bits&0x01) SPI.transfer16(fgcolor);
        else SPI.transfer16(bgcolor);
        bits = bits >> 1;     
        if (bits&0x01) SPI.transfer16(fgcolor);
        else SPI.transfer16(bgcolor);       
      }
      bits = *charpt++;     
      digitalWrite(_dc, 1);
      if (bits&0x01) SPI.transfer16(fgcolor);
      else SPI.transfer16(bgcolor);
      bits = bits >> 1;     
      if (bits&0x01) SPI.transfer16(fgcolor);
      else SPI.transfer16(bgcolor);
      bits = bits >> 1;     
      if (bits&0x01) SPI.transfer16(fgcolor);
      else SPI.transfer16(bgcolor);
      bits = bits >> 1;     
      if (bits&0x01) SPI.transfer16(fgcolor);
      else SPI.transfer16(bgcolor);
      bits = bits >> 1;     
      if (bits&0x01) SPI.transfer16(fgcolor);
      else SPI.transfer16(bgcolor);
      bits = bits >> 1;     
      if (bits&0x01) SPI.transfer16(fgcolor);
      else SPI.transfer16(bgcolor);
      bits = bits >> 1;     
      if (bits&0x01) SPI.transfer16(fgcolor);
      else SPI.transfer16(bgcolor);
      bits = bits >> 1;     
      if (bits&0x01) SPI.transfer16(fgcolor);
      else SPI.transfer16(bgcolor);
    }
    x +=8;
#ifdef ILI9341  
  digitalWrite(_dc, 0);
  SPI.transfer(ILI9341_SLPOUT);
  digitalWrite(_dc, 1);
#endif
    digitalWrite(_cs, 1);
    SPI.endTransaction();  
  }
  
  setArea(0, 0, (TFT_REALWIDTH-1), (TFT_REALHEIGHT-1));  
}


void TFT_T_DMA::drawRectNoDma(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  setArea(x,y,x+w-1,y+h-1);
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  digitalWrite(_cs, 0);
  digitalWrite(_dc, 0);
  SPI.transfer(TFT_RAMWR);
  int i;
  for (i=0; i<(w*h); i++)
  {
    digitalWrite(_dc, 1);
    SPI.transfer16(color);
  }
#ifdef ILI9341  
  digitalWrite(_dc, 0);
  SPI.transfer(ILI9341_SLPOUT);
  digitalWrite(_dc, 1);
#endif
  digitalWrite(_cs, 1);
  SPI.endTransaction();  
  
  setArea(0, 0, (TFT_REALWIDTH-1), (TFT_REALHEIGHT-1));
}



/***********************************************************************************************
    DMA functions
 ***********************************************************************************************/
uint16_t * TFT_T_DMA::getLineBuffer(int j)
{
  uint16_t * block=blocks[j>>6];  
  return(&block[(j&0x3F)*TFT_REALWIDTH]);
}

void TFT_T_DMA::writeScreen(int width, int height, int stride, uint8_t *buf, uint16_t *palette16) {
  uint8_t *buffer=buf;
  uint8_t *src; 

  int i,j,y=0;
  if (width*2 <= TFT_REALWIDTH) {
    for (j=0; j<height; j++)
    {
      uint16_t * block=blocks[y>>6];
      uint16_t * dst=&block[(y&0x3F)*TFT_WIDTH];        
      src=buffer;
      for (i=0; i<width; i++)
      {
        uint16_t val = palette16[*src++];
        *dst++ = val;
        *dst++ = val;
      }
      y++;
      if (height*2 <= TFT_HEIGHT) {
        block=blocks[y>>6];
        dst=&block[(y&0x3F)*TFT_WIDTH];          
        src=buffer;
        for (i=0; i<width; i++)
        {
          uint16_t val = palette16[*src++];
          *dst++ = val;
          *dst++ = val;
        }
        y++;      
      } 
      buffer += stride;      
    }
  }
  else if (width <= TFT_REALWIDTH) {
    //dst += (TFT_WIDTH-width)/2;
    for (j=0; j<height; j++)
    {
      uint16_t * block=blocks[y>>6];
      uint16_t * dst=&block[(y&0x3F)*TFT_WIDTH+(TFT_WIDTH-width)/2];         
      src=buffer;
      for (i=0; i<width; i++)
      {
        uint16_t val = palette16[*src++];
        *dst++ = val;
      }
      y++;
      if (height*2 <= TFT_HEIGHT) {
        block=blocks[y>>6];
        dst=&block[(y&0x3F)*TFT_WIDTH+(TFT_WIDTH-width)/2];        
        src=buffer;
        for (i=0; i<width; i++)
        {
          uint16_t val = palette16[*src++];
          *dst++ = val;
        }
        y++;
      }      
      buffer += stride;  
    }
  }   
}

void TFT_T_DMA::writeLine(int width, int height, int y, uint8_t *buf, uint16_t *palette16) {
  uint16_t * block=blocks[y>>6];
  uint16_t * dst=&block[(y&0x3F)*TFT_WIDTH]; 
  if (width > TFT_WIDTH) {
#ifdef TFT_LINEARINT    
    int delta = (width/(width-TFT_WIDTH))-1;
    int pos = delta;
    for (int i=0; i<TFT_WIDTH; i++)
    {
      uint16_t val = palette16[*buf++];
      pos--;      
      if (pos == 0) {
#ifdef LINEARINT_HACK        
        val  = ((uint32_t)palette16[*buf++] + val)/2;
#else
        uint16_t val2 = *buf++;
        val = RGBVAL16((R16(val)+R16(val2))/2,(G16(val)+G16(val2))/2,(B16(val)+B16(val2))/2);
#endif        
        pos = delta;
      }
      *dst++=val;
    }
#else
    int step = ((width << 8)/TFT_WIDTH);
    int pos = 0;
    for (int i=0; i<TFT_WIDTH; i++)
    {
      *dst++=palette16[buf[pos >> 8]];
      pos +=step;
    }  
#endif       
  }
  else {
    for (int i=0; i<width; i++)
    {
      *dst++=palette16[*buf++];
    }      
  }
}

void TFT_T_DMA::writeLine(int width, int height, int y, uint16_t *buf) {
  uint16_t * block=blocks[y>>6];
  uint16_t * dst=&block[(y&0x3F)*TFT_WIDTH];  
  if (width > TFT_WIDTH) {
#ifdef TFT_LINEARINT   
    int delta = (width/(width-TFT_WIDTH))-1;   
    int pos = delta;
    for (int i=0; i<TFT_WIDTH; i++)
    {
      uint16_t val = *buf++;
      pos--;      
      if (pos == 0) {
#ifdef LINEARINT_HACK        
        val  = ((uint32_t)*buf++ + val)/2;
#else
        uint16_t val2 = *buf++;
        val = RGBVAL16((R16(val)+R16(val2))/2,(G16(val)+G16(val2))/2,(B16(val)+B16(val2))/2);
#endif        
        pos = delta;
      }
      *dst++=val;
    }
#else
    int step = ((width << 8)/TFT_WIDTH);
    int pos = 0;
    for (int i=0; i<TFT_WIDTH; i++)
    {
      *dst++=buf[pos >> 8];
      pos +=step;
    }  
#endif       
  }
  else {
     for (int i=0; i<width; i++)
    {
      *dst++=*buf++;
    }       
  }
}

void TFT_T_DMA::fillScreen(uint16_t color) {
  int i,j;
  for (j=0; j<TFT_HEIGHT; j++)
  {
    uint16_t * block=blocks[j>>6];
    uint16_t * dst=&block[(j&0x3F)*TFT_WIDTH];
    for (i=0; i<TFT_WIDTH; i++)
    {
      *dst++ = color;
    }
  }
}

void TFT_T_DMA::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  int i,j,l=y;
  color=color;
  for (j=0; j<h; j++)
  {
    uint16_t * block=blocks[l>>6];
    uint16_t * dst=&block[(l&0x3F)*TFT_WIDTH+x];
    for (i=0; i<w; i++)
    {
      *dst++ = color;
    }
    l++;
  }
}

void TFT_T_DMA::drawText(int16_t x, int16_t y, const char * text, uint16_t fgcolor, uint16_t bgcolor, bool doublesize) {
  uint16_t c;
  uint16_t * block;
  uint16_t * dst;
  fgcolor = fgcolor;
  bgcolor = bgcolor;
  
  while ((c = *text++)) {
    const unsigned char * charpt=&font8x8[c][0];

    int l=y;
    for (int i=0;i<8;i++)
    {     
      unsigned char bits;
      if (doublesize) {
        block=blocks[l>>6];
        dst=&block[(l&0x3F)*TFT_WIDTH+x];         
        bits = *charpt;     
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        l++;       
      }
      block=blocks[l>>6];
      dst=&block[(l&0x3F)*TFT_WIDTH+x]; 
      bits = *charpt++;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      l++;
    }
    x +=8;
  } 
}

void TFT_T_DMA::drawSprite(int16_t x, int16_t y, const uint16_t *bitmap) {
    drawSprite(x,y,bitmap, 0,0,0,0);
}

void TFT_T_DMA::drawSprite(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t arx, uint16_t ary, uint16_t arw, uint16_t arh)
{
  int bmp_offx = 0;
  int bmp_offy = 0;
  uint16_t *bmp_ptr;
    
  int w =*bitmap++;
  int h = *bitmap++;


  if ( (arw == 0) || (arh == 0) ) {
    // no crop window
    arx = x;
    ary = y;
    arw = w;
    arh = h;
  }
  else {
    if ( (x>(arx+arw)) || ((x+w)<arx) || (y>(ary+arh)) || ((y+h)<ary)   ) {
      return;
    }
    
    // crop area
    if ( (x > arx) && (x<(arx+arw)) ) { 
      arw = arw - (x-arx);
      arx = arx + (x-arx);
    } else {
      bmp_offx = arx;
    }
    if ( ((x+w) > arx) && ((x+w)<(arx+arw)) ) {
      arw -= (arx+arw-x-w);
    }  
    if ( (y > ary) && (y<(ary+arh)) ) {
      arh = arh - (y-ary);
      ary = ary + (y-ary);
    } else {
      bmp_offy = ary;
    }
    if ( ((y+h) > ary) && ((y+h)<(ary+arh)) ) {
      arh -= (ary+arh-y-h);
    }     
  }

   
  int l=ary;
  bitmap = bitmap + bmp_offy*w + bmp_offx;
  for (int row=0;row<arh; row++)
  {
    uint16_t * block=blocks[l>>6];
    uint16_t * dst=&block[(l&0x3F)*TFT_WIDTH+arx];  
    bmp_ptr = (uint16_t*)bitmap;
    for (int col=0;col<arw; col++)
    {
        *dst++ = *bmp_ptr++;            
    } 
    bitmap +=  w;
    l++;
  } 
}




