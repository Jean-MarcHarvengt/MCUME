#ifndef __FILEMANAGER_H
#define __FILEMANAGER_H

#include <stdint.h>
#include "bios.h"

#define MAXFILES 4    // how many results we can simultaneously manage
#define DIRPAGESIZE 10 // how many results in one readDir
#ifndef MAXPATH
  #define MAXPATH 255
#endif

class FileManager {
 public:
  virtual ~FileManager() {};

  virtual int8_t openFile(const char *name) = 0;
  virtual void closeFile(int8_t fd) = 0;

  virtual const char *fileName(int8_t fd) = 0;

  virtual int8_t readDir(const char *where, const char *suffix, char fileDirectory[BIOS_MAXFILES][BIOS_MAXPATH+1], int16_t startIdx, uint16_t maxlen) = 0;
  virtual int16_t readDir(const char *where, const char *suffix, char *outputFN, int16_t startIdx, uint16_t maxlen) = 0;
  virtual void seekBlock(int8_t fd, uint16_t block, bool isNib = false) = 0;
  virtual bool readTrack(int8_t fd, uint8_t *toWhere, bool isNib = false) = 0;
  virtual bool readBlock(int8_t fd, uint8_t *toWhere, bool isNib = false) = 0;
  virtual bool readBlocks(int8_t fd, uint8_t *toWhere, uint8_t blocks, bool isNib = false) = 0;
  virtual bool writeBlock(int8_t fd, uint8_t *fromWhere, bool isNib = false) = 0;
  virtual bool writeTrack(int8_t fd, uint8_t *fromWhere, bool isNib = false) = 0;
  virtual bool readState(int8_t fd) = 0;
  virtual bool writeState(int8_t fd) = 0;
};

#endif
