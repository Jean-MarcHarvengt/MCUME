#include "pico.h"
#include "pico/stdlib.h"

#include "vga_t_dma.h"

#include <stdio.h>
#include <string>
#include <cstring>  

TFT_T_DMA vga;

#define BLACK       VGA_RGB(0,0,0)


#define BLUE        VGA_RGB(0, 0, 170)
#define LIGHT_BLUE  VGA_RGB(0, 136, 255)
#define GREEN       VGA_RGB(0, 170, 0)
#define LIGHT_GREEN VGA_RGB(144, 238, 144)

static int fb_width, fb_height;

static char * digits = "0123456789";

int main(void) {
    set_sys_clock_khz(230000, true);
    stdio_init_all();
    
    vga.begin(VGA_MODE_320x240);
    vga.clear(LIGHT_BLUE);
    vga.get_frame_buffer_size(&fb_width, &fb_height);

    vga.drawRect((fb_width-320)/2, (fb_height-200), 320, 200, BLUE);

    auto w = std::to_string(fb_width);
    auto h = std::to_string(fb_height);
    char* width = new char[w.length() + 1];
    char* height = new char[h.length() + 1];
    strcpy(width,w.c_str());
    strcpy(height,h.c_str());

    while (true) {
        vga.waitSync();
        vga.clear(BLUE);
        vga.drawText((fb_width-320)/2 + 1*8,(fb_height-200)/2+1*8,"Soon to be CPC emulator",LIGHT_BLUE,BLUE,false);
        vga.drawText((fb_width-320)/2 + 1*8,(fb_height-200)/2+3*8,"fb_width: ",LIGHT_BLUE,BLUE,false);
        vga.drawText((fb_width-320)/2 + 1*8 + 10*8,(fb_height-200)/2+3*8,width,LIGHT_BLUE,BLUE,false);
        vga.drawText((fb_width-320)/2 + 1*8,(fb_height-200)/2+5*8,"fb_height: ",LIGHT_BLUE,BLUE,false);
        vga.drawText((fb_width-320)/2 + 1*8 + 10*8,(fb_height-200)/2+3*8,height,LIGHT_BLUE,BLUE,false);
    }
}