/*
** Nofrendo (c) 1998-2000 Matthew Conte (matt@conte.com)
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of version 2 of the GNU Library General 
** Public License as published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** nes_rom.c
**
** NES ROM loading/saving related functions
** $Id: nes_rom.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

/* TODO: make this a generic ROM loading routine */

#include <stdio.h>
#include <string.h>
#include "noftypes.h"
#include "nes_rom.h"
#include "nes_mmc.h"
#include "nes_ppu.h"
#include "nes.h"
#include "log.h"
#include "osd.h"

extern char *osd_getromdata();

/* Max length for displayed filename */
#define  ROM_DISP_MAXLEN   20


#define  ROM_FOURSCREEN    0x08
#define  ROM_TRAINER       0x04
#define  ROM_BATTERY       0x02
#define  ROM_MIRRORTYPE    0x01
#define  ROM_INES_MAGIC    "NES\x1A"

//ToDo: packed - JD
typedef struct inesheader_s
{
   uint8 ines_magic[4]    ;
   uint8 rom_banks        ;
   uint8 vrom_banks       ;
   uint8 rom_type         ;
   uint8 mapper_hinybble  ;
   uint8 reserved[8]      ;
} inesheader_t;


#define  TRAINER_OFFSET    0x1000
#define  TRAINER_LENGTH    0x200
#define  VRAM_LENGTH       0x2000

#define  ROM_BANK_LENGTH   0x4000
#define  VROM_BANK_LENGTH  0x2000

#define  SRAM_BANK_LENGTH  0x0400
#define  VRAM_BANK_LENGTH  0x2000





static int rom_loadrom(unsigned char **rom, rominfo_t *rominfo)
{
   ASSERT(rom);
   ASSERT(rominfo);
   /* Allocate ROM space, and load it up! */
   rominfo->rom=*rom;
   *rom+=ROM_BANK_LENGTH*rominfo->rom_banks;

   /* If there's VROM, allocate and stuff it in */
   if (rominfo->vrom_banks)
   {
      rominfo->vrom=*rom;
      *rom+=VROM_BANK_LENGTH*rominfo->vrom_banks;
   }
   else
   {
      rominfo->vram = emu_Malloc(VRAM_LENGTH);
      if (NULL == rominfo->vram)
      {
         return -1;
      }
      memset(rominfo->vram, 0, VRAM_LENGTH);
   }
   return 0;
}


static int *rom_findrom(const char *filename, rominfo_t *rominfo)
{
   int fp;

   ASSERT(rominfo);

   if (NULL == filename)
      return NULL;

   /* Make a copy of the name so we can extend it */
   osd_fullname(rominfo->filename, filename);

   fp = emu_FileOpen(rominfo->filename);
   if (!fp)
   {
      /* Didn't find the file?  Maybe the .NES extension was omitted */
      if (NULL == strrchr(rominfo->filename, '.'))
         strncat(rominfo->filename, ".nes", PATH_MAX - strlen(rominfo->filename));

      /* this will either return NULL or a valid file pointer */
      fp = emu_FileOpen(rominfo->filename);
   }

   return fp;
}

/* Add ROM name to a list with dirty headers */
static int rom_adddirty(char *filename)
{
   return 0;
}

/* return 0 if this *is* an iNES file */
int rom_checkmagic(const char *filename)
{
   inesheader_t head;
   rominfo_t rominfo;
   int fp;

   fp = rom_findrom(filename, &rominfo);
   if (0 == fp)
      return -1;

   emu_FileRead(&head, 1*sizeof(head));

   emu_FileClose();

   if (0 == memcmp(head.ines_magic, ROM_INES_MAGIC, 4))
      /* not an iNES file */
      return 0;

   return -1;
}



static int rom_getheader(unsigned char **rom, rominfo_t *rominfo)
{
#define  RESERVED_LENGTH   8
   inesheader_t head;
   uint8 reserved[RESERVED_LENGTH];
   bool header_dirty;

   ASSERT(rom);
   ASSERT(*rom);
   ASSERT(rominfo);

   /* Read in the header */
//   _fread(&head, 1, sizeof(head), fp);
	log_printf("Head: %p (%x %x %x %x)\n", *rom, (*rom)[0], (*rom)[1], (*rom)[2], (*rom)[3]);
	memcpy(&head, *rom, sizeof(head));
	*rom+=sizeof(head);

   if (memcmp(head.ines_magic, ROM_INES_MAGIC, 4))
   {
      return -1;
   }

   rominfo->rom_banks = head.rom_banks;
   rominfo->vrom_banks = head.vrom_banks;
   /* iNES assumptions */
   rominfo->sram_banks = 8; /* 1kB banks, so 8KB */
   rominfo->vram_banks = 1; /* 8kB banks, so 8KB */
   rominfo->mirror = (head.rom_type & ROM_MIRRORTYPE) ? MIRROR_VERT : MIRROR_HORIZ;
   rominfo->flags = 0;
   if (head.rom_type & ROM_BATTERY)
      rominfo->flags |= ROM_FLAG_BATTERY;
   if (head.rom_type & ROM_TRAINER)
      rominfo->flags |= ROM_FLAG_TRAINER;
   if (head.rom_type & ROM_FOURSCREEN)
      rominfo->flags |= ROM_FLAG_FOURSCREEN;
   /* TODO: fourscreen a mirroring type? */
   rominfo->mapper_number = head.rom_type >> 4;

   /* Do a compare - see if we've got a clean extended header */
   memset(reserved, 0, RESERVED_LENGTH);
   if (0 == memcmp(head.reserved, reserved, RESERVED_LENGTH))
   {
      /* We were clean */
      header_dirty = false;
      rominfo->mapper_number |= (head.mapper_hinybble & 0xF0);
   }
   else
   {
      header_dirty = true;

      /* @!?#@! DiskDude. */
      if (('D' == head.mapper_hinybble) && (0 == memcmp(head.reserved, "iskDude!", 8)))
         log_printf("`DiskDude!' found in ROM header, ignoring high mapper nybble\n");
      else
      {
         log_printf("ROM header dirty, possible problem\n");
         rominfo->mapper_number |= (head.mapper_hinybble & 0xF0);
      }

      rom_adddirty(rominfo->filename);
   }

   /* TODO: this is an ugly hack, but necessary, I guess */
   /* Check for VS unisystem mapper */
   if (99 == rominfo->mapper_number)
      rominfo->flags |= ROM_FLAG_VERSUS;

   return 0;
}

/* Build the info string for ROM display */
char *rom_getinfo(rominfo_t *rominfo)
{
   static char info[PATH_MAX + 1];
   char romname[PATH_MAX + 1], temp[PATH_MAX + 1];

   /* Look to see if we were given a path along with filename */
   /* TODO: strip extensions */
   if (strrchr(rominfo->filename, PATH_SEP))
      strncpy(romname, strrchr(rominfo->filename, PATH_SEP) + 1, PATH_MAX);
   else
      strncpy(romname, rominfo->filename, PATH_MAX);

   /* If our filename is too long, truncate our displayed filename */
   if (strlen(romname) > ROM_DISP_MAXLEN)
   {
      strncpy(info, romname, ROM_DISP_MAXLEN - 3);
      strcpy(info + (ROM_DISP_MAXLEN - 3), "...");
   }
   else
   {
      strcpy(info, romname);
   }

   sprintf(temp, " [%d] %dk/%dk %c", rominfo->mapper_number,
           rominfo->rom_banks * 16, rominfo->vrom_banks * 8,
           (rominfo->mirror == MIRROR_VERT) ? 'V' : 'H');
   
   /* Stick it on there! */
   strncat(info, temp, PATH_MAX - strlen(info));

   if (rominfo->flags & ROM_FLAG_BATTERY)
      strncat(info, "B", PATH_MAX - strlen(info));
   if (rominfo->flags & ROM_FLAG_TRAINER)
      strncat(info, "T", PATH_MAX - strlen(info));
   if (rominfo->flags & ROM_FLAG_FOURSCREEN)
      strncat(info, "4", PATH_MAX - strlen(info));

   return info;
}

/* Load a ROM image into memory */
rominfo_t *rom_load(const char *filename)
{
   unsigned char *rom=(unsigned char*)osd_getromdata();
   rominfo_t *rominfo;

   rominfo = emu_Malloc(sizeof(rominfo_t));
   if (NULL == rominfo)
      return NULL;

   memset(rominfo, 0, sizeof(rominfo_t));

   /* Get the header and stick it into rominfo struct */
	if (rom_getheader(&rom, rominfo))
      goto _fail;

   /* Make sure we really support the mapper */
   if (false == mmc_peek(rominfo->mapper_number))
   {
      goto _fail;
   }

	if (rom_loadrom(&rom, rominfo))
      goto _fail;

   return rominfo;

_fail:
   rom_free(&rominfo);
   return NULL;
}

/* Free a ROM */
void rom_free(rominfo_t **rominfo)
{
   if (NULL == *rominfo)
   {
      return;
   }

   /* Restore palette if we loaded in a VS jobber */
   if ((*rominfo)->flags & ROM_FLAG_VERSUS)
   {
      /* TODO: bad idea calling nes_getcontextptr... */
      ppu_setdefaultpal(nes_getcontextptr()->ppu);
      log_printf("Default NES palette restored\n");
   }


   if ((*rominfo)->sram)
      free((*rominfo)->sram);
   if ((*rominfo)->rom)
      free((*rominfo)->rom);
   if ((*rominfo)->vrom)
      free((*rominfo)->vrom);
   if ((*rominfo)->vram)
      free((*rominfo)->vram);

   free(*rominfo);
}

/*
** $Log: nes_rom.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.8  2000/11/21 13:28:40  matt
** take care to zero allocated mem
**
** Revision 1.7  2000/11/09 14:07:28  matt
** state load fixed, state save mostly fixed
**
** Revision 1.6  2000/10/28 14:24:54  matt
** where did I put that underscore?
**
** Revision 1.5  2000/10/27 12:56:35  matt
** api change for ppu palette functions
**
** Revision 1.4  2000/10/26 22:51:44  matt
** correct NULL filename handling
**
** Revision 1.3  2000/10/25 01:23:08  matt
** basic system autodetection
**
** Revision 1.2  2000/10/25 00:23:16  matt
** makefiles updated for new directory structure
**
** Revision 1.1  2000/10/24 12:20:28  matt
** changed directory structure
**
** Revision 1.19  2000/10/21 14:35:58  matt
** typo
**
** Revision 1.18  2000/10/17 03:22:37  matt
** cleaning up rom module
**
** Revision 1.17  2000/10/10 13:58:13  matt
** stroustrup squeezing his way in the door
**
** Revision 1.16  2000/10/10 13:03:54  matt
** Mr. Clean makes a guest appearance
**
** Revision 1.15  2000/07/31 04:28:46  matt
** one million cleanups
**
** Revision 1.14  2000/07/30 04:31:26  matt
** automagic loading of the nofrendo intro
**
** Revision 1.13  2000/07/25 02:20:58  matt
** cleanups
**
** Revision 1.12  2000/07/20 01:53:27  matt
** snprintf() ain't no standard function, eh?
**
** Revision 1.11  2000/07/19 16:06:54  neil
** little error fixed (tempinfo vs rominfo->info)
**
** Revision 1.10  2000/07/19 15:59:39  neil
** PATH_MAX, strncpy, snprintf, and strncat are our friends
**
** Revision 1.9  2000/07/17 01:52:27  matt
** made sure last line of all source files is a newline
**
** Revision 1.8  2000/07/06 16:47:50  matt
** new ppu palette setting calls
**
** Revision 1.7  2000/07/05 23:21:54  neil
** fclose(fp) should not be done if fp == NULL
**
** Revision 1.6  2000/07/04 04:45:14  matt
** changed include
**
** Revision 1.5  2000/06/26 04:56:10  matt
** minor cleanup
**
** Revision 1.4  2000/06/09 15:12:25  matt
** initial revision
**
*/
