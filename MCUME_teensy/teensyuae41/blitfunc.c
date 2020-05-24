#include "shared.h"

#include "custom.h"
#include "blitfunc.h"

void blitdofast_0(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
for (j=0;j<b->vblitsize;j++) {
	for (i=0;i<b->hblitsize;i++) {
		b->bltddat=0;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_0(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
for (j=0;j<b->vblitsize;j++) {
	for (i=0;i<b->hblitsize;i++) {
		b->bltddat=0;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_a(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptc) b->bltcdat=*ptc++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srca);
	b->blitpreva=bltaold;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptc) b->bltcdat=*ptc++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca);
		b->blitpreva=b->bltadat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptc) b->bltcdat=*ptc++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca);
		b->blitpreva=bltaold;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptc) ptc+=bltcmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_a(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptc) b->bltcdat=*ptc--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srca);
	b->blitpreva=bltaold;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptc) b->bltcdat=*ptc--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca);
		b->blitpreva=b->bltadat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptc) b->bltcdat=*ptc--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca);
		b->blitpreva=bltaold;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptc) ptc-=bltcmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_2a(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptb) b->bltbdat=*ptb++;
	if (ptc) b->bltcdat=*ptc++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srca) | (srcc & ~srcb);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (srcc & ~srcb);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (srcc & ~srcb);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptb) ptb+=bltbmodw;
	if (ptc) ptc+=bltcmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_2a(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptb) b->bltbdat=*ptb--;
	if (ptc) b->bltcdat=*ptc--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srca) | (srcc & ~srcb);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (srcc & ~srcb);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (srcc & ~srcb);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptb) ptb-=bltbmodw;
	if (ptc) ptc-=bltcmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_30(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptb) b->bltbdat=*ptb++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
	b->bltddat=(~srcb & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		b->bltddat=(~srcb & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		b->bltddat=(~srcb & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptb) ptb+=bltbmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_30(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptb) b->bltbdat=*ptb--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
	b->bltddat=(~srcb & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		b->bltddat=(~srcb & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		b->bltddat=(~srcb & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptb) ptb-=bltbmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_3a(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptb) b->bltbdat=*ptb++;
	if (ptc) b->bltcdat=*ptc++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srca) | (~srcb & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (~srcb & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (~srcb & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptb) ptb+=bltbmodw;
	if (ptc) ptc+=bltcmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_3a(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptb) b->bltbdat=*ptb--;
	if (ptc) b->bltcdat=*ptc--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srca) | (~srcb & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (~srcb & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (~srcb & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptb) ptb-=bltbmodw;
	if (ptc) ptc-=bltcmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_3c(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptb) b->bltbdat=*ptb++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
	b->bltddat=(srcb & ~srca) | (~srcb & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		b->bltddat=(srcb & ~srca) | (~srcb & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		b->bltddat=(srcb & ~srca) | (~srcb & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptb) ptb+=bltbmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_3c(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptb) b->bltbdat=*ptb--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
	b->bltddat=(srcb & ~srca) | (~srcb & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		b->bltddat=(srcb & ~srca) | (~srcb & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		b->bltddat=(srcb & ~srca) | (~srcb & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptb) ptb-=bltbmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_4a(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptb) b->bltbdat=*ptb++;
	if (ptc) b->bltcdat=*ptc++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srca) | (~srcc & srcb & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (~srcc & srcb & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (~srcc & srcb & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptb) ptb+=bltbmodw;
	if (ptc) ptc+=bltcmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_4a(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptb) b->bltbdat=*ptb--;
	if (ptc) b->bltcdat=*ptc--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srca) | (~srcc & srcb & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (~srcc & srcb & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (~srcc & srcb & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptb) ptb-=bltbmodw;
	if (ptc) ptc-=bltcmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_6a(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptb) b->bltbdat=*ptb++;
	if (ptc) b->bltcdat=*ptc++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srca) | (srcc & ~srcb) | (~srcc & srcb & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (srcc & ~srcb) | (~srcc & srcb & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (srcc & ~srcb) | (~srcc & srcb & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptb) ptb+=bltbmodw;
	if (ptc) ptc+=bltcmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_6a(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptb) b->bltbdat=*ptb--;
	if (ptc) b->bltcdat=*ptc--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srca) | (srcc & ~srcb) | (~srcc & srcb & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (srcc & ~srcb) | (~srcc & srcb & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (srcc & ~srcb) | (~srcc & srcb & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptb) ptb-=bltbmodw;
	if (ptc) ptc-=bltcmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_8a(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptb) b->bltbdat=*ptb++;
	if (ptc) b->bltcdat=*ptc++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srca) | (srcc & srcb);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (srcc & srcb);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (srcc & srcb);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptb) ptb+=bltbmodw;
	if (ptc) ptc+=bltcmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_8a(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptb) b->bltbdat=*ptb--;
	if (ptc) b->bltcdat=*ptc--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srca) | (srcc & srcb);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (srcc & srcb);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (srcc & srcb);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptb) ptb-=bltbmodw;
	if (ptc) ptc-=bltcmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_8c(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptb) b->bltbdat=*ptb++;
	if (ptc) b->bltcdat=*ptc++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
	srcc=b->bltcdat;
	b->bltddat=(srcb & ~srca) | (srcc & srcb);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcb & ~srca) | (srcc & srcb);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcb & ~srca) | (srcc & srcb);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptb) ptb+=bltbmodw;
	if (ptc) ptc+=bltcmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_8c(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptb) b->bltbdat=*ptb--;
	if (ptc) b->bltcdat=*ptc--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
	srcc=b->bltcdat;
	b->bltddat=(srcb & ~srca) | (srcc & srcb);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcb & ~srca) | (srcc & srcb);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcb & ~srca) | (srcc & srcb);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptb) ptb-=bltbmodw;
	if (ptc) ptc-=bltcmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_9a(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptb) b->bltbdat=*ptb++;
	if (ptc) b->bltcdat=*ptc++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srca) | (~srcc & ~srcb & srca) | (srcc & srcb);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (~srcc & ~srcb & srca) | (srcc & srcb);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (~srcc & ~srcb & srca) | (srcc & srcb);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptb) ptb+=bltbmodw;
	if (ptc) ptc+=bltcmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_9a(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptb) b->bltbdat=*ptb--;
	if (ptc) b->bltcdat=*ptc--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srca) | (~srcc & ~srcb & srca) | (srcc & srcb);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (~srcc & ~srcb & srca) | (srcc & srcb);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (~srcc & ~srcb & srca) | (srcc & srcb);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptb) ptb-=bltbmodw;
	if (ptc) ptc-=bltcmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_a8(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptb) b->bltbdat=*ptb++;
	if (ptc) b->bltcdat=*ptc++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
	srcc=b->bltcdat;
	b->bltddat=(srcc & srcb) | (srcc & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & srcb) | (srcc & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & srcb) | (srcc & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptb) ptb+=bltbmodw;
	if (ptc) ptc+=bltcmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_a8(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptb) b->bltbdat=*ptb--;
	if (ptc) b->bltcdat=*ptc--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
	srcc=b->bltcdat;
	b->bltddat=(srcc & srcb) | (srcc & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & srcb) | (srcc & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & srcb) | (srcc & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptb) ptb-=bltbmodw;
	if (ptc) ptc-=bltcmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_aa(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
for (j=0;j<b->vblitsize;j++) {
	for (i=0;i<b->hblitsize;i++) {
		if (ptc) b->bltcdat=*ptc++;
		srcc=b->bltcdat;
		b->bltddat=(srcc);
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (ptc) ptc+=bltcmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_aa(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
for (j=0;j<b->vblitsize;j++) {
	for (i=0;i<b->hblitsize;i++) {
		if (ptc) b->bltcdat=*ptc--;
		srcc=b->bltcdat;
		b->bltddat=(srcc);
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (ptc) ptc-=bltcmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_b1(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptb) b->bltbdat=*ptb++;
	if (ptc) b->bltcdat=*ptc++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
	srcc=b->bltcdat;
	b->bltddat=(~srcc & ~srcb) | (~srcb & srca) | (srcc & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(~srcc & ~srcb) | (~srcb & srca) | (srcc & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(~srcc & ~srcb) | (~srcb & srca) | (srcc & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptb) ptb+=bltbmodw;
	if (ptc) ptc+=bltcmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_b1(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptb) b->bltbdat=*ptb--;
	if (ptc) b->bltcdat=*ptc--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
	srcc=b->bltcdat;
	b->bltddat=(~srcc & ~srcb) | (~srcb & srca) | (srcc & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(~srcc & ~srcb) | (~srcb & srca) | (srcc & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(~srcc & ~srcb) | (~srcb & srca) | (srcc & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptb) ptb-=bltbmodw;
	if (ptc) ptc-=bltcmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_ca(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptb) b->bltbdat=*ptb++;
	if (ptc) b->bltcdat=*ptc++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srca) | (srcb & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (srcb & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (srcb & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptb) ptb+=bltbmodw;
	if (ptc) ptc+=bltcmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_ca(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptb) b->bltbdat=*ptb--;
	if (ptc) b->bltcdat=*ptc--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srca) | (srcb & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (srcb & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srca) | (srcb & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptb) ptb-=bltbmodw;
	if (ptc) ptc-=bltcmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_cc(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
for (j=0;j<b->vblitsize;j++) {
	for (i=0;i<b->hblitsize;i++) {
		if (ptb) b->bltbdat=*ptb++;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		b->bltddat=(srcb);
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (ptb) ptb+=bltbmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_cc(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
for (j=0;j<b->vblitsize;j++) {
	for (i=0;i<b->hblitsize;i++) {
		if (ptb) b->bltbdat=*ptb--;
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		b->bltddat=(srcb);
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (ptb) ptb-=bltbmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_d8(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptb) b->bltbdat=*ptb++;
	if (ptc) b->bltcdat=*ptc++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
	srcc=b->bltcdat;
	b->bltddat=(srcc & srcb) | (~srcc & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & srcb) | (~srcc & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & srcb) | (~srcc & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptb) ptb+=bltbmodw;
	if (ptc) ptc+=bltcmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_d8(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptb) b->bltbdat=*ptb--;
	if (ptc) b->bltcdat=*ptc--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
	srcc=b->bltcdat;
	b->bltddat=(srcc & srcb) | (~srcc & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & srcb) | (~srcc & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & srcb) | (~srcc & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptb) ptb-=bltbmodw;
	if (ptc) ptc-=bltcmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_e2(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptb) b->bltbdat=*ptb++;
	if (ptc) b->bltcdat=*ptc++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srcb) | (srcb & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srcb) | (srcb & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srcb) | (srcb & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptb) ptb+=bltbmodw;
	if (ptc) ptc+=bltcmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_e2(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptb) b->bltbdat=*ptb--;
	if (ptc) b->bltcdat=*ptc--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
	srcc=b->bltcdat;
	b->bltddat=(srcc & ~srcb) | (srcb & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srcb) | (srcb & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc & ~srcb) | (srcb & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptb) ptb-=bltbmodw;
	if (ptc) ptc-=bltcmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_ea(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptb) b->bltbdat=*ptb++;
	if (ptc) b->bltcdat=*ptc++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
	srcc=b->bltcdat;
	b->bltddat=(srcc) | (srcb & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc) | (srcb & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		if (ptc) b->bltcdat=*ptc++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		srcc=b->bltcdat;
		b->bltddat=(srcc) | (srcb & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptb) ptb+=bltbmodw;
	if (ptc) ptc+=bltcmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_ea(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptb) b->bltbdat=*ptb--;
	if (ptc) b->bltcdat=*ptc--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
	srcc=b->bltcdat;
	b->bltddat=(srcc) | (srcb & srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc) | (srcb & srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		if (ptc) b->bltcdat=*ptc--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		srcc=b->bltcdat;
		b->bltddat=(srcc) | (srcb & srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptb) ptb-=bltbmodw;
	if (ptc) ptc-=bltcmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_f0(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	b->bltddat=(srca);
	b->blitpreva=bltaold;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		b->bltddat=(srca);
		b->blitpreva=b->bltadat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		b->bltddat=(srca);
		b->blitpreva=bltaold;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_f0(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	b->bltddat=(srca);
	b->blitpreva=bltaold;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		b->bltddat=(srca);
		b->blitpreva=b->bltadat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		b->bltddat=(srca);
		b->blitpreva=bltaold;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_fa(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptc) b->bltcdat=*ptc++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcc=b->bltcdat;
	b->bltddat=(srcc) | (srca);
	b->blitpreva=bltaold;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptc) b->bltcdat=*ptc++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcc=b->bltcdat;
		b->bltddat=(srcc) | (srca);
		b->blitpreva=b->bltadat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptc) b->bltcdat=*ptc++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcc=b->bltcdat;
		b->bltddat=(srcc) | (srca);
		b->blitpreva=bltaold;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptc) ptc+=bltcmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_fa(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptc) b->bltcdat=*ptc--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcc=b->bltcdat;
	b->bltddat=(srcc) | (srca);
	b->blitpreva=bltaold;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptc) b->bltcdat=*ptc--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcc=b->bltcdat;
		b->bltddat=(srcc) | (srca);
		b->blitpreva=b->bltadat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptc) b->bltcdat=*ptc--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcc=b->bltcdat;
		b->bltddat=(srcc) | (srca);
		b->blitpreva=bltaold;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptc) ptc-=bltcmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
void blitdofast_fc(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta++;
	if (ptb) b->bltbdat=*ptb++;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
	srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
	b->bltddat=(srcb) | (srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd++=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		srca=(((ULONG)b->blitpreva<<16)|b->bltadat)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		b->bltddat=(srcb) | (srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta++;
		if (ptb) b->bltbdat=*ptb++;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)b->blitpreva<<16)|bltaold)>>b->blitashift;
		srcb=(((ULONG)b->blitprevb<<16)|b->bltbdat)>>b->blitbshift;
		b->bltddat=(srcb) | (srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd++=b->bltddat;
	}
	if (pta) pta+=bltamodw;
	if (ptb) ptb+=bltbmodw;
	if (ptd) ptd+=bltdmodw;
}
}
}
void blitdofast_desc_fc(UWORD *pta, UWORD *ptb, UWORD *ptc, UWORD *ptd, struct bltinfo *b)
{
WORD bltamodw=b->bltamod/2,bltbmodw=b->bltbmod/2,bltcmodw=b->bltcmod/2,bltdmodw=b->bltdmod/2;
int i,j;
{
UWORD srca,srcb,srcc,bltaold,bltamask=b->bltafwm;
if (b->hblitsize==1) bltamask&=b->bltalwm;
for (j=0;j<b->vblitsize;j++) {
	if (pta) b->bltadat=*pta--;
	if (ptb) b->bltbdat=*ptb--;
	bltaold=b->bltadat & bltamask;
	srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
	srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
	b->bltddat=(srcb) | (srca);
	b->blitpreva=bltaold;
	b->blitprevb=b->bltbdat;
	if (b->bltddat) b->blitzero=0;
	if (ptd) *ptd--=b->bltddat;
	for (i=2;i<b->hblitsize;i++) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		srca=(((ULONG)b->bltadat<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		b->bltddat=(srcb) | (srca);
		b->blitpreva=b->bltadat;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (b->hblitsize>1) {
		if (pta) b->bltadat=*pta--;
		if (ptb) b->bltbdat=*ptb--;
		bltaold=b->bltadat & b->bltalwm;
		srca=(((ULONG)bltaold<<16)|b->blitpreva)>>(16-b->blitashift);
		srcb=(((ULONG)b->bltbdat<<16)|b->blitprevb)>>(16-b->blitbshift);
		b->bltddat=(srcb) | (srca);
		b->blitpreva=bltaold;
		b->blitprevb=b->bltbdat;
		if (b->bltddat) b->blitzero=0;
		if (ptd) *ptd--=b->bltddat;
	}
	if (pta) pta-=bltamodw;
	if (ptb) ptb-=bltbmodw;
	if (ptd) ptd-=bltdmodw;
}
}
}
