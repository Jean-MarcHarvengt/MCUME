/*
  PSRAM driver for IPS6404
*/

#ifndef _PSRAM_T_H_
#define _PSRAM_T_H_

#ifdef __cplusplus
#include <Arduino.h>
#include "platform_config.h"

#ifdef HAS_PSRAM
#define PAGE_SIZE    16 //32 //2048 //1024
#define MAX_PAGES    8
#else
#define PAGE_SIZE    256
#define MAX_PAGES    8 
#endif

struct Page {
   uint8_t  page[PAGE_SIZE];
   uint32_t pageid;
   int8_t next;
   int8_t prev;
};

class PSRAM_T
{
  public:
    PSRAM_T(uint8_t _CS, uint8_t _MOSI=11, uint8_t _SCLK=13, uint8_t _MISO=12);
    void begin(void);
    void pswrite(uint32_t addr, uint8_t val);
    uint8_t psread(uint32_t addr);
    uint16_t psread_w(uint32_t addr);
   
  private:
    static uint8_t psram_read(uint32_t addr);
    static void psram_read_n(uint32_t addr, uint8_t * val, int n);
    static void psram_write(uint32_t addr, uint8_t val);
    static void psram_write_n(uint32_t addr, uint8_t * val, int n);
   
  protected:
    static uint8_t _cs, _miso, _mosi, _sclk;
    static Page pages[MAX_PAGES];
    static uint8_t nbPages;
    static int8_t top;
    static int8_t last;
};
#endif

#endif


