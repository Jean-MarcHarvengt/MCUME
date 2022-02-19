
#ifndef _H_FM_FM_
#define _H_FM_FM_

#define HAS_YM2612  1

#define BUILD_YM2612  (HAS_YM2612 || HAS_YM3438)

#define FM_STEREO_MIX       0
#define FM_OUTPUT_BIT       16
#define FM_INTERNAL_TIMER   0

#define FM_LFO_SUPPORT      1
#define FM_SEG_SUPPORT      0

#if BUILD_YM2612
  /* in 2612intf.c */
  #define YM2612UpdateReq(chip) YM2612UpdateRequest(chip);
#endif

/* compiler dependence */
#ifndef OSD_CPU_H
#define OSD_CPU_H
typedef unsigned char	UINT8;   /* unsigned  8bit */
typedef unsigned short	UINT16;  /* unsigned 16bit */
typedef unsigned int	UINT32;  /* unsigned 32bit */
typedef signed char		INT8;    /* signed  8bit   */
typedef signed short	INT16;   /* signed 16bit   */
typedef signed int		INT32;   /* signed 32bit   */
#endif

#define YM2203_NUMBUF 1
#if FM_STEREO_MIX
  #define YM2151_NUMBUF 1
  #define YM2608_NUMBUF 1
  #define YM2612_NUMBUF 1
  #define YM2610_NUMBUF 1
#else
  #define YM2151_NUMBUF 2    /* FM L+R */
  #define YM2608_NUMBUF 2    /* FM L+R+ADPCM+RYTHM */
  #define YM2610_NUMBUF 2    /* FM L+R+ADPCMA+ADPCMB */
  #define YM2612_NUMBUF 2    /* FM L+R */
#endif

#if (FM_OUTPUT_BIT==16)
typedef INT16 FMSAMPLE;
typedef unsigned long FMSAMPLE_MIX;
#endif
#if (FM_OUTPUT_BIT==8)
typedef unsigned char  FMSAMPLE;
typedef unsigned short FMSAMPLE_MIX;
#endif

typedef void (*FM_TIMERHANDLER)(int n,int c,int cnt,double stepTime);
typedef void (*FM_IRQHANDLER)(int n,int irq);
/* FM_TIMERHANDLER : Stop or Start timer         */
/* int n          = chip number                  */
/* int c          = Channel 0=TimerA,1=TimerB    */
/* int count      = timer count (0=stop)         */
/* doube stepTime = step time of one count (sec.)*/

/* FM_IRQHHANDLER : IRQ level changing sense     */
/* int n       = chip number                     */
/* int irq     = IRQ level 0=OFF,1=ON            */

#if BUILD_YM2203
/* -------------------- YM2203(OPN) Interface -------------------- */

/*
** Initialize YM2203 emulator(s).
**
** 'num'           is the number of virtual YM2203's to allocate
** 'baseclock'
** 'rate'          is sampling rate
** 'TimerHandler'  timer callback handler when timer start and clear
** 'IRQHandler'    IRQ callback handler when changed IRQ level
** return      0 = success
*/
int YM2203Init(int num, int baseclock, int rate,
               FM_TIMERHANDLER TimerHandler,FM_IRQHANDLER IRQHandler);

/*
** shutdown the YM2203 emulators .. make sure that no sound system stuff
** is touching our audio buffers ...
*/
void YM2203Shutdown(void);

/*
** reset all chip registers for YM2203 number 'num'
*/
void YM2203ResetChip(int num);
/*
** update one of chip
*/

void YM2203UpdateOne(int num, INT16 *buffer, int length);

/*
** Write
** return : InterruptLevel
*/
int YM2203Write(int n,int a,unsigned char v);
/*
** Read
** return : InterruptLevel
*/
unsigned char YM2203Read(int n,int a);

/*
**	Timer OverFlow
*/
int YM2203TimerOver(int n, int c);

#endif /* BUILD_YM2203 */

#if BUILD_YM2608
/* -------------------- YM2608(OPNA) Interface -------------------- */
int YM2608Init(int num, int baseclock, int rate,
               void **pcmroma,int *pcmsizea,short *rhythmrom,int *rhythmpos,
               FM_TIMERHANDLER TimerHandler,FM_IRQHANDLER IRQHandler);
void YM2608Shutdown(void);
void YM2608ResetChip(int num);
void YM2608UpdateOne(int num, INT16 **buffer, int length);

int YM2608Write(int n, int a,unsigned char v);
unsigned char YM2608Read(int n,int a);
int YM2608TimerOver(int n, int c );
#endif /* BUILD_YM2608 */

#if (BUILD_YM2610||BUILD_YM2610B)
/* -------------------- YM2610(OPNB) Interface -------------------- */
int YM2610Init(int num, int baseclock, int rate,
               void **pcmroma,int *pcmasize,void **pcmromb,int *pcmbsize,
               FM_TIMERHANDLER TimerHandler,FM_IRQHANDLER IRQHandler);
void YM2610Shutdown(void);
void YM2610ResetChip(int num);
void YM2610UpdateOne(int num, INT16 **buffer, int length);
#if BUILD_YM2610B
void YM2610BUpdateOne(int num, INT16 **buffer, int length);
#endif

int YM2610Write(int n, int a,unsigned char v);
unsigned char YM2610Read(int n,int a);
int YM2610TimerOver(int n, int c );

#endif /* BUILD_YM2610 */

#if BUILD_YM2612
int YM2612Init(int num, int baseclock, int rate,
               FM_TIMERHANDLER TimerHandler,FM_IRQHANDLER IRQHandler);
void YM2612Shutdown(void);
void YM2612ResetChip(int num);
void YM2612UpdateOne(int num, INT16 **buffer, int length);
int YM2612Write(int n, int a,unsigned char v);
unsigned char YM2612Read(int n,int a);
int YM2612TimerOver(int n, int c );

#endif /* BUILD_YM2612 */

#if BUILD_YM2151
/* -------------------- YM2151(OPM) Interface -------------------- */
int OPMInit(int num, int baseclock, int rate,
               FM_TIMERHANDLER TimerHandler,FM_IRQHANDLER IRQHandler);
void OPMShutdown(void);
void OPMResetChip(int num);

void OPMUpdateOne(int num, INT16 **buffer, int length );
/* ---- set callback hander when port CT0/1 write ----- */
/* CT.bit0 = CT0 , CT.bit1 = CT1 */
/*
typedef void (*mem_write_handler)(int offset,int data);
*/
void OPMSetPortHander(int n,mem_write_handler PortWrite);
/* JB 981119  - so it will match MAME's memory write functions scheme*/

int YM2151Write(int n,int a,unsigned char v);
unsigned char YM2151Read(int n,int a);
int YM2151TimerOver(int n,int c);
#endif /* BUILD_YM2151 */

#endif /* _H_FM_FM_ */
