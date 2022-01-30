 /*
  * UAE - The Un*x Amiga Emulator
  *
  * AutoConfig devices
  *
  * (c) 1995 Bernd Schmidt
  */

#include "shared.h"

#ifdef HAS_HARDFILE

#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "disk.h"
#include "xwin.h"
#include "autoconf.h"
#include "execlib.h"

static int opencount = 0;
static long hardfile_size = 0;
int numtracks = 512;
static int fhdd;

static uae_u32 hardfile_open(void)
{
    uaecptr tmp1 = m68k_areg(regs, 1); /* IOReq */

    /* Check unit number */
    if (m68k_dreg(regs, 0) == 0) {
	opencount++;
	put_word (m68k_areg(regs, 6)+32, get_word (m68k_areg(regs, 6)+32) + 1);
	put_long (tmp1+24, 0); /* io_Unit */
	put_byte (tmp1+31, 0); /* io_Error */
	put_byte (tmp1+8, 7); /* ln_type = NT_REPLYMSG */
	return 0;
    }

    put_long (tmp1+20, (uae_u32)-1);
    put_byte (tmp1+31, (uae_u8)-1);
    return (uae_u32)-1;
}

static uae_u32 hardfile_close(void)
{
    opencount--;
    put_word (m68k_areg(regs, 6)+32, get_word (m68k_areg(regs, 6)+32) - 1);

    return 0;
}

static uae_u32 hardfile_expunge(void)
{
    return 0; /* Simply ignore this one... */
}

static uae_u32 hardfile_beginio(void)
{
    uae_u32 tmp1, tmp2, dataptr, offset;
    uae_u32 retval = m68k_dreg(regs, 0);

    tmp1 = m68k_areg(regs, 1);
#define DEBUGME
#ifdef DEBUGME
    printf ("hardfile: tmp1 = %08lx\n", (unsigned long)tmp1);
    printf ("hardfile: cmd  = %d\n", (int)get_word(tmp1+28));
#endif
    
    put_byte (tmp1+8, NT_MESSAGE);
    put_byte (tmp1+31, 0); /* no error yet */
    tmp2 = get_word (tmp1+28); /* io_Command */
/*    put_byte (tmp1 + 30, get_byte (tmp1 + 30) & ~1);*/
    switch (tmp2) {
     case CMD_READ:
	dataptr = get_long (tmp1 + 40);
	if (dataptr & 1)
	    goto bad_command;
	offset = get_long (tmp1 + 44);
	if (offset & 511)
	    goto bad_command;
	tmp2 = get_long (tmp1 + 36); /* io_Length */
	if (tmp2 & 511)
	    goto bad_command;
	if (tmp2 + offset > (uae_u32)numtracks * 32 * 512)
	    goto bad_command;

	put_long (tmp1 + 32, tmp2); /* set io_Actual */
	emu_FileSeek (fhdd, offset, SEEK_SET);
	while (tmp2) {
	    int i;
	    char buffer[512];
	    emu_FileRead (buffer, 512, fhdd);
	    for (i = 0; i < 512; i++, dataptr++)
		put_byte(dataptr, buffer[i]);
	    tmp2 -= 512;
	}
	break;

     case CMD_WRITE:
     case 11: /* Format */
	dataptr = get_long (tmp1 + 40);
	if (dataptr & 1)
	    goto bad_command;
	offset = get_long (tmp1 + 44);
	if (offset & 511)
	    goto bad_command;
	tmp2 = get_long (tmp1 + 36); /* io_Length */
	if (tmp2 & 511)
	    goto bad_command;
	if (tmp2 + offset > (uae_u32)numtracks * 32 * 512)
	    goto bad_command;

	put_long (tmp1 + 32, tmp2); /* set io_Actual */
	emu_FileSeek (fhdd, offset, SEEK_SET);
	while (tmp2) {
	    char buffer[512];
	    int i;
	    for (i=0; i < 512; i++, dataptr++)
		buffer[i] = get_byte(dataptr);
	    // JMH not supported : write (uaedevfd, buffer, 512);
	    tmp2 -= 512;
	}
	break;

	bad_command:
	break;

     case 18: /* GetDriveType */
	put_long (tmp1 + 32, 1); /* not exactly a 3.5" drive, but... */
	break;

     case 19: /* GetNumTracks */
	put_long (tmp1 + 32, numtracks);
	break;

	/* Some commands that just do nothing and return zero */
     case CMD_UPDATE:
     case CMD_CLEAR:
     case 9: /* Motor */
     case 10: /* Seek */
     case 12: /* Remove */
     case 13: /* ChangeNum */
     case 14: /* ChangeStatus */
     case 15: /* ProtStatus */
     case 20: /* AddChangeInt */
     case 21: /* RemChangeInt */
	put_long (tmp1+32, 0); /* io_Actual */
	retval = 0;
	break;

     default:
	/* Command not understood. */
	put_byte (tmp1+31, (uae_u8)-3); /* io_Error */
	retval = 0;
	break;
    }
#if 0
    if ((get_byte (tmp1+30) & 1) == 0) {
	/* Not IOF_QUICK -- need to ReplyMsg */
	m68k_areg(regs, 1) = tmp1;
	CallLib (get_long(4), -378);
    }
#endif
    return retval;
}

static uae_u32 hardfile_abortio(void)
{
    return (uae_u32)-3;
}


void hardfile_install(void)
{
    uae_u32 functable, datatable, inittable;
    uae_u32 initcode, openfunc, closefunc, expungefunc;
    uae_u32 beginiofunc, abortiofunc;

    emu_printf("looking hdd");
    hardfile_size = emu_FileSize(currprefs.hf0);
    //if (hardfile_size == 0) return;
    fhdd = emu_FileOpen(currprefs.hf0, "r+b");

    emu_printi(hardfile_size);
    

    ROM_hardfile_resname = ds("hardfile.device");
    ROM_hardfile_resid = ds("UAE hardfile.device 0.2");

    numtracks = hardfile_size / 16384;

    /* initcode */
#if 0
    initcode = here();
    calltrap(deftrap(hardfile_init)); dw(RTS);
#else
    initcode = filesys_initcode;
#endif
    /* Open */
    openfunc = here();
    calltrap(deftrap(hardfile_open)); dw(RTS);

    /* Close */
    closefunc = here();
    calltrap(deftrap(hardfile_close)); dw(RTS);

    /* Expunge */
    expungefunc = here();
    calltrap(deftrap(hardfile_expunge)); dw(RTS);

    /* BeginIO */
    beginiofunc = here();
    calltrap(deftrap(hardfile_beginio));
    dw (0x48E7); dw (0x8002); /* movem.l d0/a6,-(a7) */
    dw (0x0829); dw (0); dw (30); /* btst #0,30(a1) */
    dw (0x6608); /* bne.b +8 */
    dw (0x2C78); dw (0x0004); /* move.l 4,a6 */
    dw (0x4EAE); dw (-378); /* jsr ReplyMsg(a6) */
    dw (0x4CDF); dw (0x4001); /* movem.l (a7)+,d0/a6 */
    dw(RTS);

    /* AbortIO */
    abortiofunc = here();
    calltrap(deftrap(hardfile_abortio)); dw(RTS);

    /* FuncTable */
    functable = here();
    dl(openfunc); /* Open */
    dl(closefunc); /* Close */
    dl(expungefunc); /* Expunge */
    dl(EXPANSION_nullfunc); /* Null */
    dl(beginiofunc); /* BeginIO */
    dl(abortiofunc); /* AbortIO */
    dl(0xFFFFFFFFul); /* end of table */

    /* DataTable */
    datatable = here();
    dw(0xE000); /* INITuae_s8 */
    dw(0x0008); /* LN_TYPE */
    dw(0x0300); /* NT_DEVICE */
    dw(0xC000); /* INITuae_s32 */
    dw(0x000A); /* LN_NAME */
    dl(ROM_hardfile_resname);
    dw(0xE000); /* INITuae_s8 */
    dw(0x000E); /* LIB_FLAGS */
    dw(0x0600); /* LIBF_SUMUSED | LIBF_CHANGED */
    dw(0xD000); /* INITuae_s16 */
    dw(0x0014); /* LIB_VERSION */
    dw(0x0004); /* 0.4 */
    dw(0xD000);
    dw(0x0016); /* LIB_REVISION */
    dw(0x0000);
    dw(0xC000);
    dw(0x0018); /* LIB_IDSTRING */
    dl(ROM_hardfile_resid);
    dw(0x0000); /* end of table */

    ROM_hardfile_init = here();
    dl(0x00000100); /* ??? */
    dl(functable);
    dl(datatable);
    dl(initcode);

    if (hardfile_size > 0) {
	   uaecptr initfunc;
	   add_filesys_unit(NULL, "hardfile", 1);
     emu_printf("HDD installed");
    }
}



#endif

