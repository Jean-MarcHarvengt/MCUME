 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Memory management
  *
  * (c) 1995 Bernd Schmidt
  */

#include "shared.h"

#include "memory.h"
#include "custom.h"
#include "cia.h"
#ifdef HAS_ERSATZ  
#include "ersatz.h"
int ersatzkickfile = 0;
#endif

#define HALF_CHIPMEM   1

#define HALF_KICKIMAGE 1

int buserr;
addrbank membanks[256];


/* Default memory access functions */

int default_check(CPTR a, ULONG b)
{
    return 0;
}

UWORD *default_xlate(CPTR a)
{
    emu_printf("Your Amiga program just did something terribly stupid\n");
    return 0;
}

/* A dummy bank that only contains zeros */

ULONG dummy_lget(CPTR addr)
{
    return 0;
}

UWORD dummy_wget(CPTR addr)
{
    return 0;
}

UBYTE dummy_bget(CPTR addr)
{
    return 0;
}

void dummy_lput(CPTR addr, ULONG l)
{
}

void dummy_wput(CPTR addr, UWORD w)
{
}

void dummy_bput(CPTR addr, UBYTE b)
{
}

int dummy_check(CPTR addr, ULONG size)
{
    return 0;
}

UWORD *dummy_xlate(CPTR addr)
{
    return NULL;
}

/* Chip memory */

UWORD *chipmemory=NULL;
void *chipmemory_end=NULL;
#ifdef HALF_CHIPMEM
static UBYTE chipmemextra[chipmem_size/2];
UWORD *chipmemory2=(UWORD*)&chipmemextra[0];
#else
UWORD *chipmemory2=NULL;
#endif

extern UWORD *chipmemoryadd;

ULONG chipmem_lget(CPTR addr)
{
    if (chipmemory != chipmemoryadd) {
      emu_printf("aaaaaa");
      //delay(100);
      chipmemory = chipmemoryadd;
    }  
    
    addr -= chipmem_start;
    addr &= chipmem_size-1;
#ifdef HALF_CHIPMEM
    if (addr >= 0x040000) {
        addr -= 0x040000;
        return ((ULONG)chipmemory2[addr >> 1] << 16) | chipmemory2[(addr >> 1)+1];
    }
#endif    
    return ((ULONG)chipmemory[addr >> 1] << 16) | chipmemory[(addr >> 1)+1];
}

UWORD chipmem_wget(CPTR addr)
{
    /*
    if (chipmemory != chipmemoryadd) {
      emu_printf("aaaaaa");
      delay(100);
      chipmemory = chipmemoryadd;
    }
    */ 
    addr -= chipmem_start;
    addr &= chipmem_size-1;
#ifdef HALF_CHIPMEM
    if (addr >= 0x040000) {
        addr -= 0x040000;
        return chipmemory2[addr >> 1];
    }
#endif    
    return chipmemory[addr >> 1];
}

UBYTE chipmem_bget(CPTR addr)
{
    /*
    if (chipmemory != chipmemoryadd) {
      emu_printf("aaaaaa");
      delay(100);
      chipmemory = chipmemoryadd;
    }
    */  
    addr -= chipmem_start;
    addr &= chipmem_size-1;
#ifdef HALF_CHIPMEM
    if (addr >= 0x040000) {
        addr -= 0x040000;
        if (addr & 1) 
           return chipmemory2[addr >> 1];
        else
           return chipmemory2[addr >> 1] >> 8;
    }
#endif    
    if (addr & 1) 
	   return chipmemory[addr >> 1];
    else
	   return chipmemory[addr >> 1] >> 8;
}

void chipmem_lput(CPTR addr, ULONG l)
{
    addr -= chipmem_start;
    addr &= chipmem_size-1;
#ifdef HALF_CHIPMEM
    if (addr >= 0x040000) {
        addr -= 0x040000;
        chipmemory2[addr >> 1] = l >> 16;
        chipmemory2[(addr >> 1)+1] = (UWORD)l;
        return;
    }
#endif    
    chipmemory[addr >> 1] = l >> 16;
    chipmemory[(addr >> 1)+1] = (UWORD)l;
}

void chipmem_wput(CPTR addr, UWORD w)
{
    addr -= chipmem_start;
    addr &= chipmem_size-1;
#ifdef HALF_CHIPMEM
    if (addr >= 0x040000) {
        addr -= 0x040000;
        chipmemory2[addr >> 1] = w;
        return;
    }
#endif    
    chipmemory[addr >> 1] = w;
}

void chipmem_bput(CPTR addr, UBYTE b)
{
    addr -= chipmem_start;
    addr &= chipmem_size-1;
#ifdef HALF_CHIPMEM
    if (addr >= 0x040000) {
        addr -= 0x040000;
        if (!(addr & 1)) {
          chipmemory2[addr>>1] = (chipmemory2[addr>>1] & 0xff) | (((UWORD)b) << 8);
        } else {
          chipmemory2[addr>>1] = (chipmemory2[addr>>1] & 0xff00) | b;
        }    
        return;
    }
#endif    
    if (!(addr & 1)) {
      chipmemory[addr>>1] = (chipmemory[addr>>1] & 0xff) | (((UWORD)b) << 8);
    } else {
      chipmemory[addr>>1] = (chipmemory[addr>>1] & 0xff00) | b;
    }    
}

int chipmem_check(CPTR addr, ULONG size)
{
    addr -= chipmem_start;
    addr &= chipmem_size-1;
    return (addr + size) < chipmem_size;
}

UWORD *chipmem_xlate(CPTR addr)
{
    addr -= chipmem_start;
    addr &= chipmem_size-1;
#ifdef HALF_CHIPMEM
    if (addr >= 0x040000) {
        addr -= 0x040000;
        return chipmemory2 + (addr >> 1);
    }
#endif
    return chipmemory + (addr >> 1);
}


/* Kick memory */

static int zkickfile = 0;

#ifdef HAS_KICKFILE   
static UWORD kick[kickmem_size/2];
#else
#ifdef HALF_KICKIMAGE
#include "kick13_swapped.h"
#else
#include "kickfull_swapped.h"
#endif
#endif
static UWORD *kickmemory=(UWORD*)&kick[0];


static ULONG kickmem_lget(CPTR) REGPARAM;
static UWORD kickmem_wget(CPTR) REGPARAM;
static UBYTE kickmem_bget(CPTR) REGPARAM;
static void  kickmem_lput(CPTR, ULONG) REGPARAM;
static void  kickmem_wput(CPTR, UWORD) REGPARAM;
static void  kickmem_bput(CPTR, UBYTE) REGPARAM;
static int  kickmem_check(CPTR addr, ULONG size) REGPARAM;
static UWORD *kickmem_xlate(CPTR addr) REGPARAM;


ULONG kickmem_lget(CPTR addr)
{
    addr -= kickmem_start;
    addr &= kickmem_size-1;
#ifdef HALF_KICKIMAGE
    if (addr >= 0x040000) {addr -= 0x040000;}
#endif    
    return ((ULONG)kickmemory[addr >> 1] << 16) | kickmemory[(addr >> 1)+1];
}

UWORD kickmem_wget(CPTR addr)
{
    addr -= kickmem_start;
    addr &= kickmem_size-1;
#ifdef HALF_KICKIMAGE
    if (addr >= 0x040000) {addr -= 0x040000;}
#endif    
    return kickmemory[addr >> 1];
}

UBYTE kickmem_bget(CPTR addr)
{
    addr -= kickmem_start;
    addr &= kickmem_size-1;
#ifdef HALF_KICKIMAGE
    if (addr >= 0x040000) {addr -= 0x040000;}
#endif    
    return kickmemory[addr >> 1] >> (addr & 1 ? 0 : 8);
}

void kickmem_lput(CPTR a, ULONG b)
{
}

void kickmem_wput(CPTR a, UWORD b)
{
}

void kickmem_bput(CPTR a, UBYTE b)
{
}

int kickmem_check(CPTR addr, ULONG size)
{
    addr -= kickmem_start;
    addr &= kickmem_size-1;
    return (addr + size) < kickmem_size;
}

UWORD *kickmem_xlate(CPTR addr)
{
    addr -= kickmem_start;
    addr &= kickmem_size-1;
 #ifdef HALF_KICKIMAGE
    if (addr >= 0x040000) {addr -= 0x040000;}
#endif    
    return kickmemory + (addr >> 1);
}



#ifdef HAS_BOGOMEM 

/* Slow memory */

static UWORD *bogomemory;

static ULONG bogomem_lget(CPTR) REGPARAM;
static UWORD bogomem_wget(CPTR) REGPARAM;
static UBYTE bogomem_bget(CPTR) REGPARAM;
static void  bogomem_lput(CPTR, ULONG) REGPARAM;
static void  bogomem_wput(CPTR, UWORD) REGPARAM;
static void  bogomem_bput(CPTR, UBYTE) REGPARAM;
static int  bogomem_check(CPTR addr, ULONG size) REGPARAM;
static UWORD *bogomem_xlate(CPTR addr) REGPARAM;

ULONG bogomem_lget(CPTR addr)
{
    addr -= bogomem_start & (bogomem_size-1);
    addr &= bogomem_size-1;
    return ((ULONG)bogomemory[addr >> 1] << 16) | bogomemory[(addr >> 1)+1];
}

UWORD bogomem_wget(CPTR addr)
{
    addr -= bogomem_start & (bogomem_size-1);
    addr &= bogomem_size-1;
    return bogomemory[addr >> 1];
}

UBYTE bogomem_bget(CPTR addr)
{
    addr -= bogomem_start & (bogomem_size-1);
    addr &= bogomem_size-1;
    if (addr & 1)
    return bogomemory[addr >> 1];
    else
    return bogomemory[addr >> 1] >> 8;
}

void bogomem_lput(CPTR addr, ULONG l)
{
    addr -= bogomem_start & (bogomem_size-1);
    addr &= bogomem_size-1;
    bogomemory[addr >> 1] = l >> 16;
    bogomemory[(addr >> 1)+1] = (UWORD)l;
}

void bogomem_wput(CPTR addr, UWORD w)
{
    addr -= bogomem_start & (bogomem_size-1);
    addr &= bogomem_size-1;
    bogomemory[addr >> 1] = w;
}

void bogomem_bput(CPTR addr, UBYTE b)
{
    addr -= bogomem_start & (bogomem_size-1);
    addr &= bogomem_size-1;
    if (!(addr & 1)) {
    bogomemory[addr>>1] = (bogomemory[addr>>1] & 0xff) | (((UWORD)b) << 8);
    } else {
    bogomemory[addr>>1] = (bogomemory[addr>>1] & 0xff00) | b;
    }
}

int bogomem_check(CPTR addr, ULONG size)
{
    addr -= bogomem_start & (bogomem_size-1);
    addr &= bogomem_size-1;
    return (addr + size) < bogomem_size;
}

UWORD *bogomem_xlate(CPTR addr)
{
    addr -= bogomem_start & (bogomem_size-1);
    addr &= bogomem_size-1;
    return bogomemory + (addr >> 1);
}
#endif

#ifdef HAS_KICKFILE
int processFileInMem(char * infile, char * outfile, char * arrname, unsigned char * data, int size) {

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
    ULONG cksum = 0, prevck = 0;
    unsigned char buffer[8];
    
    FILE *f = fopen(romfile, "rb");
    
    if (f == NULL) {	
        emu_printf("No Kickstart ROM found.\n");
        return 0;
    }
    
    fread(buffer, 1, 8, f);
    if (buffer[4] == 0 && buffer[5] == 8 && buffer[6] == 0 && buffer[7] == 0) {
	   emu_printf("You seem to have a ZKick file there... You probably lose.\n");
	   zkickfile = 1;
    } else 
	fseek(f, 0, SEEK_SET);
    
    i = fread(kick, 1, kickmem_size, f);
    if (i == kickmem_size/2) {
	   emu_printf("Warning: Kickstart is only 256K.\n");
	   memcpy (kick + kickmem_size/4, kick, kickmem_size/2);
    } else if (i != kickmem_size) {
	   emu_printf("Error while reading Kickstart.\n");
	   return 0;
    }
    fclose (f);
    
    for (i = 0; i < kickmem_size/2; i++) {
	   UWORD *p = kick + i;
	   UBYTE *bp = (UBYTE *)p;
	   *p = (*bp << 8) | *(bp+1);
    }

    
    for (i = 0; i < kickmem_size/4; i++) {
	   ULONG data = kick[i*2]*65536 + kick[i*2+1];
	   cksum += data;
	   if (cksum < prevck)
	       cksum++;
	   prevck = cksum;
    }
    if (cksum != 0xFFFFFFFF) {
	   emu_printf("Warning: Kickstart checksum incorrect. You probably have a corrupted ROM image.\n");
    }

    return 1;
}
#endif


/* Address banks */

addrbank dummy_bank = {
    dummy_lget, dummy_wget, dummy_bget,
    dummy_lput, dummy_wput, dummy_bput,
    dummy_xlate, dummy_check
};

addrbank chipmem_bank = {
    chipmem_lget, chipmem_wget, chipmem_bget,
    chipmem_lput, chipmem_wput, chipmem_bput,
    chipmem_xlate, chipmem_check
};

addrbank kickmem_bank = {
    kickmem_lget, kickmem_wget, kickmem_bget,
    kickmem_lput, kickmem_wput, kickmem_bput,
    kickmem_xlate, kickmem_check
};

#ifdef HAS_BOGOMEM   
addrbank bogomem_bank = {
    bogomem_lget, bogomem_wget, bogomem_bget,
    bogomem_lput, bogomem_wput, bogomem_bput,
    bogomem_xlate, bogomem_check
};
#endif


void memory_init(void)
{
    int i;
    buserr = 0;

#ifdef HALF_CHIPMEM
    chipmemory = (WORD*)emu_Malloc(chipmem_size/2);
    chipmemoryadd = chipmemory;
    memset(chipmemory, 0, chipmem_size/2);
    memset(chipmemory2, 0, chipmem_size/2);
    chipmemory_end = chipmemory;
    chipmemory_end += chipmem_size/2;    
#else
    chipmemory = (WORD*)emu_Malloc(chipmem_size);
    memset(chipmemory, 0, chipmem_size);
    chipmemory_end = chipmemory;
    chipmemory_end += chipmem_size;    
#endif

    for(i = 0; i < 256; i++)
	   membanks[i] = dummy_bank;
    
    /* Map the chipmem into all of the lower 16MB */
    map_banks(chipmem_bank, 0x00, 256); //chipmem_size>>16??
    map_banks(custom_bank, 0xC0, 0x20);
    map_banks(cia_bank, 0xA0, 0x20);
    
    //0x000000 - 0x080000 
    //0xA00000 - 0xBFFFFF
    //0xC00000 - 0xDFFFFF
    //0xF80000 - 0xFFFFFF


    //map_banks(clock_bank, 0xDC, 1);
    //map_banks(rtarea_bank, 0xF0, 1); 
    //rtarea_init ();
    
#ifdef HAS_KICKFILE       
    if (!load_kickstart()) {
#ifdef HAS_ERSATZ          
	   init_ersatz_rom(kickmemory);
	   ersatzkickfile = 1;
#endif       
    }
    //processFileInMem("kick13.rom","kickbig.h","kick",kick,kickmem_size);    
#endif

    if (zkickfile)
	   map_banks(kickmem_bank, 0x20, 8);
    
    map_banks(kickmem_bank, 0xF8, 8);

#ifdef HAS_BOGOMEM   
    if (bogomem_size > 0) {
        bogomemory = (UWORD *)emu_malloc (bogomem_size);
        map_banks(bogomem_bank, 0xC0, bogomem_size >> 16);
    }
#endif

#ifdef HAS_EXPANMEM   
    fastmem_size = 0x200000;    
    if (!zkickfile)
	   map_banks(expamem_bank, 0xE8, 1);
#endif    
}

void map_banks(addrbank bank, int start, int size)
{
    int bnr;
//    int hioffs = 0;
//#if CPU_LEVEL < 2
//    for (hioffs = 0; hioffs < 256; hioffs++)
//#endif
	for (bnr = start; bnr < start+size; bnr++) 
	    membanks[bnr /*+ hioffs * 256*/] = bank;
}
