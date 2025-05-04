/**
 * minigb_apu is released under the terms listed within the LICENSE file.
 *
 * minigb_apu emulates the audio processing unit (APU) of the Game Boy. This
 * project is based on MiniGBS by Alex Baines: https://github.com/baines/MiniGBS
 */

#pragma once

#include <stdint.h>

#define MINIGB_APU_AUDIO_FORMAT_S16SYS 1

#ifndef AUDIO_SAMPLE_RATE
# define AUDIO_SAMPLE_RATE	32768
#endif

/* The audio output format is in platform native endian. */
#if defined(MINIGB_APU_AUDIO_FORMAT_S16SYS)
typedef int16_t audio_sample_t;
# define AUDIO_SAMPLE_MAX INT16_MAX
# define AUDIO_SAMPLE_MIN INT16_MIN
# define VOL_INIT_MAX (AUDIO_SAMPLE_MAX/8)
# define VOL_INIT_MIN (AUDIO_SAMPLE_MIN/8)
#elif defined(MINIGB_APU_AUDIO_FORMAT_S32SYS)
typedef int32_t audio_sample_t;
# define AUDIO_SAMPLE_MAX INT32_MAX
# define AUDIO_SAMPLE_MIN INT32_MIN
# define VOL_INIT_MAX (INT32_MAX/8)
# define VOL_INIT_MIN (INT32_MIN/8)
#else
#error MiniGB APU: Invalid or unsupported audio format selected
#endif

#define DMG_CLOCK_FREQ		4194304.0
#define SCREEN_REFRESH_CYCLES	70224.0
#define VERTICAL_SYNC		(DMG_CLOCK_FREQ/SCREEN_REFRESH_CYCLES)

/* Number of audio samples in each channel. */
#define AUDIO_SAMPLES		((unsigned)(AUDIO_SAMPLE_RATE / VERTICAL_SYNC))
/* Number of audio channels. The audio output is in interleaved stereo format.*/
#define AUDIO_CHANNELS		2
/* Number of audio samples output in each audio_callback call. */
#define AUDIO_SAMPLES_TOTAL	(AUDIO_SAMPLES * AUDIO_CHANNELS)

#define AUDIO_MEM_SIZE		(0xFF3F - 0xFF10 + 1)
#define AUDIO_ADDR_COMPENSATION	0xFF10

struct chan_len_ctr {
	uint8_t load;
	uint8_t enabled;
	uint32_t counter;
	uint32_t inc;
};

struct chan_vol_env {
	uint8_t step;
	uint8_t up;
	uint32_t counter;
	uint32_t inc;
};

struct chan_freq_sweep {
	uint8_t rate;
	uint8_t shift;
	uint8_t down;
	uint16_t freq;
	uint32_t counter;
	uint32_t inc;
};

struct chan {
	uint8_t enabled;
	uint8_t powered;
	uint8_t on_left;
	uint8_t on_right;

	uint8_t volume;
	uint8_t volume_init;

	uint16_t freq;
	uint32_t freq_counter;
	uint32_t freq_inc;

	int32_t val;

	struct chan_len_ctr    len;
	struct chan_vol_env    env;
	struct chan_freq_sweep sweep;

	union {
		struct {
			uint8_t duty;
			uint8_t duty_counter;
		} square;
		struct {
			uint16_t lfsr_reg;
			uint8_t  lfsr_wide;
			uint8_t  lfsr_div;
		} noise;
		struct {
			uint8_t sample;
		} wave;
	};
};

struct minigb_apu_ctx {
	struct chan chans[4];
	int32_t vol_l, vol_r;

	/**
	 * Memory holding audio registers between 0xFF10 and 0xFF3F inclusive.
	 */
	uint8_t audio_mem[AUDIO_MEM_SIZE];
};

/**
 * Fill allocated buffer "stream" with AUDIO_SAMPLES_TOTAL number of 16-bit
 * signed samples (native endian order) in stereo interleaved format.
 * Each call corresponds to the time taken for each VSYNC in the Game Boy.
 *
 * \param ctx Library context. Must be initialised with audio_init().
 * \param stream Allocated pointer to store audio samples. Must be at least
 *		AUDIO_SAMPLES_TOTAL in size.
 */
void minigb_apu_audio_callback(struct minigb_apu_ctx *ctx,
		audio_sample_t *stream);

/**
 * Read audio register at given address "addr".
 * \param ctx Library context. Must be initialised with audio_init().
 * \param addr Address of registers to read. Must be within 0xFF10 and 0xFF3F,
 *	inclusive.
 */
uint8_t minigb_apu_audio_read(struct minigb_apu_ctx *ctx, const uint16_t addr);

/**
 * Write "val" to audio register at given address "addr".
 * \param ctx Library context. Must be initialised with audio_init().
 * \param addr Address of registers to read. Must be within 0xFF10 and 0xFF3F,
 *	inclusive.
 * \param val Value to write to address.
 */
void minigb_apu_audio_write(struct minigb_apu_ctx *ctx,
		const uint16_t addr, const uint8_t val);

/**
 * Initialise audio driver.
 * \param ctx Library context.
 */
void minigb_apu_audio_init(struct minigb_apu_ctx *ctx);
