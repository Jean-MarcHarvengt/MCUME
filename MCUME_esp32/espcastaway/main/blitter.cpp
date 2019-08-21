/*
* Castaway
*  (C) 1994 - 2002 Martin Doering, Joachim Hoenig
*
* blitter.c - ST blitter chip emulation
*
* This file is distributed under the GPL, version 2 or at your
* option any later version.  See doc/license.txt for details.
*
* revision history
*  23.05.2002  JH  FAST1.0.1 code import: KR -> ANSI, restructuring
*  09.06.2002  JH  Renamed io.c to st.c again (io.h conflicts with system headers)
*  02.10.2002  JH  use uint16 etc.
*/
static char     sccsid[] = "$Id: blitter.c,v 1.3 2002/10/02 22:44:51 jhoenig Exp $";
#include <stdio.h>
#include "config.h"
#include "st.h"
#include "mem.h"
#include "m68k_intrf.h"

#define FXSR    0x80
#define NFSR    0x40
#define SKEW    0x0f
#define BUSY    0x80
#define HOG     0x40
#define SMUDGE  0x20
#define LINENO  0x0f

void            bitblt(void)
{
    uint32   blt_src_in;
    uint16   blt_src_out, blt_hop_out, blt_dst_in, blt_dst_out, mask_out;
    int      xc, yc, lineno, last, first;
#if (VERBOSE & 0x8)
    DBG_OUT ("bitblt: Start\n");
    DBG_OUT ("HALFT[] 0x%04x-%04x-%04x-%04x\n", (uint16) blt_halftone[0], blt_halftone[1], blt_halftone[2], blt_halftone[3]);
    DBG_OUT ("Y COUNT 0x%04x\n", (uint16) blt_y_cnt);
    DBG_OUT ("X COUNT 0x%04x\n", (uint16) blt_x_cnt);
    DBG_OUT ("X S INC 0x%04x\n", (uint16) blt_src_x_inc);
    DBG_OUT ("Y S INC 0x%04x\n", (uint16) blt_src_y_inc);
    DBG_OUT ("X D INC 0x%04x\n", (uint16) blt_dst_x_inc);
    DBG_OUT ("Y D INC 0x%04x\n", (uint16) blt_dst_y_inc);
    DBG_OUT ("ENDMASK 0x%04x-%04x-%04x\n", (uint16) blt_end_1, (uint16) blt_end_2, (uint16) blt_end_3);
    DBG_OUT ("S_ADDR  0x%08lx\n", blt_src_addr);
    DBG_OUT ("D_ADDR  0x%08lx\n", blt_dst_addr);
    DBG_OUT ("HOP=%01d, OP=%02d\n", blt_hop & 0x3, blt_op & 0xf);
    DBG_OUT ("HOPline=%02d\n", blt_status & 0xf);
    DBG_OUT ("NFSR=%d, FXSR=%d, SKEW=%02d\n", (blt_skew & NFSR) != 0,
		(blt_skew & FXSR) != 0,
		(blt_skew & SKEW));
#endif
    yc = (blt_y_cnt == 0) ? 65536 : blt_y_cnt;
    while (yc-- > 0) {
        xc = (blt_x_cnt == 0) ? 65536 : blt_x_cnt;
        first = 1;
        blt_src_in = 0;
        /* next line to get rid of obnoxious compiler warnings */
        blt_src_out = blt_hop_out = blt_dst_out = 0;
        while (xc-- > 0) {
            last = (xc == 0);
            if ((blt_hop & 0x03) >= 2) {
                /* read source into blt_src_in */
                if (blt_src_x_inc >= 0) {
                    if (first && (blt_skew & FXSR)) {
                        blt_src_in = (GetMemW (blt_src_addr) << 16);
                        blt_src_addr += blt_src_x_inc;
                    } else {
                        blt_src_in <<= 16;
                    }
                    if (last && (blt_skew & NFSR)) {
                        blt_src_addr -= blt_src_x_inc;
                    } else {
                        blt_src_in |= (uint16) GetMemW (blt_src_addr);
                        if (!last) {
                            blt_src_addr += blt_src_x_inc;
                        }
                    }
                } else {
                    if (first &&  (blt_skew & FXSR)) {
                        blt_src_in = (uint16) GetMemW (blt_src_addr);
                        blt_src_addr +=blt_src_x_inc;
                    } else {
                        blt_src_in >>= 16;
                    }
                    if (last && (blt_skew & NFSR)) {
                        blt_src_addr -= blt_src_x_inc;
                    } else {
                        blt_src_in |= (GetMemW (blt_src_addr) << 16);
                        if (!last) {
                            blt_src_addr += blt_src_x_inc;
                        }
                    }
                }
                /* shift blt_skew times into blt_src_out */
                blt_src_out = blt_src_in >> (blt_skew & SKEW);
#if (VERBOSE & 0x8)
                DBG_OUT ("%04x ", blt_src_out);
#endif
            }
            /* halftone OP */
            lineno = ((blt_status & SMUDGE) ? blt_src_out : blt_status) & LINENO;
            switch (blt_hop & 0x3) {
            case 0:
                blt_hop_out = 0xffff;
                break;
            case 1:
                blt_hop_out = blt_halftone[lineno];
                break;
            case 2:
                blt_hop_out = blt_src_out;
                break;
            case 3:
                blt_hop_out = blt_src_out & blt_halftone[lineno];
                break;
            }
            /* read destination into blt_dst_in */
            blt_dst_in = GetMemW (blt_dst_addr);
            /* op into blt_dst_out */
            switch (blt_op & 0xf) {
            case 0:
                blt_dst_out = 0;
                break;
            case 1:
                blt_dst_out = blt_hop_out & blt_dst_in;
                break;
            case 2:
                blt_dst_out = blt_hop_out & ~blt_dst_in;
                break;
            case 3:
                blt_dst_out = blt_hop_out;
                break;
            case 4:
                blt_dst_out = ~blt_hop_out & blt_dst_in;
                break;
            case 5:
                blt_dst_out = blt_dst_in;
                break;
            case 6:
                blt_dst_out = blt_hop_out ^ blt_dst_in;
                break;
            case 7:
                blt_dst_out = blt_hop_out | blt_dst_in;
                break;
            case 8:
                blt_dst_out = ~blt_hop_out & ~blt_dst_in;
                break;
            case 9:
                blt_dst_out = ~blt_hop_out ^ blt_dst_in;
                break;
            case 0xa:
                blt_dst_out = ~blt_dst_in;
                break;
            case 0xb:
                blt_dst_out = blt_hop_out | ~blt_dst_in;
                break;
            case 0xc:
                blt_dst_out = ~blt_hop_out;
                break;
            case 0xd:
                blt_dst_out = ~blt_hop_out | blt_dst_in;
                break;
            case 0xe:
                blt_dst_out = ~blt_hop_out | ~blt_dst_in;
                break;
            case 0xf:
                blt_dst_out = 0xffff;
                break;
            }
            /* and endmask */
            if (first) {
                mask_out = (blt_dst_out & blt_end_1) | (blt_dst_in & ~blt_end_1);
            } else if (last) {
                mask_out = (blt_dst_out & blt_end_3) | (blt_dst_in & ~blt_end_3);
            } else {
                mask_out = (blt_dst_out & blt_end_2) | (blt_dst_in & ~blt_end_2);
            }
            SetMemW (blt_dst_addr, mask_out);
            if (!last) {
                blt_dst_addr += blt_dst_x_inc;
            }
            first = 0;
        }
#if (VERBOSE & 0x8)
        DBG_OUT ("\n");
#endif
        blt_status = (blt_status + ((blt_dst_y_inc >= 0) ? 1 : 15)) & 0xef;
        blt_src_addr += blt_src_y_inc;
        blt_dst_addr += blt_dst_y_inc;
    }
    /* blt_status &= ~BUSY; */
    blt_y_cnt = 0;
#if (VERBOSE & 0x8)
    DBG_OUT ("bitblt: End\n");
    DBG_OUT ("HALFT[] 0x%04x-%04x-%04x-%04x\n", (uint16) blt_halftone[0], blt_halftone[1], blt_halftone[2], blt_halftone[3]);
    DBG_OUT ("Y COUNT 0x%04x\n", (uint16) blt_y_cnt);
    DBG_OUT ("X COUNT 0x%04x\n", (uint16) blt_x_cnt);
    DBG_OUT ("X S INC 0x%04x\n", (uint16) blt_src_x_inc);
    DBG_OUT ("Y S INC 0x%04x\n", (uint16) blt_src_y_inc);
    DBG_OUT ("X D INC 0x%04x\n", (uint16) blt_dst_x_inc);
    DBG_OUT ("Y D INC 0x%04x\n", (uint16) blt_dst_y_inc);
    DBG_OUT ("ENDMASK 0x%04x-%04x-%04x\n", (uint16) blt_end_1, (uint16) blt_end_2, (uint16) blt_end_3);
    DBG_OUT ("S_ADDR  0x%08lx\n", blt_src_addr);
    DBG_OUT ("D_ADDR  0x%08lx\n", blt_dst_addr);
    DBG_OUT ("HOP=%01d, OP=%02d\n", blt_hop & 0x3, blt_op & 0xf);
    DBG_OUT ("HOPline=%02d\n", blt_status & 0xf);
    DBG_OUT ("NFSR=%d, FXSR=%d, SKEW=%02d\n", (blt_skew & NFSR) != 0,
		(blt_skew & FXSR) != 0,
		(blt_skew & SKEW));
#endif
}
