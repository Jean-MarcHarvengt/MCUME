#include "applevm.h"
#include "filemanager.h"
#include "cpu.h"

#include "appledisplay.h"
#include "applekeyboard.h"
#include "physicalkeyboard.h"

#include "globals.h"

AppleVM::AppleVM()
{
  // FIXME: all this typecasting makes me knife-stabby
  vmdisplay = new AppleDisplay(videoBuffer);
  mmu = new AppleMMU((AppleDisplay *)vmdisplay);
  vmdisplay->SetMMU((AppleMMU *)mmu);

  disk6 = new DiskII((AppleMMU *)mmu);
  ((AppleMMU *)mmu)->setSlot(6, disk6);

  keyboard = new AppleKeyboard((AppleMMU *)mmu);

#ifdef TEENSYDUINO
  teensyClock = new TeensyClock((AppleMMU *)mmu);
  ((AppleMMU *)mmu)->setSlot(7, teensyClock);
#else
  plfClock = new PlfClock((AppleMMU *)mmu);
  ((AppleMMU *)mmu)->setSlot(7, plfClock);
#endif
}

AppleVM::~AppleVM()
{
#ifdef TEENSYDUINO
  delete teensyClock;
#else
  delete plfClock;
#endif
  delete disk6;
}

// fixme: make member vars
unsigned long paddleCycleTrigger[2] = {0, 0};

void AppleVM::triggerPaddleInCycles(uint8_t paddleNum,uint16_t cycleCount)
{
  paddleCycleTrigger[paddleNum] = cycleCount + g_cpu->cycles;
}

void AppleVM::cpuMaintenance(uint32_t cycles)
{
  for (uint8_t i=0; i<2; i++) {
    if (paddleCycleTrigger[i] && cycles >= paddleCycleTrigger[i]) {
      ((AppleMMU *)mmu)->triggerPaddleTimer(i);
      paddleCycleTrigger[i] = 0;
    }
  }

  keyboard->maintainKeyboard(cycles);
}

void AppleVM::Reset()
{
  disk6->Reset();
  //((AppleMMU *)mmu)->resetRAM();
  mmu->Reset();

  g_cpu->pc = (((AppleMMU *)mmu)->read(0xFFFD) << 8) | ((AppleMMU *)mmu)->read(0xFFFC);

  // give the keyboard a moment to depress keys upon startup
  keyboard->maintainKeyboard(0);
}

void AppleVM::Monitor()
{
  g_cpu->pc = 0xff69; // "call -151"                                                                             
  ((AppleMMU *)mmu)->readSwitches(0xC054); // make sure we're in page 1                                                      
  ((AppleMMU *)mmu)->readSwitches(0xC056); // and that hires is off                                                          
  ((AppleMMU *)mmu)->readSwitches(0xC051); // and text mode is on                                                            
}

const char *AppleVM::DiskName(uint8_t drivenum)
{
  return disk6->DiskName(drivenum);
}

void AppleVM::ejectDisk(uint8_t drivenum, bool drawIt)
{
  disk6->ejectDisk(drivenum, drawIt);
}

void AppleVM::insertDisk(uint8_t drivenum, const char *filename, bool drawIt)
{
  disk6->insertDisk(drivenum, filename, drawIt);
}

VMKeyboard * AppleVM::getKeyboard()
{
  return keyboard;
}
