/*
    Copyright (C) 1998, 1999, 2000  Charles Mac Donald

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


t_bitmap sbitmap;
t_cart cart;                
t_snd smssnd;
static t_input minput;
FM_OPL *ym3812;


void system_init(int rate)
{
    /* Initialize the VDP emulation */
    vdp_init();

    /* Initialize the SMS emulation */
    sms_init();

    /* Initialize the look-up tables and related data */
    render_init();

    /* Enable sound emulation if the sample rate was specified */
    audio_init(rate);

    /* Don't save SRAM by default */
    sms.save = 0;

    /* Clear emulated button state */
    memset(&minput, 0, sizeof(t_input));
}


void audio_init(int rate)
{
    /* Clear sound context */
    memset(&smssnd, 0, sizeof(t_snd));

    /* Reset logging data */
    smssnd.log = 0;
    smssnd.callback = NULL;

    /* Oops.. sound is disabled */
    if(!rate) return;

    /* Calculate buffer size in samples */
    smssnd.bufsize = (rate / 50);

    /* YM3812/YM2413 sound stream */
    smssnd.fm_buffer = (signed short int *)emu_Malloc(smssnd.bufsize * 2);
    if(!smssnd.fm_buffer) return;

    /* SN76489 sound stream */
    smssnd.psg_buffer[0] = (signed short int *)emu_Malloc(smssnd.bufsize * 2);
    smssnd.psg_buffer[1] = (signed short int *)emu_Malloc(smssnd.bufsize * 2);
    if(!smssnd.psg_buffer[0] || !smssnd.psg_buffer[1]) return;

    /* Set up SN76489 emulation */
    SN76496_init(0, MASTER_CLOCK, 255, rate);

    /* Set up YM3812 emulation */
    ym3812 = OPLCreate(OPL_TYPE_YM3812, MASTER_CLOCK, rate);
    if(!ym3812) return;

    /* Set up YM2413 emulation */
    ym2413_init(1);

    /* Inform other functions that we can use sound */
    smssnd.enabled = 1;
}


void system_shutdown(void)
{
    if(smssnd.enabled) OPLDestroy(ym3812);
}


void system_reset(void)
{
    cpu_reset();
    vdp_reset();
    sms_reset();
    render_reset();
//    system_load_sram();
    if(smssnd.enabled)
    {
        OPLResetChip(ym3812);
        ym2413_reset(0);
    }
}


void system_save_state(void *fd)
{
    /* Save VDP context */
    fwrite(&vdp, sizeof(t_vdp), 1, fd);

    /* Save SMS context */
    fwrite(&sms, sizeof(t_sms), 1, fd);

    /* Save Z80 context */
    fwrite(Z80_Context, sizeof(Z80_Regs), 1, fd);
    fwrite(&after_EI, sizeof(int), 1, fd);

    /* Save YM2413 registers */
    fwrite(&ym2413[0].reg[0], 0x40, 1, fd);

    /* Save SN76489 context */
    fwrite(&sn[0], sizeof(t_SN76496), 1, fd);
}


void system_load_state(void *fd)
{
    int i;
    byte reg[0x40];

    /* Initialize everything */
    cpu_reset();
    system_reset();

    /* Load VDP context */
    fread(&vdp, sizeof(t_vdp), 1, fd);

    /* Load SMS context */
    fread(&sms, sizeof(t_sms), 1, fd);

    /* Load Z80 context */
    fread(Z80_Context, sizeof(Z80_Regs), 1, fd);
    fread(&after_EI, sizeof(int), 1, fd);

    /* Load YM2413 registers */
    fread(reg, 0x40, 1, fd);

    /* Load SN76489 context */
    fread(&sn[0], sizeof(t_SN76496), 1, fd);

    /* Restore callbacks */
    z80_set_irq_callback(sms_irq_callback);

    cpu_readmap[0] = 0x0000; /* 0000-3FFF */
    cpu_readmap[1] = 0x2000;
    cpu_readmap[2] = 0x4000; /* 4000-7FFF */
    cpu_readmap[3] = 0x6000;
    cpu_readmap[4] = 0x0000; /* 0000-3FFF */
    cpu_readmap[5] = 0x2000;
    cpu_readmap[6] = sms.ram;
    cpu_readmap[7] = sms.ram;

    cpu_writemap[0] = sms.dummy;
    cpu_writemap[1] = sms.dummy;
    cpu_writemap[2] = sms.dummy;         
    cpu_writemap[3] = sms.dummy;
    cpu_writemap[4] = sms.dummy;         
    cpu_writemap[5] = sms.dummy;
    cpu_writemap[6] = sms.ram;           
    cpu_writemap[7] = sms.ram;

    sms_mapper_w(3, sms.fcr[3]);
    sms_mapper_w(2, sms.fcr[2]);
    sms_mapper_w(1, sms.fcr[1]);
    sms_mapper_w(0, sms.fcr[0]);

    /* Force full pattern cache update */
    sis_vram_dirty = 1;
    memset(svram_dirty, 1, 0x200);

    /* Restore palette */
    for(i = 0; i < PALETTE_SIZE; i += 1)
        palette_sync(i);

    /* Restore sound state */
    if(smssnd.enabled)
    {
        /* Restore YM2413 emulation */
        OPLResetChip(ym3812);

        /* Clear YM2413 context */
        ym2413_reset(0);

        /* Restore rhythm enable first */
        ym2413_write(0, 0, 0x0E);
        ym2413_write(0, 1, reg[0x0E]);

        /* User instrument settings */
        for(i = 0x00; i <= 0x07; i += 1)
        {
            ym2413_write(0, 0, i);
            ym2413_write(0, 1, reg[i]);
        }

        /* Channel frequency */
        for(i = 0x10; i <= 0x18; i += 1)
        {
            ym2413_write(0, 0, i);
            ym2413_write(0, 1, reg[i]);
        }

        /* Channel frequency + ctrl. */
        for(i = 0x20; i <= 0x28; i += 1)
        {
            ym2413_write(0, 0, i);
            ym2413_write(0, 1, reg[i]);
        }

        /* Instrument and volume settings  */
        for(i = 0x30; i <= 0x38; i += 1)
        {
            ym2413_write(0, 0, i);
            ym2413_write(0, 1, reg[i]);
        }
    }
}
