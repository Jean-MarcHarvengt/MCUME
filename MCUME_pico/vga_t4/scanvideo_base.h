/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SCANVIDEO_scanvideo_H_
#define SCANVIDEO_scanvideo_H_

#include "pico/types.h"

#if !PICO_NO_HARDWARE

#include "hardware/pio.h"

#endif

#ifdef __cplusplus
extern "C" {
#endif

/** \file scanvideo_base.h
 *  \defgroup pico_scanvideo pico_scanvideo
 *
 * Common Scan-out Video API
 */
// == CONFIG ============

#ifndef PICO_SCANVIDEO_ENABLE_CLOCK_PIN
#define PICO_SCANVIDEO_ENABLE_CLOCK_PIN 0
#endif

#ifndef PICO_SCANVIDEO_ENABLE_DEN_PIN
#define PICO_SCANVIDEO_ENABLE_DEN_PIN 0
#endif

#ifndef PICO_SCANVIDEO_COLOR_PIN_BASE
#define PICO_SCANVIDEO_COLOR_PIN_BASE 2
#endif

#ifndef PICO_SCANVIDEO_COLOR_PIN_COUNT
#define PICO_SCANVIDEO_COLOR_PIN_COUNT 8
#endif

#ifndef PICO_SCANVIDEO_SYNC_PIN_BASE
#define PICO_SCANVIDEO_SYNC_PIN_BASE (PICO_SCANVIDEO_COLOR_PIN_BASE + PICO_SCANVIDEO_COLOR_PIN_COUNT)
#endif

#ifndef PICO_SCANVIDEO_ENABLE_VIDEO_CLOCK_DOWN
#define PICO_SCANVIDEO_ENABLE_VIDEO_CLOCK_DOWN 0
#endif



// ======================

#define BPP 16

// most likely 24000000
extern const uint32_t video_clock_freq;

// todo pragma pack?
typedef struct scanvideo_timing {
    uint32_t clock_freq;

    uint16_t h_active;
    uint16_t v_active;

    uint16_t h_front_porch;
    uint16_t h_pulse;
    uint16_t h_total;
    uint8_t h_sync_polarity;

    uint16_t v_front_porch;
    uint16_t v_pulse;
    uint16_t v_total;
    uint8_t v_sync_polarity;

    uint8_t enable_clock;
    uint8_t clock_polarity;

    uint8_t enable_den;
} scanvideo_timing_t;

typedef struct scanvideo_pio_program scanvideo_pio_program_t;

// todo we need to handle blank data correctly (perhaps DMA should just not start for that scanline,
//  though obviously this is slightly more complicated with multiple playfields, or perhaps worse with
//  just one
typedef struct scanvideo_mode {
    const scanvideo_timing_t *default_timing;
    const scanvideo_pio_program_t *pio_program;

    uint16_t width;
    uint16_t height;
    uint8_t xscale; // 1 == normal, 2 == double wide etc. up to what pio timing allows (not sure I have an assert based on delays)
    uint16_t yscale; // same for y scale (except any yscale is possible)
    // if > 1 then yscale is divided by this to provide the effective yscale;
    // note that yscale must be > yscale_denominator; i.e. only stretching is supported
    uint16_t yscale_denominator;
} scanvideo_mode_t;

extern bool scanvideo_setup(const scanvideo_mode_t *mode);
extern bool scanvideo_setup_with_timing(const scanvideo_mode_t *mode, const scanvideo_timing_t *timing);
extern void scanvideo_timing_enable(bool enable);
// these take effect after the next vsync
extern void scanvideo_display_enable(bool enable);
// doesn't exist yet!
// extern void video_set_display_mode(const struct scanvideo_mode *mode);

// --- scanline management ---

typedef struct scanvideo_scanline_buffer {
    uint32_t scanline_id;
    uint32_t *data;
    uint16_t data_used;
    uint16_t data_max;
    void *user_data;
    uint8_t status;
} scanvideo_scanline_buffer_t;

enum {
    SCANLINE_OK = 1,
    SCANLINE_ERROR,
    SCANLINE_SKIPPED
};

// note frame numbers wrap
static inline uint16_t scanvideo_frame_number(uint32_t scanline_id) {
    return (uint16_t) (scanline_id >> 16u);
}

static inline uint16_t scanvideo_scanline_number(uint32_t scanline_id) {
    return (uint16_t) scanline_id;
}

/**
 * @return the current vga mode (if there is one)
 */
extern scanvideo_mode_t scanvideo_get_mode();

extern void scanvideo_wait_for_vblank();


/**
 * Return a scanline that has been generated / or at least the client is done with.
 *
 * The status field indicates whether the scanline was actually generated OK
 *
 * This method may be called concurrently (for different buffers)
 *
 * @param scanline_buffer \todo
 */
void scanvideo_end_scanline_generation(scanvideo_scanline_buffer_t *scanline_buffer);

typedef uint (*scanvideo_scanline_repeat_count_fn)(uint32_t scanline_id);
void scanvideo_set_scanline_repeat_fn(scanvideo_scanline_repeat_count_fn fn);

extern const scanvideo_timing_t vga_timing_640x480_60_default;
extern const scanvideo_mode_t vga_mode_320x240_60;


// mode implementation

struct scanvideo_pio_program {
#if !PICO_NO_HARDWARE
    const pio_program_t *program;
    const uint8_t entry_point;
    // modifiable_instructions is of size program->length
    bool (*adapt_for_mode)(const scanvideo_pio_program_t *program, const scanvideo_mode_t *mode,
                           scanvideo_scanline_buffer_t *missing_scanline_buffer, uint16_t *modifiable_instructions);
    pio_sm_config (*configure_pio)(pio_hw_t *pio, uint sm, uint offset);
#else
    const char *id;
#endif
};

extern const scanvideo_pio_program_t video_24mhz_composable;

#if !PICO_NO_HARDWARE
extern void scanvideo_default_configure_pio(pio_hw_t *pio, uint sm, uint offset, pio_sm_config *config, bool overlay);
#endif


// note this is not necessarily an absolute gpio pin mask, it is still shifted by PICO_SCANVIDEO_COLOR_PIN_BASE
#define PICO_SCANVIDEO_ALPHA_MASK (1u << PICO_SCANVIDEO_ALPHA_PIN)

#ifndef PICO_SCANVIDEO_PIXEL_FROM_RGB8
#define PICO_SCANVIDEO_PIXEL_FROM_RGB8(r, g, b) ((((b)>>3u)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|(((g)>>3u)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|(((r)>>3u)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#endif

#ifndef PICO_SCANVIDEO_PIXEL_FROM_RGB5
#define PICO_SCANVIDEO_PIXEL_FROM_RGB5(r, g, b) (((b)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((g)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((r)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#endif

#ifndef PICO_SCANVIDEO_R5_FROM_PIXEL
#define PICO_SCANVIDEO_R5_FROM_PIXEL(p) (((p)>>PICO_SCANVIDEO_PIXEL_RSHIFT)&0x1f)
#endif

#ifndef PICO_SCANVIDEO_G5_FROM_PIXEL
#define PICO_SCANVIDEO_G5_FROM_PIXEL(p) (((p)>>PICO_SCANVIDEO_PIXEL_GSHIFT)&0x1f)
#endif

#ifndef PICO_SCANVIDEO_B5_FROM_PIXEL
#define PICO_SCANVIDEO_B5_FROM_PIXEL(p) (((p)>>PICO_SCANVIDEO_PIXEL_BSHIFT)&0x1f)
#endif

#ifdef __cplusplus
}
#endif

#endif //_VIDEO_H
