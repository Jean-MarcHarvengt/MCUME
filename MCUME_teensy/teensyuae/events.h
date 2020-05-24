 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Events
  * 
  * (c) 1995 Bernd Schmidt
  */

/* This is tunable. 4 gives good results. */
#ifdef LINUX_SOUND_SLOW_MACHINE
#define cycles_per_instruction 8
#else
#define cycles_per_instruction 4
#endif

#undef DEFFERED_INT
#if !defined(NO_FAST_BLITTER)
#define DEFERRED_INT
#endif

extern unsigned long int cycles, nextevent, nextev_count;
typedef void (*evfunc)(void);

struct ev
{
    int active;
    unsigned long int evtime, oldcycles;
    evfunc handler;
};

enum { 
    ev_hsync, ev_copper, ev_cia,
#ifdef DEFERRED_INT
    ev_deferint,
#endif
    ev_max
};

extern struct ev eventtab[ev_max];

static __inline__ void events_schedule(void)
{
    int i;
    
    unsigned long int mintime = ~0L;
    nextevent = cycles + mintime;
    for(i = 0; i < ev_max; i++) {
	unsigned long int eventtime = eventtab[i].evtime + eventtab[i].oldcycles - cycles;
	if (eventtab[i].active && eventtime < mintime) {	    
	    mintime = eventtime;
	    nextevent = cycles + mintime;
	}
    }
    nextev_count = nextevent - cycles;
}

static __inline__ void do_cycles(void)
{
    if (nextev_count <= cycles_per_instruction) {
	int j;
	for(j = 0; j < cycles_per_instruction; j++) {
	    if (++cycles == nextevent) {
		unsigned long int old_nextevent = nextevent;
		int i;
		
		for(i = 0; i < ev_max; i++) {
		    if (eventtab[i].active && (eventtab[i].evtime + eventtab[i].oldcycles) == old_nextevent) {
			(*eventtab[i].handler)();
		    }
		}
		events_schedule();
	    } else nextev_count--;
	}
    } else {
    	nextev_count -= cycles_per_instruction;
	cycles += cycles_per_instruction;
    }
}
