/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma GCC push_options

#if !PICO_SCANVIDEO_DEBUG_IMPL
#undef PARAM_ASSERTIONS_DISABLE_ALL
#define PARAM_ASSERTIONS_DISABLE_ALL 1
#pragma GCC optimize("O3")
#endif

#include <stdlib.h>
#include <stdio.h>
#include "pico/platform.h"
#include "pico/sem.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "timing.pio.h"
#include "scanvideo.h"
#include "composable_scanline.h"
#include "hardware/structs/bus_ctrl.h"
#include "pico/binary_info.h"

#include "iopins.h"

#ifdef USE_VGA

// PICO_CONFIG: PICO_SCANVIDEO_ADJUST_BUS_PRIORITY, Enable/disable adjust bus priority, type=bool, default=0, group=video
#ifndef PICO_SCANVIDEO_ADJUST_BUS_PRIORITY
#define PICO_SCANVIDEO_ADJUST_BUS_PRIORITY 0
#endif


// PICO_CONFIG: PICO_SCANVIDEO_DEBUG_IMPL, Enable/disable debug implementation, type=bool, default=0, group=video
#ifndef PICO_SCANVIDEO_DEBUG_IMPL
#define PICO_SCANVIDEO_DEBUG_IMPL 0
#endif

#define PICO_SCANVIDEO_SCANLINE_SM 0u
#define PICO_SCANVIDEO_SCANLINE_DMA_CHANNEL 0u

#define PICO_SCANVIDEO_TIMING_SM 3u

#define PICO_SCANVIDEO_SCANLINE_DMA_CHANNELS_MASK (1u << PICO_SCANVIDEO_SCANLINE_DMA_CHANNEL)


extern uint32_t * pio_fb;
extern int pio_fbwidth;


const scanvideo_timing_t vga_timing_640x480_60_default =
        {
                .clock_freq = 25000000,

                .h_active = 640,
                .v_active = 480,

                .h_front_porch = 16,
                .h_pulse = 64,
                .h_total = 800,
                .h_sync_polarity = 1,

                .v_front_porch = 1,
                .v_pulse = 2,
                .v_total = 523,
                .v_sync_polarity = 1,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 0
        };

const scanvideo_timing_t vga_timing_wide_480_50 =
        {
                .clock_freq = 24000000, //24000000,

                .h_active = 800,
                .v_active = 480,

                .h_front_porch = 32, //12,
                .h_pulse = 48,
                .h_total = 960,
                .h_sync_polarity = 0,

                .v_front_porch = 1,
                .v_pulse = 2,
                .v_total = 500,
                .v_sync_polarity = 0,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 0
        };

const scanvideo_mode_t vga_mode_320x240_60 =
        {
                .default_timing = &vga_timing_640x480_60_default,
                .pio_program = &video_24mhz_composable,
                .width = 320,
                .height = 240,
                .xscale = 2,
                .yscale = 2,
        };


const scanvideo_mode_t vga_mode_tft_400x240_50 =
        {
                .default_timing = &vga_timing_wide_480_50,
                .pio_program = &video_24mhz_composable,
                .width = 400,
                .height = 240,
                .xscale = 2,
                .yscale = 2,
        };

#define scanline_assert(x) (void)0

#define video_pio pio0

#if PICO_SCANVIDEO_ADJUST_BUS_PRIORITY
#define VIDEO_ADJUST_BUS_PRIORITY_VAL (BUSCTRL_BUS_PRIORITY_PROC0_BITS | BUSCTRL_BUS_PRIORITY_PROC1_BITS)
#endif

#ifdef VIDEO_TIME_CRITICAL_CODE_SECTION
#define __video_time_critical_func(x) __attribute__((section(__XSTRING(VIDEO_TIME_CRITICAL_CODE_SECTION) "." x))) x
#else
#define __video_time_critical_func(x) __not_in_flash_func(x)
#endif
// --- video_24mhz_composable ---

#define video_24mhz_composable_program __CONCAT(video_24mhz_composable_prefix, _program)
#define video_24mhz_composable_wrap_target __CONCAT(video_24mhz_composable_prefix, _wrap_target)
#define video_24mhz_composable_wrap __CONCAT(video_24mhz_composable_prefix, _wrap)

bool video_24mhz_composable_adapt_for_mode(const scanvideo_pio_program_t *program, const scanvideo_mode_t *mode,
                                           scanvideo_scanline_buffer_t *missing_scanline_buffer,
                                           uint16_t *modifiable_instructions);

pio_sm_config video_24mhz_composable_configure_pio(pio_hw_t *pio, uint sm, uint offset);

const scanvideo_pio_program_t video_24mhz_composable = {
        .program = &video_24mhz_composable_program,
        .entry_point = video_24mhz_composable_program_extern(entry_point),
        .adapt_for_mode = video_24mhz_composable_adapt_for_mode,
        .configure_pio = video_24mhz_composable_configure_pio
};

static uint8_t video_htiming_load_offset;
static uint8_t video_program_load_offset;

// --- video timing stuff

// 4 possible instructions; index into program below
enum {
    SET_IRQ_0 = 0u,
    SET_IRQ_1 = 1u,
    SET_IRQ_SCANLINE = 2u,
    CLEAR_IRQ_SCANLINE = 3u,
};

static struct {
    int32_t v_active;
    int32_t v_total;
    int32_t v_pulse_start;
    int32_t v_pulse_end;
    // todo replace with plain polarity
    uint32_t vsync_bits_pulse;
    uint32_t vsync_bits_no_pulse;

    uint32_t a, a_vblank, b1, b2, c, c_vblank;
    uint32_t vsync_bits;
    uint16_t dma_state_index;
    int32_t timing_scanline;
} timing_state;

#define DMA_STATE_COUNT 4
static uint32_t dma_states[DMA_STATE_COUNT];

// todo get rid of this altogether
#undef PICO_SCANVIDEO_ENABLE_VIDEO_CLOCK_DOWN
#define PICO_SCANVIDEO_ENABLE_VIDEO_CLOCK_DOWN 1

#if PICO_SCANVIDEO_ENABLE_VIDEO_CLOCK_DOWN
static uint16_t video_clock_down_times_2;
#endif

semaphore_t vblank_begin;

// --- scanline stuff
// private representation of scanline buffer (adds link for one list this scanline buffer is currently in)
typedef struct full_scanline_buffer {
    scanvideo_scanline_buffer_t core;
    struct full_scanline_buffer *next;
} full_scanline_buffer_t;


// This state is sensitive as it it accessed by either core, and multiple IRQ handlers which may be re-entrant
// Nothing in here should be touched except when protected by the appropriate spin lock.
//
// The separations by spin lock (other than the need for spin locks to protect state consistency) is to allow
// safe concurrent operation by both cores, client, IRQ and nested IRQ (pre-emption) where desirable due
// to timing concerns.
static struct {
    struct {
        full_scanline_buffer_t *current_scanline_buffer;
        uint32_t last_scanline_id;
        uint32_t next_scanline_id;
        // 0 based index of y repeat... goes 0, 0, 0 in non scaled mode, 0, 1, 0, 1 in doubled etc.
        uint16_t y_repeat_index;
        uint16_t y_repeat_target;
        bool in_vblank;
    } scanline;


    // these are not updated, so not locked
} shared_state;



static full_scanline_buffer_t scanline_buffer;

static void prepare_for_active_scanline_irqs_enabled();

static void scanline_dma_complete_irqs_enabled();

static void setup_sm(int sm, uint offset);

// -- MISC stuff
static scanvideo_mode_t video_mode;
static bool video_timing_enabled = false;
static bool display_enabled = true;

static scanvideo_scanline_repeat_count_fn _scanline_repeat_count_fn;



static inline uint32_t scanline_id_after(uint32_t scanline_id) {
    uint32_t tmp = scanline_id & 0xffffu;

    if (tmp < video_mode.height - 1) {
        return scanline_id + 1;
    } else {
        return scanline_id + 0x10000u - tmp;
    }
}



static void set_next_scanline_id(uint32_t scanline_id) {
    shared_state.scanline.next_scanline_id = scanline_id;
    shared_state.scanline.y_repeat_target = _scanline_repeat_count_fn(scanline_id) * video_mode.yscale;
}

void __video_time_critical_func(prepare_for_active_scanline_irqs_enabled)() {
    full_scanline_buffer_t *fsb = &scanline_buffer;
    uint16_t scanline = shared_state.scanline.next_scanline_id & 0xffffu; 
    if (scanline < 240)
        fsb->core.data = &pio_fb[(pio_fbwidth+1)*scanline];
    else 
        fsb->core.data = &pio_fb[0];
    dma_channel_transfer_from_buffer_now(PICO_SCANVIDEO_SCANLINE_DMA_CHANNEL, fsb->core.data,
                                         (uint32_t) fsb->core.data_used);
    shared_state.scanline.in_vblank = false;
    shared_state.scanline.y_repeat_index += video_mode.yscale_denominator;
    if (shared_state.scanline.y_repeat_index >= shared_state.scanline.y_repeat_target) {
        shared_state.scanline.y_repeat_index -= shared_state.scanline.y_repeat_target;
        set_next_scanline_id(scanline_id_after(shared_state.scanline.next_scanline_id));
        shared_state.scanline.current_scanline_buffer = NULL;
    } else /*if (!was_correct_scanline)*/ {
        // not at the the end of yscale, but the wrong (or missing) scanline anyway, so clear that
        shared_state.scanline.current_scanline_buffer = NULL;
    }
}

static void __video_time_critical_func(prepare_for_vblank_scanline_irqs_enabled)() {
    bool signal = false;
    if (!shared_state.scanline.in_vblank) {
        shared_state.scanline.in_vblank = true;
        shared_state.scanline.y_repeat_index = 0;

        // generally this should already have wrapped, but may not have just after a sync
        if (scanvideo_scanline_number(shared_state.scanline.next_scanline_id) != 0) {
            // set up for scanline 0 of the next frame when we come out of vblank
            shared_state.scanline.next_scanline_id =
                    (scanvideo_frame_number(shared_state.scanline.next_scanline_id) + 1u) << 16u;
            shared_state.scanline.y_repeat_target = _scanline_repeat_count_fn(shared_state.scanline.next_scanline_id);
        }
        signal = true;
    }
    if (signal) {
        sem_release(&vblank_begin);
    }
}

#define setup_dma_states_vblank() if (true) { dma_states[0] = timing_state.a_vblank; dma_states[1] = timing_state.b1; dma_states[2] = timing_state.b2; dma_states[3] = timing_state.c_vblank; } else __builtin_unreachable()
#define setup_dma_states_no_vblank() if (true) { dma_states[0] = timing_state.a; dma_states[1] = timing_state.b1; dma_states[2] = timing_state.b2; dma_states[3] = timing_state.c; } else __builtin_unreachable()

static inline void top_up_timing_pio_fifo() {
    // todo better irq reset ... we are seeing irq get set again, handled in this loop, then we re-enter here when we don't need to
    // keep filling until SM3 TX is full
    while (!(video_pio->fstat & (1u << (PICO_SCANVIDEO_TIMING_SM + PIO_FSTAT_TXFULL_LSB)))) {
        pio_sm_put(video_pio, PICO_SCANVIDEO_TIMING_SM, dma_states[timing_state.dma_state_index] | timing_state.vsync_bits);
        // todo simplify this now we have a1, a2, b, c
        // todo display enable (only goes positive on start of screen)

        // todo right now we are fixed... make this generic for timing and improve
        if (++timing_state.dma_state_index >= DMA_STATE_COUNT) {
            timing_state.dma_state_index = 0;
            timing_state.timing_scanline++;

            // todo check code and put these in a current state struct
            if (timing_state.timing_scanline >= timing_state.v_active) {
                if (timing_state.timing_scanline >= timing_state.v_total) {
                    timing_state.timing_scanline = 0;
                    // active display - gives irq 0 and irq 4
                    setup_dma_states_no_vblank();
                } else if (timing_state.timing_scanline <= timing_state.v_pulse_end) {
                    if (timing_state.timing_scanline == timing_state.v_active) {
                        setup_dma_states_vblank();
                    } else if (timing_state.timing_scanline == timing_state.v_pulse_start) {
                        timing_state.vsync_bits = timing_state.vsync_bits_pulse;
                    } else if (timing_state.timing_scanline == timing_state.v_pulse_end) {
                        timing_state.vsync_bits = timing_state.vsync_bits_no_pulse;
                    }
                }
            }
        }
    }
}

void __isr __video_time_critical_func(isr_pio0_0)() {
#if PICO_SCANVIDEO_ADJUST_BUS_PRIORITY
    bus_ctrl_hw->priority = VIDEO_ADJUST_BUS_PRIORITY_VAL;
#endif

    // handler for explicit PIO_IRQ0 from PICO_SCANVIDEO_TIMING_SM at a good time to start a DMA for a scanline
    // this called once per scanline during non vblank
    if (video_pio->irq & 1u) {
        video_pio->irq = 1;
        if (display_enabled) {
            prepare_for_active_scanline_irqs_enabled();
        }
    }
#if PICO_SCANVIDEO_ADJUST_BUS_PRIORITY
    bus_ctrl_hw->priority = 0;
#endif
    // handler for explicit PIO_IRQ1 from PICO_SCANVIDEO_TIMING_SM at a good time to prepare for a scanline
    // this is only called once per scanline during vblank
    if (video_pio->irq & 2u) {
//        video_pio->irq = 2;
        video_pio->irq = 3; // we clear irq1 for good measure, in case we had gotten out of sync
        prepare_for_vblank_scanline_irqs_enabled();
    }
}

// irq for PIO FIFO
void __isr __video_time_critical_func(isr_pio0_1)() {
    top_up_timing_pio_fifo();
}


static inline bool is_scanline_sm(int sm) {
    return sm == PICO_SCANVIDEO_SCANLINE_SM;
}

void setup_sm(int sm, uint offset) {
    pio_sm_config config = is_scanline_sm(sm) ? video_mode.pio_program->configure_pio(video_pio, sm, offset) :
                           video_htiming_program_get_default_config(offset);

#if PICO_SCANVIDEO_ENABLE_VIDEO_CLOCK_DOWN
    sm_config_set_clkdiv_int_frac(&config, video_clock_down_times_2 / 2, (video_clock_down_times_2 & 1u) << 7u);
#endif

    if (!is_scanline_sm(sm)) {
        // enable auto-pull
        sm_config_set_out_shift(&config, true, true, 32);
        const uint BASE = PICO_SCANVIDEO_SYNC_PIN_BASE; // hsync and vsync are +0 and +1, clock is +2
        uint pin_count;
        // 2 OUT pins and 1 sideset pin following them
        pin_count = 2;
        sm_config_set_out_pins(&config, BASE, pin_count);
        pio_sm_set_consecutive_pindirs(video_pio, sm, BASE, pin_count, true);
    }

    pio_sm_init(video_pio, sm, offset, &config); // now paused
}

scanvideo_mode_t scanvideo_get_mode() {
    return video_mode;
}


static uint default_scanvideo_scanline_repeat_count_fn(uint32_t scanline_id) {
    return 1;
}


//#pragma GCC pop_options

void scanvideo_set_scanline_repeat_fn(scanvideo_scanline_repeat_count_fn fn) {
    _scanline_repeat_count_fn = fn ? fn : default_scanvideo_scanline_repeat_count_fn;
}


static pio_program_t copy_program(const pio_program_t *program, uint16_t *instructions,
                                       uint32_t max_instructions) {
    assert(max_instructions >= program->length);
    pio_program_t copy = *program;
    __builtin_memcpy(instructions, program->instructions, MIN(program->length, max_instructions) * sizeof(uint16_t));
    copy.instructions = instructions;
    return copy;
}

static bool scanvideo_setup_with_timing(const scanvideo_mode_t *mode, const scanvideo_timing_t *timing) {
    __builtin_memset(&shared_state, 0, sizeof(shared_state));

    // init non zero members
    shared_state.scanline.last_scanline_id = 0xffffffff;
    video_mode = *mode;
    video_mode.default_timing = timing;

    if (!video_mode.yscale_denominator) video_mode.yscale_denominator = 1;
    // todo is this still necessary?
    //invalid_params_if(SCANVIDEO_DPI, (timing->v_active % mode->yscale));
    
    sem_init(&vblank_begin, 0, 1);


    __mem_fence_release();

    uint pin_mask = 3u << PICO_SCANVIDEO_SYNC_PIN_BASE;
    bi_decl_if_func_used(bi_2pins_with_names(PICO_SCANVIDEO_SYNC_PIN_BASE, "HSync",
                                               PICO_SCANVIDEO_SYNC_PIN_BASE + 1, "VSync"));
    static_assert(PICO_SCANVIDEO_PIXEL_RSHIFT + PICO_SCANVIDEO_PIXEL_RCOUNT <= PICO_SCANVIDEO_COLOR_PIN_COUNT, "red bits do not fit in color pins");
    static_assert(PICO_SCANVIDEO_PIXEL_GSHIFT + PICO_SCANVIDEO_PIXEL_GCOUNT <= PICO_SCANVIDEO_COLOR_PIN_COUNT, "green bits do not fit in color pins");
    static_assert(PICO_SCANVIDEO_PIXEL_BSHIFT + PICO_SCANVIDEO_PIXEL_BCOUNT <= PICO_SCANVIDEO_COLOR_PIN_COUNT, "blue bits do not fit in color pins");
#define RMASK ((1u << PICO_SCANVIDEO_PIXEL_RCOUNT) - 1u)
#define GMASK ((1u << PICO_SCANVIDEO_PIXEL_GCOUNT) - 1u)
#define BMASK ((1u << PICO_SCANVIDEO_PIXEL_BCOUNT) - 1u)
    pin_mask |= RMASK << (PICO_SCANVIDEO_COLOR_PIN_BASE + PICO_SCANVIDEO_PIXEL_RSHIFT);
    pin_mask |= GMASK << (PICO_SCANVIDEO_COLOR_PIN_BASE + PICO_SCANVIDEO_PIXEL_GSHIFT);
    pin_mask |= BMASK << (PICO_SCANVIDEO_COLOR_PIN_BASE + PICO_SCANVIDEO_PIXEL_BSHIFT);
    bi_decl_if_func_used(bi_pin_mask_with_name(RMASK << (PICO_SCANVIDEO_COLOR_PIN_BASE + PICO_SCANVIDEO_PIXEL_RSHIFT), RMASK == 1 ? "Red" : ("Red 0-" __XSTRING(PICO_SCANVIDEO_PIXEL_GCOUNT))));
    bi_decl_if_func_used(bi_pin_mask_with_name(GMASK << (PICO_SCANVIDEO_COLOR_PIN_BASE + PICO_SCANVIDEO_PIXEL_GSHIFT), GMASK == 1 ? "Green" : ("Green 0-" __XSTRING(PICO_SCANVIDEO_PIXEL_GCOUNT))));
    bi_decl_if_func_used(bi_pin_mask_with_name(BMASK << (PICO_SCANVIDEO_COLOR_PIN_BASE + PICO_SCANVIDEO_PIXEL_BSHIFT), BMASK == 1 ? "Blue" : ("Blue 0-" __XSTRING(PICO_SCANVIDEO_PIXEL_BCOUNT))));

    for(uint8_t i = 0; pin_mask; i++, pin_mask>>=1u) {
        if (pin_mask & 1) gpio_set_function(i, GPIO_FUNC_PIO0);
    }

#if !PICO_SCANVIDEO_ENABLE_VIDEO_CLOCK_DOWN
    valid_params_if(SCANVIDEO_DPI, timing->clock_freq == video_clock_freq);
#else
    uint sys_clk = clock_get_hz(clk_sys);
    video_clock_down_times_2 = sys_clk / timing->clock_freq;
    //if (video_clock_down_times_2 * timing->clock_freq != sys_clk) {
    //    panic("System clock (%d) must be an integer multiple of the requested pixel clock (%d).", sys_clk, timing->clock_freq);
    //}
#endif

    valid_params_if(SCANVIDEO_DPI, mode->width * mode->xscale <= timing->h_active);
    valid_params_if(SCANVIDEO_DPI, mode->height * mode->yscale <= timing->v_active * video_mode.yscale_denominator);

    uint16_t instructions[32];
    pio_program_t modified_program = copy_program(mode->pio_program->program, instructions,
                                                       count_of(instructions));

    if (!mode->pio_program->adapt_for_mode(mode->pio_program, mode, &scanline_buffer.core, instructions)) {
        valid_params_if(SCANVIDEO_DPI, false);
    }
    valid_params_if(SCANVIDEO_DPI, scanline_buffer.core.data && scanline_buffer.core.data_used);
    video_program_load_offset = pio_add_program(video_pio, &modified_program);

    setup_sm(PICO_SCANVIDEO_SCANLINE_SM, video_program_load_offset);

    uint32_t side_set_xor = 0;
    modified_program = copy_program(&video_htiming_program, instructions, count_of(instructions));

    if (timing->clock_polarity) {
        side_set_xor = 0x1000; // flip the top side set bit

        for (uint i = 0; i < video_htiming_program.length; i++) {
            instructions[i] ^= side_set_xor;
        }
    }

    video_htiming_load_offset = pio_add_program(video_pio, &modified_program);

    setup_sm(PICO_SCANVIDEO_TIMING_SM, video_htiming_load_offset);

    // make this highest priority
#if PICO_DEFAULT_IRQ_PRIORITY < 0x40
#warning pico_scanvideo_dpi may not always function correctly without PIO_IRQ_0 at a higher priority than other interrupts.
    irq_set_priority(PIO0_IRQ_1, 0x40); // lower priority by 1
    irq_set_priority(DMA_IRQ_0, 0x80); // lower priority by 2
#else
    irq_set_priority(PIO0_IRQ_0, 0); // highest priority
    irq_set_priority(PIO0_IRQ_1, 0x40); // lower priority by 1
#endif

    dma_claim_mask(PICO_SCANVIDEO_SCANLINE_DMA_CHANNELS_MASK);
    dma_set_irq0_channel_mask_enabled(PICO_SCANVIDEO_SCANLINE_DMA_CHANNELS_MASK, true);
    // also done in scanvideo_timing_enable
//    video_pio->inte1 = 1u << (PICO_SCANVIDEO_TIMING_SM + PIO_IRQ1_INTE_SM0_TXNFULL_LSB);

    // todo reset DMA channels

    dma_channel_config channel_config = dma_channel_get_default_config(PICO_SCANVIDEO_SCANLINE_DMA_CHANNEL);
    channel_config_set_dreq(&channel_config, DREQ_PIO0_TX0 +
                                     PICO_SCANVIDEO_SCANLINE_SM);  // Select scanline dma dreq to be PICO_SCANVIDEO_SCANLINE_SM TX FIFO not full
    dma_channel_configure(PICO_SCANVIDEO_SCANLINE_DMA_CHANNEL,
                          &channel_config,
                          &video_pio->txf[PICO_SCANVIDEO_SCANLINE_SM],
                          NULL, // set later
                          0, // set later
                          false);

    // clear scanline irq
    pio_sm_exec(video_pio, PICO_SCANVIDEO_TIMING_SM, video_htiming_states_program.instructions[CLEAR_IRQ_SCANLINE]);

    // todo there are probably some restrictions :-)
    //assert(timing->v_front_porch == 1);
    //assert(timing->v_pulse == 2);
    //assert(timing->v_total == 500);

    timing_state.v_total = timing->v_total;
    timing_state.v_active = timing->v_active;
    timing_state.v_pulse_start = timing->v_active + timing->v_front_porch;
    timing_state.v_pulse_end = timing_state.v_pulse_start + timing->v_pulse;
    const uint32_t vsync_bit = 0x40000000;
    timing_state.vsync_bits_pulse = timing->v_sync_polarity ? 0 : vsync_bit;
    timing_state.vsync_bits_no_pulse = timing->v_sync_polarity ? vsync_bit : 0;

    // these are read bitwise backwards (lsb to msb) by PIO pogram

    // we can probably do smaller
#define HTIMING_MIN 8

#define TIMING_CYCLE 3u
#define timing_encode(state, length, pins) ((video_htiming_states_program.instructions[state] ^ side_set_xor)| (((uint32_t)(length) - TIMING_CYCLE) << 16u) | ((uint32_t)(pins) << 29u))
#define A_CMD SET_IRQ_0
#define A_CMD_VBLANK SET_IRQ_1
#define B1_CMD CLEAR_IRQ_SCANLINE
#define B2_CMD CLEAR_IRQ_SCANLINE
#define C_CMD SET_IRQ_SCANLINE
#define C_CMD_VBLANK CLEAR_IRQ_SCANLINE

    int h_sync_bit = timing->h_sync_polarity ? 0 : 1;
    timing_state.a = timing_encode(A_CMD, 4, h_sync_bit);
    static_assert(HTIMING_MIN >= 4, "");
    timing_state.a_vblank = timing_encode(A_CMD_VBLANK, 4, h_sync_bit);
    int h_back_porch = timing->h_total - timing->h_front_porch - timing->h_pulse - timing->h_active;

    valid_params_if(SCANVIDEO_DPI, timing->h_pulse - 4 >= HTIMING_MIN);
    timing_state.b1 = timing_encode(B1_CMD, timing->h_pulse - 4, h_sync_bit);

    // todo decide on what these should be - we should really be asserting the timings
    //
    // todo note that the placement of the active scanline IRQ from the timing program is super important.
    //  if it gets moved too much (or indeed at all) it may be that there are problems with DMA/SM IRQ
    //  overlap, which may require the addition of a separate timing state for the prepare for scanline
    //  (separate from the needs of setting the hsync pulse)
    valid_params_if(SCANVIDEO_DPI, timing->h_active >= HTIMING_MIN);
    //assert(timing->h_front_porch >= HTIMING_MIN);
    valid_params_if(SCANVIDEO_DPI, h_back_porch >= HTIMING_MIN);
    valid_params_if(SCANVIDEO_DPI, (timing->h_total - h_back_porch - timing->h_pulse) >= HTIMING_MIN);
    timing_state.b2 = timing_encode(B2_CMD, h_back_porch, !h_sync_bit);
    timing_state.c = timing_encode(C_CMD, timing->h_total - h_back_porch - timing->h_pulse, 4 | !h_sync_bit);
    timing_state.c_vblank = timing_encode(C_CMD_VBLANK, timing->h_total - h_back_porch - timing->h_pulse, !h_sync_bit);

    // this is two scanlines in vblank
    setup_dma_states_vblank();
    timing_state.vsync_bits = timing_state.vsync_bits_no_pulse;
    scanvideo_set_scanline_repeat_fn(NULL);
    return true;
}

bool scanvideo_setup(const scanvideo_mode_t *mode) {
    return scanvideo_setup_with_timing(mode, mode->default_timing);
}


bool video_24mhz_composable_adapt_for_mode(const scanvideo_pio_program_t *program, const scanvideo_mode_t *mode,
                                           scanvideo_scanline_buffer_t *missing_scanline_buffer,
                                           uint16_t *modifiable_instructions) {
    int delay0 = 2 * mode->xscale - 2;
    int delay1 = delay0 + 1;
    valid_params_if(SCANVIDEO_DPI, delay0 <= 31);
    valid_params_if(SCANVIDEO_DPI, delay1 <= 31);

    // todo macro-ify this
    modifiable_instructions[video_24mhz_composable_program_extern(delay_d_0)] |= (unsigned) delay0 << 8u;
    modifiable_instructions[video_24mhz_composable_program_extern(delay_e_0)] |= (unsigned) delay0 << 8u;

    missing_scanline_buffer->data_used = missing_scanline_buffer->data_max = pio_fbwidth;
    missing_scanline_buffer->status = SCANLINE_OK;

    return true;
}


pio_sm_config video_24mhz_composable_configure_pio(pio_hw_t *pio, uint sm, uint offset) {
    pio_sm_config config = video_24mhz_composable_default_program_get_default_config(offset);
    pio_sm_set_consecutive_pindirs(pio, sm, PICO_SCANVIDEO_COLOR_PIN_BASE, PICO_SCANVIDEO_COLOR_PIN_COUNT, true);
    sm_config_set_out_pins(&config, PICO_SCANVIDEO_COLOR_PIN_BASE, PICO_SCANVIDEO_COLOR_PIN_COUNT);
    sm_config_set_out_shift(&config, true, true, 32); // autopull
    sm_config_set_fifo_join(&config, PIO_FIFO_JOIN_TX);
    sm_config_set_out_special(&config, 1, 0, 0);
    return config;
}

void scanvideo_timing_enable(bool enable) {
    // todo we need to protect our state here... this can't be frame synced obviously (at least turning on)
    // todo but we should make sure we clear out state when we turn it off, and probably reset scanline counter when we turn it on
    if (enable != video_timing_enabled) {
        // todo should we disable these too? if not move to scanvideo_setup
        video_pio->inte0 = PIO_IRQ0_INTE_SM0_BITS | PIO_IRQ0_INTE_SM1_BITS;
        video_pio->inte1 = (1u << (PICO_SCANVIDEO_TIMING_SM + PIO_IRQ1_INTE_SM0_TXNFULL_LSB));
        irq_set_mask_enabled((1u << PIO0_IRQ_0)
                              | (1u << PIO0_IRQ_1)
                , enable);
        uint32_t sm_mask = (1u << PICO_SCANVIDEO_SCANLINE_SM) | 1u << PICO_SCANVIDEO_TIMING_SM;
        pio_claim_sm_mask(video_pio, sm_mask);
        pio_set_sm_mask_enabled(video_pio, sm_mask, false);
#if PICO_SCANVIDEO_ENABLE_VIDEO_CLOCK_DOWN
        pio_clkdiv_restart_sm_mask(video_pio, sm_mask);
#endif

        if (enable) {
            uint jmp = video_program_load_offset + pio_encode_jmp(video_mode.pio_program->entry_point);
            pio_sm_exec(video_pio, PICO_SCANVIDEO_SCANLINE_SM, jmp);
            // todo we should offset the addresses for the SM
            pio_sm_exec(video_pio, PICO_SCANVIDEO_TIMING_SM,
                        pio_encode_jmp(video_htiming_load_offset + video_htiming_offset_entry_point));
            pio_set_sm_mask_enabled(video_pio, sm_mask, true);
        }
        video_timing_enabled = enable;
    }
}


void scanvideo_wait_for_vblank() {
    sem_acquire_blocking(&vblank_begin);
}

#endif

#pragma GCC pop_options
