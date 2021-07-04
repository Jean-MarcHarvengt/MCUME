#include "pico.h"
#include "pico/stdlib.h"
#include <stdio.h>

#ifdef USE_VGA
#include "vga_t_dma.h"
#else
#include "tft_t_dma.h"
#endif

extern "C" {
  #include "iopins.h"  
  #include "emuapi.h"  
}

TFT_T_DMA tft;

#define BLUE       RGBVAL16(0, 0, 170)
#define LIGHT_BLUE RGBVAL16(0, 136, 255)

static int fb_width, fb_height;


#include "hardware/clocks.h"
#include "hardware/vreg.h"

static const char * digits = "0123456789ABCDEF";

bool repeating_timer_callback(struct repeating_timer *t) {
    uint16_t bClick = emu_DebounceLocalKeys();
    //emu_Input(bClick);    
    return true;
}

int main(void) {
    vreg_set_voltage(VREG_VOLTAGE_1_05);
//    set_sys_clock_khz(125000, true);    
//    set_sys_clock_khz(150000, true);    
//    set_sys_clock_khz(133000, true);    
//    set_sys_clock_khz(200000, true);    
//    set_sys_clock_khz(225000, true);    
    set_sys_clock_khz(250000, true);    

    stdio_init_all();

    printf("start\n"); 
    tft.begin();  
    emu_init();
    emu_start();
	//tft.startDMA();
    struct repeating_timer timer;
    add_repeating_timer_ms(25, repeating_timer_callback, NULL, &timer);      
    
	tft.fillScreenNoDma(LIGHT_BLUE);
	tft.get_frame_buffer_size(&fb_width, &fb_height);
	tft.drawRectNoDma((fb_width-320)/2,(fb_height-200)/2, 320,200, BLUE);
	tft.drawTextNoDma((fb_width-320)/2,(fb_height-200)/2+1*8,"    **** COMMODORE 64 BASIC V2 ****     ",LIGHT_BLUE,BLUE,false);
	tft.drawTextNoDma((fb_width-320)/2,(fb_height-200)/2+3*8," 64K RAM SYSTEM  38911 BASIC BYTES FREE ",LIGHT_BLUE,BLUE,false);
	tft.drawTextNoDma((fb_width-320)/2,(fb_height-200)/2+5*8,"READY.",LIGHT_BLUE,BLUE,false);

    char buf[4] = {32,32,32,0};
	uint sys_clk = clock_get_hz(clk_sys)/1000000;
	uint r1 = sys_clk/100;
	uint r = sys_clk - r1*100;
	uint r2 = r/10;
	r = sys_clk - r1*100 - r2*10;
	uint r3 = r;
	buf[0] = digits[r1];
	buf[1] = digits[r2];
	buf[2] = digits[r3];
	tft.drawTextNoDma(4*8,0,buf,BLUE,LIGHT_BLUE,false);

    while (true) {
        uint16_t bClick = emu_GetPad();
        char buf[5] = {0,0,0,0,0};
        buf[0] = digits[(bClick>>12)&0xf];
        buf[1] = digits[(bClick>>8)&0xf];
        buf[2] = digits[(bClick>>4)&0xf];
        buf[3] = digits[bClick&0xf];
        tft.drawTextNoDma(4*8,16,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
        buf[3] = 0;
        int key = emu_ReadI2CKeyboard();
        buf[0] = digits[(key>>8)&0xf];
        buf[1] = digits[(key>>4)&0xf];
        buf[2] = digits[key&0xf];        
        tft.drawTextNoDma(4*8,16*2,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);

        buf[2] = 0;
        key = emu_ReadI2CKeyboard2(0);
        buf[0] = digits[(key>>4)&0xf];
        buf[1] = digits[key&0xf];
        tft.drawTextNoDma(4*8,16*4,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
        key = emu_ReadI2CKeyboard2(1);
        buf[0] = digits[(key>>4)&0xf];
        buf[1] = digits[key&0xf];
        tft.drawTextNoDma(4*8,16*5,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
        key = emu_ReadI2CKeyboard2(2);
        buf[0] = digits[(key>>4)&0xf];
        buf[1] = digits[key&0xf];
        tft.drawTextNoDma(4*8,16*6,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
        key = emu_ReadI2CKeyboard2(3);
        buf[0] = digits[(key>>4)&0xf];
        buf[1] = digits[key&0xf];
        tft.drawTextNoDma(4*8,16*7,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
        key = emu_ReadI2CKeyboard2(4);
        buf[0] = digits[(key>>4)&0xf];
        buf[1] = digits[key&0xf];
        tft.drawTextNoDma(4*8,16*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
        key = emu_ReadI2CKeyboard2(5);
        buf[0] = digits[(key>>4)&0xf];
        buf[1] = digits[key&0xf];
        tft.drawTextNoDma(4*8,16*9,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
        
        sleep_ms(20);
          	
/*
		sleep_ms(2000);	
		tft.stopDMA();
		tft.fillScreenNoDma( RGBVAL16(0xff,0x00,0x00) );
		sleep_ms(2000);	
		tft.startDMA();
		tft.fillScreen(LIGHT_BLUE);		
*/
    	
        //tft.waitSync();
		//tft.drawText((fb_width-320)/2,(fb_height-200)/2+6*8," ",BLUE,LIGHT_BLUE,false);
		//sleep_ms(500);
		//tft.waitSync();
		//tft.drawText((fb_width-320)/2,(fb_height-200)/2+6*8," ",BLUE,BLUE,false);
		//sleep_ms(500);
    }
}






