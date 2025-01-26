/*
    sound.c
    YM2612 and SN76489 emulation
*/

#include "shared.h"

/* YM2612 data */
int fm_timera_tab[0x400];   /* Precalculated timer A values */
int fm_timerb_tab[0x100];   /* Precalculated timer B values */
uint8 fm_reg[2][0x100];     /* Register arrays (2x256) */
uint8 fm_latch[2];          /* Register latches */
uint8 fm_status;            /* Read-only status flags */
t_timer timer[2];           /* Timers A and B */

/* Initialize the YM2612 and SN76489 emulation */
void sound_init(void)
{
    /* Timers run at half the YM2612 input clock */
    float clock = ((53.693175 / 7) / 2);
    int i;

    /* Make Timer A table */
    for(i = 0; i < 1024; i += 1)
    {
      /* Formula is "time(us) = 72 * (1024 - A) / clock" */
      fm_timera_tab[i] = ((int)(float)(72 * (1024 - i)) / (clock));
    }

    /* Make Timer B table */
    for(i = 0; i < 256; i += 1)
    {
        /* Formula is "time(us) = 1152 * (256 - B) / clock" */
        fm_timerb_tab[i] = (int)(float)(1152 * (256 - i)) / clock;
    }
}

void sound_reset(void)
{
    if(snd.enabled)
    {
        YM2612ResetChip(0);
    }
}

void fm_write(int address, int data)
{
    int a0 = (address & 1);
    int a1 = (address >> 1) & 1;

    if(a0)
    {
        /* Register data */
        fm_reg[a1][fm_latch[a1]] = data;

        /* Timer control only in set A */
        if(a1 == 0)
        switch(fm_latch[a1])
        {
            case 0x24: /* Timer A (LSB) */
                timer[0].index = (timer[0].index & 0x0003) | (data << 2);
                timer[0].index &= 0x03FF;
                timer[0].base = fm_timera_tab[timer[0].index];
                break;

            case 0x25: /* Timer A (MSB) */
                timer[0].index = (timer[0].index & 0x03FC) | (data & 3);
                timer[0].index &= 0x03FF;
                timer[0].base = fm_timera_tab[timer[0].index];
                break;
            
            case 0x26: /* Timer B */
                timer[1].index = data;
                timer[1].base = timer[1].count = fm_timerb_tab[timer[1].index];
                break;
 
            case 0x27: /* Timer Control */

                /* LOAD */
                timer[0].running = (data >> 0) & 1;
                if(timer[0].running) timer[0].count = 0;
                timer[1].running = (data >> 1) & 1;
                if(timer[1].running) timer[1].count = 0;

                /* ENABLE */
                timer[0].enable = (data >> 2) & 1;
                timer[1].enable = (data >> 3) & 1;

                /* RESET */
                if(data & 0x10) fm_status &= ~1;
                if(data & 0x20) fm_status &= ~2;
                break;
        }
    }
    else
    {
        /* Register latch */
        fm_latch[a1] = data;
    }

    if(snd.enabled)
    {
        if(snd.fm.curStage - snd.fm.lastStage > 1)
        {
            int16 *tempBuffer[2];       
            tempBuffer[0] = snd.fm.buffer[0] + snd.fm.lastStage;
            tempBuffer[1] = snd.fm.buffer[1] + snd.fm.lastStage;
            YM2612UpdateOne(0, (int16 **)tempBuffer, snd.fm.curStage - snd.fm.lastStage);
            snd.fm.lastStage = snd.fm.curStage;
        }

        YM2612Write(0, address & 3, data);
    }
}


int fm_read(int address)
{
    return (fm_status);
}


void fm_update_timers(void)
{
    int i;    

    /* Process YM2612 timers */
    for(i = 0; i < 2; i += 1)
    {
        /* Is the timer running? */
        if(timer[i].running)
        {
            /* Each scanline takes up roughly 64 microseconds */
            timer[i].count += 64;

            /* Check if the counter overflowed */
            if(timer[i].count > timer[i].base)
            {
                /* Reload counter */
                timer[i].count = 0;

                /* Disable timer */
                timer[i].running = 0;

                /* Set overflow flag (if flag setting is enabled) */
                if(timer[i].enable)
                {
                    fm_status |= (1 << i);
                }
            }
        }
    }
}

void psg_write(int data)
{
    if(snd.enabled)
    {
        if(snd.psg.curStage - snd.psg.lastStage > 1)
        {
            int16 *tempBuffer;
            tempBuffer = snd.psg.buffer + snd.psg.lastStage;
            SN76496Update(0, tempBuffer, snd.psg.curStage - snd.psg.lastStage);
            snd.psg.lastStage = snd.psg.curStage;
        }

        SN76496Write(0, data);
    }
}

