 /*
 * Castaway
 *  (C) 1994 - 2002 Martin Doering, Joachim Hoenig
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#ifndef STH
#define STH

#define MAX_DISC_SIZE 1050*1024


/*
 * I/O register addresses
 */
#define MEM_CONF        0xff8001

#define VID_BASEH       0xff8201
#define VID_BASEM       0xff8203
#define VID_ADRH        0xff8205
#define VID_ADRM        0xff8207
#define VID_ADRL        0xff8209
#define VID_SYNCMODE    0xff820a
#define VID_BASEL	    0xff820d
#define VID_LINEWIDTH   0xff820f
#define VID_COL0        0xff8240
#define VID_COL1        0xff8242
#define VID_COL2        0xff8244
#define VID_COL3        0xff8246
#define VID_COL4        0xff8248
#define VID_COL5        0xff824a
#define VID_COL6        0xff824c
#define VID_COL7        0xff824e
#define VID_COL8        0xff8250
#define VID_COL9        0xff8252
#define VID_COL10       0xff8254
#define VID_COL11       0xff8256
#define VID_COL12       0xff8258
#define VID_COL13       0xff825a
#define VID_COL14       0xff825c
#define VID_COL15       0xff825e
#define VID_SHIFTMODE   0xff8260

#define DMA_CAR         0xff8604
#define DMA_SCR         0xff8604
#define DMA_SR          0xff8606
#define DMA_MODE        0xff8606
#define DMA_ADRH        0xff8609
#define DMA_ADRM        0xff860b
#define DMA_ADRL        0xff860d

#define SND_RS          0xff8800
#define SND_WD          0xff8802

#define BLT_HFT         0xff8a00
#define BLT_SXINC       0xff8a20
#define BLT_SYINC       0xff8a22
#define BLT_SADR        0xff8a24
#define BLT_END1        0xff8a28
#define BLT_END2        0xff8a2a
#define BLT_END3        0xff8a2c
#define BLT_DXINC       0xff8a2e
#define BLT_DYINC       0xff8a30
#define BLT_DADR        0xff8a32
#define BLT_XCNT        0xff8a36
#define BLT_YCNT        0xff8a38
#define BLT_HOP         0xff8a3a
#define BLT_OP          0xff8a3b
#define BLT_STAT        0xff8a3c
#define BLT_SKEW        0xff8a3d

#define MFP_GPIP        0xfffa01
#define MFP_AER         0xfffa03
#define MFP_DDR         0xfffa05
#define MFP_IERA        0xfffa07
#define MFP_IERB        0xfffa09
#define MFP_IPRA        0xfffa0b
#define MFP_IPRB        0xfffa0d
#define MFP_ISRA        0xfffa0f
#define MFP_ISRB        0xfffa11
#define MFP_IMRA        0xfffa13
#define MFP_IMRB        0xfffa15
#define MFP_IVR         0xfffa17
#define MFP_TACR        0xfffa19
#define MFP_TBCR        0xfffa1b
#define MFP_TCDCR       0xfffa1d
#define MFP_TADR        0xfffa1f
#define MFP_TBDR        0xfffa21
#define MFP_TCDR        0xfffa23
#define MFP_TDDR        0xfffa25
#define MFP_SCR         0xfffa27
#define MFP_UCR         0xfffa29
#define MFP_RSR         0xfffa2b
#define MFP_TSR         0xfffa2d
#define MFP_UDR         0xfffa2f

#define ACIA1_SR        0xfffc00
#define ACIA1_DR        0xfffc02

#define ACIA2_SR        0xfffc04
#define ACIA2_DR        0xfffc06

#define RTC_SECL        0xfffc21
#define RTC_SECH        0xfffc23
#define RTC_MINL        0xfffc25
#define RTC_MINH        0xfffc27
#define RTC_HRSL        0xfffc29
#define RTC_HRSH        0xfffc2b
#define RTC_DAY         0xfffc2d
#define RTC_DAYL        0xfffc2f
#define RTC_DAYH        0xfffc31
#define RTC_MONL        0xfffc33
#define RTC_MONH        0xfffc35
#define RTC_YRL         0xfffc37
#define RTC_YRH         0xfffc39
#define RTC_RES1        0xfffc3b
#define RTC_RES2        0xfffc3d
#define RTC_RES3        0xfffc3f

#define IBS 32
#define OBS 64

/*
 * ROM/Cartridge file names
 */
extern char cartridge[80], rom[80];
/*
 * I/O register values
 */
extern uint8    memconf;
extern uint8    mfp_gpip, mfp_aer, mfp_ddr, mfp_iera, mfp_ierb, mfp_ipra, mfp_iprb,
                mfp_isra, mfp_isrb, mfp_imra, mfp_imrb, mfp_ivr, mfp_tacr,
                mfp_tbcr, mfp_tcdcr, mfp_scr, mfp_ucr, mfp_rsr, mfp_tsr, mfp_udr;

//Mfp delay timer variables
extern int32 mfp_reg[12];
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

extern uint8    acia1_cr, acia1_sr, acia1_dr, acia2_cr, acia2_sr,
                acia2_dr;

extern int checkedsample;
extern int checkedsound;
//Video shifter
extern uint32	vid_adr;
extern uint8    vid_baseh, vid_basem;
extern uint32   vid_mem;
extern uint8    vid_syncmode, vid_shiftmode;
extern int16    vid_col[];
extern int      vid_flag;


extern uint16   dma_car, dma_scr, dma_sr, dma_mode;
extern uint8    dma_adrh, dma_adrm, dma_adrl;
extern const int32	mfpcycletab[16];

extern uint32	psg[26];
//extern unsigned char sample[10000];
#define phase0 psg[16]
#define phase1 psg[17]
#define phase2 psg[18]
#define phase3 psg[19]
#define psg_epos psg[20]
#define psgcontrol psg[21]
#define phase4 psg[22]
#define nrand psg[23]
#define samppos psg[24]
#define lastpsg psg[25]

/* ikbd.c */
extern void     IkbdLoop(void);
extern void     IkbdRecv(uint8);
extern void     IkbdSend(uint8);
extern void     IkbdWriteBuffer(void);  /* write byte IKBD -> ST */
extern void     IkbdKeyPress(unsigned short keysym); /* key press */
extern void     IkbdKeyRelease(unsigned short keysym); /* key release */
extern void     IkbdMousePress(int);    /* mouse button press */
extern void     IkbdMouseRelease(int);  /* mouse button release */
extern void     IkbdMouseMotion(int x, int y); /* mouse movement */
//extern void     IkbdMouseMotion(int x, int y, int dx, int dy); /* mouse movement */
extern void	joystickevent(int joystate);
extern void	IkbdReset(void);


/* fdc.c */
extern unsigned char fdc_data, fdc_track, fdc_sector,
        fdc_status, fdc_command, fdc_int;
extern unsigned char	disk_ejected[2];
extern unsigned char	disk_changed[2];
struct Disk {
    int         file;
    char        name[80];
    int         head;
    int         sides;
    int         tracks;
    int         sectors;
    int         secsize;
    int         disksize;
};
extern struct Disk disk[2];
extern int      FDCInit(int i);
extern void     FDCCommand(void);
void FDCeject(int num);
int unzipdisk(unsigned char *RomPath,unsigned char *buf);

/* blitter.c */
extern uint16   blt_halftone[16];
extern int16    blt_src_x_inc, blt_src_y_inc;
extern uint32   blt_src_addr;
extern int16    blt_end_1, blt_end_2, blt_end_3;
extern int16    blt_dst_x_inc, blt_dst_y_inc;
extern uint32   blt_dst_addr;
extern uint16   blt_x_cnt, blt_y_cnt;
extern int8     blt_hop, blt_op, blt_status, blt_skew;

extern void     bitblt(void);

/* init.c */
extern int     Init();

/* main.c */
extern unsigned ips;

/* mem.c */
extern int     MemInit(void);
extern void     MemTableSet(uint32 base, uint32 size,
    void (*setbyte)(uint32, uint8),
    void (*setword)(uint32, uint16),
    void (*setlong)(uint32, uint32 ),
    uint8  (*getbyte)(uint32),
    uint16 (*getword)(uint32),
    uint32 (*getlong)(uint32) );

extern void RamSetB(uint32, uint8);
extern void RamSetW(uint32, uint16);
extern void RamSetL(uint32, uint32 );
extern uint8  RamGetB(uint32);
extern uint16 RamGetW(uint32);
extern uint32  RamGetL(uint32);
/*
 * Video shift modes
 */
#define COL4 0
#define COL2 1
#define MONO 2
extern int display_mode;

/*
 * Read/Write IO Registers (return value != 0 if bus error)
 */
uint8            DoIORB(uint32 address);
uint16           DoIORW(uint32 address);
uint32           DoIORL(uint32 address);
void             DoIOWB(uint32 address, uint8 value);
void             DoIOWW(uint32 address, uint16 value);
void             DoIOWL(uint32 address, uint32 value);

void IOInit(void);
struct _mouse {
    char            buttons;    /* real buttons */
    char            stbuttons;  /* buttons as known to st */
    int             xscale, yscale;
    int             xmax, ymax;
    int             xkcm, ykcm;
    int             xth, yth;
    int             x, y;       /* real mouse position */
    int             stx, sty;   /* mouse position st */
    int             button_action;
    int             mode;
    int             yinv;
    int		    flag;
};


struct _joystick {
    int             mode;
    int             rate;
    int             state0; /* joystick 0 state */
    int             state1; /* joystick 1 state */
    int             rx, ry, tx, ty, vx, vy;
};


extern struct _mouse mouse;
extern struct _joystick joystick;

void  Ikbd_Reset(void);
void  IkbdJoystickChange(int number, uint8 state);
int unzipdisk(unsigned char *RomPath,unsigned char *buf);
void reset_st_video(void);

#endif
