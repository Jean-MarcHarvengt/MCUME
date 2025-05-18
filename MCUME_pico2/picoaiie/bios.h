#ifndef __BIOS_H
#define __BIOS_H

//#include <Arduino.h>
#include <stdint.h>

#define BIOS_MAXFILES 10 // number of files in a page of listing
#define BIOS_MAXPATH 127  // maximum length of a single filename that we'll support

class BIOS {
 public:
  BIOS();
  ~BIOS();

  // return true if a persistent change needs to be stored in EEPROM
  bool runUntilDone();
  bool updateDiagnostics();

 private:
  uint8_t GetAction(int8_t &prevAction);
  bool isActionActive(int8_t action);
  void DrawMainMenu(int8_t selection);
  void UpdateMainMenu(int8_t selection);
  void DrawMenuItem(int8_t item, int8_t selection);
  void ClearMenuItem(int8_t item);
  void drawKeyboard();
  void drawInfo();
  void drawThumb();
  bool stickTrim();
  bool stickSpeed();

  void WarmReset();
  void ColdReboot();

  bool SelectDiskImage();
  void DrawDiskName(uint8_t index, uint8_t page, int8_t selection);
  void DrawDiskNames(uint8_t page, int8_t selection, bool force);
  uint8_t GatherFilenames(uint8_t pageOffset);
  bool HasNextPage(uint8_t pageOffset);
//  bool GetFilename(uint8_t pageOffset, uint8_t index);

  void stripDirectory();
  bool saveState();
  bool restoreState();

 private:
  void biosIdle();
  void drawBiosScreen(int8_t prevAction);
  int8_t selectedFile;
  char fileDirectory[BIOS_MAXFILES][BIOS_MAXPATH+1];
  int8_t fileCount;
  bool hasNextPage;
  bool inMainMenu;
//  char fileName[BIOS_MAXPATH + 1];
  char rootPath[255-BIOS_MAXPATH];
  int8_t dirPage;
};

#endif
