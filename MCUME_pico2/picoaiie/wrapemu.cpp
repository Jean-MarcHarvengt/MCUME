#include <stdio.h>
#include <string.h>

extern "C" {
#include "emuapi.h"
#include "platform_config.h"
}


// Apple2 2 emulation includes
#include "bios.h"
#include "cpu.h"
#include "applevm.h"
#include "plf-display.h"
#include "plf-keyboard.h"
#include "plf-speaker.h"
#include "plf-paddles.h"
#include "plf-filemanager.h"
#include "globals.h"

#include <sys/time.h>

double time_micro(void)
{
   struct timeval tv;
   double t;

   gettimeofday(&tv, 0);

   t = (double)tv.tv_sec + ((double)tv.tv_usec / 1E6);

   return t;
}


uint32_t nextInstructionMicros;
uint32_t startMicros;


static int ik;    // joypad key
static int pik=0; 

static int ihk;   // I2C keyboard key
static int iusbhk;// USB keyboard key
static int prevhk;

void aiie_Input(int bClick) {
  ik  = emu_GetPad();
  ihk = emu_ReadI2CKeyboard();
}

void emu_KeyboardOnDown(int keymodifer, int key) {
  int keyCode = -1; //INV_KEY;
  if ((key >=0xc0) && (key <=0xdf)) {
    keyCode = ((key-0xc0) & 0x1f) + 0x7f;
  }
  else {
    keyCode = key & 0x7f;
  }
  
  //Serial.println(keyCode);
  
  if (keyCode != -1) {
    iusbhk = keyCode;
  }
}

void emu_KeyboardOnUp(int keymodifer, int key) {
  iusbhk = 0;
}





void aiie_Init(void)
{
  g_speaker = new PlfSpeaker(0);

  // First create the filemanager - the interface to the host file system.
  g_filemanager = new PlfFileManager();

  // Construct the interface to the host display. This will need the
  // VM's video buffer in order to draw the VM, but we don't have that
  // yet. 
  g_display = new PlfDisplay();

  // Next create the virtual CPU. This needs the VM's MMU in order to
  // run, but we don't have that yet.
  g_cpu = new Cpu();

  // Create the virtual machine. This may read from g_filemanager to
  // get ROMs if necessary.  (The actual Apple VM we've built has them
  // compiled in, though.) It will create its virutal hardware (MMU,
  // video driver, floppy, paddles, whatever).
  g_vm = new AppleVM();

  // Now that the VM exists and it has created an MMU, we tell the CPU
  // how to access memory through the MMU.
  g_cpu->SetMMU(g_vm->getMMU());

  // And the physical keyboard needs hooks in to the virtual keyboard...
  g_keyboard = new PlfKeyboard(g_vm->getKeyboard());

  g_paddles = new PlfPaddles(1, 1);

  // Now that all the virtual hardware is glued together, reset the VM
  g_vm->Reset();

  g_display->redraw();

  g_cpu->cycles = 0;
  startMicros = nextInstructionMicros = time_micro();
  startMicros = 0;
  nextInstructionMicros = time_micro();

#ifdef HAS_SND  
  emu_sndInit();
#endif  
}


void aiie_Start(char * filename)
{
  emu_printf("emu starting");

  ((AppleVM *)g_vm)->insertDisk(0, filename, false);

  emu_printf("emu started");
}

static int padx = 128;
static int pady = 128;
static int padxinc = 0;
static int padyinc = 0;

void aiie_Step(void)
{
  int n = 1000;
  while (n-- > 0) {
  //if (micros() >= nextInstructionMicros) {
    g_cpu->Run(24);
    //g_speaker->beginMixing();
    ((AppleVM *)g_vm)->cpuMaintenance(g_cpu->cycles);
    //g_speaker->maintainSpeaker(g_cpu->cycles);
    // The CPU of the Apple //e ran at 1.023 MHz. Adjust when we think
    // the next instruction should run based on how long the execution
    // was ((1000/1023) * numberOfCycles) - which is about 97.8%.
    nextInstructionMicros = startMicros + ((double)g_cpu->cycles * 0.978d);
  //}
  }

  ((AppleVM*)g_vm)->disk6->fillDiskBuffer();
  g_keyboard->maintainKeyboard();
  g_vm->vmdisplay->needsRedraw();
  AiieRect what = g_vm->vmdisplay->getDirtyRect();
  g_vm->vmdisplay->didRedraw();
  g_display->blit(what);
  
  emu_DrawVsync();
 
  int hk=ihk;
  if (iusbhk) hk = iusbhk;

  if ( (hk) && (hk != prevhk) ) {
    if (hk == 10) hk=RET;
    else if (hk == 153) hk=UARR; //U L R D
    else if (hk == 151) hk=LARR;
    else if (hk == 150) hk=RARR;
    else if (hk == 152) hk=DARR;
    else if (hk == 127) hk=DEL;
    prevhk = hk;
    g_keyboard->onPress(hk);
//    Serial.println(hk);
  }
  else if ( (prevhk) && (!hk) ) {
    g_keyboard->onRelease(prevhk);
//    Serial.println("release");
    prevhk = 0;  
  }


  int k=ik; 
#ifdef TEECOMPUTER
  // Ignore joypad if shift/fn is pressed!!!
  if ( !(k & MASK_KEY_USER1) && !(k & MASK_KEY_USER2) )
#endif
  {
    if ( !(pik & MASK_JOY2_BTN) && (k & MASK_JOY2_BTN) ) {
      g_keyboard->onPress(RA);
      g_keyboard->onPress(LA);
    }
    else if ( (pik & MASK_JOY2_BTN) && !(k & MASK_JOY2_BTN) ) {
      g_keyboard->onRelease(RA);
      g_keyboard->onRelease(LA);
   } 

    if ( !(pik & MASK_JOY1_BTN) && (k & MASK_JOY1_BTN) ) {
      g_keyboard->onPress(RA);
      g_keyboard->onPress(LA);

    }
    else if ( (pik & MASK_JOY1_BTN) && !(k & MASK_JOY1_BTN) ) {
      g_keyboard->onRelease(RA);
      g_keyboard->onRelease(LA);
    } 

    if ( (k & MASK_JOY2_RIGHT) || (k & MASK_JOY1_RIGHT) )  {
      if (padxinc < 0) padxinc = 0;
      if (padxinc != +1) padxinc += 1;
      if (padx != 255) padx += padxinc;
      g_paddles->setPaddle0(padx);
    }
    else if ( (k & MASK_JOY2_LEFT) ||  (k & MASK_JOY1_LEFT) )  {
      if (padxinc > 0) padxinc = 0;
      if (padxinc != -1) padxinc -= 1;
      if (padx != 0) padx += padxinc;
      g_paddles->setPaddle0(padx);
    }

    if ( (k & MASK_JOY2_UP) || (k & MASK_JOY1_UP) ) {
      if (padyinc < 0) padyinc = 0;
      if (padyinc != 1) padyinc += 1;
      if (pady != 255) pady += padyinc;
      g_paddles->setPaddle1(pady);
    }
    else if ( (k & MASK_JOY2_DOWN) ||  (k & MASK_JOY1_DOWN) ) {
      if (padyinc > 0) padyinc = 0;
      if (padyinc != -1) padyinc -= 1;
      if (pady != 0) pady += padyinc;
      g_paddles->setPaddle1(pady);
    }
  }  
  pik = k;


#ifdef HAS_SND  
#endif  
}


 
