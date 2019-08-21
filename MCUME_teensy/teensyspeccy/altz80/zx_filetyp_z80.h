#pragma once
#include "Z80.h"

void ZX_ReadFromFlash_SNA(Z80 *R, const char * filename);
void ZX_ReadFromFlash_Z80(Z80 *R, const uint8_t *data, uint16_t length);

