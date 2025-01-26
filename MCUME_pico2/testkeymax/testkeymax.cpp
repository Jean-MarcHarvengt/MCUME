#include "pico.h"
#include "pico/stdlib.h"
#include <stdio.h>
extern "C" {
  #include "iopins.h"  
  #include "emuapi.h"  
}
#include "keyboard_osd.h"
#include "pico_dsp.h"

#define BLUE       RGBVAL16(0, 0, 170)
#define LIGHT_BLUE RGBVAL16(0, 136, 255)

PICO_DSP tft;
static int fb_width, fb_height;


#include "hardware/clocks.h"
#include "hardware/vreg.h"

static const char * digits = "0123456789ABCDEF";
static int hk = 0;
static int prevhk = 0;
static int col=0;
static int row=0;


void emu_Input(uint16_t bClick) {
    hk = emu_ReadI2CKeyboard();
}

bool repeating_timer_callback(struct repeating_timer *t) {
    uint16_t bClick = emu_DebounceLocalKeys();
    emu_Input(bClick);    
    return true;
}

//extern "C" int sd_init_driver(void);
//#include <ff.h>
//#include <diskio.h>
//#include "tf_card.h"

int main(void) {
//    vreg_set_voltage(VREG_VOLTAGE_1_05);
//    set_sys_clock_khz(125000, true);    
//    set_sys_clock_khz(150000, true);    
//    set_sys_clock_khz(133000, true);    
//    set_sys_clock_khz(200000, true);    
//    set_sys_clock_khz(210000, true);    
////    set_sys_clock_khz(230000, true);    
//    set_sys_clock_khz(225000, true);    
//    set_sys_clock_khz(250000, true);  

    set_sys_clock_khz(120000, true);

//    set_sys_clock_khz(240000, true);
//    *((uint32_t *)(0x40010000+0x58)) = 2 << 16; //CLK_HSTX_DIV = 2 << 16; // HSTX clock/2


 /*
    pico_fatfs_spi_config_t config = {
        spi0,
        CLK_SLOW_DEFAULT,
        CLK_FAST_DEFAULT,
        SD_MISO,
        SD_CS,
        SD_SCLK,
        SD_MOSI,
        true  // use internal pullup
    };
    pico_fatfs_set_config(&config);
*/
/*
    stdio_init_all();
    FATFS fatfs;
    FRESULT fr;
    for (int i = 0; i < 5; i++) {
        fr = f_mount(&fatfs, "", 1);
        if (fr == FR_OK) { break; }
        printf("mount error %d -> retry %d\n", fr, i);
        pico_fatfs_reboot_spi();
    }
*/
    emu_init();

#ifdef FILEBROWSER
    while (true) {    
        if (menuActive()) {
            uint16_t bClick = emu_DebounceLocalKeys();
            int action = handleMenu(bClick);
            char * filename = menuSelection();   
            if (action == ACTION_RUN) {
              break;  
            }  
            tft.waitSync();
        }
    }    
#endif
    emu_start();
	tft.startRefresh();
    struct repeating_timer timer;
    add_repeating_timer_ms(20, repeating_timer_callback, NULL, &timer);      

	tft.fillScreen(LIGHT_BLUE);
	tft.get_frame_buffer_size(&fb_width, &fb_height);
	tft.drawRect((fb_width-320)/2,(fb_height-200)/2, 320,200, BLUE);

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
	tft.drawText(8,8,buf,BLUE,LIGHT_BLUE,false);

    while (true) {
        uint16_t bClick = emu_GetPad();
        char buf[5] = {0,0,0,0,0};
        buf[0] = digits[(bClick>>12)&0xf];
        buf[1] = digits[(bClick>>8)&0xf];
        buf[2] = digits[(bClick>>4)&0xf];
        buf[3] = digits[bClick&0xf];
        tft.drawText(4*8,0,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),false);
        buf[3] = 0;
        int key = emu_ReadI2CKeyboard();
        buf[0] = digits[(key>>8)&0xf];
        buf[1] = digits[(key>>4)&0xf];
        buf[2] = digits[key&0xf];        
        tft.drawText(4*8,8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),false);

        buf[2] = 0;
        key = emu_ReadI2CKeyboard2(0);
        buf[0] = digits[(key>>4)&0xf];
        buf[1] = digits[key&0xf];
        tft.drawText(9*8+0*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
        key = emu_ReadI2CKeyboard2(1);
        buf[0] = digits[(key>>4)&0xf];
        buf[1] = digits[key&0xf];
        tft.drawText(9*8+1*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
        key = emu_ReadI2CKeyboard2(2);
        buf[0] = digits[(key>>4)&0xf];
        buf[1] = digits[key&0xf];
        tft.drawText(9*8+2*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
        key = emu_ReadI2CKeyboard2(3);
        buf[0] = digits[(key>>4)&0xf];
        buf[1] = digits[key&0xf];
        tft.drawText(9*8+3*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
        key = emu_ReadI2CKeyboard2(4);
        buf[0] = digits[(key>>4)&0xf];
        buf[1] = digits[key&0xf];
        tft.drawText(9*8+4*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
        key = emu_ReadI2CKeyboard2(5);
        buf[0] = digits[(key>>4)&0xf];
        buf[1] = digits[key&0xf];
        tft.drawText(9*8+5*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
#ifdef PICOZX        
        key = emu_ReadI2CKeyboard2(6);
        buf[0] = digits[(key>>4)&0xf];
        buf[1] = digits[key&0xf];
        tft.drawText(9*8+6*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
#endif
        if ( (hk != 0) && (hk < 128) ) {
            buf[0] = (char)(hk&0xff);  
            buf[1] = 0;
            tft.drawText(col*8,(row+3)*8,buf,LIGHT_BLUE,BLUE,false);
            col += 1;
            if (col >= 40) {
                col=0;
                row += 1;
                if (row >= 25) {
                    row=0;  
                }
            }
            if (hk != prevhk) {
                sleep_ms(200);
            }
            else {
                sleep_ms(100);
            }
        }
        prevhk = hk;
        sleep_ms(20);
    }
}

void emu_KeyboardOnDown(int keymodifer, int key) {
}

void emu_KeyboardOnUp(int keymodifer, int key) {
}




