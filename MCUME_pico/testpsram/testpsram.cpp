#include "pico.h"
#include "pico/stdlib.h"

extern "C" {
  #include "iopins.h"  
  #include "emuapi.h"  
}

#include <stdio.h>
#include "psram_t.h"

#ifdef USE_VGA
#include "vga_t_dma.h"
#else
#include "tft_t_dma.h"
#endif


TFT_T_DMA tft;

#define BLUE       RGBVAL16(0, 0, 170)
#define LIGHT_BLUE RGBVAL16(0, 136, 255)

static int fb_width, fb_height;


#include "hardware/clocks.h"
#include "hardware/vreg.h"

static const char * digits = "0123456789ABCDEF";
static int hk = 0;

void emu_Input(uint16_t bClick) {
    hk = emu_ReadI2CKeyboard();
}

bool repeating_timer_callback(struct repeating_timer *t) {
    uint16_t bClick = emu_DebounceLocalKeys();
    emu_Input(bClick);    
    return true;
}


PSRAM_T psram = PSRAM_T(PSRAM_CS, PSRAM_MOSI, PSRAM_SCLK, PSRAM_MISO);

extern void  write_loram(int address, uint8_t val) {
  psram.pswrite(address,val); 
}

extern uint8_t read_loram(int address) {
  return (psram.psread(address)); 
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
#ifdef USE_VGA    
//    tft.begin(VGA_MODE_400x240);
    tft.begin(VGA_MODE_320x240);
#else
    tft.begin();
#endif
    psram.begin();   
    emu_init();
    emu_start();
	tft.startDMA();
    struct repeating_timer timer;
    add_repeating_timer_ms(20, repeating_timer_callback, NULL, &timer);      
    
	tft.fillScreenNoDma(LIGHT_BLUE);
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
	tft.drawText(0,0,buf,BLUE,LIGHT_BLUE,false);
    int addr = 0;
    while (true) {
        char buf[5] = {0,0,0,0,0};
        buf[0] = digits[(addr>>12)&0xf];
        buf[1] = digits[(addr>>8)&0xf];
        buf[2] = digits[(addr>>4)&0xf];
        buf[3] = digits[addr&0xf];
        buf[4] = 0;
        tft.drawText(8,64+0,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),false);

        for (int i=0; i<0x10; i++) {
            write_loram(addr,i);
            uint8_t val = 0xff;
            val = read_loram(addr);
            buf[0] = digits[(val>>4)&0xf];
            buf[1] = digits[val&0xf];
            buf[2] = 0;
            tft.drawText(8,64+16+i*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),false);
            addr += 1;
            addr &= 0x1fff;
        }

        sleep_ms(100);
    }
}






