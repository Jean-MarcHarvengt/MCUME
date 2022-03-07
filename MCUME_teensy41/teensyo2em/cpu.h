#ifndef CPU_H
#define CPU_H

#include "types.h"

extern Byte acc;		/* Accumulator */
extern ADDRESS pc;     /* Program counter */
extern long clk;		/* clock */

extern Byte itimer;		/* Internal timer */
extern Byte reg_pnt;	/* pointer to register bank */
extern Byte timer_on;  /* 0=timer off/1=timer on */
extern Byte count_on;  /* 0=count off/1=count on */

extern Byte t_flag;    /* Timer flag */

extern Byte psw;		/* Processor status word */
extern Byte sp;		/* Stack pointer (part of psw) */

extern Byte p1;		/* I/O port 1 */
extern Byte p2;    /* I/O port 2 */

extern Byte xirq_pend;
extern Byte tirq_pend;

void init_cpu(void);
void cpu_exec(void);
void ext_IRQ(void);
void tim_IRQ(void);
void make_psw_debug(void);


#endif  /* CPU_H */

