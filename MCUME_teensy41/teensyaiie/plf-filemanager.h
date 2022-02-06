#ifndef __PLFFILEMANAGER_H
#define __PLFFILEMANAGER_H

#include "filemanager.h"
#include <stdint.h>

class PlfFileManager : public FileManager {
 public:
  PlfFileManager();
  virtual ~PlfFileManager();
  
  virtual int8_t openFile(const char *name);
  virtual void closeFile(int8_t fd);

  virtual const char *fileName(int8_t fd);

  virtual int8_t readDir(const char *where, const char *suffix, char fileDirectory[BIOS_MAXFILES][BIOS_MAXPATH+1], int16_t startIdx, uint16_t maxlen);
  virtual int16_t readDir(const char *where, const char *suffix, char *outputFN, int16_t startIdx, uint16_t maxlen);
  virtual void seekBlock(int8_t fd, uint16_t block, bool isNib = false);
  virtual bool readTrack(int8_t fd, uint8_t *toWhere, bool isNib = false);
  virtual bool readBlock(int8_t fd, uint8_t *toWhere, bool isNib = false);
  virtual bool readBlocks(int8_t fd, uint8_t *toWhere, uint8_t blocks, bool isNib = false);
  virtual bool writeBlock(int8_t fd, uint8_t *fromWhere, bool isNib = false);
  virtual bool writeTrack(int8_t fd, uint8_t *fromWhere, bool isNib = false);
  virtual bool readState(int8_t fd);
  virtual bool writeState(int8_t fd);

 private:
  int8_t numCached;
  char cachedNames[MAXFILES][MAXPATH];
  unsigned long fileSeekPositions[MAXFILES];
};


#endif
