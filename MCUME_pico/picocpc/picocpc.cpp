#include "pico.h"
#include "pico/stdlib.h"

extern "C" {
  #include "iopins.h"  
  #include "emuapi.h"  
}
#include "keyboard_osd.h"


#include "cpc.h"
#include <stdio.h>

#ifdef USE_VGA
#include "vga_t_dma.h"
#else
#include "tft_t_dma.h"
#endif
volatile bool vbl=true;

bool repeating_timer_callback(struct repeating_timer *t) {
    uint16_t bClick = emu_DebounceLocalKeys();
    emu_Input(bClick);     
    if (vbl) {
        vbl = false;
    } else {
        vbl = true;
    }   
    return true;
}
TFT_T_DMA tft;

static int skip=0;

#include "hardware/clocks.h"
#include "hardware/vreg.h"

int main(void) {
//    vreg_set_voltage(VREG_VOLTAGE_1_05);
//    set_sys_clock_khz(125000, true);    
//    set_sys_clock_khz(150000, true);    
//    set_sys_clock_khz(133000, true);    
//    set_sys_clock_khz(200000, true);    
//    set_sys_clock_khz(210000, true);    
    set_sys_clock_khz(230000, true);    
//    set_sys_clock_khz(225000, true);    
//    set_sys_clock_khz(250000, true);  
    stdio_init_all();

#ifdef USE_VGA    
    tft.begin(VGA_MODE_320x240);
#else
    tft.begin();
#endif
    emu_init();
    while (true) {
        if (menuActive()) {
            uint16_t bClick = emu_DebounceLocalKeys();
            int action = ACTION_RUNTFT;  //handleMenu(bClick);
            char * filename = menuSelection();   
            if (action == ACTION_RUNTFT) {
              toggleMenu(false);
              emu_Init(filename);   
              emu_start();        
              tft.fillScreenNoDma( RGBVAL16(0x00,0x00,0x00) );
              tft.startDMA(); 
              struct repeating_timer timer;
              add_repeating_timer_ms(5, repeating_timer_callback, NULL, &timer);                               
            }  
            tft.waitSync();
        }
        else {
            emu_Step();   
        }
        //int c = getchar_timeout_us(0);
        //switch (c) {
        //    case ' ':
        //        printf("test: %d\n", 1);
        //        break;
        //}
    }
}

static unsigned char  palette8[PALETTE_SIZE];
static unsigned short palette16[PALETTE_SIZE];

void emu_SetPaletteEntry(unsigned char r, unsigned char g, unsigned char b, int index)
{
    if (index<PALETTE_SIZE) {
        palette8[index]  = RGBVAL8(r,g,b);
        palette16[index]  = RGBVAL16(r,g,b);        
    }
    printf("index: %d palette[8]: %d, r %d, g %d, b %d\n", index, palette8[index], r, g, b);
}

void emu_DrawVsync(void)
{
    skip += 1;
    skip &= VID_FRAME_SKIP;
    volatile bool vb=vbl; 
    while (vbl==vb) {};
#ifdef USE_VGA   
   // tft.waitSync();                   
#else                      
//    volatile bool vb=vbl; 
//    while (vbl==vb) {};
#endif
}

void emu_DrawPixel(unsigned char * VBuf, int x, int y)
{
    tft.drawPixel(x, y, (uint8_t) *VBuf++);
}


void emu_DrawLine(unsigned char * VBuf, int width, int height, int line) 
{
    if (skip == 0) {
#ifdef USE_VGA                        
         tft.writeLine(width,height,line, VBuf, palette8);
#else
         tft.writeLine(width,height,line, VBuf, palette16);
#endif      
    }
}  

void emu_DrawLine8(unsigned char * VBuf, int width, int height, int line) 
{
    if (skip == 0) {
#ifdef USE_VGA                        
      tft.writeLine(width,height,line, VBuf);
#endif      
    }
} 

void emu_DrawLine16(unsigned short * VBuf, int width, int height, int line) 
{
    if (skip == 0) {
#ifdef USE_VGA        
        tft.writeLine16(width,height,line, VBuf);
#endif        
    }
}  

void emu_DrawScreen(unsigned char * VBuf, int width, int height, int stride) 
{
    if (skip == 0) {
#ifdef USE_VGA                
        tft.writeScreen(width,height-TFT_VBUFFER_YCROP,stride, VBuf+(TFT_VBUFFER_YCROP/2)*stride, palette8);
#endif
    }
}  

int emu_FrameSkip(void)
{
    return skip;
}

void * emu_LineBuffer(int line)
{
    return (void*)tft.getLineBuffer(line);    
}