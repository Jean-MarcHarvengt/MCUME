#include "pico.h"
#include "pico/stdlib.h"

extern "C" {
  #include "iopins.h"  
  #include "emuapi.h"  
}
#include "keyboard_osd.h"
#include "vga_t_dma.h"

#include <stdio.h>

TFT_T_DMA tft;
static uint8_t sector_data[1024];


int main(void) {
//    set_sys_clock_48mhz();    
    stdio_init_all();

    printf("start\n");   
    tft.begin(VGA_MODE_320x240);
    emu_init();
    emu_sndInit();    
    while (true) {
        //printf("Hello, world!\n");
        if (menuActive()) {
            uint16_t bClick = emu_DebounceLocalKeys();
            int action = handleMenu(bClick);
            char * filename = menuSelection();   
            if (action == ACTION_RUNTFT) {
              toggleMenu(false);
              //emu_start();        
              //emu_Init(filename);      
              tft.fillScreenNoDma( RGBVAL16(0x00,0x00,0x00) );
              tft.startDMA();      
            }  
            tft.waitSync();
        }
        else {  
            //emu_Step();   
            uint16_t bClick = emu_DebounceLocalKeys();
            emu_Input(bClick);      
        }
        int c = getchar_timeout_us(0);
        switch (c) {
            case ' ':
                printf("Inverted: %d\n", 1);
                break;
        }
    }
}


#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "rock.h"

#define AUDIO_PIN 28


static int cur_sample = 0;

void pwm_irh() {
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));
    pwm_set_gpio_level(AUDIO_PIN, audio_buffer[cur_sample >> 2]);

    uint8_t sample = audio_buffer[cur_sample >> 2];

    if (cur_sample < (AUDIO_SAMPLES * 4) - 1) {
        ++cur_sample;
    } else {
        cur_sample = 0;
    }
}

void emu_sndInit() {
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);

    int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);

    // Setup PWM interrupt to fire when PWM cycle is complete
    pwm_clear_irq(audio_pin_slice);
    pwm_set_irq_enabled(audio_pin_slice, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_irh);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    // Setup PWM for audio output
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 5.5f);
    pwm_config_set_wrap(&config, 254);
    pwm_init(audio_pin_slice, &config, true);

    pwm_set_gpio_level(AUDIO_PIN, 0);
}

void emu_sndPlaySound(int chan, int volume, int freq)
{
}





