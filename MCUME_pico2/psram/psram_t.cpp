#include "iopins.h"

#ifdef HAS_PSRAM
/*
  PSRAM driver for IPS6404
*/

#include "psram_t.h"

#include <cstdio>

#include <pico/stdlib.h>

#include "hardware/spi.h"
#include "psram_spi.h"

#ifdef PSCACHE 
Page PSRAM_T::pages[MAX_PAGES];
uint8_t PSRAM_T::nbPages=0;
int8_t PSRAM_T::top=0;
int8_t PSRAM_T::last=0;
#endif


static psram_spi_inst_t psram_spi;

psram_spi_inst_t psram_spi_init_clkdiv(PIO pio, int sm, float clkdiv, bool fudge) {
    psram_spi_inst_t spi;
    spi.pio = pio;
    spi.offset = pio_add_program(spi.pio, fudge ? &spi_psram_fudge_program : &spi_psram_program);
    if (sm == -1) {
        spi.sm = pio_claim_unused_sm(spi.pio, true);
    } else {
        spi.sm = sm;
    }

    //gpio_set_drive_strength(PSRAM_PIN_CS, GPIO_DRIVE_STRENGTH_4MA);
    //gpio_set_drive_strength(PSRAM_PIN_SCK, GPIO_DRIVE_STRENGTH_4MA);
    //gpio_set_drive_strength(PSRAM_PIN_MOSI, GPIO_DRIVE_STRENGTH_4MA);
    
    gpio_init(PSRAM_SCLK);
    gpio_pull_up(PSRAM_SCLK);
    gpio_init(PSRAM_MISO);
    gpio_pull_up(PSRAM_MISO);
    gpio_init(PSRAM_MOSI);
    gpio_pull_up(PSRAM_MOSI);
    gpio_init(PSRAM_CS);
    gpio_pull_up(PSRAM_CS);

    /* gpio_set_slew_rate(PSRAM_PIN_CS, GPIO_SLEW_RATE_FAST); */
    /* gpio_set_slew_rate(PSRAM_PIN_SCK, GPIO_SLEW_RATE_FAST); */
    /* gpio_set_slew_rate(PSRAM_PIN_MOSI, GPIO_SLEW_RATE_FAST); */

    pio_spi_psram_cs_init(spi.pio, spi.sm, spi.offset, 8 /*n_bits*/, clkdiv, fudge, PSRAM_CS, PSRAM_MOSI, PSRAM_MISO);

    // Write DMA channel setup
    spi.write_dma_chan = PSR_DMA_CHANNEL; //dma_claim_unused_channel(true);
    spi.write_dma_chan_config = dma_channel_get_default_config(spi.write_dma_chan);
    channel_config_set_transfer_data_size(&spi.write_dma_chan_config, DMA_SIZE_8);
    channel_config_set_read_increment(&spi.write_dma_chan_config, true);
    channel_config_set_write_increment(&spi.write_dma_chan_config, false);
    channel_config_set_dreq(&spi.write_dma_chan_config, pio_get_dreq(spi.pio, spi.sm, true));
    dma_channel_set_write_addr(spi.write_dma_chan, &spi.pio->txf[spi.sm], false);
    dma_channel_set_config(spi.write_dma_chan, &spi.write_dma_chan_config, false);

    // Read DMA channel setup
    spi.read_dma_chan = PSR_DMA_CHANNEL+1; //dma_claim_unused_channel(true);
    spi.read_dma_chan_config = dma_channel_get_default_config(spi.read_dma_chan);
    channel_config_set_transfer_data_size(&spi.read_dma_chan_config, DMA_SIZE_8);
    channel_config_set_read_increment(&spi.read_dma_chan_config, false);
    channel_config_set_write_increment(&spi.read_dma_chan_config, true);
    channel_config_set_dreq(&spi.read_dma_chan_config, pio_get_dreq(spi.pio, spi.sm, false));
    dma_channel_set_read_addr(spi.read_dma_chan, &spi.pio->rxf[spi.sm], false);
    dma_channel_set_config(spi.read_dma_chan, &spi.read_dma_chan_config, false);


    uint8_t psram_reset_en_cmd[] = {
        8,      // 8 bits to write
        0,      // 0 bits to read
        0x66u   // Reset enable command
    };
    pio_spi_write_read_dma_blocking(&spi, psram_reset_en_cmd, 3, 0, 0);
    busy_wait_us(50);
    uint8_t psram_reset_cmd[] = {
        8,      // 8 bits to write
        0,      // 0 bits to read
        0x99u   // Reset command
    };
    pio_spi_write_read_dma_blocking(&spi, psram_reset_cmd, 3, 0, 0);
    busy_wait_us(100);
    
    return spi;
};


PSRAM_T::PSRAM_T(uint8_t cs, uint8_t mosi, uint8_t sclk, uint8_t miso)
{
#ifdef PSCACHE 
  nbPages = 0;
  top = 0;
  last = 0;
#endif

}

void PSRAM_T::begin(void)
{

#ifdef PIO_SPI
  psram_spi = psram_spi_init_clkdiv(pio2, 0, 1.0, true);
#else
  gpio_init(PSRAM_SCLK);
  gpio_pull_up(PSRAM_SCLK);
  gpio_init(PSRAM_MISO);
  gpio_pull_up(PSRAM_MISO);
  gpio_init(PSRAM_MOSI);
  gpio_pull_up(PSRAM_MOSI);
  gpio_init(PSRAM_CS);
  gpio_set_dir(PSRAM_CS, GPIO_OUT);  
  //gpio_pull_up(PSRAM_CS);
  spi_init(PSRAM_SPIREG, 60000000);
  spi_set_format(PSRAM_SPIREG, 8, SPI_CPOL_1, SPI_CPHA_0, SPI_MSB_FIRST);
  gpio_set_function(PSRAM_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PSRAM_SCLK, GPIO_FUNC_SPI);
  gpio_set_function(PSRAM_MOSI, GPIO_FUNC_SPI);
  gpio_set_function(PSRAM_CS, GPIO_FUNC_SPI);

    uint8_t psram_reset_en_cmd[] = {
        8,      // 8 bits to write
        0,      // 0 bits to read
        0x66u   // Reset enable command
    };
    spi_write_blocking(PSRAM_SPIREG, psram_reset_en_cmd, 3);
    busy_wait_us(50);
    uint8_t psram_reset_cmd[] = {
        8,      // 8 bits to write
        0,      // 0 bits to read
        0x99u   // Reset command
    };
    spi_write_blocking(PSRAM_SPIREG, psram_reset_cmd, 3);
    busy_wait_us(100);
#endif


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

//void PSRAM_T::psram_write_n(uint32_t addr, uint8_t * val, int n) 
//{
//  psram_writen(&psram_spi, addr, val, n);  
//}





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
