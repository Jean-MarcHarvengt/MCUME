/*
  TFT/VGA driver
  DMA TFT driver based on C64 ILI9341 dma driver from Frank Bösing, 2017
*/

#include "T4_DSP.h"

#include <SPI.h>
#include <DMAChannel.h>

#include "font8x8.h"

// TFT constants and variables
#define TFT_LINEARINT  1
#define LINEARINT_HACK 1
#define DMA_LINES_PER_BLOCK 64
#define DMA_NUM_SETTINGS    4

#define TFT_SWRESET    0x01
#define TFT_SLPOUT     0x11
#define TFT_INVON      0x21
#define TFT_DISPOFF    0x28
#define TFT_DISPON     0x29
#define TFT_CASET      0x2A
#define TFT_PASET      0x2B
#define TFT_RAMWR      0x2C
#define TFT_MADCTL     0x36
#define TFT_PIXFMT     0x3A
#define TFT_MADCTL_MY  0x80
#define TFT_MADCTL_MX  0x40
#define TFT_MADCTL_MV  0x20
#define TFT_MADCTL_ML  0x10
#define TFT_MADCTL_RGB 0x00
#define TFT_MADCTL_BGR 0x08
#define TFT_MADCTL_MH  0x04

#define RGBVAL16(r,g,b)  ( (((r>>3)&0x1f)<<11) | (((g>>2)&0x3f)<<5) | (((b>>3)&0x1f)<<0) )
//#define RGBVAL32(r,g,b)  ( (r<<16) | (g<<8) | b )
//#define RGBVAL8(r,g,b)   ( (((r>>5)&0x07)<<5) | (((g>>5)&0x07)<<2) | (((b>>6)&0x3)<<0) )
//#define R16(rgb) ((rgb>>8)&0xf8) 
//#define G16(rgb) ((rgb>>3)&0xfc) 
//#define B16(rgb) ((rgb<<3)&0xf8) 

// LPSPI4 = SPI0 in Teensy 4.0
// LPSPI3 = SPI1 in Teensy 4.0
// LPSPI1 = SPI2 in Teensy 4.0 (used for SD on T4.0 but not T4.1) 

#ifdef TFTSPI1
#define SPI SPI1
#define LPSPIP_TDR LPSPI3_TDR
#define LPSPIP_CR LPSPI3_CR
#define LPSPIP_CFGR1 LPSPI3_CFGR1
#define LPSPIP_TCR LPSPI3_TCR
#define LPSPIP_DER LPSPI3_DER
#define DMAMUX_SOURCE_LPSPIP_TX DMAMUX_SOURCE_LPSPI3_TX
#else
#define LPSPIP_TDR LPSPI4_TDR
#define LPSPIP_CR LPSPI4_CR
#define LPSPIP_CFGR1 LPSPI4_CFGR1
#define LPSPIP_TCR LPSPI4_TCR
#define LPSPIP_DER LPSPI4_DER
#define DMAMUX_SOURCE_LPSPIP_TX DMAMUX_SOURCE_LPSPI4_TX
#endif

#define SPICLOCK 60000000
#define SPI_MODE SPI_MODE0

// VGA constants and macros
typedef uint8_t vga_pixel;
#define VGA_RGB(r,g,b)   ( (((r>>5)&0x07)<<5) | (((g>>5)&0x07)<<2) | (((b>>6)&0x3)<<0) )

static DMASetting dmasettings[DMA_NUM_SETTINGS];
static DMAChannel dmatx;
static uint16_t * blocks[DMA_NUM_SETTINGS]; // for DMA transfer, you need to divide in blocks < 128K
static volatile uint8_t rstop = 0;
static volatile bool cancelled = false;
static volatile uint8_t curTransfer = 0;
static uint8_t nbTransfer = 0;
static uint16_t * tft_buffer;
static int  tft_width;
static int  tft_height;
static int  tft_stride;

#define DELAY_MASK     0x80
PROGMEM static const uint8_t init_commands[] = { 
  1+DELAY_MASK, TFT_SWRESET,  150,
  1+DELAY_MASK, TFT_SLPOUT,   255,
  2+DELAY_MASK, TFT_PIXFMT, 0x55, 10,
  2,            TFT_MADCTL, TFT_MADCTL_MV | TFT_MADCTL_BGR,
  1, TFT_INVON,
  1, TFT_DISPON,
  0
};

// VGA constants and variables
// Objective:
// generates VGA signal fully in hardware with as little as possible CPU help

// Principle:
// QTimer3 (timer3) used to generate H-PUSE and line interrupt (and V-PULSE)
// 2 FlexIO shift registers (1 and 2) and 2 DMA channels used to generate
// RGB out, combined to create 8bits(/12bits) output.

// Note:
// - supported resolutions: 320x240,320x480,640x240 and 640x480 pixels
// - experimental resolution: 352x240,352x480
// - experimental resolution: 512x240,512x480 (not stable)
// - video memory is allocated using malloc in T4 heap
// - as the 2 DMA transfers are not started exactly at same time, there is a bit of color smearing 
//   but tried to be compensated by pixel shifting 
// - Default is 8bits RRRGGGBB (332) 
//   But 12bits GBB0RRRRGGGBB (444) feasible BUT NOT TESTED !!!!
// - Only ok at 600MHz else some disturbances visible



#define TOP_BORDER    40
#define PIN_HBLANK    15

#define PIN_R_B2      33
#define PIN_R_B1      4
#define PIN_R_B0      3
#define PIN_G_B2      2
#define PIN_G_B1      13
#define PIN_G_B0      11
#define PIN_B_B1      12
#define PIN_B_B0      10


#define DMA_HACK      0x80

#define R16(rgb) ((rgb>>8)&0xf8) 
#define G16(rgb) ((rgb>>3)&0xfc) 
#define B16(rgb) ((rgb<<3)&0xf8) 

// VGA 640x480@60Hz
// Screen refresh rate 60 Hz
// Vertical refresh    31.46875 kHz
// Pixel freq.         25.175 MHz
//
// Visible area        640  25.422045680238 us
// Front porch         16   0.63555114200596 us
// Sync pulse          96   3.8133068520357 us
// Back porch          48   1.9066534260179 us
// Whole line          800  31.777557100298 us

#define frame_freq     60.0     // Hz
#define line_freq      31.46875 // KHz
#define pix_freq       (line_freq*800) // KHz (25.175 MHz)

// pix_period = 39.7ns
// H-PULSE is 3.8133us = 3813.3ns => 96 pixels (see above for the rest)
#define frontporch_pix  20 //16
#define backporch_pix   44 //48

// Flexio Clock
// PLL3 SW CLOCK    (3) => 480 MHz
// PLL5 VIDEO CLOCK (2) => See formula for clock (we take 604200 KHz as /24 it gives 25175)
#define FLEXIO_CLK_SEL_PLL3 3
#define FLEXIO_CLK_SEL_PLL5 2

/* Set video PLL */
// There are /1, /2, /4, /8, /16 post dividers for the Video PLL. 
// The output frequency can be set by programming the fields in the CCM_ANALOG_PLL_VIDEO, 
// and CCM_ANALOG_MISC2 register sets according to the following equation.
// PLL output frequency = Fref * (DIV_SELECT + NUM/DENOM)

// nfact: 
// This field controls the PLL loop divider. 
// Valid range for DIV_SELECT divider value: 27~54.

#define POST_DIV_SELECT 2

// Full buffer including back/front porch 
static vga_pixel * gfxbuffer __attribute__((aligned(32))) = NULL;
static uint32_t dstbuffer __attribute__((aligned(32)));

// Visible vuffer
static vga_pixel * vga_buffer;
static int  vga_width;
static int  vga_height;
static int  vga_stride;

static int  maxpixperline;
static int  left_border;
static int  right_border;
static int  line_double;
static int  pix_shift;
static int  ref_div_select;
static int  ref_freq_num;
static int  ref_freq_denom;
static int  ref_pix_shift;
static int  combine_shiftreg;

#ifdef DEBUG
static uint32_t   ISRTicks_prev = 0;
volatile uint32_t ISRTicks = 0;
#endif 

uint8_t    T4_DSP::_rst;
uint8_t    T4_DSP::_cs;
uint8_t    T4_DSP::_dc;
uint8_t    T4_DSP::_mosi;
uint8_t    T4_DSP::_sclk;
uint8_t    T4_DSP::_vsync_pin = -1;
DMAChannel T4_DSP::flexio1DMA;
DMAChannel T4_DSP::flexio2DMA; 

static volatile uint32_t VSYNC = 0;
static volatile uint32_t currentLine=0;
#define NOP asm volatile("nop\n\t");
static gfx_mode_t gfxmode = MODE_UNDEFINED;


FASTRUN void T4_DSP::TFT_isr(void) {
  dmatx.clearInterrupt();
  curTransfer++;
  if (curTransfer >= nbTransfer) {
    curTransfer = 0;
    if (cancelled) {
        dmatx.disable();
        rstop = 1;
    }
  }
  arm_dcache_flush(blocks[curTransfer], DMA_LINES_PER_BLOCK*TFT_WIDTH*2);  
}

FASTRUN void T4_DSP::QT3_isr(void) {
  TMR3_SCTRL3 &= ~(TMR_SCTRL_TCF);
  TMR3_CSCTRL3 &= ~(TMR_CSCTRL_TCF1|TMR_CSCTRL_TCF2);

  cli();
  
  // V-PULSE
  if (currentLine > 0) {
    digitalWrite(_vsync_pin, 1);
    VSYNC = 0;
  } else {
    digitalWrite(_vsync_pin, 0);
    VSYNC = 1;
  }
  
  currentLine++;
  currentLine = currentLine % 525;


  uint32_t y = (currentLine - TOP_BORDER) >> line_double;
  // Visible area  

  if (y >= 0 && y < vga_height) {  
    // Disable DMAs
    //DMA_CERQ = flexio2DMA.channel;
    //DMA_CERQ = flexio1DMA.channel; 

    // Setup source adress
    // Aligned 32 bits copy
    unsigned long * p=(uint32_t *)&gfxbuffer[vga_stride*y];  
    flexio2DMA.TCD->SADDR = p;
    if (pix_shift & DMA_HACK) 
    {
      // Unaligned copy
      uint8_t * p2=(uint8_t *)&gfxbuffer[vga_stride*y+(pix_shift&0xf)];
      flexio1DMA.TCD->SADDR = p2;
    }
    else  {
      p=(uint32_t *)&gfxbuffer[vga_stride*y+(pix_shift&0xc)]; // multiple of 4
      flexio1DMA.TCD->SADDR = p;
    }

    // Enable DMAs
    //flexio2DMA.enable();
    //flexio1DMA.enable();
    DMA_SERQ = flexio2DMA.channel; 
    DMA_SERQ = flexio1DMA.channel; 
    arm_dcache_flush_delete((void*)((uint32_t *)&gfxbuffer[vga_stride*y]), vga_stride);
  }
  sei();  

#ifdef DEBUG
  ISRTicks++; 
#endif  
  asm volatile("dsb");
}

static void setDmaStruct() {
  uint32_t remaining = TFT_HEIGHT*TFT_WIDTH*2;
  uint16_t * fb = (uint16_t*)malloc(remaining);
  tft_buffer = fb;
  tft_width = TFT_WIDTH;
  tft_height = TFT_HEIGHT;
  tft_stride = TFT_WIDTH;
  uint16_t col=RGBVAL16(0x00,0x00,0x00);
  int i=0;
  while (remaining > 0) {
    int32_t len = (remaining >= (DMA_LINES_PER_BLOCK*TFT_WIDTH*2)?DMA_LINES_PER_BLOCK*TFT_WIDTH*2:remaining);      
    blocks[i] = fb;       
    for (int j=0;j<len/2;j++) fb[j]=col;
    dmasettings[i].sourceBuffer(fb, len);
    dmasettings[i].destination((uint8_t &)LPSPIP_TDR);
    dmasettings[i].TCD->ATTR_DST = 1;
    dmasettings[i].replaceSettingsOnCompletion(dmasettings[i+1]);
    dmasettings[i].interruptAtCompletion();
    fb += len/2;    
    remaining -= len;
    i++;
  }
  dmasettings[i-1].replaceSettingsOnCompletion(dmasettings[0]);
  nbTransfer = i;
}

static void set_videoClock(int nfact, int32_t nmult, uint32_t ndiv, bool force) // sets PLL5
{
//if (!force && (CCM_ANALOG_PLL_VIDEO & CCM_ANALOG_PLL_VIDEO_ENABLE)) return;
    CCM_ANALOG_PLL_VIDEO = CCM_ANALOG_PLL_VIDEO_BYPASS | CCM_ANALOG_PLL_VIDEO_ENABLE
                    | CCM_ANALOG_PLL_VIDEO_POST_DIV_SELECT(1) // 2: 1/1; 1: 1/2; 0: 1/4
                   | CCM_ANALOG_PLL_VIDEO_DIV_SELECT(nfact);
  CCM_ANALOG_PLL_VIDEO_NUM   = nmult /*& CCM_ANALOG_PLL_VIDEO_NUM_MASK*/;
  CCM_ANALOG_PLL_VIDEO_DENOM = ndiv /*& CCM_ANALOG_PLL_VIDEO_DENOM_MASK*/;    
  CCM_ANALOG_PLL_VIDEO &= ~CCM_ANALOG_PLL_VIDEO_POWERDOWN;//Switch on PLL
  while (!(CCM_ANALOG_PLL_VIDEO & CCM_ANALOG_PLL_VIDEO_LOCK)) {}; //Wait for pll-lock
    const int div_post_pll = 1; // other values: 2,4
    if(div_post_pll>3) CCM_ANALOG_MISC2 |= CCM_ANALOG_MISC2_DIV_MSB;
    CCM_ANALOG_PLL_VIDEO &= ~CCM_ANALOG_PLL_VIDEO_BYPASS;//Disable Bypass
}

T4_DSP::T4_DSP()
{
  _cs   = TFT_CS;
  _dc   = TFT_DC;
  _rst  = TFT_RST;
  _mosi = TFT_MOSI;
  _sclk = TFT_SCLK;
  pinMode(_dc, OUTPUT);
  pinMode(_cs, OUTPUT); 
  digitalWrite(_cs, 1);
  digitalWrite(_dc, 1);
  _vsync_pin = 8;
}


void T4_DSP::setArea(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2) {
  int dx=0;
  int dy=0;
  digitalWrite(_dc, 0);
  SPI.transfer(TFT_CASET);
  digitalWrite(_dc, 1);
  SPI.transfer16(x1+dx);
  digitalWrite(_dc, 1);
  SPI.transfer16(x2+dx);  
  digitalWrite(_dc, 0);
  SPI.transfer(TFT_PASET);
  digitalWrite(_dc, 1);
  SPI.transfer16(y1+dy);
  digitalWrite(_dc, 1);
  SPI.transfer16(y2+dy);  
  digitalWrite(_dc, 0);
  SPI.transfer(TFT_RAMWR);
  digitalWrite(_dc, 1);
  return;
}

void T4_DSP::tft_setup(bool isST) {
    SPI.setMOSI(_mosi);
    SPI.setSCK(_sclk);
    SPI.begin();
    // RESET if reset pin defined
    if (_rst != 0xff) {
      pinMode(_rst, OUTPUT);
      digitalWrite(_rst, HIGH);
      delay(100);
      digitalWrite(_rst, LOW);
      delay(100);
      digitalWrite(_rst, HIGH);
      delay(200);
    }
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
    const uint8_t *addr = init_commands;
    uint8_t count;
    digitalWrite(_cs, 0);
    while (count = *addr++) {
      uint8_t command = *addr++;
      if ( (command == TFT_INVON) && (!isST) ) {
        // Skip TFT_INVON for ILI
      }
      else {
        digitalWrite(_dc, 0); // command
        SPI.transfer(command);
        uint16_t ms = count & DELAY_MASK;
        count &= ~DELAY_MASK;
        while (--count > 0) { // data
          uint8_t data = *addr++;
          if ( (command == TFT_MADCTL) && (isST) ) {
            data = TFT_MADCTL_MX | TFT_MADCTL_MV |TFT_MADCTL_RGB;
          }
          digitalWrite(_dc, 1);
          SPI.transfer(data);
        }
        if (ms) {
          ms = *addr++; // Read post-command delay time (ms)
          if(ms == 255) ms = 500;   // If 255, delay for 500 ms
          digitalWrite(_cs, 1);
          SPI.endTransaction();
          delay(2);
          SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
          digitalWrite(_cs, 0);      
        }     
      }
    }
    digitalWrite(_cs, 1);
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
    setArea(0, 0, TFT_REALWIDTH-1, TFT_REALHEIGHT-1);  
    SPI.endTransaction();
    /*
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
    digitalWrite(_dc, 0);
    digitalWrite(_cs, 0);
    SPI.transfer(TFT_MADCTL);
    digitalWrite(_dc, 1);
      SPI.transfer(TFT_MADCTL_MV | TFT_MADCTL_BGR);
  //    SPI.transfer(TFT_MADCTL_MX | TFT_MADCTL_MV |TFT_MADCTL_RGB);
    digitalWrite(_cs, 1);  
    SPI.endTransaction();
    */ 
    cancelled = false;
}

// display VGA image
gfx_error_t T4_DSP::begin(gfx_mode_t mode)
{
  uint32_t flexio_clock_div;
  combine_shiftreg = 0;
//    int div_select = 49;
//    int num = 135;  
//    int denom = 100;  
  int div_select = 20;
  int num = 9800;
  int denom = 10000;  
  int flexio_clk_sel = FLEXIO_CLK_SEL_PLL5;   
  int flexio_freq = ( 24000*div_select + (num*24000)/denom )/POST_DIV_SELECT;
  set_videoClock(div_select,num,denom,true);

#ifdef DEBUG
    Serial.println(mode);
    Serial.println("mode");
#endif 

  switch(mode) 
  {
    case MODE_TFTILI_320x240:
#ifdef DEBUG
      Serial.println("TFTILI_320x240");
#endif    
      tft_setup(false);
      gfxmode = mode;
      break;
    case MODE_TFTST_320x240:
#ifdef DEBUG
      Serial.println("TFTST_320x240");
#endif    
      tft_setup(true);
      gfxmode = mode;
      break;
      
    case MODE_VGA_320x240:
#ifdef DEBUG
      Serial.println("VGA_320x240");
#endif       
      left_border = backporch_pix/2;
      right_border = frontporch_pix/2;
      vga_width = 320;
      vga_height = 240 ;
      vga_stride = left_border+vga_width+right_border;
      flexio_clock_div = flexio_freq/(pix_freq/2);
      line_double = 1;
      pix_shift = 2+DMA_HACK;    
      break;
      
    case MODE_VGA_320x480:
#ifdef DEBUG
      Serial.println("VGA_320x480");
#endif      
      left_border = backporch_pix/2;
      right_border = frontporch_pix/2;
      vga_width = 320;
      vga_height = 480 ;
      vga_stride = left_border+vga_width+right_border;
      flexio_clock_div = flexio_freq/(pix_freq/2); 
      line_double = 0;
      pix_shift = 2+DMA_HACK;
      break;
      
    case MODE_VGA_640x240:
#ifdef DEBUG
      Serial.println("VGA_640x240");
#endif      
      left_border = backporch_pix;
      right_border = frontporch_pix;
      vga_width = 640;
      vga_height = 240 ;
      vga_stride = left_border+vga_width+right_border;
      flexio_clock_div = flexio_freq/pix_freq;
      line_double = 1;
      pix_shift = 4;
      combine_shiftreg = 1;
      break;
      
    case MODE_VGA_640x480:
#ifdef DEBUG
      Serial.println("VGA_640x480");
#endif      
      left_border = backporch_pix;
      right_border = frontporch_pix;
      vga_width = 640;
      vga_height = 480 ;
      vga_stride = left_border+vga_width+right_border;
      flexio_clock_div = (flexio_freq/pix_freq); 
      line_double = 0;
      pix_shift = 4;
      combine_shiftreg = 1;
      break;
         
    case MODE_VGA_512x240:
#ifdef DEBUG
      Serial.println("VGA_512x240");
#endif      
      left_border = backporch_pix/1.3;
      right_border = frontporch_pix/1.3;
      vga_width = 512;
      vga_height = 240 ;
      vga_stride = left_border+vga_width+right_border;
      flexio_clock_div = flexio_freq/(pix_freq/1.3)+2; 
      line_double = 1;
      pix_shift = 0;
      break;
      
    case MODE_VGA_512x480:
#ifdef DEBUG
      Serial.println("VGA_512x480");
#endif      
      left_border = backporch_pix/1.3;
      right_border = frontporch_pix/1.3;
      vga_width = 512;
      vga_height = 480 ;
      vga_stride = left_border+vga_width+right_border;
      flexio_clock_div = flexio_freq/(pix_freq/1.3)+2; 
      line_double = 0;
      pix_shift = 0;
      break;
       
    case MODE_VGA_352x240:
#ifdef DEBUG
      Serial.println("VGA_352x240");
#endif      
      left_border = backporch_pix/1.75;
      right_border = frontporch_pix/1.75;
      vga_width = 352;
      vga_height = 240 ;
      vga_stride = left_border+vga_width+right_border;
      flexio_clock_div = flexio_freq/(pix_freq/1.75)+2; 
      line_double = 1;
      pix_shift = 2+DMA_HACK;
      break;
      
    case MODE_VGA_352x480:
#ifdef DEBUG
      Serial.println("VGA_352x480");
#endif      
      left_border = backporch_pix/1.75;
      right_border = frontporch_pix/1.75;
      vga_width = 352;
      vga_height = 480 ;
      vga_stride = left_border+vga_width+right_border;
      flexio_clock_div = flexio_freq/(pix_freq/1.75)+2; 
      line_double = 0;
      pix_shift = 2+DMA_HACK;
      break;         
  }
    
  if (mode >= MODE_VGA_320x240)
  {
    if ( (gfxmode != MODE_UNDEFINED) && (gfxmode < MODE_VGA_320x240) ) {  
      fillScreenNoDma(RGBVAL16(0x0,0x00,0x00));
      digitalWrite(_cs, 0);
      digitalWrite(_dc, 0);
      SPI.transfer(TFT_DISPOFF);
      digitalWrite(_cs, 1);
      delay(20);
      digitalWrite(_cs, 0);
      digitalWrite(_cs, 1);         
    }
    gfxmode = mode;    

    maxpixperline = vga_stride;  
    // Save param for tweek adjustment
    ref_div_select = div_select;
    ref_freq_num = num;
    ref_freq_denom = denom;
    ref_pix_shift = pix_shift;
  
#ifdef DEBUG
    Serial.println("frequency");
    Serial.println(flexio_freq);
    Serial.println("div");
    Serial.println(flexio_freq/pix_freq);
#endif
  
    pinMode(_vsync_pin, OUTPUT);
    pinMode(PIN_HBLANK, OUTPUT);
  
    /* Basic pin setup FlexIO1 */
    pinMode(PIN_G_B2, OUTPUT);  // FlexIO1:4 = 0x10
    pinMode(PIN_R_B0, OUTPUT);  // FlexIO1:5 = 0x20
    pinMode(PIN_R_B1, OUTPUT);  // FlexIO1:6 = 0x40
    pinMode(PIN_R_B2, OUTPUT);  // FlexIO1:7 = 0x80
    /* Basic pin setup FlexIO2 */
    pinMode(PIN_B_B0, OUTPUT);  // FlexIO2:0 = 0x00001
    pinMode(PIN_B_B1, OUTPUT);  // FlexIO2:1 = 0x00002
    pinMode(PIN_G_B0, OUTPUT);  // FlexIO2:2 = 0x00004
    pinMode(PIN_G_B1, OUTPUT);  // FlexIO2:3 = 0x00008
  
    /* High speed and drive strength configuration */
    *(portControlRegister(PIN_G_B2)) = 0xFF; 
    *(portControlRegister(PIN_R_B0)) = 0xFF;
    *(portControlRegister(PIN_R_B1)) = 0xFF;
    *(portControlRegister(PIN_R_B2)) = 0xFF;
    *(portControlRegister(PIN_B_B0)) = 0xFF; 
    *(portControlRegister(PIN_B_B1)) = 0xFF;
    *(portControlRegister(PIN_G_B0)) = 0xFF;
    *(portControlRegister(PIN_G_B1)) = 0xFF;
  
    /* Set clock for FlexIO1 and FlexIO2 */
    CCM_CCGR5 &= ~CCM_CCGR5_FLEXIO1(CCM_CCGR_ON);
    CCM_CDCDR = (CCM_CDCDR & ~(CCM_CDCDR_FLEXIO1_CLK_SEL(3) | CCM_CDCDR_FLEXIO1_CLK_PRED(7) | CCM_CDCDR_FLEXIO1_CLK_PODF(7))) 
      | CCM_CDCDR_FLEXIO1_CLK_SEL(flexio_clk_sel) | CCM_CDCDR_FLEXIO1_CLK_PRED(0) | CCM_CDCDR_FLEXIO1_CLK_PODF(0);
    CCM_CCGR3 &= ~CCM_CCGR3_FLEXIO2(CCM_CCGR_ON);
    CCM_CSCMR2 = (CCM_CSCMR2 & ~(CCM_CSCMR2_FLEXIO2_CLK_SEL(3))) | CCM_CSCMR2_FLEXIO2_CLK_SEL(flexio_clk_sel);
    CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_FLEXIO2_CLK_PRED(7)|CCM_CS1CDR_FLEXIO2_CLK_PODF(7)) )
      | CCM_CS1CDR_FLEXIO2_CLK_PRED(0) | CCM_CS1CDR_FLEXIO2_CLK_PODF(0);
  
  
   /* Set up pin mux FlexIO1 */
    *(portConfigRegister(PIN_G_B2)) = 0x14;
    *(portConfigRegister(PIN_R_B0)) = 0x14;
    *(portConfigRegister(PIN_R_B1)) = 0x14;
    *(portConfigRegister(PIN_R_B2)) = 0x14;
    /* Set up pin mux FlexIO2 */
    *(portConfigRegister(PIN_B_B0)) = 0x14;
    *(portConfigRegister(PIN_B_B1)) = 0x14;
    *(portConfigRegister(PIN_G_B0)) = 0x14;
    *(portConfigRegister(PIN_G_B1)) = 0x14;
  
    /* Enable the clock */
    CCM_CCGR5 |= CCM_CCGR5_FLEXIO1(CCM_CCGR_ON);
    CCM_CCGR3 |= CCM_CCGR3_FLEXIO2(CCM_CCGR_ON);
    /* Enable the FlexIO with fast access */
    FLEXIO1_CTRL = FLEXIO_CTRL_FLEXEN | FLEXIO_CTRL_FASTACC;
    FLEXIO2_CTRL = FLEXIO_CTRL_FLEXEN | FLEXIO_CTRL_FASTACC;
  
    uint32_t timerSelect, timerPolarity, pinConfig, pinSelect, pinPolarity, shifterMode, parallelWidth, inputSource, stopBit, startBit;
    uint32_t triggerSelect, triggerPolarity, triggerSource, timerMode, timerOutput, timerDecrement, timerReset, timerDisable, timerEnable;
  
    /* Shifter 0 registers for FlexIO2 */ 
    parallelWidth = FLEXIO_SHIFTCFG_PWIDTH(4);  // 8-bit parallel shift width
    pinSelect = FLEXIO_SHIFTCTL_PINSEL(0);      // Select pins FXIO_D0 through FXIO_D3
    inputSource = FLEXIO_SHIFTCFG_INSRC*(1);    // Input source from next shifter
    stopBit = FLEXIO_SHIFTCFG_SSTOP(0);         // Stop bit disabled
    startBit = FLEXIO_SHIFTCFG_SSTART(0);       // Start bit disabled, transmitter loads data on enable 
    timerSelect = FLEXIO_SHIFTCTL_TIMSEL(0);    // Use timer 0
    timerPolarity = FLEXIO_SHIFTCTL_TIMPOL*(1); // Shift on negedge of clock 
    pinConfig = FLEXIO_SHIFTCTL_PINCFG(3);      // Shifter pin output
    pinPolarity = FLEXIO_SHIFTCTL_PINPOL*(0);   // Shifter pin active high polarity
    shifterMode = FLEXIO_SHIFTCTL_SMOD(2);      // Shifter transmit mode
    /* Shifter 0 registers for FlexIO1 */
    FLEXIO2_SHIFTCFG0 = parallelWidth | inputSource | stopBit | startBit;
    FLEXIO2_SHIFTCTL0 = timerSelect | timerPolarity | pinConfig | pinSelect | pinPolarity | shifterMode;
  
    /* Shifter 0 registers for FlexIO1 */ 
    parallelWidth = FLEXIO_SHIFTCFG_PWIDTH(4);  // 8-bit parallel shift width
    pinSelect = FLEXIO_SHIFTCTL_PINSEL(4);      // Select pins FXIO_D4 through FXIO_D7 
    FLEXIO1_SHIFTCFG0 = parallelWidth | inputSource | stopBit | startBit;
    FLEXIO1_SHIFTCTL0 = timerSelect | timerPolarity | pinConfig | pinSelect | pinPolarity | shifterMode;
    
    if (combine_shiftreg) {
      pinConfig = FLEXIO_SHIFTCTL_PINCFG(0);    // Shifter pin output disabled
      FLEXIO2_SHIFTCFG1 = parallelWidth | inputSource | stopBit | startBit;
      FLEXIO2_SHIFTCTL1 = timerSelect | timerPolarity | pinConfig | shifterMode;
      FLEXIO1_SHIFTCFG1 = parallelWidth | inputSource | stopBit | startBit;
      FLEXIO1_SHIFTCTL1 = timerSelect | timerPolarity | pinConfig | shifterMode;
    }
    /* Timer 0 registers for FlexIO2 */ 
    timerOutput = FLEXIO_TIMCFG_TIMOUT(1);      // Timer output is logic zero when enabled and is not affected by the Timer reset
    timerDecrement = FLEXIO_TIMCFG_TIMDEC(0);   // Timer decrements on FlexIO clock, shift clock equals timer output
    timerReset = FLEXIO_TIMCFG_TIMRST(0);       // Timer never reset
    timerDisable = FLEXIO_TIMCFG_TIMDIS(2);     // Timer disabled on Timer compare
    timerEnable = FLEXIO_TIMCFG_TIMENA(2);      // Timer enabled on Trigger assert
    stopBit = FLEXIO_TIMCFG_TSTOP(0);           // Stop bit disabled
    startBit = FLEXIO_TIMCFG_TSTART*(0);        // Start bit disabled
    if (combine_shiftreg) {
      triggerSelect = FLEXIO_TIMCTL_TRGSEL(1+4*(1)); // Trigger select Shifter 1 status flag
    }
    else {
      triggerSelect = FLEXIO_TIMCTL_TRGSEL(1+4*(0)); // Trigger select Shifter 0 status flag
    }    
    triggerPolarity = FLEXIO_TIMCTL_TRGPOL*(1); // Trigger active low
    triggerSource = FLEXIO_TIMCTL_TRGSRC*(1);   // Internal trigger selected
    pinConfig = FLEXIO_TIMCTL_PINCFG(0);        // Timer pin output disabled
    //pinSelect = FLEXIO_TIMCTL_PINSEL(0);        // Select pin FXIO_D0
    //pinPolarity = FLEXIO_TIMCTL_PINPOL*(0);     // Timer pin polarity active high
    timerMode = FLEXIO_TIMCTL_TIMOD(1);         // Dual 8-bit counters baud mode
    // flexio_clock_div : Output clock frequency is N times slower than FlexIO clock (41.7 ns period) (23.980MHz?)
  
    int shifts_per_transfer;
    if (combine_shiftreg) {
      shifts_per_transfer = 8; // Shift out 8 times with every transfer = 64-bit word = contents of Shifter 0+1
    }
    else {
      shifts_per_transfer = 4; // Shift out 4 times with every transfer = 32-bit word = contents of Shifter 0 
    }
    FLEXIO2_TIMCFG0 = timerOutput | timerDecrement | timerReset | timerDisable | timerEnable | stopBit | startBit;
    FLEXIO2_TIMCTL0 = triggerSelect | triggerPolarity | triggerSource | pinConfig /*| pinSelect | pinPolarity*/ | timerMode;
    FLEXIO2_TIMCMP0 = ((shifts_per_transfer*2-1)<<8) | ((flexio_clock_div/2-1)<<0);
    /* Timer 0 registers for FlexIO1 */
    FLEXIO1_TIMCFG0 = timerOutput | timerDecrement | timerReset | timerDisable | timerEnable | stopBit | startBit;
    FLEXIO1_TIMCTL0 = triggerSelect | triggerPolarity | triggerSource | pinConfig /*| pinSelect | pinPolarity*/ | timerMode;
    FLEXIO1_TIMCMP0 = ((shifts_per_transfer*2-1)<<8) | ((flexio_clock_div/2-1)<<0);
#ifdef DEBUG
    Serial.println("FlexIO setup complete");
#endif
  
    /* Enable DMA trigger on Shifter0, DMA request is generated when data is transferred from buffer0 to shifter0 */ 
    if (combine_shiftreg) {
      FLEXIO2_SHIFTSDEN |= (1<<1); 
      FLEXIO1_SHIFTSDEN |= (1<<1);
    }
    else {
      FLEXIO2_SHIFTSDEN |= (1<<0); 
      FLEXIO1_SHIFTSDEN |= (1<<0);
    }
    /* Disable DMA channel so it doesn't start transferring yet */
    flexio1DMA.disable();
    flexio2DMA.disable();
    /* Set up DMA channel to use Shifter 0 trigger */
    flexio1DMA.triggerAtHardwareEvent(DMAMUX_SOURCE_FLEXIO1_REQUEST0);
    flexio2DMA.triggerAtHardwareEvent(DMAMUX_SOURCE_FLEXIO2_REQUEST0);
  
  
    if (combine_shiftreg) {
      flexio2DMA.TCD->NBYTES = 8;
      flexio2DMA.TCD->SOFF = 4;
      flexio2DMA.TCD->SLAST = -maxpixperline;
      flexio2DMA.TCD->BITER = maxpixperline / 8;
      flexio2DMA.TCD->CITER = maxpixperline / 8;
      flexio2DMA.TCD->DADDR = &FLEXIO2_SHIFTBUF0;
      flexio2DMA.TCD->DOFF = 0;
      flexio2DMA.TCD->DLASTSGA = 0;
      flexio2DMA.TCD->ATTR = DMA_TCD_ATTR_SSIZE(2) | DMA_TCD_ATTR_DSIZE(3); // 32bits => 64bits
      flexio2DMA.TCD->CSR |= DMA_TCD_CSR_DREQ;
    
      flexio1DMA.TCD->NBYTES = 8;
      flexio1DMA.TCD->SOFF = 4;
      flexio1DMA.TCD->SLAST = -maxpixperline;
      flexio1DMA.TCD->BITER = maxpixperline / 8;
      flexio1DMA.TCD->CITER = maxpixperline / 8;
      flexio1DMA.TCD->DADDR = &FLEXIO1_SHIFTBUFNBS0;
      flexio1DMA.TCD->DOFF = 0;
      flexio1DMA.TCD->DLASTSGA = 0;
      flexio1DMA.TCD->ATTR = DMA_TCD_ATTR_SSIZE(2) | DMA_TCD_ATTR_DSIZE(3); // 32bits => 64bits
      flexio1DMA.TCD->CSR |= DMA_TCD_CSR_DREQ;     
    }
    else {
      // Setup DMA2 Flexio2 copy
      flexio2DMA.TCD->NBYTES = 4;
      flexio2DMA.TCD->SOFF = 4;
      flexio2DMA.TCD->SLAST = -maxpixperline;
      flexio2DMA.TCD->BITER = maxpixperline / 4;
      flexio2DMA.TCD->CITER = maxpixperline / 4;
      flexio2DMA.TCD->DADDR = &FLEXIO2_SHIFTBUF0;
      flexio2DMA.TCD->DOFF = 0;
      flexio2DMA.TCD->DLASTSGA = 0;
      flexio2DMA.TCD->ATTR = DMA_TCD_ATTR_SSIZE(2) | DMA_TCD_ATTR_DSIZE(2); // 32bits
      flexio2DMA.TCD->CSR |= DMA_TCD_CSR_DREQ;
  
      // Setup DMA1 Flexio1 copy
      // Use pixel shift to avoid color smearing?
      if (pix_shift & DMA_HACK)
      {
        if (pix_shift & 0x3 == 0) {
          // Aligned 32 bits copy (32bits to 32bits)
          flexio1DMA.TCD->NBYTES = 4;
          flexio1DMA.TCD->SOFF = 4;
          flexio1DMA.TCD->SLAST = -maxpixperline;
          flexio1DMA.TCD->BITER = maxpixperline / 4;
          flexio1DMA.TCD->CITER = maxpixperline / 4;
          flexio1DMA.TCD->DADDR = &FLEXIO1_SHIFTBUFNBS0;
          flexio1DMA.TCD->DOFF = 0;
          flexio1DMA.TCD->DLASTSGA = 0;
          flexio1DMA.TCD->ATTR = DMA_TCD_ATTR_SSIZE(2) | DMA_TCD_ATTR_DSIZE(2); // 32bits to 32bits
          flexio1DMA.TCD->CSR |= DMA_TCD_CSR_DREQ;
        }
        else {
          // Unaligned (source) 32 bits copy (8bits to 32bits)
          flexio1DMA.TCD->NBYTES = 4;
          flexio1DMA.TCD->SOFF = 1;
          flexio1DMA.TCD->SLAST = -maxpixperline;
          flexio1DMA.TCD->BITER = maxpixperline / 4;
          flexio1DMA.TCD->CITER = maxpixperline / 4;
          flexio1DMA.TCD->DADDR = &FLEXIO1_SHIFTBUFNBS0;
          flexio1DMA.TCD->DOFF = 0;
          flexio1DMA.TCD->DLASTSGA = 0;
          flexio1DMA.TCD->ATTR = DMA_TCD_ATTR_SSIZE(0) | DMA_TCD_ATTR_DSIZE(2); // 8bits to 32bits
          flexio1DMA.TCD->CSR |= DMA_TCD_CSR_DREQ; // disable on completion
        }
      }
      else 
      {
          // Aligned 32 bits copy
          flexio1DMA.TCD->NBYTES = 4;
          flexio1DMA.TCD->SOFF = 4;
          flexio1DMA.TCD->SLAST = -maxpixperline;
          flexio1DMA.TCD->BITER = maxpixperline / 4;
          flexio1DMA.TCD->CITER = maxpixperline / 4;
          flexio1DMA.TCD->DADDR = &FLEXIO1_SHIFTBUFNBS0;
          flexio1DMA.TCD->DOFF = 0;
          flexio1DMA.TCD->DLASTSGA = 0;
          flexio1DMA.TCD->ATTR = DMA_TCD_ATTR_SSIZE(2) | DMA_TCD_ATTR_DSIZE(2); // 32bits
          flexio1DMA.TCD->CSR |= DMA_TCD_CSR_DREQ;
      }    
    }
  
#ifdef DEBUG
    Serial.println("DMA setup complete");
#endif
  
    // enable clocks for QTIMER3: generates the 15KHz for hsync
    // Pulse:
    // low  : 3.8133 us => 569x6.7ns
    // total: 31.777 us => 4743x6.7ns (high = 4174x6.7ns)
    // (OLD TEST)
    // (4us low, 28us high => 32us)
    // (597x6.7ns for 4us)
    // (4179x6.7ns for 28us) 
    CCM_CCGR6 |= 0xC0000000;              //enable clocks to CG15 of CGR6 for QT3
    //configure QTIMER3 Timer3 for test of alternating Compare1 and Compare2
    
    #define MARGIN_N 1005 // 1206 at 720MHz //1005 at 600MHz
    #define MARGIN_D 1000
  
    TMR3_CTRL3 = 0b0000000000100000;      //stop all functions of timer 
    // Invert output pin as we want the interupt on rising edge
    TMR3_SCTRL3 = 0b0000000000000011;     //0(TimerCompareFlag),0(TimerCompareIntEnable),00(TimerOverflow)0000(NoCapture),0000(Capture Disabled),00, 1(INV output),1(OFLAG to Ext Pin)
    TMR3_CNTR3 = 0;
    TMR3_LOAD3 = 0;
  
    /* Inverted timings */
    unsigned long long cpu_freq = F_CPU;
    unsigned long long rate = (1005ULL * cpu_freq) / 600000000ULL;
    int rate2 = rate;
    
    int substract = 1;
    if (cpu_freq > 950000000ULL && cpu_freq < 990000000ULL) {
      substract=5;
      rate2=1652;
    }
    
 #ifdef DEBUG
    Serial.print("MARGIN_N is: ");
    Serial.println(rate2, DEC);
    Serial.print("SUBSTRACT is: ");
    Serial.println(substract, DEC);
#endif  
    TMR3_COMP13 = ((4174*rate2)/MARGIN_D)-substract;
    TMR3_CMPLD13 = ((4174*rate2)/MARGIN_D)-substract;
    TMR3_COMP23 = ((569*rate2)/MARGIN_D)-substract;
    TMR3_CMPLD23 = ((569*rate2)/MARGIN_D)-substract;
    /*
    TMR3_COMP13 = ((4174*MARGIN_N)/MARGIN_D)-1;
    TMR3_CMPLD13 = ((4174*MARGIN_N)/MARGIN_D)-1;
    TMR3_COMP23 = ((569*MARGIN_N)/MARGIN_D)-1;
    TMR3_CMPLD23 = ((569*MARGIN_N)/MARGIN_D)-1;
    */
    TMR3_CSCTRL3 = 0b0000000010000101;    //Compare1 only enabled - Compare Load1 control and Compare Load2 control both on
    TMR3_CTRL3 = 0b0011000000100100;      // 001(Count rising edges Primary Source),1000(IP Bus Clock),00 (Secondary Source), 
                                          // 0(Count Once),1(Count up to Compare),0(Count Up),0(Co Channel Init),100(Toggle OFLAG on alternating Compare1/Compare2)
    //configure Teensy pin Compare output
    IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_03 = 1;      // QT3 Timer3 is now on pin 15
    attachInterruptVector(IRQ_QTIMER3, QT3_isr);  //declare which routine performs the ISR function
    NVIC_ENABLE_IRQ(IRQ_QTIMER3);  
#ifdef DEBUG
    Serial.println("QTIMER3 setup complete");
    Serial.print("V-PIN is ");
    Serial.println(_vsync_pin);
#endif
  
    /* initialize gfx buffer */
    if (gfxbuffer == NULL) gfxbuffer = (vga_pixel*)malloc(vga_stride*vga_height*sizeof(vga_pixel)+4); // 4bytes for pixel shift 
    if (gfxbuffer == NULL) return(GFX_ERROR);
#ifdef DEBUG
    Serial.println("Memory allocated");
#endif
  
    memset((void*)&gfxbuffer[0],0, vga_stride*vga_height*sizeof(vga_pixel)+4);
    vga_buffer = (vga_pixel*)&gfxbuffer[left_border]; 
#ifdef DEBUG
    Serial.println(vga_stride);
    Serial.println(vga_height);
    Serial.println("Screen cleared");
#endif
  }

  return(GFX_OK);
}

gfx_mode_t T4_DSP::getMode(void)
{
  return gfxmode;
}

void T4_DSP::startRefresh(void) {
  if (gfxmode < MODE_VGA_320x240) {  
    curTransfer = 0;  
    rstop = 0;     
    //dmatx.begin(true);
    dmatx.attachInterrupt(TFT_isr);
    setDmaStruct();
    setArea((TFT_REALWIDTH-TFT_WIDTH)/2, (TFT_REALHEIGHT-TFT_HEIGHT)/2, (TFT_REALWIDTH-TFT_WIDTH)/2 + TFT_WIDTH-1, (TFT_REALHEIGHT-TFT_HEIGHT)/2+TFT_HEIGHT-1);  
    fillScreen(RGBVAL16(0x00,0x00,0x00));
    
    digitalWrite(_cs, HIGH);
    SPI.begin();
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
  
    LPSPIP_CR &= ~LPSPI_CR_MEN;//disable LPSPI:
    LPSPIP_CFGR1 |= LPSPI_CFGR1_NOSTALL; //prevent stall from RX
    LPSPIP_TCR = 15; // Framesize 16 Bits
    //LPSPIP_FCR = 0; // Fifo Watermark
    LPSPIP_DER = LPSPI_DER_TDDE; //TX DMA Request Enable
    LPSPIP_CR |= LPSPI_CR_MEN; //enable LPSPI:
    dmatx.triggerAtHardwareEvent( DMAMUX_SOURCE_LPSPIP_TX );
  
    dmatx = dmasettings[0];
    digitalWrite(_cs, 0); 
    setArea((TFT_REALWIDTH-TFT_WIDTH)/2, (TFT_REALHEIGHT-TFT_HEIGHT)/2, (TFT_REALWIDTH-TFT_WIDTH)/2+TFT_WIDTH-1, (TFT_REALHEIGHT-TFT_HEIGHT)/2+TFT_HEIGHT-1);  
    digitalWrite(_dc, 0);
    SPI.transfer(TFT_RAMWR); 
    digitalWrite(_dc, 1);      
    dmatx.enable();    
  }
}


void T4_DSP::stopRefresh(void) {
  if (gfxmode < MODE_VGA_320x240) {  
    rstop = 1;
    unsigned long m = millis(); 
    cancelled = true; 
    while (!rstop)  {
      if ((millis() - m) > 100) break;
      delay(10);
      asm volatile("wfi");
    };
    rstop = 0;    
    delay(50);
    cancelled = false;  
    dmatx.detachInterrupt();
    fillScreen(RGBVAL16(0x00,0x00,0x00));
    SPI.end();
#ifdef ST7789  
//    begin(gfxmode);
#endif 
    setArea(0, 0, TFT_REALWIDTH-1, TFT_REALHEIGHT-1);    
  }
}

int T4_DSP::get_frame_buffer_size(int *width, int *height) {
  if (gfxmode < MODE_VGA_320x240) {
    if (width != nullptr) *width = tft_width;
    if (height != nullptr) *height = tft_height;
    return tft_stride;
  }
  else {
    if (width != nullptr) *width = vga_width;
    if (height != nullptr) *height = vga_height;
    return vga_stride;
  }
} 

void T4_DSP::waitSync()
{
  if (gfxmode >= MODE_VGA_320x240) {  
    while (VSYNC == 0) {};    
  }
}

void T4_DSP::waitLine(int line)
{
  if (gfxmode >= MODE_VGA_320x240) {  
    while (currentLine != line) {};      
  }
}

/***********************************************************************************************
    No DMA functions
 ***********************************************************************************************/
void T4_DSP::fillScreenNoDma(uint16_t color) {
  if (gfxmode < MODE_VGA_320x240) { 
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
    digitalWrite(_cs, 0);
    setArea(0, 0, TFT_REALWIDTH-1, TFT_REALHEIGHT-1);  
    int i,j;
    for (j=0; j<TFT_REALHEIGHT; j++)
    {
      for (i=0; i<TFT_REALWIDTH; i++) {
        //digitalWrite(_dc, 1);
        SPI.transfer16(color);     
      }
    }
    digitalWrite(_cs, 1);
    setArea(0, 0, (TFT_REALWIDTH-1), (TFT_REALHEIGHT-1));  
    SPI.endTransaction();
  }
  else {
    fillScreen(color);
  }

}

void T4_DSP::drawRectNoDma(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if (gfxmode < MODE_VGA_320x240) { 
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
    digitalWrite(_cs, 0);
    setArea(x,y,x+w-1,y+h-1);
    int i;
    for (i=0; i<(w*h); i++)
    {
      SPI.transfer16(color);
    }
    digitalWrite(_cs, 1);
    setArea(0, 0, (TFT_REALWIDTH-1), (TFT_REALHEIGHT-1));
    SPI.endTransaction();
  }
  else {
    drawRect(x, y, w, h, color);
  }
}


void T4_DSP::drawSpriteNoDma(int16_t x, int16_t y, const uint16_t *bitmap) {
    drawSpriteNoDma(x,y,bitmap, 0,0,0,0);
}

void T4_DSP::drawSpriteNoDma(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t arx, uint16_t ary, uint16_t arw, uint16_t arh)
{
  if (gfxmode < MODE_VGA_320x240) { 
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
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
    digitalWrite(_cs, 0);
    setArea(arx, ary, arx+arw-1, ary+arh-1);
    bitmap = bitmap + bmp_offy*w + bmp_offx;
    for (int row=0;row<arh; row++)
    {
      bmp_ptr = (uint16_t*)bitmap;
      for (int col=0;col<arw; col++)
      {
          uint16_t color = *bmp_ptr++;
          SPI.transfer16(color);             
      } 
      bitmap +=  w;
    }
    setArea(0, 0, TFT_REALWIDTH-1, TFT_REALHEIGHT-1);  
    digitalWrite(_cs, 1);
    SPI.endTransaction();
  }
  else {
    drawSprite(x, y, bitmap, arx, ary, arw, arh);
  }  
}

void T4_DSP::drawTextNoDma(int16_t x, int16_t y, const char * text, uint16_t fgcolor, uint16_t bgcolor, bool doublesize) {
  if (gfxmode < MODE_VGA_320x240) { 
    uint16_t c;
    while ((c = *text++)) {
      const unsigned char * charpt=&font8x8[c][0];
      SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
      digitalWrite(_cs, 0);
      setArea(x,y,x+7,y+(doublesize?15:7));
      for (int i=0;i<8;i++)
      {
        unsigned char bits;
        if (doublesize) {
          bits = *charpt;     
          //digitalWrite(_dc, 1);
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
        //digitalWrite(_dc, 1);
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
      digitalWrite(_cs, 1);
      SPI.endTransaction();  
    }
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
    digitalWrite(_cs, 0);
    setArea(0, 0, (TFT_REALWIDTH-1), (TFT_REALHEIGHT-1));  
    digitalWrite(_cs, 1);
    SPI.endTransaction(); 
  }
  else {
    drawText(x, y, text, fgcolor, bgcolor, doublesize);
  } 
}


/***********************************************************************************************
    DMA functions
 ***********************************************************************************************/
void T4_DSP::fillScreen(uint16_t color) {
  int i,j;
  if (gfxmode < MODE_VGA_320x240) {  
    for (j=0; j<tft_height; j++)
    {
      uint16_t * dst=&tft_buffer[j*tft_stride];
      for (i=0; i<tft_width; i++)
      {
        *dst++ = color;
      }
    }
  }
  else {
    vga_pixel color8 = VGA_RGB(R16(color),G16(color),B16(color));
    for (j=0; j<vga_height; j++)
    {
      vga_pixel * dst=&vga_buffer[j*vga_stride];
      for (i=0; i<vga_width; i++)
      {
        *dst++ = color8;
      }
    }
  }
}

void T4_DSP::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  int i,j,l=y;
  if (gfxmode < MODE_VGA_320x240) {
    for (j=0; j<h; j++)
    {
      uint16_t * dst=&tft_buffer[l*tft_stride+x];
      for (i=0; i<w; i++)
      {
        *dst++ = color;
      }
      l++;
    }
  }
  else {
    vga_pixel color8 = VGA_RGB(R16(color),G16(color),B16(color));  
    for (j=0; j<h; j++)
    {
      vga_pixel * dst=&vga_buffer[l*vga_stride+x];
      for (i=0; i<w; i++)
      {
        *dst++ = color8;
      }
      l++;
    }
  }
}

void T4_DSP::drawText(int16_t x, int16_t y, const char * text, uint16_t fgcolor, uint16_t bgcolor, bool doublesize) {
  uint16_t c;
  if (gfxmode < MODE_VGA_320x240) {
    uint16_t * dst;
    while ((c = *text++)) {
      const unsigned char * charpt=&font8x8[c][0];
      int l=y;
      for (int i=0;i<8;i++)
      {     
        unsigned char bits;
        if (doublesize) {
          dst=&tft_buffer[l*tft_stride+x];                 
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
        dst=&tft_buffer[l*tft_stride+x];         
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
  else {
    vga_pixel fgcolor8 = VGA_RGB(R16(fgcolor),G16(fgcolor),B16(fgcolor));
    vga_pixel bgcolor8 = VGA_RGB(R16(bgcolor),G16(bgcolor),B16(bgcolor));    
    vga_pixel * dst;
    while ((c = *text++)) {
      const unsigned char * charpt=&font8x8[c][0];
      int l=y;
      for (int i=0;i<8;i++)
      {     
        unsigned char bits;
        if (doublesize) {
          dst=&vga_buffer[l*vga_stride+x];                 
          bits = *charpt;     
          if (bits&0x01) *dst++=fgcolor8;
          else *dst++=bgcolor8;
          bits = bits >> 1;     
          if (bits&0x01) *dst++=fgcolor8;
          else *dst++=bgcolor8;
          bits = bits >> 1;     
          if (bits&0x01) *dst++=fgcolor8;
          else *dst++=bgcolor8;
          bits = bits >> 1;     
          if (bits&0x01) *dst++=fgcolor8;
          else *dst++=bgcolor8;
          bits = bits >> 1;     
          if (bits&0x01) *dst++=fgcolor8;
          else *dst++=bgcolor8;
          bits = bits >> 1;     
          if (bits&0x01) *dst++=fgcolor8;
          else *dst++=bgcolor8;
          bits = bits >> 1;     
          if (bits&0x01) *dst++=fgcolor8;
          else *dst++=bgcolor8;
          bits = bits >> 1;     
          if (bits&0x01) *dst++=fgcolor8;
          else *dst++=bgcolor8;
          l++;       
        }
        dst=&vga_buffer[l*vga_stride+x];         
        bits = *charpt++;     
        if (bits&0x01) *dst++=fgcolor8;
        else *dst++=bgcolor8;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor8;
        else *dst++=bgcolor8;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor8;
        else *dst++=bgcolor8;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor8;
        else *dst++=bgcolor8;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor8;
        else *dst++=bgcolor8;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor8;
        else *dst++=bgcolor8;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor8;
        else *dst++=bgcolor8;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor8;
        else *dst++=bgcolor8;
        l++;
      }
      x +=8;
    }   
  }
}

void T4_DSP::drawSprite(int16_t x, int16_t y, const uint16_t *bitmap) {
    drawSprite(x,y,bitmap, 0,0,0,0);
}

void T4_DSP::drawSprite(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t arx, uint16_t ary, uint16_t arw, uint16_t arh)
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
   
  if (gfxmode < MODE_VGA_320x240) {
    for (int row=0;row<arh; row++)
    {
      uint16_t * dst=&tft_buffer[l*tft_stride+arx];      
      bmp_ptr = (uint16_t*)bitmap;
      for (int col=0;col<arw; col++)
      {
          *dst++ = *bmp_ptr++;            
      } 
      bitmap +=  w;
      l++;
    } 
  }
  else {  
    for (int row=0;row<arh; row++)
    {
      vga_pixel * dst=&vga_buffer[l*vga_stride+arx];      
      bmp_ptr = (uint16_t*)bitmap;
      for (int col=0;col<arw; col++)
      {
          uint16_t pix= *bmp_ptr++;
          *dst++ = VGA_RGB(R16(pix),G16(pix),B16(pix));       
      } 
      bitmap +=  w;
      l++;
    } 
  }
}

void T4_DSP::writeLine(int width, int height, int y, uint16_t *buf) {
  if (gfxmode < MODE_VGA_320x240) {
    uint16_t * dst=&tft_buffer[y*tft_stride];          
    if (width > tft_width) {
#ifdef TFT_LINEARINT   
      int delta = (width/(width-tft_width))-1;   
      int pos = delta;
      for (int i=0; i<tft_width; i++)
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
      int step = ((width << 8)/tft_width);
      int pos = 0;
      for (int i=0; i<tft_width; i++)
      {
        *dst++=buf[pos >> 8];
        pos +=step;
      }  
  #endif       
    }
    else if ((width*2) == tft_width) 
    {
      for (int i=0; i<width; i++)
      {
        *dst++=*buf;
        *dst++=*buf++;
      }       
    }
    else
    {
      if (width <= tft_width) {
        dst += (tft_width-width)/2;
      }
      for (int i=0; i<width; i++)
      {
        *dst++=*buf++;
      }       
    }    
  }
  else {
    if ( (height<vga_height) && (height > 2) ) y += (vga_height-height)/2;
    vga_pixel * dst=&vga_buffer[y*vga_stride];     
    if (width > vga_width) {
      int step = ((width << 8)/vga_width);
      int pos = 0;
      for (int i=0; i<vga_width; i++)
      {
        uint16_t pix = buf[pos >> 8];
        *dst++ = VGA_RGB(R16(pix),G16(pix),B16(pix)); 
        pos +=step;
      }        
    }
    else if ((width*2) == vga_width) {
      for (int i=0; i<width; i++)
      {
        uint16_t pix = *buf++;
        vga_pixel col = VGA_RGB(R16(pix),G16(pix),B16(pix));
        *dst++= col;
        *dst++= col;
      }       
    }
    else {
      if (width <= vga_width) {
        dst += (vga_width-width)/2;
      }
      for (int i=0; i<width; i++)
      {
        uint16_t pix = *buf++;
        *dst++= VGA_RGB(R16(pix),G16(pix),B16(pix));
      }      
    }
  }  
}

void T4_DSP::writeLinePal(int width, int height, int y, uint8_t *buf, uint16_t *palette) {
  if (gfxmode < MODE_VGA_320x240) {
    if ( (height<tft_height) && (height > 2) ) y += (tft_height-height)/2;
    uint16_t * dst=&tft_buffer[y*tft_stride];     
    if (width > tft_width) {
#ifdef TFT_LINEARINT    
      int delta = (width/(width-tft_width))-1;
      int pos = delta;
      for (int i=0; i<tft_width; i++)
      {
        uint16_t val = palette[*buf++];
        pos--;
        if (pos == 0) {
#ifdef LINEARINT_HACK
          val  = ((uint32_t)palette[*buf++] + val)/2;
#else
          uint16_t val2 = *buf++;
          val = RGBVAL16((R16(val)+R16(val2))/2,(G16(val)+G16(val2))/2,(B16(val)+B16(val2))/2);
#endif        
          pos = delta;
        }
        *dst++=val;
      }
#else
      int step = ((width << 8)/tft_width);
      int pos = 0;
      for (int i=0; i<tft_width; i++)
      {
        *dst++=palette[buf[pos >> 8]];
        pos +=step;
      }  
#endif
    }
    else if ((width*2) == tft_width) {
      for (int i=0; i<width; i++)
      {
        *dst++=palette[*buf];
        *dst++=palette[*buf++];
      } 
    }
    else {
      if (width <= tft_width) {
        dst += (tft_width-width)/2;
      }
      for (int i=0; i<width; i++)
      {
        *dst++=palette[*buf++];
      } 
    }    
  }  
  else {
    if ( (height<vga_height) && (height > 2) ) y += (vga_height-height)/2;
    vga_pixel * dst=&vga_buffer[y*vga_stride];     
    if (width > vga_width) {
      int step = ((width << 8)/vga_width);
      int pos = 0;
      for (int i=0; i<vga_width; i++)
      {
        uint16_t pix = palette[buf[pos >> 8]];
        *dst++= VGA_RGB(R16(pix),G16(pix),B16(pix));
        pos +=step;
      }  
    }
    else if ((width*2) == vga_width) {
      for (int i=0; i<width; i++)
      {
        uint16_t pix = palette[*buf++];
        *dst++= VGA_RGB(R16(pix),G16(pix),B16(pix));
        *dst++= VGA_RGB(R16(pix),G16(pix),B16(pix));
      } 
    }
    else {
      if (width <= vga_width) {
        dst += (vga_width-width)/2;
      }
      for (int i=0; i<width; i++)
      {
        uint16_t pix = palette[*buf++];
        *dst++= VGA_RGB(R16(pix),G16(pix),B16(pix));
      } 
    }
  }
}

void T4_DSP::writeScreenPal(int width, int height, int stride, uint8_t *buf, uint16_t *palette16) {
  uint8_t *src; 
  int i,j,y=0;
  int sy = 0;  
  int systep=(1<<8);
  int h = height;
  
  if (gfxmode < MODE_VGA_320x240) {
    if (height <= ( (2*tft_height)/3)) {
      systep=(systep*height)/tft_height;
      h = tft_height;
    }    
    if (width*2 <= tft_width) {
      for (j=0; j<h; j++)
      {
        uint16_t * dst=&tft_buffer[y*tft_stride];        
        src=&buf[(sy>>8)*stride];
        for (i=0; i<width; i++)
        {
          uint16_t val = palette16[*src++];
          *dst++ = val;
          *dst++ = val;
        }
        y++;
        sy+=systep;  
      }
    }
    else if (width <= tft_width) {
      for (j=0; j<h; j++)
      {
        uint16_t * dst=&tft_buffer[y*tft_stride+(tft_width-width)/2];                
        src=&buf[(sy>>8)*stride];
        for (i=0; i<width; i++)
        {
          uint16_t val = palette16[*src++];
          *dst++ = val;
        }
        y++;
        sy+=systep;  
      }
    }
  }       
  else { // VGA
    if (height <= ( (2*vga_height)/3)) {
      systep=(systep*height)/vga_height;
      h = vga_height;
    }    
    if (width*2 <= vga_width) {
      for (j=0; j<h; j++)
      {
        vga_pixel * dst=&vga_buffer[y*vga_stride];                
        src=&buf[(sy>>8)*stride];
        for (i=0; i<width; i++)
        {
          uint16_t pix = palette16[*src++];
          *dst++ = VGA_RGB(R16(pix),G16(pix),B16(pix));
          *dst++ = VGA_RGB(R16(pix),G16(pix),B16(pix));
        }
        y++;
        sy+=systep;  
      }
    }
    else if (width <= vga_width) {
      for (j=0; j<h; j++)
      {
        vga_pixel * dst=&vga_buffer[y*vga_stride+(vga_width-width)/2];                
        src=&buf[(sy>>8)*stride];
        for (i=0; i<width; i++)
        {
          uint16_t pix = palette16[*src++];
          *dst++ = VGA_RGB(R16(pix),G16(pix),B16(pix));
        }
        y++;
        sy+=systep;  
      }
    }
  }
}


void T4_DSP::writeLine8(int width, int height, int y, uint8_t *buf, uint16_t *palette) {
  if (gfxmode < MODE_VGA_320x240) {
    uint16_t * dst=&tft_buffer[y*tft_stride];          
    if (width > tft_width) {
      int step = ((width << 8)/tft_width);
      int pos = 0;
      for (int i=0; i<tft_width; i++)
      {
        *dst++=palette[buf[pos >> 8]];
        pos +=step;
      }        
    }
    else if ((width*2) == tft_width) 
    {
      for (int i=0; i<width; i++)
      {
        uint16_t val = palette[*buf++];
        *dst++=val;
        *dst++=val;
      }       
    }
    else
    {
      if (width <= tft_width) {
        dst += (tft_width-width)/2;
      }
      for (int i=0; i<width; i++)
      {
        *dst++=palette[*buf++];
      }       
    }   
  }
  else {
    if ( (height<vga_height) && (height > 2) ) y += (vga_height-height)/2;
    vga_pixel * dst=&vga_buffer[y*vga_stride];     
    if (width > vga_width) {
      int step = ((width << 8)/vga_width);
      int pos = 0;
      for (int i=0; i<vga_width; i++)
      {
        uint8_t pix = buf[pos >> 8];
        *dst++ = pix; 
        pos +=step;
      }        
    }
    else if ((width*2) == vga_width) {
      for (int i=0; i<width; i++)
      {
        vga_pixel col = *buf++;
        *dst++= col;
        *dst++= col;
      }       
    }
    else {
      if (width <= vga_width) {
        dst += (vga_width-width)/2;
      }
      for (int i=0; i<width; i++)
      {
        *dst++= *buf++;
      }      
    }
  }  
}
