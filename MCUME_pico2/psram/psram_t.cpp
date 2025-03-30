#include "platform_config.h"

#ifdef HAS_PSRAM
/*
  PSRAM driver for IPS6404
*/

#include "psram_t.h"

#include <cstdio>

#include <pico/stdlib.h>

#include "psram_spi.h"

#ifdef PSCACHE 
Page PSRAM_T::pages[MAX_PAGES];
uint8_t PSRAM_T::nbPages=0;
int8_t PSRAM_T::top=0;
int8_t PSRAM_T::last=0;
#endif


#define RAM_READ  0xB
//#define RAM_READ  0x3
#define RAM_WRITE 0x2

static psram_spi_inst_t psram_spi;


PSRAM_T::PSRAM_T(uint8_t cs, uint8_t mosi, uint8_t sclk, uint8_t miso)
{
}

void PSRAM_T::begin(void)
{
  psram_spi = psram_spi_init(pio2, 0);
}


uint8_t PSRAM_T::psram_read(uint32_t addr) 
{
  return psram_read8(&psram_spi, addr);
}

uint16_t PSRAM_T::psram_read_w(uint32_t addr) 
{
  return psram_read16(&psram_spi, addr);
}


void PSRAM_T::psram_read_n(uint32_t addr, uint8_t * val, int n) 
{
  //printf("r %d %d\n", addr, n);
  psram_readn(&psram_spi, addr, val, n); 
}


void PSRAM_T::psram_write(uint32_t addr, uint8_t val) 
{
  psram_write8(&psram_spi, addr, val); 
}

void PSRAM_T::psram_write_w(uint32_t addr, uint16_t val) 
{
  psram_write16(&psram_spi, addr, val); 
}

static uint8_t resp[PAGE_SIZE];


void PSRAM_T::psram_write_n(uint32_t addr, uint8_t * val, int n) 
{
  printf("w %d %d\n", addr, n);
  psram_writen(&psram_spi, addr, val, n);  
}





void PSRAM_T::pswrite(uint32_t addr, uint8_t val) 
{
  psram_write(addr, val);
#ifdef PSCACHE  
  uint32_t curPage=addr&(~(PAGE_SIZE-1));
  for (int i=0; i<nbPages; i++) {
    if (pages[i].pageid == curPage) {
      pages[i].page[addr&(PAGE_SIZE-1)] = val;
      break;
    }
  }  
#endif   
}




uint8_t PSRAM_T::psread(uint32_t addr) 
{
#ifdef PSCACHE  
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
#else
  return psram_read(addr);
#endif
}

uint16_t PSRAM_T::psread_w(uint32_t addr) 
{
#ifdef PSCACHE
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
#else
  return psram_read_w(addr);
#endif  
}

void PSRAM_T::pswrite_w(uint32_t addr, uint16_t val)
{
  psram_write_w(addr, val);
#ifdef PSCACHE 
  uint32_t curPage=addr&(~(PAGE_SIZE-1));
  for (int i=0; i<nbPages; i++) {
    if (pages[i].pageid == curPage) {
      pages[i].page[addr&(PAGE_SIZE-1)] = val&0xff;
      pages[i].page[(addr+1)&(PAGE_SIZE-1)] = val>>8 ;
      break;
    }
  }
#endif
}
#endif
