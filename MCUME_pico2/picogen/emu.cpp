#include <string.h>

#include "emuapi.h"
#include "iopins.h" 

#include <pico/stdlib.h>

/* Gwenesis Emulator */
extern "C" {
#include "gwenesis/cpus/M68K/m68k.h"
#include "gwenesis/sound/z80inst.h"
#include "gwenesis/bus/gwenesis_bus.h"
#include "gwenesis/io/gwenesis_io.h"
#include "gwenesis/vdp/gwenesis_vdp.h"
#include "gwenesis/savestate/gwenesis_savestate.h"
#include <gwenesis/sound/gwenesis_sn76489.h>
#include <gwenesis/sound/ym2612.h>
}

#include "flash_t.h"

// SETTINGS
bool show_fps = true;
bool limit_fps = true;
bool interlace = true;
bool frameskip = true;
bool flash_line = false;
bool flash_frame = false;
int z80_enable_mode = 2;
bool sn76489_enabled = true;
//int z80_enable_mode = 0;
//bool sn76489_enabled = false;


uint8_t snd_accurate = 0;
/* shared variables with gwenesis_sn76589 */
int16_t gwenesis_sn76489_buffer[GWENESIS_AUDIO_BUFFER_LENGTH_NTSC * 2];  // 888 = NTSC, PAL = 1056 (too big) //GWENESIS_AUDIO_BUFFER_LENGTH_PAL];
int sn76489_index;                                                      /* sn78649 audio buffer index */
int sn76489_clock;                                                      /* sn78649 clock in video clock resolution */


int audio_enabled = 1;
int snd_output_volume = 9;

void gwenesis_io_get_buttons() {
}

void emu_KeyboardOnDown(int keymodifer, int key) {
}

void emu_KeyboardOnUp(int keymodifer, int key) {
}


void gen_Init(void)
{
  emu_printf("gen_Init");  
  emu_printf("gen_Init done");
}


static int hk = 0;
static int k = 0;


void gen_Input(int click) {
  hk = emu_ReadI2CKeyboard();
  k = emu_ReadKeys();    
}

extern void * emu_LineBuffer(int line);

void gen_Start(char * filename)
{
  emu_printf("gen_Start");

  int size = flash_load(filename);

  load_cartridge((uintptr_t)flash_start);
  power_on();
  reset_emulation();
  gwenesis_vdp_set_buffer((uint8_t *) emu_LineBuffer(0));

#ifdef SOUND_PRESENT
#ifdef HAS_SND  
  emu_sndInit();
#endif  
#else
#endif

  emu_printf("gen_Start done");
}

int start_time = 0;
int frame, frame_cnt = 0;
int frame_timer_start = 0;

/* Clocks and synchronization */
/* system clock is video clock */
int system_clock;

unsigned int lines_per_frame = LINES_PER_FRAME_NTSC; //262; /* NTSC: 262, PAL: 313 */
int scan_line;
unsigned int frame_counter = 0;
unsigned int drawFrame = 1;

extern unsigned char gwenesis_vdp_regs[0x20];
extern unsigned int gwenesis_vdp_status;
extern unsigned int screen_width, screen_height;
extern int hint_pending;

void gen_Step(void) {
        /* Eumulator loop */
        int hint_counter = gwenesis_vdp_regs[10];

        const bool is_pal = REG1_PAL;
        screen_width = REG12_MODE_H40 ? 320 : 256;
        screen_height = is_pal ? 240 : 224;
        lines_per_frame = is_pal ? LINES_PER_FRAME_PAL : LINES_PER_FRAME_NTSC;

        // graphics_set_buffer(buffer, screen_width, screen_height);
        // TODO: move to separate function graphics_set_dimensions ?
//        graphics_set_buffer((uint8_t*)SCREEN, screen_width, screen_height);
//        graphics_set_offset(screen_width != 320 ? 32 : 0, screen_height != 240 ? 8 : 0);
        gwenesis_vdp_render_config();

        zclk = 0;
        /* Reset the difference clocks and audio index */
        system_clock = 0;
        sn76489_clock = 0;
        sn76489_index = 0;
        scan_line = 0;
         if (z80_enable_mode == 1)
            z80_run(lines_per_frame * VDP_CYCLES_PER_LINE);
  
        while (scan_line < lines_per_frame) {
            /* CPUs */
            m68k_run(system_clock + VDP_CYCLES_PER_LINE);
            if (z80_enable_mode == 2)
                    z80_run(system_clock + VDP_CYCLES_PER_LINE);
            /* Video */
            // Interlace mode
            if (drawFrame && !interlace || (frame % 2 == 0 && scan_line % 2) || scan_line % 2 == 0) {
                if (scan_line < 200)
                gwenesis_vdp_render_line(scan_line); /* render scan_line */
            }

            // On these lines, the line counter interrupt is reloaded
            if (scan_line == 0 || scan_line > screen_height) {
                hint_counter = REG10_LINE_COUNTER;
            }

            // interrupt line counter
            if (--hint_counter < 0) {
                if (REG0_LINE_INTERRUPT != 0 && scan_line <= screen_height) {
                    hint_pending = 1;
                    if ((gwenesis_vdp_status & STATUS_VIRQPENDING) == 0)
                        m68k_update_irq(4);
                }
                hint_counter = REG10_LINE_COUNTER;
            }

            scan_line++;

            // vblank begin at the end of last rendered line
            if (scan_line == screen_height) {
                if (REG1_VBLANK_INTERRUPT != 0) {
                    gwenesis_vdp_status |= STATUS_VIRQPENDING;
                    m68k_set_irq(6);
                }
                z80_irq_line(1);
            }

            if (!is_pal && scan_line == screen_height + 1) {
                z80_irq_line(0);
                // FRAMESKIP every 3rd frame
                drawFrame = frameskip && frame % 3 != 0;
                // if (frameskip && frame % 3 == 0) {
                //     drawFrame = 0;
                // } else {
                //     drawFrame = 1;
                // }
            }

            system_clock += VDP_CYCLES_PER_LINE;


        }
/*
        frame++;
        if (limit_fps) {
            frame_cnt++;
            if (frame_cnt == (is_pal ? 5 : 6)) {
                while (time_us_64() - frame_timer_start < (is_pal ? 20000 * 5 : 16666 * 6)) {
                    busy_wait_at_least_cycles(10);
                }; // 60 Hz
                frame_timer_start = time_us_64();
                frame_cnt = 0;
            }
        }
*/
#if HDMI | SOFTTV | TV
//        if (audio_enabled)
//            gwenesis_SN76489_run(REG1_PAL ? LINES_PER_FRAME_PAL : LINES_PER_FRAME_NTSC * VDP_CYCLES_PER_LINE);
#endif
        // ym2612_run(262 * VDP_CYCLES_PER_LINE);
        /*
        gwenesis_SN76489_run(262 * VDP_CYCLES_PER_LINE);
        ym2612_run(262 * VDP_CYCLES_PER_LINE);
        static int16_t snd_buf[GWENESIS_AUDIO_BUFFER_LENGTH_NTSC * 2];
        for (int h = 0; h < ym2612_index * 2 * GWENESIS_AUDIO_SAMPLING_DIVISOR; h++) {
            snd_buf[h] = (gwenesis_sn76489_buffer[h / 2 / GWENESIS_AUDIO_SAMPLING_DIVISOR]) << 3;
        }
        i2s_dma_write(&i2s_config, snd_buf);*/
        // reset m68k cycles to the begin of next frame cycle
        m68k.cycles -= system_clock;

        /* copy audio samples for DMA */
        //gwenesis_sound_submit();


/*
  uint32_t buttons = 0;
  if (( k & MASK_JOY1_RIGHT) || ( k & MASK_JOY2_RIGHT)) {
    buttons |= JOY_LEFT;
  }
  if (( k & MASK_JOY1_LEFT) || ( k & MASK_JOY2_LEFT)) {
    buttons |= JOY_RIGHT;
  }
  if (( k & MASK_JOY1_UP) || ( k & MASK_JOY2_UP)) {
    buttons |= JOY_UP;
  }
  if (( k & MASK_JOY1_DOWN) || ( k & MASK_JOY2_DOWN)) {
    buttons |= JOY_DOWN;
  }  
  if ( k & MASK_JOY2_BTN) {
    buttons |= JOY_A;
  }

  if (k & MASK_KEY_USER1)  buttons |= JOY_B;
  //if (k & MASK_KEY_USER3)  buttons |= JOY_SELECT;
  if (k & MASK_KEY_USER2)  buttons |= JOY_RUN;
*/          

  //psg_update((int16_t *) audio_buffer, AUDIO_BUFFER_LENGTH, 0xff);
  //i2s_dma_write(&i2s_config, (const int16_t *) audio_buffer);

  //emu_DrawVsync();   
}

void SND_Process(void *stream, int len) {
//  psg_update((int16*)stream, 0, len);  
} 
