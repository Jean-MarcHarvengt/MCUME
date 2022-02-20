/*
    Copyright (C) 2000  Charles Mac Donald

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

    YM2413 (OPLL) emulator
    by Charles Mac Donald
    E-mail: cgfm2@hooked.net
    WWW: http://cgfm2.emuviews.com

    Change log:

    [061300]
    - Fixed bug where channel numbers larger than 9 could be written to
      register groups $10-18, $20-28, $30-38.

    [060800]
    - Now the YM2413 chip number is passed to the OPL_WRITE macro,
      and the user instrument data is stored in the YM2413 context,
      both for multiple YM2413 emulation.

    [060100]
    - Added alternate instrument table taken from Allegro's 'fm_inst.h'.
    - Changed source so it can compile seperately from SMS Plus.
    - Added 'ym2413_reset' function and changed ym2413_init.
    
    Known issues:

    - The sustain on/off flag (bit 5 of register group $20-28) is not
      emulated. According to the manual, the release rate is set to
      five when this bit is set.

    - The table of fixed instrument values probably need to be compared
      against a real YM2413, so they can be hand-tuned.

    - The rhythm instruments sound good, but are too loud.
      The same settings are used for channels 7, 8, 9, which can't be right.

    I based the YM2413 emulation on the following documents. If you want
    to improve it or make changes, I'd advise reading the following:

    - Yamaha's YMF-272 (OPL-3) programmer's manual. (ymf272.pdf)
      (Has useful table of how the operators map to YM3812 registers)

    - Yamaha's YM2413 programmer's manual. (ym2413.lzh)

    - Vladimir Arnost's OPL-3 programmer's guide. (opl3.txt)
      (Explains operator allocation in rhythm mode)

    - The YM2413 emulation from MAME. (ym2413.c/2413intf.h)
*/

#include "shared.h"

/* You can replace this to output to another YM3812 emulator
   or a perhaps a real OPL-2/OPL-3 sound chip */
#if USE_ADLIB
#define OPL_WRITE(c,r,d)  { outp(0x388+c*2, r); outp(0x389+c*2, d); }
#else
#define OPL_WRITE(c,r,d)  OPLWriteReg(ym3812, r, d)
#endif

/* YM2413 chip contexts */
t_ym2413 ym2413[MAX_YM2413];

/* Fixed instrument settings, from MAME's YM2413 emulation */
/* This might need some tweaking... */
unsigned char table[16][11] =
{
   /*   20    23    40    43    60    63    80    83    E0    E3    C0 */
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
#if 1 /* Instrument settings from MAME */
    { 0x01, 0x22, 0x23, 0x07, 0xF0, 0xF0, 0x07, 0x18, 0x00, 0x00, 0x00 },
    { 0x23, 0x01, 0x68, 0x05, 0xF2, 0x74, 0x6C, 0x89, 0x00, 0x00, 0x00 },
    { 0x13, 0x11, 0x25, 0x00, 0xD2, 0xB2, 0xF4, 0xF4, 0x00, 0x00, 0x00 },
    { 0x22, 0x21, 0x1B, 0x05, 0xC0, 0xA1, 0x18, 0x08, 0x00, 0x00, 0x00 },
    { 0x22, 0x21, 0x2C, 0x03, 0xD2, 0xA1, 0x18, 0x57, 0x00, 0x00, 0x00 },
    { 0x01, 0x22, 0xBA, 0x01, 0xF1, 0xF1, 0x1E, 0x04, 0x00, 0x00, 0x00 },
    { 0x21, 0x21, 0x28, 0x06, 0xF1, 0xF1, 0x6B, 0x3E, 0x00, 0x00, 0x00 },
    { 0x27, 0x21, 0x60, 0x00, 0xF0, 0xF0, 0x0D, 0x0F, 0x00, 0x00, 0x00 },
    { 0x20, 0x21, 0x2B, 0x06, 0x85, 0xF1, 0x6D, 0x89, 0x00, 0x00, 0x00 },
    { 0x01, 0x21, 0xBF, 0x02, 0x53, 0x62, 0x5F, 0xAE, 0x01, 0x00, 0x00 },
    { 0x23, 0x21, 0x70, 0x07, 0xD4, 0xA3, 0x4E, 0x64, 0x01, 0x00, 0x00 },
    { 0x2B, 0x21, 0xA4, 0x07, 0xF6, 0x93, 0x5C, 0x4D, 0x00, 0x00, 0x00 },
    { 0x21, 0x23, 0xAD, 0x07, 0x77, 0xF1, 0x18, 0x37, 0x00, 0x00, 0x00 },
    { 0x21, 0x21, 0x2A, 0x03, 0xF3, 0xE2, 0x29, 0x46, 0x00, 0x00, 0x00 },
    { 0x21, 0x23, 0x37, 0x03, 0xF3, 0xE2, 0x29, 0x46, 0x00, 0x00, 0x00 },
#else /* Instrument settings from Allegro */
    { 0x31, 0x21, 0x15, 0x09, 0xdd, 0x56, 0x13, 0x26, 0x01, 0x00, 0x08 }, /* Violin */
    { 0x03, 0x11, 0x54, 0x09, 0xf3, 0xf1, 0x9a, 0xe7, 0x01, 0x00, 0x0c }, /* Acoustic Guitar(steel) */
    { 0x21, 0x21, 0x8f, 0x0c, 0xf2, 0xf2, 0x45, 0x76, 0x00, 0x00, 0x08 }, /* Acoustic Grand */
    { 0xe1, 0xe1, 0x46, 0x09, 0x88, 0x65, 0x5f, 0x1a, 0x00, 0x00, 0x00 }, /* Flute */
    { 0x32, 0x21, 0x90, 0x09, 0x9b, 0x72, 0x21, 0x17, 0x00, 0x00, 0x04 }, /* Clarinet */
    { 0x21, 0x21, 0x4b, 0x09, 0xaa, 0x8f, 0x16, 0x0a, 0x01, 0x00, 0x08 }, /* Oboe */
    { 0x21, 0x21, 0x92, 0x0a, 0x85, 0x8f, 0x17, 0x09, 0x00, 0x00, 0x0c }, /* Trumpet */
    { 0x23, 0xb1, 0x93, 0x09, 0x97, 0x55, 0x23, 0x14, 0x01, 0x00, 0x04 }, /* Church Organ */
    { 0x21, 0x21, 0x9b, 0x09, 0x61, 0x7f, 0x6a, 0x0a, 0x00, 0x00, 0x02 }, /* French Horn */
    { 0x71, 0x72, 0x57, 0x09, 0x54, 0x7a, 0x05, 0x05, 0x00, 0x00, 0x0c }, /* Synth Voice */
    { 0x21, 0x36, 0x80, 0x17, 0xa2, 0xf1, 0x01, 0xd5, 0x00, 0x00, 0x08 }, /* Harpsichord */
    { 0x18, 0x81, 0x62, 0x09, 0xf3, 0xf2, 0xe6, 0xf6, 0x00, 0x00, 0x00 }, /* Vibraphone */
    { 0x31, 0x31, 0x8b, 0x09, 0xf4, 0xf1, 0xe8, 0x78, 0x00, 0x00, 0x0a }, /* Synth Bass 1 */
    { 0x21, 0xa2, 0x1e, 0x09, 0x94, 0xc3, 0x06, 0xa6, 0x00, 0x00, 0x02 }, /* Acoustic Bass */
    { 0x03, 0x21, 0x87, 0x89, 0xf6, 0xf3, 0x22, 0xf8, 0x01, 0x00, 0x06 }, /* Electric Guitar(clean) */
#endif
};


/*--------------------------------------------------------------------------*/


void ym2413_init(int count)
{
    int n;
    for(n = 0; n < count; n += 1)
    {
        /* Reset YM2413 data */
        ym2413_reset(n);
    }
}


void ym2413_reset(int chip)
{
    int n;

    /* Point to current YM2413 context */
    t_ym2413 *opll = &ym2413[chip];

    /* Clear channel data context */
    memset(opll, 0, sizeof(t_ym2413));

    /* Clear all YM3812 registers */
    for(n = 0; n < 0x100; n += 1)
    {
        OPL_WRITE(chip, n, 0x00);
    }

    /* Turn off rhythm mode and key-on bits */
    opll->rhythm = 0;
    OPL_WRITE(chip, 0xBD, 0x00);

    /* Enable waveform select */
    OPL_WRITE(chip, 0x01, 0x20);
}


void ym2413_write(int chip, int address, int data)
{
    /* Point to current YM2413 context */
    t_ym2413 *opll = &ym2413[chip];

    if(address & 1) /* Data port */
    {
        /* Store register data */
        opll->reg[opll->latch] = data;

        switch(opll->latch & 0x30)
        {
            case 0x00: /* User instrument registers */
                switch(opll->latch & 0x0F)
                {
                    case 0x00: /* Misc. ctrl. (modulator) */
                    case 0x01: /* Misc. ctrl. (carrier) */
                    case 0x02: /* Key scale level and total level (modulator) */
                    case 0x04: /* Attack / Decay (modulator) */
                    case 0x05: /* Attack / Decay (carrier) */
                    case 0x06: /* Sustain / Release (modulator) */
                    case 0x07: /* Sustain / Release (carrier) */
                        opll->user[(opll->latch & 0x07)] = data;
                        break;
    
                    case 0x03: /* Key scale level, carrier/modulator waveform, feedback */
    
                        /* Key scale level (carrier) */
                        /* Don't touch the total level (channel volume) */
                        opll->user[3] = (opll->user[3] & 0x3F) | (data & 0xC0);
    
                        /* Waveform select for the modulator */
                        opll->user[8] = (data >> 3) & 1;
    
                        /* Waveform select for the carrier */
                        opll->user[9] = (data >> 4) & 1;
    
                        /* Store feedback level in YM3812 format */
                        opll->user[10] = ((data & 0x07) << 1) & 0x0E;
                        break;
    
                    case 0x0E: /* Rhythm enable and key-on bits */
                        if((data & 0x20) && (opll->rhythm == 0))
                        {
                            opll->rhythm = 1;
                            rhythm_mode_init(chip);
                        }
                        else
                        {
                            opll->rhythm = 0;
                        }
                        OPL_WRITE(chip, 0xBD, (data & 0x3F));
                        break;
                }
    
                /* If the user instrument registers were accessed, then
                   go through each channel and update the ones that were
                   currently using the user instrument. We can skip the
                   last three channels in rhythm mode since they can
                   only use percussion sounds anyways. */
                if(opll->latch <= 0x07)
                {
                    int x;
                    for(x = 0; x < ((opll->reg[0x0E] & 0x20) ? 6 : 9); x += 1)
                        if(opll->channel[x].instrument == 0x00)
                            load_instrument(chip, x, 0x00, opll->channel[x].volume);
                }
                break;
    
            case 0x10: /* Channel Frequency (LSB) */
            case 0x20: /* Channel Frequency (MSB) + key-on and sustain control */
                {
                    int block;
                    int frequency;
                    int ch = (opll->latch & 0x0F);

                    /* Ensure proper channel range */
                    if(ch > 0x08) break;
    
                    /* Get YM2413 channel frequency */
                    frequency = ((opll->reg[0x10 + ch] & 0xFF) | ((opll->reg[0x20 + ch] & 0x01) << 8));
    
                    /* Scale 9 bit frequency to 10 bits */
                    frequency = (frequency << 1) & 0x1FFF;
    
                    /* Get YM2413 block */
                    block = (opll->reg[0x20 + ch] >> 1) & 7;
    
                    /* Add in block */
                    frequency |= (block << 10);
    
                    /* Add key-on flag */
                    if(opll->reg[0x20 + ch] & 0x10) frequency |= 0x2000;
    
                    /* Save current frequency/block/key-on setting */
                    opll->channel[ch].frequency = (frequency & 0x3FFF);
    
                    /* TODO: Handle sustain flag (bit 5) before key-on */
    
                    /* Write changes to YM3812 */
                    OPL_WRITE(chip, 0xA0 + ch, (opll->channel[ch].frequency >> 0) & 0xFF);
                    OPL_WRITE(chip, 0xB0 + ch, (opll->channel[ch].frequency >> 8) & 0xFF);
                }
                break;
    
            case 0x30: /* Channel Volume Level and Instrument Select */

                /* Ensure proper channel range */
                if(opll->latch > 0x38) break;
    
                /* If we're accessing registers 36, 37, or 38, and we're
                   in rhythm mode, then update the individual volume
                   settings. */
                if((opll->latch >= 0x36) && (opll->reg[0x0E] & 0x20))
                {
                    switch(opll->latch & 0x0F)
                    {
                        case 0x06: /* Bass drum */
                          OPL_WRITE(chip, 0x53, ((data >> 0) & 0x0F) << 2);
                          break;
        
                        case 0x07: /* High hat and snare drum */
                          OPL_WRITE(chip, 0x51, ((data >> 4) & 0x0F) << 2);
                          OPL_WRITE(chip, 0x54, ((data >> 0) & 0x0F) << 2);
                          break;
        
                        case 0x08: /* Tom-top and top cymbal */
                          OPL_WRITE(chip, 0x52, ((data >> 4) & 0x0F) << 2);
                          OPL_WRITE(chip, 0x55, ((data >> 0) & 0x0F) << 2);
                          break;
                    }
                }
                else /* Set the new instrument and volume for this channel */
                {
                    int ch = (opll->latch & 0x0F);
                    int inst = (data >> 4) & 0x0F;
                    int vol = (data & 0x0F) << 2;
    
                    load_instrument(chip, ch, inst, vol);
                }
                break;
        }
    }
    else /* Register latch */
    {
        opll->latch = (data & 0x3F);
    }
}


void rhythm_mode_init(int chip)
{
    /* Point to current YM2413 context */
    t_ym2413 *opll = &ym2413[chip];

    /* Load instrument settings for channel seven. (Bass drum) */
    OPL_WRITE(chip, 0x30, 0x13);
    OPL_WRITE(chip, 0x33, 0x11);
    OPL_WRITE(chip, 0x50, 0x25);
    OPL_WRITE(chip, 0x53, ((opll->reg[0x36] >> 0) & 0x0F) << 2);
    OPL_WRITE(chip, 0x70, 0xD7);
    OPL_WRITE(chip, 0x73, 0xB7);
    OPL_WRITE(chip, 0x90, 0xF4);
    OPL_WRITE(chip, 0x93, 0xF4);
    OPL_WRITE(chip, 0xF0, 0x00);
    OPL_WRITE(chip, 0xF3, 0x00);
    OPL_WRITE(chip, 0xC6, 0x00);
    /* Use old frequency, but strip key-on bit */
    OPL_WRITE(chip, 0xA6, (opll->channel[6].frequency >> 0) & 0xFF);
    OPL_WRITE(chip, 0xB6, (opll->channel[6].frequency >> 8) & 0x1F);

    /* Load instrument settings for channel eight. (High hat and snare drum) */
    OPL_WRITE(chip, 0x31, 0x13);
    OPL_WRITE(chip, 0x34, 0x11);
    OPL_WRITE(chip, 0x51, ((opll->reg[0x37] >> 4) & 0x0f) << 2);
    OPL_WRITE(chip, 0x54, ((opll->reg[0x37] >> 0) & 0x0f) << 2);
    OPL_WRITE(chip, 0x71, 0xD7);
    OPL_WRITE(chip, 0x74, 0xB7);
    OPL_WRITE(chip, 0x91, 0xF4);
    OPL_WRITE(chip, 0x94, 0xF4);
    OPL_WRITE(chip, 0xF1, 0x00);
    OPL_WRITE(chip, 0xF4, 0x00);
    OPL_WRITE(chip, 0xC7, 0x00);
    /* Use old frequency, but strip key-on bit */
    OPL_WRITE(chip, 0xA7, (opll->channel[7].frequency >> 0) & 0xFF);
    OPL_WRITE(chip, 0xB7, (opll->channel[7].frequency >> 8) & 0x1F);

    /* Load instrument settings for channel nine. (Tom-tom and top cymbal) */
    OPL_WRITE(chip, 0x32, 0x13);
    OPL_WRITE(chip, 0x35, 0x11);
    OPL_WRITE(chip, 0x52, ((opll->reg[0x38] >> 4) & 0x0F) << 2);
    OPL_WRITE(chip, 0x55, ((opll->reg[0x38] >> 0) & 0x0F) << 2);
    OPL_WRITE(chip, 0x72, 0xD7);
    OPL_WRITE(chip, 0x75, 0xB7);
    OPL_WRITE(chip, 0x92, 0xF4);
    OPL_WRITE(chip, 0x95, 0xF4);
    OPL_WRITE(chip, 0xF2, 0x00);
    OPL_WRITE(chip, 0xF5, 0x00);
    OPL_WRITE(chip, 0xC8, 0x00);
    /* Use old frequency, but strip key-on bit */
    OPL_WRITE(chip, 0xA8, (opll->channel[8].frequency >> 0) & 0xFF);
    OPL_WRITE(chip, 0xB8, (opll->channel[8].frequency >> 8) & 0x1F);
}


/* channel (0-9), instrument (0-F), volume (0-3F, YM3812 format) */
void load_instrument(int chip, int ch, int inst, int vol)
{
    /* Point to current YM2413 context */
    t_ym2413 *opll = &ym2413[chip];

    /* Point to fixed instrument or user table */
    unsigned char *param = (inst == 0) ? &opll->user[0] : &table[inst][0];

    /* Maps channels to operator registers */
    unsigned char ch2op[] = {0, 1, 2, 8, 9, 10, 16, 17, 18};

    /* Make operator offset from requested channel */
    int op = ch2op[ch];

    /* Store volume level */
    opll->channel[ch].volume = (vol & 0x3F);

    /* Store instrument number */
    opll->channel[ch].instrument = (inst & 0x0F);

    /* Update instrument settings, except frequency registers */
    OPL_WRITE(chip, 0x20 + op,  param[0]);
    OPL_WRITE(chip, 0x23 + op,  param[1]);
    OPL_WRITE(chip, 0x40 + op,  param[2]);
    OPL_WRITE(chip, 0x43 + op, (param[3] & 0xC0) | opll->channel[ch].volume);
    OPL_WRITE(chip, 0x60 + op,  param[4]);
    OPL_WRITE(chip, 0x63 + op,  param[5]);
    OPL_WRITE(chip, 0x80 + op,  param[6]);
    OPL_WRITE(chip, 0x83 + op,  param[7]);
    OPL_WRITE(chip, 0xE0 + op,  param[8]);
    OPL_WRITE(chip, 0xE3 + op,  param[9]);
    OPL_WRITE(chip, 0xC0 + ch,  param[10]);
}
