#pragma once

#include <time.h>

#include "hardware/rtc.h"
#include "pico/util/datetime.h"

#ifdef __cplusplus
extern "C" {
#endif

extern time_t epochtime;

void time_init();
void setrtc(datetime_t *t);

#ifdef __cplusplus
}
#endif
