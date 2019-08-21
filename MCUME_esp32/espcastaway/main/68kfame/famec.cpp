/****************************************************************************/
/* FAME (Fast and Accurate Motorola 68000 Emulation Library)                */
/* Emulador de 68000 en C                                                   */
/* Autor: Oscar Orallo Pelaez                                               */
/* Fecha de comienzo: 03-10-2006                                            */
/* Ultima actualizacion: 28-10-2009                                         */
/* Based on the excellent FAMEC emulator by Stephane Dallongueville         */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef _MSC_VER
/* Ignore unary minus applied to unsigned type */
#pragma warning( disable : 4146 )
#endif

/* Options */
/*
 Do not use the following lines to enable/disable features
 They are here as a reference only
 Define them in your project as you need instead
*/
/* #define FAME_INLINE_LOOP */
/* #define FAME_IRQ_CLOCKING */
/* #define FAME_CHECK_BRANCHES */
/* #define FAME_DIRECT_MAPPING */
/* #define FAME_EXTRA_INLINE */
/* #define FAME_EMULATE_TRACE */
/* #define FAME_BYPASS_TAS_WRITEBACK */
/* #define FAME_ACCURATE_TIMING */
/* #define FAME_GLOBAL_CONTEXT */
/* #define FAME_DEBUG */
/* #define FAME_GOTOS */
/* #define FAME_BIG_ENDIAN */

#define FAME_SECURE_ALL_BANKS

#ifndef FAME_ADDR_BITS
#define FAME_ADDR_BITS  24
#endif

#ifndef FAME_PC_BITS
#define FAME_PC_BITS  24
#endif

#ifndef FAME_FETCHBITS
#define FAME_FETCHBITS 12
#endif

#ifndef FAME_DATABITS
#define FAME_DATABITS  12
#endif

#ifndef FAME_PREFIX
#define FAME_PREFIX m68k
#endif

/* Options */

#define CONCAT(P1,P2)  P1##P2
#define FAME_FNT(P,F)  CONCAT(P,_##F)
#define FAME_DT(P,D)   CONCAT(P,D)
#define FAME_API(F)    FAME_FNT(FAME_PREFIX,F)
#define FAME_CONTEXT   FAME_DT(FAME_PREFIX,context)


#ifndef INLINE
#define INLINE
#endif

#ifndef FAME_EXTRA_INLINE
#define EXTRA_INLINE
#else
#define EXTRA_INLINE INLINE
#endif

/* Return codes */
#define M68K_OK 0
#define M68K_RUNNING 1
#define M68K_NO_SUP_ADDR_SPACE 2
#define M68K_INV_REG -1

/* Hardware interrupt state */
#define M68K_IRQ_LEVEL_ERROR -1
#define M68K_IRQ_INV_PARAMS -2

/* Defines to specify hardware interrupt type */
#define M68K_AUTOVECTORED_IRQ -1
#define M68K_SPURIOUS_IRQ -2

/* Defines to specify address space */
#define M68K_SUP_ADDR_SPACE 0
#define M68K_USER_ADDR_SPACE 2
#define M68K_PROG_ADDR_SPACE 0
#define M68K_DATA_ADDR_SPACE 1

/******************************/
/* 68K core types definitions */
/******************************/

#if FAME_ADDR_BITS < 32
#define M68K_ADDR_MASK  ((1 << FAME_ADDR_BITS)-1)
#else
#define M68K_ADDR_MASK  0xFFFFFFFF
#endif

#define M68K_FETCHSFT  (FAME_ADDR_BITS - FAME_FETCHBITS)
#define M68K_FETCHBANK (1 << FAME_FETCHBITS)
#define M68K_FETCHMASK (M68K_FETCHBANK - 1)

#define M68K_DATASFT  (FAME_ADDR_BITS - FAME_DATABITS)
#define M68K_DATABANK (1 << FAME_DATABITS)
#define M68K_DATAMASK (M68K_DATABANK - 1)

#define M68K_SR_C_SFT   8
#define M68K_SR_V_SFT   7
#define M68K_SR_Z_SFT   0
#define M68K_SR_N_SFT   7
#define M68K_SR_X_SFT   8

#define M68K_SR_S_SFT   13
#define M68K_SR_T_SFT   15

#define M68K_SR_C       (1 << M68K_SR_C_SFT)
#define M68K_SR_V       (1 << M68K_SR_V_SFT)
#define M68K_SR_Z       0
#define M68K_SR_N       (1 << M68K_SR_N_SFT)
#define M68K_SR_X       (1 << M68K_SR_X_SFT)

#define M68K_SR_S       (1 << M68K_SR_S_SFT)
#define M68K_SR_T       (1 << M68K_SR_T_SFT)

#define M68K_CCR_MASK   0x1F

#ifdef FAME_IRQ_CLOCKING
#define INT_TIMING 44
#else
#define INT_TIMING  0
#endif

#ifdef FAME_EMULATE_TRACE
#define M68K_SR_MASK    (M68K_SR_T | M68K_SR_S | 0x0700 | M68K_CCR_MASK)
#else
#define M68K_SR_MASK    (M68K_SR_S | 0x0700 | M68K_CCR_MASK)
#endif

/* exception defines taken from musashi core */
#define M68K_RESET_EX                   1
#define M68K_BUS_ERROR_EX               2
#define M68K_ADDRESS_ERROR_EX           3
#define M68K_ILLEGAL_INSTRUCTION_EX     4
#define M68K_ZERO_DIVIDE_EX             5
#define M68K_CHK_EX                     6
#define M68K_TRAPV_EX                   7
#define M68K_PRIVILEGE_VIOLATION_EX     8
#define M68K_TRACE_EX                   9
#define M68K_1010_EX                    10
#define M68K_1111_EX                    11
#define M68K_FORMAT_ERROR_EX            14
#define M68K_UNINITIALIZED_INTERRUPT_EX 15
#define M68K_SPURIOUS_INTERRUPT_EX      24
#define M68K_INTERRUPT_AUTOVECTOR_EX    24
#define M68K_TRAP_BASE_EX               32

#define M68K_INT_ACK_AUTOVECTOR         -1

/*#define M68K_RUNNING    0x01 */
#define M68K_HALTED     0x80
#define M68K_WAITING    0x04
#define M68K_DISABLE    0x20
/* #define M68K_FAULTED    0x40 */
#define M68K_EMULATE_GROUP_0  0x02
#define M68K_EMULATE_TRACE    0x08
#define M68K_DO_TRACE    0x10

#ifdef FAME_LITTLE_ENDIAN
#ifdef FAME_BIG_ENDIAN
#undef FAME_BIG_ENDIAN
#endif
#else
#ifndef FAME_BIG_ENDIAN
#if defined(__hppa__) || \
    defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
    (defined(__MIPS__) && defined(__MISPEB__)) || \
    defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
    defined(__sparc__)
#define FAME_BIG_ENDIAN
#else
#define FAME_LITTLE_ENDIAN
#endif
#endif
#endif


/*
 internals core macros
*/

#define DREG(X)         (FAME_CONTEXT.dreg[(X)].D)
#define DREGu32(X)      (FAME_CONTEXT.dreg[(X)].D)
#define DREGs32(X)      (FAME_CONTEXT.dreg[(X)].SD)
#define DREGu16(X)      (FAME_CONTEXT.dreg[(X)].w.W)
#define DREGs16(X)      (FAME_CONTEXT.dreg[(X)].sw.SW)
#define DREGu8(X)       (FAME_CONTEXT.dreg[(X)].b.B)
#define DREGs8(X)       (FAME_CONTEXT.dreg[(X)].sb.SB)

#define AREG(X)         (FAME_CONTEXT.areg[(X)].D)
#define AREGu32(X)      (FAME_CONTEXT.areg[(X)].D)
#define AREGs32(X)      (FAME_CONTEXT.areg[(X)].SD)
#define AREGu16(X)      (FAME_CONTEXT.areg[(X)].w.W)
#define AREGs16(X)      (FAME_CONTEXT.areg[(X)].sw.SW)

#define ASP             (FAME_CONTEXT.asp)

#define LSL(A, C)       ((A) << (C))
#define LSR(A, C)       ((A) >> (C))

#define LSR_32(A, C)    ((C) < 32 ? (A) >> (C) : 0)
#define LSL_32(A, C)    ((C) < 32 ? (A) << (C) : 0)

#define ROL_8(A, C)     (LSL(A, C) | LSR(A, 8-(C)))
#define ROL_9(A, C)     (LSL(A, C) | LSR(A, 9-(C)))
#define ROL_16(A, C)    (LSL(A, C) | LSR(A, 16-(C)))
#define ROL_17(A, C)    (LSL(A, C) | LSR(A, 17-(C)))
#define ROL_32(A, C)    (LSL_32(A, C) | LSR_32(A, 32-(C)))
#define ROL_33(A, C)    (LSL_32(A, C) | LSR_32(A, 33-(C)))

#define ROR_8(A, C)     (LSR(A, C) | LSL(A, 8-(C)))
#define ROR_9(A, C)     (LSR(A, C) | LSL(A, 9-(C)))
#define ROR_16(A, C)    (LSR(A, C) | LSL(A, 16-(C)))
#define ROR_17(A, C)    (LSR(A, C) | LSL(A, 17-(C)))
#define ROR_32(A, C)    (LSR_32(A, C) | LSL_32(A, 32-(C)))
#define ROR_33(A, C)    (LSR_32(A, C) | LSL_32(A, 33-(C)))

/* Flag setup */
#define SET_FLAGS_Z_VC0 \
    flag_C = 0; \
    flag_V = 0; \
    flag_NotZ = res;

#define SET_FLAGS_NZ_VC0 \
	SET_FLAGS_Z_VC0 \
    flag_N = res;

#define SET_FLAGS_DIV_ZERO \
	flag_V = 0; \
	flag_C = 0;

#define SET_FLAGS_DIV_OVERFLOW \
	flag_C = 0; \
	flag_V = M68K_SR_V; \
	flag_N = M68K_SR_N; \
	/* Z flag is undefined on division overflow */ \
	/* but is set here to match FAME versions */ \
	flag_NotZ = 1; \


#ifdef FAME_DEBUG
#define DEBUG_OPCODE(OP) printf(":Opcode %.4X\n",Opcode);
#else
#define DEBUG_OPCODE(OP)
#endif

#ifdef FAME_GOTOS
#define NEXT                    \
    FETCH_WORD(Opcode);         \
    DEBUG_OPCODE(Opcode) \
    goto *JumpTable[Opcode];

#ifdef FAME_INLINE_LOOP
#define RET(A)                                      \
    io_cycle_counter -= (A);                        \
    if (io_cycle_counter <= 0) goto famec_Exec_End;	\
    NEXT
#else
#define RET(A)                                      \
    io_cycle_counter -= (A);                        \
    if (io_cycle_counter <= 0) goto famec_Exec_End;	\
    goto famec_Exec;
#endif

#define RET_STOP(C) \
    io_cycle_counter -= (C);                        \
    if (io_cycle_counter > 0) io_cycle_counter = 0;	\
    goto famec_Exec_End;

#else
//printf("%8x",PC);
//printf("=>%8x\n",Opcode);

#define NEXT \
	do { \
		FETCH_WORD(Opcode); \
		DEBUG_OPCODE(Opcode) \
		JumpTable[Opcode](); \
	} while(io_cycle_counter>0);

#ifdef FAME_INLINE_LOOP
#define RET(A) \
    io_cycle_counter -= (A);  \
    if (io_cycle_counter > 0) \
    { \
	    FETCH_WORD(Opcode); \
	    DEBUG_OPCODE(Opcode) \
	    JumpTable[Opcode](); \
    } \
    return;

#else

#define RET(A) \
    io_cycle_counter -= (A);  \
    return;

#endif

#define RET_STOP(C) \
    io_cycle_counter -= (C);                        \
    if (io_cycle_counter > 0) io_cycle_counter = 0;	\
    return;

#endif

#define M68K_PPL (FAME_CONTEXT.sr >> 8) & 7

#if FAME_PC_BITS == 32

#define UNBASED_PC PC

#define READ_BASED_PC BasePC[(PC & M68K_ADDR_MASK) >> 1]

#define READ_BASED_PC_IDX(IDX) BasePC[((PC & M68K_ADDR_MASK) >> 1) + IDX]

#define SET_PC(A)               \
    BasePC = (u16 *)Fetch[(((A) & M68K_ADDR_MASK) >> M68K_FETCHSFT) & M68K_FETCHMASK];    \
	PC = A;

#define INC_PC(I) (PC += I)

#else

#define UNBASED_PC ((u32)PC - BasePC)

#define READ_BASED_PC (*PC)

#define READ_BASED_PC_IDX(IDX) PC[IDX]

#define SET_PC(A)               \
    BasePC = Fetch[((A) >> M68K_FETCHSFT) & M68K_FETCHMASK];    \
    PC = (u16*)(((A) & M68K_ADDR_MASK) + BasePC);

#define INC_PC(I) (PC += (I) >> 1)

#endif

#define READ_BYTE_F(A, D)           \
		D = Read_Byte(A) & 0xFF;

#define READ_WORD_F(A, D)           \
		D = Read_Word(A) & 0xFFFF;

#define READ_LONG_F(A, D)               \
		D = Read_Word((A)) << 16;          \
		D |= Read_Word((A) + 2) & 0xFFFF;

#define READ_LONG_DEC_F(A, D)           \
		D = Read_Word((A) + 2) & 0xFFFF;   \
		D |= Read_Word((A)) << 16;

#define READSX_LONG_F(A, D)             \
		D = Read_Word((A)) << 16;          \
		D |= Read_Word((A) + 2) & 0xFFFF;

#define WRITE_LONG_F(A, D)              \
		Write_Word((A), (D) >> 16);        \
		Write_Word((A) + 2, (D) & 0xFFFF);

#define WRITE_LONG_DEC_F(A, D)         \
		Write_Word((A) + 2, (D) & 0xFFFF); \
		Write_Word((A), (D) >> 16);

#define FETCH_LONG(A)               \
		(A) = READ_BASED_PC_IDX(1) | (READ_BASED_PC_IDX(0) << 16);    \
		INC_PC(4);

#define PUSH_32_F(D)                            \
    	AREG(7) -= 4;                                   \
    	Write_Word(AREG(7), (D) >> 16);	\
    	Write_Word(AREG(7) + 2, (D) & 0xFFFF);

#define POP_32_F(D)                         \
    	D = Read_Word(AREG(7)) << 16;          \
    	D |= Read_Word(AREG(7) + 2) & 0xFFFF;  \
    	AREG(7) += 4;

#define GET_SWORD           \
		((s16)READ_BASED_PC)

#define FETCH_BYTE(A)       \
		(A) = READ_BASED_PC & 0xFF; INC_PC(2);

#define FETCH_SBYTE(A)      \
		(A) = (s8)((READ_BASED_PC) & 0xFF); INC_PC(2);

#define FETCH_WORD(A)       \
		(A) = READ_BASED_PC; INC_PC(2);

#define FETCH_SWORD(A)      \
		(A) = (s16)READ_BASED_PC; INC_PC(2);

#define DECODE_EXT_WORD     \
	{                           \
	    u32 ext = READ_BASED_PC; INC_PC(2);       \
	    adr += (s8)(ext);                               \
	    if (ext & 0x0800) adr += DREGs32(ext >> 12);    \
	    else adr += DREGs16(ext >> 12);                 \
	}

#define READSX_BYTE_F(A, D)             \
    D = (s8)Read_Byte(A);

#define READSX_WORD_F(A, D)             \
    D = (s16)Read_Word(A);


#define WRITE_BYTE_F(A, D)      \
    Write_Byte(A, D);

#define WRITE_WORD_F(A, D)      \
    Write_Word(A, D);

#define PUSH_16_F(D)                    \
    Write_Word(AREG(7) -= 2, D);   \

#define POP_16_F(D)                     \
    D = (u16)Read_Word(AREG(7));   \
    AREG(7) += 2;

#define GET_CCR                                     \
    (((flag_C >> (M68K_SR_C_SFT - 0)) & 1) |   \
     ((flag_V >> (M68K_SR_V_SFT - 1)) & 2) |   \
     (((!flag_NotZ) & 1) << 2) |               \
     ((flag_N >> (M68K_SR_N_SFT - 3)) & 8) |   \
     ((flag_X >> (M68K_SR_X_SFT - 4)) & 0x10))

#ifdef FAME_EMULATE_TRACE
#define GET_SR                  \
    ((flag_S << 0)  |      \
     (flag_I << 8)  |      \
     (flag_T ) | \
     GET_CCR)
#else
#define GET_SR                  \
    ((flag_S << 0)  |      \
     (flag_I << 8)  |      \
     GET_CCR)
#endif

#define SET_CCR(A)                              \
    flag_C = (A) << (M68K_SR_C_SFT - 0);   \
    flag_V = (A) << (M68K_SR_V_SFT - 1);   \
    flag_NotZ = ~(A) & 4;                  \
    flag_N = (A) << (M68K_SR_N_SFT - 3);   \
    flag_X = (A) << (M68K_SR_X_SFT - 4);


#ifdef FAME_EMULATE_TRACE
#define SET_SR(A)                   \
    SET_CCR(A)                      \
    flag_T = (A) & M68K_SR_T; \
    flag_S = (A) & M68K_SR_S; \
    flag_I = ((A) >> 8) & 7;
#else
#define SET_SR(A)                   \
    SET_CCR(A)                      \
    flag_S = (A) & M68K_SR_S; \
    flag_I = ((A) >> 8) & 7;
#endif

#define CHECK_INT_TO_JUMP(CLK) \
	if (interrupt_chk()) \
	{ \
		/* \
		  si los ciclos restantes son menores o iguales \
		  que los de la instruccion en curso, \
		  no proceder a fijar el contador, pues cycles_needed \
		  sera negativo, haciendo que el calculo de ciclos \
		  ejecutados al final de emulate sea incorrecto \
		*/ \
		if(io_cycle_counter > (CLK)) \
		{ \
			cycles_needed=io_cycle_counter-(CLK); \
			io_cycle_counter=0;  \
		} \
	}

#define BANKEND_TAG ((u32)-1)

#define SETUP_FETCH_BANK(FNT, BANK) \
	{ \
		u32 i = 0; \
		while (BANK[i].low_addr != BANKEND_TAG) \
		{ \
			FNT(BANK[i].low_addr, BANK[i].high_addr, BANK[i].offset); \
			i++; \
		} \
	}

#define SETUP_DATA_BANK(FNT, BANK) \
	{ \
		u32 i = 0; \
		while (BANK[i].low_addr != BANKEND_TAG) \
		{ \
			FNT(BANK[i].low_addr, BANK[i].high_addr, BANK[i].mem_handler, BANK[i].data); \
			i++; \
		} \
	}


#ifdef FAME_CHECK_BRANCHES

#ifdef FAME_GOTOS
#define CHECK_BRANCH_EXCEPTION_GOTO_END goto famec_Exec_End;
#else
#define CHECK_BRANCH_EXCEPTION_GOTO_END io_cycle_counter=0; return;
#endif

#define CHECK_BRANCH_EXCEPTION(_PC_) \
	if ((_PC_)&1) \
	{ \
		u32 pr_PC=UNBASED_PC; \
		FAME_CONTEXT.execinfo |= M68K_EMULATE_GROUP_0; \
		execute_exception_group_0(M68K_ADDRESS_ERROR_EX, 0, pr_PC, 0x12 ); \
		CHECK_BRANCH_EXCEPTION_GOTO_END \
	}
#else
#define CHECK_BRANCH_EXCEPTION(_PC_)
#endif

#define BUILD_OPCODE_TABLE \
{ \
	u32 i, j; \
	for(i = 0x0000; i <= 0xFFFF; i += 0x0001) \
		JumpTable[0x0000 + i] = CAST_OP(0x4AFC); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0000 + i] = CAST_OP(0x0000); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0010 + i] = CAST_OP(0x0010); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0018 + i] = CAST_OP(0x0018); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0020 + i] = CAST_OP(0x0020); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0028 + i] = CAST_OP(0x0028); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0030 + i] = CAST_OP(0x0030); \
	JumpTable[0x0038] = CAST_OP(0x0038); \
	JumpTable[0x0039] = CAST_OP(0x0039); \
	JumpTable[0x001F] = CAST_OP(0x001F); \
	JumpTable[0x0027] = CAST_OP(0x0027); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0040 + i] = CAST_OP(0x0040); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0050 + i] = CAST_OP(0x0050); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0058 + i] = CAST_OP(0x0058); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0060 + i] = CAST_OP(0x0060); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0068 + i] = CAST_OP(0x0068); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0070 + i] = CAST_OP(0x0070); \
	JumpTable[0x0078] = CAST_OP(0x0078); \
	JumpTable[0x0079] = CAST_OP(0x0079); \
	JumpTable[0x005F] = CAST_OP(0x005F); \
	JumpTable[0x0067] = CAST_OP(0x0067); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0080 + i] = CAST_OP(0x0080); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0090 + i] = CAST_OP(0x0090); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0098 + i] = CAST_OP(0x0098); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x00A0 + i] = CAST_OP(0x00A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x00A8 + i] = CAST_OP(0x00A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x00B0 + i] = CAST_OP(0x00B0); \
	JumpTable[0x00B8] = CAST_OP(0x00B8); \
	JumpTable[0x00B9] = CAST_OP(0x00B9); \
	JumpTable[0x009F] = CAST_OP(0x009F); \
	JumpTable[0x00A7] = CAST_OP(0x00A7); \
	JumpTable[0x003C] = CAST_OP(0x003C); \
	JumpTable[0x007C] = CAST_OP(0x007C); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0200 + i] = CAST_OP(0x0200); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0210 + i] = CAST_OP(0x0210); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0218 + i] = CAST_OP(0x0218); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0220 + i] = CAST_OP(0x0220); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0228 + i] = CAST_OP(0x0228); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0230 + i] = CAST_OP(0x0230); \
	JumpTable[0x0238] = CAST_OP(0x0238); \
	JumpTable[0x0239] = CAST_OP(0x0239); \
	JumpTable[0x021F] = CAST_OP(0x021F); \
	JumpTable[0x0227] = CAST_OP(0x0227); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0240 + i] = CAST_OP(0x0240); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0250 + i] = CAST_OP(0x0250); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0258 + i] = CAST_OP(0x0258); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0260 + i] = CAST_OP(0x0260); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0268 + i] = CAST_OP(0x0268); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0270 + i] = CAST_OP(0x0270); \
	JumpTable[0x0278] = CAST_OP(0x0278); \
	JumpTable[0x0279] = CAST_OP(0x0279); \
	JumpTable[0x025F] = CAST_OP(0x025F); \
	JumpTable[0x0267] = CAST_OP(0x0267); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0280 + i] = CAST_OP(0x0280); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0290 + i] = CAST_OP(0x0290); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0298 + i] = CAST_OP(0x0298); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x02A0 + i] = CAST_OP(0x02A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x02A8 + i] = CAST_OP(0x02A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x02B0 + i] = CAST_OP(0x02B0); \
	JumpTable[0x02B8] = CAST_OP(0x02B8); \
	JumpTable[0x02B9] = CAST_OP(0x02B9); \
	JumpTable[0x029F] = CAST_OP(0x029F); \
	JumpTable[0x02A7] = CAST_OP(0x02A7); \
	JumpTable[0x023C] = CAST_OP(0x023C); \
	JumpTable[0x027C] = CAST_OP(0x027C); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0A00 + i] = CAST_OP(0x0A00); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0A10 + i] = CAST_OP(0x0A10); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0A18 + i] = CAST_OP(0x0A18); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0A20 + i] = CAST_OP(0x0A20); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0A28 + i] = CAST_OP(0x0A28); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0A30 + i] = CAST_OP(0x0A30); \
	JumpTable[0x0A38] = CAST_OP(0x0A38); \
	JumpTable[0x0A39] = CAST_OP(0x0A39); \
	JumpTable[0x0A1F] = CAST_OP(0x0A1F); \
	JumpTable[0x0A27] = CAST_OP(0x0A27); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0A40 + i] = CAST_OP(0x0A40); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0A50 + i] = CAST_OP(0x0A50); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0A58 + i] = CAST_OP(0x0A58); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0A60 + i] = CAST_OP(0x0A60); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0A68 + i] = CAST_OP(0x0A68); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0A70 + i] = CAST_OP(0x0A70); \
	JumpTable[0x0A78] = CAST_OP(0x0A78); \
	JumpTable[0x0A79] = CAST_OP(0x0A79); \
	JumpTable[0x0A5F] = CAST_OP(0x0A5F); \
	JumpTable[0x0A67] = CAST_OP(0x0A67); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0A80 + i] = CAST_OP(0x0A80); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0A90 + i] = CAST_OP(0x0A90); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0A98 + i] = CAST_OP(0x0A98); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0AA0 + i] = CAST_OP(0x0AA0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0AA8 + i] = CAST_OP(0x0AA8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0AB0 + i] = CAST_OP(0x0AB0); \
	JumpTable[0x0AB8] = CAST_OP(0x0AB8); \
	JumpTable[0x0AB9] = CAST_OP(0x0AB9); \
	JumpTable[0x0A9F] = CAST_OP(0x0A9F); \
	JumpTable[0x0AA7] = CAST_OP(0x0AA7); \
	JumpTable[0x0A3C] = CAST_OP(0x0A3C); \
	JumpTable[0x0A7C] = CAST_OP(0x0A7C); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0400 + i] = CAST_OP(0x0400); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0410 + i] = CAST_OP(0x0410); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0418 + i] = CAST_OP(0x0418); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0420 + i] = CAST_OP(0x0420); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0428 + i] = CAST_OP(0x0428); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0430 + i] = CAST_OP(0x0430); \
	JumpTable[0x0438] = CAST_OP(0x0438); \
	JumpTable[0x0439] = CAST_OP(0x0439); \
	JumpTable[0x041F] = CAST_OP(0x041F); \
	JumpTable[0x0427] = CAST_OP(0x0427); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0440 + i] = CAST_OP(0x0440); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0450 + i] = CAST_OP(0x0450); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0458 + i] = CAST_OP(0x0458); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0460 + i] = CAST_OP(0x0460); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0468 + i] = CAST_OP(0x0468); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0470 + i] = CAST_OP(0x0470); \
	JumpTable[0x0478] = CAST_OP(0x0478); \
	JumpTable[0x0479] = CAST_OP(0x0479); \
	JumpTable[0x045F] = CAST_OP(0x045F); \
	JumpTable[0x0467] = CAST_OP(0x0467); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0480 + i] = CAST_OP(0x0480); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0490 + i] = CAST_OP(0x0490); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0498 + i] = CAST_OP(0x0498); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x04A0 + i] = CAST_OP(0x04A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x04A8 + i] = CAST_OP(0x04A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x04B0 + i] = CAST_OP(0x04B0); \
	JumpTable[0x04B8] = CAST_OP(0x04B8); \
	JumpTable[0x04B9] = CAST_OP(0x04B9); \
	JumpTable[0x049F] = CAST_OP(0x049F); \
	JumpTable[0x04A7] = CAST_OP(0x04A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0600 + i] = CAST_OP(0x0600); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0610 + i] = CAST_OP(0x0610); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0618 + i] = CAST_OP(0x0618); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0620 + i] = CAST_OP(0x0620); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0628 + i] = CAST_OP(0x0628); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0630 + i] = CAST_OP(0x0630); \
	JumpTable[0x0638] = CAST_OP(0x0638); \
	JumpTable[0x0639] = CAST_OP(0x0639); \
	JumpTable[0x061F] = CAST_OP(0x061F); \
	JumpTable[0x0627] = CAST_OP(0x0627); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0640 + i] = CAST_OP(0x0640); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0650 + i] = CAST_OP(0x0650); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0658 + i] = CAST_OP(0x0658); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0660 + i] = CAST_OP(0x0660); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0668 + i] = CAST_OP(0x0668); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0670 + i] = CAST_OP(0x0670); \
	JumpTable[0x0678] = CAST_OP(0x0678); \
	JumpTable[0x0679] = CAST_OP(0x0679); \
	JumpTable[0x065F] = CAST_OP(0x065F); \
	JumpTable[0x0667] = CAST_OP(0x0667); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0680 + i] = CAST_OP(0x0680); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0690 + i] = CAST_OP(0x0690); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0698 + i] = CAST_OP(0x0698); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x06A0 + i] = CAST_OP(0x06A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x06A8 + i] = CAST_OP(0x06A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x06B0 + i] = CAST_OP(0x06B0); \
	JumpTable[0x06B8] = CAST_OP(0x06B8); \
	JumpTable[0x06B9] = CAST_OP(0x06B9); \
	JumpTable[0x069F] = CAST_OP(0x069F); \
	JumpTable[0x06A7] = CAST_OP(0x06A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0C00 + i] = CAST_OP(0x0C00); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0C10 + i] = CAST_OP(0x0C10); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0C18 + i] = CAST_OP(0x0C18); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0C20 + i] = CAST_OP(0x0C20); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0C28 + i] = CAST_OP(0x0C28); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0C30 + i] = CAST_OP(0x0C30); \
	JumpTable[0x0C38] = CAST_OP(0x0C38); \
	JumpTable[0x0C39] = CAST_OP(0x0C39); \
	JumpTable[0x0C1F] = CAST_OP(0x0C1F); \
	JumpTable[0x0C27] = CAST_OP(0x0C27); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0C40 + i] = CAST_OP(0x0C40); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0C50 + i] = CAST_OP(0x0C50); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0C58 + i] = CAST_OP(0x0C58); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0C60 + i] = CAST_OP(0x0C60); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0C68 + i] = CAST_OP(0x0C68); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0C70 + i] = CAST_OP(0x0C70); \
	JumpTable[0x0C78] = CAST_OP(0x0C78); \
	JumpTable[0x0C79] = CAST_OP(0x0C79); \
	JumpTable[0x0C5F] = CAST_OP(0x0C5F); \
	JumpTable[0x0C67] = CAST_OP(0x0C67); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0C80 + i] = CAST_OP(0x0C80); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0C90 + i] = CAST_OP(0x0C90); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0C98 + i] = CAST_OP(0x0C98); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0CA0 + i] = CAST_OP(0x0CA0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0CA8 + i] = CAST_OP(0x0CA8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0CB0 + i] = CAST_OP(0x0CB0); \
	JumpTable[0x0CB8] = CAST_OP(0x0CB8); \
	JumpTable[0x0CB9] = CAST_OP(0x0CB9); \
	JumpTable[0x0C9F] = CAST_OP(0x0C9F); \
	JumpTable[0x0CA7] = CAST_OP(0x0CA7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0800 + i] = CAST_OP(0x0800); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0810 + i] = CAST_OP(0x0810); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0818 + i] = CAST_OP(0x0818); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0820 + i] = CAST_OP(0x0820); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0828 + i] = CAST_OP(0x0828); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0830 + i] = CAST_OP(0x0830); \
	JumpTable[0x0838] = CAST_OP(0x0838); \
	JumpTable[0x0839] = CAST_OP(0x0839); \
	JumpTable[0x083A] = CAST_OP(0x083A); \
	JumpTable[0x083B] = CAST_OP(0x083B); \
	JumpTable[0x081F] = CAST_OP(0x081F); \
	JumpTable[0x0827] = CAST_OP(0x0827); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0840 + i] = CAST_OP(0x0840); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0850 + i] = CAST_OP(0x0850); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0858 + i] = CAST_OP(0x0858); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0860 + i] = CAST_OP(0x0860); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0868 + i] = CAST_OP(0x0868); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0870 + i] = CAST_OP(0x0870); \
	JumpTable[0x0878] = CAST_OP(0x0878); \
	JumpTable[0x0879] = CAST_OP(0x0879); \
	JumpTable[0x085F] = CAST_OP(0x085F); \
	JumpTable[0x0867] = CAST_OP(0x0867); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0880 + i] = CAST_OP(0x0880); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x0890 + i] = CAST_OP(0x0890); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x0898 + i] = CAST_OP(0x0898); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x08A0 + i] = CAST_OP(0x08A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x08A8 + i] = CAST_OP(0x08A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x08B0 + i] = CAST_OP(0x08B0); \
	JumpTable[0x08B8] = CAST_OP(0x08B8); \
	JumpTable[0x08B9] = CAST_OP(0x08B9); \
	JumpTable[0x089F] = CAST_OP(0x089F); \
	JumpTable[0x08A7] = CAST_OP(0x08A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x08C0 + i] = CAST_OP(0x08C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x08D0 + i] = CAST_OP(0x08D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x08D8 + i] = CAST_OP(0x08D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x08E0 + i] = CAST_OP(0x08E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x08E8 + i] = CAST_OP(0x08E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x08F0 + i] = CAST_OP(0x08F0); \
	JumpTable[0x08F8] = CAST_OP(0x08F8); \
	JumpTable[0x08F9] = CAST_OP(0x08F9); \
	JumpTable[0x08DF] = CAST_OP(0x08DF); \
	JumpTable[0x08E7] = CAST_OP(0x08E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0100 + i + j] = CAST_OP(0x0100); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0110 + i + j] = CAST_OP(0x0110); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0118 + i + j] = CAST_OP(0x0118); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0120 + i + j] = CAST_OP(0x0120); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0128 + i + j] = CAST_OP(0x0128); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0130 + i + j] = CAST_OP(0x0130); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x0138 + i] = CAST_OP(0x0138); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x0139 + i] = CAST_OP(0x0139); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x013A + i] = CAST_OP(0x013A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x013B + i] = CAST_OP(0x013B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x013C + i] = CAST_OP(0x013C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x011F + i] = CAST_OP(0x011F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x0127 + i] = CAST_OP(0x0127); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0140 + i + j] = CAST_OP(0x0140); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0150 + i + j] = CAST_OP(0x0150); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0158 + i + j] = CAST_OP(0x0158); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0160 + i + j] = CAST_OP(0x0160); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0168 + i + j] = CAST_OP(0x0168); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0170 + i + j] = CAST_OP(0x0170); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x0178 + i] = CAST_OP(0x0178); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x0179 + i] = CAST_OP(0x0179); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x015F + i] = CAST_OP(0x015F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x0167 + i] = CAST_OP(0x0167); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0180 + i + j] = CAST_OP(0x0180); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0190 + i + j] = CAST_OP(0x0190); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0198 + i + j] = CAST_OP(0x0198); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x01A0 + i + j] = CAST_OP(0x01A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x01A8 + i + j] = CAST_OP(0x01A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x01B0 + i + j] = CAST_OP(0x01B0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x01B8 + i] = CAST_OP(0x01B8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x01B9 + i] = CAST_OP(0x01B9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x019F + i] = CAST_OP(0x019F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x01A7 + i] = CAST_OP(0x01A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x01C0 + i + j] = CAST_OP(0x01C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x01D0 + i + j] = CAST_OP(0x01D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x01D8 + i + j] = CAST_OP(0x01D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x01E0 + i + j] = CAST_OP(0x01E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x01E8 + i + j] = CAST_OP(0x01E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x01F0 + i + j] = CAST_OP(0x01F0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x01F8 + i] = CAST_OP(0x01F8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x01F9 + i] = CAST_OP(0x01F9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x01DF + i] = CAST_OP(0x01DF); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x01E7 + i] = CAST_OP(0x01E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0108 + i + j] = CAST_OP(0x0108); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0148 + i + j] = CAST_OP(0x0148); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x0188 + i + j] = CAST_OP(0x0188); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x01C8 + i + j] = CAST_OP(0x01C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1000 + i + j] = CAST_OP(0x1000); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1080 + i + j] = CAST_OP(0x1080); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x10C0 + i + j] = CAST_OP(0x10C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x1100 + i + j] = CAST_OP(0x1100); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1140 + i + j] = CAST_OP(0x1140); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1180 + i + j] = CAST_OP(0x1180); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x11C0 + i] = CAST_OP(0x11C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x13C0 + i] = CAST_OP(0x13C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x1EC0 + i] = CAST_OP(0x1EC0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x1F00 + i] = CAST_OP(0x1F00); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1008 + i + j] = CAST_OP(0x1008); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1088 + i + j] = CAST_OP(0x1088); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x10C8 + i + j] = CAST_OP(0x10C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x1108 + i + j] = CAST_OP(0x1108); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1148 + i + j] = CAST_OP(0x1148); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1188 + i + j] = CAST_OP(0x1188); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x11C8 + i] = CAST_OP(0x11C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x13C8 + i] = CAST_OP(0x13C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x1EC8 + i] = CAST_OP(0x1EC8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x1F08 + i] = CAST_OP(0x1F08); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1010 + i + j] = CAST_OP(0x1010); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1090 + i + j] = CAST_OP(0x1090); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x10D0 + i + j] = CAST_OP(0x10D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x1110 + i + j] = CAST_OP(0x1110); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1150 + i + j] = CAST_OP(0x1150); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1190 + i + j] = CAST_OP(0x1190); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x11D0 + i] = CAST_OP(0x11D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x13D0 + i] = CAST_OP(0x13D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x1ED0 + i] = CAST_OP(0x1ED0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x1F10 + i] = CAST_OP(0x1F10); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1018 + i + j] = CAST_OP(0x1018); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1098 + i + j] = CAST_OP(0x1098); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x10D8 + i + j] = CAST_OP(0x10D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x1118 + i + j] = CAST_OP(0x1118); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1158 + i + j] = CAST_OP(0x1158); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1198 + i + j] = CAST_OP(0x1198); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x11D8 + i] = CAST_OP(0x11D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x13D8 + i] = CAST_OP(0x13D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x1ED8 + i] = CAST_OP(0x1ED8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x1F18 + i] = CAST_OP(0x1F18); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1020 + i + j] = CAST_OP(0x1020); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x10A0 + i + j] = CAST_OP(0x10A0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x10E0 + i + j] = CAST_OP(0x10E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x1120 + i + j] = CAST_OP(0x1120); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1160 + i + j] = CAST_OP(0x1160); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x11A0 + i + j] = CAST_OP(0x11A0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x11E0 + i] = CAST_OP(0x11E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x13E0 + i] = CAST_OP(0x13E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x1EE0 + i] = CAST_OP(0x1EE0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x1F20 + i] = CAST_OP(0x1F20); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1028 + i + j] = CAST_OP(0x1028); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x10A8 + i + j] = CAST_OP(0x10A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x10E8 + i + j] = CAST_OP(0x10E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x1128 + i + j] = CAST_OP(0x1128); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1168 + i + j] = CAST_OP(0x1168); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x11A8 + i + j] = CAST_OP(0x11A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x11E8 + i] = CAST_OP(0x11E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x13E8 + i] = CAST_OP(0x13E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x1EE8 + i] = CAST_OP(0x1EE8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x1F28 + i] = CAST_OP(0x1F28); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1030 + i + j] = CAST_OP(0x1030); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x10B0 + i + j] = CAST_OP(0x10B0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x10F0 + i + j] = CAST_OP(0x10F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x1130 + i + j] = CAST_OP(0x1130); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x1170 + i + j] = CAST_OP(0x1170); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x11B0 + i + j] = CAST_OP(0x11B0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x11F0 + i] = CAST_OP(0x11F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x13F0 + i] = CAST_OP(0x13F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x1EF0 + i] = CAST_OP(0x1EF0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x1F30 + i] = CAST_OP(0x1F30); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x1038 + i] = CAST_OP(0x1038); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x10B8 + i] = CAST_OP(0x10B8); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x10F8 + i] = CAST_OP(0x10F8); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x1138 + i] = CAST_OP(0x1138); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x1178 + i] = CAST_OP(0x1178); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x11B8 + i] = CAST_OP(0x11B8); \
	JumpTable[0x11F8] = CAST_OP(0x11F8); \
	JumpTable[0x13F8] = CAST_OP(0x13F8); \
	JumpTable[0x1EF8] = CAST_OP(0x1EF8); \
	JumpTable[0x1F38] = CAST_OP(0x1F38); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x1039 + i] = CAST_OP(0x1039); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x10B9 + i] = CAST_OP(0x10B9); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x10F9 + i] = CAST_OP(0x10F9); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x1139 + i] = CAST_OP(0x1139); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x1179 + i] = CAST_OP(0x1179); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x11B9 + i] = CAST_OP(0x11B9); \
	JumpTable[0x11F9] = CAST_OP(0x11F9); \
	JumpTable[0x13F9] = CAST_OP(0x13F9); \
	JumpTable[0x1EF9] = CAST_OP(0x1EF9); \
	JumpTable[0x1F39] = CAST_OP(0x1F39); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x103A + i] = CAST_OP(0x103A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x10BA + i] = CAST_OP(0x10BA); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x10FA + i] = CAST_OP(0x10FA); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x113A + i] = CAST_OP(0x113A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x117A + i] = CAST_OP(0x117A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x11BA + i] = CAST_OP(0x11BA); \
	JumpTable[0x11FA] = CAST_OP(0x11FA); \
	JumpTable[0x13FA] = CAST_OP(0x13FA); \
	JumpTable[0x1EFA] = CAST_OP(0x1EFA); \
	JumpTable[0x1F3A] = CAST_OP(0x1F3A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x103B + i] = CAST_OP(0x103B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x10BB + i] = CAST_OP(0x10BB); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x10FB + i] = CAST_OP(0x10FB); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x113B + i] = CAST_OP(0x113B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x117B + i] = CAST_OP(0x117B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x11BB + i] = CAST_OP(0x11BB); \
	JumpTable[0x11FB] = CAST_OP(0x11FB); \
	JumpTable[0x13FB] = CAST_OP(0x13FB); \
	JumpTable[0x1EFB] = CAST_OP(0x1EFB); \
	JumpTable[0x1F3B] = CAST_OP(0x1F3B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x103C + i] = CAST_OP(0x103C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x10BC + i] = CAST_OP(0x10BC); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x10FC + i] = CAST_OP(0x10FC); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x113C + i] = CAST_OP(0x113C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x117C + i] = CAST_OP(0x117C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x11BC + i] = CAST_OP(0x11BC); \
	JumpTable[0x11FC] = CAST_OP(0x11FC); \
	JumpTable[0x13FC] = CAST_OP(0x13FC); \
	JumpTable[0x1EFC] = CAST_OP(0x1EFC); \
	JumpTable[0x1F3C] = CAST_OP(0x1F3C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x101F + i] = CAST_OP(0x101F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x109F + i] = CAST_OP(0x109F); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x10DF + i] = CAST_OP(0x10DF); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x111F + i] = CAST_OP(0x111F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x115F + i] = CAST_OP(0x115F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x119F + i] = CAST_OP(0x119F); \
	JumpTable[0x11DF] = CAST_OP(0x11DF); \
	JumpTable[0x13DF] = CAST_OP(0x13DF); \
	JumpTable[0x1EDF] = CAST_OP(0x1EDF); \
	JumpTable[0x1F1F] = CAST_OP(0x1F1F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x1027 + i] = CAST_OP(0x1027); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x10A7 + i] = CAST_OP(0x10A7); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x10E7 + i] = CAST_OP(0x10E7); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x1127 + i] = CAST_OP(0x1127); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x1167 + i] = CAST_OP(0x1167); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x11A7 + i] = CAST_OP(0x11A7); \
	JumpTable[0x11E7] = CAST_OP(0x11E7); \
	JumpTable[0x13E7] = CAST_OP(0x13E7); \
	JumpTable[0x1EE7] = CAST_OP(0x1EE7); \
	JumpTable[0x1F27] = CAST_OP(0x1F27); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2000 + i + j] = CAST_OP(0x2000); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2080 + i + j] = CAST_OP(0x2080); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x20C0 + i + j] = CAST_OP(0x20C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x2100 + i + j] = CAST_OP(0x2100); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2140 + i + j] = CAST_OP(0x2140); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2180 + i + j] = CAST_OP(0x2180); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x21C0 + i] = CAST_OP(0x21C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x23C0 + i] = CAST_OP(0x23C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x2EC0 + i] = CAST_OP(0x2EC0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x2F00 + i] = CAST_OP(0x2F00); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2008 + i + j] = CAST_OP(0x2008); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2088 + i + j] = CAST_OP(0x2088); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x20C8 + i + j] = CAST_OP(0x20C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x2108 + i + j] = CAST_OP(0x2108); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2148 + i + j] = CAST_OP(0x2148); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2188 + i + j] = CAST_OP(0x2188); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x21C8 + i] = CAST_OP(0x21C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x23C8 + i] = CAST_OP(0x23C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x2EC8 + i] = CAST_OP(0x2EC8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x2F08 + i] = CAST_OP(0x2F08); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2010 + i + j] = CAST_OP(0x2010); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2090 + i + j] = CAST_OP(0x2090); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x20D0 + i + j] = CAST_OP(0x20D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x2110 + i + j] = CAST_OP(0x2110); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2150 + i + j] = CAST_OP(0x2150); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2190 + i + j] = CAST_OP(0x2190); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x21D0 + i] = CAST_OP(0x21D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x23D0 + i] = CAST_OP(0x23D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x2ED0 + i] = CAST_OP(0x2ED0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x2F10 + i] = CAST_OP(0x2F10); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2018 + i + j] = CAST_OP(0x2018); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2098 + i + j] = CAST_OP(0x2098); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x20D8 + i + j] = CAST_OP(0x20D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x2118 + i + j] = CAST_OP(0x2118); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2158 + i + j] = CAST_OP(0x2158); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2198 + i + j] = CAST_OP(0x2198); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x21D8 + i] = CAST_OP(0x21D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x23D8 + i] = CAST_OP(0x23D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x2ED8 + i] = CAST_OP(0x2ED8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x2F18 + i] = CAST_OP(0x2F18); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2020 + i + j] = CAST_OP(0x2020); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x20A0 + i + j] = CAST_OP(0x20A0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x20E0 + i + j] = CAST_OP(0x20E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x2120 + i + j] = CAST_OP(0x2120); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2160 + i + j] = CAST_OP(0x2160); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x21A0 + i + j] = CAST_OP(0x21A0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x21E0 + i] = CAST_OP(0x21E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x23E0 + i] = CAST_OP(0x23E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x2EE0 + i] = CAST_OP(0x2EE0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x2F20 + i] = CAST_OP(0x2F20); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2028 + i + j] = CAST_OP(0x2028); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x20A8 + i + j] = CAST_OP(0x20A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x20E8 + i + j] = CAST_OP(0x20E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x2128 + i + j] = CAST_OP(0x2128); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2168 + i + j] = CAST_OP(0x2168); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x21A8 + i + j] = CAST_OP(0x21A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x21E8 + i] = CAST_OP(0x21E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x23E8 + i] = CAST_OP(0x23E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x2EE8 + i] = CAST_OP(0x2EE8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x2F28 + i] = CAST_OP(0x2F28); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2030 + i + j] = CAST_OP(0x2030); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x20B0 + i + j] = CAST_OP(0x20B0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x20F0 + i + j] = CAST_OP(0x20F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x2130 + i + j] = CAST_OP(0x2130); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2170 + i + j] = CAST_OP(0x2170); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x21B0 + i + j] = CAST_OP(0x21B0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x21F0 + i] = CAST_OP(0x21F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x23F0 + i] = CAST_OP(0x23F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x2EF0 + i] = CAST_OP(0x2EF0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x2F30 + i] = CAST_OP(0x2F30); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x2038 + i] = CAST_OP(0x2038); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x20B8 + i] = CAST_OP(0x20B8); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x20F8 + i] = CAST_OP(0x20F8); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x2138 + i] = CAST_OP(0x2138); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x2178 + i] = CAST_OP(0x2178); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x21B8 + i] = CAST_OP(0x21B8); \
	JumpTable[0x21F8] = CAST_OP(0x21F8); \
	JumpTable[0x23F8] = CAST_OP(0x23F8); \
	JumpTable[0x2EF8] = CAST_OP(0x2EF8); \
	JumpTable[0x2F38] = CAST_OP(0x2F38); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x2039 + i] = CAST_OP(0x2039); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x20B9 + i] = CAST_OP(0x20B9); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x20F9 + i] = CAST_OP(0x20F9); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x2139 + i] = CAST_OP(0x2139); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x2179 + i] = CAST_OP(0x2179); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x21B9 + i] = CAST_OP(0x21B9); \
	JumpTable[0x21F9] = CAST_OP(0x21F9); \
	JumpTable[0x23F9] = CAST_OP(0x23F9); \
	JumpTable[0x2EF9] = CAST_OP(0x2EF9); \
	JumpTable[0x2F39] = CAST_OP(0x2F39); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x203A + i] = CAST_OP(0x203A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x20BA + i] = CAST_OP(0x20BA); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x20FA + i] = CAST_OP(0x20FA); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x213A + i] = CAST_OP(0x213A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x217A + i] = CAST_OP(0x217A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x21BA + i] = CAST_OP(0x21BA); \
	JumpTable[0x21FA] = CAST_OP(0x21FA); \
	JumpTable[0x23FA] = CAST_OP(0x23FA); \
	JumpTable[0x2EFA] = CAST_OP(0x2EFA); \
	JumpTable[0x2F3A] = CAST_OP(0x2F3A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x203B + i] = CAST_OP(0x203B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x20BB + i] = CAST_OP(0x20BB); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x20FB + i] = CAST_OP(0x20FB); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x213B + i] = CAST_OP(0x213B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x217B + i] = CAST_OP(0x217B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x21BB + i] = CAST_OP(0x21BB); \
	JumpTable[0x21FB] = CAST_OP(0x21FB); \
	JumpTable[0x23FB] = CAST_OP(0x23FB); \
	JumpTable[0x2EFB] = CAST_OP(0x2EFB); \
	JumpTable[0x2F3B] = CAST_OP(0x2F3B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x203C + i] = CAST_OP(0x203C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x20BC + i] = CAST_OP(0x20BC); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x20FC + i] = CAST_OP(0x20FC); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x213C + i] = CAST_OP(0x213C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x217C + i] = CAST_OP(0x217C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x21BC + i] = CAST_OP(0x21BC); \
	JumpTable[0x21FC] = CAST_OP(0x21FC); \
	JumpTable[0x23FC] = CAST_OP(0x23FC); \
	JumpTable[0x2EFC] = CAST_OP(0x2EFC); \
	JumpTable[0x2F3C] = CAST_OP(0x2F3C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x201F + i] = CAST_OP(0x201F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x209F + i] = CAST_OP(0x209F); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x20DF + i] = CAST_OP(0x20DF); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x211F + i] = CAST_OP(0x211F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x215F + i] = CAST_OP(0x215F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x219F + i] = CAST_OP(0x219F); \
	JumpTable[0x21DF] = CAST_OP(0x21DF); \
	JumpTable[0x23DF] = CAST_OP(0x23DF); \
	JumpTable[0x2EDF] = CAST_OP(0x2EDF); \
	JumpTable[0x2F1F] = CAST_OP(0x2F1F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x2027 + i] = CAST_OP(0x2027); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x20A7 + i] = CAST_OP(0x20A7); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x20E7 + i] = CAST_OP(0x20E7); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x2127 + i] = CAST_OP(0x2127); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x2167 + i] = CAST_OP(0x2167); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x21A7 + i] = CAST_OP(0x21A7); \
	JumpTable[0x21E7] = CAST_OP(0x21E7); \
	JumpTable[0x23E7] = CAST_OP(0x23E7); \
	JumpTable[0x2EE7] = CAST_OP(0x2EE7); \
	JumpTable[0x2F27] = CAST_OP(0x2F27); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3000 + i + j] = CAST_OP(0x3000); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3080 + i + j] = CAST_OP(0x3080); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x30C0 + i + j] = CAST_OP(0x30C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x3100 + i + j] = CAST_OP(0x3100); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3140 + i + j] = CAST_OP(0x3140); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3180 + i + j] = CAST_OP(0x3180); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x31C0 + i] = CAST_OP(0x31C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x33C0 + i] = CAST_OP(0x33C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x3EC0 + i] = CAST_OP(0x3EC0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x3F00 + i] = CAST_OP(0x3F00); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3008 + i + j] = CAST_OP(0x3008); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3088 + i + j] = CAST_OP(0x3088); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x30C8 + i + j] = CAST_OP(0x30C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x3108 + i + j] = CAST_OP(0x3108); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3148 + i + j] = CAST_OP(0x3148); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3188 + i + j] = CAST_OP(0x3188); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x31C8 + i] = CAST_OP(0x31C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x33C8 + i] = CAST_OP(0x33C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x3EC8 + i] = CAST_OP(0x3EC8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x3F08 + i] = CAST_OP(0x3F08); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3010 + i + j] = CAST_OP(0x3010); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3090 + i + j] = CAST_OP(0x3090); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x30D0 + i + j] = CAST_OP(0x30D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x3110 + i + j] = CAST_OP(0x3110); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3150 + i + j] = CAST_OP(0x3150); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3190 + i + j] = CAST_OP(0x3190); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x31D0 + i] = CAST_OP(0x31D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x33D0 + i] = CAST_OP(0x33D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x3ED0 + i] = CAST_OP(0x3ED0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x3F10 + i] = CAST_OP(0x3F10); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3018 + i + j] = CAST_OP(0x3018); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3098 + i + j] = CAST_OP(0x3098); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x30D8 + i + j] = CAST_OP(0x30D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x3118 + i + j] = CAST_OP(0x3118); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3158 + i + j] = CAST_OP(0x3158); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3198 + i + j] = CAST_OP(0x3198); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x31D8 + i] = CAST_OP(0x31D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x33D8 + i] = CAST_OP(0x33D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x3ED8 + i] = CAST_OP(0x3ED8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x3F18 + i] = CAST_OP(0x3F18); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3020 + i + j] = CAST_OP(0x3020); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x30A0 + i + j] = CAST_OP(0x30A0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x30E0 + i + j] = CAST_OP(0x30E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x3120 + i + j] = CAST_OP(0x3120); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3160 + i + j] = CAST_OP(0x3160); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x31A0 + i + j] = CAST_OP(0x31A0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x31E0 + i] = CAST_OP(0x31E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x33E0 + i] = CAST_OP(0x33E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x3EE0 + i] = CAST_OP(0x3EE0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x3F20 + i] = CAST_OP(0x3F20); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3028 + i + j] = CAST_OP(0x3028); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x30A8 + i + j] = CAST_OP(0x30A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x30E8 + i + j] = CAST_OP(0x30E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x3128 + i + j] = CAST_OP(0x3128); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3168 + i + j] = CAST_OP(0x3168); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x31A8 + i + j] = CAST_OP(0x31A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x31E8 + i] = CAST_OP(0x31E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x33E8 + i] = CAST_OP(0x33E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x3EE8 + i] = CAST_OP(0x3EE8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x3F28 + i] = CAST_OP(0x3F28); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3030 + i + j] = CAST_OP(0x3030); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x30B0 + i + j] = CAST_OP(0x30B0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x30F0 + i + j] = CAST_OP(0x30F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x3130 + i + j] = CAST_OP(0x3130); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3170 + i + j] = CAST_OP(0x3170); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x31B0 + i + j] = CAST_OP(0x31B0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x31F0 + i] = CAST_OP(0x31F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x33F0 + i] = CAST_OP(0x33F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x3EF0 + i] = CAST_OP(0x3EF0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x3F30 + i] = CAST_OP(0x3F30); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x3038 + i] = CAST_OP(0x3038); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x30B8 + i] = CAST_OP(0x30B8); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x30F8 + i] = CAST_OP(0x30F8); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x3138 + i] = CAST_OP(0x3138); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x3178 + i] = CAST_OP(0x3178); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x31B8 + i] = CAST_OP(0x31B8); \
	JumpTable[0x31F8] = CAST_OP(0x31F8); \
	JumpTable[0x33F8] = CAST_OP(0x33F8); \
	JumpTable[0x3EF8] = CAST_OP(0x3EF8); \
	JumpTable[0x3F38] = CAST_OP(0x3F38); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x3039 + i] = CAST_OP(0x3039); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x30B9 + i] = CAST_OP(0x30B9); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x30F9 + i] = CAST_OP(0x30F9); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x3139 + i] = CAST_OP(0x3139); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x3179 + i] = CAST_OP(0x3179); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x31B9 + i] = CAST_OP(0x31B9); \
	JumpTable[0x31F9] = CAST_OP(0x31F9); \
	JumpTable[0x33F9] = CAST_OP(0x33F9); \
	JumpTable[0x3EF9] = CAST_OP(0x3EF9); \
	JumpTable[0x3F39] = CAST_OP(0x3F39); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x303A + i] = CAST_OP(0x303A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x30BA + i] = CAST_OP(0x30BA); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x30FA + i] = CAST_OP(0x30FA); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x313A + i] = CAST_OP(0x313A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x317A + i] = CAST_OP(0x317A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x31BA + i] = CAST_OP(0x31BA); \
	JumpTable[0x31FA] = CAST_OP(0x31FA); \
	JumpTable[0x33FA] = CAST_OP(0x33FA); \
	JumpTable[0x3EFA] = CAST_OP(0x3EFA); \
	JumpTable[0x3F3A] = CAST_OP(0x3F3A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x303B + i] = CAST_OP(0x303B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x30BB + i] = CAST_OP(0x30BB); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x30FB + i] = CAST_OP(0x30FB); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x313B + i] = CAST_OP(0x313B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x317B + i] = CAST_OP(0x317B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x31BB + i] = CAST_OP(0x31BB); \
	JumpTable[0x31FB] = CAST_OP(0x31FB); \
	JumpTable[0x33FB] = CAST_OP(0x33FB); \
	JumpTable[0x3EFB] = CAST_OP(0x3EFB); \
	JumpTable[0x3F3B] = CAST_OP(0x3F3B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x303C + i] = CAST_OP(0x303C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x30BC + i] = CAST_OP(0x30BC); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x30FC + i] = CAST_OP(0x30FC); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x313C + i] = CAST_OP(0x313C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x317C + i] = CAST_OP(0x317C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x31BC + i] = CAST_OP(0x31BC); \
	JumpTable[0x31FC] = CAST_OP(0x31FC); \
	JumpTable[0x33FC] = CAST_OP(0x33FC); \
	JumpTable[0x3EFC] = CAST_OP(0x3EFC); \
	JumpTable[0x3F3C] = CAST_OP(0x3F3C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x301F + i] = CAST_OP(0x301F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x309F + i] = CAST_OP(0x309F); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x30DF + i] = CAST_OP(0x30DF); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x311F + i] = CAST_OP(0x311F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x315F + i] = CAST_OP(0x315F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x319F + i] = CAST_OP(0x319F); \
	JumpTable[0x31DF] = CAST_OP(0x31DF); \
	JumpTable[0x33DF] = CAST_OP(0x33DF); \
	JumpTable[0x3EDF] = CAST_OP(0x3EDF); \
	JumpTable[0x3F1F] = CAST_OP(0x3F1F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x3027 + i] = CAST_OP(0x3027); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x30A7 + i] = CAST_OP(0x30A7); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x30E7 + i] = CAST_OP(0x30E7); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x3127 + i] = CAST_OP(0x3127); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x3167 + i] = CAST_OP(0x3167); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x31A7 + i] = CAST_OP(0x31A7); \
	JumpTable[0x31E7] = CAST_OP(0x31E7); \
	JumpTable[0x33E7] = CAST_OP(0x33E7); \
	JumpTable[0x3EE7] = CAST_OP(0x3EE7); \
	JumpTable[0x3F27] = CAST_OP(0x3F27); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2040 + i + j] = CAST_OP(0x2040); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2048 + i + j] = CAST_OP(0x2048); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2050 + i + j] = CAST_OP(0x2050); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2058 + i + j] = CAST_OP(0x2058); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2060 + i + j] = CAST_OP(0x2060); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2068 + i + j] = CAST_OP(0x2068); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x2070 + i + j] = CAST_OP(0x2070); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x2078 + i] = CAST_OP(0x2078); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x2079 + i] = CAST_OP(0x2079); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x207A + i] = CAST_OP(0x207A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x207B + i] = CAST_OP(0x207B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x207C + i] = CAST_OP(0x207C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x205F + i] = CAST_OP(0x205F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x2067 + i] = CAST_OP(0x2067); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3040 + i + j] = CAST_OP(0x3040); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3048 + i + j] = CAST_OP(0x3048); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3050 + i + j] = CAST_OP(0x3050); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3058 + i + j] = CAST_OP(0x3058); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3060 + i + j] = CAST_OP(0x3060); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3068 + i + j] = CAST_OP(0x3068); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x3070 + i + j] = CAST_OP(0x3070); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x3078 + i] = CAST_OP(0x3078); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x3079 + i] = CAST_OP(0x3079); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x307A + i] = CAST_OP(0x307A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x307B + i] = CAST_OP(0x307B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x307C + i] = CAST_OP(0x307C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x305F + i] = CAST_OP(0x305F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x3067 + i] = CAST_OP(0x3067); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4000 + i] = CAST_OP(0x4000); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4010 + i] = CAST_OP(0x4010); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4018 + i] = CAST_OP(0x4018); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4020 + i] = CAST_OP(0x4020); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4028 + i] = CAST_OP(0x4028); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4030 + i] = CAST_OP(0x4030); \
	JumpTable[0x4038] = CAST_OP(0x4038); \
	JumpTable[0x4039] = CAST_OP(0x4039); \
	JumpTable[0x401F] = CAST_OP(0x401F); \
	JumpTable[0x4027] = CAST_OP(0x4027); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4040 + i] = CAST_OP(0x4040); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4050 + i] = CAST_OP(0x4050); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4058 + i] = CAST_OP(0x4058); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4060 + i] = CAST_OP(0x4060); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4068 + i] = CAST_OP(0x4068); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4070 + i] = CAST_OP(0x4070); \
	JumpTable[0x4078] = CAST_OP(0x4078); \
	JumpTable[0x4079] = CAST_OP(0x4079); \
	JumpTable[0x405F] = CAST_OP(0x405F); \
	JumpTable[0x4067] = CAST_OP(0x4067); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4080 + i] = CAST_OP(0x4080); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4090 + i] = CAST_OP(0x4090); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4098 + i] = CAST_OP(0x4098); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x40A0 + i] = CAST_OP(0x40A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x40A8 + i] = CAST_OP(0x40A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x40B0 + i] = CAST_OP(0x40B0); \
	JumpTable[0x40B8] = CAST_OP(0x40B8); \
	JumpTable[0x40B9] = CAST_OP(0x40B9); \
	JumpTable[0x409F] = CAST_OP(0x409F); \
	JumpTable[0x40A7] = CAST_OP(0x40A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4200 + i] = CAST_OP(0x4200); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4210 + i] = CAST_OP(0x4210); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4218 + i] = CAST_OP(0x4218); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4220 + i] = CAST_OP(0x4220); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4228 + i] = CAST_OP(0x4228); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4230 + i] = CAST_OP(0x4230); \
	JumpTable[0x4238] = CAST_OP(0x4238); \
	JumpTable[0x4239] = CAST_OP(0x4239); \
	JumpTable[0x421F] = CAST_OP(0x421F); \
	JumpTable[0x4227] = CAST_OP(0x4227); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4240 + i] = CAST_OP(0x4240); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4250 + i] = CAST_OP(0x4250); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4258 + i] = CAST_OP(0x4258); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4260 + i] = CAST_OP(0x4260); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4268 + i] = CAST_OP(0x4268); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4270 + i] = CAST_OP(0x4270); \
	JumpTable[0x4278] = CAST_OP(0x4278); \
	JumpTable[0x4279] = CAST_OP(0x4279); \
	JumpTable[0x425F] = CAST_OP(0x425F); \
	JumpTable[0x4267] = CAST_OP(0x4267); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4280 + i] = CAST_OP(0x4280); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4290 + i] = CAST_OP(0x4290); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4298 + i] = CAST_OP(0x4298); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x42A0 + i] = CAST_OP(0x42A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x42A8 + i] = CAST_OP(0x42A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x42B0 + i] = CAST_OP(0x42B0); \
	JumpTable[0x42B8] = CAST_OP(0x42B8); \
	JumpTable[0x42B9] = CAST_OP(0x42B9); \
	JumpTable[0x429F] = CAST_OP(0x429F); \
	JumpTable[0x42A7] = CAST_OP(0x42A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4400 + i] = CAST_OP(0x4400); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4410 + i] = CAST_OP(0x4410); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4418 + i] = CAST_OP(0x4418); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4420 + i] = CAST_OP(0x4420); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4428 + i] = CAST_OP(0x4428); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4430 + i] = CAST_OP(0x4430); \
	JumpTable[0x4438] = CAST_OP(0x4438); \
	JumpTable[0x4439] = CAST_OP(0x4439); \
	JumpTable[0x441F] = CAST_OP(0x441F); \
	JumpTable[0x4427] = CAST_OP(0x4427); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4440 + i] = CAST_OP(0x4440); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4450 + i] = CAST_OP(0x4450); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4458 + i] = CAST_OP(0x4458); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4460 + i] = CAST_OP(0x4460); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4468 + i] = CAST_OP(0x4468); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4470 + i] = CAST_OP(0x4470); \
	JumpTable[0x4478] = CAST_OP(0x4478); \
	JumpTable[0x4479] = CAST_OP(0x4479); \
	JumpTable[0x445F] = CAST_OP(0x445F); \
	JumpTable[0x4467] = CAST_OP(0x4467); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4480 + i] = CAST_OP(0x4480); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4490 + i] = CAST_OP(0x4490); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4498 + i] = CAST_OP(0x4498); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x44A0 + i] = CAST_OP(0x44A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x44A8 + i] = CAST_OP(0x44A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x44B0 + i] = CAST_OP(0x44B0); \
	JumpTable[0x44B8] = CAST_OP(0x44B8); \
	JumpTable[0x44B9] = CAST_OP(0x44B9); \
	JumpTable[0x449F] = CAST_OP(0x449F); \
	JumpTable[0x44A7] = CAST_OP(0x44A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4600 + i] = CAST_OP(0x4600); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4610 + i] = CAST_OP(0x4610); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4618 + i] = CAST_OP(0x4618); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4620 + i] = CAST_OP(0x4620); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4628 + i] = CAST_OP(0x4628); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4630 + i] = CAST_OP(0x4630); \
	JumpTable[0x4638] = CAST_OP(0x4638); \
	JumpTable[0x4639] = CAST_OP(0x4639); \
	JumpTable[0x461F] = CAST_OP(0x461F); \
	JumpTable[0x4627] = CAST_OP(0x4627); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4640 + i] = CAST_OP(0x4640); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4650 + i] = CAST_OP(0x4650); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4658 + i] = CAST_OP(0x4658); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4660 + i] = CAST_OP(0x4660); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4668 + i] = CAST_OP(0x4668); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4670 + i] = CAST_OP(0x4670); \
	JumpTable[0x4678] = CAST_OP(0x4678); \
	JumpTable[0x4679] = CAST_OP(0x4679); \
	JumpTable[0x465F] = CAST_OP(0x465F); \
	JumpTable[0x4667] = CAST_OP(0x4667); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4680 + i] = CAST_OP(0x4680); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4690 + i] = CAST_OP(0x4690); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4698 + i] = CAST_OP(0x4698); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x46A0 + i] = CAST_OP(0x46A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x46A8 + i] = CAST_OP(0x46A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x46B0 + i] = CAST_OP(0x46B0); \
	JumpTable[0x46B8] = CAST_OP(0x46B8); \
	JumpTable[0x46B9] = CAST_OP(0x46B9); \
	JumpTable[0x469F] = CAST_OP(0x469F); \
	JumpTable[0x46A7] = CAST_OP(0x46A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x40C0 + i] = CAST_OP(0x40C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x40D0 + i] = CAST_OP(0x40D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x40D8 + i] = CAST_OP(0x40D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x40E0 + i] = CAST_OP(0x40E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x40E8 + i] = CAST_OP(0x40E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x40F0 + i] = CAST_OP(0x40F0); \
	JumpTable[0x40F8] = CAST_OP(0x40F8); \
	JumpTable[0x40F9] = CAST_OP(0x40F9); \
	JumpTable[0x40DF] = CAST_OP(0x40DF); \
	JumpTable[0x40E7] = CAST_OP(0x40E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x44C0 + i] = CAST_OP(0x44C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x44D0 + i] = CAST_OP(0x44D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x44D8 + i] = CAST_OP(0x44D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x44E0 + i] = CAST_OP(0x44E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x44E8 + i] = CAST_OP(0x44E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x44F0 + i] = CAST_OP(0x44F0); \
	JumpTable[0x44F8] = CAST_OP(0x44F8); \
	JumpTable[0x44F9] = CAST_OP(0x44F9); \
	JumpTable[0x44FA] = CAST_OP(0x44FA); \
	JumpTable[0x44FB] = CAST_OP(0x44FB); \
	JumpTable[0x44FC] = CAST_OP(0x44FC); \
	JumpTable[0x44DF] = CAST_OP(0x44DF); \
	JumpTable[0x44E7] = CAST_OP(0x44E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x46C0 + i] = CAST_OP(0x46C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x46D0 + i] = CAST_OP(0x46D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x46D8 + i] = CAST_OP(0x46D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x46E0 + i] = CAST_OP(0x46E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x46E8 + i] = CAST_OP(0x46E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x46F0 + i] = CAST_OP(0x46F0); \
	JumpTable[0x46F8] = CAST_OP(0x46F8); \
	JumpTable[0x46F9] = CAST_OP(0x46F9); \
	JumpTable[0x46FA] = CAST_OP(0x46FA); \
	JumpTable[0x46FB] = CAST_OP(0x46FB); \
	JumpTable[0x46FC] = CAST_OP(0x46FC); \
	JumpTable[0x46DF] = CAST_OP(0x46DF); \
	JumpTable[0x46E7] = CAST_OP(0x46E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4800 + i] = CAST_OP(0x4800); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4810 + i] = CAST_OP(0x4810); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4818 + i] = CAST_OP(0x4818); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4820 + i] = CAST_OP(0x4820); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4828 + i] = CAST_OP(0x4828); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4830 + i] = CAST_OP(0x4830); \
	JumpTable[0x4838] = CAST_OP(0x4838); \
	JumpTable[0x4839] = CAST_OP(0x4839); \
	JumpTable[0x481F] = CAST_OP(0x481F); \
	JumpTable[0x4827] = CAST_OP(0x4827); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4850 + i] = CAST_OP(0x4850); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4868 + i] = CAST_OP(0x4868); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4870 + i] = CAST_OP(0x4870); \
	JumpTable[0x4878] = CAST_OP(0x4878); \
	JumpTable[0x4879] = CAST_OP(0x4879); \
	JumpTable[0x487A] = CAST_OP(0x487A); \
	JumpTable[0x487B] = CAST_OP(0x487B); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4840 + i] = CAST_OP(0x4840); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4890 + i] = CAST_OP(0x4890); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x48A0 + i] = CAST_OP(0x48A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x48A8 + i] = CAST_OP(0x48A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x48B0 + i] = CAST_OP(0x48B0); \
	JumpTable[0x48B8] = CAST_OP(0x48B8); \
	JumpTable[0x48B9] = CAST_OP(0x48B9); \
	JumpTable[0x48A7] = CAST_OP(0x48A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x48D0 + i] = CAST_OP(0x48D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x48E0 + i] = CAST_OP(0x48E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x48E8 + i] = CAST_OP(0x48E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x48F0 + i] = CAST_OP(0x48F0); \
	JumpTable[0x48F8] = CAST_OP(0x48F8); \
	JumpTable[0x48F9] = CAST_OP(0x48F9); \
	JumpTable[0x48E7] = CAST_OP(0x48E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4880 + i] = CAST_OP(0x4880); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x48C0 + i] = CAST_OP(0x48C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4A00 + i] = CAST_OP(0x4A00); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4A10 + i] = CAST_OP(0x4A10); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4A18 + i] = CAST_OP(0x4A18); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4A20 + i] = CAST_OP(0x4A20); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4A28 + i] = CAST_OP(0x4A28); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4A30 + i] = CAST_OP(0x4A30); \
	JumpTable[0x4A38] = CAST_OP(0x4A38); \
	JumpTable[0x4A39] = CAST_OP(0x4A39); \
	JumpTable[0x4A1F] = CAST_OP(0x4A1F); \
	JumpTable[0x4A27] = CAST_OP(0x4A27); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4A40 + i] = CAST_OP(0x4A40); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4A50 + i] = CAST_OP(0x4A50); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4A58 + i] = CAST_OP(0x4A58); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4A60 + i] = CAST_OP(0x4A60); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4A68 + i] = CAST_OP(0x4A68); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4A70 + i] = CAST_OP(0x4A70); \
	JumpTable[0x4A78] = CAST_OP(0x4A78); \
	JumpTable[0x4A79] = CAST_OP(0x4A79); \
	JumpTable[0x4A5F] = CAST_OP(0x4A5F); \
	JumpTable[0x4A67] = CAST_OP(0x4A67); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4A80 + i] = CAST_OP(0x4A80); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4A90 + i] = CAST_OP(0x4A90); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4A98 + i] = CAST_OP(0x4A98); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4AA0 + i] = CAST_OP(0x4AA0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4AA8 + i] = CAST_OP(0x4AA8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4AB0 + i] = CAST_OP(0x4AB0); \
	JumpTable[0x4AB8] = CAST_OP(0x4AB8); \
	JumpTable[0x4AB9] = CAST_OP(0x4AB9); \
	JumpTable[0x4A9F] = CAST_OP(0x4A9F); \
	JumpTable[0x4AA7] = CAST_OP(0x4AA7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4AC0 + i] = CAST_OP(0x4AC0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4AD0 + i] = CAST_OP(0x4AD0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4AD8 + i] = CAST_OP(0x4AD8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4AE0 + i] = CAST_OP(0x4AE0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4AE8 + i] = CAST_OP(0x4AE8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4AF0 + i] = CAST_OP(0x4AF0); \
	JumpTable[0x4AF8] = CAST_OP(0x4AF8); \
	JumpTable[0x4AF9] = CAST_OP(0x4AF9); \
	JumpTable[0x4ADF] = CAST_OP(0x4ADF); \
	JumpTable[0x4AE7] = CAST_OP(0x4AE7); \
	JumpTable[0x4AFC] = CAST_OP(0x4AFC); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4C90 + i] = CAST_OP(0x4C90); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4C98 + i] = CAST_OP(0x4C98); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4CA8 + i] = CAST_OP(0x4CA8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4CB0 + i] = CAST_OP(0x4CB0); \
	JumpTable[0x4CB8] = CAST_OP(0x4CB8); \
	JumpTable[0x4CB9] = CAST_OP(0x4CB9); \
	JumpTable[0x4CBA] = CAST_OP(0x4CBA); \
	JumpTable[0x4CBB] = CAST_OP(0x4CBB); \
	JumpTable[0x4C9F] = CAST_OP(0x4C9F); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4CD0 + i] = CAST_OP(0x4CD0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x4CD8 + i] = CAST_OP(0x4CD8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4CE8 + i] = CAST_OP(0x4CE8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4CF0 + i] = CAST_OP(0x4CF0); \
	JumpTable[0x4CF8] = CAST_OP(0x4CF8); \
	JumpTable[0x4CF9] = CAST_OP(0x4CF9); \
	JumpTable[0x4CFA] = CAST_OP(0x4CFA); \
	JumpTable[0x4CFB] = CAST_OP(0x4CFB); \
	JumpTable[0x4CDF] = CAST_OP(0x4CDF); \
	for(i = 0x0000; i <= 0x000F; i += 0x0001) \
		JumpTable[0x4E40 + i] = CAST_OP(0x4E40); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4E50 + i] = CAST_OP(0x4E50); \
	JumpTable[0x4E57] = CAST_OP(0x4E57); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4E58 + i] = CAST_OP(0x4E58); \
	JumpTable[0x4E5F] = CAST_OP(0x4E5F); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4E60 + i] = CAST_OP(0x4E60); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4E68 + i] = CAST_OP(0x4E68); \
	JumpTable[0x4E70] = CAST_OP(0x4E70); \
	JumpTable[0x4E71] = CAST_OP(0x4E71); \
	JumpTable[0x4E72] = CAST_OP(0x4E72); \
	JumpTable[0x4E73] = CAST_OP(0x4E73); \
	JumpTable[0x4E75] = CAST_OP(0x4E75); \
	JumpTable[0x4E76] = CAST_OP(0x4E76); \
	JumpTable[0x4E77] = CAST_OP(0x4E77); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4E90 + i] = CAST_OP(0x4E90); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4EA8 + i] = CAST_OP(0x4EA8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4EB0 + i] = CAST_OP(0x4EB0); \
	JumpTable[0x4EB8] = CAST_OP(0x4EB8); \
	JumpTable[0x4EB9] = CAST_OP(0x4EB9); \
	JumpTable[0x4EBA] = CAST_OP(0x4EBA); \
	JumpTable[0x4EBB] = CAST_OP(0x4EBB); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4ED0 + i] = CAST_OP(0x4ED0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4EE8 + i] = CAST_OP(0x4EE8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x4EF0 + i] = CAST_OP(0x4EF0); \
	JumpTable[0x4EF8] = CAST_OP(0x4EF8); \
	JumpTable[0x4EF9] = CAST_OP(0x4EF9); \
	JumpTable[0x4EFA] = CAST_OP(0x4EFA); \
	JumpTable[0x4EFB] = CAST_OP(0x4EFB); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x4180 + i + j] = CAST_OP(0x4180); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x4190 + i + j] = CAST_OP(0x4190); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x4198 + i + j] = CAST_OP(0x4198); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x41A0 + i + j] = CAST_OP(0x41A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x41A8 + i + j] = CAST_OP(0x41A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x41B0 + i + j] = CAST_OP(0x41B0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x41B8 + i] = CAST_OP(0x41B8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x41B9 + i] = CAST_OP(0x41B9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x41BA + i] = CAST_OP(0x41BA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x41BB + i] = CAST_OP(0x41BB); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x41BC + i] = CAST_OP(0x41BC); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x419F + i] = CAST_OP(0x419F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x41A7 + i] = CAST_OP(0x41A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x41D0 + i + j] = CAST_OP(0x41D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x41E8 + i + j] = CAST_OP(0x41E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x41F0 + i + j] = CAST_OP(0x41F0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x41F8 + i] = CAST_OP(0x41F8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x41F9 + i] = CAST_OP(0x41F9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x41FA + i] = CAST_OP(0x41FA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x41FB + i] = CAST_OP(0x41FB); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x50C0 + i] = CAST_OP(0x50C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x51C0 + i] = CAST_OP(0x51C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x52C0 + i] = CAST_OP(0x52C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x53C0 + i] = CAST_OP(0x53C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x54C0 + i] = CAST_OP(0x54C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x55C0 + i] = CAST_OP(0x55C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x56C0 + i] = CAST_OP(0x56C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x57C0 + i] = CAST_OP(0x57C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x58C0 + i] = CAST_OP(0x58C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x59C0 + i] = CAST_OP(0x59C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5AC0 + i] = CAST_OP(0x5AC0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5BC0 + i] = CAST_OP(0x5BC0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5CC0 + i] = CAST_OP(0x5CC0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5DC0 + i] = CAST_OP(0x5DC0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5EC0 + i] = CAST_OP(0x5EC0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5FC0 + i] = CAST_OP(0x5FC0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x50D0 + i] = CAST_OP(0x50D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x51D0 + i] = CAST_OP(0x51D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x52D0 + i] = CAST_OP(0x52D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x53D0 + i] = CAST_OP(0x53D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x54D0 + i] = CAST_OP(0x54D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x55D0 + i] = CAST_OP(0x55D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x56D0 + i] = CAST_OP(0x56D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x57D0 + i] = CAST_OP(0x57D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x58D0 + i] = CAST_OP(0x58D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x59D0 + i] = CAST_OP(0x59D0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5AD0 + i] = CAST_OP(0x5AD0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5BD0 + i] = CAST_OP(0x5BD0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5CD0 + i] = CAST_OP(0x5CD0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5DD0 + i] = CAST_OP(0x5DD0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5ED0 + i] = CAST_OP(0x5ED0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5FD0 + i] = CAST_OP(0x5FD0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x50D8 + i] = CAST_OP(0x50D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x51D8 + i] = CAST_OP(0x51D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x52D8 + i] = CAST_OP(0x52D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x53D8 + i] = CAST_OP(0x53D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x54D8 + i] = CAST_OP(0x54D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x55D8 + i] = CAST_OP(0x55D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x56D8 + i] = CAST_OP(0x56D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x57D8 + i] = CAST_OP(0x57D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x58D8 + i] = CAST_OP(0x58D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x59D8 + i] = CAST_OP(0x59D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x5AD8 + i] = CAST_OP(0x5AD8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x5BD8 + i] = CAST_OP(0x5BD8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x5CD8 + i] = CAST_OP(0x5CD8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x5DD8 + i] = CAST_OP(0x5DD8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x5ED8 + i] = CAST_OP(0x5ED8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x5FD8 + i] = CAST_OP(0x5FD8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x50E0 + i] = CAST_OP(0x50E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x51E0 + i] = CAST_OP(0x51E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x52E0 + i] = CAST_OP(0x52E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x53E0 + i] = CAST_OP(0x53E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x54E0 + i] = CAST_OP(0x54E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x55E0 + i] = CAST_OP(0x55E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x56E0 + i] = CAST_OP(0x56E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x57E0 + i] = CAST_OP(0x57E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x58E0 + i] = CAST_OP(0x58E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x59E0 + i] = CAST_OP(0x59E0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x5AE0 + i] = CAST_OP(0x5AE0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x5BE0 + i] = CAST_OP(0x5BE0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x5CE0 + i] = CAST_OP(0x5CE0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x5DE0 + i] = CAST_OP(0x5DE0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x5EE0 + i] = CAST_OP(0x5EE0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x5FE0 + i] = CAST_OP(0x5FE0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x50E8 + i] = CAST_OP(0x50E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x51E8 + i] = CAST_OP(0x51E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x52E8 + i] = CAST_OP(0x52E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x53E8 + i] = CAST_OP(0x53E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x54E8 + i] = CAST_OP(0x54E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x55E8 + i] = CAST_OP(0x55E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x56E8 + i] = CAST_OP(0x56E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x57E8 + i] = CAST_OP(0x57E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x58E8 + i] = CAST_OP(0x58E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x59E8 + i] = CAST_OP(0x59E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5AE8 + i] = CAST_OP(0x5AE8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5BE8 + i] = CAST_OP(0x5BE8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5CE8 + i] = CAST_OP(0x5CE8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5DE8 + i] = CAST_OP(0x5DE8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5EE8 + i] = CAST_OP(0x5EE8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5FE8 + i] = CAST_OP(0x5FE8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x50F0 + i] = CAST_OP(0x50F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x51F0 + i] = CAST_OP(0x51F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x52F0 + i] = CAST_OP(0x52F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x53F0 + i] = CAST_OP(0x53F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x54F0 + i] = CAST_OP(0x54F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x55F0 + i] = CAST_OP(0x55F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x56F0 + i] = CAST_OP(0x56F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x57F0 + i] = CAST_OP(0x57F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x58F0 + i] = CAST_OP(0x58F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x59F0 + i] = CAST_OP(0x59F0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5AF0 + i] = CAST_OP(0x5AF0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5BF0 + i] = CAST_OP(0x5BF0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5CF0 + i] = CAST_OP(0x5CF0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5DF0 + i] = CAST_OP(0x5DF0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5EF0 + i] = CAST_OP(0x5EF0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5FF0 + i] = CAST_OP(0x5FF0); \
	JumpTable[0x50F8] = CAST_OP(0x50F8); \
	JumpTable[0x51F8] = CAST_OP(0x51F8); \
	JumpTable[0x52F8] = CAST_OP(0x52F8); \
	JumpTable[0x53F8] = CAST_OP(0x53F8); \
	JumpTable[0x54F8] = CAST_OP(0x54F8); \
	JumpTable[0x55F8] = CAST_OP(0x55F8); \
	JumpTable[0x56F8] = CAST_OP(0x56F8); \
	JumpTable[0x57F8] = CAST_OP(0x57F8); \
	JumpTable[0x58F8] = CAST_OP(0x58F8); \
	JumpTable[0x59F8] = CAST_OP(0x59F8); \
	JumpTable[0x5AF8] = CAST_OP(0x5AF8); \
	JumpTable[0x5BF8] = CAST_OP(0x5BF8); \
	JumpTable[0x5CF8] = CAST_OP(0x5CF8); \
	JumpTable[0x5DF8] = CAST_OP(0x5DF8); \
	JumpTable[0x5EF8] = CAST_OP(0x5EF8); \
	JumpTable[0x5FF8] = CAST_OP(0x5FF8); \
	JumpTable[0x50F9] = CAST_OP(0x50F9); \
	JumpTable[0x51F9] = CAST_OP(0x51F9); \
	JumpTable[0x52F9] = CAST_OP(0x52F9); \
	JumpTable[0x53F9] = CAST_OP(0x53F9); \
	JumpTable[0x54F9] = CAST_OP(0x54F9); \
	JumpTable[0x55F9] = CAST_OP(0x55F9); \
	JumpTable[0x56F9] = CAST_OP(0x56F9); \
	JumpTable[0x57F9] = CAST_OP(0x57F9); \
	JumpTable[0x58F9] = CAST_OP(0x58F9); \
	JumpTable[0x59F9] = CAST_OP(0x59F9); \
	JumpTable[0x5AF9] = CAST_OP(0x5AF9); \
	JumpTable[0x5BF9] = CAST_OP(0x5BF9); \
	JumpTable[0x5CF9] = CAST_OP(0x5CF9); \
	JumpTable[0x5DF9] = CAST_OP(0x5DF9); \
	JumpTable[0x5EF9] = CAST_OP(0x5EF9); \
	JumpTable[0x5FF9] = CAST_OP(0x5FF9); \
	JumpTable[0x50DF] = CAST_OP(0x50DF); \
	JumpTable[0x51DF] = CAST_OP(0x51DF); \
	JumpTable[0x52DF] = CAST_OP(0x52DF); \
	JumpTable[0x53DF] = CAST_OP(0x53DF); \
	JumpTable[0x54DF] = CAST_OP(0x54DF); \
	JumpTable[0x55DF] = CAST_OP(0x55DF); \
	JumpTable[0x56DF] = CAST_OP(0x56DF); \
	JumpTable[0x57DF] = CAST_OP(0x57DF); \
	JumpTable[0x58DF] = CAST_OP(0x58DF); \
	JumpTable[0x59DF] = CAST_OP(0x59DF); \
	JumpTable[0x5ADF] = CAST_OP(0x5ADF); \
	JumpTable[0x5BDF] = CAST_OP(0x5BDF); \
	JumpTable[0x5CDF] = CAST_OP(0x5CDF); \
	JumpTable[0x5DDF] = CAST_OP(0x5DDF); \
	JumpTable[0x5EDF] = CAST_OP(0x5EDF); \
	JumpTable[0x5FDF] = CAST_OP(0x5FDF); \
	JumpTable[0x50E7] = CAST_OP(0x50E7); \
	JumpTable[0x51E7] = CAST_OP(0x51E7); \
	JumpTable[0x52E7] = CAST_OP(0x52E7); \
	JumpTable[0x53E7] = CAST_OP(0x53E7); \
	JumpTable[0x54E7] = CAST_OP(0x54E7); \
	JumpTable[0x55E7] = CAST_OP(0x55E7); \
	JumpTable[0x56E7] = CAST_OP(0x56E7); \
	JumpTable[0x57E7] = CAST_OP(0x57E7); \
	JumpTable[0x58E7] = CAST_OP(0x58E7); \
	JumpTable[0x59E7] = CAST_OP(0x59E7); \
	JumpTable[0x5AE7] = CAST_OP(0x5AE7); \
	JumpTable[0x5BE7] = CAST_OP(0x5BE7); \
	JumpTable[0x5CE7] = CAST_OP(0x5CE7); \
	JumpTable[0x5DE7] = CAST_OP(0x5DE7); \
	JumpTable[0x5EE7] = CAST_OP(0x5EE7); \
	JumpTable[0x5FE7] = CAST_OP(0x5FE7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x50C8 + i] = CAST_OP(0x50C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x51C8 + i] = CAST_OP(0x51C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x52C8 + i] = CAST_OP(0x52C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x53C8 + i] = CAST_OP(0x53C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x54C8 + i] = CAST_OP(0x54C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x55C8 + i] = CAST_OP(0x55C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x56C8 + i] = CAST_OP(0x56C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x57C8 + i] = CAST_OP(0x57C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x58C8 + i] = CAST_OP(0x58C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x59C8 + i] = CAST_OP(0x59C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5AC8 + i] = CAST_OP(0x5AC8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5BC8 + i] = CAST_OP(0x5BC8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5CC8 + i] = CAST_OP(0x5CC8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5DC8 + i] = CAST_OP(0x5DC8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5EC8 + i] = CAST_OP(0x5EC8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0x5FC8 + i] = CAST_OP(0x5FC8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5000 + i + j] = CAST_OP(0x5000); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5010 + i + j] = CAST_OP(0x5010); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5018 + i + j] = CAST_OP(0x5018); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5020 + i + j] = CAST_OP(0x5020); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5028 + i + j] = CAST_OP(0x5028); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5030 + i + j] = CAST_OP(0x5030); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x5038 + i] = CAST_OP(0x5038); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x5039 + i] = CAST_OP(0x5039); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x501F + i] = CAST_OP(0x501F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x5027 + i] = CAST_OP(0x5027); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5040 + i + j] = CAST_OP(0x5040); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5048 + i + j] = CAST_OP(0x5048); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5050 + i + j] = CAST_OP(0x5050); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5058 + i + j] = CAST_OP(0x5058); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5060 + i + j] = CAST_OP(0x5060); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5068 + i + j] = CAST_OP(0x5068); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5070 + i + j] = CAST_OP(0x5070); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x5078 + i] = CAST_OP(0x5078); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x5079 + i] = CAST_OP(0x5079); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x505F + i] = CAST_OP(0x505F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x5067 + i] = CAST_OP(0x5067); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5080 + i + j] = CAST_OP(0x5080); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5088 + i + j] = CAST_OP(0x5088); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5090 + i + j] = CAST_OP(0x5090); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5098 + i + j] = CAST_OP(0x5098); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x50A0 + i + j] = CAST_OP(0x50A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x50A8 + i + j] = CAST_OP(0x50A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x50B0 + i + j] = CAST_OP(0x50B0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x50B8 + i] = CAST_OP(0x50B8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x50B9 + i] = CAST_OP(0x50B9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x509F + i] = CAST_OP(0x509F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x50A7 + i] = CAST_OP(0x50A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5100 + i + j] = CAST_OP(0x5100); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5110 + i + j] = CAST_OP(0x5110); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5118 + i + j] = CAST_OP(0x5118); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5120 + i + j] = CAST_OP(0x5120); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5128 + i + j] = CAST_OP(0x5128); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5130 + i + j] = CAST_OP(0x5130); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x5138 + i] = CAST_OP(0x5138); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x5139 + i] = CAST_OP(0x5139); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x511F + i] = CAST_OP(0x511F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x5127 + i] = CAST_OP(0x5127); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5140 + i + j] = CAST_OP(0x5140); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5148 + i + j] = CAST_OP(0x5148); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5150 + i + j] = CAST_OP(0x5150); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5158 + i + j] = CAST_OP(0x5158); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5160 + i + j] = CAST_OP(0x5160); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5168 + i + j] = CAST_OP(0x5168); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5170 + i + j] = CAST_OP(0x5170); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x5178 + i] = CAST_OP(0x5178); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x5179 + i] = CAST_OP(0x5179); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x515F + i] = CAST_OP(0x515F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x5167 + i] = CAST_OP(0x5167); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5180 + i + j] = CAST_OP(0x5180); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5188 + i + j] = CAST_OP(0x5188); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5190 + i + j] = CAST_OP(0x5190); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x5198 + i + j] = CAST_OP(0x5198); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x51A0 + i + j] = CAST_OP(0x51A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x51A8 + i + j] = CAST_OP(0x51A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x51B0 + i + j] = CAST_OP(0x51B0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x51B8 + i] = CAST_OP(0x51B8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x51B9 + i] = CAST_OP(0x51B9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x519F + i] = CAST_OP(0x519F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x51A7 + i] = CAST_OP(0x51A7); \
	for(i = 0x0001; i <= 0x00FF; i += 0x0001) \
		JumpTable[0x6200 + i] = CAST_OP(0x6201); \
	for(i = 0x0001; i <= 0x00FF; i += 0x0001) \
		JumpTable[0x6300 + i] = CAST_OP(0x6301); \
	for(i = 0x0001; i <= 0x00FF; i += 0x0001) \
		JumpTable[0x6400 + i] = CAST_OP(0x6401); \
	for(i = 0x0001; i <= 0x00FF; i += 0x0001) \
		JumpTable[0x6500 + i] = CAST_OP(0x6501); \
	for(i = 0x0001; i <= 0x00FF; i += 0x0001) \
		JumpTable[0x6600 + i] = CAST_OP(0x6601); \
	for(i = 0x0001; i <= 0x00FF; i += 0x0001) \
		JumpTable[0x6700 + i] = CAST_OP(0x6701); \
	for(i = 0x0001; i <= 0x00FF; i += 0x0001) \
		JumpTable[0x6800 + i] = CAST_OP(0x6801); \
	for(i = 0x0001; i <= 0x00FF; i += 0x0001) \
		JumpTable[0x6900 + i] = CAST_OP(0x6901); \
	for(i = 0x0001; i <= 0x00FF; i += 0x0001) \
		JumpTable[0x6A00 + i] = CAST_OP(0x6A01); \
	for(i = 0x0001; i <= 0x00FF; i += 0x0001) \
		JumpTable[0x6B00 + i] = CAST_OP(0x6B01); \
	for(i = 0x0001; i <= 0x00FF; i += 0x0001) \
		JumpTable[0x6C00 + i] = CAST_OP(0x6C01); \
	for(i = 0x0001; i <= 0x00FF; i += 0x0001) \
		JumpTable[0x6D00 + i] = CAST_OP(0x6D01); \
	for(i = 0x0001; i <= 0x00FF; i += 0x0001) \
		JumpTable[0x6E00 + i] = CAST_OP(0x6E01); \
	for(i = 0x0001; i <= 0x00FF; i += 0x0001) \
		JumpTable[0x6F00 + i] = CAST_OP(0x6F01); \
	JumpTable[0x6200] = CAST_OP(0x6200); \
	JumpTable[0x6300] = CAST_OP(0x6300); \
	JumpTable[0x6400] = CAST_OP(0x6400); \
	JumpTable[0x6500] = CAST_OP(0x6500); \
	JumpTable[0x6600] = CAST_OP(0x6600); \
	JumpTable[0x6700] = CAST_OP(0x6700); \
	JumpTable[0x6800] = CAST_OP(0x6800); \
	JumpTable[0x6900] = CAST_OP(0x6900); \
	JumpTable[0x6A00] = CAST_OP(0x6A00); \
	JumpTable[0x6B00] = CAST_OP(0x6B00); \
	JumpTable[0x6C00] = CAST_OP(0x6C00); \
	JumpTable[0x6D00] = CAST_OP(0x6D00); \
	JumpTable[0x6E00] = CAST_OP(0x6E00); \
	JumpTable[0x6F00] = CAST_OP(0x6F00); \
	for(i = 0x0001; i <= 0x00FF; i += 0x0001) \
		JumpTable[0x6000 + i] = CAST_OP(0x6001); \
	JumpTable[0x6000] = CAST_OP(0x6000); \
	for(i = 0x0001; i <= 0x00FF; i += 0x0001) \
		JumpTable[0x6100 + i] = CAST_OP(0x6101); \
	JumpTable[0x6100] = CAST_OP(0x6100); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		for(j = 0x0000; j <= 0x00FF; j += 0x0001) \
				JumpTable[0x7000 + i + j] = CAST_OP(0x7000); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8000 + i + j] = CAST_OP(0x8000); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8010 + i + j] = CAST_OP(0x8010); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8018 + i + j] = CAST_OP(0x8018); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8020 + i + j] = CAST_OP(0x8020); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8028 + i + j] = CAST_OP(0x8028); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8030 + i + j] = CAST_OP(0x8030); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x8038 + i] = CAST_OP(0x8038); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x8039 + i] = CAST_OP(0x8039); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x803A + i] = CAST_OP(0x803A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x803B + i] = CAST_OP(0x803B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x803C + i] = CAST_OP(0x803C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x801F + i] = CAST_OP(0x801F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x8027 + i] = CAST_OP(0x8027); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8040 + i + j] = CAST_OP(0x8040); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8050 + i + j] = CAST_OP(0x8050); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8058 + i + j] = CAST_OP(0x8058); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8060 + i + j] = CAST_OP(0x8060); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8068 + i + j] = CAST_OP(0x8068); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8070 + i + j] = CAST_OP(0x8070); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x8078 + i] = CAST_OP(0x8078); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x8079 + i] = CAST_OP(0x8079); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x807A + i] = CAST_OP(0x807A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x807B + i] = CAST_OP(0x807B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x807C + i] = CAST_OP(0x807C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x805F + i] = CAST_OP(0x805F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x8067 + i] = CAST_OP(0x8067); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8080 + i + j] = CAST_OP(0x8080); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8090 + i + j] = CAST_OP(0x8090); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8098 + i + j] = CAST_OP(0x8098); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x80A0 + i + j] = CAST_OP(0x80A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x80A8 + i + j] = CAST_OP(0x80A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x80B0 + i + j] = CAST_OP(0x80B0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x80B8 + i] = CAST_OP(0x80B8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x80B9 + i] = CAST_OP(0x80B9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x80BA + i] = CAST_OP(0x80BA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x80BB + i] = CAST_OP(0x80BB); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x80BC + i] = CAST_OP(0x80BC); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x809F + i] = CAST_OP(0x809F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x80A7 + i] = CAST_OP(0x80A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8110 + i + j] = CAST_OP(0x8110); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8118 + i + j] = CAST_OP(0x8118); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8120 + i + j] = CAST_OP(0x8120); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8128 + i + j] = CAST_OP(0x8128); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8130 + i + j] = CAST_OP(0x8130); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x8138 + i] = CAST_OP(0x8138); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x8139 + i] = CAST_OP(0x8139); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x811F + i] = CAST_OP(0x811F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x8127 + i] = CAST_OP(0x8127); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8150 + i + j] = CAST_OP(0x8150); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8158 + i + j] = CAST_OP(0x8158); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8160 + i + j] = CAST_OP(0x8160); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8168 + i + j] = CAST_OP(0x8168); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8170 + i + j] = CAST_OP(0x8170); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x8178 + i] = CAST_OP(0x8178); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x8179 + i] = CAST_OP(0x8179); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x815F + i] = CAST_OP(0x815F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x8167 + i] = CAST_OP(0x8167); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8190 + i + j] = CAST_OP(0x8190); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8198 + i + j] = CAST_OP(0x8198); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x81A0 + i + j] = CAST_OP(0x81A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x81A8 + i + j] = CAST_OP(0x81A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x81B0 + i + j] = CAST_OP(0x81B0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x81B8 + i] = CAST_OP(0x81B8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x81B9 + i] = CAST_OP(0x81B9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x819F + i] = CAST_OP(0x819F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x81A7 + i] = CAST_OP(0x81A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x8100 + i + j] = CAST_OP(0x8100); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x8108 + i + j] = CAST_OP(0x8108); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x810F + i] = CAST_OP(0x810F); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x8F08 + i] = CAST_OP(0x8F08); \
	JumpTable[0x8F0F] = CAST_OP(0x8F0F); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x80C0 + i + j] = CAST_OP(0x80C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x80D0 + i + j] = CAST_OP(0x80D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x80D8 + i + j] = CAST_OP(0x80D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x80E0 + i + j] = CAST_OP(0x80E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x80E8 + i + j] = CAST_OP(0x80E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x80F0 + i + j] = CAST_OP(0x80F0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x80F8 + i] = CAST_OP(0x80F8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x80F9 + i] = CAST_OP(0x80F9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x80FA + i] = CAST_OP(0x80FA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x80FB + i] = CAST_OP(0x80FB); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x80FC + i] = CAST_OP(0x80FC); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x80DF + i] = CAST_OP(0x80DF); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x80E7 + i] = CAST_OP(0x80E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x81C0 + i + j] = CAST_OP(0x81C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x81D0 + i + j] = CAST_OP(0x81D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x81D8 + i + j] = CAST_OP(0x81D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x81E0 + i + j] = CAST_OP(0x81E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x81E8 + i + j] = CAST_OP(0x81E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x81F0 + i + j] = CAST_OP(0x81F0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x81F8 + i] = CAST_OP(0x81F8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x81F9 + i] = CAST_OP(0x81F9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x81FA + i] = CAST_OP(0x81FA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x81FB + i] = CAST_OP(0x81FB); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x81FC + i] = CAST_OP(0x81FC); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x81DF + i] = CAST_OP(0x81DF); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x81E7 + i] = CAST_OP(0x81E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9000 + i + j] = CAST_OP(0x9000); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9008 + i + j] = CAST_OP(0x9008); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9010 + i + j] = CAST_OP(0x9010); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9018 + i + j] = CAST_OP(0x9018); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9020 + i + j] = CAST_OP(0x9020); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9028 + i + j] = CAST_OP(0x9028); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9030 + i + j] = CAST_OP(0x9030); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x9038 + i] = CAST_OP(0x9038); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x9039 + i] = CAST_OP(0x9039); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x903A + i] = CAST_OP(0x903A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x903B + i] = CAST_OP(0x903B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x903C + i] = CAST_OP(0x903C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x901F + i] = CAST_OP(0x901F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x9027 + i] = CAST_OP(0x9027); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9040 + i + j] = CAST_OP(0x9040); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9048 + i + j] = CAST_OP(0x9048); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9050 + i + j] = CAST_OP(0x9050); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9058 + i + j] = CAST_OP(0x9058); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9060 + i + j] = CAST_OP(0x9060); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9068 + i + j] = CAST_OP(0x9068); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9070 + i + j] = CAST_OP(0x9070); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x9078 + i] = CAST_OP(0x9078); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x9079 + i] = CAST_OP(0x9079); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x907A + i] = CAST_OP(0x907A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x907B + i] = CAST_OP(0x907B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x907C + i] = CAST_OP(0x907C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x905F + i] = CAST_OP(0x905F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x9067 + i] = CAST_OP(0x9067); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9080 + i + j] = CAST_OP(0x9080); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9088 + i + j] = CAST_OP(0x9088); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9090 + i + j] = CAST_OP(0x9090); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9098 + i + j] = CAST_OP(0x9098); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x90A0 + i + j] = CAST_OP(0x90A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x90A8 + i + j] = CAST_OP(0x90A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x90B0 + i + j] = CAST_OP(0x90B0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x90B8 + i] = CAST_OP(0x90B8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x90B9 + i] = CAST_OP(0x90B9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x90BA + i] = CAST_OP(0x90BA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x90BB + i] = CAST_OP(0x90BB); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x90BC + i] = CAST_OP(0x90BC); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x909F + i] = CAST_OP(0x909F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x90A7 + i] = CAST_OP(0x90A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9110 + i + j] = CAST_OP(0x9110); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9118 + i + j] = CAST_OP(0x9118); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9120 + i + j] = CAST_OP(0x9120); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9128 + i + j] = CAST_OP(0x9128); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9130 + i + j] = CAST_OP(0x9130); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x9138 + i] = CAST_OP(0x9138); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x9139 + i] = CAST_OP(0x9139); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x911F + i] = CAST_OP(0x911F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x9127 + i] = CAST_OP(0x9127); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9150 + i + j] = CAST_OP(0x9150); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9158 + i + j] = CAST_OP(0x9158); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9160 + i + j] = CAST_OP(0x9160); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9168 + i + j] = CAST_OP(0x9168); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9170 + i + j] = CAST_OP(0x9170); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x9178 + i] = CAST_OP(0x9178); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x9179 + i] = CAST_OP(0x9179); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x915F + i] = CAST_OP(0x915F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x9167 + i] = CAST_OP(0x9167); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9190 + i + j] = CAST_OP(0x9190); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9198 + i + j] = CAST_OP(0x9198); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x91A0 + i + j] = CAST_OP(0x91A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x91A8 + i + j] = CAST_OP(0x91A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x91B0 + i + j] = CAST_OP(0x91B0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x91B8 + i] = CAST_OP(0x91B8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x91B9 + i] = CAST_OP(0x91B9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x919F + i] = CAST_OP(0x919F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x91A7 + i] = CAST_OP(0x91A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9100 + i + j] = CAST_OP(0x9100); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9140 + i + j] = CAST_OP(0x9140); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x9180 + i + j] = CAST_OP(0x9180); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x9108 + i + j] = CAST_OP(0x9108); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x9148 + i + j] = CAST_OP(0x9148); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0x9188 + i + j] = CAST_OP(0x9188); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x910F + i] = CAST_OP(0x910F); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x914F + i] = CAST_OP(0x914F); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0x918F + i] = CAST_OP(0x918F); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x9F08 + i] = CAST_OP(0x9F08); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x9F48 + i] = CAST_OP(0x9F48); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0x9F88 + i] = CAST_OP(0x9F88); \
	JumpTable[0x9F0F] = CAST_OP(0x9F0F); \
	JumpTable[0x9F4F] = CAST_OP(0x9F4F); \
	JumpTable[0x9F8F] = CAST_OP(0x9F8F); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x90C0 + i + j] = CAST_OP(0x90C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x90C8 + i + j] = CAST_OP(0x90C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x90D0 + i + j] = CAST_OP(0x90D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x90D8 + i + j] = CAST_OP(0x90D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x90E0 + i + j] = CAST_OP(0x90E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x90E8 + i + j] = CAST_OP(0x90E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x90F0 + i + j] = CAST_OP(0x90F0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x90F8 + i] = CAST_OP(0x90F8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x90F9 + i] = CAST_OP(0x90F9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x90FA + i] = CAST_OP(0x90FA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x90FB + i] = CAST_OP(0x90FB); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x90FC + i] = CAST_OP(0x90FC); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x90DF + i] = CAST_OP(0x90DF); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x90E7 + i] = CAST_OP(0x90E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x91C0 + i + j] = CAST_OP(0x91C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x91C8 + i + j] = CAST_OP(0x91C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x91D0 + i + j] = CAST_OP(0x91D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x91D8 + i + j] = CAST_OP(0x91D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x91E0 + i + j] = CAST_OP(0x91E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x91E8 + i + j] = CAST_OP(0x91E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0x91F0 + i + j] = CAST_OP(0x91F0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x91F8 + i] = CAST_OP(0x91F8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x91F9 + i] = CAST_OP(0x91F9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x91FA + i] = CAST_OP(0x91FA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x91FB + i] = CAST_OP(0x91FB); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x91FC + i] = CAST_OP(0x91FC); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x91DF + i] = CAST_OP(0x91DF); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0x91E7 + i] = CAST_OP(0x91E7); \
	for(i = 0x0000; i <= 0x0FFF; i += 0x0001) \
		JumpTable[0xA000 + i] = CAST_OP(0xA000); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB000 + i + j] = CAST_OP(0xB000); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB008 + i + j] = CAST_OP(0xB008); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB010 + i + j] = CAST_OP(0xB010); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB018 + i + j] = CAST_OP(0xB018); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB020 + i + j] = CAST_OP(0xB020); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB028 + i + j] = CAST_OP(0xB028); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB030 + i + j] = CAST_OP(0xB030); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB038 + i] = CAST_OP(0xB038); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB039 + i] = CAST_OP(0xB039); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB03A + i] = CAST_OP(0xB03A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB03B + i] = CAST_OP(0xB03B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB03C + i] = CAST_OP(0xB03C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB01F + i] = CAST_OP(0xB01F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB027 + i] = CAST_OP(0xB027); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB040 + i + j] = CAST_OP(0xB040); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB048 + i + j] = CAST_OP(0xB048); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB050 + i + j] = CAST_OP(0xB050); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB058 + i + j] = CAST_OP(0xB058); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB060 + i + j] = CAST_OP(0xB060); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB068 + i + j] = CAST_OP(0xB068); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB070 + i + j] = CAST_OP(0xB070); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB078 + i] = CAST_OP(0xB078); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB079 + i] = CAST_OP(0xB079); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB07A + i] = CAST_OP(0xB07A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB07B + i] = CAST_OP(0xB07B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB07C + i] = CAST_OP(0xB07C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB05F + i] = CAST_OP(0xB05F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB067 + i] = CAST_OP(0xB067); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB080 + i + j] = CAST_OP(0xB080); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB088 + i + j] = CAST_OP(0xB088); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB090 + i + j] = CAST_OP(0xB090); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB098 + i + j] = CAST_OP(0xB098); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB0A0 + i + j] = CAST_OP(0xB0A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB0A8 + i + j] = CAST_OP(0xB0A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB0B0 + i + j] = CAST_OP(0xB0B0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB0B8 + i] = CAST_OP(0xB0B8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB0B9 + i] = CAST_OP(0xB0B9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB0BA + i] = CAST_OP(0xB0BA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB0BB + i] = CAST_OP(0xB0BB); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB0BC + i] = CAST_OP(0xB0BC); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB09F + i] = CAST_OP(0xB09F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB0A7 + i] = CAST_OP(0xB0A7); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0xB108 + i + j] = CAST_OP(0xB108); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0xB148 + i + j] = CAST_OP(0xB148); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0xB188 + i + j] = CAST_OP(0xB188); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0xB10F + i] = CAST_OP(0xB10F); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0xB14F + i] = CAST_OP(0xB14F); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0xB18F + i] = CAST_OP(0xB18F); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xBF08 + i] = CAST_OP(0xBF08); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xBF48 + i] = CAST_OP(0xBF48); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xBF88 + i] = CAST_OP(0xBF88); \
	JumpTable[0xBF0F] = CAST_OP(0xBF0F); \
	JumpTable[0xBF4F] = CAST_OP(0xBF4F); \
	JumpTable[0xBF8F] = CAST_OP(0xBF8F); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB100 + i + j] = CAST_OP(0xB100); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB110 + i + j] = CAST_OP(0xB110); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB118 + i + j] = CAST_OP(0xB118); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB120 + i + j] = CAST_OP(0xB120); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB128 + i + j] = CAST_OP(0xB128); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB130 + i + j] = CAST_OP(0xB130); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB138 + i] = CAST_OP(0xB138); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB139 + i] = CAST_OP(0xB139); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB11F + i] = CAST_OP(0xB11F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB127 + i] = CAST_OP(0xB127); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB140 + i + j] = CAST_OP(0xB140); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB150 + i + j] = CAST_OP(0xB150); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB158 + i + j] = CAST_OP(0xB158); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB160 + i + j] = CAST_OP(0xB160); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB168 + i + j] = CAST_OP(0xB168); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB170 + i + j] = CAST_OP(0xB170); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB178 + i] = CAST_OP(0xB178); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB179 + i] = CAST_OP(0xB179); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB15F + i] = CAST_OP(0xB15F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB167 + i] = CAST_OP(0xB167); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB180 + i + j] = CAST_OP(0xB180); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB190 + i + j] = CAST_OP(0xB190); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB198 + i + j] = CAST_OP(0xB198); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB1A0 + i + j] = CAST_OP(0xB1A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB1A8 + i + j] = CAST_OP(0xB1A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB1B0 + i + j] = CAST_OP(0xB1B0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB1B8 + i] = CAST_OP(0xB1B8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB1B9 + i] = CAST_OP(0xB1B9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB19F + i] = CAST_OP(0xB19F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB1A7 + i] = CAST_OP(0xB1A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB0C0 + i + j] = CAST_OP(0xB0C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB0C8 + i + j] = CAST_OP(0xB0C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB0D0 + i + j] = CAST_OP(0xB0D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB0D8 + i + j] = CAST_OP(0xB0D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB0E0 + i + j] = CAST_OP(0xB0E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB0E8 + i + j] = CAST_OP(0xB0E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB0F0 + i + j] = CAST_OP(0xB0F0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB0F8 + i] = CAST_OP(0xB0F8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB0F9 + i] = CAST_OP(0xB0F9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB0FA + i] = CAST_OP(0xB0FA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB0FB + i] = CAST_OP(0xB0FB); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB0FC + i] = CAST_OP(0xB0FC); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB0DF + i] = CAST_OP(0xB0DF); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB0E7 + i] = CAST_OP(0xB0E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB1C0 + i + j] = CAST_OP(0xB1C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB1C8 + i + j] = CAST_OP(0xB1C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB1D0 + i + j] = CAST_OP(0xB1D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB1D8 + i + j] = CAST_OP(0xB1D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB1E0 + i + j] = CAST_OP(0xB1E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB1E8 + i + j] = CAST_OP(0xB1E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xB1F0 + i + j] = CAST_OP(0xB1F0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB1F8 + i] = CAST_OP(0xB1F8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB1F9 + i] = CAST_OP(0xB1F9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB1FA + i] = CAST_OP(0xB1FA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB1FB + i] = CAST_OP(0xB1FB); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB1FC + i] = CAST_OP(0xB1FC); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB1DF + i] = CAST_OP(0xB1DF); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xB1E7 + i] = CAST_OP(0xB1E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC000 + i + j] = CAST_OP(0xC000); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC010 + i + j] = CAST_OP(0xC010); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC018 + i + j] = CAST_OP(0xC018); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC020 + i + j] = CAST_OP(0xC020); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC028 + i + j] = CAST_OP(0xC028); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC030 + i + j] = CAST_OP(0xC030); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC038 + i] = CAST_OP(0xC038); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC039 + i] = CAST_OP(0xC039); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC03A + i] = CAST_OP(0xC03A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC03B + i] = CAST_OP(0xC03B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC03C + i] = CAST_OP(0xC03C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC01F + i] = CAST_OP(0xC01F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC027 + i] = CAST_OP(0xC027); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC040 + i + j] = CAST_OP(0xC040); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC050 + i + j] = CAST_OP(0xC050); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC058 + i + j] = CAST_OP(0xC058); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC060 + i + j] = CAST_OP(0xC060); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC068 + i + j] = CAST_OP(0xC068); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC070 + i + j] = CAST_OP(0xC070); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC078 + i] = CAST_OP(0xC078); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC079 + i] = CAST_OP(0xC079); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC07A + i] = CAST_OP(0xC07A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC07B + i] = CAST_OP(0xC07B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC07C + i] = CAST_OP(0xC07C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC05F + i] = CAST_OP(0xC05F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC067 + i] = CAST_OP(0xC067); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC080 + i + j] = CAST_OP(0xC080); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC090 + i + j] = CAST_OP(0xC090); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC098 + i + j] = CAST_OP(0xC098); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC0A0 + i + j] = CAST_OP(0xC0A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC0A8 + i + j] = CAST_OP(0xC0A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC0B0 + i + j] = CAST_OP(0xC0B0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC0B8 + i] = CAST_OP(0xC0B8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC0B9 + i] = CAST_OP(0xC0B9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC0BA + i] = CAST_OP(0xC0BA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC0BB + i] = CAST_OP(0xC0BB); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC0BC + i] = CAST_OP(0xC0BC); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC09F + i] = CAST_OP(0xC09F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC0A7 + i] = CAST_OP(0xC0A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC110 + i + j] = CAST_OP(0xC110); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC118 + i + j] = CAST_OP(0xC118); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC120 + i + j] = CAST_OP(0xC120); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC128 + i + j] = CAST_OP(0xC128); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC130 + i + j] = CAST_OP(0xC130); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC138 + i] = CAST_OP(0xC138); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC139 + i] = CAST_OP(0xC139); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC11F + i] = CAST_OP(0xC11F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC127 + i] = CAST_OP(0xC127); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC150 + i + j] = CAST_OP(0xC150); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC158 + i + j] = CAST_OP(0xC158); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC160 + i + j] = CAST_OP(0xC160); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC168 + i + j] = CAST_OP(0xC168); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC170 + i + j] = CAST_OP(0xC170); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC178 + i] = CAST_OP(0xC178); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC179 + i] = CAST_OP(0xC179); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC15F + i] = CAST_OP(0xC15F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC167 + i] = CAST_OP(0xC167); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC190 + i + j] = CAST_OP(0xC190); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC198 + i + j] = CAST_OP(0xC198); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC1A0 + i + j] = CAST_OP(0xC1A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC1A8 + i + j] = CAST_OP(0xC1A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC1B0 + i + j] = CAST_OP(0xC1B0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC1B8 + i] = CAST_OP(0xC1B8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC1B9 + i] = CAST_OP(0xC1B9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC19F + i] = CAST_OP(0xC19F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC1A7 + i] = CAST_OP(0xC1A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC100 + i + j] = CAST_OP(0xC100); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0xC108 + i + j] = CAST_OP(0xC108); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0xC10F + i] = CAST_OP(0xC10F); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xCF08 + i] = CAST_OP(0xCF08); \
	JumpTable[0xCF0F] = CAST_OP(0xCF0F); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC0C0 + i + j] = CAST_OP(0xC0C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC0D0 + i + j] = CAST_OP(0xC0D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC0D8 + i + j] = CAST_OP(0xC0D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC0E0 + i + j] = CAST_OP(0xC0E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC0E8 + i + j] = CAST_OP(0xC0E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC0F0 + i + j] = CAST_OP(0xC0F0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC0F8 + i] = CAST_OP(0xC0F8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC0F9 + i] = CAST_OP(0xC0F9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC0FA + i] = CAST_OP(0xC0FA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC0FB + i] = CAST_OP(0xC0FB); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC0FC + i] = CAST_OP(0xC0FC); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC0DF + i] = CAST_OP(0xC0DF); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC0E7 + i] = CAST_OP(0xC0E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC1C0 + i + j] = CAST_OP(0xC1C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC1D0 + i + j] = CAST_OP(0xC1D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC1D8 + i + j] = CAST_OP(0xC1D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC1E0 + i + j] = CAST_OP(0xC1E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC1E8 + i + j] = CAST_OP(0xC1E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC1F0 + i + j] = CAST_OP(0xC1F0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC1F8 + i] = CAST_OP(0xC1F8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC1F9 + i] = CAST_OP(0xC1F9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC1FA + i] = CAST_OP(0xC1FA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC1FB + i] = CAST_OP(0xC1FB); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC1FC + i] = CAST_OP(0xC1FC); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC1DF + i] = CAST_OP(0xC1DF); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xC1E7 + i] = CAST_OP(0xC1E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC140 + i + j] = CAST_OP(0xC140); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC148 + i + j] = CAST_OP(0xC148); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xC188 + i + j] = CAST_OP(0xC188); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD000 + i + j] = CAST_OP(0xD000); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD008 + i + j] = CAST_OP(0xD008); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD010 + i + j] = CAST_OP(0xD010); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD018 + i + j] = CAST_OP(0xD018); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD020 + i + j] = CAST_OP(0xD020); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD028 + i + j] = CAST_OP(0xD028); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD030 + i + j] = CAST_OP(0xD030); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD038 + i] = CAST_OP(0xD038); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD039 + i] = CAST_OP(0xD039); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD03A + i] = CAST_OP(0xD03A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD03B + i] = CAST_OP(0xD03B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD03C + i] = CAST_OP(0xD03C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD01F + i] = CAST_OP(0xD01F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD027 + i] = CAST_OP(0xD027); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD040 + i + j] = CAST_OP(0xD040); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD048 + i + j] = CAST_OP(0xD048); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD050 + i + j] = CAST_OP(0xD050); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD058 + i + j] = CAST_OP(0xD058); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD060 + i + j] = CAST_OP(0xD060); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD068 + i + j] = CAST_OP(0xD068); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD070 + i + j] = CAST_OP(0xD070); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD078 + i] = CAST_OP(0xD078); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD079 + i] = CAST_OP(0xD079); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD07A + i] = CAST_OP(0xD07A); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD07B + i] = CAST_OP(0xD07B); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD07C + i] = CAST_OP(0xD07C); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD05F + i] = CAST_OP(0xD05F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD067 + i] = CAST_OP(0xD067); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD080 + i + j] = CAST_OP(0xD080); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD088 + i + j] = CAST_OP(0xD088); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD090 + i + j] = CAST_OP(0xD090); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD098 + i + j] = CAST_OP(0xD098); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD0A0 + i + j] = CAST_OP(0xD0A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD0A8 + i + j] = CAST_OP(0xD0A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD0B0 + i + j] = CAST_OP(0xD0B0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD0B8 + i] = CAST_OP(0xD0B8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD0B9 + i] = CAST_OP(0xD0B9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD0BA + i] = CAST_OP(0xD0BA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD0BB + i] = CAST_OP(0xD0BB); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD0BC + i] = CAST_OP(0xD0BC); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD09F + i] = CAST_OP(0xD09F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD0A7 + i] = CAST_OP(0xD0A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD110 + i + j] = CAST_OP(0xD110); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD118 + i + j] = CAST_OP(0xD118); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD120 + i + j] = CAST_OP(0xD120); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD128 + i + j] = CAST_OP(0xD128); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD130 + i + j] = CAST_OP(0xD130); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD138 + i] = CAST_OP(0xD138); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD139 + i] = CAST_OP(0xD139); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD11F + i] = CAST_OP(0xD11F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD127 + i] = CAST_OP(0xD127); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD150 + i + j] = CAST_OP(0xD150); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD158 + i + j] = CAST_OP(0xD158); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD160 + i + j] = CAST_OP(0xD160); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD168 + i + j] = CAST_OP(0xD168); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD170 + i + j] = CAST_OP(0xD170); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD178 + i] = CAST_OP(0xD178); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD179 + i] = CAST_OP(0xD179); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD15F + i] = CAST_OP(0xD15F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD167 + i] = CAST_OP(0xD167); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD190 + i + j] = CAST_OP(0xD190); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD198 + i + j] = CAST_OP(0xD198); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD1A0 + i + j] = CAST_OP(0xD1A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD1A8 + i + j] = CAST_OP(0xD1A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD1B0 + i + j] = CAST_OP(0xD1B0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD1B8 + i] = CAST_OP(0xD1B8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD1B9 + i] = CAST_OP(0xD1B9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD19F + i] = CAST_OP(0xD19F); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD1A7 + i] = CAST_OP(0xD1A7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD100 + i + j] = CAST_OP(0xD100); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD140 + i + j] = CAST_OP(0xD140); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD180 + i + j] = CAST_OP(0xD180); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0xD108 + i + j] = CAST_OP(0xD108); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0xD148 + i + j] = CAST_OP(0xD148); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0C00; j += 0x0200) \
				JumpTable[0xD188 + i + j] = CAST_OP(0xD188); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0xD10F + i] = CAST_OP(0xD10F); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0xD14F + i] = CAST_OP(0xD14F); \
	for(i = 0x0000; i <= 0x0C00; i += 0x0200) \
		JumpTable[0xD18F + i] = CAST_OP(0xD18F); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xDF08 + i] = CAST_OP(0xDF08); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xDF48 + i] = CAST_OP(0xDF48); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xDF88 + i] = CAST_OP(0xDF88); \
	JumpTable[0xDF0F] = CAST_OP(0xDF0F); \
	JumpTable[0xDF4F] = CAST_OP(0xDF4F); \
	JumpTable[0xDF8F] = CAST_OP(0xDF8F); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD0C0 + i + j] = CAST_OP(0xD0C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD0C8 + i + j] = CAST_OP(0xD0C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD0D0 + i + j] = CAST_OP(0xD0D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD0D8 + i + j] = CAST_OP(0xD0D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD0E0 + i + j] = CAST_OP(0xD0E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD0E8 + i + j] = CAST_OP(0xD0E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD0F0 + i + j] = CAST_OP(0xD0F0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD0F8 + i] = CAST_OP(0xD0F8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD0F9 + i] = CAST_OP(0xD0F9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD0FA + i] = CAST_OP(0xD0FA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD0FB + i] = CAST_OP(0xD0FB); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD0FC + i] = CAST_OP(0xD0FC); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD0DF + i] = CAST_OP(0xD0DF); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD0E7 + i] = CAST_OP(0xD0E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD1C0 + i + j] = CAST_OP(0xD1C0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD1C8 + i + j] = CAST_OP(0xD1C8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD1D0 + i + j] = CAST_OP(0xD1D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD1D8 + i + j] = CAST_OP(0xD1D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD1E0 + i + j] = CAST_OP(0xD1E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD1E8 + i + j] = CAST_OP(0xD1E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xD1F0 + i + j] = CAST_OP(0xD1F0); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD1F8 + i] = CAST_OP(0xD1F8); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD1F9 + i] = CAST_OP(0xD1F9); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD1FA + i] = CAST_OP(0xD1FA); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD1FB + i] = CAST_OP(0xD1FB); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD1FC + i] = CAST_OP(0xD1FC); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD1DF + i] = CAST_OP(0xD1DF); \
	for(i = 0x0000; i <= 0x0E00; i += 0x0200) \
		JumpTable[0xD1E7 + i] = CAST_OP(0xD1E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE000 + i + j] = CAST_OP(0xE000); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE040 + i + j] = CAST_OP(0xE040); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE080 + i + j] = CAST_OP(0xE080); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE008 + i + j] = CAST_OP(0xE008); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE048 + i + j] = CAST_OP(0xE048); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE088 + i + j] = CAST_OP(0xE088); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE010 + i + j] = CAST_OP(0xE010); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE050 + i + j] = CAST_OP(0xE050); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE090 + i + j] = CAST_OP(0xE090); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE018 + i + j] = CAST_OP(0xE018); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE058 + i + j] = CAST_OP(0xE058); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE098 + i + j] = CAST_OP(0xE098); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE100 + i + j] = CAST_OP(0xE100); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE140 + i + j] = CAST_OP(0xE140); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE180 + i + j] = CAST_OP(0xE180); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE108 + i + j] = CAST_OP(0xE108); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE148 + i + j] = CAST_OP(0xE148); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE188 + i + j] = CAST_OP(0xE188); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE110 + i + j] = CAST_OP(0xE110); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE150 + i + j] = CAST_OP(0xE150); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE190 + i + j] = CAST_OP(0xE190); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE118 + i + j] = CAST_OP(0xE118); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE158 + i + j] = CAST_OP(0xE158); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE198 + i + j] = CAST_OP(0xE198); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE020 + i + j] = CAST_OP(0xE020); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE060 + i + j] = CAST_OP(0xE060); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE0A0 + i + j] = CAST_OP(0xE0A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE028 + i + j] = CAST_OP(0xE028); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE068 + i + j] = CAST_OP(0xE068); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE0A8 + i + j] = CAST_OP(0xE0A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE030 + i + j] = CAST_OP(0xE030); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE070 + i + j] = CAST_OP(0xE070); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE0B0 + i + j] = CAST_OP(0xE0B0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE038 + i + j] = CAST_OP(0xE038); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE078 + i + j] = CAST_OP(0xE078); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE0B8 + i + j] = CAST_OP(0xE0B8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE120 + i + j] = CAST_OP(0xE120); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE160 + i + j] = CAST_OP(0xE160); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE1A0 + i + j] = CAST_OP(0xE1A0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE128 + i + j] = CAST_OP(0xE128); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE168 + i + j] = CAST_OP(0xE168); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE1A8 + i + j] = CAST_OP(0xE1A8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE130 + i + j] = CAST_OP(0xE130); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE170 + i + j] = CAST_OP(0xE170); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE1B0 + i + j] = CAST_OP(0xE1B0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE138 + i + j] = CAST_OP(0xE138); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE178 + i + j] = CAST_OP(0xE178); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		for(j = 0x0000; j <= 0x0E00; j += 0x0200) \
				JumpTable[0xE1B8 + i + j] = CAST_OP(0xE1B8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE0D0 + i] = CAST_OP(0xE0D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xE0D8 + i] = CAST_OP(0xE0D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xE0E0 + i] =CAST_OP(0xE0E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE0E8 + i] = CAST_OP(0xE0E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE0F0 + i] = CAST_OP(0xE0F0); \
	JumpTable[0xE0F8] = CAST_OP(0xE0F8); \
	JumpTable[0xE0F9] = CAST_OP(0xE0F9); \
	JumpTable[0xE0DF] = CAST_OP(0xE0DF); \
	JumpTable[0xE0E7] = CAST_OP(0xE0E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE2D0 + i] = CAST_OP(0xE2D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xE2D8 + i] = CAST_OP(0xE2D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xE2E0 + i] = CAST_OP(0xE2E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE2E8 + i] = CAST_OP(0xE2E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE2F0 + i] = CAST_OP(0xE2F0); \
	JumpTable[0xE2F8] = CAST_OP(0xE2F8); \
	JumpTable[0xE2F9] = CAST_OP(0xE2F9); \
	JumpTable[0xE2DF] = CAST_OP(0xE2DF); \
	JumpTable[0xE2E7] = CAST_OP(0xE2E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE4D0 + i] = CAST_OP(0xE4D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xE4D8 + i] = CAST_OP(0xE4D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xE4E0 + i] = CAST_OP(0xE4E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE4E8 + i] = CAST_OP(0xE4E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE4F0 + i] = CAST_OP(0xE4F0); \
	JumpTable[0xE4F8] = CAST_OP(0xE4F8); \
	JumpTable[0xE4F9] = CAST_OP(0xE4F9); \
	JumpTable[0xE4DF] = CAST_OP(0xE4DF); \
	JumpTable[0xE4E7] = CAST_OP(0xE4E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE6D0 + i] = CAST_OP(0xE6D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xE6D8 + i] = CAST_OP(0xE6D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xE6E0 + i] = CAST_OP(0xE6E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE6E8 + i] = CAST_OP(0xE6E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE6F0 + i] = CAST_OP(0xE6F0); \
	JumpTable[0xE6F8] = CAST_OP(0xE6F8); \
	JumpTable[0xE6F9] = CAST_OP(0xE6F9); \
	JumpTable[0xE6DF] = CAST_OP(0xE6DF); \
	JumpTable[0xE6E7] = CAST_OP(0xE6E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE1D0 + i] = CAST_OP(0xE1D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xE1D8 + i] = CAST_OP(0xE1D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xE1E0 + i] = CAST_OP(0xE1E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE1E8 + i] = CAST_OP(0xE1E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE1F0 + i] = CAST_OP(0xE1F0); \
	JumpTable[0xE1F8] = CAST_OP(0xE1F8); \
	JumpTable[0xE1F9] = CAST_OP(0xE1F9); \
	JumpTable[0xE1DF] = CAST_OP(0xE1DF); \
	JumpTable[0xE1E7] = CAST_OP(0xE1E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE3D0 + i] = CAST_OP(0xE3D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xE3D8 + i] = CAST_OP(0xE3D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xE3E0 + i] = CAST_OP(0xE3E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE3E8 + i] = CAST_OP(0xE3E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE3F0 + i] = CAST_OP(0xE3F0); \
	JumpTable[0xE3F8] = CAST_OP(0xE3F8); \
	JumpTable[0xE3F9] = CAST_OP(0xE3F9); \
	JumpTable[0xE3DF] = CAST_OP(0xE3DF); \
	JumpTable[0xE3E7] = CAST_OP(0xE3E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE5D0 + i] = CAST_OP(0xE5D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xE5D8 + i] = CAST_OP(0xE5D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xE5E0 + i] = CAST_OP(0xE5E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE5E8 + i] = CAST_OP(0xE5E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE5F0 + i] = CAST_OP(0xE5F0); \
	JumpTable[0xE5F8] = CAST_OP(0xE5F8); \
	JumpTable[0xE5F9] = CAST_OP(0xE5F9); \
	JumpTable[0xE5DF] = CAST_OP(0xE5DF); \
	JumpTable[0xE5E7] = CAST_OP(0xE5E7); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE7D0 + i] = CAST_OP(0xE7D0); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xE7D8 + i] = CAST_OP(0xE7D8); \
	for(i = 0x0000; i <= 0x0006; i += 0x0001) \
		JumpTable[0xE7E0 + i] = CAST_OP(0xE7E0); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE7E8 + i] = CAST_OP(0xE7E8); \
	for(i = 0x0000; i <= 0x0007; i += 0x0001) \
		JumpTable[0xE7F0 + i] = CAST_OP(0xE7F0); \
	JumpTable[0xE7F8] = CAST_OP(0xE7F8); \
	JumpTable[0xE7F9] = CAST_OP(0xE7F9); \
	JumpTable[0xE7DF] = CAST_OP(0xE7DF); \
	JumpTable[0xE7E7] = CAST_OP(0xE7E7); \
	for(i = 0x0000; i <= 0x0FFF; i += 0x0001) \
		JumpTable[0xF000 + i] = CAST_OP(0xF000); \
}

typedef unsigned char  u8;
typedef signed char    s8;
typedef unsigned short u16;
typedef signed short   s16;
typedef unsigned int   u32;
typedef signed int     s32;

#ifdef FAME_EMULATE_TRACE
static u32 flag_T;
#endif
static u32 flag_C;
static u32 flag_V;
static u32 flag_NotZ;
static u32 flag_N;
static u32 flag_X;         /* 16 bytes aligned */
static u32 flag_S;
static u32 flag_I;

typedef union
{
#ifndef FAME_BIG_ENDIAN
    struct
    {
        u8 B,B1,B2,B3;
    } b;
    struct
    {
        s8 SB,SB1,SB2,SB3;
    } sb;
    struct
    {
        u16 W,W1;
    } w;
    struct
    {
        s16 SW,SW1;
    } sw;
#else
    struct
    {
        u8 B3,B2,B1,B;
    } b;
    struct
    {
        s8 SB3,SB2,SB1,SB;
    } sb;
    struct
    {
        u16 W1,W;
    } w;
    struct
    {
        s16 SW1,SW;
    } sw;
#endif
    u32 D;
    s32 SD;
} famec_union32;

/* M68K registers */
typedef enum
{
    M68K_REG_D0=0,
    M68K_REG_D1,
    M68K_REG_D2,
    M68K_REG_D3,
    M68K_REG_D4,
    M68K_REG_D5,
    M68K_REG_D6,
    M68K_REG_D7,
    M68K_REG_A0,
    M68K_REG_A1,
    M68K_REG_A2,
    M68K_REG_A3,
    M68K_REG_A4,
    M68K_REG_A5,
    M68K_REG_A6,
    M68K_REG_A7,
    M68K_REG_ASP,
    M68K_REG_PC,
    M68K_REG_SR
} m68k_register;


/* The memory blocks must be in native (Motorola) format */
typedef struct
{
    u32 low_addr;
    u32 high_addr;
    u32 offset;
} M68K_PROGRAM;

/* The memory blocks must be in native (Motorola) format */
typedef struct
{
    u32 low_addr;
    u32 high_addr;
    void    *mem_handler;
    void    *data;
} M68K_DATA;

/* M68K CPU CONTEXT */
typedef struct
{
    M68K_PROGRAM *fetch;
    M68K_DATA *read_byte;
    M68K_DATA *read_word;
    M68K_DATA *write_byte;
    M68K_DATA *write_word;
    M68K_PROGRAM *sv_fetch;
    M68K_DATA *sv_read_byte;
    M68K_DATA *sv_read_word;
    M68K_DATA *sv_write_byte;
    M68K_DATA *sv_write_word;
    M68K_PROGRAM *user_fetch;
    M68K_DATA *user_read_byte;
    M68K_DATA *user_read_word;
    M68K_DATA *user_write_byte;
    M68K_DATA *user_write_word;
    void           (*reset_handler)(void);
    void           (*iack_handler)(u32 level);
    u32 *icust_handler;
    famec_union32   dreg[8];
    famec_union32   areg[8];
    u32 asp;
    u32  pc;
    u32 cycles_counter;
    u8  interrupts[8];
    u16 sr;
    u16 execinfo;
} M68K_CONTEXT;


/* Custom function handler */
typedef void (*icust_handler_func)(u32 vector);

/*
 global variable
*/

/* Main CPU context */
#ifdef FAME_GLOBAL_CONTEXT
M68K_CONTEXT FAME_CONTEXT;
s32 io_cycle_counter;
#else
static M68K_CONTEXT FAME_CONTEXT;
static s32 io_cycle_counter;
#endif

static s32 cycles_needed=0;
static s32 cycles_to_do=0;
#if FAME_PC_BITS == 32
static u32 PC;
static u16* BasePC;
#else
static u16 *PC;
static u32 BasePC;
#endif
static u32 Fetch[M68K_FETCHBANK];

/* Lookup IRQ level to attend */
/* Indexed by interrupts[0] */
static u8 irq_level_lookup[256] =
{
    0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};

typedef u8 (*mem8_handler_func)(s32 address);
typedef u16 (*mem16_handler_func)(s32 address);
typedef u32 (*mem32_handler_func)(s32 address);
typedef void (*memw_handler_func)(s32 address, s32 data);

#ifdef FAME_SECURE_ALL_BANKS
static unsigned char dummy_fetch[(1<<M68K_FETCHSFT)];
#endif

#ifdef FAME_DIRECT_MAPPING
typedef struct
{
    void    *mem_handler;
    void    *data;
} M68K_INTERNAL_DATA;

#ifdef FAME_SECURE_ALL_BANKS
static void dummy_write(unsigned adr, unsigned btr){}
static int dummy_read(unsigned adr)
{
    return -1;
}
#endif

static M68K_INTERNAL_DATA DataRB[M68K_DATABANK];
static M68K_INTERNAL_DATA DataRW[M68K_DATABANK];
static M68K_INTERNAL_DATA DataWB[M68K_DATABANK];
static M68K_INTERNAL_DATA DataWW[M68K_DATABANK];

#else

#define DataRB FAME_CONTEXT.read_byte
#define DataRW FAME_CONTEXT.read_word
#define DataWB FAME_CONTEXT.write_byte
#define DataWW FAME_CONTEXT.write_word

#endif

/* Custom function handler */
typedef void (*opcode_func)(void);



static u32 initialised = 0;

/* exception cycle table (taken from musashi core) */
static const s32 exception_cycle_table[256] =
{
    4, /*  0: Reset - Initial Stack Pointer */
    4, /*  1: Reset - Initial Program Counter */
    50, /*  2: Bus Error */
    50, /*  3: Address Error */
    34, /*  4: Illegal Instruction */
    38, /*  5: Divide by Zero */
    40, /*  6: CHK */
    34, /*  7: TRAPV */
    34, /*  8: Privilege Violation */
    34, /*  9: Trace */
    4, /* 10: */
    4, /* 11: */
    4, /* 12: RESERVED */
    4, /* 13: Coprocessor Protocol Violation */
    4, /* 14: Format Error */
    44, /* 15: Uninitialized Interrupt */
    4, /* 16: RESERVED */
    4, /* 17: RESERVED */
    4, /* 18: RESERVED */
    4, /* 19: RESERVED */
    4, /* 20: RESERVED */
    4, /* 21: RESERVED */
    4, /* 22: RESERVED */
    4, /* 23: RESERVED */
    INT_TIMING, /* 24: Spurious Interrupt */
    INT_TIMING, /* 25: Level 1 Interrupt Autovector */
    INT_TIMING, /* 26: Level 2 Interrupt Autovector */
    INT_TIMING, /* 27: Level 3 Interrupt Autovector */
    INT_TIMING, /* 28: Level 4 Interrupt Autovector */
    INT_TIMING, /* 29: Level 5 Interrupt Autovector */
    INT_TIMING, /* 30: Level 6 Interrupt Autovector */
    INT_TIMING, /* 31: Level 7 Interrupt Autovector */
    34, /* 32: TRAP #0 */
    34, /* 33: TRAP #1 */
    34, /* 34: TRAP #2 */
    34, /* 35: TRAP #3 */
    34, /* 36: TRAP #4 */
    34, /* 37: TRAP #5 */
    34, /* 38: TRAP #6 */
    34, /* 39: TRAP #7 */
    34, /* 40: TRAP #8 */
    34, /* 41: TRAP #9 */
    34, /* 42: TRAP #10 */
    34, /* 43: TRAP #11 */
    34, /* 44: TRAP #12 */
    34, /* 45: TRAP #13 */
    34, /* 46: TRAP #14 */
    34, /* 47: TRAP #15 */
    4, /* 48: FP Branch or Set on Unknown Condition */
    4, /* 49: FP Inexact Result */
    4, /* 50: FP Divide by Zero */
    4, /* 51: FP Underflow */
    4, /* 52: FP Operand Error */
    4, /* 53: FP Overflow */
    4, /* 54: FP Signaling NAN */
    4, /* 55: FP Unimplemented Data Type */
    4, /* 56: MMU Configuration Error */
    4, /* 57: MMU Illegal Operation Error */
    4, /* 58: MMU Access Level Violation Error */
    4, /* 59: RESERVED */
    4, /* 60: RESERVED */
    4, /* 61: RESERVED */
    4, /* 62: RESERVED */
    4, /* 63: RESERVED */
    /* 64-255: User Defined */
    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4
};


/*
 helper functions
*/

static void famec_SetDummyFetch(void)
{
#ifdef FAME_SECURE_ALL_BANKS
    u32 i,j;

    i = (0 >> M68K_FETCHSFT) & M68K_FETCHMASK;
    j = (0xFFFFFFFF >> M68K_FETCHSFT) & M68K_FETCHMASK;
    while (i <= j)
    {
        Fetch[i] = ((u32)&dummy_fetch)-(i*(1<<M68K_FETCHSFT));
        i++;
    }
#endif
}

static void famec_SetFetch(u32 low_adr, u32 high_adr, u32 fetch_adr)
{
    u32 i, j;

    i = (low_adr >> M68K_FETCHSFT) & M68K_FETCHMASK;
    j = (high_adr >> M68K_FETCHSFT) & M68K_FETCHMASK;

    while (i <= j)
        Fetch[i++] = fetch_adr;
}

#ifdef FAME_DIRECT_MAPPING

static void famec_SetDummyData(void)
{
#ifdef FAME_SECURE_ALL_BANKS
    u32 i, j;

    i = (0 >> M68K_DATASFT) & M68K_DATAMASK;
    j = (0xFFFFFFFF >> M68K_DATASFT) & M68K_DATAMASK;

    while (i <= j)
    {
        DataRB[i].mem_handler = DataRW[i].mem_handler = (void *)&dummy_read;
        DataWB[i].mem_handler = DataWW[i].mem_handler = (void *)&dummy_write;
        DataRB[i].data = DataRW[i].data = DataWB[i].data = DataWW[i].data = NULL;
        i++;
    }
#endif
}

static void famec_SetDataRB(u32 low_adr, u32 high_adr, void *mh, void *dt)
{
    u32 i, j;

    i = (low_adr >> M68K_DATASFT) & M68K_DATAMASK;
    j = (high_adr >> M68K_DATASFT) & M68K_DATAMASK;

    while (i <= j)
    {
        DataRB[i].mem_handler = mh;
        DataRB[i++].data = dt;
    }
}

static void famec_SetDataRW(u32 low_adr, u32 high_adr, void *mh, void *dt)
{
    u32 i, j;

    i = (low_adr >> M68K_DATASFT) & M68K_DATAMASK;
    j = (high_adr >> M68K_DATASFT) & M68K_DATAMASK;

    while (i <= j)
    {
        DataRW[i].mem_handler = mh;
        DataRW[i++].data = dt;
    }
}

static void famec_SetDataWB(u32 low_adr, u32 high_adr, void *mh, void *dt)
{
    u32 i, j;

    i = (low_adr >> M68K_DATASFT) & M68K_DATAMASK;
    j = (high_adr >> M68K_DATASFT) & M68K_DATAMASK;

    while (i <= j)
    {
        DataWB[i].mem_handler = mh;
        DataWB[i++].data = dt;
    }
}

static void famec_SetDataWW(u32 low_adr, u32 high_adr, void *mh, void *dt)
{
    u32 i, j;

    i = (low_adr >> M68K_DATASFT) & M68K_DATAMASK;
    j = (high_adr >> M68K_DATASFT) & M68K_DATAMASK;

    while (i <= j)
    {
        DataWW[i].mem_handler = mh;
        DataWW[i++].data = dt;
    }
}
#endif

static void famec_SetBanks(void)
{
    famec_SetDummyFetch();

	SETUP_FETCH_BANK(famec_SetFetch, FAME_CONTEXT.fetch)

#ifdef FAME_DIRECT_MAPPING
    famec_SetDummyData();

	SETUP_DATA_BANK(famec_SetDataRB, FAME_CONTEXT.read_byte)
	SETUP_DATA_BANK(famec_SetDataRW, FAME_CONTEXT.read_word)
	SETUP_DATA_BANK(famec_SetDataWB, FAME_CONTEXT.write_byte)
	SETUP_DATA_BANK(famec_SetDataWW, FAME_CONTEXT.write_word)
#endif
}

#ifdef FAME_ACCURATE_TIMING
/*
 Functions used to compute accurate opcode timing (MUL/DIV)
*/

static EXTRA_INLINE u8 bitset_count(u32 data)
{
    unsigned int const MASK1  = 0x55555555;
    unsigned int const MASK2  = 0x33333333;
    unsigned int const MASK4  = 0x0f0f0f0f;
    unsigned int const MASK6 = 0x0000003f;

    unsigned int const w = (data & MASK1) + ((data >> 1) & MASK1);
    unsigned int const x = (w & MASK2) + ((w >> 2) & MASK2);
    unsigned int const y = ((x + (x >> 4)) & MASK4);
    unsigned int const z = (y + (y >> 8));
    unsigned int const c = (z + (z >> 16)) & MASK6;

    return c;
}

/*
 DIVU
 Unsigned division
*/
static u32 getDivu68kCycles(u32 dividend, u16 divisor)
{
    u32 mcycles;
    u32 hdivisor;
    int i;

    if ( (u16) divisor == 0)
        return 0;

    /* Overflow */
    if ( (dividend >> 16) >= divisor)
        return (mcycles = 5) * 2;

    mcycles = 38;
    hdivisor = ((u32) divisor) << 16;

    for ( i = 0; i < 15; i++)
    {
        u32 temp;
        temp = dividend;

        dividend <<= 1;

        /* If carry from shift */
        if ( (int) temp < 0)
        {
            dividend -= hdivisor;
        }

        else
        {
            mcycles += 2;
            if ( dividend >= hdivisor)
            {
                dividend -= hdivisor;
                mcycles--;
            }
        }
    }

    return mcycles * 2;
}

/*
 DIVS
 Signed division
*/
static u32 getDivs68kCycles(s32 dividend, s16 divisor)
{
    u32 mcycles;
    u32 aquot;
    int i;

    if ( (s16) divisor == 0)
        return 0;

    mcycles = 6;

    if ( dividend < 0)
        mcycles++;

    /*  Check for absolute overflow */
    if ( ((u32) abs( dividend) >> 16) >= (u16) abs( divisor))
    {
        return (mcycles + 2) * 2;
    }

    /* Absolute quotient */
    aquot = (u32) abs( dividend) / (u16) abs( divisor);

    mcycles += 55;

    if ( divisor >= 0)
    {
        if ( dividend >= 0)
            mcycles--;
        else
            mcycles++;
    }

    /* Count 15 msbits in absolute of quotient */

    for ( i = 0; i < 15; i++)
    {
        if ( (s16) aquot >= 0)
            mcycles++;
        aquot <<= 1;
    }

    return mcycles * 2;
}
#endif

/*
 Read / Write functions
*/

static EXTRA_INLINE u32 Read_Byte(u32 addr)
{
    u32 i=0;
    s32 val;

    addr&=M68K_ADDR_MASK;
#ifdef FAME_DEBUG
    printf("Reading byte from addr = 0x%08X\n",addr);
#endif

#ifndef FAME_DIRECT_MAPPING
    while ( ((FAME_CONTEXT.read_byte[i].low_addr > addr) || (FAME_CONTEXT.read_byte[i].high_addr < addr)) && (FAME_CONTEXT.read_byte[i].low_addr != BANKEND_TAG) )
        i++;

    if (FAME_CONTEXT.read_byte[i].low_addr == BANKEND_TAG)
        return (u32)-1;
#else
    i=addr>>M68K_DATASFT;
#endif

     if (DataRB[i].mem_handler)
        val= (*((mem8_handler_func *)&DataRB[i].mem_handler))(addr);
    else
#ifndef FAME_BIG_ENDIAN
        val = *((u8 *)(((u32)DataRB[i].data) + (addr^1)));
#else
        val = *((u8 *)(((u32)DataRB[i].data) + (addr)));
#endif

#ifdef FAME_DEBUG
    printf("Reading 0x%08X = 0x%04X...\n",addr,val);
#endif

    return val;
}

static EXTRA_INLINE u32 Read_Word(u32 addr)
{
    u32 i=0;
    s32 val;

    addr&=M68K_ADDR_MASK;
#ifdef FAME_DEBUG
    printf("Reading from addr = 0x%08X\n",addr);
#endif

#ifndef FAME_DIRECT_MAPPING
    while ( ((FAME_CONTEXT.read_word[i].low_addr > addr) || (FAME_CONTEXT.read_word[i].high_addr < addr)) && (FAME_CONTEXT.read_word[i].low_addr != BANKEND_TAG) )
        i++;

    if (FAME_CONTEXT.read_word[i].low_addr == BANKEND_TAG)
        return (u32)-1;
#else
    i=addr>>M68K_DATASFT;
#endif

    if (DataRW[i].mem_handler)
        val= (*((mem16_handler_func *)&DataRW[i].mem_handler))(addr);
    else
        val = *((u16 *)(((u32)DataRW[i].data) + addr));

#ifdef FAME_DEBUG
    printf("Reading 0x%08X = 0x%04X...\n",addr,val);
#endif

    return val;
}

static EXTRA_INLINE void Write_Byte(u32 addr, u32 data)
{
    u32 i=0;

    addr&=M68K_ADDR_MASK;
#ifdef FAME_DEBUG
    printf("Writing byte 0x%08X = 0x%04X...\n",addr,data);
#endif

#ifndef FAME_DIRECT_MAPPING
    while ( ((FAME_CONTEXT.write_byte[i].low_addr > addr) || (FAME_CONTEXT.write_byte[i].high_addr < addr)) && (FAME_CONTEXT.write_byte[i].low_addr != BANKEND_TAG) )
        i++;
    if (FAME_CONTEXT.write_byte[i].low_addr == BANKEND_TAG)
        return;
#else
    i=addr>>M68K_DATASFT;
#endif

    if (DataWB[i].mem_handler != NULL)
        (*((memw_handler_func *)&DataWB[i].mem_handler))(addr,data);
    else
#ifndef FAME_BIG_ENDIAN
        *((u8 *)(((u32)DataWB[i].data)+ (addr^1))) = data;
#else
        *((u8 *)(((u32)DataWB[i].data)+ (addr))) = data;
#endif
}


static EXTRA_INLINE void Write_Word(u32 addr, u32 data)
{
    u32 i=0;

    addr&=M68K_ADDR_MASK;
#ifdef FAME_DEBUG
    printf("Writing 0x%08X = 0x%04X...\n",addr,data);
#endif

#ifndef FAME_DIRECT_MAPPING
    while ( ((FAME_CONTEXT.write_word[i].low_addr > addr) || (FAME_CONTEXT.write_word[i].high_addr < addr)) && (FAME_CONTEXT.write_word[i].low_addr != BANKEND_TAG) )
        i++;
    if (FAME_CONTEXT.write_word[i].low_addr == BANKEND_TAG)
        return;
#else
    i=addr>>M68K_DATASFT;
#endif

    if (DataWW[i].mem_handler != NULL)
        (*((memw_handler_func *)&DataWW[i].mem_handler))(addr,data);
    else
        *((u16 *)(((u32)DataWW[i].data) + addr)) = data;
}

static u32 Opcode;

/*
 Chequea las interrupciones y las inicia
*/
static EXTRA_INLINE s32 interrupt_chk(void)
{
    /* Bit 0 MUST be zero at all times */
    assert((FAME_CONTEXT.interrupts[0] & 1) == 0);

    if (FAME_CONTEXT.interrupts[0])
    {
        /* There is a pending IRQ */
        const u8 irql = irq_level_lookup[FAME_CONTEXT.interrupts[0]];

        if (irql == 7)		/* NMI */
            return irql;
        else if (irql > flag_I)
            return irql;
    }

#ifdef FAME_EMULATE_TRACE
    if (flag_T)
    {
        /*
        		FAME_CONTEXT.execinfo |= M68K_EMULATE_TRACE;
        		cycles_needed= io_cycle_counter;
        		io_cycle_counter=0;
        */
        return -1;
    }
#endif
    return 0;
}


static EXTRA_INLINE void execute_exception(s32 vect)
{
	/* comprobar si hay tabla funciones manejadoras */
    if (FAME_CONTEXT.icust_handler && FAME_CONTEXT.icust_handler[vect])
    {
        FAME_CONTEXT.sr = GET_SR;
        FAME_CONTEXT.pc = UNBASED_PC;
        (*(icust_handler_func*)&FAME_CONTEXT.icust_handler[vect])(vect);
    }
    else
    {
        u32 newPC;
        u32 oldPC;
        u32 oldSR = GET_SR;

        READ_LONG_F(vect * 4, newPC)

        /* swap A7 and USP */
        if (!flag_S)
        {
            u32 tmpSP;

            tmpSP = ASP;
            ASP = AREG(7);
            AREG(7) = tmpSP;
        }

        oldPC = UNBASED_PC;
        PUSH_32_F(oldPC)
        PUSH_16_F(oldSR)

        /* adjust SR */
        flag_S = M68K_SR_S;

        SET_PC(newPC)
    }

	io_cycle_counter -= exception_cycle_table[vect];
}

static EXTRA_INLINE void interrupt_attend(s32 line)
{
	/* al atender la IRQ, la CPU sale del estado parado */
	FAME_CONTEXT.execinfo &= ~M68K_HALTED;

	/* Desactivar interrupcion */
	FAME_CONTEXT.interrupts[0] &= ~(1 << ((u32)line));

	execute_exception(FAME_CONTEXT.interrupts[(u32)line]);

	/* comprobar si hay rutina de acknowledge */
	if (FAME_CONTEXT.iack_handler != NULL)
		FAME_CONTEXT.iack_handler(line);

	flag_I = (u32)line;
}

/* Group 0 exceptions are not handled actually */
static EXTRA_INLINE void execute_exception_group_0(s32 vect, u16 inst_reg, s32 addr, u16 spec_info)
{
    execute_exception(vect);
    if (!(FAME_CONTEXT.icust_handler && FAME_CONTEXT.icust_handler[vect]))
    {
        PUSH_16_F(inst_reg);
        PUSH_32_F(addr);
        PUSH_16_F(spec_info);
    }
}

/* Performs the required actions to finish the emulate call */
static EXTRA_INLINE void finish_emulate(const s32 cycles_to_add)
{
    FAME_CONTEXT.sr = GET_SR;
    FAME_CONTEXT.pc = UNBASED_PC;

    FAME_CONTEXT.execinfo &= ~M68K_RUNNING;

    /* Actualizar contador de ciclos */
    FAME_CONTEXT.cycles_counter += cycles_to_add;

#ifdef FAME_DEBUG
    printf("<-PC=0x%08X (%p-%p)\n",UNBASED_PC,PC,BasePC);
#endif
}

#ifndef FAME_GOTOS

#define OPCODE(N_OP) static void OP_##N_OP(void)
#define CAST_OP(N_OP) (opcode_func)&OP_##N_OP
#include "famec_opcodes.h"
#else
#define OPCODE(N_OP) OP_##N_OP:
#define CAST_OP(N_OP) (opcode_func)&&OP_##N_OP
#endif

//static opcode_func JumpTable[0x10000];
#include "jumptable.h"

/***********************/
/* core main functions */
/***********************/

/***************************************************************************/
/* m68k_init()                                                             */
/* Debe ser llamado para inicializar la tabla de saltos de instruccion     */
/* No recibe parametros y no devuelve nada                                 */
/***************************************************************************/
void FAME_API(init)(void)
{
#ifdef FAME_DEBUG
    puts("Initializing FAME...");
#endif

#ifndef FAME_GOTOS
    //BUILD_OPCODE_TABLE
    initialised = 1;
#endif

#ifdef FAME_DEBUG
    puts("FAME initialized.");
#endif
}

/******************************************************************************/
/* m68k_reset()																  */
/* Parametros: Ninguno														  */
/* Retorno: Exito de la operacion                                             */
/*     M68K_OK (0):  La funcion se ha ejecutado satisfactoriamente            */
/*     M68K_RUNNING (1): No se puede resetear porque la CPU esta en ejecucion */
/*     M68K_NO_SUP_ADDR_SPACE (2):  No se puede resetear porque no hay mapa   */
/*             de memoria supervisor de extraccion de opcodes                 */
/******************************************************************************/
u32 FAME_API(reset)(void)
{
#ifndef FAME_GOTOS
    assert(initialised);
#endif

    /* Si la CPU esta en ejecucion, salir con M68K_RUNNING */
    if (FAME_CONTEXT.execinfo & M68K_RUNNING)
        return M68K_RUNNING;

    /* Si no hay mapa de memoria supervisor, salir con M68K_NO_SUP_ADDR_SPACE */
    if (!FAME_CONTEXT.sv_fetch)
        return M68K_NO_SUP_ADDR_SPACE;

    FAME_CONTEXT.fetch = FAME_CONTEXT.sv_fetch;
    FAME_CONTEXT.read_byte = FAME_CONTEXT.sv_read_byte;
    FAME_CONTEXT.read_word = FAME_CONTEXT.sv_read_word;
    FAME_CONTEXT.write_byte = FAME_CONTEXT.sv_write_byte;
    FAME_CONTEXT.write_word = FAME_CONTEXT.sv_write_word;

    /* Resetear registros */
    memset(&FAME_CONTEXT.dreg[0], 0, 16*4);

    /* Resetear interrupts, execinfo y ASP */
    memset(&FAME_CONTEXT.interrupts[0], 0, 8);
    FAME_CONTEXT.execinfo = 0;
    ASP = 0;

    /* Fijar registro de estado */
    FAME_CONTEXT.sr = 0x2700;

    /* Obtener puntero de pila inicial y PC */
	READ_LONG_F(0, AREG(7));
	READ_LONG_F(4, FAME_CONTEXT.pc);

#ifdef FAME_DEBUG
    puts("Reset 68k done!\n");
    printf("PC = 0x%08X\n",FAME_CONTEXT.pc);
#endif

    return M68K_OK;
}


/******************************************************************************/
/* m68k_raise_irq(level,vector)			                                      */
/* Parametros: level = nivel de interrupcion                                  */
/*             vector = puntero a la direccion de la rutina de atencion       */
/*                      -1 auto, -2 interrupcion espuria                      */
/* Retorno: Exito de la operacion                                             */
/*         0  La interrupcion se ha habilitado satisfactoriamente             */
/*        -1  No se ha podido habilitar porque ya existe otra interrupcion    */
/*            en ese nivel.                                                   */
/*        -2  No se ha podido habilitar porque el vector no es valido o       */
/*	                 el nivel es igual a 0.                                   */
/******************************************************************************/
s32 FAME_API(raise_irq)(s32 level, s32 vector)
{
    /* Enmascarar nivel de interrupcion */
    level &=7;

    /* Nivel de interrupcion = 0 no es valido */
    if (!level) return M68K_IRQ_INV_PARAMS;

    /* Comprobar si existe una interrupcion activada en ese nivel */
    if (FAME_CONTEXT.interrupts[0] & (1 << level))
        return M68K_IRQ_LEVEL_ERROR;

    /* El vector de interrupcion no puede ser > 255 ni menor que -2 */
    if ((vector > 255) || (vector < M68K_SPURIOUS_IRQ))
    {
        return M68K_IRQ_INV_PARAMS;
    }

    /* Dar de alta la interrupcion en interrupts */
    FAME_CONTEXT.interrupts[0] |= (1 << level);

    switch (vector)
    {
    case M68K_SPURIOUS_IRQ:
        FAME_CONTEXT.interrupts[level] = 0x18;
        break;
    case M68K_AUTOVECTORED_IRQ:
        FAME_CONTEXT.interrupts[level] = level + 0x18;
        break;
    default:
        FAME_CONTEXT.interrupts[level] = vector;
        break;
    }
#ifdef FAME_DEBUG
    printf("RAISE interrupts[%i]=0x%X\n",level,FAME_CONTEXT.interrupts[level]);
#endif

    /* Testear si la CPU esta detenida (mediante STOP) */
    if (FAME_CONTEXT.execinfo & M68K_HALTED)
    {
        /* Si la IRQ es NMI o si supera la mascara de interrupcion, */
        /* salir de estado parado                                   */
        if ((level == 7) || (level > ((FAME_CONTEXT.sr >> 8) & 0x7)))
        {
            FAME_CONTEXT.execinfo &= ~M68K_HALTED;
        }
    }

    return M68K_OK;
}


/******************************************************************************/
/* m68k_lower_irq(level)                                                      */
/* Parametros: Nivel de la interrupcion a retirar                             */
/* Retorno: Exito de la operacion                                             */
/*         0  La interrupcion se ha retirado satisfactoriamente               */
/*        -1  No se ha podido retirar porque esa interrupcion                 */
/*			  no esta habilitada.                                             */
/*        -2  No se ha podido retirar porque el nivel es 0 o mayor            */
/*				o igual que 7 (no se puede retirar la NMI)                    */
/******************************************************************************/
s32 FAME_API(lower_irq)(s32 level)
{
    /* Enmascarar nivel de interrupcion */
    level &=7;

    /* Nivel de interrupcion = 0 no es valido */
    if (!level) return M68K_IRQ_INV_PARAMS;

    /* La interrupcion de nivel 7 (NMI) no se puede bajar */
    if (level > 6)
    {
        return M68K_IRQ_INV_PARAMS;
    }

    /* Comprobar que la interrupcion este activada */
    if (FAME_CONTEXT.interrupts[0] & (1 << level))
    {
        /* Dar de baja la interrupcion */
        FAME_CONTEXT.interrupts[0] &= ~(1 << level);
    }
    else
    {
        return M68K_IRQ_LEVEL_ERROR;
    }

    return M68K_OK;
}

/******************************************************************************/
/* m68k_get_context_size                                                      */
/* No recibe parametros                                                       */
/* Retorno: Tamano del contexto en bytes                                      */
/******************************************************************************/
s32 FAME_API(get_context_size)(void)
{
    return sizeof(M68K_CONTEXT);
}

/***************************************************************************/
/* m68k_get_context(address)                                               */
/* Parametro: Direccion del contexto                                       */
/* No retorna ningun valor                                                 */
/***************************************************************************/
void FAME_API(get_context)(void *context)
{
    memcpy(context,&FAME_CONTEXT,sizeof(M68K_CONTEXT));
}

/***************************************************************************/
/* m68k_set_context(address)                                               */
/* Parametro: Direccion del contexto                                       */
/* No retorna ningun valor                                                 */
/***************************************************************************/
void FAME_API(set_context)(void *context)
{
    memcpy(&FAME_CONTEXT,context,sizeof(M68K_CONTEXT));
    famec_SetBanks();
}

/****************************************************************************/
/* m68k_get_pc()                                                            */
/* No recibe parametros                                                     */
/* Retorna 68k PC                                                           */
/****************************************************************************/
u32 FAME_API(get_pc)(void)
{
    return (FAME_CONTEXT.execinfo & M68K_RUNNING) ? UNBASED_PC : FAME_CONTEXT.pc;
}

/***************************************************************************/
/*  m68k_get_register(register)                                            */
/*  Parametro: Registro a obtener valor (indice)                           */
/*  Retorno: Valor del registro requerido                                  */
/*  Observacion: En caso de que el indice no sea correcto                  */
/*               la funcion devolvera -1                                   */
/***************************************************************************/
s32 FAME_API(get_register)(m68k_register reg)
{
    switch (reg)
    {
    case M68K_REG_D0:
    case M68K_REG_D1:
    case M68K_REG_D2:
    case M68K_REG_D3:
    case M68K_REG_D4:
    case M68K_REG_D5:
    case M68K_REG_D6:
    case M68K_REG_D7:
        return DREG(reg - M68K_REG_D0);

    case M68K_REG_A0:
    case M68K_REG_A1:
    case M68K_REG_A2:
    case M68K_REG_A3:
    case M68K_REG_A4:
    case M68K_REG_A5:
    case M68K_REG_A6:
    case M68K_REG_A7:
        return AREG(reg - M68K_REG_A0);

    case M68K_REG_ASP:
        return ASP;

    case M68K_REG_PC:
        return FAME_API(get_pc)();

    case M68K_REG_SR:
		return (FAME_CONTEXT.execinfo & M68K_RUNNING) ? GET_SR : FAME_CONTEXT.sr;

    default:
        return M68K_INV_REG;
    }
}

/***********************************************************************/
/*  m68k_set_register(register,value)                                  */
/*  Parametros: Registro (indice) y valor a asignar                    */
/*  Retorno: Exito de la operacion                                     */
/*           0  La operacion se ha realizado satisfactoriamente        */
/*           1  El indice del registro no es valido (fuera de limites) */
/***********************************************************************/
s32 FAME_API(set_register)(m68k_register reg, u32 value)
{
    switch (reg)
    {
    case M68K_REG_D0:
    case M68K_REG_D1:
    case M68K_REG_D2:
    case M68K_REG_D3:
    case M68K_REG_D4:
    case M68K_REG_D5:
    case M68K_REG_D6:
    case M68K_REG_D7:
        DREG(reg - M68K_REG_D0) = value;
        break;

    case M68K_REG_A0:
    case M68K_REG_A1:
    case M68K_REG_A2:
    case M68K_REG_A3:
    case M68K_REG_A4:
    case M68K_REG_A5:
    case M68K_REG_A6:
    case M68K_REG_A7:
        AREG(reg - M68K_REG_A0) = value;
        break;

    case M68K_REG_ASP:
        ASP = value;
        break;

    case M68K_REG_PC:
        if (FAME_CONTEXT.execinfo & M68K_RUNNING)
        {
            SET_PC(value);
        }
        else
        {
            FAME_CONTEXT.pc = value;
        }
        break;

    case M68K_REG_SR:
        if (FAME_CONTEXT.execinfo & M68K_RUNNING)
        {
			SET_SR(value);
		}
		else
		{
	        FAME_CONTEXT.sr = value & 0xFFFF;
		}
        break;

    default:
        return M68K_INV_REG;
    }

    return M68K_OK;
}

/*********************************************************/
/*  m68k_fetch(address,access_type)                      */
/*  Lee una palabra del espacio de memoria del 68k       */
/*  Parametro: Direccion de la palabra y tipo de acceso  */
/*  Retorno: La palabra o -1 en caso de dir. no valida   */
/*********************************************************/
s32 FAME_API(fetch)(u32 addr, u32 memory_space)
{
    u32 i=0;
    s32 val;
    M68K_DATA *ds = NULL;
    M68K_PROGRAM *ps = NULL;

    switch (memory_space & 2)
    {
    case M68K_SUP_ADDR_SPACE:
        if ((memory_space & 1) == M68K_PROG_ADDR_SPACE)
            ps = FAME_CONTEXT.sv_fetch;
        else
            ds = FAME_CONTEXT.sv_read_word;
        break;

    case M68K_USER_ADDR_SPACE:
        if ((memory_space & 1) == M68K_PROG_ADDR_SPACE)
            ps = FAME_CONTEXT.user_fetch;
        else
            ds = FAME_CONTEXT.user_read_word;
        break;
    }

    if (ps == NULL)
    {
        while ( ((ds[i].low_addr > addr) || (ds[i].high_addr < addr)) && (ds[i].low_addr != BANKEND_TAG) )
        {
#ifdef FAME_DEBUG
            printf("RW not found in %d region... 0x%08X - 0x%08X --> 0x%08X\n",i,ds[i].low_addr,ds[i].high_addr,addr);
#endif

            i++;
        }

        if (ds[i].low_addr == BANKEND_TAG)
        {
            /* Error de BUS */
#ifdef FAME_DEBUG
            printf("ERROR de BUS en region %d...\n",i);
#endif
            return -1;
        }
        else
        {
            if (ds[i].mem_handler != NULL)
            {
#ifdef FAME_DEBUG
                puts("Handled...\n");
#endif
                val= (*((mem16_handler_func *)&ds[i].mem_handler))(addr);
            }
            else
            {
#ifdef FAME_DEBUG
                printf("Ptr en region %d... addr: %p\n",i,ds[i].data);
#endif
                val = *((u16 *)(((u32)ds[i].data) + addr));
#ifdef FAME_DEBUG
                puts("read");
#endif
            }
        }

#ifdef FAME_DEBUG
        printf("Reading 0x%08X = 0x%04X...\n",addr,val);
#endif
    }
    else
    {
        u32 tmp=Fetch[((addr) >> M68K_FETCHSFT) & M68K_FETCHMASK];
        u16 *p= (u16*)(((addr) & M68K_ADDR_MASK) + (tmp));
        val = *p;
#ifdef FAME_DEBUG
        printf("@%08X *%p=%04X\n",addr,p,val);
#endif
    }

    return val;
}

/******************************************************/
/*  m68k_get_cycles_counter()                         */
/*  Retorna el cycles_counter                         */
/*  Parametro: Ninguno                                */
/*  Retorno: cycles_counter                           */
/******************************************************/
u32 FAME_API(get_cycles_counter)(void)
{
    if (FAME_CONTEXT.execinfo & M68K_RUNNING)
		return (cycles_to_do - io_cycle_counter) + FAME_CONTEXT.cycles_counter;
	else
		return FAME_CONTEXT.cycles_counter;
}

/******************************************************/
/*  m68k_trip_cycles_counter()                        */
/*  Retorna el cycles_counter y lo reinicializa       */
/*  Parametro: Ninguno                                */
/*  Retorno: cycles_counter                           */
/******************************************************/
u32 FAME_API(trip_cycles_counter)(void)
{
    s32 ret=FAME_CONTEXT.cycles_counter;

    if (FAME_CONTEXT.execinfo & M68K_RUNNING)
    {
        cycles_to_do-=io_cycle_counter;
        io_cycle_counter=0;
    }
    io_cycle_counter=FAME_CONTEXT.cycles_counter=0;
    return ret;
}

/**********************************************************/
/*  m68k_control_cycles_counter(n)                        */
/*  Retorna el cycles_counter y lo reinicializa si        */
/*  cycles_counter = n                                    */
/*  Parametro: ciclos = n                                 */
/*  Retorno: cycles_counter                               */
/**********************************************************/
u32 FAME_API(control_cycles_counter)(s32 cycles)
{
    return (cycles)?FAME_API(trip_cycles_counter)():FAME_API(get_cycles_counter)();
}

/******************************************************/
/*  m68k_release_timeslice()                          */
/*  Finaliza la ejecucion del micro                   */
/*   los ciclos sin ejecutar quedan en cycles_counter */
/*  Parametro: Ninguno                                */
/*  Retorno: Ninguno                                  */
/******************************************************/
void FAME_API(release_timeslice)(void)
{
    if (FAME_CONTEXT.execinfo & M68K_RUNNING)
    {
        io_cycle_counter = 0;
    }
}

/******************************************************/
/*  m68k_stop_emulating()                             */
/*  Finaliza la ejecucion del micro si esta en        */
/*  ejecucion. los ciclos ejecutados seran menos      */
/*  de lo normal reflejando asi la salida             */
/*  Parametro: Ninguno                                */
/*  Retorno: Ninguno                                  */
/******************************************************/
void FAME_API(stop_emulating)(void)
{
    if (FAME_CONTEXT.execinfo & M68K_RUNNING)
    {
		/*
		   es necesario quitar los ciclos de cycles_counter
		   para que no sean vistos como ejecutados al final
		   de emulate, ya que stop_emulating incrementa
		   el contador con los ejecutados realmente, no los
		   ciclos requeridos por la llamada a emulate
		*/
		FAME_CONTEXT.cycles_counter -= io_cycle_counter;
        io_cycle_counter = 0;
    }
}

/******************************************************/
/*  m68k_add_cycles()                                 */
/*  Incrementa los ciclos de reloj a ejecutar por la  */
/*  CPU en las llamadas a emulate                     */
/*  Parametro: Ninguno                                */
/*  Retorno: Ninguno                                  */
/******************************************************/
void FAME_API(add_cycles)(s32 cycles)
{
    if (FAME_CONTEXT.execinfo & M68K_RUNNING)
    {
    	/*
		  when the CPU is running, io_cycle_counter stores the remaining cycles to be run
		  therefore, we have to substract in order to "increment" the clock counter
    	*/
        io_cycle_counter -= cycles;
    }
    else
    {
        FAME_CONTEXT.cycles_counter += cycles;
    }
}


/******************************************************/
/*  m68k_release_cycles()                             */
/*  Decrementa los ciclos de reloj a ejecutar por la  */
/*  CPU en las llamadas a emulate                     */
/*  Parametro: Ninguno                                */
/*  Retorno: Ninguno                                  */
/******************************************************/
void FAME_API(release_cycles)(s32 cycles)
{
    if (FAME_CONTEXT.execinfo & M68K_RUNNING)
    {
    	/*
    	  when the CPU is running, io_cycle_counter stores the remaining cycles to be run
    	  therefore, we have to add in order to "decrement" the clock counter
    	*/
        io_cycle_counter += cycles;
    }
    else
    {
        FAME_CONTEXT.cycles_counter -= cycles;
    }
}

/*****************************************************************************/
/* m68k_get_cpu_state()                                                      */
/* No recibe parametros                                                      */
/* Retorna el estado de la CPU                                               */
/*****************************************************************************/
u32 FAME_API(get_cpu_state)(void)
{
    return FAME_CONTEXT.execinfo;
}


/*
 main exec function
*/

/*
 m68k_emulate()
 Parametros: Numero de ciclos a ejecutar
 Retorno: Exito de la operacion
          0  La operacion se ha realizado satisfactoriamente
          -1 La CPU esta detenida debido a un ERROR DE BUS DOBLE (linea)
             El PC ha salido de los limites (bucle no en linea)
*/
u32 FAME_API(emulate)(s32 cycles)
{
#ifdef FAME_GOTOS
    if (!initialised)
    {
        //BUILD_OPCODE_TABLE
        initialised = 1;
    }
#else
    assert(initialised);
#endif

#ifdef FAME_DEBUG
    printf("m68k_emulate(%d)\n",cycles);
#endif

#if 0
    /* El bit M68K_FAULTED no esta actualmente en uso */
    /* Comprobar si la CPU esta detenida debido a un doble error de bus */
    if (FAME_CONTEXT.execinfo & M68K_FAULTED) return (u32)-1;
#endif

    /* Poner la CPU en estado de ejecucion */
    FAME_CONTEXT.execinfo |= M68K_RUNNING;

    /* Cache SR */
    SET_SR(FAME_CONTEXT.sr)

    /* Cache PPL */
    flag_I = M68K_PPL;

    /* Fijar PC */
    SET_PC(FAME_CONTEXT.pc)

#ifdef FAME_DEBUG
    printf("->PC=0x%08X (%p-%p)\n",UNBASED_PC,PC,BasePC);
#endif

    /* guardar ciclos de ejecucion solicitados */
    io_cycle_counter = cycles_to_do = cycles;
    cycles_needed = 0;

#ifdef FAME_EMULATE_TRACE
    if (!(FAME_CONTEXT.execinfo & M68K_EMULATE_TRACE))
#endif
    {
        s32 line = interrupt_chk();
       if (line > 0)
        {
        	interrupt_attend(line);

#ifdef FAME_IRQ_CLOCKING
			if(io_cycle_counter <= 0)
			{
				finish_emulate(cycles_to_do - io_cycle_counter);
				return M68K_OK;
			}
#endif
        }
#ifdef FAME_EMULATE_TRACE
        else if (flag_T)
        {
            /* al atender la excepcion de traza, la CPU sale del estado parado */
            FAME_CONTEXT.execinfo &= ~M68K_HALTED;

            FAME_CONTEXT.execinfo |= M68K_EMULATE_TRACE;
            cycles_needed= io_cycle_counter;
            io_cycle_counter=0;
        }
#endif
    }

    /* Comprobar si la CPU esta parada */
    if (FAME_CONTEXT.execinfo & M68K_HALTED)
    {
        /* La CPU esta detenida mediante la instruccion STOP */
        /* Agregar ciclos de reloj requeridos */
        finish_emulate(cycles_to_do);
        return M68K_OK;
    }

#ifdef FAME_GOTOS
famec_Exec:
#endif

#ifdef FAME_DEBUG
    printf("Antes de NEXT... PC = 0x%08X\n",UNBASED_PC);
#endif

    NEXT

#ifdef FAME_GOTOS
#include "famec_opcodes.h"

famec_Exec_End:
#endif

#ifdef FAME_EMULATE_TRACE
    if (FAME_CONTEXT.execinfo & M68K_EMULATE_TRACE)
    {
        io_cycle_counter= cycles_needed;
        cycles_needed=0;
        FAME_CONTEXT.execinfo &= ~M68K_EMULATE_TRACE;
        FAME_CONTEXT.execinfo |= M68K_DO_TRACE;
        execute_exception(M68K_TRACE_EX);
        flag_T=0;
        if (io_cycle_counter > 0)
        {
            NEXT
        }
    }
    else
#endif
        if (cycles_needed>0)
			{
				s32 line=interrupt_chk();
				io_cycle_counter= cycles_needed;
				cycles_needed=0;
				if (line>0)
				{
					interrupt_attend(line);
				}
#ifdef FAME_EMULATE_TRACE
				else
					if (!(flag_T))
#endif
						if (io_cycle_counter > 0)
						{
							NEXT
						}
        }

	finish_emulate(cycles_to_do - io_cycle_counter);
    return M68K_OK;
}
