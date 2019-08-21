/*
  ILI9341 SPI driver inspired from the Teensy version of Frank Bösing, 2017
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"

#include <string.h>

#include "ili9341_t3dma.h"
#include "font8x8.h"



static spi_device_handle_t lcdspi;
static spi_transaction_t trans[MAX_SPI_TRANS];

static uint16_t *blocks[NR_OF_BLOCK];
static uint8_t _rst, _cs, _dc;
static uint8_t _miso, _mosi, _clk;
static uint8_t _touch_irq, _touch_cs;  

//DRAM_ATTR static uint16_t block0[320*LINES_PER_BLOCK];
//DRAM_ATTR static uint16_t block1[320*LINES_PER_BLOCK];

static const lcd_init_cmd_t ili_init_cmds[]={
  {0xEF, {0x03, 0x80, 0x02}, 3},
  {0xCF, {0x00, 0XC1, 0X30}, 3},
  {0xED, {0x64, 0x03, 0X12, 0X81},  4},
  {0xE8, {0x85, 0x00, 0x78}, 3},
  {0xCB, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5},
  {0xF7, {0x20}, 1},
  {0xEA, {0x00, 0x00}, 2},
  {ILI9341_PWCTR1, {0x23}, 1}, // Power control
  {ILI9341_PWCTR2, {0x10}, 1}, // Power control
  {ILI9341_VMCTR1, {0x3e, 0x28}, 2}, // VCM control
  {ILI9341_VMCTR2, {0x86}, 1}, // VCM control2
  {ILI9341_MADCTL, {0x48}, 1}, // Memory Access Control
  {ILI9341_PIXFMT, {0x55}, 1},
  {ILI9341_FRMCTR1, {0x00, 0x18}, 2},
  {ILI9341_DFUNCTR, {0x08, 0x82, 0x27}, 3}, // Display Function Control
  {0xF2, {0x00}, 1}, // Gamma Function Disable
  {ILI9341_GAMMASET, {0x01}, 1}, // Gamma curve selected
  {ILI9341_GMCTRP1, {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08,
  0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00}, 15}, // Set Gamma
  {ILI9341_GMCTRN1, {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07,
  0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F}, 15}, // Set Gamma
//  3, 0xb1, 0x00, 0x1f, // FrameRate Control 61Hz
  {0xb1, {0x00, 0x10}, 2}, // FrameRate Control 119Hz
  {ILI9341_MADCTL, {MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR}, 1},

  /* Sleep out */
  {ILI9341_SLPOUT, {0}, 0x80},
  /* Display on */
  {ILI9341_DISPON, {0}, 0x80},
  

  // Area width, hight
  {ILI9341_CASET, {0, 0, (ILI9341_TFTREALWIDTH)>>8, (ILI9341_TFTREALWIDTH)&0xff}, 4},
  {ILI9341_PASET, {0, 0, (ILI9341_TFTREALHEIGHT)>>8, (ILI9341_TFTREALHEIGHT)&0xff}, 4},
  
  {0, {0}, 0xff},
};

static void lcd_cmd(spi_device_handle_t spi, const uint8_t cmd)
{
  esp_err_t ret;
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));       //Zero out the transaction
  t.length=8;                     //Command is 8 bits
  t.tx_buffer=&cmd;               //The data is the cmd itself
  t.user=(void*)0;                //D/C needs to be set to 0
  t.flags=0;    
  ret=spi_device_polling_transmit(spi, &t);  //Transmit!
  assert(ret==ESP_OK);            //Should have had no issues.
}


static void lcd_data(spi_device_handle_t spi, const uint8_t *data, int len)
{
  esp_err_t ret;
  spi_transaction_t t;
  if (len==0) return;             //no need to send anything
  memset(&t, 0, sizeof(t));       //Zero out the transaction
  t.length=len*8;                 //Len is in bytes, transaction length is in bits.
  t.tx_buffer=data;               //Data
  t.user=(void*)1;                //D/C needs to be set to 1
  t.flags=0;
  ret=spi_device_polling_transmit(spi, &t);  //Transmit!
  assert(ret==ESP_OK);            //Should have had no issues.
}

//This function is called (in irq context!) just before a transmission starts. It will
//set the D/C line to the value indicated in the user field.
static void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
  int dc=(int)t->user;
  gpio_set_level((gpio_num_t)_dc, dc);
}


//Initialize the display
static void lcd_init(spi_device_handle_t spi)
{
  //Initialize non-SPI GPIOs
  gpio_set_direction((gpio_num_t)_dc, GPIO_MODE_OUTPUT);

  printf("LCD ILI9341 initialization.\n");
  //Send all the commands


  //memcpy(ili_init_cmds, ili_init_cmdos, sizeof(ili_init_cmdos));
  int cmd=0;
  while (ili_init_cmds[cmd].databytes!=0xff) {
      lcd_cmd(spi, ili_init_cmds[cmd].cmd);
      lcd_data(spi, ili_init_cmds[cmd].data, ili_init_cmds[cmd].databytes&0x1F);
      if (ili_init_cmds[cmd].databytes&0x80) {
          vTaskDelay(100 / portTICK_RATE_MS);
      }
      cmd++;
  }
  

  
  //Allocate memory block buffers and DMA transactions
  printf("Allocate video mem and DMA transactions\n");
  
  int i=0;    
  trans[i].tx_data[0]=ILI9341_RAMWR;
  trans[i].length=8;
  trans[i].user=(void*)0;
  trans[i++].flags=SPI_TRANS_USE_TXDATA; 

  //blocks[0]= &block0[0];
  //blocks[1]= &block1[0];
     
  int remaininglines=ILI9341_TFTREALHEIGHT;
  for (int j=0; j<NR_OF_BLOCK; j++) 
  {      
    int lines_per_block = LINES_PER_BLOCK;
    if ((remaininglines - LINES_PER_BLOCK) < 0) {
      lines_per_block = remaininglines;
    }
    remaininglines -= LINES_PER_BLOCK;        
    /*if (j > 1)*/ blocks[j]= (uint16_t*)heap_caps_malloc(ILI9341_TFTREALWIDTH*lines_per_block*sizeof(uint16_t), MALLOC_CAP_DMA);
    assert(blocks[j]!=NULL);

    trans[i].tx_buffer=blocks[j];
    trans[i].length=ILI9341_TFTREALWIDTH*2*8*lines_per_block;
    trans[i].user=(void*)1;
    trans[i++].flags=0; //undo SPI_TRANS_USE_TXDATA flag
      
      uint16_t color;
      switch (j) {
        case 0:
          color=0xf000;
          break;
        case 1:
          color=0x0f00;
          break;
        case 2:
          color=0x00f0;
          break;
        case 3:
        default:         
          color=0x000f;
          break;         
      }
      uint16_t * fb = blocks[j];
      for (int y=0;y<lines_per_block;y++)
        for (int x=0;x<ILI9341_TFTREALWIDTH;x++)
          *fb++=color;
  }    
  
}



static void send_blocks(spi_device_handle_t spi)
{
  esp_err_t ret;
  //Queue all transactions.
  for (int j=0; j<(NR_OF_BLOCK+1); j++) {
      ret=spi_device_queue_trans(spi, &trans[j], portMAX_DELAY);
      assert(ret==ESP_OK);
  }
}


static void send_blocks_finish(spi_device_handle_t spi)
{
  spi_transaction_t *rtrans;
  esp_err_t ret;
  for (int j=0; j<(NR_OF_BLOCK+1); j++) {
      ret=spi_device_get_trans_result(spi, &rtrans, portMAX_DELAY);
      assert(ret==ESP_OK);
  }
}


ILI9341_t3DMA::ILI9341_t3DMA(uint8_t cs, uint8_t dc, uint8_t rst, uint8_t mosi, uint8_t clk, uint8_t miso,  uint8_t touch_cs,  uint8_t touch_irq)
{
  _cs   = cs;
  _dc   = dc;
  _rst  = rst;
  _mosi = mosi;
  _clk  = clk;
  _miso = miso;
  _touch_irq = touch_irq;
  _touch_cs = touch_cs;
}


void ILI9341_t3DMA::begin(void) {
  esp_err_t ret;

  spi_bus_config_t buscfg;
  memset(&buscfg, 0, sizeof(buscfg));
  buscfg.miso_io_num=_miso;
  buscfg.mosi_io_num=_mosi;
  buscfg.sclk_io_num=_clk;
  buscfg.quadwp_io_num=-1;
  buscfg.quadhd_io_num=-1;
  buscfg.max_transfer_sz=LINES_PER_BLOCK*ILI9341_TFTREALWIDTH*2+8;

  spi_device_interface_config_t devcfg;
  memset(&devcfg, 0, sizeof(devcfg));
  devcfg.clock_speed_hz=60*1000*1000;           //Clock out
  devcfg.mode=0;                                //SPI mode 0
  devcfg.spics_io_num=_cs;                      //CS pin
  devcfg.queue_size=MAX_SPI_TRANS;              
  devcfg.pre_cb=lcd_spi_pre_transfer_callback;  //Specify pre-transfer callback to handle D/C line
  devcfg.flags = SPI_DEVICE_HALFDUPLEX ;        //With IOMUX and half duplex can go faster
  
  //Initialize the SPI bus
  ret=spi_bus_initialize(VSPI_HOST, &buscfg, 1);
  ESP_ERROR_CHECK(ret);
  //Attach the LCD to the SPI bus
  ret=spi_bus_add_device(VSPI_HOST, &devcfg, &lcdspi);
  ESP_ERROR_CHECK(ret);  
};

void ILI9341_t3DMA::start(void) {
  lcd_init(lcdspi); 
}

void ILI9341_t3DMA::refresh(void) {
  send_blocks(lcdspi);
  send_blocks_finish(lcdspi);  
}

void ILI9341_t3DMA::refreshPrepare(void) {
  send_blocks(lcdspi);
}

void ILI9341_t3DMA::refreshFinish(void) {
  send_blocks_finish(lcdspi);  
}

void ILI9341_t3DMA::flipscreen(bool flip)
{
  if (flip) {
    flipped=true;    
  }
  else {
    flipped=false;   
  }
}

bool ILI9341_t3DMA::isflipped(void)
{
  return(flipped);
}

uint16_t * ILI9341_t3DMA::getLineBuffer(int j)
{
  uint16_t * block=blocks[j>>6];  
  return(&block[(j&0x3F)*ILI9341_TFTREALWIDTH]);
}


void ILI9341_t3DMA::fillScreen(uint16_t color) {
  int i,j;
  color=SPI_SWAP_DATA_TX(color,16);
  for (j=0; j<ILI9341_TFTREALHEIGHT; j++)
  {
    uint16_t * block=blocks[j>>6];
    uint16_t * dst=&block[(j&0x3F)*ILI9341_TFTREALWIDTH];
    for (i=0; i<ILI9341_TFTREALWIDTH; i++)
    {
      *dst++ = color;
    }
  }
}

void ILI9341_t3DMA::writeScreen(int width, int height, int stride, uint8_t *buf, uint16_t *palette16) {
  uint8_t *buffer=buf;
  uint8_t *src; 

  int i,j,y=0;
  if (width*2 <= ILI9341_TFTREALWIDTH) {
    for (j=0; j<height; j++)
    {
      uint16_t * block=blocks[y>>6];
      uint16_t * dst=&block[(y&0x3F)*ILI9341_TFTREALWIDTH];        
      src=buffer;
      for (i=0; i<width; i++)
      {
        uint16_t val = SPI_SWAP_DATA_TX(palette16[*src++],16);
        *dst++ = val;
        *dst++ = val;
      }
      y++;
      if (height*2 <= ILI9341_TFTREALHEIGHT) {
        block=blocks[y>>6];
        dst=&block[(y&0x3F)*ILI9341_TFTREALWIDTH];          
        src=buffer;
        for (i=0; i<width; i++)
        {
          uint16_t val = SPI_SWAP_DATA_TX(palette16[*src++],16);
          *dst++ = val;
          *dst++ = val;
        }
        y++;      
      } 
      buffer += stride;      
    }
  }
  else if (width <= ILI9341_TFTREALWIDTH) {
    //dst += (ILI9341_TFTWIDTH-width)/2;
    for (j=0; j<height; j++)
    {
      uint16_t * block=blocks[y>>6];
      uint16_t * dst=&block[(y&0x3F)*ILI9341_TFTREALWIDTH+(ILI9341_TFTREALWIDTH-width)/2];         
      src=buffer;
      for (i=0; i<width; i++)
      {
        uint16_t val = SPI_SWAP_DATA_TX(palette16[*src++],16);
        *dst++ = val;
      }
      y++;
      if (height*2 <= ILI9341_TFTREALHEIGHT) {
        block=blocks[y>>6];
        dst=&block[(y&0x3F)*ILI9341_TFTREALWIDTH+(ILI9341_TFTREALWIDTH-width)/2];        
        src=buffer;
        for (i=0; i<width; i++)
        {
          uint16_t val = SPI_SWAP_DATA_TX(palette16[*src++],16);
          *dst++ = val;
        }
        y++;
      }      
      buffer += stride;  
    }
  }    
}

void ILI9341_t3DMA::writeLine(int width, int height, int y, uint8_t *buf, uint16_t *palette16) {
  if (ILI9341_TFTHEIGHT > height)
    y += (ILI9341_TFTHEIGHT - height)/2;
  uint8_t  * src=buf;
  uint16_t * block=blocks[y>>6];
  uint16_t * dst=&block[(y&0x3F)*ILI9341_TFTREALWIDTH];
  if (ILI9341_TFTWIDTH > width)
    dst += (ILI9341_TFTWIDTH - width)/2;    
  for (int i=0; i<width; i++)
  {
    uint8_t val = *src++;
    *dst++=SPI_SWAP_DATA_TX(palette16[val],16);
  }
}

void ILI9341_t3DMA::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

  int i,j,l=y;
  color=SPI_SWAP_DATA_TX(color,16);
  for (j=0; j<h; j++)
  {
    uint16_t * block=blocks[l>>6];
    uint16_t * dst=&block[(l&0x3F)*ILI9341_TFTREALWIDTH+x];
    for (i=0; i<w; i++)
    {
      *dst++ = color;
    }
    l++;
  }
}

void ILI9341_t3DMA::drawSprite(int16_t x, int16_t y, const uint16_t *bitmap) {
    drawSprite(x,y,bitmap, 0,0,0,0);
}

void ILI9341_t3DMA::drawSprite(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t arx, uint16_t ary, uint16_t arw, uint16_t arh)
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
    uint16_t * dst=&block[(l&0x3F)*ILI9341_TFTREALWIDTH+arx];  
    bmp_ptr = (uint16_t*)bitmap;
    for (int col=0;col<arw; col++)
    {
        *dst++ = SPI_SWAP_DATA_TX(*bmp_ptr++,16);            
    } 
    bitmap +=  w;
    l++;
  }
}

void ILI9341_t3DMA::drawText(int16_t x, int16_t y, const char * text, uint16_t fgcolor, uint16_t bgcolor, bool doublesize) {
  uint16_t c;
  uint16_t * block;
  uint16_t * dst;
  fgcolor = SPI_SWAP_DATA_TX(fgcolor,16);
  bgcolor = SPI_SWAP_DATA_TX(bgcolor,16);
  
  while ((c = *text++)) {
    const unsigned char * charpt=&font8x8[c][0];

    int l=y;
    for (int i=0;i<8;i++)
    {     
      unsigned char bits;
      if (doublesize) {
        block=blocks[l>>6];
        dst=&block[(l&0x3F)*ILI9341_TFTREALWIDTH+x];         
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
      dst=&block[(l&0x3F)*ILI9341_TFTREALWIDTH+x]; 
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



// TOUCH
#define _BV(bit) (1 << (bit))
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



static spi_device_handle_t touchspi;

//void ILI9341_t3DMA::touchBegin(uint8_t mosi, uint8_t miso, uint8_t clk, uint8_t cs) {
void ILI9341_t3DMA::touchBegin() {
  esp_err_t ret;
  
  gpio_set_direction((gpio_num_t)_touch_irq, GPIO_MODE_INPUT);
  gpio_set_pull_mode((gpio_num_t)_touch_irq, GPIO_PULLUP_ONLY);

  spi_device_interface_config_t devcfg;
  memset(&devcfg, 0, sizeof(devcfg));
  devcfg.clock_speed_hz=2500000;
  devcfg.mode=0;
  devcfg.spics_io_num=_touch_cs;
  devcfg.queue_size=2;   
  devcfg.flags = SPI_DEVICE_HALFDUPLEX ;

  /*
  spi_bus_config_t buscfg;
  memset(&buscfg, 0, sizeof(buscfg));
  buscfg.miso_io_num=miso;
  buscfg.mosi_io_num=mosi;
  buscfg.sclk_io_num=clk;
  buscfg.quadwp_io_num=-1;
  buscfg.quadhd_io_num=-1;
  buscfg.max_transfer_sz=48;

  ret=spi_bus_initialize(HSPI_HOST, &buscfg, 2);
  ESP_ERROR_CHECK(ret);
  */
  
  ret=spi_bus_add_device(HSPI_HOST, &devcfg, &touchspi);
  ESP_ERROR_CHECK(ret);  
}


uint16_t touch_get_data(spi_device_handle_t spi, const uint8_t cmd)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&cmd;               //The data is the cmd itself
    esp_err_t ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    if (ret==ESP_OK) {
      memset(&t, 0, sizeof(t));
      t.rxlength=8*2;
      t.flags = SPI_TRANS_USE_RXDATA;
      ret = spi_device_polling_transmit(spi, &t);
      if (ret==ESP_OK) {
        printf("touch data failed\n");
        return 0;
      }
    }
    else {
      printf("touch cmd failed\n");
    } 
    return *(uint16_t*)t.rx_data;    
}



bool ILI9341_t3DMA::isTouching()
{ 
  return ((gpio_get_level((gpio_num_t)_touch_irq) == 0 ? true : false)); 
}

void ILI9341_t3DMA::readRo(uint16_t * oX, uint16_t * oY, uint16_t * oZ) {
  uint16_t x = 0;
  uint16_t y = 0;
  uint16_t z1 = 0;
  uint16_t z2 = 0;
  uint8_t i = 0;
  int16_t xraw=0, yraw=0;


  for(; i < 15; i++) {
    y  += touch_get_data(touchspi, (XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_Y) | XPT2046_CFG_PWR(3)));
    x  += touch_get_data(touchspi, (XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_X) | XPT2046_CFG_PWR(3)));
    z1 += touch_get_data(touchspi, (XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_Z1)| XPT2046_CFG_PWR(3)));
    z2 += touch_get_data(touchspi, (XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_Z2)| XPT2046_CFG_PWR(3)));
  }  
  printf("%d %d %d %d \n",x/15,y/15,z1/15,z2/15);  
/*
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
  */
}

void ILI9341_t3DMA::readRaw(uint16_t * oX, uint16_t * oY, uint16_t * oZ) {
   
}

void ILI9341_t3DMA::readCal(uint16_t * oX, uint16_t * oY, uint16_t * oZ){
   
}

void ILI9341_t3DMA::callibrateTouch(uint16_t xMin,uint16_t yMin,uint16_t xMax,uint16_t yMax){
   
}


