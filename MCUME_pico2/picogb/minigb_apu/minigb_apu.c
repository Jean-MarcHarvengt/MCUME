/**
 * minigb_apu is released under the terms listed within the LICENSE file.
 *
 * minigb_apu emulates the audio processing unit (APU) of the Game Boy. This
 * project is based on MiniGBS by Alex Baines: https://github.com/baines/MiniGBS
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "minigb_apu.h"

#define DMG_CLOCK_FREQ_U	((unsigned)DMG_CLOCK_FREQ)
#define AUDIO_NSAMPLES		(AUDIO_SAMPLES * 2u)

#define AUDIO_MEM_SIZE		(0xFF3F - 0xFF10 + 1)
#define AUDIO_ADDR_COMPENSATION	0xFF10

#define MAX(a, b)		( a > b ? a : b )
#define MIN(a, b)		( a <= b ? a : b )

#define VOL_INIT_MAX		(INT16_MAX/8)
#define VOL_INIT_MIN		(INT16_MIN/8)

/* Handles time keeping for sound generation.
 * FREQ_INC_REF must be equal to, or larger than AUDIO_SAMPLE_RATE in order
 * to avoid a division by zero error.
 * Using a square of 2 simplifies calculations. */
#define FREQ_INC_REF		(AUDIO_SAMPLE_RATE * 16)

#define MAX_CHAN_VOLUME		15

/**
 * Memory holding audio registers between 0xFF10 and 0xFF3F inclusive.
 */
static uint8_t audio_mem[AUDIO_MEM_SIZE];

struct chan_len_ctr {
	uint8_t load;
	unsigned enabled : 1;
	uint32_t counter;
	uint32_t inc;
};

struct chan_vol_env {
	uint8_t step;
	unsigned up : 1;
	uint32_t counter;
	uint32_t inc;
};

struct chan_freq_sweep {
	uint16_t freq;
	uint8_t rate;
	uint8_t shift;
	unsigned up : 1;
	uint32_t counter;
	uint32_t inc;
};

static struct chan {
	unsigned enabled : 1;
	unsigned powered : 1;
	unsigned on_left : 1;
	unsigned on_right : 1;
	unsigned muted : 1;

	uint8_t volume;
	uint8_t volume_init;

	uint16_t freq;
	uint32_t freq_counter;
	uint32_t freq_inc;

	int_fast16_t val;

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
} chans[4];

static int32_t vol_l, vol_r;

static void set_note_freq(struct chan *c, const uint32_t freq)
{
	/* Lowest expected value of freq is 64. */
	c->freq_inc = freq * (uint32_t)(FREQ_INC_REF / AUDIO_SAMPLE_RATE);
}

static void chan_enable(const uint_fast8_t i, const bool enable)
{
	uint8_t val;

	chans[i].enabled = enable;
	val = (audio_mem[0xFF26 - AUDIO_ADDR_COMPENSATION] & 0x80) |
		(chans[3].enabled << 3) | (chans[2].enabled << 2) |
		(chans[1].enabled << 1) | (chans[0].enabled << 0);

	audio_mem[0xFF26 - AUDIO_ADDR_COMPENSATION] = val;
	//audio_mem[0xFF26 - AUDIO_ADDR_COMPENSATION] |= 0x80 | ((uint8_t)enable) << i;
}

static void update_env(struct chan *c)
{
	c->env.counter += c->env.inc;

	while (c->env.counter > FREQ_INC_REF) {
		if (c->env.step) {
			c->volume += c->env.up ? 1 : -1;
			if (c->volume == 0 || c->volume == MAX_CHAN_VOLUME) {
				c->env.inc = 0;
			}
			c->volume = MAX(0, MIN(MAX_CHAN_VOLUME, c->volume));
		}
		c->env.counter -= FREQ_INC_REF;
	}
}

static void update_len(struct chan *c)
{
	if (!c->len.enabled)
		return;

	c->len.counter += c->len.inc;
	if (c->len.counter > FREQ_INC_REF) {
		chan_enable(c - chans, 0);
		c->len.counter = 0;
	}
}

static bool update_freq(struct chan *c, uint32_t *pos)
{
	uint32_t inc = c->freq_inc - *pos;
	c->freq_counter += inc;

	if (c->freq_counter > FREQ_INC_REF) {
		*pos		= c->freq_inc - (c->freq_counter - FREQ_INC_REF);
		c->freq_counter = 0;
		return true;
	} else {
		*pos = c->freq_inc;
		return false;
	}
}

static void update_sweep(struct chan *c)
{
	c->sweep.counter += c->sweep.inc;

	while (c->sweep.counter > FREQ_INC_REF) {
		if (c->sweep.shift) {
			uint16_t inc = (c->sweep.freq >> c->sweep.shift);
			if (!c->sweep.up)
				inc *= -1;

			c->freq += inc;
			if (c->freq > 2047) {
				c->enabled = 0;
			} else {
				set_note_freq(c,
					DMG_CLOCK_FREQ_U / ((2048 - c->freq)<< 5));
				c->freq_inc *= 8;
			}
		} else if (c->sweep.rate) {
			c->enabled = 0;
		}
		c->sweep.counter -= FREQ_INC_REF;
	}
}

static void update_square(int16_t* samples, const bool ch2)
{
	uint32_t freq;
	struct chan* c = chans + ch2;

	if (!c->powered || !c->enabled)
		return;

	freq = DMG_CLOCK_FREQ_U / ((2048 - c->freq) << 5);
	set_note_freq(c, freq);
	c->freq_inc *= 8;

	for (uint_fast16_t i = 0; i < AUDIO_NSAMPLES; i += 2) {
		update_len(c);

		if (!c->enabled)
			continue;

		update_env(c);
		if (!ch2)
			update_sweep(c);

		uint32_t pos = 0;
		uint32_t prev_pos = 0;
		int32_t sample = 0;

		while (update_freq(c, &pos)) {
			c->square.duty_counter = (c->square.duty_counter + 1) & 7;
			sample += ((pos - prev_pos) / c->freq_inc) * c->val;
			c->val = (c->square.duty & (1 << c->square.duty_counter)) ?
				VOL_INIT_MAX / MAX_CHAN_VOLUME :
				VOL_INIT_MIN / MAX_CHAN_VOLUME;
			prev_pos = pos;
		}

		if (c->muted)
			continue;

		sample += c->val;
		sample *= c->volume;
		sample /= 4;

		samples[i + 0] += sample * c->on_left * vol_l;
		samples[i + 1] += sample * c->on_right * vol_r;
	}
}

static uint8_t wave_sample(const unsigned int pos, const unsigned int volume)
{
	uint8_t sample;

	sample =  audio_mem[(0xFF30 + pos / 2) - AUDIO_ADDR_COMPENSATION];
	if (pos & 1) {
		sample &= 0xF;
	} else {
		sample >>= 4;
	}
	return volume ? (sample >> (volume - 1)) : 0;
}

static void update_wave(int16_t *samples)
{
	uint32_t freq;
	struct chan *c = chans + 2;

	if (!c->powered || !c->enabled)
		return;

	freq = (DMG_CLOCK_FREQ_U / 64) / (2048 - c->freq);
	set_note_freq(c, freq);

	c->freq_inc *= 32;

	for (uint_fast16_t i = 0; i < AUDIO_NSAMPLES; i += 2) {
		update_len(c);

		if (!c->enabled)
			continue;

		uint32_t pos      = 0;
		uint32_t prev_pos = 0;
		int32_t sample   = 0;

		c->wave.sample = wave_sample(c->val, c->volume);

		while (update_freq(c, &pos)) {
			c->val = (c->val + 1) & 31;
			sample += ((pos - prev_pos) / c->freq_inc) *
				((int)c->wave.sample - 8) * (INT16_MAX/64);
			c->wave.sample = wave_sample(c->val, c->volume);
			prev_pos  = pos;
		}

		sample += ((int)c->wave.sample - 8) * (int)(INT16_MAX/64);

		if (c->volume == 0)
			continue;

		{
			/* First element is unused. */
			int16_t div[] = { INT16_MAX, 1, 2, 4 };
			sample = sample / (div[c->volume]);
		}

		if (c->muted)
			continue;

		sample /= 4;

		samples[i + 0] += sample * c->on_left * vol_l;
		samples[i + 1] += sample * c->on_right * vol_r;
	}
}

static void update_noise(int16_t *samples)
{
	struct chan *c = chans + 3;

	if (!c->powered)
		return;

	{
		const uint32_t lfsr_div_lut[] = {
			8, 16, 32, 48, 64, 80, 96, 112
		};
		uint32_t freq;

		freq = DMG_CLOCK_FREQ_U / (lfsr_div_lut[c->noise.lfsr_div] << c->freq);
		set_note_freq(c, freq);
	}

	if (c->freq >= 14)
		c->enabled = 0;

	for (uint_fast16_t i = 0; i < AUDIO_NSAMPLES; i += 2) {
		update_len(c);

		if (!c->enabled)
			continue;

		update_env(c);

		uint32_t pos      = 0;
		uint32_t prev_pos = 0;
		int32_t sample    = 0;

		while (update_freq(c, &pos)) {
			c->noise.lfsr_reg = (c->noise.lfsr_reg << 1) |
				(c->val >= VOL_INIT_MAX/MAX_CHAN_VOLUME);

			if (c->noise.lfsr_wide) {
				c->val = !(((c->noise.lfsr_reg >> 14) & 1) ^
						((c->noise.lfsr_reg >> 13) & 1)) ?
					VOL_INIT_MAX / MAX_CHAN_VOLUME :
					VOL_INIT_MIN / MAX_CHAN_VOLUME;
			} else {
				c->val = !(((c->noise.lfsr_reg >> 6) & 1) ^
						((c->noise.lfsr_reg >> 5) & 1)) ?
					VOL_INIT_MAX / MAX_CHAN_VOLUME :
					VOL_INIT_MIN / MAX_CHAN_VOLUME;
			}

			sample += ((pos - prev_pos) / c->freq_inc) * c->val;
			prev_pos = pos;
		}

		if (c->muted)
			continue;

		sample += c->val;
		sample *= c->volume;
		sample /= 4;

		samples[i + 0] += sample * c->on_left * vol_l;
		samples[i + 1] += sample * c->on_right * vol_r;
	}
}

/**
 * SDL2 style audio callback function.
 */
void audio_callback(void *userdata, int16_t *stream, size_t len)
{
	/* Appease unused variable warning. */
	(void)userdata;

	memset(stream, 0, len);

	update_square(stream, 0);
	update_square(stream, 1);
	update_wave(stream);
	update_noise(stream);
}

static void chan_trigger(uint_fast8_t i)
{
	struct chan *c = chans + i;

	chan_enable(i, 1);
	c->volume = c->volume_init;

	// volume envelope
	{
		uint8_t val =
			audio_mem[(0xFF12 + (i * 5)) - AUDIO_ADDR_COMPENSATION];

		c->env.step = val & 0x07;
		c->env.up   = val & 0x08 ? 1 : 0;
		c->env.inc  = c->env.step ?
			(FREQ_INC_REF * 64ul) / ((uint32_t)c->env.step * AUDIO_SAMPLE_RATE) :
			(8ul * FREQ_INC_REF) / AUDIO_SAMPLE_RATE ;
		c->env.counter = 0;
	}

	// freq sweep
	if (i == 0) {
		uint8_t val = audio_mem[0xFF10 - AUDIO_ADDR_COMPENSATION];

		c->sweep.freq  = c->freq;
		c->sweep.rate  = (val >> 4) & 0x07;
		c->sweep.up    = !(val & 0x08);
		c->sweep.shift = (val & 0x07);
		c->sweep.inc   = c->sweep.rate ?
			((128 * FREQ_INC_REF) / (c->sweep.rate * AUDIO_SAMPLE_RATE)) : 0;
		c->sweep.counter = FREQ_INC_REF;
	}

	int len_max = 64;

	if (i == 2) { // wave
		len_max = 256;
		c->val = 0;
	} else if (i == 3) { // noise
		c->noise.lfsr_reg = 0xFFFF;
		c->val = VOL_INIT_MIN / MAX_CHAN_VOLUME;
	}

	c->len.inc = (256 * FREQ_INC_REF) / (AUDIO_SAMPLE_RATE * (len_max - c->len.load));
	c->len.counter = 0;
}

/**
 * Read audio register.
 * \param addr	Address of audio register. Must be 0xFF10 <= addr <= 0xFF3F.
 *				This is not checked in this function.
 * \return	Byte at address.
 */
uint8_t audio_read(const uint16_t addr)
{
	static const uint8_t ortab[] = {
		0x80, 0x3f, 0x00, 0xff, 0xbf,
		0xff, 0x3f, 0x00, 0xff, 0xbf,
		0x7f, 0xff, 0x9f, 0xff, 0xbf,
		0xff, 0xff, 0x00, 0x00, 0xbf,
		0x00, 0x00, 0x70,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	return audio_mem[addr - AUDIO_ADDR_COMPENSATION] |
		ortab[addr - AUDIO_ADDR_COMPENSATION];
}

/**
 * Write audio register.
 * \param addr	Address of audio register. Must be 0xFF10 <= addr <= 0xFF3F.
 *				This is not checked in this function.
 * \param val	Byte to write at address.
 */
void audio_write(const uint16_t addr, const uint8_t val)
{
	/* Find sound channel corresponding to register address. */
	uint_fast8_t i;

	if(addr == 0xFF26)
	{
		audio_mem[addr - AUDIO_ADDR_COMPENSATION] = val & 0x80;
		/* On APU power off, clear all registers apart from wave
		 * RAM. */
		if((val & 0x80) == 0)
		{
			memset(audio_mem, 0x00, 0xFF26 - AUDIO_ADDR_COMPENSATION);
			chans[0].enabled = false;
			chans[1].enabled = false;
			chans[2].enabled = false;
			chans[3].enabled = false;
		}

		return;
	}

	/* Ignore register writes if APU powered off. */
	if(audio_mem[0xFF26 - AUDIO_ADDR_COMPENSATION] == 0x00)
		return;

	audio_mem[addr - AUDIO_ADDR_COMPENSATION] = val;
	i = (addr - AUDIO_ADDR_COMPENSATION) / 5;

	switch (addr) {
	case 0xFF12:
	case 0xFF17:
	case 0xFF21: {
		chans[i].volume_init = val >> 4;
		chans[i].powered     = (val >> 3) != 0;

		// "zombie mode" stuff, needed for Prehistorik Man and probably
		// others
		if (chans[i].powered && chans[i].enabled) {
			if ((chans[i].env.step == 0 && chans[i].env.inc != 0)) {
				if (val & 0x08) {
					chans[i].volume++;
				} else {
					chans[i].volume += 2;
				}
			} else {
				chans[i].volume = 16 - chans[i].volume;
			}

			chans[i].volume &= 0x0F;
			chans[i].env.step = val & 0x07;
		}
	} break;

	case 0xFF1C:
		chans[i].volume = chans[i].volume_init = (val >> 5) & 0x03;
		break;

	case 0xFF11:
	case 0xFF16:
	case 0xFF20: {
		const uint8_t duty_lookup[] = { 0x10, 0x30, 0x3C, 0xCF };
		chans[i].len.load = val & 0x3f;
		chans[i].square.duty = duty_lookup[val >> 6];
		break;
	}

	case 0xFF1B:
		chans[i].len.load = val;
		break;

	case 0xFF13:
	case 0xFF18:
	case 0xFF1D:
		chans[i].freq &= 0xFF00;
		chans[i].freq |= val;
		break;

	case 0xFF1A:
		chans[i].powered = (val & 0x80) != 0;
		chan_enable(i, val & 0x80);
		break;

	case 0xFF14:
	case 0xFF19:
	case 0xFF1E:
		chans[i].freq &= 0x00FF;
		chans[i].freq |= ((val & 0x07) << 8);
		/* Intentional fall-through. */
	case 0xFF23:
		chans[i].len.enabled = val & 0x40 ? 1 : 0;
		if (val & 0x80)
			chan_trigger(i);

		break;

	case 0xFF22:
		chans[3].freq = val >> 4;
		chans[3].noise.lfsr_wide = !(val & 0x08);
		chans[3].noise.lfsr_div = val & 0x07;
		break;

	case 0xFF24:
	{
		vol_l = ((val >> 4) & 0x07);
		vol_r = (val & 0x07);
		break;
	}

	case 0xFF25:
		for (uint_fast8_t j = 0; j < 4; j++) {
			chans[j].on_left  = (val >> (4 + j)) & 1;
			chans[j].on_right = (val >> j) & 1;
		}
		break;
	}
}

void audio_init(void)
{
	/* Initialise channels and samples. */
	memset(chans, 0, sizeof(chans));
	chans[0].val = chans[1].val = -1;

	/* Initialise IO registers. */
	{
		const uint8_t regs_init[] = { 0x80, 0xBF, 0xF3, 0xFF, 0x3F,
					      0xFF, 0x3F, 0x00, 0xFF, 0x3F,
					      0x7F, 0xFF, 0x9F, 0xFF, 0x3F,
					      0xFF, 0xFF, 0x00, 0x00, 0x3F,
					      0x77, 0xF3, 0xF1 };

		for(uint_fast8_t i = 0; i < sizeof(regs_init); ++i)
			audio_write(0xFF10 + i, regs_init[i]);
	}

	/* Initialise Wave Pattern RAM. */
	{
		const uint8_t wave_init[] = { 0xac, 0xdd, 0xda, 0x48,
					      0x36, 0x02, 0xcf, 0x16,
					      0x2c, 0x04, 0xe5, 0x2c,
					      0xac, 0xdd, 0xda, 0x48 };

		for(uint_fast8_t i = 0; i < sizeof(wave_init); ++i)
			audio_write(0xFF30 + i, wave_init[i]);
	}
}
