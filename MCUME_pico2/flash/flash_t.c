/*
  RP2350 flash driver
*/

#include "flash_t.h"

#include "hardware/flash.h"
#include "hardware/sync.h"
#include "emuapi.h"


unsigned char * flash_start = (unsigned char *)(XIP_BASE + HW_FLASH_STORAGE_BASE);
unsigned char * flash_end = (unsigned char *)(XIP_BASE + HW_FLASH_STORAGE_TOP);

static uint8_t cache[FLASH_SECTOR_SIZE];

int flash_load(const char * filename) 
{
  uint32_t offset = HW_FLASH_STORAGE_BASE;
  int n;
  int size = 0;
  emu_printf("flash_load...");
  int f = emu_FileOpen(filename,"r+b");
  if (f) {
    while ( (offset < (HW_FLASH_STORAGE_TOP-FLASH_SECTOR_SIZE)) && (n = emu_FileRead(cache,FLASH_SECTOR_SIZE,f) ) ) {
      //memcpy(cache, (unsigned char *)offset, n);
      uint32_t ints = save_and_disable_interrupts();   
      flash_range_erase(offset, FLASH_SECTOR_SIZE);
      flash_range_program(offset, (const uint8_t *)&cache[0], FLASH_SECTOR_SIZE);
      restore_interrupts(ints);      
      emu_printi(n);
      emu_printi(offset);          
      //uint8_t * pt = (uint8_t*)(XIP_BASE + offset);
      //emu_printi(pt[0]);
      //emu_printi(pt[1]);
      //emu_printi(pt[2]);
      //emu_printi(pt[3]);
      offset += FLASH_SECTOR_SIZE;
      size += n;          
    }
    emu_FileClose(f);
    emu_printf("flash_load OK.");    
  }

  return size;
}

int flash_load_bswap(const char * filename) 
{
  uint32_t offset = HW_FLASH_STORAGE_BASE;
  int n;
  int size = 0;
  emu_printf("flash_load...");
  int f = emu_FileOpen(filename,"r+b");
  if (f) {
    while ( (offset < (HW_FLASH_STORAGE_TOP-FLASH_SECTOR_SIZE)) && (n = emu_FileRead(cache,FLASH_SECTOR_SIZE,f) ) ) {
      for (int i=0;i<n; i+=2) {
        uint8_t k = cache[i];
        cache[i]=cache[i+1];
        cache[i+1] = k;
      }  
      //memcpy(cache, (unsigned char *)offset, n);
      uint32_t ints = save_and_disable_interrupts();   
      flash_range_erase(offset, FLASH_SECTOR_SIZE);
      flash_range_program(offset, (const uint8_t *)&cache[0], FLASH_SECTOR_SIZE);
      restore_interrupts(ints);      
      emu_printi(n);
      emu_printi(offset);          
      //uint8_t * pt = (uint8_t*)(XIP_BASE + offset);
      //emu_printi(pt[0]);
      //emu_printi(pt[1]);
      //emu_printi(pt[2]);
      //emu_printi(pt[3]);
      offset += FLASH_SECTOR_SIZE;
      size += n;          
    }
    emu_FileClose(f);
    emu_printf("flash_load OK.");    
  }

  return size;
}

int flash_verify(unsigned char * buf, int size)
{
  unsigned char * datapt = (unsigned char *)(XIP_BASE + HW_FLASH_STORAGE_BASE);
  emu_printf("flash_verify...");
  int count = size;
  while (count++ < size) {
    if (*datapt != *buf) {
      emu_printf("mismatch at ");
      emu_printi(count);
      emu_printi(*datapt);
      emu_printi(*buf);
      return 1;
    }  
    datapt++;
    buf++; 
  }  
  emu_printf("flash_verify OK.");
  return 0;
}