#ifndef __VMDISPLAY_H
#define __VMDISPLAY_H

class MMU;

typedef struct {
  uint8_t top;
  uint16_t left;
  uint8_t bottom;
  uint16_t right;
} AiieRect;

class VMDisplay {
 public:
  VMDisplay(uint8_t *vb) { videoBuffer = vb; }
  virtual ~VMDisplay() { videoBuffer = NULL; };

  virtual void SetMMU(MMU *m) { mmu = m; }

  virtual bool needsRedraw() = 0;
  virtual void didRedraw() = 0;
  virtual AiieRect getDirtyRect() = 0;

  MMU *mmu;
  uint8_t *videoBuffer;
};

#endif
