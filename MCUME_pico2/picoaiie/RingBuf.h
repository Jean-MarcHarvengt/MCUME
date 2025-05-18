#ifndef __RINGBUF_H
#define __RINGBUF_H

#include <stdint.h>

class RingBuf {
 public:
  RingBuf(int16_t length);
  ~RingBuf();

  void clear();

  bool isFull();
  bool hasData();
  bool addByte(uint8_t b);
  bool addBytes(uint8_t *b, int count);
  bool replaceByte(uint8_t b);
  uint8_t consumeByte();
  uint8_t peek(int16_t idx);
  uint16_t getPeekCursor();
  void setPeekCursor(int16_t idx);
  void resetPeekCursor();
  uint8_t peekNext();
  int16_t count();

 private:
  uint8_t *buffer;
  int16_t max;
  int16_t ptr;
  int16_t fill;
  int16_t cursor;
};

#endif
