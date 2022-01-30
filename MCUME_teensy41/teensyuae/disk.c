 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Floppy disk emulation
  *
  * Copyright 1995 Bernd Schmidt, Hannu Rummukainen
  */

#include <stdio.h>

#include "shared.h"

#include "memory.h"
#include "events.h"
#include "custom.h"
#include "ersatz.h"
#include "disk.h"
#include "gui.h"
#include "zfile.h"
#include "autoconf.h"
#include "readcpu.h"
#include "newcpu.h"
#include "xwin.h"

#define NB_DRIVE 1


#define STATIC_INLINE static

#define FLOPPY_GAP_LEN 360

#define FLOPPY_SPEED 50

#ifdef HAS_FLOPPYWRITE
uae_u16* mfmwrite;
static uae_u16 mfmwrbuffer[16384]; /* space for maximum disk DMA transfer */
#endif

static int side, direction, step;
static uae_u8 selected = 15;
static int dskready;

typedef struct {
    uae_u16 sync;
    uae_u16 len;
    uae_u32 offs;
} trackid;

#define MAX_TRACKS 328

typedef enum { ADF_NORMAL, ADF_EXT1 } drive_filetype;
typedef struct {
    FILE *diskfile;
    drive_filetype filetype;
    trackid trackdata[MAX_TRACKS];
    unsigned int buffered_cyl, buffered_side;
    unsigned int cyl;
    int side;
    int motoroff;
    int wrprot;
    uae_u16 bigmfmbuf[0x4000];
    int mfmbufpos;
    int tracklen;
    unsigned long last_cycles;
    int mfmpos;
    unsigned int num_tracks, num_secs;
    /* dskchange is set to a small number when a disk is removed. At a step
     * impulse, it is reset if there is a new disk in the drive or counted
     * down to 1 if the drive contains a disk. DISK_check_change will only
     * insert a new disk when dskchange is 1, or if the dskchange_time timer
     * expires. The last condition can happen if AmigaOS isn't running and a
     * game/demo doesn't use the step motor to check for a new disk.
     */
    int dskchange;
    int dskchange_time;
} drive;

drive floppy[NB_DRIVE];

static void drive_fill_bigbuf (drive *drv);

#ifdef unused
FILE *DISK_validate_filename (const char *fname, int leave_open, int *wrprot)
{
    FILE *f = fopen (fname, "r+b");
    if (f) {
  if (wrprot)
      *wrprot = 0;
    } else {
  if (wrprot)
      *wrprot = 1;
  f = fopen (fname, "rb");
    }
    if (!leave_open)
  fclose (f);
    return f;
}
#endif

static int drive_insert (drive *drv, int dnum, const char *fname)
{
    unsigned char buffer[10];
    emu_printf("insert");
    emu_printf(fname);

#ifdef unused
    drv->diskfile = DISK_validate_filename (fname, 1, &drv->wrprot);
#endif
  drv->diskfile = emu_FileOpen(fname, "r+b");    
    if (drv->diskfile == 0) {
  drv->tracklen = 2000;
  return 0;
    }
 
    strncpy (currprefs.df[dnum], fname, 255);
    currprefs.df[dnum][255] = 0;
    strncpy (changed_prefs.df[dnum], fname, 255);
    changed_prefs.df[dnum][255] = 0;
    //gui_filename(dnum, fname);

    emu_FileRead(buffer, 8, drv->diskfile);
    if (strncmp((char *)buffer,"UAE--ADF",8) == 0) {
    emu_printf("UAE--ADF");
  int offs = 160*4+8;
  int i;

  drv->filetype = ADF_EXT1;
  drv->num_tracks = 160;
  drv->num_secs = 11;

  drv->wrprot = 1; /* write to adf_ext1 not implemented */
  for(i=0; i<160; i++) {
      emu_FileRead(buffer, 4, drv->diskfile);
      drv->trackdata[i].sync = buffer[0]*256 + buffer[1];
      drv->trackdata[i].len = buffer[2]*256 + buffer[3];
      drv->trackdata[i].offs = offs;
      offs += drv->trackdata[i].len;
  }
    } else {
  unsigned int i;
    emu_printf("NORMAL");

  drv->filetype = ADF_NORMAL;

  emu_FileClose(drv->diskfile);    
  i = emu_FileSize(fname);
  drv->diskfile = emu_FileOpen(fname, "r+b");    
  /* High-density disk? */
  if (i >= 160*22*512)
      drv->num_tracks = i / (512*(drv->num_secs = 22));
  else
      drv->num_tracks = i / (512*(drv->num_secs = 11));

  if (drv->num_tracks > MAX_TRACKS)
      write_log ("Your diskfile is too big!\n");
  for(i = 0; i < drv->num_tracks; i++) {
      drv->trackdata[i].len = 512 * drv->num_secs;
      drv->trackdata[i].sync = 0;
      drv->trackdata[i].offs = i * 512 * drv->num_secs;
  }
    }
    drv->buffered_side = 2; /* will force read */
    drive_fill_bigbuf (drv);
    return 1;
}

static int drive_empty (drive *drv)
{
    return drv->diskfile == 0;
}

static void drive_step (drive *drv)
{
    if (!drive_empty(drv))
  drv->dskchange = 0;
    else if (drv->dskchange > 1) {
/*  printf("Stepping...\n");
  drv->dskchange--;*/
    }
    if (direction) {
  if (drv->cyl) drv->cyl--;
    } else {
  if (drv->cyl < drv->num_tracks/2) drv->cyl++;
    }
}


static int drive_track0 (drive *drv)
{
    return drv->cyl == 0;
}

static int drive_writeprotected (drive *drv)
{
    return drv->wrprot || drv->diskfile == NULL;
}

static int drive_running (drive *drv)
{
    return !drv->motoroff;
}

static void drive_motor (drive *drv, int off)
{
    if (drv->motoroff && !off) {
  drv->last_cycles = cycles;
  drv->mfmpos = 0;
  eventtab[ev_diskindex].active = 1;
  if (drv->tracklen)
      eventtab[ev_diskindex].evtime = FLOPPY_SPEED * drv->tracklen + cycles;
  else
      eventtab[ev_diskindex].evtime = cycles + 1000;
  eventtab[ev_diskindex].oldcycles = cycles;
    } else if (off)
  eventtab[ev_diskindex].active = 0;
    drv->motoroff = off;
    events_schedule ();
}

static void drive_fill_bigbuf(drive *drv)
{
    int tr = drv->cyl*2 + side;

    if (!drv->diskfile) {
  drv->tracklen = 2000;
  memset (drv->bigmfmbuf,0xaa,drv->tracklen*2);
  return;
    }
    if (drv->buffered_cyl == drv->cyl && drv->buffered_side == side)
  return;

/*    if (drv->num_tracks <= tr) {
  write_log ("Access beyond end of floppy image!\n");
    }*/
    
    if (drv->trackdata[tr].sync == 0) {
  /* Normal AmigaDOS format track */
  unsigned int sec;
  drv->tracklen = drv->num_secs*544 + FLOPPY_GAP_LEN;
  memset (drv->bigmfmbuf,0xaa,FLOPPY_GAP_LEN*2);

  for (sec = 0; sec < drv->num_secs; sec++) {
      uae_u8 secbuf[544];
      int i;
      uae_u16 *mfmbuf = drv->bigmfmbuf + 544*sec + FLOPPY_GAP_LEN;
      uae_u32 deven,dodd;
      uae_u32 hck=0,dck=0;

      secbuf[0] = secbuf[1] = 0x00;
      secbuf[2] = secbuf[3] = 0xa1;
      secbuf[4] = 0xff;
      secbuf[5] = tr;
      secbuf[6] = sec;
      secbuf[7] = drv->num_secs-sec;

      for(i = 8; i < 24; i++)
    secbuf[i] = 0;

      emu_FileSeek (drv->diskfile, drv->trackdata[tr].offs + sec*512,
       SEEK_SET);
      emu_FileRead (&secbuf[32],512, drv->diskfile);

      mfmbuf[0] = mfmbuf[1] = 0xaaaa;
      mfmbuf[2] = mfmbuf[3] = 0x4489;

      deven = ((secbuf[4] << 24) | (secbuf[5] << 16)
         | (secbuf[6] << 8) | (secbuf[7]));
      dodd = deven >> 1;
      deven &= 0x55555555; dodd &= 0x55555555;

      mfmbuf[4] = dodd >> 16;
      mfmbuf[5] = dodd;
      mfmbuf[6] = deven>> 16;
      mfmbuf[7] = deven;

      for (i = 8; i < 48; i++)
    mfmbuf[i] = 0xaaaa;
      for (i = 0; i < 512; i += 4){
    deven = ((secbuf[i+32] << 24) | (secbuf[i+33] << 16)
       | (secbuf[i+34] << 8) | (secbuf[i+35]));
    dodd = deven >> 1;
    deven &= 0x55555555; dodd &= 0x55555555;
    mfmbuf[(i>>1) + 32] = dodd >> 16;
    mfmbuf[(i>>1) + 33] = dodd;
    mfmbuf[(i>>1) + 256 + 32] = deven >> 16;
    mfmbuf[(i>>1) + 256 + 33] = deven;
      }

      for(i = 4; i < 24; i += 2)
    hck ^= (mfmbuf[i] << 16) | mfmbuf[i+1];

      deven = dodd = hck; dodd >>= 1;
      mfmbuf[24] = dodd >> 16; mfmbuf[25] = dodd;
      mfmbuf[26] = deven >> 16; mfmbuf[27] = deven;

      for(i = 32; i < 544; i += 2)
    dck ^= (mfmbuf[i] << 16) | mfmbuf[i+1];

      deven = dodd = dck; dodd >>= 1;
      mfmbuf[28] = dodd >> 16; mfmbuf[29] = dodd;
      mfmbuf[30] = deven >> 16; mfmbuf[31] = deven;
  }
    } else {
  int i;
  drv->tracklen = drv->trackdata[tr].len/2 + 1;
  drv->bigmfmbuf[0] = drv->trackdata[tr].sync;
  emu_FileSeek(drv->diskfile, drv->trackdata[tr].offs, SEEK_SET);
  emu_FileRead((char *)(drv->bigmfmbuf + 1), drv->trackdata[tr].len, drv->diskfile);
  for (i = 0; i < drv->trackdata[tr].len/2; i++) {
      uae_u16 *mfm = drv->bigmfmbuf + i + 1;
      uae_u8 *data = (uae_u8 *)mfm;

      *mfm = 256 * *data + *(data+1);
  }
    }
    drv->buffered_side = side;
    drv->buffered_cyl = drv->cyl;
    drv->last_cycles = cycles;
    drv->mfmpos = 0;
}

/* Perform DMA for the time interval since the last call.  */
static int drive_update_reads (drive *drv, uaecptr *ptr, int *length, int *syncfound, int sync)
{
    unsigned long maxbytes = (cycles - drv->last_cycles) * 2 / FLOPPY_SPEED;
    unsigned long maxcount = maxbytes >> 1;
    drv->last_cycles += maxcount * FLOPPY_SPEED;

    drive_fill_bigbuf(drv);

    if (! ptr) {
  if (! drv->motoroff)
      drv->mfmpos += maxcount;
  drv->mfmpos %= drv->tracklen;
    } else {
  while (maxcount-- > 0 && *length > 0) {
      unsigned int word;
      if (drv->mfmpos > drv->tracklen) {
    write_log ("Bug in disk code - mfmpos too large\n");
    drv->mfmpos = 0;
      }
      word = drv->bigmfmbuf[drv->mfmpos];
      if (*syncfound) {
    put_word (*ptr, word);
    (*ptr) += 2;
    (*length)--;
      }
      if (word == sync)
    *syncfound = 1;
      if (! drv->motoroff)
    drv->mfmpos++;
      if (drv->mfmpos > drv->tracklen)
    drv->mfmpos = 0;
  }
    }
    return ! drv->motoroff && maxbytes >= 1;
}

/* We assume that drive_update_reads has already been called.  */
static void drive_get_data (drive *drv, uae_u16 *mfm, uae_u16 *byt)
{
    *mfm = drv->bigmfmbuf[drv->mfmpos];
    if (cycles - drv->last_cycles > (FLOPPY_SPEED / 2))
  *byt = *mfm & 0xff;
    else
  *byt = (*mfm >> 8) & 0xff;
}

#define MFMMASK 0x55555555
STATIC_INLINE uae_u32 getmfmlong (uae_u16* mbuf)
{
    return ((*mbuf << 16) | *(mbuf + 1)) & MFMMASK;
}

static void drive_write_data (drive *drv, uae_u16 *mbuf, int length)
{
#ifdef HAS_FLOPPYWRITE
    int i, secwritten = 0;
    uae_u32 odd, even, chksum, id, dlong;
    uae_u8* secdata;
    uae_u8 secbuf[544];
    uae_u16 *mend = mbuf + length;

    if (drive_writeprotected (drv))
  return;

    drive_fill_bigbuf (drv);
    mend -= (4 + 16 + 8 + 512);
    while (length > 0) {
  int trackoffs;

  do {
      while (*mbuf++ != 0x4489) {
    if (mbuf >= mend)
        return;
      }
  } while (*mbuf++ != 0x4489);

  odd = getmfmlong (mbuf);
  even = getmfmlong (mbuf + 2);
  mbuf += 4;
  id = (odd << 1) | even;

  trackoffs = (id & 0xff00) >> 8;
  if (trackoffs > 10) {
      //write_log ("Disk write: weird sector number %d\n", trackoffs);
      continue;
  }
  chksum = odd ^ even;
  for (i = 0; i < 4; i++) {
      odd = getmfmlong(mbuf);
      even = getmfmlong(mbuf+8);
      mbuf += 2;

      dlong = (odd << 1) | even;
      if (dlong)
    secwritten = -200;
      chksum ^= odd ^ even;
  }  /* could check here if the label is nonstandard */
  mbuf += 8;
  odd = getmfmlong (mbuf); even = getmfmlong (mbuf+2); mbuf += 4;
  if (((odd << 1) | even) != chksum
      || ((id & 0x00ff0000) >> 16) != drv->cyl*2 + side)
  {
      write_log ("Disk write: checksum error on sector header\n");
      continue;
  }
  odd = getmfmlong(mbuf); even = getmfmlong(mbuf+2); mbuf += 4;
  chksum = (odd << 1) | even;
  secdata = secbuf + 32;
  for (i=0; i<128; i++) {
      odd = getmfmlong(mbuf); even = getmfmlong(mbuf+256); mbuf += 2;
      dlong = (odd << 1) | even;
      *secdata++ = dlong >> 24; *secdata++ = (dlong >> 16) & 0xff;
      *secdata++ = dlong >> 8; *secdata++ = dlong;
      chksum ^= odd ^ even;
  }
  mbuf += 256;
  if (chksum) {
      write_log ("Disk write: data checksum error\n");
      continue;
  }
  secwritten++;
  emu_FileSeek (drv->diskfile, drv->trackdata[drv->cyl*2 + side].offs + trackoffs*512,
         SEEK_SET);
  emu_FileRead (secbuf+32, 512, drv->diskfile);
    }
    drv->buffered_side = 2; /* will force read */

    if (secwritten == 0)
  write_log ("Disk write in unsupported format\n");
    if (secwritten < 0)
  write_log ("Disk write: sector labels ignored\n");
#endif
}


static void drive_eject (drive *drv)
{
    if (! drive_empty (drv))
  emu_FileClose(drv->diskfile);

    drv->dskchange = 4;
    drv->dskchange_time = 20;
/*    printf("setting changed bit %d\n", drv-floppy);*/
    drv->diskfile = 0;
}

/* We use this function if we have no Kickstart ROM.
 * No error checking - we trust our luck. */
void DISK_ersatz_read (int tr, int sec, uaecptr dest)
{
    uae_u8 *dptr = get_real_address(dest);
    //emu_FileSeek (drv->diskfile, floppy[0].trackdata[tr].offs + sec*512, SEEK_SET);
    //emu_FileRead (dptr, 512, drv->diskfile);
}

void disk_eject (int num)
{
    //gui_filename(num, "");
    drive_eject(floppy + num);
    *currprefs.df[num] = *changed_prefs.df[num] = 0;
}

void disk_insert (int num, const char *name)
{
    /* just to be sure */
    drive_eject (floppy + num);
    drive_insert (floppy + num, num, name);
}

void DISK_check_change (void)
{
    int i;
    static int count = 0;
    count++;

    for (i = 0; i < NB_DRIVE; i++) {
  if (strcmp (currprefs.df[i], changed_prefs.df[i]) != 0) {
      if (currprefs.df[i][0] != '\0') {
    drive_eject(floppy + i);
    currprefs.df[i][0] = '\0';
    //gui_filename(i, "");
      } else if (floppy[i].dskchange == 1) {
    /* In theory, it should work without the dskchange test.
     * In practice, it doesn't. */
    drive_insert (floppy + i, i, changed_prefs.df[i]);
      } else if (floppy[i].dskchange > 1 && floppy[i].dskchange_time > 0) {
    /* Force the dskchange bit to go to 1 after a given timeout */
    if (--floppy[i].dskchange_time == 0) {
        floppy[i].dskchange = 1;
    }
      }
  }
    }
}

int disk_empty (int num)
{
    return drive_empty (floppy + num);
}

void DISK_init (void)
{
    int i;
    for (i = 0; i < NB_DRIVE; i++)
  if (!drive_insert (floppy+i, i, currprefs.df[i]))
      disk_eject (i);

    if (disk_empty (0))
  write_log ("No disk in drive 0.\n");
}

static int ledstate[] = { 0,0,0,0 };

void DISK_select (uae_u8 data)
{
    int step_pulse;
    int dr;

    if (selected != ((data >> 3) & 15))
  dskready = 0;
    selected = (data >> 3) & 15;
    side = 1 - ((data >> 2) & 1);

    direction = (data >> 1) & 1;
    step_pulse = data & 1;
    if (step != step_pulse) {
  step = step_pulse;
  if (step == 0){
      for (dr = 0; dr < NB_DRIVE; dr++){
    if (!(selected & (1 << dr))) {
        drive_step (floppy + dr);
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
  if (state == ledstate[dr])
      continue;
  //gui_led (dr + 1, state);
  //ledstate[dr] = state;
  //gui_ledstate &= ~(1 << (dr + 1));
  //gui_ledstate |= state << (dr + 1);
    }
}

uae_u8 DISK_status (void)
{
    uae_u8 st = 0x3c;
    int dr;

    for (dr = 0; dr < NB_DRIVE; dr++){
  drive *drv = floppy + dr;
  if (!(selected & (1 << dr))) {
      if (drive_running(drv)){
    if (dskready) st &= ~0x20;
    dskready = 1;
      } else {
    st &= ~0x20; /* report drive ID */
      }

      if (drive_track0 (drv)) { st &= ~0x10; }
      if (drive_writeprotected (drv)) { st &= ~8; }
      if (drv->dskchange) {
    /*printf("changed bit set: %d\n",dr); */
    st &= ~0x4;
    if (drv->dskchange > 1)
        drv->dskchange--;
      }
  }
    }
    return st;
}

void DISK_GetData (uae_u16 *mfm, uae_u16 *byt)
{
    int dr;
    for (dr = 0; dr < NB_DRIVE; dr++){
  if (!(selected & (1<<dr))) {
      drive_get_data (floppy + dr, mfm, byt);
  }
    }
}


void DISK_InitWrite (void)
{
#ifdef HAS_FLOPPYWRITE
    mfmwrite = mfmwrbuffer;
#endif    
}

int DISK_update_reads (uaecptr *ptr, int *length, int *syncfound, int sync)
{
    int dr;
    for (dr = 0; dr < NB_DRIVE; dr++) {
  if (!(selected & (1<<dr))) {
      return drive_update_reads (floppy + dr, ptr, length, syncfound, sync);
  }
    }
    return 0;
}

void DISK_WriteData (int length)
{
#ifdef HAS_FLOPPYWRITE
    int dr;
    for (dr = 0; dr < NB_DRIVE; dr++) {
  if (!(selected & (1<<dr))) {
      drive_write_data(floppy + dr, mfmwrbuffer, length);
  }
    }
#endif    
}

void DISK_search_sync (int cycle_limit, unsigned int sync)
{
    int dr;
    for (dr = 0; dr < NB_DRIVE; dr++) {
  if (!(selected & (1 << dr))) {
      drive *drv = floppy + dr;
      unsigned long offs = drv->mfmpos;
      unsigned long missing = FLOPPY_SPEED - (cycles - drv->last_cycles);
      unsigned long count = 0;
      if (missing == 0) {
    write_log ("bug in disk code: missing == 0\n");
    return;
      }

      while (missing < cycle_limit) {
    count += missing;
    cycle_limit -= missing;
    missing = FLOPPY_SPEED;
    if (drv->bigmfmbuf[offs] == sync) {
        eventtab[ev_disksync].active = 1;
        eventtab[ev_disksync].oldcycles = cycles;
        eventtab[ev_disksync].evtime = cycles + count;
        events_schedule ();
    }
    offs++;
      }
  }
    }    
}

/* Called when DMA gets restarted, so that we don't do DMA for the
   parts of the track that went past us while DMA was off.  */
void DISK_reset_cycles (void)
{
    int dr;
    for (dr = 0; dr < NB_DRIVE; dr++) {
  drive *drv = floppy + dr;
  unsigned long count = (cycles - drv->last_cycles) / FLOPPY_SPEED;
  drv->mfmpos += count;
  drv->mfmpos %= drv->tracklen;
  drv->last_cycles += count * FLOPPY_SPEED;
    }
}

void DISK_StartRead ()
{
    DISK_reset_cycles ();
}

static int first = 0;

void disk_swap(const char * disk0, const char * disk1)
{
    emu_printf("disk swap");
    drive_eject(floppy + 0);
    if (first == 0)
    {
        first = 1;
        emu_printf(disk0);
        drive_insert(floppy + 0, 0, disk0);
    }
    else
    {
        first = 0;
        emu_printf(disk1);        
        drive_insert(floppy + 0, 0, disk1);
    }
}


