/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SCANVIDEO_COMPOSABLE_SCANLINE_H_
#define SCANVIDEO_COMPOSABLE_SCANLINE_H_

#include "pico/types.h"
#include "scanvideo.pio.h"


#define video_24mhz_composable_prefix video_24mhz_composable_default

// seems needed on some platforms
#define __EXTRA_CONCAT(x, y) __CONCAT(x,y)
#define video_24mhz_composable_program_extern(x) __EXTRA_CONCAT( __EXTRA_CONCAT(video_24mhz_composable_prefix, _offset_), x)
#define __DVP_JMP(x) ((unsigned)video_24mhz_composable_program_extern(x))
#define COMPOSABLE_EOL_SKIP_ALIGN __DVP_JMP(end_of_scanline_skip_ALIGN)
#define COMPOSABLE_RAW_RUN __DVP_JMP(raw_run)
#define COMPOSABLE_RAW_1P __DVP_JMP(raw_1p)

#endif
