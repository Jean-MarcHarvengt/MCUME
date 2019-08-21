extern "C" {
#include "emuapi.h"
}


#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include <string.h>

#include "reSID/reSID.h"
#include "LibFC14/fc14audiodecoder.h"
#include "StSnd/StSoundLibrary.h"


enum MusType {
  Undefined,
  C64Dmp,
  StYm,
  AmFc
};  

static MusType mustype = Undefined; 
static bool playing = false;
static int mussize=0;
static void * musbuffer=nullptr;

char buffer[26];
char oldbuffer[26];

static AudioPlaySID playSID;
static void *fcDecoder = nullptr;  
static YMMUSIC *ymDecoder;


void snd_Init(void)
{
  switch (mustype) {
    case C64Dmp:
      playing = true;
      break;
    case AmFc:
      if (musbuffer) {
        bool haveModule = false;  
        fcDecoder = fc14dec_new();
        haveModule = fc14dec_init(fcDecoder,(void*)musbuffer,mussize);
        if ( !haveModule ) {
          fc14dec_delete(fcDecoder);
          printf("FC module not supported\n");
        }
        else {
          printf("FC music loaded\n");
          fc14dec_mixer_init(fcDecoder,22050,16,1,0 /*32767*/);
          playing = true;
        }
      }
      break;
    case StYm:
      if (musbuffer) {
        ymDecoder = ymMusicCreate();
        printf("YM music loader\n");    
        if (ymMusicLoadMemory(ymDecoder,(void*)musbuffer,mussize))
        { 
          printf("YM music loaded\n");
          ymMusicInfo_t info;
          ymMusicGetInfo(ymDecoder,&info);
          printf("Name.....: %s\n",info.pSongName);
          printf("Author...: %s\n",info.pSongAuthor);
          printf("Comment..: %s\n",info.pSongComment);
          printf("Duration.: %d:%02d\n",info.musicTimeInSec/60,info.musicTimeInSec%60);
          printf("Driver...: %s\n", info.pSongPlayer);
          ymMusicSetLoopMode(ymDecoder,YMTRUE); 
          ymMusicPlay(ymDecoder);         
          playing = true;
        } 
      }
      break;
    default:
      break;
  }  
}

int oldSec = -1;
void snd_Step(void)
{
  vTaskDelay(20 / portTICK_PERIOD_MS);
  switch (mustype) {
    case C64Dmp:
      if (emu_FileRead(buffer, 25) == 25 ) {
        for(int i=0;i<25;i++) {
          if(buffer[i] != oldbuffer[i]) {       
             playSID.setreg(i, buffer[i]);
            oldbuffer[i] = buffer[i];                  
          } 
        }
      } else {
        if (playSID.isPlaying()) {
          emu_FileClose();
          playSID.stop();
        } 
      }
      break;
    case AmFc:
      break;
    case StYm:
      //int sec = ymMusicGetPos(ymDecoder) / 1000;
      //if (sec != oldSec)
     // {
     //   printf("Time: %d:%02d\r",sec/60,sec%60);
     //   oldSec = sec;
     // }  
      break;
    default:
      break;
  }    
}

void snd_Start(char * filename)
{
  mussize = emu_FileSize(filename);
  char *dot = strrchr(filename, '.');
  if (dot && !strcmp(dot, ".dmp")) {
    mustype = C64Dmp; 
    if (emu_FileOpen(filename) == 0) {
      printf("SID music failure, cannot open\n");        
    }   
  }
  else if (dot && !strcmp(dot, ".fc")) {
    mustype = AmFc; 
    musbuffer = emu_Malloc(mussize);
    if (musbuffer)
    {
      if (emu_FileOpen(filename)) {
        if (emu_FileRead((char*)musbuffer, mussize) != mussize ) {
          musbuffer = nullptr;
        }
        emu_FileClose();
      } else {
        musbuffer = nullptr;
      }  
    }
    if (musbuffer == nullptr) {
      printf("FC music failure, cannot open/allocate\n");
    }           
  }
  else if (dot && !strcmp(dot, ".ym")) {
    mustype = StYm; 
  }
}

void  SND_Process( void * stream, int len )
{
  if (playing) {
    switch (mustype) {
      case C64Dmp:
        playSID.update(stream, len);
        break;
      case AmFc:
        fc14dec_buffer_fill(fcDecoder,stream,len*2);  
        break;
      case StYm:
        ymMusicCompute((void*)ymDecoder,(ymsample*)stream,len);
       break;
      default:
        break;
    }  
  }
}


