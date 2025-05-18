#ifndef __CPU_H
#define __CPU_H

#include <stdlib.h>
#include <stdint.h>

class MMU;

// Flags (P) register bit definitions.
// Negative
#define F_N (1<<7)
// Overflow
#define F_V (1<<6)
#define F_UNK (1<<5) // What the heck is this?
// Break
#define F_B (1<<4)
// Decimal
#define F_D (1<<3)
// Interrupt Disable
#define F_I (1<<2)
// Zero
#define F_Z (1<<1)
// Carry
#define F_C (1<<0)

class Cpu {
 public:
  Cpu();
  ~Cpu();

  void Reset();

  void nmi();
  void rst();
  void brk();
  void irq();

  uint8_t Run(uint8_t numSteps);
  uint8_t step();

  uint8_t X();
  uint8_t Y();
  uint8_t A();
  uint16_t PC();
  uint8_t SP();
  uint8_t P();

  void stageIRQ();

 protected:
  // Stack manipulation
  void pushS8(uint8_t b);
  void pushS16(uint16_t w);
  uint8_t popS8();
  uint16_t popS16();

 public:
  void SetMMU(MMU *mmu) { this->mmu = mmu; }

 public:
  uint16_t pc;
  uint8_t sp;
  uint8_t a;
  uint8_t x;
  uint8_t y;
  uint8_t flags;

  uint64_t cycles;

  bool irqPending;
  
  MMU *mmu;
};



#endif
