/** EMULib Emulation Library *********************************/
/**                                                         **/
/**                          Sound.h                        **/
/**                                                         **/
/** This file defines standard sound generation API and     **/
/** functions needed to log soundtrack into a MIDI file.    **/
/** See Sound.c and the sound drivers for the code.         **/ 
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1996-2003                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/
#ifndef SOUND_H
#define SOUND_H

                               /* SetSound() arguments:      */
#define SND_MELODIC     0      /* Melodic sound (default)    */
#define SND_RECTANGLE   0      /* Rectangular wave           */
#define SND_TRIANGLE    1      /* Triangular wave (1/2 rect.)*/
#define SND_NOISE       2      /* White noise                */
#define SND_PERIODIC    3      /* Periodic noise (not im-ed) */
#define SND_WAVE        4      /* Wave sound set by SetWave()*/
#define SND_MIDI        0x100  /* MIDI instrument (ORable)   */

                               /* Drum() arguments:          */
#define DRM_CLICK       0      /* Click (default)            */
#define DRM_MIDI        0x100  /* MIDI drum (ORable)         */

                               /* MIDI characteristics:      */
#define MIDI_CHANNELS   16     /* Number of MIDI channels    */
#define MIDI_MINFREQ    9      /* Min MIDI frequency (Hz)    */
#define MIDI_MAXFREQ    12285  /* Max MIDI frequency (Hz)    */
#define MIDI_DIVISIONS  1000   /* Number of ticks per second */

                               /* MIDILogging() arguments:   */
#define MIDI_OFF        0      /* Turn MIDI logging off      */
#define MIDI_ON         1      /* Turn MIDI logging on       */
#define MIDI_TOGGLE     2      /* Toggle MIDI logging        */
#define MIDI_QUERY      3      /* Query MIDI logging status  */

/** TrashSound() *********************************************/
/** Shut down sound driver. Each driver implements its own  **/
/** TrashSound() function.                                  **/
/*************************************************************/
void TrashSound(void);

/** Sound() **************************************************/
/** Generate sound of given frequency (Hz) and volume       **/
/** (0..255) via given channel. Setting Freq=0 or Volume=0  **/
/** turns sound off.                                        **/
/*************************************************************/
void Sound(int Channel,int Freq,int Volume);

/** Drum() ***************************************************/
/** Hit a drum of given type with given force (0..255).     **/
/** MIDI drums can be used by ORing their numbers with      **/
/** SND_MIDI.                                               **/
/*************************************************************/
void Drum(int Type,int Force);

/** SetSound() ***********************************************/
/** Set sound type at a given channel. MIDI instruments can **/
/** be set directly by ORing their numbers with SND_MIDI.   **/
/*************************************************************/
void SetSound(int Channel,int NewType);

/** SetChannels() ********************************************/
/** Set master volume (0..255) and switch channels on/off.  **/
/** Each channel N has corresponding bit 2^N in Switch. Set **/
/** or reset this bit to turn the channel on or off.        **/ 
/*************************************************************/
void SetChannels(int Volume,int Switch);

/** SetWave() ************************************************/
/** Set waveform for a given channel. The channel will be   **/
/** marked with sound type SND_WAVE. Set Rate=0 if you want **/
/** waveform to be an instrument or set it to the waveform  **/
/** own playback rate.                                      **/
/*************************************************************/
void SetWave(int Channel,const signed char *Data,int Length,int Rate);

/** InitMIDI() ***********************************************/
/** Initialize soundtrack logging into MIDI file FileName.  **/
/** Repeated calls to InitMIDI() will close current MIDI    **/
/** file and continue logging into a new one.               **/ 
/*************************************************************/
void InitMIDI(const char *FileName);

/** TrashMIDI() **********************************************/
/** Finish logging soundtrack and close the MIDI file.      **/
/*************************************************************/
void TrashMIDI(void);

/** MIDILogging() ********************************************/
/** Turn soundtrack logging on/off and return its current   **/
/** status. Possible values of Switch are MIDI_OFF (turn    **/
/** logging off), MIDI_ON (turn logging on), MIDI_TOGGLE    **/
/** (toggle logging), and MIDI_QUERY (just return current   **/
/** state of logging).                                      **/
/*************************************************************/
int MIDILogging(int Switch);

/** MIDITicks() **********************************************/
/** Log N 1ms MIDI ticks.                                   **/
/*************************************************************/
void MIDITicks(int N);

#define SND_CHANNELS    16   /* Number of channels           */
#define SND_SAMPLESIZE  128 //256  /* Max. SetWave() sample size   */
#define SND_BUFSIZE     128 //256  /* Buffer size, <= 2^SND_BITS   */
#define SND_BITS        8    /* Number of bits in a fragment */
#define SND_BUFFERS     8    /* Number of fragments, >= 2    */
    /* Bigger value results in better behaviour on loaded    */
    /* but output gets more delayed.                         */

/** InitSound() **********************************************/
/** Initialize Unix sound driver with given synthesis rate. **/
/** Returns Rate on success, 0 otherwise. Pass Rate=0 to    **/
/** skip initialization and be silent. Pass Verbose!=0 to   **/
/** see initialization messages.                            **/
/*************************************************************/
int InitSound(int Rate,int Verbose);

/** StopSound() **********************************************/
/** Temporarily suspend sound.                              **/
/*************************************************************/
void StopSound(void);

/** ResumeSound() ********************************************/
/** Resume sound after StopSound().                         **/
/*************************************************************/
void ResumeSound(void);


/** SndDriver ************************************************/
/** Each sound driver should fill this structure with       **/
/** pointers to hardware-dependent handlers. This has to be **/
/** done inside the InitSound() function.                   **/
/*************************************************************/
struct SndDriverStruct
{
  void (*SetSound)(int Channel,int NewType);
  void (*Drum)(int Type,int Force);
  void (*SetChannels)(int Volume,int Switch);
  void (*Sound)(int Channel,int NewFreq,int NewVolume);
  void (*SetWave)(int Channel,const signed char *Data,int Length,int Freq);
};
extern struct SndDriverStruct SndDriver;

#endif /* SOUND_H */
