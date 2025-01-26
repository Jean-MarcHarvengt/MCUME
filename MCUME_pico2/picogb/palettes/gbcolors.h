/**
 * MIT License
 *
 * Copyright (c) 2022 Vincent Mistler
 * Original source code from deltabeard/Peanut-GB Copyright (c) 2018 Mahyar Koshkouei
 * https://github.com/deltabeard/Peanut-GB/blob/master/examples/sdl2/peanut_sdl.c
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>

#define NUMBER_OF_MANUAL_PALETTES 13
#define PALETTE_SIZE_IN_BYTES (3 * 4 * sizeof(uint16_t))

typedef uint16_t palette_t[3][4];

/*
 * Get an RGB565 colour palette by entry ID & shuffling flags
 * 
 * A total of 51 unique palette configurations are in here. 
 * 45 are known to be used by some games (6 of which are shared with
 * manual palette selection), and 6 are unique to manual palette selection
 * 
 * Palettes configurations from The Cutting Room Floor
 * https://tcrf.net/Game_Boy_Color_Bootstrap_ROM
 */
void get_colour_palette(palette_t selected_palette,uint8_t table_entry,uint8_t shuffling_flags)
{
	printf("I get_colour_palette(table_entry=0x%02X,shuffling_flags=0x%02X)\n",
		table_entry,
		shuffling_flags);
	if(table_entry==0x00 && shuffling_flags==0x01)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFB80, 0x9200, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xAD70, 0x438F, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0xAD70, 0x438F, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x00 && shuffling_flags==0x03)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFB80, 0x9200, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xFB80, 0x9200, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0xAD70, 0x438F, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x00 && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFB80, 0x9200, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x5DFF, 0xF800, 0x001F }, /* OBJ1 */
			{ 0xFFFF, 0xAD70, 0x438F, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x01 && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFE28, 0xFEA0, 0x91C0, 0x4800 }, /* OBJ0 */
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ1 */
			{ 0xFFF3, 0x95BF, 0x64AE, 0x01C7 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x02 && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFFFF, 0x653F, 0x001F }, /* OBJ0 */
			{ 0xFFFF, 0xFD6C, 0x8180, 0x0000 }, /* OBJ1 */
			{ 0x6FE0, 0xFFFF, 0xFA89, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x03 && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFFFF, 0x653F, 0x001F }, /* OBJ0 */
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ1 */
			{ 0x56E0, 0xFC20, 0xFFE0, 0xFFFF }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x04 && shuffling_flags==0x03)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0x7FE0, 0xB380, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x05 && shuffling_flags==0x00)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0x57E0, 0xFA00, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x57E0, 0xFA00, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0x57E0, 0xFA00, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x05 && shuffling_flags==0x03)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0x57E0, 0xFA00, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x05 && shuffling_flags==0x04)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0x57E0, 0xFA00, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x5DFF, 0xF800, 0x001F }, /* OBJ1 */
			{ 0xFFFF, 0x57E0, 0xFA00, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x06 && shuffling_flags==0x00)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFCE0, 0xF800, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xFCE0, 0xF800, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0xFCE0, 0xF800, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x06 && shuffling_flags==0x03)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0xFCE0, 0xF800, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x06 && shuffling_flags==0x04)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFCE0, 0xF800, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x5DFF, 0xF800, 0x001F }, /* OBJ1 */
			{ 0xFFFF, 0xFCE0, 0xF800, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x07 && shuffling_flags==0x00)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFFE0, 0xF800, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xFFE0, 0xF800, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0xFFE0, 0xF800, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x07 && shuffling_flags==0x04)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFFE0, 0xF800, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x5DFF, 0xF800, 0x001F }, /* OBJ1 */
			{ 0xFFFF, 0xFFE0, 0xF800, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x08 && shuffling_flags==0x00)
	{
		const palette_t palette = 	{
			{ 0xA4FF, 0xFFE0, 0x0300, 0x0000 }, /* OBJ0 */
			{ 0xA4FF, 0xFFE0, 0x0300, 0x0000 }, /* OBJ1 */
			{ 0xA4FF, 0xFFE0, 0x0300, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x08 && shuffling_flags==0x03)
	{
		const palette_t palette = 	{
			{ 0xFB0A, 0xD000, 0x6000, 0x0000 }, /* OBJ0 */
			{ 0xFB0A, 0xD000, 0x6000, 0x0000 }, /* OBJ1 */
			{ 0xA4FF, 0xFFE0, 0x0300, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x08 && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFB0A, 0xD000, 0x6000, 0x0000 }, /* OBJ0 */
			{ 0x001F, 0xFFFF, 0xFFEF, 0x043F }, /* OBJ1 */
			{ 0xA4FF, 0xFFE0, 0x0300, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x09 && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFB80, 0x9200, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }, /* OBJ1 */
			{ 0xFFF9, 0x677D, 0x9C26, 0x5ACB }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x0A && shuffling_flags==0x03)
	{
		const palette_t palette = 	{
			{ 0x0000, 0xFFFF, 0xFC30, 0x91C7 }, /* OBJ0 */
			{ 0x0000, 0xFFFF, 0xFC30, 0x91C7 }, /* OBJ1 */
			{ 0xB5BF, 0xFFF2, 0xAAC8, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x0B && shuffling_flags==0x01)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x0B && shuffling_flags==0x02)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x0B && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xFFEF, 0x043F, 0xF800 }, /* OBJ1 */
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x0C && shuffling_flags==0x02)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0x8C7B, 0x5291, 0x0000 }, /* OBJ0 */
			{ 0xFE28, 0xFEA0, 0x91C0, 0x4800 }, /* OBJ1 */
			{ 0xFFFF, 0x8C7B, 0x5291, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x0C && shuffling_flags==0x03)
	{
		const palette_t palette = 	{
			{ 0xFE28, 0xFEA0, 0x91C0, 0x4800 }, /* OBJ0 */
			{ 0xFE28, 0xFEA0, 0x91C0, 0x4800 }, /* OBJ1 */
			{ 0xFFFF, 0x8C7B, 0x5291, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x0C && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFE28, 0xFEA0, 0x91C0, 0x4800 }, /* OBJ0 */
			{ 0xFFFF, 0x5DFF, 0xF800, 0x001F }, /* OBJ1 */
			{ 0xFFFF, 0x8C7B, 0x5291, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x0D && shuffling_flags==0x01)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x8C7B, 0x5291, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0x8C7B, 0x5291, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x0D && shuffling_flags==0x03)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0x8C7B, 0x5291, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x0D && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xFD6C, 0x8180, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0x8C7B, 0x5291, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x0E && shuffling_flags==0x03)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0x7FE6, 0x0420, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x0E && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0x7FE6, 0x0420, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x0F && shuffling_flags==0x03)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0xFD6C, 0x8180, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x0F && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x7FE6, 0x0420, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0xFD6C, 0x8180, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x10 && shuffling_flags==0x01)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0x7FE6, 0x0420, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x10 && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0x7FE6, 0x0420, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x11 && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0x07E0, 0x3420, 0x0240 }, /* OBJ0 */
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x12 && shuffling_flags==0x00)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFD6C, 0x8180, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xFD6C, 0x8180, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0xFD6C, 0x8180, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x12 && shuffling_flags==0x03)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0x7FE6, 0x0420, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x7FE6, 0x0420, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0xFD6C, 0x8180, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x12 && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0x7FE6, 0x0420, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0xFD6C, 0x8180, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x13 && shuffling_flags==0x00)
	{
		const palette_t palette = 	{
			{ 0x0000, 0x0430, 0xFEE0, 0xFFFF }, /* OBJ0 */
			{ 0x0000, 0x0430, 0xFEE0, 0xFFFF }, /* OBJ1 */
			{ 0x0000, 0x0430, 0xFEE0, 0xFFFF }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x14 && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFFE0, 0xF800, 0x6000, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x7FE6, 0x0420, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x15 && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFD6C, 0x8180, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0xAD70, 0x438F, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x16 && shuffling_flags==0x00)
	{
		const palette_t palette = 	{
			{ 0xB634, 0x8CCF, 0x63AA, 0x31C4 }, /* OBJ0 */
			{ 0xB634, 0x8CCF, 0x63AA, 0x31C4 }, /* OBJ1 */
			{ 0xB634, 0x8CCF, 0x63AA, 0x31C4 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x17 && shuffling_flags==0x00)
	{
		const palette_t palette = 	{
			{ 0xFFF4, 0xFCB2, 0x94BF, 0x0000 }, /* OBJ0 */
			{ 0xFFF4, 0xFCB2, 0x94BF, 0x0000 }, /* OBJ1 */
			{ 0xFFF4, 0xFCB2, 0x94BF, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x18 && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x7FE6, 0x0420, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x19 && shuffling_flags==0x03)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFD6C, 0x8180, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xFD6C, 0x8180, 0x0000 }, /* OBJ1 */
			{ 0xC864, 0xCCF0, 0x8345, 0x5981 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x1A && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x7FE6, 0x0420, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0xFFE0, 0x7A40, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x1B && shuffling_flags==0x00)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFE60, 0x9B00, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xFE60, 0x9B00, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0xFE60, 0x9B00, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x1C && shuffling_flags==0x01)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x7FE6, 0x0318, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0x7FE6, 0x0318, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x1C && shuffling_flags==0x03)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0x14c8, 0x0318, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0x1C && shuffling_flags==0x05)
	{
		const palette_t palette = 	{
			{ 0xFFFF, 0xFC30, 0x91C7, 0x0000 }, /* OBJ0 */
			{ 0xFFFF, 0x653F, 0x001F, 0x0000 }, /* OBJ1 */
			{ 0xFFFF, 0x7FE6, 0x0318, 0x0000 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	if(table_entry==0xFF && shuffling_flags==0xFF)
	{
		/* Game Boy DMG palette (4 shades of green) */
		const palette_t palette = 	{
			{ 0xDFEA, 0xAE68, 0x74E6, 0x4388 }, /* OBJ0 */
			{ 0xDFEA, 0xAE68, 0x74E6, 0x4388 }, /* OBJ1 */
			{ 0xDFEA, 0xAE68, 0x74E6, 0x4388 }  /* BG */
		};
		memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
		return;
	}
	/* default palette */
	printf("E get_colour_palette: No palette found for table_entry=0x%02X shuffling_flags=0x%02X\n",
		table_entry,
		shuffling_flags);
	/* Game Boy DMG palette (4 shades of green) */
	const palette_t palette = 	{
		{ 0xDFEA, 0xAE68, 0x74E6, 0x4388 }, /* OBJ0 */
		{ 0xDFEA, 0xAE68, 0x74E6, 0x4388 }, /* OBJ1 */
		{ 0xDFEA, 0xAE68, 0x74E6, 0x4388 }  /* BG */
	};
	memcpy(selected_palette, palette, PALETTE_SIZE_IN_BYTES);
}

/**
 * Automatically assigns a colour palette to the game using a given game
 * checksum and disambiguation characters (4th character of the game title)
 * 
 * "The Game Boy Color Bootstrap ROM is executed when the GBC is turned on.
 * The ROM assigns color palettes to certain monochrome Game Boy games by 
 * computing a hash of the ROM's header title for every Nintendo Licensee game 
 * and checking it against an internal database of hashes. 
 * The resulting index is then used to obtain an entry ID 
 * (from 0x00 up to and including 0x1C) and shuffling flags (a 3-bit bitfield). 
 * An entry is a triplet of palettes, and the "shuffling flags" replace some 
 * of the triplet's palettes with others." 
 * Source: The Cutting Room Floor (https://tcrf.net/Game_Boy_Color_Bootstrap_ROM)
 */
void auto_assign_palette(uint16_t palette[3][4], uint8_t game_checksum, const char *game_title)
{
	char disambiguation_character=game_title[3]; /* e.g. 'METROID' -> R */
	printf("I auto_assign_palette(0x%02X,%s)\n", game_checksum,game_title);
	switch(game_checksum)
	{
		case 0x00:
		{
			/*  */
			get_colour_palette(palette,0x1C,0x03);
			break;
		}
		case 0x01:
		{
			/* Arcade Classic No. 4 - Defender & Joust (USA, Europe) */
			get_colour_palette(palette,0x0F,0x05);
			break;
		}
		case 0x0C:
		{
			/* Nigel Mansell's World Championship Racing (Europe) */
			get_colour_palette(palette,0x12,0x00);
			break;
		}
		case 0x0D:
		{
			if(disambiguation_character=='E') {
				/* Pocket Bomberman (Europe) */
				get_colour_palette(palette,0x0C,0x03);
				break;
			} else {
				/* Tetris 2 (Europe) */ 
				/* Tetris 2 (USA) */
				get_colour_palette(palette,0x07,0x04);
				break;
			}

		}
		case 0x10:
		{
			/* Super R.C. Pro-Am (USA, Europe) */
			get_colour_palette(palette,0x0F,0x05);
			break;
		}
		case 0x14:
		{
			/* Game Boy Camera Gold (USA) */
			/* Pocket Monsters Aka (Japan) */
			/* Pocket Monsters Aka (Japan) (Rev A) */
			/* Pokemon - Edicion Roja (Spain) */
			/* Pokemon - Red Version (USA, Europe) */
			/* Pokemon - Rote Edition (Germany) */
			/* Pokemon - Version Rouge (France) */
			/* Pokemon - Versione Rossa (Italy) */
			get_colour_palette(palette,0x10,0x01);
			break;
		}
		case 0x15:
		{
			/* Pocket Monsters - Pikachu (Japan) (Rev 0A) */
			/* Pocket Monsters - Pikachu (Japan) (Rev B) */
			/* Pocket Monsters - Pikachu (Japan) (Rev C) */
			/* Pocket Monsters - Pikachu (Japan) (Rev D) */
			get_colour_palette(palette,0x07,0x00);
			break;
		}
		case 0x16:
		{
			if(disambiguation_character=='M') {
				/* CUSTOM */
				/* Batman - The Animated Series (USA, Europe) */
				get_colour_palette(palette,0x0D,0x05);
				break;
			} else {
				/* Donkey Kong Land (Japan) */
				get_colour_palette(palette,0x0C,0x05);
				break;
			}

			/* Yakuman (Japan);; Yakuman (Japan) (Rev A) */
			get_colour_palette(palette,0x12,0x00);
			break;
		}
		case 0x17:
		{
			/* Othello (Europe) */
			get_colour_palette(palette,0x0E,0x05);
			break;
		}
		case 0x18:
		{
			if(disambiguation_character=='I') {
				/* Wario Blast Featuring Bomberman! (USA, Europe) */
				get_colour_palette(palette,0x1C,0x03);
				break;
			} else {
				/* Donkey Kong Land (Japan) */
				get_colour_palette(palette,0x0C,0x05);
				break;
			}
		}
		case 0x19:
		{
			/* Donkey Kong (World) */
			/* Donkey Kong (World) (Rev A) */
			get_colour_palette(palette,0x06,0x03);
			break;
		}
		case 0x1D:
		{
			/* Kirby no Pinball (Japan) */
			/* Kirby's Pinball Land (USA, Europe) */
			get_colour_palette(palette,0x08,0x03);
			break;
		}
		case 0x27:
		{
			if(disambiguation_character=='B') {
				/* Kirby no Block Ball (Japan) */
				/* Kirby's Block Ball (USA, Europe) */
				get_colour_palette(palette,0x08,0x05);
				break;
			} else {
				/* Magnetic Soccer (Europe) */
				get_colour_palette(palette,0x0E,0x05);
				break;
			}
		}
		case 0x28:
		{
			if(disambiguation_character=='A') {
				/* Arcade Classic No. 3 - Galaga & Galaxian (USA) */
				get_colour_palette(palette,0x13,0x00);
				break;	
			} else {
				/* Golf (World) */
				get_colour_palette(palette,0x0E,0x03);
				break;
			}
		}
		case 0x29:
		{
			/* Mega Man III (Europe) */
			get_colour_palette(palette,0x0F,0x05);
			break;
		}
		case 0x2B:
		{
			/* Mega Man V (USA) */
			get_colour_palette(palette,0x0F,0x05);
			break;
		}
		case 0x34:
		{
			/* Game Boy Gallery (Japan) */
			get_colour_palette(palette,0x04,0x03);
			break;
		}
		case 0x35:
		{
			/* Mario no Picross (Japan) */
			/* Mario's Picross (USA, Europe) */
			get_colour_palette(palette,0x12,0x00);
			break;
		}
		case 0x36:
		{
			/* Baseball (World) */
			get_colour_palette(palette,0x03,0x05);
			break;
		}
		case 0x39:
		{
			/* Dynablaster (Europe) */
			get_colour_palette(palette,0x0F,0x03);
			break;
		}
		case 0x3C:
		{
			/* Dr. Mario (World) */
			/* Dr. Mario (World) (Rev A) */
			get_colour_palette(palette,0x0B,0x02);
			break;
		}
		case 0x3D:
		{
			/* Yoshi (USA) */
			/* Yoshi no Tamago (Japan) */
			get_colour_palette(palette,0x05,0x03);
			break;
		}
		case 0x3E:
		{
			/* Yoshi no Cookie (Japan) */
			get_colour_palette(palette,0x06,0x04);
			break;
		}
		case 0x3F:
		{
			/* Tetris Plus (USA, Europe) */
			get_colour_palette(palette,0x1C,0x03);
			break;
		}
		case 0x43:
		{
			/* Chessmaster, The (Europe) */
			get_colour_palette(palette,0x0F,0x03);
			break;
		}
		case 0x46:
		{
			if(disambiguation_character=='E') {
				/* Super Mario Land (World) */
				/* Super Mario Land (World) (Rev A) */
				get_colour_palette(palette,0x0A,0x03);
				break;
			} else {
				/* Metroid II - Return of Samus (World) */
				get_colour_palette(palette,0x14,0x05);
				break;
			}
		}
		case 0x49:
		{
			/* Kirby's Dream Land (USA, Europe) */
			get_colour_palette(palette,0x08,0x05);
			break;
		}
		case 0x4B:
		{
			/* Play Action Football (USA) */
			get_colour_palette(palette,0x0E,0x03);
			break;
		}
		case 0x4E:
		{
			/* Wave Race (USA, Europe) */
			get_colour_palette(palette,0x0B,0x05);
			break;
		}
		case 0x50:
		{
			/* Castlevania II - Belmont's Revenge (USA, Europe) */
			get_colour_palette(palette,0x0C,0x05);
			break;
		}
		case 0x52:
		{
			/* Street Fighter II (USA, Europe) (Rev A) */
			get_colour_palette(palette,0x0F,0x05);
			break;
		}
		case 0x58:
		{
			/* X (Japan) */
			get_colour_palette(palette,0x16,0x00);
			break;
		}
		case 0x59:
		{
			/* Wario Land - Super Mario Land 3 (World) */
			get_colour_palette(palette,0x00,0x05);
			break;
		}
		case 0x5C:
		{
			/* Hoshi no Kirby (Japan) */ 
			/* Hoshi no Kirby (Japan) (Rev A) */
			get_colour_palette(palette,0x08,0x05);
			break;
		}
		case 0x5D:
		{
			/* Battle Arena Toshinden (USA) */
			get_colour_palette(palette,0x0F,0x05);
			break;
		}
		case 0x61:
		{
			if(disambiguation_character=='A') {
				/* Vegas Stakes (USA, Europe) */
			 	get_colour_palette(palette,0x0E,0x05);
			 	break;
			} else {
				/* Pocket Monsters Ao (Japan) */
				/* Pokemon - Blaue Edition (Germany) */
				/* Pokemon - Blue Version (USA, Europe) */
				/* Pokemon - Edicion Azul (Spain) */
				/* Pokemon - Version Bleue (France) */
				/* Pokemon - Versione Blu (Italy) */
				get_colour_palette(palette,0x0B,0x01);
				break;
			}
		}
		case 0x66:
		{
			if(disambiguation_character=='E') {
				/* Game Boy Gallery 2 (Australia) */
				/* Game Boy Gallery 2 (Japan) */
				get_colour_palette(palette,0x04,0x03);
				break;
			} else {
				/* Arcade Classic No. 2 - Centipede & Millipede (USA, Europe) */
				get_colour_palette(palette,0x1C,0x03);
				break;
			}
		}
		case 0x67:
		{
			/* Kirby's Star Stacker (USA, Europe) */
			get_colour_palette(palette,0x12,0x00);
			break;
		}
		case 0x68:
		{
			/* Adventures of Lolo (Europe) */
			/* Mega Man II (Europe) */
			get_colour_palette(palette,0x0F,0x05);
			break;
		}
		case 0x69:
		{
			/* Tetris Flash (Japan) */
			get_colour_palette(palette,0x07,0x04);
			break;
		}
		case 0x6A:
		{
			if(disambiguation_character=='K') {
				/* Donkey Kong Land 2 (USA, Europe) */
				get_colour_palette(palette,0x0C,0x05);
				break;
			} else {
				/* Mario & Yoshi (Europe) */
				get_colour_palette(palette,0x05,0x03);
				break;
			}
		}
		case 0x6B:
		{
			/* Castlevania Adventure, The (USA) */
			/* Donkey Kong Land III (USA, Europe) */
			/* Donkey Kong Land III (USA, Europe) (Rev A) */
			get_colour_palette(palette,0x0C,0x05);
			break;
		}
		case 0x6D:
		{
			/* King of Fighters '95, The (USA) */
			get_colour_palette(palette,0x0F,0x05);
			break;
		}
		case 0x6F:
		{
			/* Pocket Camera (Japan) (Rev A) */
			get_colour_palette(palette,0x1B,0x00);
			break;
		}
		case 0x70:
		{
			/* Legend of Zelda, The - Link's Awakening (France) */
			/* Legend of Zelda, The - Link's Awakening (Germany) */
			/* Legend of Zelda, The - Link's Awakening (USA, Europe) */
			/* Legend of Zelda, The - Link's Awakening (USA, Europe) (Rev A) */
			/* Legend of Zelda, The - Link's Awakening (USA, Europe) (Rev B) */
			/* Zelda no Densetsu - Yume o Miru Shima (Japan) */
			/* Zelda no Densetsu - Yume o Miru Shima (Japan) (Rev A) */
			get_colour_palette(palette,0x11,0x05);
			break;
		}
		case 0x71:
		{
			/* Tetris Blast (USA, Europe) */
			get_colour_palette(palette,0x06,0x00);
			break;
		}
		case 0x75:
		{
			/* Picross 2 (Japan) */
			get_colour_palette(palette,0x12,0x00);
			break;
		}
		case 0x86:
		{
			/* Donkey Kong Land (USA, Europe) */
			get_colour_palette(palette,0x01,0x05);
			break;
		}
		case 0x88:
		{
			/* Alleyway (World) */
			get_colour_palette(palette,0x08,0x00);
			break;
		}
		case 0x8B:
		{
			/* Mystic Quest (Europe);; Mystic Quest (France) */
			/* Mystic Quest (Germany) */
			get_colour_palette(palette,0x0E,0x05);
			break;
		}
		case 0x8C:
		{
			/* Radar Mission (Japan) */
			/* Radar Mission (USA, Europe) */
			get_colour_palette(palette,0x00,0x01);
			break;
		}
		case 0x90:
		{
			/* Nintendo World Cup (USA, Europe) */
			get_colour_palette(palette,0x0E,0x03);
			break;
		}
		case 0x92:
		{
			/* F-1 Race (World);; F-1 Race (World) (Rev A) */
			get_colour_palette(palette,0x12,0x00);
			break;
		}
		case 0x95:
		{
			/* Yoshi no Panepon (Japan) */
			get_colour_palette(palette,0x05,0x04);
			break;
		}
		case 0x97:
		{
			/* King of the Zoo (Europe) */
			get_colour_palette(palette,0x0F,0x03);
			break;
		}
		case 0x99:
		{
			/* Kirby no Kirakira Kids (Japan) */
			get_colour_palette(palette,0x12,0x00);
			break;
		}
		case 0x9A:
		{
			/* Arcade Classic No. 1 - Asteroids & Missile Command (USA, Europe) */
			get_colour_palette(palette,0x0E,0x03);
			break;
		}
		case 0x9C:
		{
			/* Pinocchio (Europe) */
			get_colour_palette(palette,0x0C,0x02);
			break;
		}
		case 0x9D:
		{
			/* Killer Instinct (USA, Europe) */
			get_colour_palette(palette,0x0D,0x05);
			break;
		}
		case 0xA2:
		{
			/* Star Wars (USA, Europe) (Rev A) */
			get_colour_palette(palette,0x12,0x05);
			break;
		}
		case 0xA5:
		{
			if(disambiguation_character=='R') {
				/* Battletoads in Ragnarok's World (Europe) */
				get_colour_palette(palette,0x12,0x03);
				break;
			} else {
				/* Solar Striker (World) */
				get_colour_palette(palette,0x13,0x00);
				break;
			}
		}
		case 0xA8:
		{
			/* Super Donkey Kong GB (Japan) */
			get_colour_palette(palette,0x01,0x05);
			break;
		}
		case 0xAA:
		{
			/* James Bond 007 (USA, Europe) */
			/* Pocket Monsters Midori (Japan) */
			/* Pocket Monsters Midori (Japan) (Rev A) */
			get_colour_palette(palette,0x1C,0x01);
			break;
		}
		case 0xB3:
		{
			if(disambiguation_character=='U') {
				/* Moguranya (Japan) */
				/* Mole Mania (USA, Europe) */
				get_colour_palette(palette,0x00,0x03);
				break;
			} else if(disambiguation_character=='R') {
				/* Tetris Attack (USA) */
				/* Tetris Attack (USA, Europe) (Rev A) */
				get_colour_palette(palette,0x05,0x04);
				break;
			} else {
				/* Hoshi no Kirby 2 (Japan) */
				/* Kirby's Dream Land 2 (USA, Europe) */
				get_colour_palette(palette,0x08,0x05);
				break;
			}
		}
		case 0xB7:
		{
			/* Game Boy Gallery (Europe) */
			get_colour_palette(palette,0x12,0x00);
			break;
		}
		case 0xBD:
		{
			/* Toy Story (Europe) */
			get_colour_palette(palette,0x0E,0x03);
			break;
		}
		case 0xBF:
		{
			if(disambiguation_character=='C') {
				/* Soccer (Europe) (En,Fr,De) */
				get_colour_palette(palette,0x02,0x05);
				break;
			} else {
				/* Kid Icarus - Of Myths and Monsters (USA, Europe) */
				get_colour_palette(palette,0x0D,0x03);
				break;
			}
		}
		case 0xC6:
		{
			if(disambiguation_character==' ') {
				/* Ken Griffey Jr. presents Major League Baseball (USA, Europe) */
				get_colour_palette(palette,0x1C,0x03);
				break;
			} else {
				/* Game Boy Wars (Japan) */
				get_colour_palette(palette,0x00,0x05);
				break;
			}
		}
		case 0xC9:
		{
			/* Super Mario Land 2 - 6 Golden Coins (USA, Europe) */
			/* Super Mario Land 2 - 6 Golden Coins (USA, Europe) (Rev A) */
			/* Super Mario Land 2 - 6 Golden Coins (USA, Europe) (Rev B) */
			/* Super Mario Land 2 - 6-tsu no Kinka (Japan) */
			/* Super Mario Land 2 - 6-tsu no Kinka (Japan) (Rev B) */
			get_colour_palette(palette,0x09,0x05);
			break;
		}
		case 0xCE:
		{
			/* Top Ranking Tennis (Europe) */
			get_colour_palette(palette,0x02,0x05);
			break;
		}
		case 0xD1:
		{
			/* Tennis (World) */
			get_colour_palette(palette,0x02,0x05);
			break;
		}
		case 0xD3:
		{
			if(disambiguation_character=='R') {
				/* Kaeru no Tame ni Kane wa Naru (Japan) */
				get_colour_palette(palette,0x0D,0x01);
				break;
			} else {
				/* Wario Land II (USA, Europe) */
				get_colour_palette(palette,0x15,0x05);
				break;
			}
		}
		case 0xDB:
		{
			/* Tetris (World);; Tetris (World) (Rev A) */
			get_colour_palette(palette,0x07,0x00);
			break;
		}
		case 0xE0:
		{
			/* Yoshi's Cookie (USA, Europe) */
			get_colour_palette(palette,0x06,0x04);
			break;
		}
		case 0xE8:
		{
			/* Space Invaders (Europe) */
			/* Space Invaders (USA) */
			get_colour_palette(palette,0x13,0x00);
			break;
		}
		case 0xF0:
		{
			/* Top Rank Tennis (USA) */
			get_colour_palette(palette,0x02,0x05);
			break;
		}
		case 0xF2:
		{
			/* Qix (World) */
			get_colour_palette(palette,0x07,0x04);
			break;
		}
		case 0xF4:
		{
			if(disambiguation_character==' ') {
				/* Game & Watch Gallery (Europe)*/
				/* Game & Watch Gallery (USA) */
				/* Game & Watch Gallery (USA) (Rev A) */
				get_colour_palette(palette,0x04,0x03);
				break;	
			} else {
				/* Pac-In-Time (USA) */
				get_colour_palette(palette,0x1C,0x05);
				break;
			}
		}
		case 0xF6:
		{
			/* Mega Man - Dr. Wily's Revenge (Europe) */
			get_colour_palette(palette,0x0F,0x05);
			break;
		}
		case 0xF7:
		{
			/* Boy and His Blob in the Rescue of Princess Blobette, A (Europe) */
			get_colour_palette(palette,0x12,0x05);
			break;
		}
		case 0xFF:
		{
			/* Balloon Kid (USA, Europe) */
			get_colour_palette(palette,0x06,0x00);
			break;
		}
		default:
		{
			printf("E auto_assign_palette: No palette found for checksum 0x%02X.\n", game_checksum);
			/* Original Game Boy DMG color palette (monochrome 4-shades of green!) */
 			get_colour_palette(palette,0xFF,0xFF);
			break;
		}
	}
}

/**
 * Manually assigns a palette. This is used to allow the user to manually select a
 * different colour palette if one was not found automatically, or if the user
 * prefers a different colour palette.
 * selection is the requested colour palette. This should be a maximum of
 * NUMBER_OF_MANUAL_PALETTES - 1. The default palette is selected otherwise.
 * The entries are listed here in the order they are stored in the ROM.
 * 
 * A total of twelve palette configurations are in here. 
 * Six of these are unique to the manual selection mode. 
 * The button combinations trigger the respective configuration 
 * and are previewed in the respective shades while the Nintendo
 * Game Boy Color logo is still being displayed on the screen.
 */
void manual_assign_palette(palette_t palette, uint8_t selection)
{
	printf("I manual_assign_palette(%d)\n", selection);
	switch(selection)
	{
		case 0:
		{
			/* Right */
			get_colour_palette(palette,0x05,0x00);
			break;
		}
		case 1:
		{
			/* A + Down */
			get_colour_palette(palette,0x07,0x00);
			break;
		}
		case 2:
		{
			/* Up */
			get_colour_palette(palette,0x12,0x00);
			break;
		}
		case 3:
		{
			/* B + Right */
			get_colour_palette(palette,0x13,0x00);
			break;
		}
		case 4:
		{
			/* B + Left Game Boy Pocket (monochrome 4-shades of white) */
			get_colour_palette(palette,0x16,0x00);
			break;
		}
		case 5:
		{
			/* Down */
			get_colour_palette(palette,0x17,0x00);
			break;
		}
		case 6:
		{
			/* B + Up */
			get_colour_palette(palette,0x19,0x03);
			break;
		}
		case 7:
		{
			/* A + Right */
			get_colour_palette(palette,0x1C,0x03);
			break;
		}
		case 8:
		{
			/* A + Left */
			get_colour_palette(palette,0x0D,0x05);
			break;
		}
		case 9:
		{
			/* A + Up */
			get_colour_palette(palette,0x10,0x05);
			break;
		}
		case 10:
		{
			/* Left */
			get_colour_palette(palette,0x18,0x05);
			break;
		}
		case 11:
		{
			/* B + Down */
			get_colour_palette(palette,0x1A,0x05);
			break;	
		}
	 	case 12:
 		{
			/* A + B Original Game Boy DMG color palette (monochrome 4-shades of green!) */
 			get_colour_palette(palette,0xFF,0xFF);
 			break;
		}
		default:
		{
			/* Original Game Boy DMG color palette (monochrome 4-shades of green!) */
 			get_colour_palette(palette,0xFF,0xFF);
 			break;
		}
	}
}