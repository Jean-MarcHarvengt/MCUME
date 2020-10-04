 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  *  Expansion Slots
  *
  * Copyright 1996 Stefan Reinauer <stepan@matrix.kommune.schokola.de>
  */

#include "shared.h"

#include "uae.h"
#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "autoconf.h"

#define MAX_EXPANSION_BOARDS 5

/* 00 / 02 */

#define MEM_8MB    0x00        /* Size of Memory Block           */
#define MEM_4MB    0x07
#define MEM_2MB    0x06
#define MEM_1MB    0x05
#define MEM_512KB  0x04
#define MEM_256KB  0x03
#define MEM_128KB  0x02
#define MEM_64KB   0x01
 
#define same_slot  0x08     /* Next card is in the same Slot  */
#define rom_card   0x10     /* Card has valid ROM             */
#define add_memory 0x20     /* Add Memory to List of Free Ram */

#define generic    0xc0     /* Type of Expansion Card         */
#define future1    0x00
#define future2    0x40
#define future3    0x80

/* ********************************************************** */

/* Card Data */

/* 04 - 06 & 10-16 */
#define commodore_g   513  /* Commodore Braunschweig (Germany) */
#define commodore     514  /* Commodore West Chester           */
#define gvp	     2017  /* GVP */
#define hackers_id   2011

#define   commodore_a2091         3  /* A2091 / A590 Card from C=   */
#define   commodore_a2091_ram    10  /* A2091 / A590 Ram on HD-Card */
#define   commodore_a2232        70  /* A2232 Multiport Expansion   */

#define   gvp_series_2_scsi      11
#define   gvp_iv_24_gfx          32

/* ********************************************************** */
/* 08-0A */

#define no_shutup  64  /* Card cannot receive Shut_up_forever */
#define care_addr 128  /* Adress HAS to be $200000-$9fffff    */

/* ********************************************************** */

/* 40-42 */
#define enable_irq   1  /* enable Interrupt                   */ 
#define reset_card   4  /* Reset of Expansion Card            */
#define card_int2   16  /* READ ONLY: IRQ 2 occured           */
#define card_irq6   32  /* READ ONLY: IRQ 6 occured           */
#define card_irq7   64  /* READ ONLY: IRQ 7 occured           */
#define does_irq   128  /* READ ONLY: Karte loest ger. IRQ aus*/

/* ********************************************************** */

/* ROM defines */

#define rom_4bit    (0x00<<14) /* ROM width */
#define rom_8bit    (0x01<<14)
#define rom_16bit   (0x02<<14)

#define rom_never   (0x00<<12) /* Never run Boot Code       */
#define rom_install (0x01<<12) /* run code at install time  */
#define rom_binddrv (0x02<<12) /* run code with binddrivers */

uaecptr ROM_filesys_resname = 0, ROM_filesys_resid = 0;
uaecptr ROM_filesys_diagentry = 0;
uaecptr ROM_hardfile_resname = 0, ROM_hardfile_resid = 0;
uaecptr ROM_hardfile_init = 0;

/* ********************************************************** */

static void expamem_init_clear(void);
static void expamem_map_clear(void);
static void expamem_init_fastcard(void);
static void expamem_map_fastcard(void);
static void expamem_init_filesys(void);
static void expamem_map_filesys(void);

void (*card_init[MAX_EXPANSION_BOARDS])(void);
void (*card_map[MAX_EXPANSION_BOARDS])(void);

int ecard = 0;

/*
 *  Fast Memory
 */

static uae_u32 fastmem_mask;

static uae_u32 fastmem_lget(uaecptr) REGPARAM;
static uae_u32 fastmem_wget(uaecptr) REGPARAM;
static uae_u32 fastmem_bget(uaecptr) REGPARAM;
static void  fastmem_lput(uaecptr, uae_u32) REGPARAM;
static void  fastmem_wput(uaecptr, uae_u32) REGPARAM;
static void  fastmem_bput(uaecptr, uae_u32) REGPARAM;
static int   fastmem_check(uaecptr addr, uae_u32 size) REGPARAM;
static uae_u8 *fastmem_xlate(uaecptr addr) REGPARAM;

#include "arduinoproto.h"

static uae_u32 fastmem_start; /* Determined by the OS */
EXTMEM static uae_u8 fastmemory[fastmem_size];

uae_u32 REGPARAM2 fastmem_lget(uaecptr addr)
{
    uae_u8 *m;
    addr -= fastmem_start & fastmem_mask;
    addr &= fastmem_mask;
    m = fastmemory + addr;
    return do_get_mem_long ((uae_u32 *)m);
}

uae_u32 REGPARAM2 fastmem_wget(uaecptr addr)
{
    uae_u8 *m;
    addr -= fastmem_start & fastmem_mask;
    addr &= fastmem_mask;
    m = fastmemory + addr;
    return do_get_mem_word ((uae_u16 *)m);
}

uae_u32 REGPARAM2 fastmem_bget(uaecptr addr)
{
    addr -= fastmem_start & fastmem_mask;
    addr &= fastmem_mask;
    return fastmemory[addr];
}

void REGPARAM2 fastmem_lput(uaecptr addr, uae_u32 l)
{
    uae_u8 *m;
    addr -= fastmem_start & fastmem_mask;
    addr &= fastmem_mask;
    m = fastmemory + addr;
    do_put_mem_long ((uae_u32 *)m, l);
}

void REGPARAM2 fastmem_wput(uaecptr addr, uae_u32 w)
{
    uae_u8 *m;
    addr -= fastmem_start & fastmem_mask;
    addr &= fastmem_mask;
    m = fastmemory + addr;
    do_put_mem_word ((uae_u16 *)m, w);
}

void REGPARAM2 fastmem_bput(uaecptr addr, uae_u32 b)
{
    addr -= fastmem_start & fastmem_mask;
    addr &= fastmem_mask;
    fastmemory[addr] = b;
}

static int REGPARAM2 fastmem_check(uaecptr addr, uae_u32 size)
{
    addr -= fastmem_start & fastmem_mask;
    addr &= fastmem_mask;
    return (addr + size) < fastmem_size;
}

static uae_u8 REGPARAM2 *fastmem_xlate(uaecptr addr)
{
    addr -= fastmem_start & fastmem_mask;
    addr &= fastmem_mask;
    return fastmemory + addr;
}

addrbank fastmem_bank = {
    fastmem_lget, fastmem_wget, fastmem_bget,
    fastmem_lput, fastmem_wput, fastmem_bput,
    fastmem_xlate, fastmem_check
};


#ifdef HAS_FILESYS
/*
 * Filesystem device ROM
 * This is very simple, the Amiga shouldn't be doing things with it. 
 */

static uae_u32 filesys_lget(uaecptr) REGPARAM;
static uae_u32 filesys_wget(uaecptr) REGPARAM;
static uae_u32 filesys_bget(uaecptr) REGPARAM;
static void  filesys_lput(uaecptr, uae_u32) REGPARAM;
static void  filesys_wput(uaecptr, uae_u32) REGPARAM;
static void  filesys_bput(uaecptr, uae_u32) REGPARAM;

static uae_u32 filesys_start; /* Determined by the OS */

#include "arduinoproto.h"

EXTMEM uae_u8 filesysory[65536];

uae_u32 REGPARAM2 filesys_lget(uaecptr addr)
{
    uae_u8 *m;
    addr -= filesys_start & 65535;
    addr &= 65535;
    m = filesysory + addr;
    return do_get_mem_long ((uae_u32 *)m);
}

uae_u32 REGPARAM2 filesys_wget(uaecptr addr)
{
    uae_u8 *m;
    addr -= filesys_start & 65535;
    addr &= 65535;
    m = filesysory + addr;
    return do_get_mem_word ((uae_u16 *)m);
}

uae_u32 REGPARAM2 filesys_bget(uaecptr addr)
{
    addr -= filesys_start & 65535;
    addr &= 65535;
    return filesysory[addr];
}

static void REGPARAM2 filesys_lput(uaecptr addr, uae_u32 l)
{
    write_log ("filesys_lput called\n");
}

static void REGPARAM2 filesys_wput(uaecptr addr, uae_u32 w)
{
    write_log ("filesys_wput called\n");
}

static void REGPARAM2 filesys_bput(uaecptr addr, uae_u32 b)
{
    fprintf (stderr, "filesys_bput called. This usually means that you are using\n");
    fprintf (stderr, "Kickstart 1.2. Please give UAE the \"-a\" option next time\n");
    fprintf (stderr, "you start it. If you are _not_ using Kickstart 1.2, then\n");
    fprintf (stderr, "there's a bug somewhere.\n");
    fprintf (stderr, "Exiting...\n");
    //uae_quit ();
}

addrbank filesys_bank = {
    filesys_lget, filesys_wget, filesys_bget,
    filesys_lput, filesys_wput, filesys_bput,
    default_xlate, default_check
};
#endif

/* Autoconfig address space at 0xE80000 */
EXTMEM static uae_u8 expamem[65536];

static uae_u8 expamem_lo;
static uae_u8 expamem_hi;

static uae_u32 expamem_lget(uaecptr) REGPARAM;
static uae_u32 expamem_wget(uaecptr) REGPARAM;
static uae_u32 expamem_bget(uaecptr) REGPARAM;
static void  expamem_lput(uaecptr, uae_u32) REGPARAM;
static void  expamem_wput(uaecptr, uae_u32) REGPARAM;
static void  expamem_bput(uaecptr, uae_u32) REGPARAM;

addrbank expamem_bank = {
    expamem_lget, expamem_wget, expamem_bget,
    expamem_lput, expamem_wput, expamem_bput,
    default_xlate, default_check
};

static uae_u32 REGPARAM2 expamem_lget(uaecptr addr)
{
    sprintf (warning_buffer, "warning: READ.L from address $%lx \n", addr);
    write_log (warning_buffer);
    return 0xfffffffful;
}

static uae_u32 REGPARAM2 expamem_wget(uaecptr addr)
{
    sprintf (warning_buffer, "warning: READ.W from address $%lx \n", addr);
    write_log (warning_buffer);
    return 0xffff;
}

static uae_u32 REGPARAM2 expamem_bget(uaecptr addr)
{
    uae_u8 value;
    addr &= 0xFFFF;
    return expamem[addr];
}

static void  REGPARAM2 expamem_write(uaecptr addr, uae_u32 value)
{
    addr &= 0xffff;
    if (addr==00 || addr==02 || addr==0x40 || addr==0x42) {
	expamem[addr] = (value & 0xf0);
	expamem[addr+2] = (value & 0x0f) << 4;
    } else {
	expamem[addr] = ~(value & 0xf0);
	expamem[addr+2] = ~((value & 0x0f) << 4);
    }
}

static void REGPARAM2 expamem_lput(uaecptr addr, uae_u32 value)
{
    fprintf(stderr,"warning: WRITE.L to address $%lx : value $%lx\n",addr,value);
}

static void REGPARAM2 expamem_wput(uaecptr addr, uae_u32 value)
{
    fprintf(stderr,"warning: WRITE.W to address $%lx : value $%x\n",addr,value);
}

static void REGPARAM2 expamem_bput(uaecptr addr, uae_u32 value)
{
    static char buffer[80];
    switch (addr&0xff) {
     case 0x30:
     case 0x32:
	expamem_hi = 0;
	expamem_lo = 0;
	expamem_write (0x48, 0x00);
	break;

     case 0x48:
	expamem_hi = value;
        (*card_map[ecard])();
        sprintf (buffer, "   Card %d done.\n",ecard+1);
	write_log (buffer);
        ++ecard;
        if (ecard <= MAX_EXPANSION_BOARDS)
	    (*card_init[ecard])();
	else
	    expamem_init_clear();
       	break;
    
     case 0x4a:	
	expamem_lo = value;
	break;
	
     case 0x4c:
        sprintf (buffer,"   Card %d had no success.\n",ecard+1);
	write_log (buffer);
        ++ecard;
        if (ecard <= MAX_EXPANSION_BOARDS) 
	    (*card_init[ecard])();
	else  
	    expamem_init_clear();
        break;
    }
}

void expamem_reset()
{
    int cardno = 0;

    ecard = 0;
    
    if (fastmemory != NULL) {
	card_init[cardno] = expamem_init_fastcard;
	card_map[cardno++] = expamem_map_fastcard;
    }


#ifdef HAS_FILESYS
    if (currprefs.automount_uaedev) {
 #ifdef HAS_ERSATZ         
       if (!ersatzkickfile)
 #endif       
       { 
	       card_init[cardno] = expamem_init_filesys;
	       card_map[cardno++] = expamem_map_filesys;
       }
    }
#endif    

    while (cardno < MAX_EXPANSION_BOARDS) {
	card_init[cardno] = expamem_init_clear;
	card_map[cardno++] = expamem_map_clear;
    }
    
    (*card_init[0])();
}




void expansion_init(void)
{
    if (fastmem_size > 0) {
	   fastmem_mask = fastmem_size - 1;
	   //fastmemory = (uae_u8 *)calloc(fastmem_size,1);
	   //if (fastmemory == NULL) {
	   //    write_log ("Out of memory for fastmem card.\n");
	   //}
    }
}

/*
 *     Expansion Card (ZORRO II) for 1/2/4/8 MB of Fast Memory
 */

void expamem_map_fastcard()
{
    char buffer[80];
    fastmem_start = ((expamem_hi|(expamem_lo>>4)) << 16);
    map_banks (&fastmem_bank, fastmem_start >> 16, fastmem_size >> 16);
    sprintf (buffer, "Fastcard: mapped @$%lx: %dMB fast memory\n",fastmem_start, fastmem_size >>20);
    write_log (buffer);
}

void expamem_init_fastcard()
{
    expamem_init_clear();
    if (fastmem_size==0x100000)
	expamem_write (0x00, MEM_1MB+add_memory+generic);
    else if (fastmem_size==0x200000)
	expamem_write (0x00, MEM_2MB+add_memory+generic);
    else if (fastmem_size==0x400000)
	expamem_write (0x00, MEM_4MB+add_memory+generic);
    else if (fastmem_size==0x800000)
	expamem_write (0x00, MEM_8MB+add_memory+generic);

    expamem_write (0x08, care_addr);

    expamem_write (0x04, 1);
    expamem_write (0x10, hackers_id >> 8);
    expamem_write (0x14, hackers_id & 0x0f);

    expamem_write (0x18, 0x00); /* ser.no. Byte 0 */
    expamem_write (0x1c, 0x00); /* ser.no. Byte 1 */
    expamem_write (0x20, 0x00); /* ser.no. Byte 2 */
    expamem_write (0x24, 0x01); /* ser.no. Byte 3 */

    expamem_write (0x28, 0x00); /* Rom-Offset hi  */
    expamem_write (0x2c, 0x00); /* ROM-Offset lo  */

    expamem_write (0x40, 0x00); /* Ctrl/Statusreg.*/
}


#ifdef HAS_FILESYS
/* 
 * Filesystem device
 */

void expamem_map_filesys()
{
    uaecptr a;
    
    filesys_start = ((expamem_hi | (expamem_lo >> 4)) << 16);
    map_banks(&filesys_bank, filesys_start >> 16, 1);
    /* 68k code needs to know this. */
    a = here ();
    org (0xF0FFFC);
    dl (filesys_start + 0x2000);
    org (a);
}

void expamem_init_filesys()
{
    uae_u8 diagarea[] = { 0x90, 0x00, 0x01, 0x0C, 0x01, 0x00, 0x01, 0x06 };

    expamem_init_clear();
    expamem_write (0x00, MEM_64KB | rom_card | generic);

    expamem_write (0x08, care_addr | no_shutup);

    expamem_write (0x04, 2);
    expamem_write (0x10, hackers_id >> 8);
    expamem_write (0x14, hackers_id & 0x0f);

    expamem_write (0x18, 0x00); /* ser.no. Byte 0 */
    expamem_write (0x1c, 0x00); /* ser.no. Byte 1 */
    expamem_write (0x20, 0x00); /* ser.no. Byte 2 */
    expamem_write (0x24, 0x01); /* ser.no. Byte 3 */

    expamem_write (0x28, 0x10); /* Rom-Offset hi  */
    expamem_write (0x2c, 0x00); /* ROM-Offset lo  */

    expamem_write (0x40, 0x00); /* Ctrl/Statusreg.*/

    /* Build a DiagArea */
    memcpy(expamem + 0x1000, diagarea, sizeof diagarea);
    
    /* Call DiagEntry */
    do_put_mem_word ((uae_u16 *)(expamem + 0x1100), 0x4EF9); /* JMP */
    do_put_mem_long ((uae_u32 *)(expamem + 0x1102), ROM_filesys_diagentry);

    /* What comes next is a plain bootblock */
    do_put_mem_word ((uae_u16 *)(expamem + 0x1106), 0x4EF9); /* JMP */
    do_put_mem_long ((uae_u32 *)(expamem + 0x1108), EXPANSION_bootcode);
    
    /* Build a DOS param packet template */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 12), 0); /* Device flags */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 16), 16); /* Env. size */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 20), 128); /* 512 bytes/block */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 24), 0); /* unused */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 28), 1); /* heads */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 32), 1); /* unused */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 36), 32); /* secs per track */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 40), 1); /* reserved blocks */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 44), 0); /* unused */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 48), 0); /* interleave */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 52), 0); /* lowCyl */
    {
    extern int numtracks;
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 56), numtracks-1); /* upperCyl */
    }
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 60), 0); /* Number of buffers */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 64), 0); /* Buffer mem type */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 68), 0x7FFFFFFF); /* largest transfer */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 72), ~1); /* addMask (?) */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 76), (uae_u32)-1); /* bootPri */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 80), 0x444f5300); /* DOS\0 */
    do_put_mem_long ((uae_u32 *)(expamem + 0x2000 + 84), 0); /* pad */
    
    memcpy(filesysory, expamem, 0x3000);
}
#endif

/*
 *  Dummy Entries to show that there's no card in a slot
 */

void expamem_map_clear()
{
    fprintf(stderr, "expamem_map_clear() got called. Shouldn't happen.\n");
}

void expamem_init_clear()
{
    memset (expamem,0xff,sizeof expamem);
}
