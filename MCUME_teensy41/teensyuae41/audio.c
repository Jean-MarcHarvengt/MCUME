 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * OS specific functions
  * 
  * Copyright 1995, 1996, 1997 Bernd Schmidt
  * Copyright 1996 Marcus Sundberg
  * Copyright 1996 Manfred Thole
  */

#include "shared.h"

#include "memory.h"
#include "custom.h"
#include "gensound.h"
#include "sounddep/sound.h"
#include "events.h"
#include "audio.h"

int sound_available = 0;

struct audio_channel_data audio_channel[4];
#ifdef HAS_TABLE
static short sound_table[256][64];
#endif
unsigned long int sample_evtime;

void init_sound_table16(void)
{
#ifdef HAS_TABLE
    int i,j;
    
    for (i = 0; i < 256; i++)
	for (j = 0; j < 64; j++)
	    sound_table[i][j] = j * (uae_s8)i;
#endif
}


void AUDxDAT(int nr, uae_u16 v) 
{
#ifndef DONT_WANT_SOUND
    struct audio_channel_data *cdp = audio_channel + nr;
    cdp->dat = v;
    if (cdp->state == 0 && !(INTREQR() & (0x80 << nr))) {
	cdp->state = 2;
	INTREQ(0x8000 | (0x80 << nr));
	/* data_written = 2 ???? */
	eventtab[ev_aud0 + nr].evtime = cycles + cdp->per;
	eventtab[ev_aud0 + nr].oldcycles = cycles;
	eventtab[ev_aud0 + nr].active = 1;
	events_schedule();
    }
#endif
}

void AUDxLCH(int nr, uae_u16 v) { audio_channel[nr].lc = (audio_channel[nr].lc & 0xffff) | ((uae_u32)v << 16); }
void AUDxLCL(int nr, uae_u16 v) { audio_channel[nr].lc = (audio_channel[nr].lc & ~0xffff) | (v & 0xFFFE); }
void AUDxPER(int nr, uae_u16 v)
{
    if (v <= 0) {
#if 0 /* v == 0 is rather common, and harmless, and the value isn't signed anyway */
  static int warned = 0;
  if (!warned)
      write_log ("Broken program accessing the sound hardware\n"), warned++;
#endif
  v = 65535;
    }
    if (v < maxhpos/2 && currprefs.produce_sound < 3)
  v = maxhpos/2;

    audio_channel[nr].per = v;
}

void AUDxVOL(int nr, uae_u16 v) { audio_channel[nr].vol = v & 64 ? 63 : v & 63; }
void AUDxLEN(int nr, uae_u16 v) { audio_channel[nr].len = v; }


/* Templates! I want templates! */
void sample16_handler(void)
{
#ifndef DONT_WANT_SOUND
    int nr, adk;
    uae_u32 data = SOUND16_BASE_VAL;

    eventtab[ev_sample].evtime = cycles + sample_evtime;
    eventtab[ev_sample].oldcycles = cycles;

    adk = adkcon;

#ifdef HAS_TABLE
    if (!(adk & 0x11))
		data += sound_table[audio_channel[0].current_sample][audio_channel[0].vol];
    if (!(adk & 0x22))
		data += sound_table[audio_channel[1].current_sample][audio_channel[1].vol];
    if (!(adk & 0x44))
		data += sound_table[audio_channel[2].current_sample][audio_channel[2].vol];
    if (!(adk & 0x88))
		data += sound_table[audio_channel[3].current_sample][audio_channel[3].vol];
#else	
    if (!(adk & 0x11))
		data += (uae_s8)audio_channel[0].current_sample*audio_channel[0].vol;
    if (!(adk & 0x22))
		data += (uae_s8)audio_channel[1].current_sample*audio_channel[1].vol;
    if (!(adk & 0x44))
		data += (uae_s8)audio_channel[2].current_sample*audio_channel[2].vol;
    if (!(adk & 0x88))
		data += (uae_s8)audio_channel[3].current_sample*audio_channel[3].vol;
#endif
    PUT_SOUND_WORD (data);
    check_sound_buffers ();
#endif
}



#ifdef HAS_ULAW
static uae_u8 int2ulaw(int ch)
{
    int mask;

    if (ch < 0) {
      ch = -ch;
      mask = 0x7f;
    }
    else {
      mask = 0xff;
    }

    if (ch < 32) {
	ch = 0xF0 | ( 15 - (ch/2) );
    } else if (ch < 96) {
        ch = 0xE0 | ( 15 - (ch-32)/4 );
    } else if (ch < 224) {
	ch = 0xD0 | ( 15 - (ch-96)/8 );
    } else if (ch < 480) {
	ch = 0xC0 | ( 15 - (ch-224)/16 );
    } else if (ch < 992 ) {
	ch = 0xB0 | ( 15 - (ch-480)/32 );
    } else if (ch < 2016) {
	ch = 0xA0 | ( 15 - (ch-992)/64 );
    } else if (ch < 4064) {
	ch = 0x90 | ( 15 - (ch-2016)/128 );
    } else if (ch < 8160) {
	ch = 0x80 | ( 15 - (ch-4064)/256 );
    } else {
	ch = 0x80;
    }
    return (uae_u8)(mask & ch);
}

void sample_ulaw_handler(void)
{
#ifndef DONT_WANT_SOUND
    int nr;
    uae_u32 data = 0;
    char *bp = (char *)sndbufpt;

    eventtab[ev_sample].evtime += cycles - eventtab[ev_sample].oldcycles;
    eventtab[ev_sample].oldcycles = cycles;

    for (nr = 0; nr < 4; nr++) {
#ifdef HAS_TABLE
	if (!(adkcon & (0x11 << nr)))
	    data += sound_table[audio_channel[nr].current_sample][audio_channel[nr].vol];
#endif
    }
    PUT_SOUND_BYTE (int2ulaw (data));
    check_sound_buffers ();
#endif
}
#endif

#ifndef DONT_WANT_SOUND
static void audio_handler(int nr) 
{
    struct audio_channel_data *cdp = audio_channel + nr;

    switch (cdp->state) {
    case 0:
	//fprintf(stderr, "Bug in sound code\n");
	break;

     case 1:
	/* We come here at the first hsync after DMA was turned on. */
	eventtab[ev_aud0 + nr].evtime += maxhpos;
	eventtab[ev_aud0 + nr].oldcycles += maxhpos;
	
	cdp->state = 5;
	INTREQ(0x8000 | (0x80 << nr));
	if (cdp->wlen != 1)
	    cdp->wlen--;
	cdp->nextdat = chipmem_bank.wget(cdp->pt);

	cdp->pt += 2;
	break;

     case 5:
	/* We come here at the second hsync after DMA was turned on. */
	if (currprefs.produce_sound == 0)
	    cdp->per = 65535;

	eventtab[ev_aud0 + nr].evtime = cycles + cdp->per;
	eventtab[ev_aud0 + nr].oldcycles = cycles;
	cdp->dat = cdp->nextdat;
	cdp->current_sample = (uae_u8)(cdp->dat >> 8);

	cdp->state = 2;
	{
	    int audav = adkcon & (1 << nr);
	    int audap = adkcon & (16 << nr);
	    int napnav = (!audav && !audap) || audav;
	    if (napnav)
		cdp->data_written = 2;
	}
	break;
	
     case 2:
	/* We come here when a 2->3 transition occurs */
	if (currprefs.produce_sound == 0)
	    cdp->per = 65535;

	cdp->current_sample = (uae_u8)(cdp->dat & 0xFF);
	eventtab[ev_aud0 + nr].evtime = cycles + cdp->per;
	eventtab[ev_aud0 + nr].oldcycles = cycles;

	cdp->state = 3;

	/* Period attachment? */
	if (adkcon & (0x10 << nr)) {
	    if (cdp->intreq2 && cdp->dmaen)
		INTREQ(0x8000 | (0x80 << nr));
	    cdp->intreq2 = 0;

	    cdp->dat = cdp->nextdat;
	    if (cdp->dmaen)
		cdp->data_written = 2;
	    if (nr < 3) {
		if (cdp->dat == 0)
		    (cdp+1)->per = 65535;

		else if (cdp->dat < maxhpos/2 && currprefs.produce_sound < 3)
		    (cdp+1)->per = maxhpos/2;
		else
		    (cdp+1)->per = cdp->dat;
	    }
	}
	break;
	
     case 3:
	/* We come here when a 3->2 transition occurs */
	if (currprefs.produce_sound == 0)
	    cdp->per = 65535;

	eventtab[ev_aud0 + nr].evtime = cycles + cdp->per;
	eventtab[ev_aud0 + nr].oldcycles = cycles;
	
	if ((INTREQR() & (0x80 << nr)) && !cdp->dmaen) {
	    cdp->state = 0;
	    cdp->current_sample = 0;
	    eventtab[ev_aud0 + nr].active = 0;
	    break;
	} else {
	    int audav = adkcon & (1 << nr);
	    int audap = adkcon & (16 << nr);
	    int napnav = (!audav && !audap) || audav;
	    cdp->state = 2;
	    
	    if ((cdp->intreq2 && cdp->dmaen && napnav)
		|| (napnav && !cdp->dmaen))
		INTREQ(0x8000 | (0x80 << nr));
	    cdp->intreq2 = 0;
	    
	    cdp->dat = cdp->nextdat;
	    cdp->current_sample = (uae_u8)(cdp->dat >> 8);

	    if (cdp->dmaen && napnav)
		cdp->data_written = 2;
	    
	    /* Volume attachment? */
	    if (audav) {
		if (nr < 3)
		    (cdp+1)->vol = cdp->dat;
	    }
	}
	break;
	    
     default:
	cdp->state = 0;
	eventtab[ev_aud0 + nr].active = 0;
	break;
    }
}

void aud0_handler(void)
{
    audio_handler(0);
}
void aud1_handler(void)
{
    audio_handler(1);
}
void aud2_handler(void)
{
    audio_handler(2);
}
void aud3_handler(void)
{
    audio_handler(3);
}
#endif
