 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * AutoConfig devices
  *
  * Copyright 1995, 1996 Bernd Schmidt
  * Copyright 1996 Ed Hanway
  */

#include "shared.h"

#include "machdep/m68k.h"
#include "uae.h"
#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "disk.h"
#include "xwin.h"
#include "autoconf.h"

/* We'll need a lot of these. */
#define MAX_TRAPS 32 //4096
static TrapFunction traps[MAX_TRAPS];
static uae_u8 trapmode[MAX_TRAPS];
static uaecptr trapoldfunc[MAX_TRAPS];

static int max_trap = 0;


static uae_u32 m68k_mode_return(void)
{
    return 0;
}


/* Commonly used autoconfig strings */

uaecptr EXPANSION_explibname, EXPANSION_doslibname, EXPANSION_uaeversion;
uaecptr EXPANSION_uaedevname, EXPANSION_explibbase = 0, EXPANSION_haveV36;
uaecptr EXPANSION_bootcode, EXPANSION_nullfunc;

static int current_deviceno = 0;

int get_new_device(char **devname, uaecptr *devname_amiga)
{
    char buffer[80];
    strcpy(buffer,"UAE1");
    //buffer[0]=0;
    
    sprintf(buffer,"UAE%d", current_deviceno);
    emu_printf("get new device");
    emu_printf(buffer);

    *devname_amiga = ds(*devname = my_strdup(buffer));
    return current_deviceno++;
}

/* ROM tag area memory access */
static uae_u8 rtarea[65536];
//static uae_u8 * rtarea;

static uae_u32 rtarea_lget(uaecptr) REGPARAM;
static uae_u32 rtarea_wget(uaecptr) REGPARAM;
static uae_u32 rtarea_bget(uaecptr) REGPARAM;
static void  rtarea_lput(uaecptr, uae_u32) REGPARAM;
static void  rtarea_wput(uaecptr, uae_u32) REGPARAM;
static void  rtarea_bput(uaecptr, uae_u32) REGPARAM;
static uae_u8 *rtarea_xlate(uaecptr) REGPARAM;

addrbank rtarea_bank = {
    rtarea_lget, rtarea_wget, rtarea_bget,
    rtarea_lput, rtarea_wput, rtarea_bput,
    rtarea_xlate, default_check
};

uae_u8 REGPARAM2 *rtarea_xlate(uaecptr addr) 
{
    addr &= 0xFFFF;
    return rtarea + addr;
}

uae_u32 REGPARAM2 rtarea_lget(uaecptr addr)
{
    addr &= 0xFFFF;
    return (uae_u32)(rtarea_wget(addr) << 16) + rtarea_wget(addr+2);
}

uae_u32 REGPARAM2 rtarea_wget(uaecptr addr)
{
    addr &= 0xFFFF;
    return (rtarea[addr]<<8) + rtarea[addr+1];
}

uae_u32 REGPARAM2 rtarea_bget(uaecptr addr)
{
    uae_u16 data;
    addr &= 0xFFFF;
    return rtarea[addr];
}

void REGPARAM2 rtarea_lput(uaecptr addr, uae_u32 value) { }
void REGPARAM2 rtarea_bput(uaecptr addr, uae_u32 value) { }

/* Don't start at 0 -- can get bogus writes there. */
static uae_u32 trap_base_addr = 0x00F00180;


void REGPARAM2 rtarea_wput(uaecptr addr, uae_u32 value)
{
    uae_u32 retval = 0;
    uae_u32 func = ((addr  - trap_base_addr) & 0xFFFF) >> 1;
    int has_retval = (trapmode[func] & TRAPFLAG_NO_RETVAL) == 0;
    
    /* For monitoring only? */
    if (traps[func] == NULL) {
	   m68k_setpc(trapoldfunc[func]);
	   return;
    }
    if (func < max_trap) {
	   if (trapmode[func] & TRAPFLAG_EXTRA_STACK) {
	       //execute_fn_on_extra_stack(traps[func], has_retval);
	       return;
	   }
	   retval = (*traps[func])();
    }


    if (has_retval)
	   m68k_dreg(regs, 0) = retval;
}



/* some quick & dirty code to fill in the rt area and save me a lot of
 * scratch paper
 */

static int rt_addr = 0;
static int rt_straddr = 0xFF00 - 2;

uae_u32 addr(int ptr)
{
    return (uae_u32)ptr + 0x00F00000;
}

void db(uae_u8 data)
{
    rtarea[rt_addr++] = data;
}

void dw(uae_u16 data)
{
    rtarea[rt_addr++] = data >> 8;
    rtarea[rt_addr++] = data;
}

void dl(uae_u32 data)
{
    rtarea[rt_addr++] = data >> 24;
    rtarea[rt_addr++] = data >> 16;
    rtarea[rt_addr++] = data >> 8;
    rtarea[rt_addr++] = data;
}

/* store strings starting at the end of the rt area and working
 * backward.  store pointer at current address
 */

uae_u32 ds(char *str)
{
    int len = strlen(str) + 1;

    rt_straddr -= len;
    strcpy((char *)rtarea + rt_straddr, str);
    
    return addr(rt_straddr);
}

void calltrap(uae_u32 n)
{
    dw(0x33C0);	/* MOVE.W D0,abs32 */
    dl(n*2 + trap_base_addr);
}

void org(uae_u32 a)
{
    rt_addr = a - 0x00F00000;
}

uae_u32 here(void)
{
    return addr(rt_addr);
}

int deftrap2(TrapFunction func, int mode, const char *str)
{
    int num = max_trap++;
    traps[num] = func;
    trapmode[num] = mode;
    return num;
}

int deftrap(TrapFunction func)
{
    return deftrap2(func, 0, "");
}

void align(int b)
{
    rt_addr = (rt_addr + (b-1)) & ~(b-1);
}

static uae_u32 nullfunc(void)
{
    fprintf(stderr, "Null function called\n");
    return 0;
}

static uae_u32 getchipmemsize (void)
{
    return chipmem_size;
}

void rtarea_init()
{
    uae_u32 a;
    char uaever[100];
    sprintf(uaever, "uae-%d.%d.%d", (version / 100) % 10, (version / 10) % 10, (version / 1) % 10);

    //rtarea = emu_Malloc(65536);

    EXPANSION_uaeversion = ds(uaever);
    EXPANSION_explibname = ds("expansion.library");
    EXPANSION_doslibname = ds("dos.library");
    EXPANSION_uaedevname = ds("uae.device");

    deftrap(NULL); /* Generic emulator trap */

    EXPANSION_nullfunc = here();
    calltrap(deftrap(nullfunc));
    dw(RTS);
    
    a = here();
    /* Standard "return from 68k mode" trap */
    org(0xF0FF00);
    calltrap (deftrap2(m68k_mode_return, TRAPFLAG_NO_RETVAL, ""));

    org (0xF0FF80);
    calltrap (deftrap (getchipmemsize));
    dw (RTS);
    org(a);
    
    filesys_install_code ();
}

volatile int uae_int_requested = 0;

void set_uae_int_flag (void)
{
    rtarea[0xFFFB] = uae_int_requested;
}
