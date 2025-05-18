#ifndef __LCG_H
#define __LCG_H

#include <stdint.h>

class LCG {
 public:
  LCG(uint16_t s);

  void srnd(uint16_t s);
  uint8_t rnd();

 private:
  uint16_t seed;
};

#endif
