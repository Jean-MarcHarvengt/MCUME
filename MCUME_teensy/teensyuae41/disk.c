 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * Floppy disk emulation
  *
  * (c) 1995 Bernd Schmidt, Hannu Rummukainen
  */

#include "shared.h"

#include "memory.h"
#ifdef HAS_ERSATZ  
#include "ersatz.h"
#endif
#include "disk.h"
#include "gui.h"

#define NB_DRIVE 1

//#define HAS_PSRAM 1

typedef struct {
    UWORD sync;
    UWORD len;
    ULONG offs;
} trackid;

typedef enum { ADF_NORMAL, ADF_EXT1 } drive_filetype;
typedef struct {
    FILE *diskfile;
#ifdef HAS_MEMDISK
    int data_pt;
    int dnum;
#endif    
    drive_filetype filetype;
    trackid trackdata[164];
    unsigned int track;
    int motoroff;
    int wrprot;
    unsigned int trackoffs,insecpos;
    int secok;
    UBYTE secbuf[544];
    UWORD mfmbuf[544];
} drive;

#ifdef HAS_MEMDISK
#ifdef HAS_PSRAM
extern unsigned char read_rom(int address);

static void mem_FileOpen(drive *drv, int dnum)
{
  drv->data_pt = 0;
  drv->dnum = dnum;
}

static void mem_FileClose(drive *drv)
{
  drv->data_pt = 0;
}

static int mem_FileSeek(drive *drv, int offset, int seek)
{
  drv->data_pt = offset; 
}
static int mem_FileRead(drive *drv, char * buf, int size)
{
  while (size>0) {
    *buf++ = read_rom((drv->dnum*0x100000)+drv->data_pt++); 
    size--; 
  }
}
#else
#include "arduinoproto.h"
EXTMEM static unsigned char drivemem[FLOPPY_SIZE*NB_DRIVE];

void disk_memWrite(int pos, unsigned char val)
{
    drivemem[pos]=val;
}

static void mem_FileOpen(drive *drv, int dnum)
{
  drv->data_pt = 0;
  drv->dnum = dnum;
}

static void mem_FileClose(drive *drv)
{
  drv->data_pt = 0;
}

static int mem_FileSeek(drive *drv, int offset, int seek)
{
  drv->data_pt = offset; 
}
static int mem_FileRead(drive *drv, char * buf, int size)
{
  while (size>0) {
    *buf++ = drivemem[(drv->dnum*FLOPPY_SIZE)+drv->data_pt++]; 
    size--; 
  }
}
#endif
#endif

int indexpulse = 0;

#ifdef unused
UWORD* mfmwrite;
static UWORD * mfmwrbuffer; /* space for maximum disk DMA transfer */
#endif

static int side, direction, step;
static UBYTE selected = 15;
static int dskready;
static int need_read = 0;



//static drive floppy[NB_DRIVE];
static drive * floppy;


static void drive_insert(drive *drv, int dnum, char *fname)
{
    unsigned char buffer[10];
    emu_printf("inserting:");
    emu_printf(fname);
    emu_printf("into");
    emu_printi(dnum);

    if (dnum >= NB_DRIVE) {
    	drv->diskfile = 0;
    	return;
    }	
#ifdef HAS_MEMDISK
    drv->diskfile = 1;
    mem_FileOpen(drv, dnum);
#else
    drv->diskfile = emu_FileOpen(fname);
#endif    
    if (drv->diskfile) {
    	emu_printf("disk opened");
    	emu_printi(dnum);
		drv->wrprot = 0;
    } 
    /*
    else {
		drv->wrprot = 1;
		drv->diskfile = fopen(fname, "rb");
		if (!drv->diskfile) {
	    	gui_filename(dnum, "");
	    	return;
		}
    }
    gui_filename(dnum, fname);
    */
#ifdef HAS_MEMDISK
    //fread(buffer,sizeof(char),8,drv->diskfile);
    mem_FileRead(drv, buffer,sizeof(char)*8);
#else
    //fread(buffer,sizeof(char),8,drv->diskfile);
    emu_FileRead(buffer,sizeof(char)*8);
#endif    
    if (strncmp((char *)buffer,"UAE--ADF",8) == 0) {	
	int offs = 160*4+8;
	int i;
	
    drv->filetype = ADF_EXT1;
	drv->wrprot = 1; /* write to adf_ext1 not implemented */
	for(i=0; i<160; i++) {
#ifdef HAS_MEMDISK    
	    mem_FileRead(drv, buffer, 4*1);
#else
      //fread(buffer, 4, 1, drv->diskfile);
      emu_FileRead(buffer, 4*1);
#endif      
	    drv->trackdata[i].sync = buffer[0]*256 + buffer[1];
	    drv->trackdata[i].len = buffer[2]*256 + buffer[3];
	    drv->trackdata[i].offs = offs;
	    offs += drv->trackdata[i].len;
	}
    } else {
	int i;
    	drv->filetype = ADF_NORMAL;
	for(i=0; i<160; i++) {
	    drv->trackdata[i].len = 512 * 11;
	    drv->trackdata[i].sync = 0;
	    drv->trackdata[i].offs = i*512*11;
	}
    }
}


static void drive_step(drive *drv)
{
    if (direction) {
	if (drv->track) drv->track--;
    } else {
	if (drv->track < 85) drv->track++;
    }
}


static int drive_track0(drive *drv) 
{
    return drv->track == 0; 
}

static int drive_writeprotected(drive *drv) 
{
    return drv->wrprot || drv->diskfile == NULL; 
}

static int drive_empty(drive *drv)
{
    return drv->diskfile == 0; 
}

static int drive_running(drive *drv)
{
    return !drv->motoroff; 
}

static void drive_index(drive *drv)
{
    drv->trackoffs = 0;
    drv->insecpos = 0;
}

static void drive_motor(drive *drv, int off)
{
    if (drv->motoroff && !off) { 
	drv->trackoffs = 0;
	drv->insecpos = 0; 
	drv->secok = 0; 
    }
    drv->motoroff = off;
}

static ULONG drive_getmfmulong(drive *drv, unsigned int offs)
{
    return (drv->mfmbuf[offs] << 16) | drv->mfmbuf[offs+1];
}

static ULONG drive_getseculong(drive *drv, unsigned int offs)
{
    return ((drv->secbuf[offs] << 24) | (drv->secbuf[offs+1] << 16) 
	    | (drv->secbuf[offs+2] << 8) | (drv->secbuf[offs+3]));
}

static void drive_readsec(drive *drv)
{
    ULONG deven,dodd;
    ULONG hck=0,dck=0;
    int i;
    if (!drv->diskfile) return;
//emu_printf("readsec");
    
    drv->secbuf[0] = drv->secbuf[1] = 0x00;
    drv->secbuf[2] = drv->secbuf[3] = 0xa1;
    drv->secbuf[4] = 0xff;
    drv->secbuf[5] = drv->track*2 + side;
    drv->secbuf[6] = drv->trackoffs;
    drv->secbuf[7] = 11-drv->trackoffs;
    
    for(i = 8; i < 24; i++)
	drv->secbuf[i] = 0;
#ifdef HAS_MEMDISK
    mem_FileSeek(drv, drv->trackdata[drv->track*2 + side].offs + drv->trackoffs*512, 
    SEEK_SET);
    mem_FileRead(drv, &drv->secbuf[32],sizeof(UBYTE)*512);
#else    
    //fseek(drv->diskfile, 
	//  drv->trackdata[drv->track*2 + side].offs + drv->trackoffs*512, 
	//  SEEK_SET);
    //fread(&drv->secbuf[32],sizeof(UBYTE),512,drv->diskfile);
    emu_FileSeek(drv->trackdata[drv->track*2 + side].offs + drv->trackoffs*512, 
	  SEEK_SET);
    emu_FileRead(&drv->secbuf[32],sizeof(UBYTE)*512);
#endif
    drv->mfmbuf[0] = drv->mfmbuf[1] = 0xaaaa;
    drv->mfmbuf[2] = drv->mfmbuf[3] = 0x4489;
    
    deven = drive_getseculong(drv, 4); dodd = deven >> 1;
    deven &= 0x55555555; dodd &= 0x55555555;
    
    drv->mfmbuf[4] = dodd >> 16;
    drv->mfmbuf[5] = dodd;
    drv->mfmbuf[6] = deven>> 16; 
    drv->mfmbuf[7] = deven;
    
    for (i = 8; i < 48; i++)
	drv->mfmbuf[i] = 0;
    for (i = 0; i < 512; i += 4){
	deven = drive_getseculong(drv, i + 32);
	dodd = deven >> 1;
	deven &= 0x55555555; dodd &= 0x55555555;
	drv->mfmbuf[(i>>1)+32] = dodd >> 16;
	drv->mfmbuf[(i>>1)+33] = dodd;
	drv->mfmbuf[(i>>1)+256+32] = deven>> 16;
	drv->mfmbuf[(i>>1)+256+33] = deven;
    }
    
    for(i = 4; i < 24; i += 2)
	hck ^= drive_getmfmulong(drv, i);
    
    deven = dodd = hck; dodd >>= 1;
    drv->mfmbuf[24] = dodd >> 16; drv->mfmbuf[25] = dodd;
    drv->mfmbuf[26] = deven>> 16; drv->mfmbuf[27] = deven;
    
    for(i = 32; i < 544; i += 2)
	dck ^= drive_getmfmulong(drv, i);
    
    deven = dodd = dck; dodd >>= 1;
    drv->mfmbuf[28] = dodd >> 16; drv->mfmbuf[29] = dodd;
    drv->mfmbuf[30] = deven>> 16; drv->mfmbuf[31] = deven;
    
    drv->secok = 1;
    need_read = 0;
    drv->insecpos = 0;
}

static void drive_get_data(drive *drv, UWORD *mfm, UWORD *byt)
{
    if (drv->trackdata[drv->track*2 + side].sync == 0) {
	/* Normal AmigaDOS format track */
	if (!drv->secok || need_read)
	    drive_readsec(drv);
	*mfm = drv->mfmbuf[drv->insecpos]; 
	*byt = drv->secbuf[drv->insecpos++];
	
	if (drv->insecpos == 544){
	    drv->insecpos = 0;
	    drv->secok = 0;
	    if (++drv->trackoffs == 11)
		drv->trackoffs = 0;
	}
    } else {
	/* Raw MFM track */
	if (drv->insecpos > drv->trackdata[drv->track*2+side].len) 
	    drv->insecpos = 0;
	if (drv->insecpos == 0) {
	    *mfm = drv->trackdata[drv->track*2 + side].sync;
	} else  {	    
	    unsigned char data[2];
#ifdef HAS_MEMDISK
      mem_FileSeek(drv, drv->insecpos*2 - 2 + drv->trackdata[drv->track*2+side].offs, 
      SEEK_SET);
      mem_FileRead(drv, data, 2*1);
#else      
	    //fseek(drv->diskfile, 
		//  drv->insecpos*2 - 2 + drv->trackdata[drv->track*2+side].offs, 
		//  SEEK_SET);
	    //fread(data, 2, 1, drv->diskfile);
	    emu_FileSeek(drv->insecpos*2 - 2 + drv->trackdata[drv->track*2+side].offs, 
		  SEEK_SET);
	    emu_FileRead(data, 2*1);
#endif      
	    *mfm = data[0]*256 + data[1];
	    /* ??? how does this work? */
	    *byt = (data[0] & 0x55) | ((data[1] & 0x55)*2);
	}
	drv->insecpos++;
    }
}

#define MFMMASK 0x55555555
static __inline__ ULONG getmfmlong(UWORD* mbuf) 
{
    return ((*mbuf << 16) | *(mbuf + 1)) & MFMMASK;
}

static void drive_write_data(drive *drv, UWORD *mbuf, UWORD *mend)
{
    int i, secwritten = 0;
    ULONG odd, even, chksum, id, dlong;
    UBYTE* secdata;
    
    if (drive_writeprotected(drv)) return;
    mend -= (4 + 16 + 8 + 512);
    while (mbuf < mend) {
	do {
	    while (*mbuf++ != 0x4489) {
		if (mbuf >= mend) return;
	    }
	} while (*mbuf++ != 0x4489);
	
	odd = getmfmlong(mbuf);
	even = getmfmlong(mbuf+2);
	mbuf += 4;	
	id = (odd << 1) | even;
	
	drv->trackoffs = (id & 0xff00) >> 8;
	if (drv->trackoffs > 10) {
	    emu_printf("Disk write: weird sector number\n");
	    //emu_printi(drv->trackoffs);
	    continue;
	}
	chksum = odd ^ even;
	for (i=0; i<4; i++) {
	    odd = getmfmlong(mbuf);
	    even = getmfmlong(mbuf+8);
	    mbuf += 2;
	    
	    dlong = (odd << 1) | even;
	    if (dlong)  secwritten = -200;
	    chksum ^= odd ^ even;
	}  /* could check here if the label is nonstandard */
	mbuf += 8;
	odd = getmfmlong(mbuf); even = getmfmlong(mbuf+2); mbuf += 4;
	if ((((odd << 1) | even) != chksum) || 
	    (((id & 0x00ff0000) >> 16) != drv->track*2 + side)) {
	    emu_printf("Disk write: checksum error on sector header\n");
	    continue;
	}
	odd = getmfmlong(mbuf); even = getmfmlong(mbuf+2); mbuf += 4;
	chksum = (odd << 1) | even;
	secdata = drv->secbuf + 32;
	for (i=0; i<128; i++) {
	    odd = getmfmlong(mbuf); even = getmfmlong(mbuf+256); mbuf += 2;
	    dlong = (odd << 1) | even;
	    *secdata++ = dlong >> 24; *secdata++ = (dlong >> 16) & 0xff;
	    *secdata++ = dlong >> 8; *secdata++ = dlong;
	    chksum ^= odd ^ even;
	}
	mbuf += 256;
	if (chksum) {
	    emu_printf("Disk write: data checksum error\n");
	    continue;
	}
	secwritten++;
#ifdef HAS_MEMDISK
  mem_FileSeek(drv, drv->trackdata[drv->track*2 + side].offs + drv->trackoffs*512,
        SEEK_SET);
#else
//fseek(drv->diskfile, 
	//      drv->trackdata[drv->track*2 + side].offs + drv->trackoffs*512,
	//      SEEK_SET);
	//fwrite(drv->secbuf+32, sizeof(UBYTE), 512, drv->diskfile);
	emu_FileSeek(drv->trackdata[drv->track*2 + side].offs + drv->trackoffs*512,
	      SEEK_SET);
#endif        
    }
    if (++drv->trackoffs == 11) drv->trackoffs = 0;
    drv->insecpos = 0;
    drv->secok = 0;
    
    if (secwritten == 0) 
	emu_printf("Disk write in unsupported format\n");
    if (secwritten < 0)
	emu_printf("Disk write: sector labels ignored\n");
}


static void drive_eject(drive *drv)
{
#ifdef HAS_MEMDISK
    mem_FileClose(drv);
#else
    //if (!drive_empty(drv)) fclose(drv->diskfile);
    if (!drive_empty(drv)) emu_FileClose();
#endif    
    drv->diskfile = 0;
}

/* We use this function if we have no Kickstart ROM. */
void DISK_ersatz_read (int tr, int sec, CPTR dest)
{
    int i;

    floppy[0].trackoffs = sec;
    floppy[0].insecpos = 0;
    
    side = tr & 1;
    floppy[0].track = tr >> 1;
    drive_readsec(floppy);
    for (i = 0; i < 512; i++) {
	put_byte (dest + i, floppy[0].secbuf[32+i]);
    }
    
}

void disk_eject(int num)
{
    gui_filename(num, "");
    drive_eject(floppy + num);
}

void disk_insert(int num, char *name)
{
    /* just to be sure */
    drive_eject(floppy + num);
    drive_insert(floppy + num, num, name);
}

static int first = 0;

void disk_swap(void)
{
    //drive_eject(floppy + 1);
    drive_eject(floppy + 0);
    if (first == 0)
    {
        first = 1;
        drive_insert(floppy + 0, 0, df1);
        //drive_insert(floppy + 1, 1‡, df0);
    }
    else
    {
        first = 0;
        drive_insert(floppy + 0, 0, df0);
        //drive_insert(floppy + 1, 1, df1);
    }
}

int disk_empty(int num)
{
    return drive_empty(floppy + num);
}

void DISK_init()
{
#ifdef unused    
	mfmwrbuffer = (WORD*)emu_Malloc(16384*2);
#endif 
    floppy = emu_Malloc(sizeof(drive)*NB_DRIVE);
    memset(floppy, 0, sizeof(drive)*NB_DRIVE);

    drive_insert(floppy, 0, df0);
    //drive_insert(floppy + 1, 1, df1);
    //drive_insert(floppy + 2, 2, df2);
    //drive_insert(floppy + 3, 3, df3);
}

void DISK_Index()
{
    int i;
    for(i=0; i<NB_DRIVE; i++) {	
	   drive_index(floppy + i);
    }
}

static int ledstate[] = { 0,0,0,0 };

void DISK_select(UBYTE data)
{
    int step_pulse;
    int dr;
    
    if (selected != ((data >> 3) & 15)) 
	dskready = 0;
    selected = (data >> 3) & 15;
    if (side != 1 - ((data >> 2) & 1)) {
	side = 1 - ((data >> 2) & 1);
	need_read = 1;
    }
    direction = (data >> 1) & 1;
    step_pulse = data & 1;
    if (step != step_pulse) {
	step = step_pulse;
	if (step == 0){
	    for (dr = 0; dr < NB_DRIVE; dr++){
		if (!(selected & (1 << dr))) {
		    drive_step(floppy + dr);
		}
	    }
	}
    }
    for (dr = 0; dr < NB_DRIVE; dr++){
	if (!(selected & (1<<dr))) {
	    drive_motor(floppy + dr, data >> 7);
	}
    }
    for (dr = 0; dr < NB_DRIVE; dr++) {
	int state = (!(selected & (1<<dr))) | !floppy[dr].motoroff;
	if (state != ledstate[dr])
	    gui_led (dr+1, state);
	ledstate[dr] = state;
    }
}

UBYTE DISK_status()
{
    UBYTE st = 0x3c;
    int dr;
    
    for (dr = 0; dr < NB_DRIVE; dr++){
	if (!(selected & (1 << dr))) {
	    if (drive_running(floppy + dr)){
		if (dskready) st &= ~0x20;
		dskready = 1;
	    } else {
		st &= ~0x20; /* report drive ID */
	    }
		
	    if (drive_track0(floppy + dr)) { st &= ~0x10; }
	    if (drive_writeprotected(floppy + dr)) { st &= ~8; }
	    if (drive_empty(floppy + dr)) { st &= ~0x4; }
	}
    }
    return st;
}

void DISK_GetData(UWORD *mfm,UWORD *byt)
{
    int dr;
    for (dr = 0; dr < NB_DRIVE; dr++){
	if (!(selected & (1<<dr))) {
	    drive_get_data (floppy + dr, mfm, byt);
	}
    }
}

void DISK_InitWrite()
{
#ifdef unused
    mfmwrite = mfmwrbuffer;
#endif    
}

void DISK_WriteData()
{
#ifdef unused
    int dr;
    for (dr=0;dr<NB_DRIVE;dr++){
	if (!(selected & (1<<dr))) {
	    drive_write_data(floppy + dr, mfmwrbuffer, mfmwrite);
	}
    }
#endif    
}

