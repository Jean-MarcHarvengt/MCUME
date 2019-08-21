
#include "Atari2600EmulatorGlobals.h"

// indicates if the emulator thread is running or not
int bThreadRunning = 0;

//------------------------
// options
//------------------------
byte nOptions_SkipFrames = 1;
//int nOptions_SoundBufSize = 768;
int nOptions_SoundOn = 1;
//int nOptions_SoundSampleRate = 2;
//int nOptions_SoundVolume=50;
//int nOptions_SoundCreateRate = 341;
int nOptions_Interlace = 0;
int nOptions_Landscape = 0;
int nOptions_Color = 1;
int nOptions_P1Diff=1;
int nOptions_P2Diff=1;
extern int pausing = 0;

