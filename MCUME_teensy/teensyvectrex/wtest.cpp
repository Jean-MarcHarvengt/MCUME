#include <string.h>
#include <Arduino.h>

#include "emuapi.h"
extern "C" {
#include "osint.h"
#include "e8910.h"
#include "vecx.h"
}

static int mouse_x = 160;
static int mouse_y = 100;
static int prev_key = 0; 
static int prev_j = 0; 
static int prev_mouseb = 0;
static bool isMouse = true;
static int joynum = 1;
static int hk = 0;
static int prev_hk = 0;
static int k = 0;

extern void vec_Input(int click) {
  hk = emu_ReadI2CKeyboard();
  k = emu_ReadKeys();   
}

static void do_events(void) 
{
  if (hk != prev_hk) {
    prev_hk == hk;
    if ( (hk != 0) && (hk != prev_key) ) {
      prev_key = hk;
      //IkbdKeyPress ( hk );
      if (hk == 68) {
        if (isMouse) isMouse = false;
        else isMouse = true;
      }
      //IkbdLoop(); 
      //Serial.print("press ");
      //Serial.println(hk);
    }     
  }
  if ( (hk == 0) && (prev_key) ) {
      //IkbdKeyRelease ( prev_key | 0x80 );
      //IkbdLoop(); 
      //Serial.print("release ");
      //Serial.println(hk);
      prev_key = 0;
  }           

  if (!isMouse)
  {
    int j = 0;     
    if (( k & MASK_JOY1_RIGHT) || ( k & MASK_JOY2_RIGHT)) {
      j |= 0x08;
    }
    if (( k & MASK_JOY1_LEFT) || ( k & MASK_JOY2_LEFT)) {
      j |= 0x04;
    }
    if (( k & MASK_JOY1_UP) || ( k & MASK_JOY2_UP)) {
      j |= 0x01;
    }
    if (( k & MASK_JOY1_DOWN) || ( k & MASK_JOY2_DOWN)) {
      j |= 0x02;
    }  
    if ( k & MASK_JOY2_BTN) {
      j |= 0x80;
    }
    if (j != prev_j) {
      //IkbdJoystickChange(joynum,j);
      prev_j = j;
    }
  }
  else {
    if (( k & MASK_JOY1_RIGHT) || ( k & MASK_JOY2_RIGHT)) {
      if ( mouse_x < 320 ) {
        mouse_x += 1;
        //Serial.print("r");
        //IkbdMouseMotion ( mouse_x, mouse_y );
        //IkbdLoop();
      } 
    }
    else if (( k & MASK_JOY1_LEFT) || ( k & MASK_JOY2_LEFT)) {
      if ( mouse_x > 1 ) {
        mouse_x -= 1;
        //Serial.print("l");
        //IkbdMouseMotion ( mouse_x, mouse_y );
        //IkbdLoop();        
      }
    }
    else if (( k & MASK_JOY1_UP) || ( k & MASK_JOY2_UP)) {
      if ( mouse_y > 1 ) {
        mouse_y -= 1;
        //Serial.print("u");
        //IkbdMouseMotion ( mouse_x, mouse_y );
        //IkbdLoop();        
      }
    }
    else if (( k & MASK_JOY1_DOWN) || ( k & MASK_JOY2_DOWN)) {
      if ( mouse_y < 200 ) {
        mouse_y += 1;
        //Serial.print("d");
        //IkbdMouseMotion ( mouse_x, mouse_y );
        //IkbdLoop();        
      }
    }  
    
    int mouseb=0;
    if ( ( k & MASK_JOY2_BTN) ){
      mouseb=1;
    }
    if ( (mouseb != prev_mouseb) ){
      //if (mouseb) IkbdMousePress(2);
      //else IkbdMouseRelease(2);
      //Serial.println("btoggle");
      //IkbdLoop();
      prev_mouseb = mouseb;  
    }    
  }
}     


long vhash[VECTOR_HASH];
extern long * vector_hash=&vhash[0];
extern vector_t * vectors_set;

//#define USE_BYTEFB 1

#ifdef USE_BYTEFB
#define vbufsize 256*256
unsigned char * vbuf;
#else
#define vbufsize 32*256
unsigned char vbuf[vbufsize];
static unsigned short line[256];
#endif



#define min(x,y) ((x<y)?x:y)
#define max(x,y) ((x<y)?y:x)
#define abs(x) ((x<0)?-x:x)


#ifdef USE_BYTEFB
#define myPixel(x,y,c) vbuf[(y)*256+(x)]=c;
#else
const unsigned char bitsmask[8] = {0x80,0x40,0x20,0x10,0x8,0x4,0x2,0x1};
#define myPixel(x,y,c) vbuf[(y)*32+((x)>>3)]|=bitsmask[(x)&7];
#endif


/*
static void dummyline(int x1,int y1, int x2, int y2, unsigned char c )
{
  vbuf[y1*256+x1]=c;
  vbuf[y2*256+x2]=c;
}
*/

static void sline(int x, int y, int x2, int y2, unsigned char c) {
  bool yLonger=false;
  int incrementVal, endVal;
  int shortLen=y2-y;
  int longLen=x2-x;

  if (shortLen == longLen) {
    myPixel(x,y,c);
    return;
  }   

  if (abs(shortLen)>abs(longLen)) {
    int swap=shortLen;
    shortLen=longLen;
    longLen=swap;
    yLonger=true;
  }
  endVal=longLen;
  if (longLen<0) {
    incrementVal=-1;
    longLen=-longLen;
  } else incrementVal=1;

  int decInc;
  if (longLen==0) decInc=0;
  else decInc = (shortLen << 16) / longLen;
  int j=0;
  if (yLonger) {  
    for (int i=0;i!=endVal;i+=incrementVal) {
      myPixel(x+(j >> 16),y+i,c); 
      j+=decInc;
    }
  } else {
    for (int i=0;i!=endVal;i+=incrementVal) {
      myPixel(x+i,y+(j >> 16),c);
      j+=decInc;
    }
  }
}


static void fline(int x, int y, int x2, int y2, unsigned char c) {
    bool yLonger=false;
  int shortLen=y2-y;
  int longLen=x2-x;

  if (shortLen == longLen) {
    myPixel(x,y,c);
    return;
  } 
  if (abs(shortLen)>abs(longLen)) {
    int swap=shortLen;
    shortLen=longLen;
    longLen=swap;       
    yLonger=true;;
  }
  int decInc;
  if (longLen==0) decInc=0;
  else decInc = (shortLen << 8) / longLen;

  if (yLonger) {
    if (longLen>0) {
      longLen+=y;
      for (int j=0x80+(x<<8);y<=longLen;++y) {
        myPixel(j >> 8,y,c);  
        j+=decInc;
      }
      return;
    }
    longLen+=y;
    for (int j=0x80+(x<<8);y>=longLen;--y) {
      myPixel(j >> 8,y,c);  
      j-=decInc;
    }
    return; 
  }

  if (longLen>0) {
    longLen+=x;
    for (int j=0x80+(y<<8);x<=longLen;++x) {
      myPixel(x,j >> 8,c);
      j+=decInc;
    }
    return;
  }
  longLen+=x;
  for (int j=0x80+(y<<8);x>=longLen;--x) {
    myPixel(x,j >> 8,c);
    j-=decInc;
  }

}



void osint_render(void){

  memset((void*)&vbuf[0],0,vbufsize);
  //int minx=255,maxx=0,miny=255,maxy=0;

  for(int v = 0; v < vector_draw_cnt; v++){
    /*
    minx=min(vectors_draw[v].x0/128, minx);
    minx=min(vectors_draw[v].x1/128, minx);
    maxx=max(vectors_draw[v].x0/128, maxx);
    maxx=max(vectors_draw[v].x1/128, maxx);
    miny=min(vectors_draw[v].y0/128, miny);
    miny=min(vectors_draw[v].y1/128, miny);
    maxy=max(vectors_draw[v].y0/128, maxy);
    maxy=max(vectors_draw[v].y1/128, maxy);
    printf("%d %d %d %d\n",minx,maxx,miny,maxy);  
*/
    unsigned char c = vectors_draw[v].color * 256 / VECTREX_COLORS;
    sline(  vectors_draw[v].x0,
        vectors_draw[v].y0,
        vectors_draw[v].x1,
        vectors_draw[v].y1, c);
  }

#ifdef USE_BYTEFB
  emu_DrawScreen(&vbuf[0], 256, 240, 256);
#else
  int ymul = 0;
  int yinc = (256*256)/240;
  for(int y = 0; y < 240; y++){
    int pix=32;
    unsigned char * vbufpt=&vbuf[32*(ymul>>8)];
    for(int x = 0; x < 32; x++){
      unsigned char b=*vbufpt++;
      if (b&0x80) line[pix++]=0xffff; 
      else line[pix++]=0x0000; 
      if (b&0x40) line[pix++]=0xffff; 
      else line[pix++]=0x0000; 
      if (b&0x20) line[pix++]=0xffff; 
      else line[pix++]=0x0000; 
      if (b&0x10) line[pix++]=0xffff; 
      else line[pix++]=0x0000; 
      if (b&0x8) line[pix++]=0xffff; 
      else line[pix++]=0x0000; 
      if (b&0x4) line[pix++]=0xffff; 
      else line[pix++]=0x0000; 
      if (b&0x2) line[pix++]=0xffff; 
      else line[pix++]=0x0000; 
      if (b&0x1) line[pix++]=0xffff; 
      else line[pix++]=0x0000; 
    } 
      emu_DrawLine16(&line[0], 256, 240, y);
      ymul += yinc;   
  }
#endif
}


void vec_Step(void)
{
  vecx_emu(1500*20);

  alg_jch0 = 0x80;
  alg_jch1 = 0x80;
  snd_regs[14] |= 0x0f;
    if (( k & MASK_JOY1_RIGHT) || ( k & MASK_JOY2_RIGHT)) {
      alg_jch0 = 0x00;
    }
    else if (( k & MASK_JOY1_LEFT) || ( k & MASK_JOY2_LEFT)) {
      alg_jch0 = 0xff;
    }
    else if (( k & MASK_JOY1_UP) || ( k & MASK_JOY2_UP)) {
      alg_jch1 = 0xff;
    }
    else if (( k & MASK_JOY1_DOWN) || ( k & MASK_JOY2_DOWN)) {
      alg_jch1 = 0x00;
    }  
    else if ( (k & MASK_JOY1_BTN) || ( k & MASK_JOY2_BTN) ) {
      snd_regs[14] &= ~0x01;
    }
    else if ( k & MASK_KEY_USER1) {
      snd_regs[14] &= ~0x02;
    } 
    else if ( k & MASK_KEY_USER2) {
      snd_regs[14] &= ~0x04;
    } 
}


void vec_Init(void)
{
  for(int i = 0; i < PALETTE_SIZE; i++){
    emu_SetPaletteEntry(i,i,i,i);
  }

  vectors_set = (vector_t*)emu_Malloc(2 * VECTOR_CNT*sizeof(vector_t));
  if (!vectors_set)emu_printf("VSET malloc failed"); 

  //vector_hash = (long*)emu_Malloc(VECTOR_HASH*sizeof(long)); 
  //if (!vector_hash)emu_printf("VHASH malloc failed"); 

#ifdef USE_BYTEFB
  vbuf = (unsigned char*) malloc(vbufsize);
  if (!vbuf)printf("Vbuf malloc failed\n"); 
#endif

  vecx_reset();
  e8910_init_sound();  
}

void vec_Start(char * filename)
{
  emu_printf("init started");

  memset(cart, 0, sizeof (cart));

  int romsize = emu_FileSize(filename); 
  if (emu_FileOpen(filename)) {
    if (emu_FileRead((char*)cart, romsize) != romsize ) {
      emu_printf("could not load rom");   
    }
    emu_FileClose();
  }  
  emu_printf("init done");
}






