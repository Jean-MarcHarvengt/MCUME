/*
    Copyright (C) 2000, 2001  Charles MacDonald

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "shared.h"

t_bitmap bitmap;
t_input input;
t_snd pcesnd;


/* Pass 0 for no sound, or 8000-44100 for desired sample rate */
/* No error checking at the moment... */
int system_init(int sample_rate)
{
    pce_init();
    vdc_init();
    psg_init();
    render_init();
#ifdef SOUND_PRESENT
    audio_init(sample_rate);
#endif
    return (1);
}

#ifdef SOUND_PRESENT
void audio_init(int rate)
{
    memset(&pcesnd, 0, sizeof(pcesnd));

    /* Exit if no sound or invalid sample rate */
    if(!rate || ((rate < 8000) || (rate > 44100))) return;
    else
    {
        /* Buffer size = sample rate / frames per second */
        //pcesnd.buffer_size = (rate / 60);

        /* Keep local copy of sample rate for sound emulation */
        //pcesnd.sample_rate = rate;

        /* Allocate left channel buffer */
        //pcesnd.buffer[0] = emu_Malloc(pcesnd.buffer_size * sizeof(int16));
        //if(!pcesnd.buffer[0]) return;
        /* Allocate right channel buffer */
        //pcesnd.buffer[1] = emu_Malloc(pcesnd.buffer_size * sizeof(int16));
        //if(!pcesnd.buffer[1]) return;

        /* Set audio enable flag */
        pcesnd.enabled = 1;
    }
}
#endif

void system_frame(int skip)
{
    int line;

    for(y_offset = byr, line = 0; line < 262; line += 1)
    {
//        DEBUGLOG("system_frame 1\n");
        if((line + 64) == (reg[6] & 0x3FF))
        {
            if(reg[5] & 0x04)
            {
                status |= STATUS_RR;
                h6280_set_irq_line(0, ASSERT_LINE);
            }
        }
//        DEBUGLOG("system_frame 2\n");

        /* VBlank */
        if(line == 240)
        {
            if(dvssr_trigger || (reg[0x0F] & 0x10))
            {
                /* Clear DVSSR write trigger */
                dvssr_trigger = 0;

                /* Copy VRAM to object RAM */
                memcpy(objram, &vram[(reg[0x13] << 1) & 0xFFFE], 0x200);

                /* Cause transfer complete interrupt if necessary */
                if(reg[0x0F] & 0x01)
                {
                    status |= STATUS_DS;
                    h6280_set_irq_line(0, ASSERT_LINE);
                }

                /* Precalculate sprite data for the next frame */
                make_sprite_list();
            }

            /* Cause VBlank interrupt if necessary */
            if(reg[5] & 0x0008)
            {
                status |= STATUS_VD;
                h6280_set_irq_line(0, ASSERT_LINE);
            }
        }
//        DEBUGLOG("system_frame 3\n");

        /* 7.16 MHz = 455 cycles per line */
        h6280_execute(455); 
//        DEBUGLOG("system_frame 4\n");

        /* Render a line of the display */
        if((line < disp_height) && (!skip)) {

            render_line(line);
        }

//        DEBUGLOG("system_frame 5\n");

        /* Update internal line counter and wrap */
        y_offset = (y_offset + 1) & playfield_col_mask;

    }

}

void pce_sound(void)
{
    /* Update audio */
    if(pcesnd.enabled) psg_update(pcesnd.buffer[0], pcesnd.buffer[1], pcesnd.buffer_size);
}

void system_reset(void)
{
    pce_reset();
    vdc_reset();
    psg_reset();
    render_reset();
}


void system_shutdown(void)
{
    pce_shutdown();
    vdc_shutdown();
    psg_shutdown();
    render_shutdown();
}
