/*
 $Id: extern.h,v 1.2 1995/12/12 16:21:39 alex Exp alex $
 */

#ifndef VCSEXTERN_H
#define VCSEXTERN_H


#include "types.h"	/* for BYTE, ADDRESS, etc. types and structures */
/* #include "proto.h" */

extern char *progname;
extern int   clength[];

extern BYTE  accumulator;
extern BYTE  x_register;
extern BYTE  y_register;
extern BYTE  stack_pointer;
extern BYTE  status_register;
extern ADDRESS program_counter;
extern CLOCK clk;

extern int   zero_flag;
extern int   sign_flag;
extern int   overflow_flag;
extern int   break_flag;
extern int   decimal_flag;
extern int   interrupt_flag;
extern int   carry_flag;

/* Debugging */
extern int   hexflg;
extern int   verflg;
extern int   traceflg;
extern int   debugflg;
extern int   runflg;

extern int   autodump;
extern int   reuflg;


/* Memory */

extern int module;

/* Keyboard & joystick */

#ifdef JOYSTICK
extern void joystick(void) ;
extern void joyini(void) ;
extern void joyclose(void);

extern int joyfd ;
#endif

#endif






