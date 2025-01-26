/** fMSX: portable MSX emulator ******************************/
/**                                                         **/
/**                          Disk.c                         **/
/**                                                         **/
/** This file contains standard disk access drivers working **/
/** with disk images from files.                            **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994-2003                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/
#include "MSX.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#ifdef __BORLANDC__
#include <io.h>
#endif

#ifdef UNIX
#include <unistd.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

static int Drives[2] = { -1,-1 };        /* Disk image files */
static int RdOnly[2];                    /* 1 = read-only    */ 

/** DiskPresent() ********************************************/
/** Return 1 if disk drive with a given ID is present.      **/
/*************************************************************/
byte DiskPresent(byte ID)
{
  return((ID<MAXDRIVES)&&(Drives[ID]>=0));
}

/** DiskRead() ***********************************************/
/** Read requested sector from the drive into a buffer.     **/
/*************************************************************/
byte DiskRead(byte ID,byte *Buf,int N)
{
#ifdef unused  
  if((ID<MAXDRIVES)&&(Drives[ID]>=0))
    if(lseek(Drives[ID],N*512L,0)==N*512L)
      return(read(Drives[ID],Buf,512)==512);
#endif    
  return(0);
}

/** DiskWrite() **********************************************/
/** Write contents of the buffer into a given sector of the **/
/** disk.                                                   **/
/*************************************************************/
byte DiskWrite(byte ID,byte *Buf,int N)
{
#ifdef unused  
  if((ID<MAXDRIVES)&&(Drives[ID]>=0)&&!RdOnly[ID])
    if(lseek(Drives[ID],N*512L,0)==N*512L)
      return(write(Drives[ID],Buf,512)==512);
#endif    
  return(0);
}

/** ChangeDisk() *********************************************/
/** Change disk image in a given drive. Closes current disk **/
/** image if Name=0 was given. Returns 1 on success or 0 on **/
/** failure.                                                **/
/*************************************************************/
byte ChangeDisk(byte ID,char *Name)
{
#ifdef unused  
  /* We only have MAXDRIVES drives */
  if(ID>=MAXDRIVES) return(0);
  /* Close previous disk image */
  if(Drives[ID]>=0) { close(Drives[ID]);Drives[ID]=-1; }
  /* If no disk image given, consider drive empty */
  if(!Name) return(1);
  /* Open new disk image */
  Drives[ID]=open(Name,O_RDWR|O_BINARY);
  RdOnly[ID]=0;
  /* If failed to open for writing, open read-only */
  if(Drives[ID]<0)
  {
    Drives[ID]=open(Name,O_RDONLY|O_BINARY);
    RdOnly[ID]=1;
  }
#endif  
  /* Return operation result */
  return(Drives[ID]>=0);
}
