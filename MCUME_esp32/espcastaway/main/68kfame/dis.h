
static char   *disass_cc(unsigned char cc)
{
    switch(cc) {
    case 0:
        return "T ";
    case 1:
        return "F ";
    case 2:
        return "HI";
    case 3:
        return "LS";
    case 4:
        return "CC";
    case 5:
        return "CS";
    case 6:
        return "NE";
    case 7:
        return "EQ";
    case 8:
        return "VC";
    case 9:
        return "VS";
    case 0xa:
        return "PL";
    case 0xb:
        return "MI";
    case 0xc:
        return "GE";
    case 0xd:
        return "LT";
    case 0xe:
        return "GT";
    case 0xf:
        return "LE";
    default:
        return "??";
    }
}

static char   *disass_areg(unsigned char spec)
{
    switch (spec) {
    case 0: return "A0";
    case 1: return "A1";
    case 2: return "A2";
    case 3: return "A3";
    case 4: return "A4";
    case 5: return "A5";
    case 6: return "A6";
    case 7: return "A7";
    default: return "??";
    }
}

static char   *disass_dreg(unsigned char spec)
{
    switch (spec) {
    case 0: return "D0";
    case 1: return "D1";
    case 2: return "D2";
    case 3: return "D3";
    case 4: return "D4";
    case 5: return "D5";
    case 6: return "D6";
    case 7: return "D7";
    default: return "??";
    }
}

static char   *disass_reg(unsigned char spec)
{
    if (spec >= 8) return disass_areg(spec & 0x7);
    else return disass_dreg(spec);
}

static void    disass_reglist(char *buf, unsigned short reglist, int predecrement)
{
    int             index = 0, len;
    *buf = 0;
    if (!predecrement) {
        while (reglist) {
            if (reglist & 0x1) {
                strcat(buf, disass_reg(index));
                strcat(buf, "/");
            }
            index++;
            reglist = reglist >> 1;
        }
    } else {
        while (reglist) {
            if (reglist & 0x8000) {
                strcat(buf, disass_reg(index));
                strcat(buf, "/");
            }
            index++;
            reglist = reglist << 1;
        }
    }
    /* remove trailing / */
    len = strlen(buf);
    if (len > 0) *(buf + len - 1) = 0;
}

static int     disass_count(unsigned char spec)
{
    if (spec == 0) return 8;
    else return spec;
}

/*
* return # of extension words used for addressing
*/
static int     disass_displacement(char *buf, unsigned short *inst_stream, char displacement)
{
    if (displacement == 0) {
        sprintf(buf, "%04x", inst_stream[1]);
        return 1;
    } else if (displacement == -1) {
        sprintf(buf, "%04x%04x", inst_stream[1], inst_stream[2]);
        return 2;
    } else {
        sprintf(buf, "%02x", displacement);
        return 0;
    }
}

/*
* return # of extension words used for addressing
*/
static int     disass_ea(char *buf, unsigned short *inst_stream, unsigned char mode, unsigned char spec, unsigned char size)
{
    switch(mode) {
    case 0:
        strcpy(buf, disass_dreg(spec));
        return 0;
    case 1:
        strcpy(buf, disass_areg(spec));
        return 0;
    case 2:
        sprintf(buf, "(%s)", disass_areg(spec));
        return 0;
    case 3:
        sprintf(buf, "(%s)+", disass_areg(spec));
        return 0;
    case 4:
        sprintf(buf, "-(%s)", disass_areg(spec));
        return 0;
    case 5:
        sprintf(buf, "%04x(%s)", inst_stream[1], disass_areg(spec));
        return 1;
    case 6: // TODO: 680x0 full extension format
        sprintf(buf, "(%02x(%s,%s.%c*%x)",
            (char) inst_stream[1], disass_areg(spec),
            disass_reg(inst_stream[1] >> 12),
            (inst_stream[1] & 0x0800)?'L':'W',
            1 << ((inst_stream[1] & 0x0600) >> 9));
        return 1;
    case 7:
        switch(spec) {
        case 0:
            sprintf(buf, "%04x", inst_stream[1]);
            return 1;
        case 1:
            sprintf(buf, "%04x%04x", inst_stream[1], inst_stream[2]);
            return 2;
        case 2:
            sprintf(buf, "%04x(PC)", inst_stream[1]);
            return 1;
        case 3: // TODO: 680x0 full extension format
            sprintf(buf, "(%02x(PC,%s.%c*%x)",
                (char) inst_stream[1],
                disass_reg(inst_stream[1] >> 12),
                (inst_stream[1] & 0x0800)?'L':'W',
                1 << ((inst_stream[1] & 0x0600) >> 9));
            return 1;
        case 4:
            switch(size) {
            case 0:
            case 1:
                sprintf(buf, "#%04x", inst_stream[1]);
                return 1;
            case 2:
                sprintf(buf, "#%04x%04x", inst_stream[1], inst_stream[2]);
                return 2;
            case 3:
                sprintf(buf, "???");
                return 0;
            }
        }
    }
    sprintf (buf, "???");
    return 0;
}

void    disassemble68k(char *buf, unsigned short *inst_stream)
{
    char    src_ea_buf[80];
    char    tgt_ea_buf[80];
    unsigned char src_mode = (inst_stream[0] & 0x0038) >> 3;
    unsigned char src_spec = (inst_stream[0] & 0x0007);
    unsigned char src_size = (inst_stream[0] & 0x00c0) >> 6;
    unsigned char tgt_mode = (inst_stream[0] & 0x01c0) >> 6;
    unsigned char tgt_spec = (inst_stream[0] & 0x0e00) >> 9;
    unsigned char condition = (inst_stream[0] & 0x0f00) >> 8;
    int wc;
    switch (inst_stream[0] & 0xf000) {
    case 0x0000:
        // TODO: CAS2, CAS
        disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, src_size);
        switch (inst_stream[0]) {
        case 0x003c:
            sprintf(buf, "ORI.B    #%02x,CCR", (char) inst_stream[1]);
            return;
        case 0x007c:
            sprintf(buf, "ORI.W    #%04x,SR", inst_stream[1]);
            return;
        case 0x013c:
            sprintf(buf, "ANDI.B   #%02x,CCR", (char) inst_stream[1]);
            return;
        case 0x017c:
            sprintf(buf, "ANDI.W   #%04x,SR", inst_stream[1]);
            return;
        case 0x0a3c:
            sprintf(buf, "EORI.B   #%02x,CCR", (char) inst_stream[1]);
            return;
        case 0x0a7c:
            sprintf(buf, "EORI.W   #%04x,SR", inst_stream[1]);
            return;
        }
        switch (inst_stream[0] & 0x0fc0) {
        case 0x0000:
            sprintf(buf, "ORI.B    #%02x,%s", (char) inst_stream[1], src_ea_buf);
            return;
        case 0x0040:
            sprintf(buf, "ORI.W    #%04x,%s", inst_stream[1], src_ea_buf);
            return;
        case 0x0080:
            sprintf(buf, "ORI.L    #%04x%04x,%s", inst_stream[1], inst_stream[2], src_ea_buf);
            return;
        case 0x0200:
            sprintf(buf, "ANDI.B   #%02x,%s", (char) inst_stream[1], src_ea_buf);
            return;
        case 0x0240:
            sprintf(buf, "ANDI.W   #%04x,%s", inst_stream[1], src_ea_buf);
            return;
        case 0x0280:
            sprintf(buf, "ANDI.L   #%04x%04x,%s", inst_stream[1], inst_stream[2], src_ea_buf);
            return;
        case 0x0400:
            sprintf(buf, "SUBI.B   #%02x,%s", (char) inst_stream[1], src_ea_buf);
            return;
        case 0x0440:
            sprintf(buf, "SUBI.W   #%04x,%s", inst_stream[1], src_ea_buf);
            return;
        case 0x0480:
            sprintf(buf, "SUBI.L   #%04x%04x,%s", inst_stream[1], inst_stream[2], src_ea_buf);
            return;
        case 0x0600:
            sprintf(buf, "ADDI.B   #%02x,%s", (char) inst_stream[1], src_ea_buf);
            return;
        case 0x0640:
            sprintf(buf, "ADDI.W   #%04x,%s", inst_stream[1], src_ea_buf);
            return;
        case 0x0680:
            sprintf(buf, "ADDI.L   #%04x%04x,%s", inst_stream[1], inst_stream[2], src_ea_buf);
            return;
        case 0x0800:
            sprintf(buf, "BTST     #%02x,%s", (char) inst_stream[1], src_ea_buf);
            return;
        case 0x0840:
            sprintf(buf, "BCHG     #%02x,%s", (char) inst_stream[1], src_ea_buf);
            return;
        case 0x0880:
            sprintf(buf, "BCLR     #%02x,%s", (char) inst_stream[1], src_ea_buf);
            return;
        case 0x08c0:
            sprintf(buf, "BSET     #%02x,%s", (char) inst_stream[1], src_ea_buf);
            return;
        case 0x0a00:
            sprintf(buf, "EORI.B   #%02x,%s", (char) inst_stream[1], src_ea_buf);
            return;
        case 0x0a40:
            sprintf(buf, "EORI.W   #%04x,%s", inst_stream[1], src_ea_buf);
            return;
        case 0x0a80:
            sprintf(buf, "EORI.L   #%04x%04x,%s", inst_stream[1], inst_stream[2], src_ea_buf);
            return;
        case 0x0c00:
            sprintf(buf, "CMPI.B   #%02x,%s", (char) inst_stream[1], src_ea_buf);
            return;
        case 0x0c40:
            sprintf(buf, "CMPI.W   #%04x,%s", inst_stream[1], src_ea_buf);
            return;
        case 0x0c80:
            sprintf(buf, "CMPI.L   #%04x%04x,%s", inst_stream[1], inst_stream[2], src_ea_buf);
            return;
        case 0x0e00:
            switch (inst_stream[1] & 0x0800) {
            case 0x0000: sprintf(buf, "MOVES.B  %s,%s", src_ea_buf, disass_reg((inst_stream[1] >> 12) & 0xf));
                return;
            case 0x0800: sprintf(buf, "MOVES.B  %s,%s", disass_reg((inst_stream[1] >> 12) & 0xf), src_ea_buf);
                return;
            }
			case 0x0e40:
				switch (inst_stream[1] & 0x0800) {
				case 0x0000: sprintf(buf, "MOVES.W  %s,%s", src_ea_buf, disass_reg((inst_stream[1] >> 12) & 0xf));
					return;
				case 0x0800: sprintf(buf, "MOVES.W  %s,%s", disass_reg((inst_stream[1] >> 12) & 0xf), src_ea_buf);
					return;
				}
				case 0x0e80:
					switch (inst_stream[1] & 0x0800) {
					case 0x0000: sprintf(buf, "MOVES.L  %s,%s", src_ea_buf, disass_reg((inst_stream[1] >> 12) & 0xf));
						return;
					case 0x0800: sprintf(buf, "MOVES.L  %s,%s", disass_reg((inst_stream[1] >> 12) & 0xf), src_ea_buf);
						return;
					}
					case 0x06c0:
						switch (inst_stream[0] & 0x0030) {
						case 0x0000: sprintf(buf, "RTM      %s", disass_reg(inst_stream[0] & 0xf));
							return;
						case 0x0030: sprintf(buf, "CALLM    %s", src_ea_buf);
							return;
						}
						case 0x00c0:
							disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 0);
							switch (inst_stream[1] & 0x0800) {
							case 0x0000: sprintf(buf, "CMP2.B   %s,%s", src_ea_buf, disass_reg((inst_stream[1] >> 12) & 0xf));
								return;
							case 0x0800: sprintf(buf, "CHK2.B   %s,%s", src_ea_buf, disass_reg((inst_stream[1] >> 12) & 0xf));
								return;
							}
							case 0x02c0:
								disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 1);
								switch (inst_stream[1] & 0x0800) {
								case 0x0000: sprintf(buf, "CMP2.W   %s,%s", src_ea_buf, disass_reg((inst_stream[1] >> 12) & 0xf));
									return;
								case 0x0800: sprintf(buf, "CHK2.W   %s,%s", src_ea_buf, disass_reg((inst_stream[1] >> 12) & 0xf));
									return;
								}
								case 0x04c0:
									disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 2);
									switch (inst_stream[1] & 0x0800) {
									case 0x0000: sprintf(buf, "CMP2.L   %s,%s", src_ea_buf, disass_reg((inst_stream[1] >> 12) & 0xf));
										return;
									case 0x0800: sprintf(buf, "CHK2.L   %s,%s", src_ea_buf, disass_reg((inst_stream[1] >> 12) & 0xf));
										return;
									}
        }
        if (inst_stream[0] & 0x0100) {
            if (src_mode == 1) {
                switch (tgt_mode) {
                case 0x0100: sprintf(buf, "MOVEP.W  #%04x(%s),%s", inst_stream[1], disass_areg(src_spec), disass_dreg(tgt_spec));
                    return;
                case 0x0140: sprintf(buf, "MOVEP.L  #%04x(%s),%s", inst_stream[1], disass_areg(src_spec), disass_dreg(tgt_spec));
                    return;
                case 0x0180: sprintf(buf, "MOVEP.W  %s,#%04x(%s)", disass_dreg(tgt_spec), inst_stream[1], disass_areg(src_spec));
                    return;
                case 0x01c0: sprintf(buf, "MOVEP.L  %s,#%04x(%s)", disass_dreg(tgt_spec), inst_stream[1], disass_areg(src_spec));
                    return;
                }
            }
            switch (inst_stream[0] & 0x0030) {
            case 0x0000: sprintf(buf, "BTST     %s,%s", disass_dreg(tgt_spec), src_ea_buf);
                return;
            case 0x0010: sprintf(buf, "BCHG     %s,%s", disass_dreg(tgt_spec), src_ea_buf);
                return;
            case 0x0020: sprintf(buf, "BCLR     %s,%s", disass_dreg(tgt_spec), src_ea_buf);
                return;
            case 0x0030: sprintf(buf, "BSET     %s,%s", disass_dreg(tgt_spec), src_ea_buf);
                return;
            }
        }
    case 0x1000: /* Move Byte */
        wc = disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 0);
        disass_ea(tgt_ea_buf, inst_stream + wc, tgt_mode, tgt_spec, 0);
        sprintf(buf, "MOVE.B   %s,%s", src_ea_buf, tgt_ea_buf);
        return;
    case 0x2000: /* Move Long */
        wc = disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 2);
        disass_ea(tgt_ea_buf, inst_stream + wc, tgt_mode, tgt_spec, 2);
        sprintf(buf, "MOVE.L   %s,%s", src_ea_buf, tgt_ea_buf);
        return;
    case 0x3000: /* Move Word */
        wc = disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 1);
        disass_ea(tgt_ea_buf, inst_stream + wc, tgt_mode, tgt_spec, 1);
        sprintf(buf, "MOVE.W   %s,%s", src_ea_buf, tgt_ea_buf);
        return;
    case 0x4000: /* Miscellaneous */
        // TODO: MULU.L, MULS.L, DIVU.L, DIVS.L, EXTB.L
        disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, src_size);
        switch (inst_stream[0] & 0xffc0) {
        case 0x4000:
            sprintf(buf, "NEGX.B   %s", src_ea_buf);
            return;
        case 0x4040:
            sprintf(buf, "NEGX.W   %s", src_ea_buf);
            return;
        case 0x4080:
            sprintf(buf, "NEGX.L   %s", src_ea_buf);
            return;
        case 0x40c0:
            disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 1);
            sprintf(buf, "MOVE.W   SR,%s", src_ea_buf);
            return;
        case 0x4200:
            sprintf(buf, "CLR.B    %s", src_ea_buf);
            return;
        case 0x4240:
            sprintf(buf, "CLR.W    %s", src_ea_buf);
            return;
        case 0x4280:
            sprintf(buf, "CLR.L    %s", src_ea_buf);
            return;
        case 0x42c0:
            disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 1);
            sprintf(buf, "MOVE.W   CCR,%s", src_ea_buf);
            return;
        case 0x4400:
            sprintf(buf, "NEG.B    %s", src_ea_buf);
            return;
        case 0x4440:
            sprintf(buf, "NEG.W    %s", src_ea_buf);
            return;
        case 0x4480:
            sprintf(buf, "NEG.L    %s", src_ea_buf);
            return;
        case 0x44c0:
            disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 1);
            sprintf(buf, "MOVE.W   %s,CCR", src_ea_buf);
            return;
        case 0x4600:
            disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 1);
            sprintf(buf, "NOT.B    %s", src_ea_buf);
            return;
        case 0x4640:
            sprintf(buf, "NOT.W    %s", src_ea_buf);
            return;
        case 0x4680:
            sprintf(buf, "NOT.L    %s", src_ea_buf);
            return;
        case 0x46c0:
            disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 1);
            sprintf(buf, "MOVE.W   %s,SR", src_ea_buf);
            return;
        case 0x4800:
            if (src_mode == 1) {
                sprintf(buf, "LINK.L   #%04x%04x,%s",
                    inst_stream[1], inst_stream[2], disass_areg(src_spec));
            } else {
                sprintf(buf, "NBCD     %s", src_ea_buf);
            }
            return;
        case 0x4840:
            if (src_mode == 0) {
                sprintf(buf, "SWAP.W   %s", disass_dreg(src_spec));
            } else if (src_mode == 1) {
                sprintf(buf, "BKPT     #%x", src_spec);
            } else {
                sprintf(buf, "PEA      %s", src_ea_buf);
            }
            return;
        case 0x4880:
            if (src_mode == 0) {
                sprintf(buf, "EXT.W    %s", disass_dreg(src_spec));
            } else {
                disass_reglist(tgt_ea_buf, inst_stream[1], src_mode == 6);
                disass_ea(src_ea_buf, inst_stream + 1, src_mode, src_spec, src_size);
                sprintf(buf, "MOVEM.W  %s,%s", tgt_ea_buf, src_ea_buf);
            }
            return;
        case 0x48c0:
            if (src_mode == 0) {
                sprintf(buf, "EXT.L    %s", disass_dreg(src_spec));
            } else {
                disass_reglist(tgt_ea_buf, inst_stream[1], src_mode == 6);
                disass_ea(src_ea_buf, inst_stream + 1, src_mode, src_spec, src_size);
                sprintf(buf, "MOVEM.L  %s,%s", tgt_ea_buf, src_ea_buf);
            }
            return;
        case 0x4a00:
            sprintf(buf, "TST.B    %s", src_ea_buf);
            return;
        case 0x4a40:
            sprintf(buf, "TST.W    %s", src_ea_buf);
            return;
        case 0x4a80:
            sprintf(buf, "TST.L    %s", src_ea_buf);
            return;
        case 0x4ac0:
            switch (inst_stream[0] & 0x003f) {
            case 0x003a:
                sprintf(buf, "BGND   ");
                return;
            case 0x003c:
                sprintf(buf, "ILLEGAL");
                return;
            default:
                disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 0);
                sprintf(buf, "TAS    %s", src_ea_buf);
                return;
            }
			case 0x4c80:
				if (src_mode == 0) {
				} else {
					disass_reglist(tgt_ea_buf, inst_stream[1], src_mode == 6);
					disass_ea(src_ea_buf, inst_stream + 1, src_mode, src_spec, src_size);
					sprintf(buf, "MOVEM.W  %s,%s", src_ea_buf, tgt_ea_buf);
				}
				return;
			case 0x4cc0:
				if (src_mode == 0) {
				} else {
					disass_reglist(tgt_ea_buf, inst_stream[1], src_mode == 6);
					disass_ea(src_ea_buf, inst_stream + 1, src_mode, src_spec, 2);
					sprintf(buf, "MOVEM.L  %s,%s", src_ea_buf, tgt_ea_buf);
				}
				return;
			case 0x4e40:
				switch(src_mode) {
				case 0:
				case 1:
					sprintf(buf, "TRAP     #%x", inst_stream[0] & 0xf);
					return;
				case 2:
					sprintf(buf, "LINK     #%4x,%s", inst_stream[1], disass_areg(src_spec));
					return;
				case 3:
					sprintf(buf, "UNLK     %s", disass_areg(src_spec));
					return;
				case 4:
					sprintf(buf, "MOVE.L   %s,USP", disass_areg(src_spec));
					return;
				case 5:
					sprintf(buf, "MOVE.L   USP,%s", disass_areg(src_spec));
					return;
				case 6:
					switch(inst_stream[0]) {
					case 0x4e70:
						sprintf(buf, "RESET    ");
						return;
					case 0x4e71:
						sprintf(buf, "NOP      ");
						return;
					case 0x4e72:
						sprintf(buf, "STOP     ");
						return;
					case 0x4e73:
						sprintf(buf, "RTE      ");
						return;
					case 0x4e74:
						sprintf(buf, "RTD      ");
						return;
					case 0x4e75:
						sprintf(buf, "RTS      ");
						return;
					case 0x4e76:
						sprintf(buf, "TRAPV    ");
						return;
					case 0x4e77:
						sprintf(buf, "RTR      ");
						return;
					}
					case 7:
						switch(inst_stream[0]) {
						case 0x4e7a:
							sprintf(buf, "MOVEC    #%04x,%s", inst_stream[1] & 0xfff, disass_reg(inst_stream[1] >> 12));
							return;
						case 0x4e7b:
							sprintf(buf, "MOVEC    #%s,%04x", disass_reg(inst_stream[1] >> 12), inst_stream[1] & 0xfff);
							return;
						}
				}
				sprintf(buf, "???      ");
				return;
				case 0x4e80:
					sprintf(buf, "JSR      %s", src_ea_buf);
					return;
				case 0x4ec0:
					disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 2);
					sprintf(buf, "JMP      %s", src_ea_buf);
					return;
        }
        switch (tgt_mode) {
        case 4:
            sprintf(buf, "CHK.L    %s,%s", src_ea_buf, disass_dreg(tgt_spec));
            return;
        case 6:
            sprintf(buf, "CHK.W    %s,%s", src_ea_buf, disass_dreg(tgt_spec));
            return;
        case 7:
            sprintf(buf, "LEA      %s,%s", src_ea_buf, disass_areg(tgt_spec));
            return;
        }
		case 0x5000: /* ADDQ/SUBQ/Scc/DBcc/TRAPcc */
			// TODO: TRAPcc
			wc = disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, src_size);
			switch (inst_stream[0] & 0x00c0) {
			case 0x0000:
				if (inst_stream[0] & 0x0100) {
					sprintf(buf, "SUBQ.B   #%x,%s", disass_count(tgt_spec), src_ea_buf);
				} else {
					sprintf(buf, "ADDQ.B   #%x,%s", disass_count(tgt_spec), src_ea_buf);
				}
				return;
			case 0x0040:
				if (inst_stream[0] & 0x0100) {
					sprintf(buf, "SUBQ.W   #%x,%s", disass_count(tgt_spec), src_ea_buf);
				} else {
					sprintf(buf, "ADDQ.W   #%x,%s", disass_count(tgt_spec), src_ea_buf);
				}
				return;
			case 0x0080:
				if (inst_stream[0] & 0x0100) {
					sprintf(buf, "SUBQ.L   #%x,%s", disass_count(tgt_spec), src_ea_buf);
				} else {
					sprintf(buf, "ADDQ.L   #%x,%s", disass_count(tgt_spec), src_ea_buf);
				}
				return;
			case 0x00c0:
				if (src_mode == 1) {
					disass_displacement(tgt_ea_buf, inst_stream + wc, 0);
					sprintf(buf, "DB%s     %s,%s", disass_cc(condition), tgt_ea_buf, disass_dreg(src_spec));
				} else {
					sprintf(buf, "S%s      %s", disass_cc(condition), src_ea_buf);
				}
				return;
			}
			case 0x6000: /* Bcc/BSR/BRA */
				disass_displacement(tgt_ea_buf, inst_stream, inst_stream[0]);
				if ((inst_stream[0] & 0xff00) == 0x6000) {
					sprintf(buf, "BRA      %s", tgt_ea_buf);
				} else if ((inst_stream[0] & 0xff00) == 0x6100) {
					sprintf(buf, "BSR      %s", tgt_ea_buf);
				} else {
					sprintf(buf, "B%s      %s",disass_cc(condition), tgt_ea_buf);
				}
				return;
			case 0x7000: /* MOVEQ */
				sprintf(buf, "MOVEQ.L  #%02x,%s", (char) inst_stream[0], disass_dreg(tgt_spec));
				return;
			case 0x8000: /* OR/DIV/SBCD */
				// TODO: PACK, UNPK
				disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, src_size);
				switch (inst_stream[0] & 0x01c0) {
				case 0x0000:
					sprintf(buf, "OR.B     %s,%s", src_ea_buf, disass_dreg(tgt_spec));
					return;
				case 0x0040:
					sprintf(buf, "OR.W     %s,%s", src_ea_buf, disass_dreg(tgt_spec));
					return;
				case 0x0080:
					sprintf(buf, "OR.L     %s,%s", src_ea_buf, disass_dreg(tgt_spec));
					return;
				case 0x00c0:
					disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 1);
					sprintf(buf, "DIVU.W   %s,%s", src_ea_buf, disass_dreg(tgt_spec));
					return;
				case 0x0100:
					if (src_mode == 0) {
						sprintf(buf, "SBCD     %s,%s", disass_dreg(src_spec), disass_dreg(tgt_spec));
					} else if (src_mode == 1) {
						sprintf(buf, "SBCD     -(%s),-(%s)", disass_areg(src_spec), disass_areg(tgt_spec));
					} else {
						sprintf(buf, "OR.B     %s,%s", disass_dreg(tgt_spec), src_ea_buf);
					}
					return;
				case 0x0140:
					sprintf(buf, "OR.W     %s,%s", disass_reg(tgt_spec), src_ea_buf);
					return;
				case 0x0180:
					sprintf(buf, "OR.L     %s,%s", disass_reg(tgt_spec), src_ea_buf);
					return;
				case 0x01c0:
					disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 1);
					sprintf(buf, "DIVS.W   %s,%s", src_ea_buf, disass_reg(tgt_spec));
					return;
				}
				case 0x9000:
					disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, src_size);
					switch (inst_stream[0] & 0x01c0) {
					case 0x0000:
						sprintf(buf, "SUB.B    %s,%s", src_ea_buf, disass_dreg(tgt_spec));
						return;
					case 0x0040:
						sprintf(buf, "SUB.W    %s,%s", src_ea_buf, disass_dreg(tgt_spec));
						return;
					case 0x0080:
						sprintf(buf, "SUB.L    %s,%s", src_ea_buf, disass_dreg(tgt_spec));
						return;
					case 0x00c0:
						disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 1);
						sprintf(buf, "SUBA.W   %s,%s", src_ea_buf, disass_areg(tgt_spec));
						return;
					case 0x0100:
						if (src_mode == 0) {
							sprintf(buf, "SUBX.B   %s,%s", disass_dreg(src_spec), disass_dreg(tgt_spec));
						} else if (src_mode == 1) {
							sprintf(buf, "SUBX.B   -(%s),-(%s)", disass_areg(src_spec), disass_areg(tgt_spec));
						} else {
							sprintf(buf, "SUB.B    %s,%s", disass_dreg(tgt_spec), src_ea_buf);
						}
						return;
					case 0x0140:
						if (src_mode == 0) {
							sprintf(buf, "SUBX.W   %s,%s", disass_dreg(src_spec), disass_dreg(tgt_spec));
						} else if (src_mode == 1) {
							sprintf(buf, "SUBX.W   -(%s),-(%s)", disass_areg(src_spec), disass_areg(tgt_spec));
						} else {
							sprintf(buf, "SUB.W    %s,%s", disass_dreg(tgt_spec), src_ea_buf);
						}
						return;
					case 0x0180:
						if (src_mode == 0) {
							sprintf(buf, "SUBX.L   %s,%s", disass_dreg(src_spec), disass_dreg(tgt_spec));
						} else if (src_mode == 1) {
							sprintf(buf, "SUBX.L   -(%s),-(%s)", disass_areg(src_spec), disass_areg(tgt_spec));
						} else {
							sprintf(buf, "SUB.L    %s,%s", disass_dreg(tgt_spec), src_ea_buf);
						}
						return;
					case 0x01c0:
						disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 2);
						sprintf(buf, "SUBA.L   %s,%s", src_ea_buf, disass_areg(tgt_spec));
						return;
					}
					case 0xb000:
						disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, src_size);
						switch (inst_stream[0] & 0x01c0) {
						case 0x0000:
							sprintf(buf, "CMP.B    %s,%s", src_ea_buf, disass_dreg(tgt_spec));
							return;
						case 0x0040:
							sprintf(buf, "CMP.W    %s,%s", src_ea_buf, disass_dreg(tgt_spec));
							return;
						case 0x0080:
							sprintf(buf, "CMP.L    %s,%s", src_ea_buf, disass_dreg(tgt_spec));
							return;
						case 0x00c0:
							disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 1);
							sprintf(buf, "CMPA.W   %s,%s", src_ea_buf, disass_areg(tgt_spec));
							return;
						case 0x0100:
							if (src_mode == 1) {
								sprintf(buf, "CMPM.B   (%s)+,(%s)+", disass_areg(src_spec), disass_areg(tgt_spec));
							} else {
								sprintf(buf, "EOR.B    %s,%s", disass_dreg(tgt_spec), src_ea_buf);
							}
							return;
						case 0x0140:
							if (src_mode == 1) {
								sprintf(buf, "CMPM.W   (%s)+,(%s)+", disass_areg(src_spec), disass_areg(tgt_spec));
							} else {
								sprintf(buf, "EOR.W    %s,%s", disass_dreg(tgt_spec), src_ea_buf);
							}
							return;
						case 0x0180:
							if (src_mode == 1) {
								sprintf(buf, "CMPM.L   (%s)+,(%s)+", disass_areg(src_spec), disass_areg(tgt_spec));
							} else {
								sprintf(buf, "EOR.L    %s,%s", disass_dreg(tgt_spec), src_ea_buf);
							}
							return;
						case 0x01c0:
							disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 2);
							sprintf(buf, "CMPA.L   %s,%s", src_ea_buf, disass_areg(tgt_spec));
							return;
						}
						case 0xc000:
							disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, src_size);
							switch (inst_stream[0] & 0x01c0) {
							case 0x0000:
								sprintf(buf, "AND.B    %s,%s", src_ea_buf, disass_dreg(tgt_spec));
								return;
							case 0x0040:
								sprintf(buf, "AND.W    %s,%s", src_ea_buf, disass_dreg(tgt_spec));
								return;
							case 0x0080:
								sprintf(buf, "AND.L    %s,%s", src_ea_buf, disass_dreg(tgt_spec));
								return;
							case 0x00c0:
								disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 1);
								sprintf(buf, "MULU.W   %s,%s", src_ea_buf, disass_dreg(tgt_spec));
								return;
							case 0x0100:
								if (src_mode == 0) {
									sprintf(buf, "ABCD     %s,%s", disass_dreg(src_spec), disass_dreg(tgt_spec));
								} else if (src_mode == 1) {
									sprintf(buf, "ABCD     -(%s),-(%s)", disass_areg(src_spec), disass_reg(tgt_spec));
								} else {
									sprintf(buf, "AND.B    %s,%s", disass_dreg(tgt_spec), src_ea_buf);
								}
								return;
							case 0x0140:
								if (src_mode == 0) {
									sprintf(buf, "EXG      %s,%s", disass_dreg(src_spec), disass_dreg(tgt_spec));
								} else if (src_mode == 1) {
									sprintf(buf, "EXG      %s,%s", disass_areg(src_spec), disass_areg(tgt_spec));
								} else {
									sprintf(buf, "AND.W    %s,%s", disass_reg(tgt_spec), src_ea_buf);
								}
								return;
							case 0x0180:
								if (src_mode == 1) {
									sprintf(buf, "EXG      %s,%s", disass_dreg(src_spec), disass_areg(tgt_spec));
								} else {
									sprintf(buf, "AND.L    %s,%s", disass_dreg(tgt_spec), src_ea_buf);
								}
								return;
							case 0x01c0:
								disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 1);
								sprintf(buf, "MULS.W   %s,%s", src_ea_buf, disass_dreg(tgt_spec));
								return;
							}
							case 0xd000:
								disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, src_size);
								switch (inst_stream[0] & 0x01c0) {
								case 0x0000:
									sprintf(buf, "ADD.B    %s,%s", src_ea_buf, disass_dreg(tgt_spec));
									return;
								case 0x0040:
									sprintf(buf, "ADD.W    %s,%s", src_ea_buf, disass_dreg(tgt_spec));
									return;
								case 0x0080:
									sprintf(buf, "ADD.L    %s,%s", src_ea_buf, disass_dreg(tgt_spec));
									return;
								case 0x00c0:
									disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 1);
									sprintf(buf, "ADDA.W   %s,%s", src_ea_buf, disass_areg(tgt_spec));
									return;
								case 0x0100:
									if (src_mode == 0) {
										sprintf(buf, "ADDX.B   %s,%s", disass_dreg(src_spec), disass_dreg(tgt_spec));
									} else if (src_mode == 0x0008) {
										sprintf(buf, "ADDX.B   -(%s),-(%s)", disass_areg(src_spec), disass_areg(tgt_spec));
									} else {
										sprintf(buf, "ADD.B    %s,%s", disass_dreg(tgt_spec), src_ea_buf);
									}
									return;
								case 0x0140:
									if (src_mode == 0) {
										sprintf(buf, "ADDX.W   %s,%s", disass_dreg(src_spec), disass_dreg(tgt_spec));
									} else if (src_mode == 1) {
										sprintf(buf, "ADDX.W   -(%s),-(%s)", disass_areg(src_spec), disass_areg(tgt_spec));
									} else {
										sprintf(buf, "ADD.W    %s,%s", disass_dreg(tgt_spec), src_ea_buf);
									}
									return;
								case 0x0180:
									if (src_mode == 0) {
										sprintf(buf, "ADDX.L   %s,%s", disass_dreg(src_spec), disass_dreg(tgt_spec));
									} else if (src_mode == 1) {
										sprintf(buf, "ADDX.L   -(%s),-(%s)", disass_areg(src_spec), disass_areg(tgt_spec));
									} else {
										sprintf(buf, "ADD.L    %s,%s", disass_dreg(tgt_spec), src_ea_buf);
									}
									return;
								case 0x01c0:
									disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, 2);
									sprintf(buf, "ADDA.L   %s,%s", src_ea_buf, disass_areg(tgt_spec));
									return;
								}
								case 0xe000:
									// TODO: BFTST, BFEXTU, BFCHG, BFEXTS, BFCLR, BFFFO, BFSET, BFINS
									disass_ea(src_ea_buf, inst_stream, src_mode, src_spec, src_size);
									switch (inst_stream[0] & 0x07c0) {
									case 0x00c0:
										sprintf(buf, "ASR.B    #1,%s", src_ea_buf);
										return;
									case 0x01c0:
										sprintf(buf, "ASL.B    #1,%s", src_ea_buf);
										return;
									case 0x02c0:
										sprintf(buf, "LSR.B    #1,%s", src_ea_buf);
										return;
									case 0x03c0:
										sprintf(buf, "LSL.B    #1,%s", src_ea_buf);
										return;
									case 0x04c0:
										sprintf(buf, "ROXR.B   #1,%s", src_ea_buf);
										return;
									case 0x05c0:
										sprintf(buf, "ROXL.B   #1,%s", src_ea_buf);
										return;
									case 0x06c0:
										sprintf(buf, "ROR.B    #1,%s", src_ea_buf);
										return;
									case 0x07c0:
										sprintf(buf, "ROL.B    #1,%s", src_ea_buf);
										return;
									}
									switch (inst_stream[0] & 0x01f8) {
									case 0x0000:
										sprintf(buf, "ASR.B    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0008:
										sprintf(buf, "LSR.B    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0010:
										sprintf(buf, "ROXR.B   #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0018:
										sprintf(buf, "ROR.B    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0020:
										sprintf(buf, "ASR.B    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0028:
										sprintf(buf, "LSR.B    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0030:
										sprintf(buf, "ROXR.B   #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0038:
										sprintf(buf, "ROR.B    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0040:
										sprintf(buf, "ASR.W    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0048:
										sprintf(buf, "LSR.W    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0050:
										sprintf(buf, "ROXR.W   #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0058:
										sprintf(buf, "ROR.W    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0060:
										sprintf(buf, "ASR.W    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0068:
										sprintf(buf, "LSR.W    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0070:
										sprintf(buf, "ROXR.W   #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0078:
										sprintf(buf, "ROR.W    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0080:
										sprintf(buf, "ASR.L    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0088:
										sprintf(buf, "LSR.L    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0090:
										sprintf(buf, "ROXR.L   #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0098:
										sprintf(buf, "ROR.L    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x00a0:
										sprintf(buf, "ASR.L    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x00a8:
										sprintf(buf, "LSR.L    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x00b0:
										sprintf(buf, "ROXR.L   #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x00b8:
										sprintf(buf, "ROR.L    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0100:
										sprintf(buf, "ASL.B    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0108:
										sprintf(buf, "LSL.B    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0110:
										sprintf(buf, "ROXL.B   #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0118:
										sprintf(buf, "ROL.B    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0120:
										sprintf(buf, "ASL.B    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0128:
										sprintf(buf, "LSL.B    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0130:
										sprintf(buf, "ROXL.B   #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0138:
										sprintf(buf, "ROL.B    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0140:
										sprintf(buf, "ASL.W    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0148:
										sprintf(buf, "LSL.W    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0150:
										sprintf(buf, "ROXL.W   #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0158:
										sprintf(buf, "ROL.W    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0160:
										sprintf(buf, "ASL.W    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0168:
										sprintf(buf, "LSL.W    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0170:
										sprintf(buf, "ROXL.W   #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0178:
										sprintf(buf, "ROL.W    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0180:
										sprintf(buf, "ASL.L    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0188:
										sprintf(buf, "LSL.L    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0190:
										sprintf(buf, "ROXL.L   #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x0198:
										sprintf(buf, "ROL.L    #%x,%s", disass_count(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x01a0:
										sprintf(buf, "ASL.L    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x01a8:
										sprintf(buf, "LSL.L    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x01b0:
										sprintf(buf, "ROXL.L   #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
									case 0x01b8:
										sprintf(buf, "ROL.L    #%s,%s", disass_dreg(tgt_spec), disass_dreg(src_spec));
										return;
        }
    }
}


