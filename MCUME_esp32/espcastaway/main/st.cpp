/*
* Castaway
*  (C) 1994 - 2002 Martin Doering, Joachim Hoenig
*
* IO.c - ST hardware emulation
*
* This file is distributed under the GPL, version 2 or at your
* option any later version.  See doc/license.txt for details.
*
* revision history
*  23.05.2002  JH  FAST1.0.1 code import: KR -> ANSI, restructuring
*  09.06.2002  JH  Renamed io.c to st.c again (io.h conflicts with system headers)
*  12.06.2002  JH  Correct bus error/address error exception stack frame
*  14.06.2002  JH  Implemented STOP, shutdown CPU after multiple bus errors.
*                  Removed inst parameter from CPU opcode functions.
*  20.08.2002  JH  Fixed sign bug in DoIORW() and DoIORL()
*  10.09.2002  JH  Bugfix: MOVE.L 0xfffa00,d0 and the like should not raise bus error
*  16.09.2002  JH  Bugfix: Word access on unmapped I/O address stacked
*                  two bus error stack frames. Fault address corrected.
*                  Merged some code from JH_TOS206_patches branch.
*  02.10.2002  JH  Eliminated a lot of silly bugs introduced recently. Shame on me.
No more CPU bus errors from blitter.c::bitblt().
*  10.10.2002  JH  Compatibility improvements.
*/
static char     sccsid[] = "$Id: st.c,v 1.14 2002/10/10 19:41:27 jhoenig Exp $";
#include <stdio.h>
#include "config.h"
#include "st.h"
#include "mem.h"
#include "m68k_intrf.h"
#include "sound.h"

#include "dcastaway.h"

#ifdef DEBUG
#include <assert.h>
#endif


#define VALUE_OPEN  0xff
/*
* startup display mode
*/
int             display_mode = COL4;

/*
* I/O Registers
*/
uint8           memconf;

//Video shifter
uint32			vid_adr;
uint8           vid_baseh, vid_basem;
uint32			vid_mem=0x10000;
uint8           vid_syncmode=2, vid_shiftmode;
int16           vid_col[16];
int             vid_flag;

uint16          dma_car, dma_scr, dma_sr, dma_mode;
uint8           dma_adrh, dma_adrm, dma_adrl;
uint8           mfp_gpip, mfp_aer, mfp_ddr, mfp_iera, mfp_ierb, mfp_ipra,
mfp_iprb, mfp_isra, mfp_isrb, mfp_imra, mfp_imrb, mfp_ivr,
mfp_tacr, mfp_tbcr, mfp_tcdcr, mfp_scr, mfp_ucr, mfp_rsr, mfp_tsr, mfp_udr;


//Mfp delay timer variables
int32 mfp_reg[12];
#define	mfp_tadr mfp_reg[0]
#define	mfp_tbdr mfp_reg[1]
#define	mfp_tcdr mfp_reg[2]
#define	mfp_tddr mfp_reg[3]
#define	mfp_acount mfp_reg[4]
#define	mfp_bcount mfp_reg[5]
#define	mfp_ccount mfp_reg[6]
#define	mfp_dcount mfp_reg[7]
#define	mfp_ascale mfp_reg[8]
#define	mfp_bscale mfp_reg[9]
#define	mfp_cscale mfp_reg[10]
#define	mfp_dscale mfp_reg[11]

uint8           acia1_cr, acia1_sr, acia1_dr, acia2_cr, acia2_sr, acia2_dr;

uint16          blt_halftone[16];
int16           blt_src_x_inc, blt_src_y_inc;
uint32          blt_src_addr;
int16           blt_end_1, blt_end_2, blt_end_3;
int16           blt_dst_x_inc, blt_dst_y_inc;
uint32          blt_dst_addr;
uint16          blt_x_cnt, blt_y_cnt;
int8            blt_hop, blt_op, blt_status, blt_skew;
int8            blt_ready;

uint32			psg[26];
unsigned char sample[10000];
#define phase0 psg[16]
#define phase1 psg[17]
#define phase2 psg[18]
#define phase3 psg[19]
#define psg_epos psg[20]
#define psgcontrol psg[21]
#define phase4 psg[22]
#define nrand psg[23]
#define sampos psg[24]
#define lastpsg psg[25]

static int samvol[16]={0,0,0,1,1,1,2,3,5,7,11,17,25,38,57,85};
static int samvol2[4096];

int32 mfpcycletab[16] = {0,80402,32161,20100,6432,5025,3216,1608,1,0,0,0,0,0,0,0};

void IOInit(void)
{
	int n,a,b,c;
	//Create sample lookup table (4096 entries)
	for (a=0; a<16; a++) {
		for (b=0; b<16; b++) {
			for (c=0; c<16; c++) {
				samvol2[(a<<8)+(b<<4)+c]=samvol[a]+samvol[b]+samvol[c];
			}
		}
	}
	//Reset mfp variables
	mfp_tadr=256<<20; mfp_tbdr=256<<20; mfp_tcdr=256<<20; mfp_tddr=256<<20;
	mfp_tacr=0; mfp_tbcr=0; mfp_tcdcr=0;
	mfp_acount=256<<20; mfp_bcount=256<<20; mfp_ccount=256<<20; mfp_dcount=256<<20;
	mfp_ascale=0; mfp_bscale=0; mfp_cscale=0; mfp_dscale=0;
	for (n=0; n<24; n++) psg[n]=0;
	nrand=1;
	//Other stuff
    vid_baseh = 0;
    vid_basem = 0;
    vid_shiftmode = display_mode;
    dma_sr = 1;                 /* DMA status ready */
    if (display_mode == MONO) {
        mfp_gpip = 0x39;        /* no lpr, no blt, no interrupt, monochrome */
    } else {
        mfp_gpip = 0xb9;        /* no lpr, no blt, no interrupt, color */
    }
#ifdef sun
    act.sa_handler = Sigbus;
    (void) sigaction (SIGBUS, &act, &oldsigbus);
#endif
#ifdef USE_MMAP
    act.sa_handler = Sigsegv;
    (void) sigaction (SIGSEGV, &act, &oldsigsegv);
#endif
}

static void update_psg(uint8 value) {
#ifndef NO_SOUND
	Sound_Update();
#endif	
	//Update psg register
	psg[psgcontrol]=value;
	
	switch(psgcontrol)
	{
		case 13:
#ifndef NO_SOUND
			bEnvelopeFreqFlag = 1;
			bWriteEnvelopeFreq = 1;
#endif
			break;
		case 12:
			psg_epos=0;
			break;
		case 8:
#ifndef NO_SOUND
			bWriteChannelAAmp= 1;
#endif
			break;
		case 9:
#ifndef NO_SOUND
			bWriteChannelBAmp= 1;
#endif
			break;
		case 10:
#ifndef NO_SOUND
			bWriteChannelCAmp= 1;
#endif
			break;
	}
}

void DoIOWB(uint32 address, uint8 value)
{
	address&=0x7fff;
	
	//Video and dma emu
	if (address<0x800) {
		switch (address) {
		case MEM_CONF&0x7fff:
			memconf = value;
			break;
		case VID_BASEH&0x7fff:
			vid_baseh = value;
			vid_mem = (vid_baseh<<16)+(vid_basem<<8);
			printf("vmem %lu\n",vid_mem);
			break;
		case VID_BASEM&0x7fff:
			vid_basem = value;
			vid_mem = (vid_baseh<<16)+(vid_basem<<8);
			printf("vmem %lu\n",vid_mem);
			break;
		case VID_SYNCMODE&0x7fff:
			vid_syncmode = value;
			maybe_border++;
			break;
		case VID_SHIFTMODE&0x7fff:
			vid_shiftmode = value;
			vid_flag=1;
			break;
		case DMA_ADRH&0x7fff:
			dma_adrh = value;
			break;
		case DMA_ADRM&0x7fff:
			dma_adrm = value;
			break;
		case DMA_ADRL&0x7fff:
			dma_adrl = value;
			break;
		}
		return;
	}
	
	//Sound emu
	if (address<0x900) {
#if defined(USE_FAME_CORE) && defined(USE_MOVEM_FAME_PATCH)
		static unsigned back_cycles=0;
		static unsigned back_value=0;
		static unsigned back_ctrl=0;
		static unsigned new_value=0;
#endif
		if (address<0x804)
			waitstate+=1;
		address&=3;
		if (address==0) {
			psgcontrol=value; //&15;
#if defined(USE_FAME_CORE) && defined(USE_MOVEM_FAME_PATCH)
			if ((M68KCONTEXT.cycles_counter+IO_CYCLE)==back_cycles) {
				psg[back_ctrl]=back_value;
				update_psg(new_value);
			}
#endif
		}else if (address==2 && psgcontrol<16) {
#if defined(USE_FAME_CORE) && defined(USE_MOVEM_FAME_PATCH)
			back_ctrl=psgcontrol;
			back_value=psg[psgcontrol];
			new_value=value;
#endif
			update_psg(value);
		}
#if defined(USE_FAME_CORE) && defined(USE_MOVEM_FAME_PATCH)
		back_cycles=IO_CYCLE+M68KCONTEXT.cycles_counter;
#endif
		return;
	}
	
	//Bus error?
	if (address<0xb00) {
		ExceptionGroup0(BUSERR, address|0xff8000, 0);
		return;
	}
	
	//MFP emu
	if (address<0x7c00) {
		waitstate+=4;
		switch(address) {
		case MFP_AER&0x7fff:
			mfp_aer = value;
			break;
		case MFP_DDR&0x7fff:
			mfp_ddr = value;
			break;
		case MFP_IERA&0x7fff:
			mfp_iera = value;
			mfp_ipra &= mfp_iera;
			break;
		case MFP_IERB&0x7fff:
			mfp_ierb = value;
			mfp_iprb &= mfp_ierb;
			break;
		case MFP_IPRA&0x7fff:
			mfp_ipra &= value;
			break;
		case MFP_IPRB&0x7fff:
			mfp_iprb &= value;
			break;
		case MFP_ISRA&0x7fff:
			mfp_isra &= value;
#ifndef USE_FAME_CORE
			recalc_int = 1;
#endif
			break;
		case MFP_ISRB&0x7fff:
			mfp_isrb &= value;
#ifndef USE_FAME_CORE
			recalc_int = 1;
#endif
			break;
		case MFP_IMRA&0x7fff:
			mfp_imra = value;
#ifndef USE_FAME_CORE
			recalc_int = 1;
#endif
			break;
		case MFP_IMRB&0x7fff:
			mfp_imrb = value;
#ifndef USE_FAME_CORE
			recalc_int = 1;
#endif
			break;
		case MFP_IVR&0x7fff:
			mfp_ivr = value;
			break;
		case MFP_TACR&0x7fff:
			mfp_tacr = value&15;
#if defined(USE_FAME_CORE) && defined(USE_SHORT_SLICE)
			if (mfp_ascale)
				m68k_stop_emulating();
#endif
			mfp_ascale = mfpcycletab[mfp_tacr];
			break;
		case MFP_TBCR&0x7fff:
			mfp_tbcr = value&15;
#if defined(USE_FAME_CORE) && defined(USE_SHORT_SLICE)
			if (mfp_bscale)
				m68k_stop_emulating();
#endif
			mfp_bscale = mfpcycletab[mfp_tbcr];
			break;
		case MFP_TCDCR&0x7fff:
			mfp_tcdcr = value&0x77;
#if defined(USE_FAME_CORE) && defined(USE_SHORT_SLICE)
			if (mfp_cscale || mfp_dscale)
				m68k_stop_emulating();
#endif
			mfp_cscale = mfpcycletab[mfp_tcdcr>>4];
			mfp_dscale = mfpcycletab[mfp_tcdcr&7];
			break;
		case MFP_TADR&0x7fff:
			if (value==0) mfp_tadr=256<<20; else mfp_tadr=value<<20;
			if (mfp_ascale==0) mfp_acount=mfp_tadr;
			break;
		case MFP_TBDR&0x7fff:
			if (value==0) mfp_tbdr=256<<20; else mfp_tbdr=value<<20;
			if (mfp_bscale==0) mfp_bcount=mfp_tbdr;
			break;
		case MFP_TCDR&0x7fff:
			if (value==0) mfp_tcdr=256<<20; else mfp_tcdr=value<<20;
			if (mfp_cscale==0) mfp_ccount=mfp_tcdr;
			break;
		case MFP_TDDR&0x7fff:
			if (value==0) mfp_tddr=256<<20; else mfp_tddr=value<<20;
			if (mfp_dscale==0) mfp_dcount=mfp_tddr;
			break;
		case MFP_SCR&0x7fff:
			mfp_scr = value;
			break;
		case MFP_UCR&0x7fff:
			mfp_ucr = value;
			break;
		case MFP_RSR&0x7fff:
			mfp_rsr = value;
			break;
		case MFP_TSR&0x7fff:
			mfp_tsr = value;
			break;
		case MFP_UDR&0x7fff:
			mfp_udr = value;
			break;
		}
		return;
	}

	switch(address) {
    case ACIA1_SR&0x7fff:
	waitstate+=8;
        acia1_cr = value;
        break;
    case ACIA1_DR&0x7fff:
	waitstate+=8;
        IkbdRecv (value);
        break;
    case ACIA2_SR&0x7fff:
	waitstate+=8;
        acia2_cr = value;
        break;
    case ACIA2_DR&0x7fff:
	waitstate+=8;
        break;
	}
	
}

void DoIOWW(uint32 address, uint16 value)
{
    if (address >= VID_COL0 && address <= VID_COL15) {
        vid_col[(address & 0x1f) >> 1] = value&0x777;
        vid_flag = 1;
		return;
	}
	else
        switch (address) {
        case DMA_MODE:
            dma_mode = value;
            break;
        case DMA_CAR:
	    waitstate+=4;
            if (dma_mode & 0x10) dma_scr = value;
			else if (dma_mode & 0x8) dma_car = value;
			else {
				switch (dma_mode & 0x6) {
				case 0:
					fdc_command = value;
					FDCCommand ();
					break;
				case 2:
					fdc_track = value;
					break;
				case 4:
					fdc_sector = value;
					break;
				case 6:
					fdc_data = value;
					break;
				}
            }
            break;
        default:
            DoIOWB(address, value>>8);
	    DoIOWB(address+1, value);
            break;
	}
}

void DoIOWL(uint32 address, uint32 value)
{
    DoIOWW(address, value>>16);
	DoIOWW(address+2, value);
}

static __inline__ void calculate_vid_adr(void)
{
	unsigned yet=(vid_cycle[cyclenext-IO_CYCLE]-vid_adr_cycleyet)&(~3);
	vid_adr+=yet;
	vid_adr_cycleyet+=yet;
}

uint8 DoIORB(uint32 address)
{
	address&=0x7fff;
	
    //Video and dma emu
	if (address<0x800) {
		switch (address) {
		case MEM_CONF&0x7fff:
			return memconf;
		case VID_BASEH&0x7fff:
			return vid_baseh;
		case VID_BASEM&0x7fff:
			return vid_basem;
		case VID_ADRH&0x7fff:
			calculate_vid_adr();
			return (unsigned char)(vid_adr>>16);
		case VID_ADRM&0x7fff:
			calculate_vid_adr();
			return (unsigned char)(vid_adr>>8);
		case VID_ADRL&0x7fff:
			calculate_vid_adr();
			return (unsigned char)(vid_adr);
		case VID_SYNCMODE&0x7fff:
			return vid_syncmode;
		case VID_LINEWIDTH&0x7fff:
			return 0xff;
		case VID_SHIFTMODE&0x7fff:
			return vid_shiftmode;
		case DMA_ADRH&0x7fff:
			return dma_adrh;
		case DMA_ADRM&0x7fff:
			return dma_adrm;
		case DMA_ADRL&0x7fff:
			return dma_adrl;
		}
		return VALUE_OPEN;
	}
	
	//Sound emu
	if (address<0x900) {
		address&=3;
		if (!address)
		{
			waitstate+=4;
			if (psgcontrol>=16) return 0xff;
			return psg[psgcontrol];
		}
		else if (address<4)
			waitstate++;
		return VALUE_OPEN;
	}
	
	//Bus error?
	if (address<0xb00) {
		ExceptionGroup0(BUSERR, address|0xff8000, 0);
		return VALUE_OPEN;
	}
	
	//MFP emu
	if (address<0x7c00) {
		waitstate+=4;
		switch(address) {
		case MFP_GPIP&0x7fff:
			return mfp_gpip;
		case MFP_AER&0x7fff:
			return mfp_aer;
		case MFP_DDR&0x7fff:
			return mfp_ddr;
		case MFP_IERA&0x7fff:
			return mfp_iera;
		case MFP_IERB&0x7fff:
			return mfp_ierb;
		case MFP_IPRA&0x7fff:
			return mfp_ipra;
		case MFP_IPRB&0x7fff:
			return mfp_iprb;
		case MFP_ISRA&0x7fff:
			return mfp_isra;
		case MFP_ISRB&0x7fff:
			return mfp_isrb;
		case MFP_IMRA&0x7fff:
			return mfp_imra;
		case MFP_IMRB&0x7fff:
			return mfp_imrb;
		case MFP_IVR&0x7fff:
			return mfp_ivr;
		case MFP_TACR&0x7fff:
			return mfp_tacr;
		case MFP_TBCR&0x7fff:
			return mfp_tbcr;
		case MFP_TCDCR&0x7fff:
			return mfp_tcdcr;
		case MFP_TADR&0x7fff:
			return (mfp_acount+0xfffff)>>20;
		case MFP_TBDR&0x7fff:
			return (mfp_bcount+0xfffff)>>20;
		case MFP_TCDR&0x7fff:
			return (mfp_ccount+0xfffff)>>20;
		case MFP_TDDR&0x7fff:
			return (mfp_dcount+0xfffff)>>20;
		case MFP_SCR&0x7fff:
			return mfp_scr;
		case MFP_UCR&0x7fff:
			return mfp_ucr;
		case MFP_RSR&0x7fff:
			return mfp_rsr;
		case MFP_TSR&0x7fff:
			return mfp_tsr;
		case MFP_UDR&0x7fff:
			return mfp_udr;
		}
		return VALUE_OPEN;
	}
	
	//Acia emu
	switch(address) {
    case ACIA1_SR&0x7fff:
	waitstate+=8;
        return 2 | acia1_sr;
    case ACIA1_DR&0x7fff:
	waitstate+=8;
        if (!(acia1_cr & 0x20)) {acia1_sr&=0x7e; mfp_gpip|=0x10;}
        return acia1_dr;
    case ACIA2_SR&0x7fff:
		waitstate+=8;
		return 2;
    case ACIA2_DR&0x7fff:
		waitstate+=8;
		return 1;
	}
	return VALUE_OPEN;
	
}

uint16 DoIORW(uint32 address)
{
    if (address >= VID_COL0 && address <= VID_COL15) {
        return vid_col[(address & 0x1f) >> 1];
	}
    switch (address) {
	case DMA_SR:
		return dma_sr;
	case DMA_CAR:
		waitstate+=4;
		if (dma_mode & 0x10) return dma_scr;
		else if (dma_mode & 0x8) return dma_car;
		else {
			switch (dma_mode & 0x6) {
			case 0:
				if (!fdc_int) mfp_gpip |= 0x20;
				return fdc_status;
			case 2:
				return fdc_track;
			case 4:
				return fdc_sector;
			case 6:
				return fdc_data;
			}
            return 0;
		}
	default:
		return (((uint32)DoIORB(address))<<8)+DoIORB(address+1);
	}
}

uint32 DoIORL(uint32 address)
{
	return (((uint32)DoIORW(address))<<16)+DoIORW(address+2);
}

