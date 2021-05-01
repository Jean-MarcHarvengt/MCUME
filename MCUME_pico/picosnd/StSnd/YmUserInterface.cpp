/*-----------------------------------------------------------------------------

	ST-Sound ( YM files player library )

	ST-Sound library "C-like" interface wrapper

-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
* ST-Sound, ATARI-ST Music Emulator
* Copyright (c) 1995-1999 Arnaud Carre ( http://leonard.oxg.free.fr )
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*
-----------------------------------------------------------------------------*/


#include "YmMusic.h"
#include "StSoundLibrary.h"


// Static assert to check various type len

YMMUSIC	* ymMusicCreate()
{
	return (YMMUSIC*)(new CYmMusic);
}


ymbool ymMusicLoad(YMMUSIC *pMus, const char *fName)
{
	CYmMusic *pMusic = (CYmMusic*)pMus;
	return pMusic->load(fName);
}

ymbool ymMusicLoadMemory(YMMUSIC *pMus, void *pBlock, ymu32 size)
{
	CYmMusic *pMusic = (CYmMusic*)pMus;
	return pMusic->loadMemory(pBlock,size);
}

void ymMusicDestroy(YMMUSIC *pMus)
{
	CYmMusic *pMusic = (CYmMusic*)pMus;
	delete pMusic;
}

ymbool ymMusicCompute(YMMUSIC *_pMus, ymsample *pBuffer, int nbSample)
{
	CYmMusic *pMusic = (CYmMusic*)_pMus;
	return pMusic->update(pBuffer,nbSample);
}

void ymMusicSetLoopMode(YMMUSIC *_pMus, ymbool bLoop)
{
	CYmMusic *pMusic = (CYmMusic*)_pMus;
	pMusic->setLoopMode(bLoop);
}

const char * ymMusicGetLastError(YMMUSIC *_pMus)
{
	CYmMusic *pMusic = (CYmMusic*)_pMus;
	return pMusic->getLastError();
}

int ymMusicGetRegister(YMMUSIC *_pMus, ymint reg)
{
	CYmMusic *pMusic = (CYmMusic*)_pMus;
	return pMusic->readYmRegister(reg);
}

void ymMusicGetInfo(YMMUSIC *_pMus, ymMusicInfo_t *pInfo)
{
	CYmMusic *pMusic = (CYmMusic*)_pMus;
	pMusic->getMusicInfo(pInfo);
}

void ymMusicPlay(YMMUSIC *_pMus)
{
	CYmMusic *pMusic = (CYmMusic*)_pMus;
	pMusic->play();
}

void ymMusicPause(YMMUSIC *_pMus)
{
	CYmMusic *pMusic = (CYmMusic*)_pMus;
	pMusic->pause();
}

void ymMusicStop(YMMUSIC *_pMus)
{
	CYmMusic *pMusic = (CYmMusic*)_pMus;
	pMusic->stop();
}

ymbool ymMusicIsOver(YMMUSIC *_pMus)
{
	CYmMusic *pMusic = (CYmMusic*)_pMus;
	return (pMusic->getMusicOver());
}

ymbool ymMusicIsSeekable(YMMUSIC *_pMus)
{
	CYmMusic *pMusic = (CYmMusic*)_pMus;
	return pMusic->isSeekable() ? YMTRUE : YMFALSE;
}

ymu32 ymMusicGetPos(YMMUSIC *_pMus)
{
	CYmMusic *pMusic = (CYmMusic*)_pMus;
	return pMusic->getPos();
}

void ymMusicSeek(YMMUSIC *_pMus, ymu32 timeInMs)
{
	CYmMusic *pMusic = (CYmMusic*)_pMus;
	if (pMusic->isSeekable())
	{
		pMusic->setMusicTime(timeInMs);
	}
}

void ymMusicRestart(YMMUSIC *_pMus)
{
	CYmMusic *pMusic = (CYmMusic*)_pMus;
	pMusic->restart();
}

void ymMusicSetLowpassFiler(YMMUSIC *_pMus, ymbool bActive)
{
	CYmMusic *pMusic = (CYmMusic*)_pMus;
	pMusic->setLowpassFilter(bActive);
}

