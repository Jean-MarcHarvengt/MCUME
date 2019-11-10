#ifndef _MEMORY_H_
#define _MEMORY_H_

extern uint8 * vram;
extern uint16 * vramw;
extern uint8 * pceram;
extern uint8 * cdram;
extern uint8 * bram;

extern uint8 * objram;
extern uint16 *objramw;


#define BG_CACHE_SIZE  0x20000
#define OBJ_CACHE_SIZE 0x40000 //0x60000 //0x7E800 //0x80000

extern uint8 * bg_pattern_cache;
extern uint8 * obj_pattern_cache;
extern uint8 * bg_name_dirty;
extern uint16 * bg_name_list;
extern uint16 * obj_name_dirty;
extern uint16 * obj_name_list;




extern void  mem_init(void);
extern uint8 read_rom(int address);
extern void  write_rom(int address, uint8 val);
extern void memcpy_rom(int dst, int src, int size);

#endif
