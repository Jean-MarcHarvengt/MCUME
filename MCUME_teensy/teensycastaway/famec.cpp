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

#include "dcastaway.h"

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


#include <Arduino.h>
/* Lookup IRQ level to attend */
/* Indexed by interrupts[0] */
PROGMEM static const u8 irq_level_lookup[256] =
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
PROGMEM static const s32 exception_cycle_table[256] =
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
