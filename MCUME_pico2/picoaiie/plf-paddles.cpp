#include "plf-paddles.h"

/* C061: Open Apple (Paddle 0 button pressed if &= 0x80)
 * C062: Closed Apple (Paddle 1 button pressed if &= 0x80)
 * C064: PADDLE0 (sets bit 0x80 when value reached, increments of 11 us)
 * C065: PADDLE1 (sets bit 0x80 when value reached, increments of 11 us)
 * C070: "start reading paddle data" - "may take up to 3 milliseconds"
 */

#include "globals.h"

static uint8_t val0=128;
static uint8_t val1=128;


PlfPaddles::PlfPaddles(bool p0rev, bool p1rev)
{
  this->p0rev = p0rev;
  this->p1rev = p1rev;
}

PlfPaddles::~PlfPaddles()
{
}

void PlfPaddles::setPaddle0(uint8_t val)
{
  val0 = val;
}

void PlfPaddles::setPaddle1(uint8_t val)
{
  val1 = val;
}

uint8_t PlfPaddles::paddle0()
{
  uint8_t raw = val0; // 255: LEFT, 0:RIGHT
  if (p0rev) {
    raw = 255 - raw;
  }
  return raw;
}

uint8_t PlfPaddles::paddle1()
{
  uint8_t raw = val1;
  if (p1rev) {
    raw = 255 - raw;
  }
  return raw;
}

void PlfPaddles::startReading()
{
  g_vm->triggerPaddleInCycles(0, 12 * paddle0());
  g_vm->triggerPaddleInCycles(1, 12 * paddle1());
}

void PlfPaddles::setRev(bool p0rev, bool p1rev)
{
  this->p0rev = p0rev;
  this->p1rev = p1rev;
}
