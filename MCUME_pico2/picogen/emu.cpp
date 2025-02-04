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
static int z80_enable_mode = 2;
bool sn76489_enabled = true;
extern unsigned short button_state[3];

/* shared variables with gwenesis_sn76589 */
int16_t gwenesis_sn76489_buffer[GWENESIS_AUDIO_BUFFER_LENGTH_NTSC * 2];  // 888 = NTSC, PAL = 1056 (too big) //GWENESIS_AUDIO_BUFFER_LENGTH_PAL];
int sn76489_index;                                                      /* sn78649 audio buffer index */
int sn76489_clock;                                                      /* sn78649 clock in video clock resolution */
int8_t gwenesis_ym2612_buffer[GWENESIS_AUDIO_BUFFER_LENGTH_NTSC * 2];  //GWENESIS_AUDIO_BUFFER_LENGTH_PAL];
int ym2612_index;                                                     /* ym2612 audio buffer index */
int ym2612_clock;

int audio_enabled = 1;
uint8_t snd_accurate = 0;


static unsigned short screen_line[320];


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

  int size = flash_load_bswap(filename);

  load_cartridge((uintptr_t)flash_start);

  //gwenesis_system_init();
  power_on();
  reset_emulation();
  gwenesis_vdp_set_buffer(&screen_line[0]);

#ifdef HAS_SND  
  emu_sndInit();   
  memset(gwenesis_sn76489_buffer, 0, sizeof(gwenesis_sn76489_buffer));
  memset(gwenesis_ym2612_buffer, 0, sizeof(gwenesis_ym2612_buffer));  
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
    int hint_counter = gwenesis_vdp_regs[10];

    const bool is_pal = REG1_PAL;
    screen_width = REG12_MODE_H40 ? 320 : 256;
    screen_height = is_pal ? 240 : 224;
    lines_per_frame = is_pal ? LINES_PER_FRAME_PAL : LINES_PER_FRAME_NTSC;

    gwenesis_vdp_render_config();

    zclk = 0;
    /* Reset the difference clocks and audio index */
    system_clock = 0;
    sn76489_clock = 0;
    sn76489_index = 0;
    ym2612_clock = 0;
    ym2612_index = 0;        
    scan_line = 0;
    if (z80_enable_mode == 1)
        z80_run(lines_per_frame * VDP_CYCLES_PER_LINE);

    //printf("m(%x)\n", frame);
    while (scan_line < lines_per_frame) {
        /* CPUs */
        m68k_run(system_clock + VDP_CYCLES_PER_LINE);
        if (z80_enable_mode == 2)
                z80_run(system_clock + VDP_CYCLES_PER_LINE);
        /* Video */
        // Interlace mode
        //if (drawFrame && !interlace || (frame % 2 == 0 && scan_line % 2) || scan_line % 2 == 0) {
        if (drawFrame)  {
            gwenesis_vdp_set_buffer(&screen_line[0]);
            gwenesis_vdp_render_line(scan_line); /* render scan_line */
            if (scan_line < screen_height ) emu_DrawLine16(&screen_line[0], 320, screen_height, scan_line);
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
            //drawFrame = frameskip && frame % 3 != 0;
            // if (frameskip && frame % 3 == 0) {
            //     drawFrame = 0;
            // } else {
            //     drawFrame = 1;
            // }
        }

        system_clock += VDP_CYCLES_PER_LINE;
    }
    frame++;
    /*
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

#ifdef HAS_SND_SYNC
    if (audio_enabled) {
        ym2612_run(262 * VDP_CYCLES_PER_LINE);
        gwenesis_SN76489_run(262 * VDP_CYCLES_PER_LINE);
        audio_sample * snd_buf =  (audio_sample *)emu_sndGetBuffer();
        for (int h = 0; h < GWENESIS_AUDIO_BUFFER_LENGTH_NTSC * 2; h++) {         
        int16_t s1 = gwenesis_ym2612_buffer[h/ 2 / GWENESIS_AUDIO_SAMPLING_DIVISOR] + gwenesis_sn76489_buffer[h/ 2 / GWENESIS_AUDIO_SAMPLING_DIVISOR]>>8; 
        int16_t s2 = gwenesis_ym2612_buffer[(h/ 2 / GWENESIS_AUDIO_SAMPLING_DIVISOR+1) ] + gwenesis_sn76489_buffer[(h/ 2 / GWENESIS_AUDIO_SAMPLING_DIVISOR+1)]>>8;
        *snd_buf++ = ((s1+s2)/4)+128;
        }
    }     
#endif

    // reset m68k cycles to the begin of next frame cycle
    m68k.cycles -= system_clock;


  button_state[0]=0;
  if (( k & MASK_JOY1_RIGHT) || ( k & MASK_JOY2_RIGHT)) {
    button_state[0] |= (1 << PAD_LEFT);
  }
  if (( k & MASK_JOY1_LEFT) || ( k & MASK_JOY2_LEFT)) {
    button_state[0] |= (1 << PAD_RIGHT);
  }
  if (( k & MASK_JOY1_UP) || ( k & MASK_JOY2_UP)) {
    button_state[0] |= (1 << PAD_UP);
  }
  if (( k & MASK_JOY1_DOWN) || ( k & MASK_JOY2_DOWN)) {
    button_state[0] |= (1 << PAD_DOWN);
  }  
  if ( k & MASK_JOY2_BTN) {
    button_state[0] |= (1 << PAD_B);
  }
  if ( k & MASK_KEY_USER3) {
    button_state[0] |= (1 << PAD_C);
  }  
  if ( k & MASK_KEY_USER4) {
    button_state[0] |= (1 << PAD_A);
  }

  if ( k & MASK_KEY_USER1) {
    button_state[0] |= (1 << PAD_S);
  }

  button_state[0] = ~ button_state[0];

  //emu_DrawVsync();   
}


void SND_Process(void *stream, int len) {
    if (audio_enabled) {
        //if ( (z80_enable_mode == 3) && (system_clock == 0) )
        //    z80_run(262 * VDP_CYCLES_PER_LINE);
        ym2612_run(262 * VDP_CYCLES_PER_LINE);
        gwenesis_SN76489_run(262 * VDP_CYCLES_PER_LINE);
        audio_sample * snd_buf =  (audio_sample *)stream;
        for (int h = 0; h < len*2; h += 2) {            
            int16_t s1 = gwenesis_ym2612_buffer[h/ 2 / GWENESIS_AUDIO_SAMPLING_DIVISOR] + gwenesis_sn76489_buffer[h/ 2 / GWENESIS_AUDIO_SAMPLING_DIVISOR]>>8; 
            int16_t s2 = gwenesis_ym2612_buffer[(h/ 2 / GWENESIS_AUDIO_SAMPLING_DIVISOR+1) ] + gwenesis_sn76489_buffer[(h/ 2 / GWENESIS_AUDIO_SAMPLING_DIVISOR+1)]>>8;
            *snd_buf++ = ((s1+s2)/4)+128;
        }
    }
} 
