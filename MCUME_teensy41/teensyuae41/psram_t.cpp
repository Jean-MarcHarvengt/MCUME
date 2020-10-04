/*
  PSRAM driver for IPS6404
*/

#include "psram_t.h"

Page PSRAM_T::pages[MAX_PAGES];
uint8_t PSRAM_T::nbPages=0;
int8_t PSRAM_T::top=0;
int8_t PSRAM_T::last=0;


#ifdef HAS_PSRAM

#include <SPI.h>
#include <DMAChannel.h>

#define SPI SPI2

//#define SPICLOCK 104000000 
#define SPICLOCK 70000000 
//#define SPICLOCK 144e6 //Just a number..max speed
#define SPI_MODE SPI_MODE0

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
  pinMode(_cs, OUTPUT); 
  digitalWrite(_cs, 1);
}


void PSRAM_T::begin(void)
{
  SPI.setMOSI(_mosi);
  SPI.setMISO(_miso);
  SPI.setSCK(_sclk);
  SPI.begin();

  delay(1);

  digitalWrite(_cs, 0);
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  SPI.transfer(0x66);
  digitalWrite(_cs, 1);
  SPI.endTransaction();  
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  digitalWrite(_cs, 0);
  SPI.transfer(0x99);
  SPI.endTransaction();
  digitalWrite(_cs, 1);
  delayMicroseconds(20);
}



uint8_t PSRAM_T::psram_read(uint32_t addr) 
{
  uint8_t val=0;

  digitalWrite(_cs, 0);  
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  SPI.transfer(RAM_READ);
  SPI.transfer((addr>>16)&0xff);
  SPI.transfer((addr>>8)&0xff);
  SPI.transfer(addr&0xff);
#if RAM_READ == 0xB  
  SPI.transfer(0xFF);       
#endif
  val = SPI.transfer(0xFF);  
  SPI.endTransaction();
  digitalWrite(_cs, 1);
  return val;
}


void PSRAM_T::psram_read_n(uint32_t addr, uint8_t * val, int n) 
{
  digitalWrite(_cs, 0);
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  SPI.transfer(RAM_READ);
  SPI.transfer((addr>>16)&0xff);
  SPI.transfer((addr>>8)&0xff);
  SPI.transfer(addr&0xff);
#if RAM_READ == 0xB  
  SPI.transfer(0xFF);       
#endif
  /*
  while (n > 0) {
    *val++ = SPI.transfer(0xFF);
    n--;       
  }    
  */
  SPI.transfer(val,n);
  SPI.endTransaction();
  digitalWrite(_cs, 1);
}


void PSRAM_T::psram_write(uint32_t addr, uint8_t val) 
{
  digitalWrite(_cs, 0);
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  SPI.transfer(RAM_WRITE);
  SPI.transfer((addr>>16)&0xff);
  SPI.transfer((addr>>8)&0xff);
  SPI.transfer(addr&0xff);    
  SPI.transfer(val);
  SPI.endTransaction();    
  digitalWrite(_cs, 1);
}

static uint8_t resp[PAGE_SIZE];


void PSRAM_T::psram_write_n(uint32_t addr, uint8_t * val, int n) 
{
  digitalWrite(_cs, 0);
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  SPI.transfer(RAM_WRITE);
  SPI.transfer((addr>>16)&0xff);
  SPI.transfer((addr>>8)&0xff);
  SPI.transfer(addr&0xff);
  /*
  while (n > 0) {
    SPI.transfer(*val++);
    n--;       
  }
  */
  SPI.transfer(val,&resp[0],n);
  SPI.endTransaction();  
  digitalWrite(_cs, 1); 
}


#ifdef DMATRY
#include <DMAChannel.h>

DMAMEM static uint8_t src[PAGE_SIZE+4]; 
DMAMEM static uint8_t dst[PAGE_SIZE+4];
static DMASetting dmasettings[8];
static DMAChannel dmatx;
static volatile uint8_t rstop = 0;

#define DMASPI_PRINT(x) do {Serial.printf(x) ; Serial.flush();} while (0);

static void dmaInterrupt() 
{
  dmatx.clearInterrupt();
  rstop = 1;  
  //DMASPI_PRINT("irq");
}

void PSRAM_T::psram_write_n_dma(uint32_t addr, uint8_t * val, int n) {
  uint8_t * dstPt = &dst[0];
  //*dstPt++ = RAM_WRITE;
  //*dstPt++ = (addr>>16)&0xff;
  //*dstPt++ = (addr>>8)&0xff;
  //*dstPt++ = addr&0xff;
  int cnt = n;
  while (cnt > 0) {
    *dstPt++ = *val++;
    cnt--;       
  }    
  arm_dcache_flush(&dst[0], n);
  rstop = 0;     
  dmatx.disable();
  dmatx.attachInterrupt(dmaInterrupt);

  dmasettings[0].sourceBuffer(&dst[0], n);
  //dmasettings[0].destination((uint8_t &)  LPSPI4_TDR);
  dmasettings[0].destination((uint8_t &)  IMXRT_LPSPI1_S.TDR);
  dmasettings[0].TCD->ATTR_DST = 1;
  dmasettings[0].interruptAtCompletion();

Serial.printf("A") ; Serial.flush();

  digitalWrite(_cs, HIGH);
 // SPI.begin();
  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));

  digitalWrite(_cs, 0); 
  SPI.transfer(RAM_WRITE);
  SPI.transfer((addr>>16)&0xff);
  SPI.transfer((addr>>8)&0xff);
  SPI.transfer(addr&0xff);
  //SPI.transfer(0);
  //SPI.transfer(1);
Serial.printf("B") ; Serial.flush();

#ifdef XXX           
  LPSPI4_CR &= ~LPSPI_CR_MEN;//disable LPSPI:
  LPSPI4_CFGR1 |= LPSPI_CFGR1_NOSTALL; //prevent stall from RX
  //LPSPI4_TCR = 15; // Framesize 16 Bits
  IMXRT_LPSPI4_S.TCR = (IMXRT_LPSPI4_S.TCR & ~(LPSPI_TCR_FRAMESZ(31))) | LPSPI_TCR_FRAMESZ(7);  
   LPSPI4_DER = LPSPI_DER_TDDE; //TX DMA Request Enable
   // IMXRT_LPSPI4_S.DER = LPSPI_DER_TDDE; // | LPSPI_DER_RDDE; //enable DMA on both TX and RX
  LPSPI4_CR |= LPSPI_CR_MEN; //enable LPSPI:
   //IMXRT_LPSPI4_S.TCR = (IMXRT_LPSPI4_S.TCR & ~(LPSPI_TCR_FRAMESZ(31))) | LPSPI_TCR_FRAMESZ(7);  
  //  IMXRT_LPSPI4_S.FCR = 0; 
    // Lets try to output the first byte to make sure that we are in 8 bit mode...
 //   IMXRT_LPSPI4_S.SR = 0x3f00; // clear out all of the other status...
   
  dmatx.triggerAtHardwareEvent( DMAMUX_SOURCE_LPSPI4_TX );
#endif
  IMXRT_LPSPI1_S.CR &= ~LPSPI_CR_MEN;//disable LPSPI:
  IMXRT_LPSPI1_S.CFGR1 |= LPSPI_CFGR1_NOSTALL; //prevent stall from RX
  IMXRT_LPSPI1_S.TCR = (IMXRT_LPSPI4_S.TCR & ~(LPSPI_TCR_FRAMESZ(31))) | LPSPI_TCR_FRAMESZ(7);  
  IMXRT_LPSPI1_S.DER = LPSPI_DER_TDDE; //TX DMA Request Enable
  IMXRT_LPSPI1_S.CR |= LPSPI_CR_MEN; //enable LPSPI:
  dmatx.triggerAtHardwareEvent( DMAMUX_SOURCE_LPSPI1_TX );
Serial.printf("C") ; Serial.flush();
//  SPI.initDMAChannels();
  dmatx = dmasettings[0];
      
  dmatx.enable();
Serial.printf("D") ; Serial.flush();
  while (rstop == 0) {};

 Serial.printf("E") ; Serial.flush();
  digitalWrite(_cs, 1);
}
#endif



#else


#include "emuapi.h"


PSRAM_T::PSRAM_T(uint8_t cs, uint8_t mosi, uint8_t sclk, uint8_t miso)
{
}

void PSRAM_T::begin(void)
{
  emu_FileTempInit();
}

void PSRAM_T::psram_read_n(uint32_t addr, uint8_t * val, int n) 
{
  emu_FileTempRead(addr,val,n);    
}


void PSRAM_T::psram_write(uint32_t addr, uint8_t val) 
{
  emu_FileTempWrite(addr,val);  
}

#endif



void PSRAM_T::pswrite(uint32_t addr, uint8_t val) 
{
  psram_write(addr, val);
  //return;
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


