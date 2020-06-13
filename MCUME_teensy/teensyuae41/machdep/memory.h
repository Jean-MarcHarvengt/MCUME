 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * Memory access functions
  *
  * Copyright 1996 Bernd Schmidt
  */

static __inline__ ULONG do_get_mem_long(ULONG *a)
{
    UBYTE *b = (UBYTE *)a;
    
    return (*b << 24) | (*(b+1) << 16) | (*(b+2) << 8) | (*(b+3));
}

static __inline__ UWORD do_get_mem_word(UWORD *a)
{
    UBYTE *b = (UBYTE *)a;
    
    return (*b << 8) | (*(b+1));
}

static __inline__ UBYTE do_get_mem_byte(UBYTE *a)
{
    return *a;
}

static __inline__ void do_put_mem_long(ULONG *a, ULONG v)
{
    UBYTE *b = (UBYTE *)a;
    
    *b = v >> 24;
    *(b+1) = v >> 16;    
    *(b+2) = v >> 8;
    *(b+3) = v;
}

static __inline__ void do_put_mem_word(UWORD *a, UWORD v)
{
    UBYTE *b = (UBYTE *)a;
    
    *b = v >> 8;
    *(b+1) = v;
}

static __inline__ void do_put_mem_byte(UBYTE *a, UBYTE v)
{
    *a = v;
}

#define call_mem_get_func(func, addr) ((*func)(addr))
#define call_mem_put_func(func, addr, v) ((*func)(addr, v))

#undef USE_MAPPED_MEMORY
#undef CAN_MAP_MEMORY
#undef NO_INLINE_MEMORY_ACCESS
#undef MD_HAVE_MEM_1_FUNCS

