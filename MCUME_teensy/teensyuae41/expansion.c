#include "shared.h"

#ifdef HAS_EXPANMEM
 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  *  Expansion Slots
  *
  * (c) 1996 Stefan Reinauer
  */


#include "memory.h"
#include "custom.h"
#include "newcpu.h"

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

/* Card Data - Where can I get a complete List of these ????  */

/* 04 - 06 & 10-16 */
#define commodore_g   513  /* Commodore Braunschweig (Germany) */
#define commodore     514  /* Commodore West Chester           */
#define gvp	     2017  /* GVP */

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

/* ********************************************************** */

static void expamem_init_clear(void);
static void expamem_init_fastcard(void);
static void expamem_map_clear(void);
static void expamem_map_fastcard(void);

void (*card_init[5])(void) = {
 expamem_init_fastcard,
 expamem_init_clear,
 expamem_init_clear,
 expamem_init_clear,
 expamem_init_clear
};

void (*card_map[5])(void) = {
 expamem_map_fastcard,
 expamem_map_clear,
 expamem_map_clear,
 expamem_map_clear,
 expamem_map_clear
};

int card = 0;

/* **********************************************************
 *  Fast Memory
 * ********************************************************** */

#include "arduinoproto.h"

static ULONG fastmem_start; /* Determined by the OS */
//static UWORD *fastmemory = NULL;
EXTMEM static UWORD fastmemory[fastmem_size/2];



static ULONG fastmem_lget(CPTR) REGPARAM;
static UWORD fastmem_wget(CPTR) REGPARAM;
static UBYTE fastmem_bget(CPTR) REGPARAM;
static void  fastmem_lput(CPTR, ULONG) REGPARAM;
static void  fastmem_wput(CPTR, UWORD) REGPARAM;
static void  fastmem_bput(CPTR, UBYTE) REGPARAM;
static int   fastmem_check(CPTR addr, ULONG size) REGPARAM;
static UWORD *fastmem_xlate(CPTR addr) REGPARAM;

addrbank fastmem_bank = {
    fastmem_lget, fastmem_wget, fastmem_bget,
    fastmem_lput, fastmem_wput, fastmem_bput,
    fastmem_xlate, fastmem_check
};

static ULONG fastmem_lget(CPTR addr)
{
    addr -= fastmem_start & (fastmem_size-1);
    addr &= fastmem_size-1;
    return ((ULONG)fastmemory[addr >> 1] << 16) | fastmemory[(addr >> 1)+1];
}

static UWORD fastmem_wget(CPTR addr)
{
    addr -= fastmem_start & (fastmem_size-1);
    addr &= fastmem_size-1;
    return fastmemory[addr >> 1];
}

static UBYTE fastmem_bget(CPTR addr)
{
    UWORD data;
    addr -= fastmem_start & (fastmem_size-1);

    addr &= fastmem_size-1;
    data = fastmemory[addr >> 1];
    return addr & 1 ? data : data >> 8;
}

static void fastmem_lput(CPTR addr, ULONG l)
{
    addr -= fastmem_start & (fastmem_size-1);
    addr &= fastmem_size-1;
    fastmemory[addr >> 1] = l >> 16;
    fastmemory[(addr >> 1)+1] = (UWORD)l;
}

static void fastmem_wput(CPTR addr, UWORD w)
{
    addr -= fastmem_start & (fastmem_size-1);
    addr &= fastmem_size-1;
    fastmemory[addr >> 1] = w;

}

static void fastmem_bput(CPTR addr, UBYTE b)
{
    addr -= fastmem_start & (fastmem_size-1);
    addr &= fastmem_size-1;
    if (!(addr & 1)) {
        fastmemory[addr>>1] = (fastmemory[addr>>1] & 0xff) | (((UWORD)b) << 8);
    } else {
        fastmemory[addr>>1] = (fastmemory[addr>>1] & 0xff00) | b;
    }

}

static int fastmem_check(CPTR addr, ULONG size)
{
    addr -= fastmem_start & (fastmem_size-1);
    addr &= fastmem_size-1;
    return (addr + size) < fastmem_size;
}

static UWORD *fastmem_xlate(CPTR addr)
{
    addr -= fastmem_start & (fastmem_size-1);
    addr &= fastmem_size-1;
    return fastmemory + (addr >> 1);
}
/* ******************************************************* */

/* Autoconfig address space at 0xE80000 */
static UWORD expamem[0x8000];

static UBYTE expamem_lo;
static UBYTE expamem_hi;

static ULONG expamem_lget(CPTR) REGPARAM;
static UWORD expamem_wget(CPTR) REGPARAM;
static UBYTE expamem_bget(CPTR) REGPARAM;
static void  expamem_lput(CPTR, ULONG) REGPARAM;
static void  expamem_wput(CPTR, UWORD) REGPARAM;
static void  expamem_bput(CPTR, UBYTE) REGPARAM;

static void  expamem_write(CPTR addr, UBYTE value)
{
    addr &= 0xffff;
    expamem[(addr >> 1)] = (value & 0xf0) << 8;
    expamem[(addr >> 1)+1] = (value & 0x0f) << 12;
}

addrbank expamem_bank = {
    expamem_lget, expamem_wget, expamem_bget,
    expamem_lput, expamem_wput, expamem_bput,
    default_xlate, default_check
};

static ULONG expamem_lget(CPTR addr)
{
    fprintf(stderr,"warning: READ.L from address $%lx \n",addr);
    return 0xffffffff;
}

static UWORD expamem_wget(CPTR addr)
{
    fprintf(stderr,"warning: READ.W from address $%lx \n",addr);
    return 0xffff;
}

static UBYTE expamem_bget(CPTR addr)
{
    UBYTE value;
    addr &= 0xFFFF;
    value = (expamem[addr >> 1] >> (addr & 1 ? 0 : 8));
    if (addr==00 || addr==02 || addr==0x40 || addr==0x42)
	return value;
    return ~value;
}

static void expamem_lput(CPTR addr, ULONG value)
{
  fprintf(stderr,"warning: WRITE.L to address $%lx : value $%lx\n",addr,value);
}

static void expamem_wput(CPTR addr, UWORD value)
{
  fprintf(stderr,"warning: WRITE.W to address $%lx : value $%x\n",addr,value);
}

static void expamem_bput(CPTR addr, UBYTE value)
{
    switch (addr&0xff) {
     case 0x30:
     case 0x32:
	expamem_hi = 0;
	expamem_lo = 0;
	expamem_write (0x48, 0x00);
	break;

     case 0x48:    
	expamem_hi = value;
        (*card_map[card])();
        fprintf (stderr,"   Card %d done.\n",card+1);
        ++card;
        if (card<=5) (*card_init[card])();
           else      expamem_init_clear();
       	break;
    
     case 0x4a:	
	expamem_lo = value;
	break;
	
     case 0x4c:
        fprintf (stderr,"   Card %d had no success.\n",card+1);
        ++card;
        if (card<=5) (*card_init[card])();
           else      expamem_init_clear();
        break;
    }
}

void expamem_reset()
{
    card = 0;
    (*card_init[card])();
}

/* ***************************************************************
 *     Expansion Card (ZORRO II) for 1/2/4/8 MB of Fast Memory
 * *************************************************************** */

void expamem_map_fastcard()
{
  //if (fastmemory == NULL)
  //    fastmemory = (UWORD *)malloc(fastmem_size);
  fastmem_start=((expamem_hi|(expamem_lo>>4))<<16);
  map_banks(fastmem_bank,fastmem_start>>16,fastmem_size>>16);
  fprintf (stderr,"Fastcard: mapped @$%lx: %dMB fast memory\n",fastmem_start, fastmem_size >>20);
}

void expamem_init_fastcard()
{
  if (fastmem_size == 0) {
     ++card;
     if (card<=5) (*card_init[card])();
        else      expamem_init_clear();
     return;
  }
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

  expamem_write (0x04, commodore_a2091_ram);
  expamem_write (0x10, commodore>>8);
  expamem_write (0x14, commodore & 0x0f);

  expamem_write (0x18, 0x00); /* ser.no. Byte 0 */
  expamem_write (0x1c, 0x00); /* ser.no. Byte 1 */
  expamem_write (0x20, 0x00); /* ser.no. Byte 2 */
  expamem_write (0x24, 0x01); /* ser.no. Byte 3 */

  expamem_write (0x28, 0x00); /* Rom-Offset hi  */
  expamem_write (0x2c, 0x00); /* ROM-Offset lo  */

  expamem_write (0x40, 0x00); /* Ctrl/Statusreg.*/
}

/* *********************************************************
 *  Dummy Entries to show that there's no card in a slot
 * ********************************************************* */

void expamem_map_clear()
{
  /* nothing to do here */
}

void expamem_init_clear()
{
  memset (expamem,0,sizeof expamem);
}

#endif
