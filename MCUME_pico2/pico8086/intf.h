#include "pico.h"
#include "pico/stdlib.h"

//  #define RAM_SIZE 0xF8000 // 512k //655360UL
//  #define NATIVE_RAM (0X28000) //  128k+32768 //231424UL



#define RAM_SIZE 0x80000 // 512k //655360UL
#define NATIVE_RAM (0X40000) //  256k
#define NATIVE_START 0UL



#define ROM_READ(a,b) a[b]


//#define INCLUDE_ROM_BASIC


//#define BOOT_FDD
#define BOOT_HDD
//#define BOOT_BASIC

//#define FDD_180K
//#define FDD_320K
//#define FDD_360K
//#define FDD_720K
//#define FDD_122M
//#define FDD_144M


#define BAUD_RATE 1000000

//#define USE_NETWORKING
//#define USE_PARALLEL

//#define PROFILING

// END ARDUINO86 USER CONFIGURABLE OPTIONS

#define regax 0
#define regcx 1
#define regdx 2
#define regbx 3
#define regsp 4
#define regbp 5
#define regsi 6
#define regdi 7
#define reges 0
#define regcs 1
#define regss 2
#define regds 3

#define regal 0
#define regah 1
#define regcl 2
#define regch 3
#define regdl 4
#define regdh 5
#define regbl 6
#define regbh 7

#define StepIP(x) ip+=x
#define getmem8(x,y) read86(segbase(x)+(uint32_t)y)
//#define getmem16(x,y) (read86(segbase(x)+y) | ((uint16_t)read86(segbase(x)+y+1)<<8))
#define getmem16(x,y) readw86(segbase(x)+(uint32_t)y)
#define putmem8(x,y,z) write86(segbase(x)+(uint32_t)y, z)
//#define putmem16(x,y,z) write86(segbase(x)+y, ((z)&0xFF)); write86(segbase(x)+y+1, (((z)>>8)&0xFF))
#define putmem16(x,y,z) writew86(segbase(x)+(uint32_t)y, z)
#define signext(value) ((((uint16_t)value&0x80)*0x1FE)|(uint16_t)value)
#define signext32(value) ((((uint32_t)value&0x8000)*0x1FFFE)|(uint32_t)value)
#define getreg16(regid) regs.wordregs[regid]
#define getreg8(regid) regs.byteregs[byteregtable[regid]]
#define putreg16(regid, writeval) regs.wordregs[regid] = writeval
#define putreg8(regid, writeval) regs.byteregs[byteregtable[regid]] = writeval
#define getsegreg(regid) segregs[regid]
#define putsegreg(regid, writeval) segregs[regid] = writeval
#define segbase(x) ((uint32_t)x<<4)

#define makeflagsword() (2 | (uint16_t)cf | ((uint16_t)pf << 2) | ((uint16_t)af << 4) | ((uint16_t)zf << 6) \
        | ((uint16_t)sf << 7) | ((uint16_t)tf << 8) | ((uint16_t)ifl << 9) | ((uint16_t)df << 10) | ((uint16_t)of << 11))

#define decodeflagsword(x) {\
        temp16 = x;\
        cf = temp16 & 1;\
        pf = (temp16 >> 2) & 1;\
        af = (temp16 >> 4) & 1;\
        zf = (temp16 >> 6) & 1;\
        sf = (temp16 >> 7) & 1;\
        tf = (temp16 >> 8) & 1;\
        ifl = (temp16 >> 9) & 1;\
        df = (temp16 >> 10) & 1;\
        of = (temp16 >> 11) & 1;\
}


void setup_memory();
void setup_timer();
uint8_t insertdisk(uint8_t drivenum);
void reset86();
void exec86(uint32_t execloops);
uint8_t read86(uint32_t addr32);
void write86(uint32_t addr32, uint8_t value);
void doirq(uint8_t irqnum);
void incsends();
void init_display();
void write_video(uint16_t addr);
void clear_display();
void palettereset();
void display_CSIP();
void ps2poll();
void setup_ps2(uint8_t data_pin, uint8_t irq_pin);
void video_init();
uint8_t VRAM_read(uint32_t addr32);
void VRAM_write(uint32_t addr32, uint8_t value);
void setup_ps2(uint8_t data_pin, uint8_t irq_pin);
void ps2poll();
void out8253 (uint16_t portnum, uint8_t value);
uint8_t in8253 (uint16_t portnum);
void init8259();
void out8259(uint16_t portnum, uint8_t value);
uint8_t in8259(uint16_t portnum);

extern uint8_t port3da;

void initDisk(char * filename);
void init8253();
void net_init();
void net_loop();
void net_handler();
uint8_t cached_read(uint32_t addr32);
void cached_write(uint32_t addr32, uint8_t value);
void cache_init();

uint8_t net_read_ram(uint32_t addr32);
void diskhandler();

//extern uint8_t SPI_RAM_pins[8];
extern uint8_t net_mac[6];
extern uint8_t bufSerial[1600];
void outByte(uint8_t cc);

extern struct i8253_s i8253;
union _bytewordregs_{
  uint16_t wordregs[8];
  uint8_t byteregs[8];
} ;
