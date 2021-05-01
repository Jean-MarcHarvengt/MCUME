#include "pico.h"
#include "pico/stdlib.h"

extern "C" {
  #include "iopins.h"  
  #include "emuapi.h"  
}
#include "keyboard_osd.h"
#include "vga_t_dma.h"
#include "sndplay.h"

#include <stdio.h>


TFT_T_DMA tft;
static int skip=0;

bool repeating_timer_callback(struct repeating_timer *t) {
    uint16_t bClick = emu_DebounceLocalKeys();
    emu_Input(bClick);    
    return true;
}

int main(void) {
    stdio_init_all();
//set_sys_clock_48mhz();
//set_sys_clock_khz(133000, true);  
//set_sys_clock_khz(48000, true);  
//set_sys_clock_khz(270000, true);
    tft.begin(VGA_MODE_320x240);
    emu_init();
    while (true) {
        if (menuActive()) {
            uint16_t bClick = emu_DebounceLocalKeys();
            int action = handleMenu(bClick);
            char * filename = menuSelection();   
            if (action == ACTION_RUNTFT) {
              toggleMenu(false);
              emu_start();        
              emu_Init(filename);      
              tft.fillScreenNoDma( RGBVAL16(0x00,0x00,0x00) );
              tft.startDMA();
              //struct repeating_timer timer;
              //add_repeating_timer_ms(50, repeating_timer_callback, NULL, &timer);      
            }  
            tft.waitSync();
        }
        else {  
            uint16_t bClick = emu_DebounceLocalKeys();
            emu_Input(bClick);              
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

void emu_SetPaletteEntry(unsigned char r, unsigned char g, unsigned char b, int index)
{
    if (index<PALETTE_SIZE) {
        palette8[index]  = RGBVAL8(r,g,b);
    }
}

void emu_DrawVsync(void)
{
    skip += 1;
    skip &= VID_FRAME_SKIP;
    tft.waitSync(); 
}

void emu_DrawLine(unsigned char * VBuf, int width, int height, int line) 
{
    if (skip == 0) {
        tft.writeLine(width,height,line, VBuf, palette8);
    }  
}  


void emu_DrawLine8(unsigned char * VBuf, int width, int height, int line) 
{
    if (skip == 0) {
      tft.writeLine(width,height,line, VBuf);
    }
} 

void emu_DrawLine16(unsigned short * VBuf, int width, int height, int line) 
{
    if (skip == 0) {
        tft.writeLine16(width,height,line, VBuf);
    }
}  

void emu_DrawScreen(unsigned char * VBuf, int width, int height, int stride) 
{
    if (skip == 0) {
        tft.writeScreen(width,height-TFT_VBUFFER_YCROP,stride, VBuf+(TFT_VBUFFER_YCROP/2)*stride, palette8);
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



#ifdef HAS_SND
#include "AudioPlaySystem.h"
AudioPlaySystem mymixer;

void emu_sndInit() {
  tft.begin_audio(256, mymixer.snd_Mixer);
  mymixer.start();    
}

void emu_sndPlaySound(int chan, int volume, int freq)
{
  if (chan < 6) {
    mymixer.sound(chan, freq, volume); 
  }
}

void emu_sndPlayBuzz(int size, int val) {
  mymixer.buzz(size,val); 
}

#endif

