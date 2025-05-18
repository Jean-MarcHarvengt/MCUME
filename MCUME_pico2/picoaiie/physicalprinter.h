#ifndef __PHYSICALPRINTER_H
#define __PHYSICALPRINTER_H

class PhysicalPrinter {
 public:
  virtual ~PhysicalPrinter() {}
  // must be 960 pixels wide (120 bytes)
  virtual void addLine(uint8_t *rowOfBits) = 0; 
  virtual void update() = 0;
  virtual void moveDownPixels(uint8_t p) = 0;
};

#endif
