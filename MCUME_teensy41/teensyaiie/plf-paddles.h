#include <Arduino.h>

#include "physicalpaddles.h"

class PlfPaddles : public PhysicalPaddles {
 public:
  PlfPaddles(bool p0rev, bool p1rev);
  virtual ~PlfPaddles();

  void setRev(bool p0rev, bool p1rev);

  virtual void setPaddle0(uint8_t val);
  virtual void setPaddle1(uint8_t val);
  virtual uint8_t paddle0();
  virtual uint8_t paddle1();
  virtual void startReading();

  bool p0rev;
  bool p1rev;
};
