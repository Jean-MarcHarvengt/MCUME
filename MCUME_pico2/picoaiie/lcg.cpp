#include "lcg.h"

/* Dead simple 8-bit Linear Congruential Generator (PRNG).
 *
 * An LCG is defined as
 *
 *   X(n+1) = ( a * X(n) + c ) % m
 *
 * In this implementation:
 *   a = 2^7 = 128
 *   c = 251
 *   m = 256
 *
 * Like all LCGs, the low-order bits of this cycle quickly. The
 * high-order bits have better (longer) periods.
 */

LCG::LCG(uint16_t s)
{
  seed = s;
}

void LCG::srnd(uint16_t s)
{
  seed = s;
}

uint8_t LCG::rnd()
{
  seed = (seed << 7) - seed + 251;
  
  return (uint8_t)(seed + (seed>>8));
}
