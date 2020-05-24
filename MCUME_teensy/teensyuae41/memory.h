 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * memory management
  * 
  * (c) 1995 Bernd Schmidt
  */

#define chipmem_size 0x200000 
//#define bogomem_size 0x100000 /* C00000 crap mem */
#define kickmem_size 0x080000
#define fastmem_size 0x400000

#define chipmem_start 0x000000
//#define bogomem_start 0xC00000 
#define kickmem_start 0xF80000

extern UWORD *chipmemory2;

extern int ersatzkickfile;

typedef ULONG (*lget_func)(CPTR) REGPARAM;
typedef UWORD (*wget_func)(CPTR) REGPARAM;
typedef UBYTE (*bget_func)(CPTR) REGPARAM;
typedef void (*lput_func)(CPTR,ULONG) REGPARAM;
typedef void (*wput_func)(CPTR,UWORD) REGPARAM;
typedef void (*bput_func)(CPTR,UBYTE) REGPARAM;
typedef UWORD *(*xlate_func)(CPTR) REGPARAM;
typedef int (*check_func)(CPTR, ULONG) REGPARAM;

typedef struct {
    lget_func lget;
    wget_func wget;
    bget_func bget;
    lput_func lput;
    wput_func wput;
    bput_func bput;
    xlate_func xlateaddr;
    check_func check;
} addrbank;

extern addrbank chipmem_bank;
extern addrbank kickmem_bank;
extern addrbank custom_bank;
extern addrbank clock_bank;
extern addrbank cia_bank;
extern addrbank rtarea_bank;
extern addrbank expamem_bank;
extern addrbank fastmem_bank;

extern void rtarea_init (void);
extern void expamem_init (void);
extern void expamem_reset (void);


/* Default memory access functions */

extern int default_check(CPTR addr, ULONG size) REGPARAM;
extern UWORD *default_xlate(CPTR addr) REGPARAM;

extern addrbank membanks[256];
static __inline__ unsigned int bankindex(CPTR a)
{
    if ( (a>>16) > 0xFF) return 0xE0;
    return a>>16;
}

static __inline__ ULONG longget(CPTR addr)
{
    return membanks[bankindex(addr)].lget(addr);
}
static __inline__ UWORD wordget(CPTR addr)
{
    return membanks[bankindex(addr)].wget(addr);
}
static __inline__ UBYTE byteget(CPTR addr) 
{
    return membanks[bankindex(addr)].bget(addr);
}
static __inline__ void longput(CPTR addr, ULONG l)
{
    membanks[bankindex(addr)].lput(addr, l);
}
static __inline__ void wordput(CPTR addr, UWORD w)
{
    membanks[bankindex(addr)].wput(addr, w);
}
static __inline__ void byteput(CPTR addr, UBYTE b)
{
    membanks[bankindex(addr)].bput(addr, b);
}

static __inline__ int check_addr(CPTR a)
{
    return 1;
#ifdef NO_EXCEPTION_3
    return 1;
#else
    return (a & 1) == 0;
#endif
}
extern int buserr;
    
extern void memory_init(void);    
extern void map_banks(addrbank bank, int first, int count);
    
static __inline__ ULONG get_long(CPTR addr) 
{
    if (check_addr(addr))
	    return longget(addr);
    buserr = 1;
    return 0;
}
static __inline__ UWORD get_word(CPTR addr) 
{
    if (check_addr(addr))
	    return wordget(addr);
    buserr = 1;
    return 0;
}
static __inline__ UBYTE get_byte(CPTR addr) 
{
    return byteget(addr); 
}
static __inline__ void put_long(CPTR addr, ULONG l) 
{
    if (!check_addr(addr))
	    buserr = 1;
    longput(addr, l);
}
static __inline__ void put_word(CPTR addr, UWORD w) 
{
    if (!check_addr(addr))
	    buserr = 1;
    wordput(addr, w);
}
static __inline__ void put_byte(CPTR addr, UBYTE b) 
{
    byteput(addr, b);
}

static __inline__ UWORD *get_real_address(CPTR addr)
{
    if (!check_addr(addr))
	buserr = 1;
    //emu_printf("ra");
    //emu_printi(addr);
    return membanks[bankindex(addr)].xlateaddr(addr);
}

static __inline__ int valid_address(CPTR addr, ULONG size)
{
    if (!check_addr(addr))
	buserr = 1;
    return membanks[bankindex(addr)].check(addr, size);
}
