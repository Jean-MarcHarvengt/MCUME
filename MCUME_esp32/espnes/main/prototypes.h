#ifndef __PROTOTYPES_H__
#define __PROTOTYPES_H__

extern void fatalerr(char *errmsg);

extern bool mapperinit(struct cart_s *cart);
extern void CHRswap(struct cart_s *cart, uint16_t banknum, uint16_t newbank, uint16_t banksize);
extern void map4irqdecrement();
extern uint8_t mapperwrite(uint16_t addr, uint8_t value);

extern uint64_t exec6502(uint64_t tickcount);
extern void step6502();
extern void reset6502();
extern void nmi6502();
extern uint8_t read6502(uint16_t addr);
extern void write6502(uint16_t addr, uint8_t value);
extern void irq6502();

extern void renderscanline(uint16_t scanline);

extern bool loadROM(char *filename);
extern void execNES();
extern void execframe(bool skip);

extern void hookexternal(void *funcptr);
extern void initPPU();
extern void killPPU();
extern uint8_t readPPU(uint16_t addr);
extern void writePPU(uint16_t addr, uint8_t value);
extern uint8_t readPPUregs(uint16_t addr);
extern void writePPUregs(uint16_t addr, uint8_t value);
//extern bool initAPU(uint8_t CPUmode, uint32_t samplerate);
//extern void killAPU();
//extern void tickchannelsAPU();
//extern uint8_t readAPU(uint16_t addr);
//extern void writeAPU(uint16_t addr, uint8_t value);

#endif
