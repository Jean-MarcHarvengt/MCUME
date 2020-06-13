 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * memory management
  * 
  * Copyright 1995 Bernd Schmidt
  */

/* Enabling this adds one additional native memory reference per 68k memory
 * access, but saves one shift (on the x86). Enabling this is probably
 * better for the cache. My favourite benchmark (PP2) doesn't show a
 * difference, so I leave this enabled. */

#define fastmem_size 0x400000
#define chipmem_size 0x200000 //0x080000
#define bogomem_size 0x000000
#define kickmem_size 0x080000

#define chipmem_start 0x000000
#define bogomem_start 0xC00000 
#define kickmem_start 0xF80000


typedef uae_u32 (*mem_get_func)(uaecptr) REGPARAM;
typedef void (*mem_put_func)(uaecptr, uae_u32) REGPARAM;
typedef uae_u8 *(*xlate_func)(uaecptr) REGPARAM;
typedef int (*check_func)(uaecptr, uae_u32) REGPARAM;

extern char *address_space, *good_address_map;

#undef DIRECT_MEMFUNCS_SUCCESSFUL
#include "machdep/memory.h"

#ifndef CAN_MAP_MEMORY
#undef USE_COMPILER
#endif

#if defined(USE_COMPILER) && !defined(USE_MAPPED_MEMORY)
#define USE_MAPPED_MEMORY
#endif


extern int ersatzkickfile;

typedef struct {
    /* These ones should be self-explanatory... */
    mem_get_func lget, wget, bget;
    mem_put_func lput, wput, bput;
    /* Use xlateaddr to translate an Amiga address to a uae_u8 * that can
     * be used to address memory without calling the wget/wput functions. 
     * This doesn't work for all memory banks, so this function may call
     * abort(). */
    xlate_func xlateaddr;
    /* To prevent calls to abort(), use check before calling xlateaddr.
     * It checks not only that the memory bank can do xlateaddr, but also
     * that the pointer points to an area of at least the specified size. 
     * This is used for example to translate bitplane pointers in custom.c */
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
extern void rtarea_setup (void);
extern void expamem_init (void);
extern void expamem_reset (void);

#define MEM_BANK_SIZE 256 //65536

extern int address_space_24;

/* Default memory access functions */

extern int default_check(uaecptr addr, uae_u32 size) REGPARAM;
extern uae_u8 *default_xlate(uaecptr addr) REGPARAM;

//#define bankindex(addr) (((uaecptr)(addr)) >> 16)
static __inline__ unsigned int bankindex(CPTR a)
{
    if ( (a>>16) > 0xFF) return 0xE0;
    return (a>>16);
}


extern addrbank *mem_banks[MEM_BANK_SIZE];
#define get_mem_bank(addr) (*mem_banks[bankindex(addr)])
#define put_mem_bank(addr, b) (mem_banks[bankindex(addr)] = (b))


#if defined(NO_EXCEPTION_3) || CPU_LEVEL > 1
#define check_addr(a) 1
#else
#define check_addr(a) (((a) & 1) == 0)
#endif

extern int buserr;
extern uae_u16 last_op_for_exception_3;
extern uaecptr last_addr_for_exception_3;
extern uaecptr last_fault_for_exception_3;
extern uaecptr m68k_currentpc(void);

extern void memory_init(void);    
extern void map_banks(addrbank *bank, int first, int count);

#ifndef NO_INLINE_MEMORY_ACCESS

#define longget(addr) (call_mem_get_func(get_mem_bank(addr).lget, addr))
#define wordget(addr) (call_mem_get_func(get_mem_bank(addr).wget, addr))
#define byteget(addr) (call_mem_get_func(get_mem_bank(addr).bget, addr))
#define longput(addr,l) (call_mem_put_func(get_mem_bank(addr).lput, addr, l))
#define wordput(addr,w) (call_mem_put_func(get_mem_bank(addr).wput, addr, w))
#define byteput(addr,b) (call_mem_put_func(get_mem_bank(addr).bput, addr, b))

#else

extern uae_u32 alongget(uaecptr addr);
extern uae_u32 awordget(uaecptr addr);
extern uae_u32 longget(uaecptr addr);
extern uae_u32 wordget(uaecptr addr);
extern uae_u32 byteget(uaecptr addr);
extern void longput(uaecptr addr, uae_u32 l);
extern void wordput(uaecptr addr, uae_u32 w);
extern void byteput(uaecptr addr, uae_u32 b);

#endif

#ifndef MD_HAVE_MEM_1_FUNCS

#define longget_1 longget
#define wordget_1 wordget
#define byteget_1 byteget
#define longput_1 longput
#define wordput_1 wordput
#define byteput_1 byteput

#endif

static __inline__ uae_u32 get_long(uaecptr addr) 
{
    if (check_addr(addr))
	return longget_1(addr);
    buserr = 1;
    last_fault_for_exception_3 = addr;
    last_addr_for_exception_3 = m68k_currentpc();
    return 0;
}
static __inline__ uae_u32 get_word(uaecptr addr) 
{
    if (check_addr(addr))
	return wordget_1(addr);
    buserr = 1;
    last_fault_for_exception_3 = addr;
    last_addr_for_exception_3 = m68k_currentpc();
    return 0;
}
static __inline__ uae_u32 get_byte(uaecptr addr) 
{
    return byteget_1(addr); 
}
static __inline__ void put_long(uaecptr addr, uae_u32 l) 
{
    if (!check_addr(addr)) {
	buserr = 1;
	last_fault_for_exception_3 = addr;
        last_addr_for_exception_3 = m68k_currentpc();
	return;
    }
    longput_1(addr, l);
}
static __inline__ void put_word(uaecptr addr, uae_u32 w)
{
    if (!check_addr(addr)) {
	buserr = 1;
	last_fault_for_exception_3 = addr;
	last_addr_for_exception_3 = m68k_currentpc();
	return;
    }
    wordput_1(addr, w);
}
static __inline__ void put_byte(uaecptr addr, uae_u32 b) 
{
    byteput_1(addr, b);
}

static __inline__ uae_u8 *get_real_address(uaecptr addr)
{
    return get_mem_bank(addr).xlateaddr(addr);
}

static __inline__ int valid_address(uaecptr addr, uae_u32 size)
{
    return get_mem_bank(addr).check(addr, size);
}
