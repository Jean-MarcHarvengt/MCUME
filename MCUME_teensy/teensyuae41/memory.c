 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Memory management
  *
  * (c) 1995 Bernd Schmidt
  */

#include "shared.h"

#include "uae.h"
#include "osdep/memory.h"
#include "memory.h"
#ifdef HAS_ERSATZ  
#include "ersatz.h"
#endif
#include "zfile.h"

//#define HALF_KICKIMAGE 1
//#define HAS_KICKFILE 1
//#define HALF_CHIPMEM   1

#ifdef HAS_ERSATZ  
int ersatzkickfile = 0;
#endif
int buserr;

addrbank *mem_banks[MEM_BANK_SIZE];


#ifdef NO_INLINE_MEMORY_ACCESS
__inline__ uae_u32 longget(uaecptr addr)
{
    return call_mem_get_func(get_mem_bank(addr).lget, addr);
}
__inline__ uae_u32 wordget(uaecptr addr)
{
    return call_mem_get_func(get_mem_bank(addr).wget, addr);
}
__inline__ uae_u32 byteget(uaecptr addr) 
{
    return call_mem_get_func(get_mem_bank(addr).bget, addr);
}
__inline__ void longput(uaecptr addr, uae_u32 l)
{
    call_mem_put_func(get_mem_bank(addr).lput, addr, l);
}
__inline__ void wordput(uaecptr addr, uae_u32 w)
{
    call_mem_put_func(get_mem_bank(addr).wput, addr, w);
}
__inline__ void byteput(uaecptr addr, uae_u32 b)
{
    call_mem_put_func(get_mem_bank(addr).bput, addr, b);
}
#endif

/* A dummy bank that only contains zeros */

static uae_u32 dummy_lget(uaecptr) REGPARAM;
static uae_u32 dummy_wget(uaecptr) REGPARAM;
static uae_u32 dummy_bget(uaecptr) REGPARAM;
static void  dummy_lput(uaecptr, uae_u32) REGPARAM;
static void  dummy_wput(uaecptr, uae_u32) REGPARAM;
static void  dummy_bput(uaecptr, uae_u32) REGPARAM;
static int   dummy_check(uaecptr addr, uae_u32 size) REGPARAM;

uae_u32 REGPARAM2 dummy_lget(uaecptr addr)
{
    if (currprefs.illegal_mem) {
	sprintf (warning_buffer, "Illegal lget at %08lx\n", addr);
	write_log (warning_buffer);
    }
    return 0;
}

uae_u32 REGPARAM2 dummy_wget(uaecptr addr)
{
    if (currprefs.illegal_mem) {
	sprintf (warning_buffer, "Illegal wget at %08lx\n", addr);
	write_log (warning_buffer);
    }
    return 0;
}

uae_u32 REGPARAM2 dummy_bget(uaecptr addr)
{
    if (currprefs.illegal_mem) {
	sprintf (warning_buffer, "Illegal bget at %08lx\n", addr);
	write_log (warning_buffer);
    }
    return 0;
}

void REGPARAM2 dummy_lput(uaecptr addr, uae_u32 l)
{
    if (currprefs.illegal_mem) {
	sprintf (warning_buffer, "Illegal lput at %08lx\n", addr);
	write_log (warning_buffer);
    }
}
void REGPARAM2 dummy_wput(uaecptr addr, uae_u32 w)
{
    if (currprefs.illegal_mem) {
	sprintf (warning_buffer, "Illegal wput at %08lx\n", addr);
	write_log (warning_buffer);
    }
}
void REGPARAM2 dummy_bput(uaecptr addr, uae_u32 b)
{
    if (currprefs.illegal_mem) {
	sprintf (warning_buffer, "Illegal bput at %08lx\n", addr);
	write_log (warning_buffer);
    }
}

int REGPARAM2 dummy_check(uaecptr addr, uae_u32 size)
{
    if (currprefs.illegal_mem) {
	sprintf (warning_buffer, "Illegal check at %08lx\n", addr);
	write_log (warning_buffer);
    }
    return 0;
}

/* Chip memory */

uae_u32 chipmem_mask,kickmem_mask,bogomem_mask;


#include "arduinoproto.h"

#ifdef HALF_CHIPMEM
static uae_u8 *chipmemory=NULL;
static uae_u8 chipmemextra[chipmem_size/2];
uae_u8 *chipmemory2=(UWORD*)&chipmemextra[0];
#else
EXTMEM static uae_u8 chipmemory[chipmem_size];
uae_u8 *chipmemory2=NULL;
#endif


static uae_u32 chipmem_lget(uaecptr) REGPARAM;
static uae_u32 chipmem_wget(uaecptr) REGPARAM;
static uae_u32 chipmem_bget(uaecptr) REGPARAM;
static void  chipmem_lput(uaecptr, uae_u32) REGPARAM;
static void  chipmem_wput(uaecptr, uae_u32) REGPARAM;
static void  chipmem_bput(uaecptr, uae_u32) REGPARAM;

static int   chipmem_check(uaecptr addr, uae_u32 size) REGPARAM;
static uae_u8 *chipmem_xlate(uaecptr addr) REGPARAM;

uae_u32 REGPARAM2 chipmem_lget(uaecptr addr)
{
    uae_u32 *m;
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
#ifdef HALF_CHIPMEM
    if (addr >= 0x040000) {
        addr -= 0x040000;
        m = (uae_u32 *)(chipmemory2 + addr);
        return do_get_mem_long(m);
    }
#endif      
    m = (uae_u32 *)(chipmemory + addr);
    return do_get_mem_long(m);
}

uae_u32 REGPARAM2 chipmem_wget(uaecptr addr)
{
    uae_u16 *m;
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
#ifdef HALF_CHIPMEM
    if (addr >= 0x040000) {
        addr -= 0x040000;
        m = (uae_u16 *)(chipmemory2 + addr);
        return do_get_mem_word(m);
    }
#endif      
    m = (uae_u16 *)(chipmemory + addr);
    return do_get_mem_word(m);
}

uae_u32 REGPARAM2 chipmem_bget(uaecptr addr)
{
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
#ifdef HALF_CHIPMEM
    if (addr >= 0x040000) {
        addr -= 0x040000;
        return chipmemory2[addr];
    }
#endif      
    return chipmemory[addr];
}

void REGPARAM2 chipmem_lput(uaecptr addr, uae_u32 l)
{
    uae_u32 *m;
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
#ifdef HALF_CHIPMEM
    if (addr >= 0x040000) {
        addr -= 0x040000;
        m = (uae_u32 *)(chipmemory2 + addr);
        do_put_mem_long(m, l);
        return;
    }
#endif        
    m = (uae_u32 *)(chipmemory + addr);
    do_put_mem_long(m, l);
}

void REGPARAM2 chipmem_wput(uaecptr addr, uae_u32 w)
{
    uae_u16 *m;
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
#ifdef HALF_CHIPMEM
    if (addr >= 0x040000) {
        addr -= 0x040000;
        m = (uae_u16 *)(chipmemory2 + addr);
        do_put_mem_word(m, w);
        return;
    }
#endif      
    m = (uae_u16 *)(chipmemory + addr);
    do_put_mem_word(m, w);
}

void REGPARAM2 chipmem_bput(uaecptr addr, uae_u32 b)
{
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
#ifdef HALF_CHIPMEM
    if (addr >= 0x040000) {
        addr -= 0x040000;
        chipmemory2[addr] = b;
        return;
    }
#endif  
    chipmemory[addr] = b;
}

int REGPARAM2 chipmem_check(uaecptr addr, uae_u32 size)
{
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    return (addr + size) < chipmem_size;
}

uae_u8 REGPARAM2 *chipmem_xlate(uaecptr addr)
{
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
#ifdef HALF_CHIPMEM
    if (addr >= 0x040000) {
        addr -= 0x040000;
        return chipmemory2 + addr;
    }
#endif    
    return chipmemory + addr;
}

#ifdef HAS_BOGOMEM 

/* Slow memory */

static uae_u8 *bogomemory;

static uae_u32 bogomem_lget(uaecptr) REGPARAM;
static uae_u32 bogomem_wget(uaecptr) REGPARAM;
static uae_u32 bogomem_bget(uaecptr) REGPARAM;
static void  bogomem_lput(uaecptr, uae_u32) REGPARAM;
static void  bogomem_wput(uaecptr, uae_u32) REGPARAM;
static void  bogomem_bput(uaecptr, uae_u32) REGPARAM;
static int  bogomem_check(uaecptr addr, uae_u32 size) REGPARAM;
static uae_u8 *bogomem_xlate(uaecptr addr) REGPARAM;

uae_u32 REGPARAM2 bogomem_lget(uaecptr addr)
{
    uae_u32 *m;
    addr -= bogomem_start & bogomem_mask;
    addr &= bogomem_mask;
    m = (uae_u32 *)(bogomemory + addr);
    return do_get_mem_long(m);
}

uae_u32 REGPARAM2 bogomem_wget(uaecptr addr)
{
    uae_u16 *m;
    addr -= bogomem_start & bogomem_mask;
    addr &= bogomem_mask;
    m = (uae_u16 *)(bogomemory + addr);
    return do_get_mem_word(m);
}

uae_u32 REGPARAM2 bogomem_bget(uaecptr addr)
{
    addr -= bogomem_start & bogomem_mask;
    addr &= bogomem_mask;
    return bogomemory[addr];
}

void REGPARAM2 bogomem_lput(uaecptr addr, uae_u32 l)
{
    uae_u32 *m;
    addr -= bogomem_start & bogomem_mask;
    addr &= bogomem_mask;
    m = (uae_u32 *)(bogomemory + addr);
    do_put_mem_long(m, l);
}

void REGPARAM2 bogomem_wput(uaecptr addr, uae_u32 w)
{
    uae_u16 *m;
    addr -= bogomem_start & bogomem_mask;
    addr &= bogomem_mask;
    m = (uae_u16 *)(bogomemory + addr);
    do_put_mem_word(m, w);
}

void REGPARAM2 bogomem_bput(uaecptr addr, uae_u32 b)
{
    addr -= bogomem_start & bogomem_mask;
    addr &= bogomem_mask;
    bogomemory[addr] = b;
}

int REGPARAM2 bogomem_check(uaecptr addr, uae_u32 size)
{
    addr -= bogomem_start & bogomem_mask;
    addr &= bogomem_mask;
    return (addr + size) < bogomem_size;
}

uae_u8 REGPARAM2 *bogomem_xlate(uaecptr addr)
{
    addr -= bogomem_start & bogomem_mask;
    addr &= bogomem_mask;
    return bogomemory + addr;
}
#endif

/* Kick memory */

static int zkickfile = 0;
#ifdef HAS_KICKFILE   
static uae_u8 kick[kickmem_size];
#else
#ifdef HALF_KICKIMAGE
#include "kick13.h"
#else
#include "kickfull.h"
#endif
#endif
//static UWORD *kickmemory=(UWORD*)&kick[0];
uae_u8 *kickmemory=(uae_u8*)&kick[0];;

static uae_u32 kickmem_lget(uaecptr) REGPARAM;
static uae_u32 kickmem_wget(uaecptr) REGPARAM;
static uae_u32 kickmem_bget(uaecptr) REGPARAM;
static void  kickmem_lput(uaecptr, uae_u32) REGPARAM;
static void  kickmem_wput(uaecptr, uae_u32) REGPARAM;
static void  kickmem_bput(uaecptr, uae_u32) REGPARAM;
static int  kickmem_check(uaecptr addr, uae_u32 size) REGPARAM;
static uae_u8 *kickmem_xlate(uaecptr addr) REGPARAM;

uae_u32 REGPARAM2 kickmem_lget(uaecptr addr)
{
    uae_u32 *m;
    addr -= kickmem_start & kickmem_mask;
    addr &= kickmem_mask;
#ifdef HALF_KICKIMAGE
    if (addr >= 0x040000) {addr -= 0x040000;}
#endif       
    m = (uae_u32 *)(kickmemory + addr);
    return do_get_mem_long(m); 
}

uae_u32 REGPARAM2 kickmem_wget(uaecptr addr)
{
    uae_u16 *m;
    addr -= kickmem_start & kickmem_mask;
    addr &= kickmem_mask;
#ifdef HALF_KICKIMAGE
    if (addr >= 0x040000) {addr -= 0x040000;}
#endif       
    m = (uae_u16 *)(kickmemory + addr);
    return do_get_mem_word(m); 
}

uae_u32 REGPARAM2 kickmem_bget(uaecptr addr)
{
    addr -= kickmem_start & kickmem_mask;
    addr &= kickmem_mask;
#ifdef HALF_KICKIMAGE
    if (addr >= 0x040000) {addr -= 0x040000;}
#endif       
    return kickmemory[addr];
}

void REGPARAM2 kickmem_lput(uaecptr addr, uae_u32 b)
{
    if (currprefs.illegal_mem) sprintf (warning_buffer, "Illegal kickmem lput at %08lx\n", addr);
}

void REGPARAM2 kickmem_wput(uaecptr addr, uae_u32 b)
{
    if (currprefs.illegal_mem) sprintf (warning_buffer, "Illegal kickmem wput at %08lx\n", addr);
}

void REGPARAM2 kickmem_bput(uaecptr addr, uae_u32 b)
{
    if (currprefs.illegal_mem) sprintf (warning_buffer, "Illegal kickmem lput at %08lx\n", addr);
}

int REGPARAM2 kickmem_check(uaecptr addr, uae_u32 size)
{
    addr -= kickmem_start & kickmem_mask;
    addr &= kickmem_mask;
    return (addr + size) < kickmem_size;
}

uae_u8 REGPARAM2 *kickmem_xlate(uaecptr addr)
{
    addr -= kickmem_start & kickmem_mask;
    addr &= kickmem_mask;
    return kickmemory + addr;
}

/* Default memory access functions */

int REGPARAM2 default_check(uaecptr a, uae_u32 b)
{
    return 0;
}

uae_u8 REGPARAM2 *default_xlate(uaecptr a)
{
    write_log ("Your Amiga program just did something terribly stupid\n");
    uae_reset();
    return chipmem_xlate(0); /* So we don't crash. */
}


#ifdef HAS_KICKFILE
int processFileInMem(char * outfile, char * arrname, unsigned char * data, int size) {

  FILE *fp_wr = stdout;
  if ((fp_wr = fopen (outfile, "wb")) == NULL)
  {
    fprintf (stderr, "Error:  can not create file %s\n", outfile);    
  }

  int cnt=0;
  fprintf(fp_wr, "const unsigned char %s[%d] = {\n", arrname, size);

  cnt = 0;
  for (int i = 0; i < size; i++) {
    cnt++;
    if (cnt == 16) {
      fprintf(fp_wr, "0x%02X,\n",data[i]);
    }  
    else {
      fprintf(fp_wr, "0x%02X,",data[i]);
    }  
    cnt &= 15;
  }  
  fprintf(fp_wr, "};\n");

  fclose (fp_wr);
  return 1;  
}

static int load_kickstart(void)
{
    int i;
    uae_u32 cksum = 0, prevck = 0;
    unsigned char buffer[8];
    
    FILE *f = zfile_open(romfile, "rb");
    
    if (f == NULL) {	
        write_log ("No Kickstart ROM found.\n");
        return 0;
    }
    
    fread(buffer, 1, 8, f);
    if (buffer[4] == 0 && buffer[5] == 8 && buffer[6] == 0 && buffer[7] == 0) {
	   fprintf (stderr, "You seem to have a ZKick file there... You probably lose.\n");
	   zkickfile = 1;
    } else 
	   fseek(f, 0, SEEK_SET);
    
    i = fread(kickmemory, 1, kickmem_size, f);
    if (i == kickmem_size/2) {
/*	fprintf (stderr, "Warning: Kickstart is only 256K.\n"); */
	   memcpy (kickmemory + kickmem_size/2, kickmemory, kickmem_size/2);
    } 
    else if (i != kickmem_size) {
	   write_log ("Error while reading Kickstart.\n");
	   return 0;
    }
    zfile_close (f);
    

    for (i = 0; i < kickmem_size; i+=4) {
	   uae_u32 data = kickmemory[i]*65536*256 + kickmemory[i+1]*65536 + kickmemory[i+2]*256 + kickmemory[i+3];
	   cksum += data;
	   if (cksum < prevck)
	       cksum++;
	   prevck = cksum;
    }
    if (cksum != 0xFFFFFFFFul) {
	   write_log("Kickstart checksum incorrect. You probably have a corrupted ROM image.\n");
    }
    return 1;
}
#endif

/* Address banks */

addrbank dummy_bank = {
    dummy_lget, dummy_wget, dummy_bget,
    dummy_lput, dummy_wput, dummy_bput,
    default_xlate, dummy_check
};

addrbank chipmem_bank = {
    chipmem_lget, chipmem_wget, chipmem_bget,
    chipmem_lput, chipmem_wput, chipmem_bput,
    chipmem_xlate, chipmem_check
};

#ifdef HAS_BOGOMEM 
addrbank bogomem_bank = {
    bogomem_lget, bogomem_wget, bogomem_bget,
    bogomem_lput, bogomem_wput, bogomem_bput,
    bogomem_xlate, bogomem_check
};
#endif

addrbank kickmem_bank = {
    kickmem_lget, kickmem_wget, kickmem_bget,
    kickmem_lput, kickmem_wput, kickmem_bput,
    kickmem_xlate, kickmem_check
};

char *address_space, *good_address_map;
int good_address_fd;
#define MAKE_USER_PROGRAMS_BEHAVE 1
void memory_init(void)
{
    int i;
    int custom_start;

    buserr = 0;

    chipmem_mask = chipmem_size - 1;
    kickmem_mask = kickmem_size - 1;
    bogomem_mask = bogomem_size - 1;
    

#ifdef HALF_CHIPMEM
    chipmemory = (WORD*)emu_Malloc(chipmem_size/2);
    memset(chipmemory, 0, chipmem_size/2);
    memset(chipmemory2, 0, chipmem_size/2);    
#else
    //chipmemory = (WORD*)bigBuffer[0]; //chipmemory emu_Malloc(chipmem_size);
    memset(chipmemory, 0, chipmem_size); 
    emu_printf("init done !!");
#endif

    if(!chipmemory) {
	   write_log ("Virtual memory exhausted.\n");
	   abort();
    }

/*
    for(i = 0; i < MEM_BANK_SIZE; i++)
	   put_mem_bank (i<<16, &dummy_bank);
*/

    /* Map the chipmem into all of the lower 16MB */
    map_banks(&chipmem_bank, 0x00, 256); //chipmem_size>>16

    /* ??? my A500 has expansion memory, someone care to find out what
       * really is supposed to be at 0x200000 on an Amiga without? */
    custom_start = chipmem_size >> 16;
    if (custom_start < 0x20)
	   custom_start = 0x20;

    map_banks(&custom_bank, custom_start, 0xE0-custom_start);
    map_banks(&cia_bank, 0xA0, 32);

    //map_banks(&clock_bank, 0xDC, 1);

#ifdef HAS_BOGOMEM
    if (bogomem_size > 0) {
        bogomemory = (uae_u8 *)malloc(bogomem_size);
        if(!bogomemory) {
	       write_log ("Virtual memory exhausted.\n");
	       abort();
        }
        map_banks(&bogomem_bank, 0xC0, bogomem_size >> 16);
    }
#endif

#ifdef HAS_HARDFILE
    map_banks(&rtarea_bank, 0xF0, 1); 
#endif
    
#ifdef HAS_KICKFILE       
    if (!load_kickstart()) {
#ifdef HAS_ERSATZ          
       init_ersatz_rom(kickmemory);
       ersatzkickfile = 1;
#endif       
    }
    processFileInMem("kickbig.h","kick",kickmemory,kickmem_size);    
#endif

    if (zkickfile)
	   map_banks(&kickmem_bank, 0x20, 8);
    
    map_banks(&kickmem_bank, 0xF8, 8);

#ifdef HAS_EXPANMEM
    if (!zkickfile)
	   map_banks(&expamem_bank, 0xE8, 1);
#endif    
}

void map_banks(addrbank *bank, int start, int size)
{  
    //int bnr;
    //int hioffs = 0, endhioffs = 1;
    /* Some '020 Kickstarts apparently require a 24 bit address space... */
    //if (CPU_LEVEL >= 2 && !address_space_24)
	//   endhioffs = 256;
    //for (hioffs = 0; hioffs < endhioffs; hioffs++)
	//   for (bnr = start; bnr < start+size; bnr++)
	//       put_mem_bank((bnr + hioffs * 256) << 16, bank);   
    int bnr;
    for (bnr = start; bnr < start+size; bnr++) 
        //put_mem_bank(bnr, bank);
        mem_banks[bnr] = bank;             
}
