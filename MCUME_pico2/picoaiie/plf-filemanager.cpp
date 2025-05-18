#include <string.h> // strcpy
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#include "emuapi.h"

#include "plf-filemanager.h"


PlfFileManager::PlfFileManager()
{
  numCached = 0;
}

PlfFileManager::~PlfFileManager()
{
}

int8_t PlfFileManager::openFile(const char *name)
{
  //emu_printf("openFile");
  //emu_printf(name);

  // See if there's a hole to re-use...
  for (int i=0; i<numCached; i++) {
    if (cachedNames[i][0] == '\0') {
      strncpy(cachedNames[i], name, MAXPATH-1);
      cachedNames[i][MAXPATH-1] = '\0'; // safety: ensure string terminator
      fileSeekPositions[i] = 0;
      return i;
    }
  }

  // check for too many open files
  if (numCached >= MAXFILES)
    return -1;

  // No, so we'll add it to the end
  strncpy(cachedNames[numCached], name, MAXPATH-1);
  cachedNames[numCached][MAXPATH-1] = '\0'; // safety: ensure string terminator
  fileSeekPositions[numCached] = 0;

  numCached++;
  return numCached-1;
}

void PlfFileManager::closeFile(int8_t fd)
{
  // invalid fd provided?
  if (fd < 0 || fd >= numCached)
    return;

  // clear the name
  cachedNames[fd][0] = '\0';
}

const char *PlfFileManager::fileName(int8_t fd)
{
  if (fd < 0 || fd >= numCached)
    return NULL;

  return cachedNames[fd];
}

int8_t PlfFileManager::readDir(const char *where, const char *suffix, char fileDirectory[BIOS_MAXFILES][BIOS_MAXPATH+1], int16_t startIdx, uint16_t maxlen)
{
  emu_printf("readdir should not be called!!!");
  return 0;
}

// suffix may be comma-separated
int16_t PlfFileManager::readDir(const char *where, const char *suffix, char *outputFN, int16_t startIdx, uint16_t maxlen)
{
  emu_printf("readdir should not be called!!!");
  return 0;
}

void PlfFileManager::seekBlock(int8_t fd, uint16_t block, bool isNib)
{
  //emu_printf("seekBlock\n");
  //emu_printi(block);
  if (fd < 0 || fd >= numCached)
    return;

  if (isNib) {
    fileSeekPositions[fd] = block * 416;
  } else {
    fileSeekPositions[fd] = block * 256;
  }
}


bool PlfFileManager::readTrack(int8_t fd, uint8_t *toWhere, bool isNib)
{
  if (fd < 0 || fd >= numCached)
    return false;

  if (cachedNames[fd][0] == 0)
    return false;

  //printf("readTrack %s %d %lu\n", cachedNames[fd], isNib, (unsigned long)toWhere);
  //emu_printf("readTrack");

  // open, seek, read, close.
  bool ret = false;
  int f = emu_FileOpen(cachedNames[fd], "r+b");
  emu_FileSeek(f, fileSeekPositions[fd], 0);
  if (isNib) {
    ret = (emu_FileRead((char*)toWhere, 0x1A00, f) == 0x1A00);
  } else {
    int read = emu_FileRead((char*)toWhere, 256 * 16, f);
   //printf("track read %d\n", read);
    ret = ( read == 256 * 16);
  }
  //emu_printf("track read\n");
  emu_FileClose(f);

  return ret;
}

bool PlfFileManager::readBlock(int8_t fd, uint8_t *toWhere, bool isNib)
{
  //emu_printf("readBlock");

  // open, seek, read, close.
  if (fd < 0 || fd >= numCached)
    return false;

  if (cachedNames[fd][0] == 0)
    return false;

  // open, seek, read, close.
  bool ret = false;
  int f = emu_FileOpen(cachedNames[fd], "r+b");
  if (f != -1) {
    emu_FileSeek(f, fileSeekPositions[fd], SEEK_SET);
    if (isNib) {
      ret = (emu_FileRead(toWhere, 416, f) == 416);
    } else {
      ret = (emu_FileRead(toWhere, 256, f) == 256);
    }
    emu_FileClose(f);
  }

  return ret;
}

bool PlfFileManager::writeBlock(int8_t fd, uint8_t *fromWhere, bool isNib)
{
  emu_printf("writeBlock should not be called");
#ifdef unused
  // open, seek, write, close.
  if (fd < 0 || fd >= numCached)
    return false;

  if (cachedNames[fd][0] == 0)
    return false;

  // don't know how to do this without seeking through the nibblized
  // track data, so just give up for now
  if (isNib)
    return false;

  // open, seek, write, close.
  int ffd = open(cachedNames[fd], O_WRONLY);
  if (ffd != -1) {
    if (lseek(ffd, fileSeekPositions[fd], SEEK_SET) != fileSeekPositions[fd]) {
      printf("ERROR: failed to seek to %lu\n", fileSeekPositions[fd]);
      return false;
    }
    if (write(ffd, fromWhere, 256) != 256) {
      printf("ERROR: failed to write 256 bytes\n");
      return false;
    }
    close(ffd);
  }
#endif  
  return true;
}


bool PlfFileManager::readState(int8_t fd) {
    return false;
}

bool PlfFileManager::writeState(int8_t fd){
    return false;
}

bool PlfFileManager::readBlocks(int8_t fd, uint8_t *toWhere, uint8_t blocks, bool isNib)
{
  emu_printf("readBlocks should not be called\n");
  return false;
}



bool PlfFileManager::writeTrack(int8_t fd, uint8_t *fromWhere, bool isNib)
{
  emu_printf("writeTrackshould not be called");
#ifdef unused
  // open, seek, write, close.
  if (fd < 0 || fd >= numCached)
    return false;

  if (cachedNames[fd][0] == 0)
    return false;

  // open, seek, write, close.
  int ffd = open(cachedNames[fd], O_WRONLY);
  if (ffd != -1) {
    if (lseek(ffd, fileSeekPositions[fd], SEEK_SET) != fileSeekPositions[fd]) {
      printf("ERROR: failed to seek to %lu\n", fileSeekPositions[fd]);
      return false;
    }
    int16_t wrsize = 256 * 16;
    if (isNib)
      wrsize = 0x1A00;

    if (write(ffd, fromWhere, wrsize) != wrsize) {
      printf("ERROR: failed to write bytes\n");
      return false;
    }
    close(ffd);
  }
#endif  
  return true;
}
