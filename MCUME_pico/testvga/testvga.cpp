#include "pico.h"
#include "pico/stdlib.h"

#include "vga_t_dma.h"

#include <stdio.h>
#include <stdio.h>

TFT_T_DMA vga;

#define BLUE       VGA_RGB(0, 0, 170)
#define LIGHT_BLUE VGA_RGB(0, 136, 255)

static int fb_width, fb_height;

static char * digits = "0123456789";

#include "hardware/clocks.h"
#include "hardware/vreg.h"

static uint8_t pix = 0;
int main(void) {
    vreg_set_voltage(VREG_VOLTAGE_1_05);
//    set_sys_clock_khz(125000, true);    
//    set_sys_clock_khz(150000, true);    
//    set_sys_clock_khz(133000, true);    
//    set_sys_clock_khz(200000, true);    
//    set_sys_clock_khz(225000, true);    
    set_sys_clock_khz(252000, true);    


    stdio_init_all();

    printf("start\n");   

//    vga.begin(VGA_MODE_400x240);
    vga.begin(VGA_MODE_320x240);
    //tft.fillScreenNoDma( RGBVAL16(0xff,0x00,0x00) );
	vga.clear(LIGHT_BLUE);
	vga.get_frame_buffer_size(&fb_width, &fb_height);
	vga.drawRect((fb_width-320)/2,(fb_height-200)/2, 320,200, BLUE);
	vga.drawText((fb_width-320)/2,(fb_height-200)/2+1*8,"    **** COMMODORE 64 BASIC V2 ****     ",LIGHT_BLUE,BLUE,false);
	vga.drawText((fb_width-320)/2,(fb_height-200)/2+3*8," 64K RAM SYSTEM  38911 BASIC BYTES FREE ",LIGHT_BLUE,BLUE,false);
	vga.drawText((fb_width-320)/2,(fb_height-200)/2+5*8,"READY.",LIGHT_BLUE,BLUE,false);

	uint newclock = clock_get_hz(clk_sys)/1000000;
	
    char buf[4] = {32,32,32,0};
	uint r1 = newclock/100;
	uint r = newclock - r1*100;
	uint r2 = r/10;
	r = newclock - r1*100 - r2*10;
	uint r3 = r;
	buf[0] = digits[r1];
	buf[1] = digits[r2];
	buf[2] = digits[r3];
	vga.drawText(4*8,8,buf,BLUE,LIGHT_BLUE,false);


    while (true) {
        //tft.fillScreenNoDma( pix++ );
        vga.waitSync();
		vga.drawText((fb_width-320)/2,(fb_height-200)/2+6*8," ",BLUE,LIGHT_BLUE,false);
		sleep_ms(500);
		//vga.waitSync();
		vga.drawText((fb_width-320)/2,(fb_height-200)/2+6*8," ",BLUE,BLUE,false);
		sleep_ms(500);
		//vga.waitSync();
		printf("x\n");  
    }
}






