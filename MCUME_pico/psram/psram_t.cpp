/*
  PSRAM driver for IPS6404
*/

#include "iopins.h"  

#include "psram_t.h"

#ifdef HAS_PSRAM

Page PSRAM_T::pages[MAX_PAGES];
uint8_t PSRAM_T::nbPages=0;
int8_t PSRAM_T::top=0;
int8_t PSRAM_T::last=0;

#include "pico/stdlib.h"
#include "hardware/spi.h"

#define digitalWrite(pin, val) gpio_put(pin, val)

static void SPItransfer(uint8_t val)
{
  uint8_t dat8=val;
  spi_write_blocking(PSRAM_SPIREG, &dat8, 1);
}

static uint8_t SPItransferRD(uint8_t val)
{
  uint8_t dat8=0xff;
  spi_read_blocking(PSRAM_SPIREG, val, &dat8, 1);
  return dat8;
}



#define SPICLOCK 104000000 
//#define SPICLOCK 70000000 
//#define SPICLOCK 50000000 

#define SPI_MODE SPI_CPOL_0

#define RAM_READ  0xB
//#define RAM_READ  0x3
#define RAM_WRITE 0x2


uint8_t PSRAM_T::_cs, PSRAM_T::_miso, PSRAM_T::_mosi, PSRAM_T::_sclk;


PSRAM_T::PSRAM_T(uint8_t cs, uint8_t mosi, uint8_t sclk, uint8_t miso)
{
  _cs   = cs;
  _mosi = mosi;
  _sclk = sclk;
  _miso = miso;
}


void PSRAM_T::begin(void)
{
  spi_init(PSRAM_SPIREG, SPICLOCK);
  spi_set_format(PSRAM_SPIREG, 8, SPI_MODE, SPI_CPHA_0, SPI_MSB_FIRST);
  gpio_set_function(_miso , GPIO_FUNC_SPI); 
  gpio_set_function(_cs , GPIO_FUNC_SPI); 
  gpio_set_function(_sclk , GPIO_FUNC_SPI);
  gpio_set_function(_mosi , GPIO_FUNC_SPI);
  gpio_init(_cs);
  gpio_set_dir(_cs, GPIO_OUT);
  digitalWrite(_cs, 1);
  sleep_ms(1);
  digitalWrite(_cs, 0);
  SPItransfer(0x66);
  digitalWrite(_cs, 1);
  sleep_ms(1);
  digitalWrite(_cs, 0);
  SPItransfer(0x99);
  digitalWrite(_cs, 1);
  sleep_ms(20);
}


void PSRAM_T::psram_write(uint32_t addr, uint8_t val) 
{
  digitalWrite(_cs, 0);
  SPItransfer(RAM_WRITE);
  SPItransfer((addr>>16)&0xff);
  SPItransfer((addr>>8)&0xff);
  SPItransfer(addr&0xff);    
  SPItransfer(val);  
  digitalWrite(_cs, 1);
}

uint8_t PSRAM_T::psram_read(uint32_t addr) 
{
  uint8_t val=0;

  digitalWrite(_cs, 0);  
  SPItransfer(RAM_READ);
  SPItransfer((addr>>16)&0xff);
  SPItransfer((addr>>8)&0xff);
  SPItransfer(addr&0xff);
#if RAM_READ == 0xB  
  SPItransfer(0xFF);       
#endif
  val = SPItransferRD(0xFF);  
  digitalWrite(_cs, 1);
  return val;
}


void PSRAM_T::psram_read_n(uint32_t addr, uint8_t * val, int n) 
{
  digitalWrite(_cs, 0);
  SPItransfer(RAM_READ);
  SPItransfer((addr>>16)&0xff);
  SPItransfer((addr>>8)&0xff);
  SPItransfer(addr&0xff);
#if RAM_READ == 0xB  
  SPItransferRD(0xFF);       
#endif
  //while (n > 0) {
  //  *val++ = SPItransferRD(0xFF);
  //  n--;       
  //}    
  spi_read_blocking(PSRAM_SPIREG, 0xFF, val, n);
  digitalWrite(_cs, 1);
}


static uint8_t resp[PAGE_SIZE];


void PSRAM_T::psram_write_n(uint32_t addr, uint8_t * val, int n) 
{
  digitalWrite(_cs, 0);
  SPItransfer(RAM_WRITE);
  SPItransfer((addr>>16)&0xff);
  SPItransfer((addr>>8)&0xff);
  SPItransfer(addr&0xff);
  
  while (n > 0) {
    SPItransfer(*val++);
    n--;       
  }
  //SPItransfer(val,&resp[0],n); 
  digitalWrite(_cs, 1); 
}


void PSRAM_T::pswrite(uint32_t addr, uint8_t val) 
{
  psram_write(addr, val);
  //return
  uint32_t curPage=addr&(~(PAGE_SIZE-1));
  for (int i=0; i<nbPages; i++) {
    if (pages[i].pageid == curPage) {
      pages[i].page[addr&(PAGE_SIZE-1)] = val;
      break;
    }
  }   
}


uint8_t PSRAM_T::psread(uint32_t addr) 
{
  //uint8_t val = psram_read(addr);
  //return val;
  uint32_t curPage=addr&(~(PAGE_SIZE-1));
  uint32_t offs = addr&(PAGE_SIZE-1);

  for (int i=0; i<nbPages; i++) {
    if (pages[i].pageid == curPage) {
      if ( (pages[i].prev != i) && (pages[i].next != i) ) {
        pages[pages[i].prev].next =  pages[i].next;
        pages[pages[i].next].prev = pages[i].prev;     
      }
      else if (pages[i].next != i) {
        pages[pages[i].next].prev = i;
      }
      else if (pages[i].prev != i) {
        pages[pages[i].prev].next =  pages[i].prev;
        last = pages[i].prev;        
      }
       // last page accessed to top
      pages[i].prev = i; //-1;
      pages[i].next = top;      
      pages[top].prev = i;
      top = i;
      return pages[top].page[offs];
    }
  }
  if (nbPages<MAX_PAGES) 
  {
    // add at top
    pages[nbPages].pageid = curPage;           
    pages[nbPages].prev = nbPages; //-1;
    pages[nbPages].next = top;     
    pages[top].prev = nbPages;
    top = nbPages;
    nbPages++;
  }
  else {
      // replace last and move to top
      int n = pages[last].prev;
      pages[n].next = n; //-1;
      pages[last].pageid = curPage;
      pages[last].prev = last; //-1;
      pages[last].next = top;      
      pages[top].prev = last;
      top = last;
      last = n;
  }
  //emu_printi(curPage);
  psram_read_n(curPage,&(pages[top].page[0]),PAGE_SIZE);   
  return pages[top].page[offs];
}

uint16_t PSRAM_T::psread_w(uint32_t addr) 
{
  uint32_t curPage=addr&(~(PAGE_SIZE-1));
  uint32_t offs = addr&(PAGE_SIZE-1);

  for (int i=0; i<nbPages; i++) {
    if (pages[i].pageid == curPage) {
      if ( (pages[i].prev != i) && (pages[i].next != i) ) {
        pages[pages[i].prev].next =  pages[i].next;
        pages[pages[i].next].prev = pages[i].prev;     
      }
      else if (pages[i].next != i) {
        pages[pages[i].next].prev = i;
      }
      else if (pages[i].prev != i) {
        pages[pages[i].prev].next =  pages[i].prev;
        last = pages[i].prev;        
      }
       // last page accessed to top
      pages[i].prev = i; //-1;
      pages[i].next = top;      
      pages[top].prev = i;
      top = i;     
      return (pages[top].page[offs+1]<<8) + pages[top].page[offs];
    }
  }
  if (nbPages<MAX_PAGES) 
  {
    // add at top
    pages[nbPages].pageid = curPage;           
    pages[nbPages].prev = nbPages; //-1;
    pages[nbPages].next = top;     
    pages[top].prev = nbPages;
    top = nbPages;
    nbPages++;
  }
  else {
      // replace last and move to top
      int n = pages[last].prev;
      pages[n].next = n; //-1;
      pages[last].pageid = curPage;
      pages[last].prev = last; //-1;
      pages[last].next = top;      
      pages[top].prev = last;
      top = last;
      last = n;
  }
  //emu_printi(curPage);
  psram_read_n(curPage,&(pages[top].page[0]),PAGE_SIZE);   
  return (pages[top].page[offs+1]<<8) + pages[top].page[offs];
}
#endif
