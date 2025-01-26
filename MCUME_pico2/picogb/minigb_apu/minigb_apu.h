/**
 * minigb_apu is released under the terms listed within the LICENSE file.
 *
 * minigb_apu emulates the audio processing unit (APU) of the Game Boy. This
 * project is based on MiniGBS by Alex Baines: https://github.com/baines/MiniGBS
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define AUDIO_SAMPLE_RATE	44100

#define DMG_CLOCK_FREQ		4194304.0
#define SCREEN_REFRESH_CYCLES	70224.0
#define VERTICAL_SYNC		(DMG_CLOCK_FREQ/SCREEN_REFRESH_CYCLES)

#define AUDIO_SAMPLES		((unsigned)(AUDIO_SAMPLE_RATE / VERTICAL_SYNC))
#define AUDIO_BUFFER_SIZE_BYTES (AUDIO_SAMPLES*4)

/**
 * Fill allocated buffer "data" with "len" number of 32-bit floating point
 * samples (native endian order) in stereo interleaved format.
 */
void audio_callback(void *ptr, int16_t *data, size_t len);

/**
 * Read audio register at given address "addr".
 */
uint8_t audio_read(const uint16_t addr);

/**
 * Write "val" to audio register at given address "addr".
 */
void audio_write(const uint16_t addr, const uint8_t val);

/**
 * Initialise audio driver.
 */
void audio_init(void);

#ifdef __cplusplus
}
#endif
