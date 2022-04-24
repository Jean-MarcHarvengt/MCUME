/** EMULib Emulation Library *********************************/
/**                                                         **/
/**                          Sound.c                        **/
/**                                                         **/
/** This file file implements core part of the sound API    **/
/** and functions needed to log soundtrack into a MIDI      **/
/** file. See Sound.h for declarations.                     **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1996-2003                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/
#include "Sound.h"

#include <stdio.h>
#include <string.h>

#ifdef UNIX
#include <unistd.h>
#endif

typedef unsigned char byte;
typedef unsigned short word;

struct SndDriverStruct SndDriver =
{
  (void (*)(int,int))0,
  (void (*)(int,int))0,
  (void (*)(int,int))0,
  (void (*)(int,int,int))0,
  (void (*)(int,const signed char *,int,int))0
};

#ifdef unused
static const struct { byte Note;word Wheel; } Freqs[4096] =
{
#include "MIDIFreq.h"
};
#endif

static const int Programs[5] =
{
  80,  /* SND_MELODIC/SND_RECTANGLE */
  80,  /* SND_TRIANGLE */
  122, /* SND_NOISE */
  122, /* SND_PERIODIC */
  80   /* SND_WAVE */
};

#ifdef unused
static struct
{
  int Type;
  int Note;
  int Pitch;
  int Level;
} CH[MIDI_CHANNELS];

static const char *LogName  = 0;
static int  Logging   = MIDI_OFF;
static int  TickCount = 0;
static int  LastMsg   = -1;
static FILE *MIDIOut  = 0;

static void MIDISound(int Channel,int Freq,int Volume);
static void MIDISetSound(int Channel,int Type);
static void MIDIDrum(int Type,int Force);
static void MIDIMessage(byte D0,byte D1,byte D2);
static void NoteOn(byte Channel,byte Note,byte Level);
static void NoteOff(byte Channel);
static void WriteDelta(void);
static void WriteTempo(int Freq);

/** SHIFT() **************************************************/
/** Make MIDI channel#10 last, as it is normally used for   **/
/** percussion instruments only and doesn't sound nice.     **/
/*************************************************************/
#define SHIFT(Ch) (Ch==15? 9:Ch>8? Ch+1:Ch)
#endif

/** Sound() **************************************************/
/** Generate sound of given frequency (Hz) and volume       **/
/** (0..255) via given channel. Setting Freq=0 or Volume=0  **/
/** turns sound off.                                        **/
/*************************************************************/
void Sound(int Channel,int Freq,int Volume)
{
  if(Channel<0) return;
  Freq   = Freq<0? 0:Freq;
  Volume = Volume<0? 0:Volume>255? 255:Volume;

  /* Call sound driver if present */
  if(SndDriver.Sound) (*SndDriver.Sound)(Channel,Freq,Volume);

#ifdef unused
  /* Log sound to MIDI file */
  MIDISound(Channel,Freq,Volume);
#endif  
}

/** Drum() ***************************************************/
/** Hit a drum of given type with given force (0..255).     **/
/** MIDI drums can be used by ORing their numbers with      **/
/** SND_MIDI.                                               **/
/*************************************************************/
void Drum(int Type,int Force)
{
  Force = Force<0? 0:Force>255? 255:Force;

  if(SndDriver.Drum) (*SndDriver.Drum)(Type,Force);

#ifdef unused
  /* Log drum to MIDI file */
  MIDIDrum(Type,Force);
#endif  
}

/** SetSound() ***********************************************/
/** Set sound type at a given channel. MIDI instruments can **/
/** be set directly by ORing their numbers with SND_MIDI.   **/
/*************************************************************/
void SetSound(int Channel,int Type)
{
  if(Channel<0) return;

  if(SndDriver.SetSound) (*SndDriver.SetSound)(Channel,Type);

#ifdef unused
  /* Log instrument change to MIDI file */
  MIDISetSound(Channel,Type);
#endif  
}

/** SetChannels() ********************************************/
/** Set master volume (0..255) and switch channels on/off.  **/
/** Each channel N has corresponding bit 2^N in Switch. Set **/
/** or reset this bit to turn the channel on or off.        **/ 
/*************************************************************/
void SetChannels(int Volume,int Switch)
{
  Volume = Volume<0? 0:Volume>255? 255:Volume;

  if(SndDriver.SetChannels) (*SndDriver.SetChannels)(Volume,Switch);
}

/** SetWave() ************************************************/
/** Set waveform for a given channel. The channel will be   **/
/** marked with sound type SND_WAVE. Set Rate=0 if you want **/
/** waveform to be an instrument or set it to the waveform  **/
/** own playback rate.                                      **/
/*************************************************************/
void SetWave(int Channel,const signed char *Data,int Length,int Rate)
{
  if((Channel<0)||(Length<=0)) return;

  if(SndDriver.SetWave) (*SndDriver.SetWave)(Channel,Data,Length,Rate);

#ifdef unused
  /* Log instrument change to MIDI file */
  MIDISetSound(Channel,Rate? -1:SND_MELODIC);
#endif  
}

#ifdef unused
/** InitMIDI() ***********************************************/
/** Initialize soundtrack logging into MIDI file FileName.  **/
/** Repeated calls to InitMIDI() will close current MIDI    **/
/** file and continue logging into a new one.               **/ 
/*************************************************************/

void InitMIDI(const char *FileName)
{
  int WasLogging,J;

  /* Must pass a name! */
  if(!FileName) return;

  /* Memorize logging status */
  WasLogging=Logging;

  /* If MIDI logging in progress, close current file */
  if(MIDIOut) TrashMIDI();

  /* Clear instrument types */
  for(J=0;J<MIDI_CHANNELS;J++) CH[J].Type=-1;

  /* Set log file name and ticks/second parameter, no logging yet */
  //LogName   = FileName;
  Logging   = MIDI_OFF;
  LastMsg   = -1;
  TickCount = 0;
  MIDIOut   = 0;

  /* If was logging, restart */
  if(WasLogging) MIDILogging(MIDI_ON);
}

/** TrashMIDI() **********************************************/
/** Finish logging soundtrack and close the MIDI file.      **/
/*************************************************************/
void TrashMIDI(void)
{
  long Length;
  int J;

  /* If not logging, drop out */
  if(!MIDIOut) return;
  /* Turn sound off */
  for(J=0;J<MIDI_CHANNELS;J++) NoteOff(J);
  /* End of track */
  MIDIMessage(0xFF,0x2F,0x00);
  /* Put track length in file */
  fseek(MIDIOut,0,SEEK_END);
  Length=ftell(MIDIOut)-22;
  fseek(MIDIOut,18,SEEK_SET);
  fputc((Length>>24)&0xFF,MIDIOut);
  fputc((Length>>16)&0xFF,MIDIOut);
  fputc((Length>>8)&0xFF,MIDIOut);
  fputc(Length&0xFF,MIDIOut);
  /* Done logging */
  fclose(MIDIOut);
  Logging   = MIDI_OFF;
  LastMsg   = -1;
  TickCount = 0;
  MIDIOut   = 0;
}

/** MIDILogging() ********************************************/
/** Turn soundtrack logging on/off and return its current   **/
/** status. Possible values of Switch are MIDI_OFF (turn    **/
/** logging off), MIDI_ON (turn logging on), MIDI_TOGGLE    **/
/** (toggle logging), and MIDI_QUERY (just return current   **/
/** state of logging).                                      **/
/*************************************************************/
int MIDILogging(int Switch)
{
  static const char MThd[] = "MThd\0\0\0\006\0\0\0\1";
                           /* ID  DataLen   Fmt Trks */
  static const char MTrk[] = "MTrk\0\0\0\0";
                           /* ID  TrkLen   */
  int J,I;

  /* Toggle logging if requested */
  if(Switch==MIDI_TOGGLE) Switch=!Logging;

  if((Switch==MIDI_ON)||(Switch==MIDI_OFF))
    if(Switch^Logging)
    {
      /* When turning logging off, silence all channels */
      if(!Switch&&MIDIOut)
        for(J=0;J<MIDI_CHANNELS;J++) NoteOff(J);

      /* When turning logging on, open MIDI file */
      if(Switch&&!MIDIOut&&LogName)
      {
        /* No messages have been sent yet */
        LastMsg=-1;

        /* Clear all storage */
        for(J=0;J<MIDI_CHANNELS;J++)
          CH[J].Note=CH[J].Pitch=CH[J].Level=-1;

        /* Open new file and write out the header */
        MIDIOut=fopen(LogName,"wb");
        if(!MIDIOut) return(MIDI_OFF);
        if(fwrite(MThd,1,12,MIDIOut)!=12)
        { fclose(MIDIOut);MIDIOut=0;return(MIDI_OFF); }
        fputc((MIDI_DIVISIONS>>8)&0xFF,MIDIOut);
        fputc(MIDI_DIVISIONS&0xFF,MIDIOut);
        if(fwrite(MTrk,1,8,MIDIOut)!=8)
        { fclose(MIDIOut);MIDIOut=0;return(MIDI_OFF); }

        /* Write out the tempo */
        WriteTempo(MIDI_DIVISIONS);
      }

      /* Turn logging off on failure to open MIDIOut */
      if(!MIDIOut) Switch=MIDI_OFF;

      /* Assign new switch value */
      Logging=Switch;

      /* If switching logging on... */
      if(Switch)
      {
        /* Start logging without a pause */
        TickCount=0;

        /* Write instrument changes */
        for(J=0;J<MIDI_CHANNELS;J++)
          if((CH[J].Type>=0)&&(CH[J].Type&0x10000))
          {
            I=CH[J].Type&~0x10000;
            CH[J].Type=-1;
            MIDISetSound(J,I);
          }
      }
    }

  /* Return current logging status */
  return(Logging);
}

/** MIDITicks() **********************************************/
/** Log N 1ms MIDI ticks.                                   **/
/*************************************************************/
void MIDITicks(int N)
{
  if(Logging&&MIDIOut&&(N>0)) TickCount+=N;
}

/** MIDISound() **********************************************/
/** Set sound frequency (Hz) and volume (0..255) for a      **/
/** given channel.                                          **/
/*************************************************************/
void MIDISound(int Channel,int Freq,int Volume)
{
  int MIDIVolume,MIDINote,MIDIWheel;

  /* If logging off, file closed, or invalid channel, drop out */
  if(!Logging||!MIDIOut||(Channel>=MIDI_CHANNELS-1)||(Channel<0)) return;
  /* Frequency must be in range */
  if((Freq<MIDI_MINFREQ)||(Freq>MIDI_MAXFREQ)) Freq=0;
  /* Volume must be in range */
  if(Volume<0) Volume=0; else if(Volume>255) Volume=255;
  /* Instrument number must be valid */
  if(CH[Channel].Type<0) Freq=0;

  if(!Volume||!Freq) NoteOff(Channel);
  else
  {
    /* SND_TRIANGLE is twice quieter than SND_MELODIC */
    if(CH[Channel].Type==SND_TRIANGLE) Volume=(Volume+1)/2;
    /* Compute MIDI note parameters */
    MIDIVolume = (127*Volume+128)/255;
    MIDINote   = Freqs[Freq/3].Note;
    MIDIWheel  = Freqs[Freq/3].Wheel;

    /* Play new note */
    NoteOn(Channel,MIDINote,MIDIVolume);

    /* Change pitch */
    if(CH[Channel].Pitch!=MIDIWheel)
    {
      MIDIMessage(0xE0+SHIFT(Channel),MIDIWheel&0x7F,(MIDIWheel>>7)&0x7F);
      CH[Channel].Pitch=MIDIWheel;
    }
  }
}

/** MIDISetSound() *******************************************/
/** Set sound type for a given channel.                     **/
/*************************************************************/
void MIDISetSound(int Channel,int Type)
{
  /* Channel must be valid */
  if((Channel>=MIDI_CHANNELS-1)||(Channel<0)) return;

  /* If instrument changed... */
  if(CH[Channel].Type!=Type)
  {
    /* If logging off or file closed, drop out */
    if(!Logging||!MIDIOut) CH[Channel].Type=Type|0x10000;
    else
    {
      CH[Channel].Type=Type;
      if(Type<0) NoteOff(Channel);
      else
      {
        Type=Type&SND_MIDI? (Type&0x7F):Programs[Type%5];
        MIDIMessage(0xC0+SHIFT(Channel),Type,255);
      }
    }
  }
}

/** MIDIDrum() ***********************************************/
/** Hit a drum of a given type with given force.            **/
/*************************************************************/
void MIDIDrum(int Type,int Force)
{
  /* If logging off or invalid channel, drop out */
  if(!Logging||!MIDIOut) return;
  /* The only non-MIDI drum is a click ("Low Wood Block") */
  Type=Type&DRM_MIDI? (Type&0x7F):77;
  MIDIMessage(0x99,Type,(Force&0xFF)/2);
}

/** MIDIMessage() ********************************************/
/** Write out a MIDI message.                               **/
/*************************************************************/
void MIDIMessage(byte D0,byte D1,byte D2)
{
  /* Write number of ticks that passed */
  WriteDelta();

  /* Write out the command */
  if(D0!=LastMsg) { LastMsg=D0;fputc(D0,MIDIOut); }

  /* Write out the arguments */
  if(D1<128)
  {
    fputc(D1,MIDIOut);
    if(D2<128) fputc(D2,MIDIOut);
  }
}

/** NoteOn() *************************************************/
/** Turn on a note on a given channel.                      **/
/*************************************************************/
void NoteOn(byte Channel,byte Note,byte Level)
{
  Note  = Note>0x7F? 0x7F:Note;
  Level = Level>0x7F? 0x7F:Level;

  if((CH[Channel].Note!=Note)||(CH[Channel].Level!=Level))
  {
    if((CH[Channel].Note>=0)&&(CH[Channel].Note!=Note)) NoteOff(Channel);
    MIDIMessage(0x90+SHIFT(Channel),Note,Level);
    CH[Channel].Note=Note;
    CH[Channel].Level=Level;
  }
}

/** NoteOff() ************************************************/
/** Turn off a note on a given channel.                     **/
/*************************************************************/
void NoteOff(byte Channel)
{
  if(CH[Channel].Note>=0)
  {
    MIDIMessage(0x80+SHIFT(Channel),CH[Channel].Note,127);
    CH[Channel].Note=-1;
  }
}

/** WriteDelta() *********************************************/
/** Write number of ticks since the last MIDI command and   **/
/** reset the counter.                                      **/
/*************************************************************/
void WriteDelta(void)
{
  if(TickCount<128) fputc(TickCount,MIDIOut);
  else
  {
    if(TickCount<128*128)
    {
      fputc((TickCount>>7)|0x80,MIDIOut);
      fputc(TickCount&0x7F,MIDIOut);
    }
    else
    {
      fputc(((TickCount>>14)&0x7F)|0x80,MIDIOut);
      fputc(((TickCount>>7)&0x7F)|0x80,MIDIOut);
      fputc(TickCount&0x7F,MIDIOut);
    }
  }

  TickCount=0;
}

/** WriteTempo() *********************************************/
/** Write out soundtrack tempo (Hz).                        **/
/*************************************************************/
void WriteTempo(int Freq)
{
  int J;

  J=500000*MIDI_DIVISIONS*2/Freq;
  WriteDelta();
  fputc(0xFF,MIDIOut);
  fputc(0x51,MIDIOut);
  fputc(0x03,MIDIOut);
  fputc((J>>16)&0xFF,MIDIOut);
  fputc((J>>8)&0xFF,MIDIOut);
  fputc(J&0xFF,MIDIOut);
}
#endif
