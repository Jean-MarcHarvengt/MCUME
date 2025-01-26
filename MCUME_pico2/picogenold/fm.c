
#include "shared.h"
#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

#define BUILD_OPN (BUILD_YM2612)
#define BUILD_STEREO (BUILD_YM2612)
#define BUILD_LFO (BUILD_YM2612)

#define SIN_ENT 2048
#define ENV_BITS 16
#define EG_ENT   4096
#define EG_STEP (96.0/EG_ENT) /* OPL == 0.1875 dB */

#if FM_LFO_SUPPORT
/* LFO table entries */
#define LFO_ENT 512
#define LFO_SHIFT (32-9)
#define LFO_RATE 0x10000
#endif

/* -------------------- preliminary define section --------------------- */
/* attack/decay rate time rate */
#define OPM_ARRATE     399128
#define OPM_DRRATE    5514396
/* It is not checked , because I haven't YM2203 rate */
#define OPN_ARRATE  OPM_ARRATE
#define OPN_DRRATE  OPM_DRRATE

/* PG output cut off level : 78dB(14bit)? */
#define PG_CUT_OFF ((int)(78.0/EG_STEP))
/* EG output cut off level : 68dB? */
#define EG_CUT_OFF ((int)(68.0/EG_STEP))

#define FREQ_BITS 24		/* frequency turn          */

/* PG counter is 21bits @oct.7 */
#define FREQ_RATE   (1<<(FREQ_BITS-21))
#define TL_BITS    (FREQ_BITS+2)
/* OPbit = 14(13+sign) : TL_BITS+1(sign) / output = 16bit */
#define TL_SHIFT (TL_BITS+1-(14-16))

/* output final shift */
#define FM_OUTSB  (TL_SHIFT-FM_OUTPUT_BIT)
#define FM_MAXOUT ((1<<(TL_SHIFT-1))-1)
#define FM_MINOUT (-(1<<(TL_SHIFT-1)))

/* -------------------- local defines , macros --------------------- */

/* envelope counter position */
#define EG_AST   0					/* start of Attack phase */
#define EG_AED   (EG_ENT<<ENV_BITS)	/* end   of Attack phase */
#define EG_DST   EG_AED				/* start of Decay/Sustain/Release phase */
#define EG_DED   (EG_DST+(EG_ENT<<ENV_BITS)-1)	/* end   of Decay/Sustain/Release phase */
#define EG_OFF   EG_DED				/* off */
#if FM_SEG_SUPPORT
#define EG_UST   ((2*EG_ENT)<<ENV_BITS)  /* start of SEG UPSISE */
#define EG_UED   ((3*EG_ENT)<<ENV_BITS)  /* end of SEG UPSISE */
#endif

/* register number to channel number , slot offset */
#define OPN_CHAN(N) (N&3)
#define OPN_SLOT(N) ((N>>2)&3)
#define OPM_CHAN(N) (N&7)
#define OPM_SLOT(N) ((N>>3)&3)
/* slot number */
#define SLOT1 0
#define SLOT2 2
#define SLOT3 1
#define SLOT4 3

/* bit0 = Right enable , bit1 = Left enable */
#define OUTD_RIGHT  1
#define OUTD_LEFT   2
#define OUTD_CENTER 3

/* FM timer model */
#define FM_TIMER_SINGLE (0)
#define FM_TIMER_INTERVAL (1)

/* ---------- OPN / OPM one channel  ---------- */
typedef struct fm_slot {
	INT32 *DT;			/* detune          :DT_TABLE[DT]       */
	int DT2;			/* multiple,Detune2:(DT2<<4)|ML for OPM*/
	int TL;				/* total level     :TL << 8            */
	UINT8 KSR;			/* key scale rate  :3-KSR              */
	const INT32 *AR;	/* attack rate     :&AR_TABLE[AR<<1]   */
	const INT32 *DR;	/* decay rate      :&DR_TABLE[DR<<1]   */
	const INT32 *SR;	/* sustin rate     :&DR_TABLE[SR<<1]   */
	int   SL;			/* sustin level    :SL_TABLE[SL]       */
	const INT32 *RR;	/* release rate    :&DR_TABLE[RR<<2+2] */
	UINT8 SEG;			/* SSG EG type     :SSGEG              */
	UINT8 ksr;			/* key scale rate  :kcode>>(3-KSR)     */
	UINT32 mul;			/* multiple        :ML_TABLE[ML]       */
	/* Phase Generator */
	UINT32 Cnt;			/* frequency count :                   */
	UINT32 Incr;		/* frequency step  :                   */
	/* Envelope Generator */
	void (*eg_next)(struct fm_slot *SLOT);	/* pointer of phase handler */
	INT32 evc;			/* envelope counter                    */
	INT32 eve;			/* envelope counter end point          */
	INT32 evs;			/* envelope counter step               */
	INT32 evsa;			/* envelope step for Attack            */
	INT32 evsd;			/* envelope step for Decay             */
	INT32 evss;			/* envelope step for Sustain           */
	INT32 evsr;			/* envelope step for Release           */
	INT32 TLL;			/* adjusted TotalLevel                 */
	/* LFO */
	UINT8 amon;			/* AMS enable flag              */
	UINT32 ams;			/* AMS depth level of this SLOT */
}FM_SLOT;

typedef struct fm_chan {
	FM_SLOT	SLOT[4];
	UINT8 PAN;			/* PAN :NONE,LEFT,RIGHT or CENTER */
	UINT8 ALGO;			/* Algorythm                      */
	UINT8 FB;			/* shift count of self feed back  */
	INT32 op1_out[2];	/* op1 output for beedback        */
	/* Algorythm (connection) */
	INT32 *connect1;		/* pointer of SLOT1 output    */
	INT32 *connect2;		/* pointer of SLOT2 output    */
	INT32 *connect3;		/* pointer of SLOT3 output    */
	INT32 *connect4;		/* pointer of SLOT4 output    */
	/* LFO */
	INT32 pms;				/* PMS depth level of channel */
	UINT32 ams;				/* AMS depth level of channel */
	/* Phase Generator */
	UINT32 fc;			/* fnum,blk    :adjusted to sampling rate */
	UINT8 fn_h;			/* freq latch  :                   */
	UINT8 kcode;		/* key code    :                   */
} FM_CH;

/* OPN/OPM common state */
typedef struct fm_state {
	UINT8 index;		/* chip index (number of chip) */
	int clock;			/* master clock  (Hz)  */
	int rate;			/* sampling rate (Hz)  */
	double freqbase;	/* frequency base      */
	double TimerBase;	/* Timer base time     */
	UINT8 address;		/* address register    */
	UINT8 irq;			/* interrupt level     */
	UINT8 irqmask;		/* irq mask            */
	UINT8 status;		/* status flag         */
	UINT32 mode;		/* mode  CSM / 3SLOT   */
	int TA;				/* timer a             */
	int TAC;			/* timer a counter     */
	UINT8 TB;			/* timer b             */
	int TBC;			/* timer b counter     */
	/* speedup customize */
	/* local time tables */
	INT32 DT_TABLE[8][32];	/* DeTune tables       */
	INT32 AR_TABLE[94];		/* Atttack rate tables */
	INT32 DR_TABLE[94];		/* Decay rate tables   */
	/* Extention Timer and IRQ handler */
	FM_TIMERHANDLER	Timer_Handler;
	FM_IRQHANDLER	IRQ_Handler;
	/* timer model single / interval */
	UINT8 timermodel;
}FM_ST;

/* -------------------- tables --------------------- */

/* sustain lebel table (3db per step) */
/* 0 - 15: 0, 3, 6, 9,12,15,18,21,24,27,30,33,36,39,42,93 (dB)*/
#define SC(db) (int)((db*((3/EG_STEP)*(1<<ENV_BITS)))+EG_DST)
static const int SL_TABLE[16]={
 SC( 0),SC( 1),SC( 2),SC(3 ),SC(4 ),SC(5 ),SC(6 ),SC( 7),
 SC( 8),SC( 9),SC(10),SC(11),SC(12),SC(13),SC(14),SC(31)
};
#undef SC

/* size of TL_TABLE = sinwave(max cut_off) + cut_off(tl + ksr + envelope + ams) */
#define TL_MAX (PG_CUT_OFF+EG_CUT_OFF+1)

/* TotalLevel : 48 24 12  6  3 1.5 0.75 (dB) */
/* TL_TABLE[ 0      to TL_MAX          ] : plus  section */
/* TL_TABLE[ TL_MAX to TL_MAX+TL_MAX-1 ] : minus section */
static INT32 *TL_TABLE;

/* pointers to TL_TABLE with sinwave output offset */
static INT32 *SIN_TABLE[SIN_ENT];

/* envelope output curve table */
#if FM_SEG_SUPPORT
/* attack + decay + SSG upside + OFF */
static INT32 ENV_CURVE[3*EG_ENT+1];
#else
/* attack + decay + OFF */
static INT32 ENV_CURVE[2*EG_ENT+1];
#endif
/* envelope counter conversion table when change Decay to Attack phase */
static int DRAR_TABLE[EG_ENT];

#define OPM_DTTABLE OPN_DTTABLE
static UINT8 OPN_DTTABLE[4 * 32]={
/* this table is YM2151 and YM2612 data */
/* FD=0 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* FD=1 */
  0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2,
  2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 8, 8, 8, 8,
/* FD=2 */
  1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5,
  5, 6, 6, 7, 8, 8, 9,10,11,12,13,14,16,16,16,16,
/* FD=3 */
  2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7,
  8 , 8, 9,10,11,12,13,14,16,17,19,20,22,22,22,22
};

/* multiple table */
#define ML(n) (int)(n*2)
static const int MUL_TABLE[4*16]= {
/* 1/2, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15 */
   ML(0.50),ML( 1.00),ML( 2.00),ML( 3.00),ML( 4.00),ML( 5.00),ML( 6.00),ML( 7.00),
   ML(8.00),ML( 9.00),ML(10.00),ML(11.00),ML(12.00),ML(13.00),ML(14.00),ML(15.00),
/* DT2=1 *SQL(2)   */
   ML(0.71),ML( 1.41),ML( 2.82),ML( 4.24),ML( 5.65),ML( 7.07),ML( 8.46),ML( 9.89),
   ML(11.30),ML(12.72),ML(14.10),ML(15.55),ML(16.96),ML(18.37),ML(19.78),ML(21.20),
/* DT2=2 *SQL(2.5) */
   ML( 0.78),ML( 1.57),ML( 3.14),ML( 4.71),ML( 6.28),ML( 7.85),ML( 9.42),ML(10.99),
   ML(12.56),ML(14.13),ML(15.70),ML(17.27),ML(18.84),ML(20.41),ML(21.98),ML(23.55),
/* DT2=3 *SQL(3)   */
   ML( 0.87),ML( 1.73),ML( 3.46),ML( 5.19),ML( 6.92),ML( 8.65),ML(10.38),ML(12.11),
   ML(13.84),ML(15.57),ML(17.30),ML(19.03),ML(20.76),ML(22.49),ML(24.22),ML(25.95)
};
#undef ML

#if FM_LFO_SUPPORT

#define PMS_RATE 0x400
/* LFO runtime work */
static UINT32 lfo_amd;
static INT32 lfo_pmd;
#endif

/* Dummy table of Attack / Decay rate ( use when rate == 0 ) */
static const INT32 RATE_0[32]=
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/* -------------------- state --------------------- */

/* some globals */
#define TYPE_SSG    0x01    /* SSG support          */
#define TYPE_OPN    0x02    /* OPN device           */
#define TYPE_LFOPAN 0x04    /* OPN type LFO and PAN */
#define TYPE_6CH    0x08    /* FM 6CH / 3CH         */
#define TYPE_DAC    0x10    /* YM2612's DAC device  */
#define TYPE_ADPCM  0x20    /* two ADPCM unit       */

#define TYPE_YM2203 (TYPE_SSG)
#define TYPE_YM2608 (TYPE_SSG |TYPE_LFOPAN |TYPE_6CH |TYPE_ADPCM)
#define TYPE_YM2610 (TYPE_SSG |TYPE_LFOPAN |TYPE_6CH |TYPE_ADPCM)
#define TYPE_YM2612 (TYPE_6CH |TYPE_LFOPAN |TYPE_DAC)

/* current chip state */
static void *cur_chip = 0;		/* pointer of current chip struct */
static FM_ST  *State;			/* basic status */
static FM_CH  *cch[8];			/* pointer of FM channels */
#if (BUILD_LFO)
#if FM_LFO_SUPPORT
static UINT32 LFOCnt,LFOIncr;	/* LFO PhaseGenerator */
#endif
#endif

/* runtime work */
static INT32 out_ch[4];		/* channel output NONE,LEFT,RIGHT or CENTER */
static INT32 pg_in1,pg_in2,pg_in3,pg_in4;	/* PG input of SLOTs */

/* -------------------- log output  -------------------- */
/* log output level */
#define LOG_ERR  3      /* ERROR       */
#define LOG_WAR  2      /* WARNING     */
#define LOG_INF  1      /* INFORMATION */
#define LOG_LEVEL LOG_INF

/* ----- limitter ----- */
#define Limit(val, max,min) { \
	if ( val > max )      val = max; \
	else if ( val < min ) val = min; \
}

/* ----- buffering one of data(STEREO chip) ----- */
#if FM_STEREO_MIX
/* stereo mixing */
#define FM_BUFFERING_STEREO \
{														\
	/* get left & right output with clipping */			\
	out_ch[OUTD_LEFT]  += out_ch[OUTD_CENTER];				\
	Limit( out_ch[OUTD_LEFT] , FM_MAXOUT, FM_MINOUT );	\
	out_ch[OUTD_RIGHT] += out_ch[OUTD_CENTER];				\
	Limit( out_ch[OUTD_RIGHT], FM_MAXOUT, FM_MINOUT );	\
	/* buffering */										\
	*bufL++ = out_ch[OUTD_LEFT] >>FM_OUTSB;				\
	*bufL++ = out_ch[OUTD_RIGHT]>>FM_OUTSB;				\
}
#else
/* stereo separate */
#define FM_BUFFERING_STEREO \
{														\
	/* get left & right output with clipping */			\
	out_ch[OUTD_LEFT]  += out_ch[OUTD_CENTER];				\
	Limit( out_ch[OUTD_LEFT] , FM_MAXOUT, FM_MINOUT );	\
	out_ch[OUTD_RIGHT] += out_ch[OUTD_CENTER];				\
	Limit( out_ch[OUTD_RIGHT], FM_MAXOUT, FM_MINOUT );	\
	/* buffering */										\
	bufL[i] = out_ch[OUTD_LEFT] >>FM_OUTSB;				\
	bufR[i] = out_ch[OUTD_RIGHT]>>FM_OUTSB;				\
}
#endif

#if FM_INTERNAL_TIMER
/* ----- internal timer mode , update timer */
/* ---------- calcrate timer A ---------- */
#define INTERNAL_TIMER_A(ST,CSM_CH)					\
{													\
	if( ST->TAC &&  (ST->Timer_Handler==0) )		\
		if( (ST->TAC -= (int)(ST->freqbase*4096)) <= 0 )	\
		{											\
			TimerAOver( ST );						\
			/* CSM mode total level latch and auto key on */	\
			if( ST->mode & 0x80 )					\
				CSMKeyControll( CSM_CH );			\
		}											\
}
/* ---------- calcrate timer B ---------- */
#define INTERNAL_TIMER_B(ST,step)						\
{														\
	if( ST->TBC && (ST->Timer_Handler==0) )				\
		if( (ST->TBC -= (int)(ST->freqbase*4096*step)) <= 0 )	\
			TimerBOver( ST );							\
}
#else /* FM_INTERNAL_TIMER */
/* external timer mode */
#define INTERNAL_TIMER_A(ST,CSM_CH)
#define INTERNAL_TIMER_B(ST,step)
#endif /* FM_INTERNAL_TIMER */

/* --------------------- subroutines  --------------------- */
/* status set and IRQ handling */
static __inline__ void FM_STATUS_SET(FM_ST *ST,int flag)
{
	/* set status flag */
	ST->status |= flag;
	if ( !(ST->irq) && (ST->status & ST->irqmask) )
	{
		ST->irq = 1;
		/* callback user interrupt handler (IRQ is OFF to ON) */
		if(ST->IRQ_Handler) (ST->IRQ_Handler)(ST->index,1);
	}
}

/* status reset and IRQ handling */
static __inline__ void FM_STATUS_RESET(FM_ST *ST,int flag)
{
	/* reset status flag */
	ST->status &=~flag;
	if ( (ST->irq) && !(ST->status & ST->irqmask) )
	{
		ST->irq = 0;
		/* callback user interrupt handler (IRQ is ON to OFF) */
		if(ST->IRQ_Handler) (ST->IRQ_Handler)(ST->index,0);
	}
}

/* IRQ mask set */
static __inline__ void FM_IRQMASK_SET(FM_ST *ST,int flag)
{
	ST->irqmask = flag;
	/* IRQ handling check */
	FM_STATUS_SET(ST,0);
	FM_STATUS_RESET(ST,0);
}

/* ---------- event hander of Phase Generator ---------- */

/* Release end -> stop counter */
static void FM_EG_Release( FM_SLOT *SLOT )
{
	SLOT->evc = EG_OFF;
	SLOT->eve = EG_OFF+1;
	SLOT->evs = 0;
}

/* SUSTAIN end -> stop counter */
static void FM_EG_SR( FM_SLOT *SLOT )
{
	SLOT->evs = 0;
	SLOT->evc = EG_OFF;
	SLOT->eve = EG_OFF+1;
}

/* Decay end -> Sustain */
static void FM_EG_DR( FM_SLOT *SLOT )
{
	SLOT->eg_next = FM_EG_SR;
	SLOT->evc = SLOT->SL;
	SLOT->eve = EG_DED;
	SLOT->evs = SLOT->evss;
}

/* Attack end -> Decay */
static void FM_EG_AR( FM_SLOT *SLOT )
{
	/* next DR */
	SLOT->eg_next = FM_EG_DR;
	SLOT->evc = EG_DST;
	SLOT->eve = SLOT->SL;
	SLOT->evs = SLOT->evsd;
}

#if FM_SEG_SUPPORT
static void FM_EG_SSG_SR( FM_SLOT *SLOT );

/* SEG down side end  */
static void FM_EG_SSG_DR( FM_SLOT *SLOT )
{
	if( SLOT->SEG&2){
		/* reverce */
		SLOT->eg_next = FM_EG_SSG_SR;
		SLOT->evc = SLOT->SL + (EG_UST - EG_DST);
		SLOT->eve = EG_UED;
		SLOT->evs = SLOT->evss;
	}else{
		/* again */
		SLOT->evc = EG_DST;
	}
	/* hold */
	if( SLOT->SEG&1) SLOT->evs = 0;
}

/* SEG upside side end */
static void FM_EG_SSG_SR( FM_SLOT *SLOT )
{
	if( SLOT->SEG&2){
		/* reverce  */
		SLOT->eg_next = FM_EG_SSG_DR;
		SLOT->evc = EG_DST;
		SLOT->eve = EG_DED;
		SLOT->evs = SLOT->evsd;
	}else{
		/* again */
		SLOT->evc = SLOT->SL + (EG_UST - EG_DST);
	}
	/* hold check */
	if( SLOT->SEG&1) SLOT->evs = 0;
}

/* SEG Attack end */
static void FM_EG_SSG_AR( FM_SLOT *SLOT )
{
	if( SLOT->SEG&4){	/* start direction */
		/* next SSG-SR (upside start ) */
		SLOT->eg_next = FM_EG_SSG_SR;
		SLOT->evc = SLOT->SL + (EG_UST - EG_DST);
		SLOT->eve = EG_UED;
		SLOT->evs = SLOT->evss;
	}else{
		/* next SSG-DR (downside start ) */
		SLOT->eg_next = FM_EG_SSG_DR;
		SLOT->evc = EG_DST;
		SLOT->eve = EG_DED;
		SLOT->evs = SLOT->evsd;
	}
}
#endif /* FM_SEG_SUPPORT */

/* ----- key on of SLOT ----- */
#define FM_KEY_IS(SLOT) ((SLOT)->eg_next!=FM_EG_Release)

static __inline__ void FM_KEYON(FM_CH *CH , int s )
{
	FM_SLOT *SLOT = &CH->SLOT[s];
	if( !FM_KEY_IS(SLOT) )
	{
		/* restart Phage Generator */
		SLOT->Cnt = 0;
		/* phase -> Attack */
#if FM_SEG_SUPPORT
		if( SLOT->SEG&8 ) SLOT->eg_next = FM_EG_SSG_AR;
		else
#endif
		SLOT->eg_next = FM_EG_AR;
		SLOT->evs     = SLOT->evsa;
#if 0
		/* convert decay count to attack count */
		/* --- This caused the problem by credit sound of paper boy. --- */
		SLOT->evc = EG_AST + DRAR_TABLE[ENV_CURVE[SLOT->evc>>ENV_BITS]];/* + SLOT->evs;*/
#else
		/* reset attack counter */
		SLOT->evc = EG_AST;
#endif
		SLOT->eve = EG_AED;
	}
}
/* ----- key off of SLOT ----- */
static __inline__ void FM_KEYOFF(FM_CH *CH , int s )
{
	FM_SLOT *SLOT = &CH->SLOT[s];
	if( FM_KEY_IS(SLOT) )
	{
		/* if Attack phase then adjust envelope counter */
		if( SLOT->evc < EG_DST )
			SLOT->evc = (ENV_CURVE[SLOT->evc>>ENV_BITS]<<ENV_BITS) + EG_DST;
		/* phase -> Release */
		SLOT->eg_next = FM_EG_Release;
		SLOT->eve     = EG_DED;
		SLOT->evs     = SLOT->evsr;
	}
}

/* setup Algorythm and PAN connection */
static void setup_connection( FM_CH *CH )
{
	INT32 *carrier = &out_ch[CH->PAN]; /* NONE,LEFT,RIGHT or CENTER */

	switch( CH->ALGO ){
	case 0:
		/*  PG---S1---S2---S3---S4---OUT */
		CH->connect1 = &pg_in2;
		CH->connect2 = &pg_in3;
		CH->connect3 = &pg_in4;
		break;
	case 1:
		/*  PG---S1-+-S3---S4---OUT */
		/*  PG---S2-+               */
		CH->connect1 = &pg_in3;
		CH->connect2 = &pg_in3;
		CH->connect3 = &pg_in4;
		break;
	case 2:
		/* PG---S1------+-S4---OUT */
		/* PG---S2---S3-+          */
		CH->connect1 = &pg_in4;
		CH->connect2 = &pg_in3;
		CH->connect3 = &pg_in4;
		break;
	case 3:
		/* PG---S1---S2-+-S4---OUT */
		/* PG---S3------+          */
		CH->connect1 = &pg_in2;
		CH->connect2 = &pg_in4;
		CH->connect3 = &pg_in4;
		break;
	case 4:
		/* PG---S1---S2-+--OUT */
		/* PG---S3---S4-+      */
		CH->connect1 = &pg_in2;
		CH->connect2 = carrier;
		CH->connect3 = &pg_in4;
		break;
	case 5:
		/*         +-S2-+     */
		/* PG---S1-+-S3-+-OUT */
		/*         +-S4-+     */
		CH->connect1 = 0;	/* special case */
		CH->connect2 = carrier;
		CH->connect3 = carrier;
		break;
	case 6:
		/* PG---S1---S2-+     */
		/* PG--------S3-+-OUT */
		/* PG--------S4-+     */
		CH->connect1 = &pg_in2;
		CH->connect2 = carrier;
		CH->connect3 = carrier;
		break;
	case 7:
		/* PG---S1-+     */
		/* PG---S2-+-OUT */
		/* PG---S3-+     */
		/* PG---S4-+     */
		CH->connect1 = carrier;
		CH->connect2 = carrier;
		CH->connect3 = carrier;
	}
	CH->connect4 = carrier;
}

/* set detune & multiple */
static __inline__ void set_det_mul(FM_ST *ST,FM_CH *CH,FM_SLOT *SLOT,int v)
{
	SLOT->mul = MUL_TABLE[v&0x0f];
	SLOT->DT  = ST->DT_TABLE[(v>>4)&7];
	CH->SLOT[SLOT1].Incr=-1;
}

/* set total level */
static __inline__ void set_tl(FM_CH *CH,FM_SLOT *SLOT , int v,int csmflag)
{
	v &= 0x7f;
	v = (v<<7)|v; /* 7bit -> 14bit */
	SLOT->TL = (v*EG_ENT)>>14;
	/* if it is not a CSM channel , latch the total level */
	if( !csmflag )
		SLOT->TLL = SLOT->TL;
}

/* set attack rate & key scale  */
static __inline__ void set_ar_ksr(FM_CH *CH,FM_SLOT *SLOT,int v,INT32 *ar_table)
{
	SLOT->KSR  = 3-(v>>6);
	SLOT->AR   = (v&=0x1f) ? &ar_table[v<<1] : RATE_0;
	SLOT->evsa = SLOT->AR[SLOT->ksr];
	if( SLOT->eg_next == FM_EG_AR ) SLOT->evs = SLOT->evsa;
	CH->SLOT[SLOT1].Incr=-1;
}
/* set decay rate */
static __inline__ void set_dr(FM_SLOT *SLOT,int v,INT32 *dr_table)
{
	SLOT->DR = (v&=0x1f) ? &dr_table[v<<1] : RATE_0;
	SLOT->evsd = SLOT->DR[SLOT->ksr];
	if( SLOT->eg_next == FM_EG_DR ) SLOT->evs = SLOT->evsd;
}
/* set sustain rate */
static __inline__ void set_sr(FM_SLOT *SLOT,int v,INT32 *dr_table)
{
	SLOT->SR = (v&=0x1f) ? &dr_table[v<<1] : RATE_0;
	SLOT->evss = SLOT->SR[SLOT->ksr];
	if( SLOT->eg_next == FM_EG_SR ) SLOT->evs = SLOT->evss;
}
/* set release rate */
static __inline__ void set_sl_rr(FM_SLOT *SLOT,int v,INT32 *dr_table)
{
	SLOT->SL = SL_TABLE[(v>>4)];
	SLOT->RR = &dr_table[((v&0x0f)<<2)|2];
	SLOT->evsr = SLOT->RR[SLOT->ksr];
	if( SLOT->eg_next == FM_EG_Release ) SLOT->evs = SLOT->evsr;
}

/* operator output calcrator */
#define OP_OUT(PG,EG)   SIN_TABLE[(PG/(0x1000000/SIN_ENT))&(SIN_ENT-1)][EG]
#define OP_OUTN(PG,EG)  NOISE_TABLE[(PG/(0x1000000/SIN_ENT))&(SIN_ENT-1)][EG]

/* eg calcration */
#if FM_LFO_SUPPORT
#define FM_CALC_EG(OUT,SLOT)						\
{													\
	if( (SLOT.evc += SLOT.evs) >= SLOT.eve) 		\
		SLOT.eg_next(&(SLOT));						\
	OUT = SLOT.TLL+ENV_CURVE[SLOT.evc>>ENV_BITS];	\
	if(SLOT.ams)									\
		OUT += (SLOT.ams*lfo_amd/LFO_RATE);			\
}
#else
#define FM_CALC_EG(OUT,SLOT)						\
{													\
	if( (SLOT.evc += SLOT.evs) >= SLOT.eve) 		\
		SLOT.eg_next(&(SLOT));						\
	OUT = SLOT.TLL+ENV_CURVE[SLOT.evc>>ENV_BITS];	\
}
#endif

/* ---------- calcrate one of channel ---------- */
static __inline__ void FM_CALC_CH( FM_CH *CH )
{
	UINT32 eg_out1,eg_out2,eg_out3,eg_out4;  //envelope output

	/* Phase Generator */
#if FM_LFO_SUPPORT
	INT32 pms = lfo_pmd * CH->pms / LFO_RATE;
	if(pms)
	{
		pg_in1 = (CH->SLOT[SLOT1].Cnt += CH->SLOT[SLOT1].Incr + (INT32)(pms * CH->SLOT[SLOT1].Incr) / PMS_RATE);
		pg_in2 = (CH->SLOT[SLOT2].Cnt += CH->SLOT[SLOT2].Incr + (INT32)(pms * CH->SLOT[SLOT2].Incr) / PMS_RATE);
		pg_in3 = (CH->SLOT[SLOT3].Cnt += CH->SLOT[SLOT3].Incr + (INT32)(pms * CH->SLOT[SLOT3].Incr) / PMS_RATE);
		pg_in4 = (CH->SLOT[SLOT4].Cnt += CH->SLOT[SLOT4].Incr + (INT32)(pms * CH->SLOT[SLOT4].Incr) / PMS_RATE);
	}
	else
#endif
	{
		pg_in1 = (CH->SLOT[SLOT1].Cnt += CH->SLOT[SLOT1].Incr);
		pg_in2 = (CH->SLOT[SLOT2].Cnt += CH->SLOT[SLOT2].Incr);
		pg_in3 = (CH->SLOT[SLOT3].Cnt += CH->SLOT[SLOT3].Incr);
		pg_in4 = (CH->SLOT[SLOT4].Cnt += CH->SLOT[SLOT4].Incr);
	}

	/* Envelope Generator */
	FM_CALC_EG(eg_out1,CH->SLOT[SLOT1]);
	FM_CALC_EG(eg_out2,CH->SLOT[SLOT2]);
	FM_CALC_EG(eg_out3,CH->SLOT[SLOT3]);
	FM_CALC_EG(eg_out4,CH->SLOT[SLOT4]);

	/* Connection */
	if( eg_out1 < EG_CUT_OFF )	/* SLOT 1 */
	{
		if( CH->FB ){
			/* with self feed back */
			pg_in1 += (CH->op1_out[0]+CH->op1_out[1])>>CH->FB;
			CH->op1_out[1] = CH->op1_out[0];
		}
		CH->op1_out[0] = OP_OUT(pg_in1,eg_out1);
		/* output slot1 */
		if( !CH->connect1 )
		{
			/* algorythm 5  */
			pg_in2 += CH->op1_out[0];
			pg_in3 += CH->op1_out[0];
			pg_in4 += CH->op1_out[0];
		}else{
			/* other algorythm */
			*CH->connect1 += CH->op1_out[0];
		}
	}
	if( eg_out2 < EG_CUT_OFF )	/* SLOT 2 */
		*CH->connect2 += OP_OUT(pg_in2,eg_out2);
	if( eg_out3 < EG_CUT_OFF )	/* SLOT 3 */
		*CH->connect3 += OP_OUT(pg_in3,eg_out3);
	if( eg_out4 < EG_CUT_OFF )	/* SLOT 4 */
		*CH->connect4 += OP_OUT(pg_in4,eg_out4);
}
/* ---------- frequency counter for operater update ---------- */
static __inline__ void CALC_FCSLOT(FM_SLOT *SLOT , int fc , int kc )
{
	int ksr;

	/* frequency step counter */
	SLOT->Incr= (fc+SLOT->DT[kc])*SLOT->mul;	/* verified on real chip */
	/*	SLOT->Incr= fc*SLOT->mul + SLOT->DT[kc]; */
	ksr = kc >> SLOT->KSR;
	if( SLOT->ksr != ksr )
	{
		SLOT->ksr = ksr;
		/* attack , decay rate recalcration */
		SLOT->evsa = SLOT->AR[ksr];
		SLOT->evsd = SLOT->DR[ksr];
		SLOT->evss = SLOT->SR[ksr];
		SLOT->evsr = SLOT->RR[ksr];
	}
}

/* ---------- frequency counter  ---------- */
static __inline__ void OPN_CALC_FCOUNT(FM_CH *CH )
{
	if( CH->SLOT[SLOT1].Incr==-1){
		int fc = CH->fc;
		int kc = CH->kcode;
		CALC_FCSLOT(&CH->SLOT[SLOT1] , fc , kc );
		CALC_FCSLOT(&CH->SLOT[SLOT2] , fc , kc );
		CALC_FCSLOT(&CH->SLOT[SLOT3] , fc , kc );
		CALC_FCSLOT(&CH->SLOT[SLOT4] , fc , kc );
	}
}

/* ----------- initialize time tabls ----------- */
static void init_timetables( FM_ST *ST , UINT8 *DTTABLE , int ARRATE , int DRRATE )
{
	int i,d;
	double rate;

	/* DeTune table */
	for (d = 0;d <= 3;d++){
		for (i = 0;i <= 31;i++){
			rate = (double)DTTABLE[d*32 + i] * ST->freqbase * FREQ_RATE;
			ST->DT_TABLE[d][i]   = (INT32) rate;
			ST->DT_TABLE[d+4][i] = (INT32)-rate;
		}
	}
	/* make Attack & Decay tables */
	for (i = 0;i < 4;i++) ST->AR_TABLE[i] = ST->DR_TABLE[i] = 0;
	for (i = 4;i < 64;i++){
		rate  = ST->freqbase;						/* frequency rate */
		if( i < 60 ) rate *= 1.0+(i&3)*0.25;		/* b0-1 : x1 , x1.25 , x1.5 , x1.75 */
		rate *= 1<<((i>>2)-1);						/* b2-5 : shift bit */
		rate *= (double)(EG_ENT<<ENV_BITS);
		ST->AR_TABLE[i] = (INT32)(rate / ARRATE);
		ST->DR_TABLE[i] = (INT32)(rate / DRRATE);
	}
	ST->AR_TABLE[62] = EG_AED;
	ST->AR_TABLE[63] = EG_AED;
	for (i = 64;i < 94 ;i++){	/* make for overflow area */
		ST->AR_TABLE[i] = ST->AR_TABLE[63];
		ST->DR_TABLE[i] = ST->DR_TABLE[63];
	}

}

/* ---------- reset one of channel  ---------- */
static void reset_channel( FM_ST *ST , FM_CH *CH , int chan )
{
	int c,s;

	ST->mode   = 0;	/* normal mode */
	FM_STATUS_RESET(ST,0xff);
	ST->TA     = 0;
	ST->TAC    = 0;
	ST->TB     = 0;
	ST->TBC    = 0;

	for( c = 0 ; c < chan ; c++ )
	{
		CH[c].fc = 0;
		CH[c].PAN = OUTD_CENTER;
		for(s = 0 ; s < 4 ; s++ )
		{
			CH[c].SLOT[s].SEG = 0;
			CH[c].SLOT[s].eg_next= FM_EG_Release;
			CH[c].SLOT[s].evc = EG_OFF;
			CH[c].SLOT[s].eve = EG_OFF+1;
			CH[c].SLOT[s].evs = 0;
		}
	}
}


/* ---------- generic table initialize ---------- */
static int FMInitTable( void )
{
	int s,t;
	double rate;
	int i,j;
	double pom;

	/* allocate total level table plus+minus section */
	TL_TABLE = (INT32 *)emu_Malloc(2*TL_MAX*sizeof(int));
	if( TL_TABLE == 0 ) return 0;
	/* make total level table */
	for (t = 0;t < TL_MAX ;t++){
		if(t >= PG_CUT_OFF)
			rate = 0;	/* under cut off area */
		else
			rate = ((1<<TL_BITS)-1)/pow(10,EG_STEP*t/20);	/* dB -> voltage */
		TL_TABLE[       t] =  (int)rate;
		TL_TABLE[TL_MAX+t] = -TL_TABLE[t];
	}

	/* make sinwave table (pointer of total level) */
	for (s = 1;s <= SIN_ENT/4;s++){
		pom = sin(2.0*PI*s/SIN_ENT); /* sin   */
		pom = 20*log10(1/pom);	     /* -> decibel */
		j = (int)(pom / EG_STEP);    /* TL_TABLE steps */
		/* cut off check */
		if(j > PG_CUT_OFF)
			j = PG_CUT_OFF;
		/* degree 0   -  90    , degree 180 -  90 : plus section */
		SIN_TABLE[          s] = SIN_TABLE[SIN_ENT/2-s] = &TL_TABLE[j];
		/* degree 180 - 270    , degree 360 - 270 : minus section */
		SIN_TABLE[SIN_ENT/2+s] = SIN_TABLE[SIN_ENT  -s] = &TL_TABLE[TL_MAX+j];

	}
	/* degree 0 = degree 180                   = off */
	SIN_TABLE[0] = SIN_TABLE[SIN_ENT/2]        = &TL_TABLE[PG_CUT_OFF];

	/* envelope counter -> envelope output table */
	for (i=0; i<EG_ENT; i++)
	{
		/* ATTACK curve */
		/* !!!!! preliminary !!!!! */
		pom = pow( ((double)(EG_ENT-1-i)/EG_ENT) , 8 ) * EG_ENT;
		/* if( pom >= EG_ENT ) pom = EG_ENT-1; */
		ENV_CURVE[i] = (int)pom;
		/* DECAY ,RELEASE curve */
		ENV_CURVE[(EG_DST>>ENV_BITS)+i]= i;
#if FM_SEG_SUPPORT
		/* DECAY UPSIDE (SSG ENV) */
		ENV_CURVE[(EG_UST>>ENV_BITS)+i]= EG_ENT-1-i;
#endif
	}
	/* off */
	ENV_CURVE[EG_OFF>>ENV_BITS]= EG_ENT-1;

	/* decay to reattack envelope converttable */
	j = EG_ENT-1;
	for (i=0; i<EG_ENT; i++)
	{
		while( j && (ENV_CURVE[j] < i) ) j--;
		DRAR_TABLE[i] = j<<ENV_BITS;

	}
	return 1;
}


static void FMCloseTable( void )
{
	if( TL_TABLE ) emu_Free( TL_TABLE );
	return;
}

/* OPN/OPM Mode  Register Write */
static __inline__ void FMSetMode( FM_ST *ST ,int n,int v )
{
	/* b7 = CSM MODE */
	/* b6 = 3 slot mode */
	/* b5 = reset b */
	/* b4 = reset a */
	/* b3 = timer enable b */
	/* b2 = timer enable a */
	/* b1 = load b */
	/* b0 = load a */
	ST->mode = v;

	/* reset Timer b flag */
	if( v & 0x20 )
		FM_STATUS_RESET(ST,0x02);
	/* reset Timer a flag */
	if( v & 0x10 )
		FM_STATUS_RESET(ST,0x01);
	/* load b */
	if( v & 0x02 )
	{
		if( ST->TBC == 0 )
		{
			ST->TBC = ( 256-ST->TB)<<4;
			/* External timer handler */
			if (ST->Timer_Handler) (ST->Timer_Handler)(n,1,ST->TBC,ST->TimerBase);
		}
	}else if (ST->timermodel == FM_TIMER_INTERVAL)
	{	/* stop interbval timer */
		if( ST->TBC != 0 )
		{
			ST->TBC = 0;
			if (ST->Timer_Handler) (ST->Timer_Handler)(n,1,0,ST->TimerBase);
		}
	}
	/* load a */
	if( v & 0x01 )
	{
		if( ST->TAC == 0 )
		{
			ST->TAC = (1024-ST->TA);
			/* External timer handler */
			if (ST->Timer_Handler) (ST->Timer_Handler)(n,0,ST->TAC,ST->TimerBase);
		}
	}else if (ST->timermodel == FM_TIMER_INTERVAL)
	{	/* stop interbval timer */
		if( ST->TAC != 0 )
		{
			ST->TAC = 0;
			if (ST->Timer_Handler) (ST->Timer_Handler)(n,0,0,ST->TimerBase);
		}
	}
}

/* Timer A Overflow */
static __inline__ void TimerAOver(FM_ST *ST)
{
	/* status set if enabled */
	if(ST->mode & 0x04) FM_STATUS_SET(ST,0x01);
	/* clear or reload the counter */
	if (ST->timermodel == FM_TIMER_INTERVAL)
	{
		ST->TAC = (1024-ST->TA);
		if (ST->Timer_Handler) (ST->Timer_Handler)(ST->index,0,ST->TAC,ST->TimerBase);
	}
	else ST->TAC = 0;
}
/* Timer B Overflow */
static __inline__ void TimerBOver(FM_ST *ST)
{
	/* status set if enabled */
	if(ST->mode & 0x08) FM_STATUS_SET(ST,0x02);
	/* clear or reload the counter */
	if (ST->timermodel == FM_TIMER_INTERVAL)
	{
		ST->TBC = ( 256-ST->TB)<<4;
		if (ST->Timer_Handler) (ST->Timer_Handler)(ST->index,1,ST->TBC,ST->TimerBase);
	}
	else ST->TBC = 0;
}
/* CSM Key Controll */
static __inline__ void CSMKeyControll(FM_CH *CH)
{
	/* all key off */
	/* FM_KEYOFF(CH,SLOT1); */
	/* FM_KEYOFF(CH,SLOT2); */
	/* FM_KEYOFF(CH,SLOT3); */
	/* FM_KEYOFF(CH,SLOT4); */
	/* total level latch */
	CH->SLOT[SLOT1].TLL = CH->SLOT[SLOT1].TL;
	CH->SLOT[SLOT2].TLL = CH->SLOT[SLOT2].TL;
	CH->SLOT[SLOT3].TLL = CH->SLOT[SLOT3].TL;
	CH->SLOT[SLOT4].TLL = CH->SLOT[SLOT4].TL;
	/* all key on */
	FM_KEYON(CH,SLOT1);
	FM_KEYON(CH,SLOT2);
	FM_KEYON(CH,SLOT3);
	FM_KEYON(CH,SLOT4);
}

#if BUILD_OPN
/***********************************************************/
/* OPN unit                                                */
/***********************************************************/

/* OPN 3slot struct */
typedef struct opn_3slot {
	UINT32  fc[3];		/* fnum3,blk3  :calcrated */
	UINT8 fn_h[3];		/* freq3 latch            */
	UINT8 kcode[3];		/* key code    :          */
}FM_3SLOT;

/* OPN/A/B common state */
typedef struct opn_f {
	UINT8 type;		/* chip type         */
	FM_ST ST;				/* general state     */
	FM_3SLOT SL3;			/* 3 slot mode state */
	FM_CH *P_CH;			/* pointer of CH     */
	UINT32 FN_TABLE[2048]; /* fnumber -> increment counter */
#if FM_LFO_SUPPORT
	/* LFO */
	UINT32 LFOCnt;
	UINT32 LFOIncr;
	UINT32 LFO_FREQ[8];/* LFO FREQ table */
#endif
} FM_OPN;

/* OPN key frequency number -> key code follow table */
/* fnum higher 4bit -> keycode lower 2bit */
static const UINT8 OPN_FKTABLE[16]={0,0,0,0,0,0,0,1,2,3,3,3,3,3,3,3};

#if FM_LFO_SUPPORT
/* OPN LFO waveform table */
static INT32 OPN_LFO_wave[LFO_ENT];
#endif

static int OPNInitTable(void)
{
	int i;

#if FM_LFO_SUPPORT
	/* LFO wave table */
	for(i=0;i<LFO_ENT;i++)
	{
		OPN_LFO_wave[i]= i<LFO_ENT/2 ? i*LFO_RATE/(LFO_ENT/2) : (LFO_ENT-i)*LFO_RATE/(LFO_ENT/2);
	}
#endif
	return FMInitTable();
}

/* ---------- priscaler set(and make time tables) ---------- */
static void OPNSetPris(FM_OPN *OPN , int pris , int TimerPris, int SSGpris)
{
	int i;

	/* frequency base */
	OPN->ST.freqbase = (OPN->ST.rate) ? ((double)OPN->ST.clock / OPN->ST.rate) / pris : 0;
	/* Timer base time */
	OPN->ST.TimerBase = 1.0/((double)OPN->ST.clock / (double)TimerPris);
//  /* SSG part  priscaler set */
//  if( SSGpris ) SSGClk( OPN->ST.index, OPN->ST.clock * 2 / SSGpris );
	/* make time tables */
	init_timetables( &OPN->ST , OPN_DTTABLE , OPN_ARRATE , OPN_DRRATE );
	/* make fnumber -> increment counter table */
	for( i=0 ; i < 2048 ; i++ )
	{
		/* it is freq table for octave 7 */
		/* opn freq counter = 20bit */
		OPN->FN_TABLE[i] = (UINT32)( (double)i * OPN->ST.freqbase * FREQ_RATE * (1<<7) / 2 );
	}
#if FM_LFO_SUPPORT
	/* LFO freq. table */
	{
		/* 3.98Hz,5.56Hz,6.02Hz,6.37Hz,6.88Hz,9.63Hz,48.1Hz,72.2Hz @ 8MHz */
#define FM_LF(Hz) ((double)LFO_ENT*(1<<LFO_SHIFT)*(Hz)/(8000000.0/144))
		static const double freq_table[8] = { FM_LF(3.98),FM_LF(5.56),FM_LF(6.02),FM_LF(6.37),FM_LF(6.88),FM_LF(9.63),FM_LF(48.1),FM_LF(72.2) };
#undef FM_LF
		for(i=0;i<8;i++)
		{
			OPN->LFO_FREQ[i] = (UINT32)(freq_table[i] * OPN->ST.freqbase);
		}
	}
#endif

}

/* ---------- write a OPN mode register 0x20-0x2f ---------- */
static void OPNWriteMode(FM_OPN *OPN, int r, int v)
{
	UINT8 c;
	FM_CH *CH;

	switch(r){
	case 0x21:	/* Test */
		break;
#if FM_LFO_SUPPORT
	case 0x22:	/* LFO FREQ (YM2608/YM2612) */
		if( OPN->type & TYPE_LFOPAN )
		{
			OPN->LFOIncr = (v&0x08) ? OPN->LFO_FREQ[v&7] : 0;
			cur_chip = NULL;
		}
		break;
#endif
	case 0x24:	/* timer A High 8*/
		OPN->ST.TA = (OPN->ST.TA & 0x03)|(((int)v)<<2);
		break;
	case 0x25:	/* timer A Low 2*/
		OPN->ST.TA = (OPN->ST.TA & 0x3fc)|(v&3);
		break;
	case 0x26:	/* timer B */
		OPN->ST.TB = v;
		break;
	case 0x27:	/* mode , timer controll */
		FMSetMode( &(OPN->ST),OPN->ST.index,v );
		break;
	case 0x28:	/* key on / off */
		c = v&0x03;
		if( c == 3 ) break;
		if( (v&0x04) && (OPN->type & TYPE_6CH) ) c+=3;
		CH = OPN->P_CH;
		CH = &CH[c];
		/* csm mode */
		/* if( c == 2 && (OPN->ST.mode & 0x80) ) break; */
		if(v&0x10) FM_KEYON(CH,SLOT1); else FM_KEYOFF(CH,SLOT1);
		if(v&0x20) FM_KEYON(CH,SLOT2); else FM_KEYOFF(CH,SLOT2);
		if(v&0x40) FM_KEYON(CH,SLOT3); else FM_KEYOFF(CH,SLOT3);
		if(v&0x80) FM_KEYON(CH,SLOT4); else FM_KEYOFF(CH,SLOT4);

		break;
	}
}

/* ---------- write a OPN register (0x30-0xff) ---------- */
static void OPNWriteReg(FM_OPN *OPN, int r, int v)
{
	UINT8 c;
	FM_CH *CH;
	FM_SLOT *SLOT;

	/* 0x30 - 0xff */
	if( (c = OPN_CHAN(r)) == 3 ) return; /* 0xX3,0xX7,0xXB,0xXF */
	if( (r >= 0x100) /* && (OPN->type & TYPE_6CH) */ ) c+=3;
		CH = OPN->P_CH;
		CH = &CH[c];

	SLOT = &(CH->SLOT[OPN_SLOT(r)]);
	switch( r & 0xf0 ) {
	case 0x30:	/* DET , MUL */
		set_det_mul(&OPN->ST,CH,SLOT,v);
		break;
	case 0x40:	/* TL */
		set_tl(CH,SLOT,v,(c == 2) && (OPN->ST.mode & 0x80) );
		break;
	case 0x50:	/* KS, AR */
		set_ar_ksr(CH,SLOT,v,OPN->ST.AR_TABLE);
		break;
	case 0x60:	/*     DR */
		/* bit7 = AMS_ON ENABLE(YM2612) */
		set_dr(SLOT,v,OPN->ST.DR_TABLE);
#if FM_LFO_SUPPORT
		if( OPN->type & TYPE_LFOPAN)
		{
			SLOT->amon = v>>7;
			SLOT->ams = CH->ams * SLOT->amon;
		}
#endif
		break;
	case 0x70:	/*     SR */
		set_sr(SLOT,v,OPN->ST.DR_TABLE);
		break;
	case 0x80:	/* SL, RR */
		set_sl_rr(SLOT,v,OPN->ST.DR_TABLE);
		break;
	case 0x90:	/* SSG-EG */
		SLOT->SEG = v&0x0f;
		break;
	case 0xa0:
		switch( OPN_SLOT(r) ){
		case 0:		/* 0xa0-0xa2 : FNUM1 */
			{
				UINT32 fn  = (((UINT32)( (CH->fn_h)&7))<<8) + v;
				UINT8 blk = CH->fn_h>>3;
				/* make keyscale code */
				CH->kcode = (blk<<2)|OPN_FKTABLE[(fn>>7)];
				/* make basic increment counter 32bit = 1 cycle */
				CH->fc = OPN->FN_TABLE[fn]>>(7-blk);
				CH->SLOT[SLOT1].Incr=-1;
			}
			break;
		case 1:		/* 0xa4-0xa6 : FNUM2,BLK */
			CH->fn_h = v&0x3f;
			break;
		case 2:		/* 0xa8-0xaa : 3CH FNUM1 */
			if( r < 0x100)
			{
				UINT32 fn  = (((UINT32)(OPN->SL3.fn_h[c]&7))<<8) + v;
				UINT8 blk = OPN->SL3.fn_h[c]>>3;
				/* make keyscale code */
				OPN->SL3.kcode[c]= (blk<<2)|OPN_FKTABLE[(fn>>7)];
				/* make basic increment counter 32bit = 1 cycle */
				OPN->SL3.fc[c] = OPN->FN_TABLE[fn]>>(7-blk);
				(OPN->P_CH)[2].SLOT[SLOT1].Incr=-1;
			}
			break;
		case 3:		/* 0xac-0xae : 3CH FNUM2,BLK */
			if( r < 0x100)
				OPN->SL3.fn_h[c] = v&0x3f;
			break;
		}
		break;
	case 0xb0:
		switch( OPN_SLOT(r) ){
		case 0:		/* 0xb0-0xb2 : FB,ALGO */
			{
				int feedback = (v>>3)&7;
				CH->ALGO = v&7;
				CH->FB   = feedback ? 8+1 - feedback : 0;
				setup_connection( CH );
			}
			break;
		case 1:		/* 0xb4-0xb6 : L , R , AMS , PMS (YM2612/YM2608) */
			if( OPN->type & TYPE_LFOPAN)
			{
#if FM_LFO_SUPPORT
				/* b0-2 PMS */
				/* 0,3.4,6.7,10,14,20,40,80(cent) */
				static const double pmd_table[8]={0,3.4,6.7,10,14,20,40,80};
				static const int amd_table[4]={(int)(0/EG_STEP),(int)(1.4/EG_STEP),(int)(5.9/EG_STEP),(int)(11.8/EG_STEP) };
				CH->pms = (INT32)( (1.5/1200.0)*pmd_table[v & 7] * PMS_RATE);
				/* b4-5 AMS */
				/* 0 , 1.4 , 5.9 , 11.8(dB) */
				CH->ams = amd_table[(v>>4) & 0x03];
				CH->SLOT[SLOT1].ams = CH->ams * CH->SLOT[SLOT1].amon;
				CH->SLOT[SLOT2].ams = CH->ams * CH->SLOT[SLOT2].amon;
				CH->SLOT[SLOT3].ams = CH->ams * CH->SLOT[SLOT3].amon;
				CH->SLOT[SLOT4].ams = CH->ams * CH->SLOT[SLOT4].amon;
#endif
				/* PAN */
				CH->PAN = (v>>6)&0x03; /* PAN : b6 = R , b7 = L */
				setup_connection( CH );

			}
			break;
		}
		break;
	}
}
#endif /* BUILD_OPN */

#if BUILD_YM2612
/*******************************************************************************/
/*		YM2612 local section                                                   */
/*******************************************************************************/
/* here's the virtual YM2612 */
typedef struct ym2612_f {
	FM_OPN OPN;						/* OPN state       */
	FM_CH CH[6];					/* channel state */
	int address1;	/* address register1 */
	/* dac output (YM2612) */
	int dacen;
	int dacout;
} YM2612;

static int YM2612NumChips;	/* total chip */
static YM2612 *FM2612=NULL;	/* array of YM2612's */

static int dacen;

/* ---------- update one of chip ----------- */
void YM2612UpdateOne(int num, INT16 **buffer, int length)
{
	YM2612 *F2612 = &(FM2612[num]);
	FM_OPN *OPN   = &(FM2612[num].OPN);
	int i;
	FM_CH *ch,*ech;
	FMSAMPLE  *bufL,*bufR;
	int dacout  = F2612->dacout;

	/* set bufer */
	bufL = buffer[0];
	bufR = buffer[1];

	if( (void *)F2612 != cur_chip ){
		cur_chip = (void *)F2612;

		State = &OPN->ST;
		cch[0]   = &F2612->CH[0];
		cch[1]   = &F2612->CH[1];
		cch[2]   = &F2612->CH[2];
		cch[3]   = &F2612->CH[3];
		cch[4]   = &F2612->CH[4];
		cch[5]   = &F2612->CH[5];
		/* DAC mode */
		dacen = F2612->dacen;
#if FM_LFO_SUPPORT
		LFOCnt  = OPN->LFOCnt;
		LFOIncr = OPN->LFOIncr;
		if( !LFOIncr ) lfo_amd = lfo_pmd = 0;
#endif
	}
	/* update frequency counter */
	OPN_CALC_FCOUNT( cch[0] );
	OPN_CALC_FCOUNT( cch[1] );
	if( (State->mode & 0xc0) ){
		/* 3SLOT MODE */
		if( cch[2]->SLOT[SLOT1].Incr==-1){
			/* 3 slot mode */
			CALC_FCSLOT(&cch[2]->SLOT[SLOT1] , OPN->SL3.fc[1] , OPN->SL3.kcode[1] );
			CALC_FCSLOT(&cch[2]->SLOT[SLOT2] , OPN->SL3.fc[2] , OPN->SL3.kcode[2] );
			CALC_FCSLOT(&cch[2]->SLOT[SLOT3] , OPN->SL3.fc[0] , OPN->SL3.kcode[0] );
			CALC_FCSLOT(&cch[2]->SLOT[SLOT4] , cch[2]->fc , cch[2]->kcode );
		}
	}else OPN_CALC_FCOUNT( cch[2] );
	OPN_CALC_FCOUNT( cch[3] );
	OPN_CALC_FCOUNT( cch[4] );
	OPN_CALC_FCOUNT( cch[5] );

	ech = dacen ? cch[4] : cch[5];
	/* buffering */
    for( i=0; i < length ; i++ )
	{
#if FM_LFO_SUPPORT
		/* LFO */
		if( LFOIncr )
		{
			lfo_amd = OPN_LFO_wave[(LFOCnt+=LFOIncr)>>LFO_SHIFT];
			lfo_pmd = lfo_amd-(LFO_RATE/2);
		}
#endif
		/* clear output acc. */
		out_ch[OUTD_LEFT] = out_ch[OUTD_RIGHT]= out_ch[OUTD_CENTER] = 0;
		/* calcrate channel output */
		for(ch = cch[0] ; ch <= ech ; ch++)
			FM_CALC_CH( ch );
		if( dacen )  *cch[5]->connect4 += dacout;
		/* buffering */
		FM_BUFFERING_STEREO;
		/* timer A controll */
		INTERNAL_TIMER_A( State , cch[2] )
	}
	INTERNAL_TIMER_B(State,length)
#if FM_LFO_SUPPORT
	OPN->LFOCnt = LFOCnt;
#endif
}

/* -------------------------- YM2612 ---------------------------------- */
int YM2612Init(int num, int clock, int rate,
               FM_TIMERHANDLER TimerHandler,FM_IRQHANDLER IRQHandler)
{
	int i;

    if (FM2612) return (-1);	/* duplicate init. */
    cur_chip = NULL;	/* hiro-shi!! */

	YM2612NumChips = num;

	/* allocate extend state space */
	if( (FM2612 = (YM2612 *)emu_Malloc(sizeof(YM2612) * YM2612NumChips))==NULL)
		return (-1);
	/* clear */
	memset(FM2612,0,sizeof(YM2612) * YM2612NumChips);
	/* allocate total level table (128kb space) */
	if( !OPNInitTable() )
	{
		emu_Free( FM2612 );
		return (-1);
	}

	for ( i = 0 ; i < YM2612NumChips; i++ ) {
		FM2612[i].OPN.ST.index = i;
		FM2612[i].OPN.type = TYPE_YM2612;
		FM2612[i].OPN.P_CH = FM2612[i].CH;
		FM2612[i].OPN.ST.clock = clock;
		FM2612[i].OPN.ST.rate = rate;
		/* FM2612[i].OPN.ST.irq = 0; */
		/* FM2612[i].OPN.ST.status = 0; */
		FM2612[i].OPN.ST.timermodel = FM_TIMER_INTERVAL;
		/* Extend handler */
		FM2612[i].OPN.ST.Timer_Handler = TimerHandler;
		FM2612[i].OPN.ST.IRQ_Handler   = IRQHandler;
		YM2612ResetChip(i);
	}
	return 0;
}

/* ---------- shut down emurator ----------- */
void YM2612Shutdown()
{
    if (!FM2612) return;

	FMCloseTable();
	emu_Free(FM2612);
	FM2612 = NULL;
}

/* ---------- reset one of chip ---------- */
void YM2612ResetChip(int num)
{
	int i;
	YM2612 *F2612 = &(FM2612[num]);
	FM_OPN *OPN   = &(FM2612[num].OPN);

	OPNSetPris( OPN , 12*12, 12*12, 0);
	/* status clear */
	FM_IRQMASK_SET(&OPN->ST,0x03);
	OPNWriteMode(OPN,0x27,0x30); /* mode 0 , timer reset */

	reset_channel( &OPN->ST , &F2612->CH[0] , 6 );

	for(i = 0xb6 ; i >= 0xb4 ; i-- )
	{
		OPNWriteReg(OPN,i      ,0xc0);
		OPNWriteReg(OPN,i|0x100,0xc0);
	}
	for(i = 0xb2 ; i >= 0x30 ; i-- )
	{
		OPNWriteReg(OPN,i      ,0);
		OPNWriteReg(OPN,i|0x100,0);
	}
	for(i = 0x26 ; i >= 0x20 ; i-- ) OPNWriteReg(OPN,i,0);
	/* DAC mode clear */
	F2612->dacen = 0;
}

/* YM2612 write */
/* n = number  */
/* a = address */
/* v = value   */
int YM2612Write(int n, int a,UINT8 v)
{
	YM2612 *F2612 = &(FM2612[n]);
	int addr;

	switch( a&3){
	case 0:	/* address port 0 */
		F2612->OPN.ST.address = v & 0xff;
		break;
	case 1:	/* data port 0    */
		addr = F2612->OPN.ST.address;
		switch( addr & 0xf0 )
		{
		case 0x20:	/* 0x20-0x2f Mode */
			switch( addr )
			{
			case 0x2a:	/* DAC data (YM2612) */
				F2612->dacout = ((int)v - 0x80)<<(TL_BITS-7);
                                F2612->dacout = ((int)(v/2)-0x80)<<(TL_BITS-7);
				break;
			case 0x2b:	/* DAC Sel  (YM2612) */
				/* b7 = dac enable */
				F2612->dacen = v & 0x80;
				cur_chip = NULL;
				break;
			default:	/* OPN section */
				/* write register */
				 OPNWriteMode(&(F2612->OPN),addr,v);
			}
			break;
		default:	/* 0x30-0xff OPN section */
			/* write register */
			 OPNWriteReg(&(F2612->OPN),addr,v);
		}
		break;
	case 2:	/* address port 1 */
		F2612->address1 = v & 0xff;
		break;
	case 3:	/* data port 1    */
		addr = F2612->address1;
		OPNWriteReg(&(F2612->OPN),addr|0x100,v);
		break;
	}
	return F2612->OPN.ST.irq;
}
UINT8 YM2612Read(int n,int a)
{
	YM2612 *F2612 = &(FM2612[n]);

	switch( a&3){
	case 0:	/* status 0 */
		return F2612->OPN.ST.status;
	case 1:
	case 2:
	case 3:
		return F2612->OPN.ST.status;
	}
	return 0;
}

int YM2612TimerOver(int n,int c)
{
	YM2612 *F2612 = &(FM2612[n]);

	if( c )
	{	/* Timer B */
		TimerBOver( &(F2612->OPN.ST) );
	}
	else
	{	/* Timer A */
		/* timer update */
		TimerAOver( &(F2612->OPN.ST) );
		/* CSM mode key,TL controll */
		if( F2612->OPN.ST.mode & 0x80 )
		{	/* CSM mode total level latch and auto key on */
			CSMKeyControll( &(F2612->CH[2]) );
		}
	}
	return F2612->OPN.ST.irq;
}

#endif /* BUILD_YM2612 */
