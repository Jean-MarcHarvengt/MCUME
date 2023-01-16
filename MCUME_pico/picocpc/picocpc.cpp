#include "pico.h"
#include "pico/stdlib.h"

#include "vga_t_dma.h"

#include <stdio.h>

TFT_T_DMA vga;

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

    while (true) {
        vga.waitSync();
        sleep_ms(1000);
        vga.clear(LIGHT_GREEN);
        vga.drawRect((fb_width-320)/2, (fb_height-200), 320, 200, GREEN);
        sleep_ms(1000);
        vga.clear(LIGHT_BLUE);
        vga.drawRect((fb_width-320)/2, (fb_height-200), 320, 200, BLUE);
    }
}