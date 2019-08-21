#include <stdlib.h>
#include <stdio.h>
#include <string.h>
extern "C" {
#include "emuapi.h"
}
#include "config.h"
#include "st.h"
#include "mem.h"
#include "m68k_intrf.h"
#ifndef NO_SOUND  
#include "sound.h"
#endif
#include "tossw12.h"


int8 *membase;
int8 *rombase;


int hbl = 0;
int end_visible_screen = 264;
int hsync = 0;
int vsyncpend = 0, hsyncpend = 0;

int exmousex=160,exmousey=100,MouseRelease1=0,MouseRelease2=0;
int CompleteSndBufIdx; 


int waitstate=0;
int dcastaway_disc_writed[2] = { 0 , 0 };
int dcastaway_disc_for_write[2] = { 0 , 0 };
int draw_border=0, maybe_border=0;
unsigned cyclenext=512;
unsigned vid_adr_cycleyet=0;

#ifndef USE_FAME_CORE
unsigned IO_CYCLE=0;
#endif
static unsigned char vid_cycles_pal[1024];
static unsigned char vid_cycles_ntsc[1024];
unsigned char *vid_cycle=(unsigned char *)&vid_cycles_pal;

int readdsk=1;
extern unsigned char fdc_motor;

#ifndef NO_SOUND  
void SND_Process(void *stream, int len) {
  Sound_UpdateFromCallBack16((short *)stream, len);
} 
#endif


static unsigned short line[320];

#define PALMULT8(x)  ((x)<<5)
#define RGBVAL16(r,g,b)  ( (((r>>3)&0x1f)<<11) | (((g>>2)&0x3f)<<5) | (((b>>3)&0x1f)<<0) )

extern signed char videobuf[32768];

void Redraw16 ( int row, int vid_adr ) {

  static unsigned short palmap [ 16 ];
  //Source address
//  register unsigned short *line_i = (unsigned short *)(&membase[vid_adr]);
  register unsigned short *line_i = (unsigned short *)(&videobuf[row*160]);
  register unsigned short *line_o= &line[0];

  //Build paletter
  if (vid_flag) {
    unsigned char i, r, g, b;
    for (i = 0; i < 16; i++) {
      b = PALMULT8 ( (vid_col[i] & 0x7) );
      g = PALMULT8 ( ((vid_col[i] >> 4) & 0x7) );
      r = PALMULT8 ( ((vid_col[i] >> 8) & 0x7) );
      palmap [ i ] = RGBVAL16(r,g,b); 
    }
    vid_flag=0;
  }

  register int col;
  register int bit;
  for (col=0; col<20; col++) {
    register unsigned short pl0=*line_i++,pl1=*line_i++,pl2=*line_i++,pl3=*line_i++;
    for (bit=15;bit>=0;bit--) {
      int ind = (pl0 >> bit) & 0x1;
      ind += ((pl1 >> bit) & 0x1)<<1;
      ind += ((pl2 >> bit) & 0x1)<<2;
      ind += ((pl3 >> bit) & 0x1)<<3;
      *line_o++ = palmap [ ind ]; 
    }
  }

  emu_DrawLine16(&line[0], 320, 200, row);
}

void Redraw16_med ( int row, int vid_adr ) {
  static unsigned short palmap [ 4 ];
  //Source address
//  register unsigned short *line_i = (unsigned short *)(&membase[vid_adr]);
  register unsigned short *line_i = (unsigned short *)(&videobuf[row*160]);
  register unsigned short *line_o= &line[0];

  //Build paletter
  if (vid_flag) {
    unsigned char i, r, g, b;
    for (i = 0; i < 4; i++) {
      b = PALMULT8 ( (vid_col[i] & 0x7) );
      g = PALMULT8 ( ((vid_col[i] >> 4) & 0x7) );
      r = PALMULT8 ( ((vid_col[i] >> 8) & 0x7) );
      palmap [ i ] = RGBVAL16(r,g,b); 
    }
    //palmap [ 0 ] = RGBVAL16(0xff,0xff,0xff); 
    vid_flag=0;
  }

  register int col;
  register int bit;
  for (col=0; col<40; col++) {
    register unsigned short pl0=*line_i++,pl1=*line_i++;
    for (bit=15;bit>=0;bit--) {
      int ind = (pl0 >> bit) & 0x1;
      ind += ((pl1 >> bit) & 0x1)<<1;
      if (bit & 0x01)
        *line_o++ = palmap [ ind ]; 
    }
  }

  emu_DrawLine16(&line[0], 320, 200, row);
}


static int mouse_x = 160;
static int mouse_y = 100;
static int prev_key = 0; 
static int prev_j = 0; 

static int hk=0;
static int k=0;
static bool isMouse = true;
static int joynum = 1;

void mac_Input(int bClick) {
  //if (bClick & MASK_KEY_USER1) {
  //  printf("selected %d\n", isMouse);    
  //  if (isMouse) isMouse=false;
  //  else isMouse=true;
  //}  
  hk = emu_ReadI2CKeyboard();
  k = emu_ReadKeys();  
}

static void do_events(void) 
{
  //int hk = emu_ReadI2CKeyboard();
  if ( (hk != 0) && (hk != prev_key) ) {
    prev_key = hk;
    IkbdKeyPress ( hk );
    //printf("press %d\n", hk); 
  } else {
    if (prev_key) {
        //printf("release %d\n", prev_key); 
        IkbdKeyRelease ( prev_key | 0x80 );
        prev_key = 0;
    } 
  } 


  //int k = emu_ReadKeys(); 
  //if (k) printf("keys 0x%04X\n",k);
  if (isMouse)
  {
    if (( k & MASK_JOY1_RIGHT) || ( k & MASK_JOY2_RIGHT)) {
      if ( mouse_x < 320 ) {
        mouse_x += 1;
        IkbdMouseMotion ( mouse_x, mouse_y );
      } 
    }
    else if (( k & MASK_JOY1_LEFT) || ( k & MASK_JOY2_LEFT)) {
      if ( mouse_x > 1 ) {
        mouse_x -= 1;
        IkbdMouseMotion ( mouse_x, mouse_y );
      }
    }
    else if (( k & MASK_JOY1_UP) || ( k & MASK_JOY2_UP)) {
      if ( mouse_y > 1 ) {
        mouse_y -= 1;
        IkbdMouseMotion ( mouse_x, mouse_y );
      }
    }
    else if (( k & MASK_JOY1_DOWN) || ( k & MASK_JOY2_DOWN)) {
      if ( mouse_y < 200 ) {
        mouse_y += 1;
        IkbdMouseMotion ( mouse_x, mouse_y );
      }
    }  
    if ( /*( k & MASK_JOY1_BTN) ||*/ ( k & MASK_JOY2_BTN) ){
      IkbdMousePress(2);
    }
    else {
      IkbdMouseRelease(2); 
    }
  }
  else
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
      printf("fire\n"); 
      j |= 0x80;
    }
    if (j != prev_j) IkbdJoystickChange(joynum,j);
    prev_j = j;
  }   
  IkbdLoop();    
}      

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"




void mac_Init(void)
{

  membase = (int8*) heap_caps_malloc(MEMSIZE,MALLOC_CAP_SPIRAM);
  //membase = (int8*) malloc(MEMSIZE); 
  if (!membase)printf("malloc failed\n"); 
  rombase=(int8 *)&tos[0]-ROMBASE;   
  (void) memcpy (membase, &tos[0], 8);

 
  //extern unsigned char *msabuff;
  extern unsigned char *disc[2];
  //msabuff = (uint8*) heap_caps_malloc(1050L*1024L,MALLOC_CAP_SPIRAM);
  //disc [ 0 ] = (uint8*) heap_caps_malloc ( 1050L*1024L,MALLOC_CAP_SPIRAM );
  //disc [ 1 ] = (uint8*) heap_caps_malloc ( 10L*1024L,MALLOC_CAP_SPIRAM );
  disc [ 0 ] = (uint8*) malloc ( 256 );
  disc [ 1 ] = (uint8*) malloc ( 256 );


//#define DISKA  "/sdcard/st/med_009.st" //"800ATARI.st"     /* Disk A pathname */
//#define DISKB  "/sdcard/st/A_004.MSA"//"boot" //"A_000.MSA" //"disk.st"     /* Disk B pathname */
}

void mac_Start(char * filename)
{

  strncpy (disk[0].name, filename, sizeof(disk[0].name));

  printf("Init loading disk %s\n",disk[0].name);
  //MemClean();
  initialize_memmap();
  //MemInit();
  FDCInit(0);
  //FDCeject(0);
  FDCeject(1);
  IkbdReset(); 
  IOInit();
#ifndef NO_SOUND  
  Sound_Init(); 
#endif   
  HWReset(); /* CPU Reset */

  printf("Init done\n");    
}


void mac_Step(void)
{
  int delay_fdc_motor=0;
  unsigned long cycleco=0;
  unsigned long oldpend,newpend;
  hsync=0;
  hbl=0;
  vsyncpend=0;
  hsyncpend=0;


  int running = 1;
 /* Event loop */
  while (running) 
  {


#ifndef USE_FAME_CORE
      recalc_int = 0;
#endif      
      cycleco=cpu_loop(cyclenext);
      cycleco+=waitstate;
      waitstate=0;
#ifndef NO_SOUND
      SoundCycles+=cycleco;
#endif            
      //MFP timer A delay mode
      if (mfp_ascale>1) {
          mfp_acount-=mfp_ascale*cycleco;
          if (mfp_acount<=0) {
            do {mfp_acount+=mfp_tadr;} while (mfp_acount<=0);
            oldpend=mfp_ipra; newpend=(oldpend|0x20)&mfp_iera;
            if (newpend!=oldpend) {mfp_ipra=newpend;
  #ifndef USE_FAME_CORE
                recalc_int=1;
  #endif
            }
          }
  #ifdef USE_SHORT_SLICE
          cyclenext=4+(mfp_acount/mfp_ascale);
  #endif
      }
  #ifdef USE_SHORT_SLICE
      else
          cyclenext=512;
  #endif
      //MFP timer B delay mode
      if (mfp_bscale>1) {
          mfp_bcount-=mfp_bscale*cycleco;
          if (mfp_bcount<=0) {
            do {mfp_bcount+=mfp_tbdr;} while (mfp_bcount<=0);
              oldpend=mfp_ipra; newpend=(oldpend|0x1)&mfp_iera;
              if (newpend!=oldpend) {mfp_ipra=newpend;
  #ifndef USE_FAME_CORE
                  recalc_int=1;
  #endif
              }
          }
  #ifdef USE_SHORT_SLICE
          {
              int n=4+(mfp_bcount/mfp_bscale);
              if (n<cyclenext)
                  cyclenext=n;
          }
  #endif
      }
      //MFP timer C delay mode
      if (mfp_cscale>1) {
          mfp_ccount-=mfp_cscale*cycleco;
          if (mfp_ccount<=0) {
            do {mfp_ccount+=mfp_tcdr;} while (mfp_ccount<=0);
              oldpend=mfp_iprb; newpend=(oldpend|0x20)&mfp_ierb;
              if (newpend!=oldpend) {mfp_iprb=newpend;
  #ifndef USE_FAME_CORE
                  recalc_int=1;
  #endif
              }
          }
  #ifdef USE_SHORT_SLICE
          {
              int n=4+(mfp_ccount/mfp_cscale);
              if (n<cyclenext)
                  cyclenext=n;
          }
  #endif
      }
      //MFP timer D delay mode
      if (mfp_dscale>1) {
          mfp_dcount-=mfp_dscale*cycleco;
          if (mfp_dcount<=0) {
            do {mfp_dcount+=mfp_tddr;} while (mfp_dcount<=0);
              oldpend=mfp_iprb; newpend=(oldpend|0x10)&mfp_ierb;
              if (newpend!=oldpend) {mfp_iprb=newpend;
  #ifndef USE_FAME_CORE
                  recalc_int=1;
  #endif
              }
          }
  #ifdef USE_SHORT_SLICE
          {
              int n=4+(mfp_dcount/mfp_dscale);
              if (n<cyclenext)
                  cyclenext=n;
          }
  #endif
      }
    
      
      vid_adr+=(cycleco)&(~3);

      //Count hbls
      hsync-=cycleco;

    if (hsync<=0) {
      hbl++; hsync+=512;
      //Generate hbl interrupt
  #ifndef USE_FAME_CORE
      if (hsyncpend==0) {hsyncpend=1; recalc_int=1;}
  #else
      Interrupt(AUTOINT2, 2);
  #endif
      

      //Trigger event counters for visible scan-lines
      if (hbl<64) {
        vid_adr=(vid_baseh<<16)+(vid_basem<<8);
      }
      else if (hbl<264) {
        if ( (hbl == 132 )) {
//        if ( (hbl % (end_visible_screen/2) ) == 0) {
            //CPUEvent();
            //timer_tick = 1; 
          }
        //Do IO every 64 hbls
        if (!(hbl&63)) {
          //Generate FDC interrupt in mfp?
          if (!(mfp_gpip & 0x20)) {
            mfp_iprb |= 0x80;
            mfp_iprb &= mfp_ierb;
#ifndef USE_FAME_CORE
            recalc_int=1;
#endif            
          } 
          //Generate ACIA interrupt in mfp?
          IkbdWriteBuffer();
          if (!(mfp_gpip & 0x10)) {
            mfp_iprb |= 0x40;
            mfp_iprb &= mfp_ierb;
#ifndef USE_FAME_CORE
            recalc_int=1;
#endif            
          }
        }


        if (emu_FrameSkip() == 0 ) {
          //Update video address
          vid_adr=(vid_baseh<<16)+(vid_basem<<8)+(hbl-64)*160;
        
          //Draw screen line
          if (vid_shiftmode==COL2) {
            Redraw16_med ( hbl - 64, vid_adr );
          } else {
            Redraw16 ( hbl - 64, vid_adr );
          }          
        }

        //Timer-A event count mode
        if (mfp_ascale==1) {
          mfp_acount-=1048576;
          if (mfp_acount<=0) {
            mfp_acount=mfp_tadr;
            oldpend=mfp_ipra; newpend=(oldpend|0x20)&mfp_iera;
            if (newpend!=oldpend) {mfp_ipra=newpend;
  #ifndef USE_FAME_CORE
              recalc_int=1;
  #endif
            }
          }
        }
        //Timer-B event count mode
        if (mfp_bscale==1) {
          mfp_bcount-=1048576;
          if (mfp_bcount<=0) {
            mfp_bcount=mfp_tbdr;
            oldpend=mfp_ipra; newpend=(oldpend|0x1)&mfp_iera;
            if (newpend!=oldpend) {mfp_ipra=newpend;
  #ifndef USE_FAME_CORE
              recalc_int=1;
  #endif
            }
          }
        }
      }


      //Vertical blank?
      else if (hbl>=313)
      {
//  printf("hbl%d\n",hbl);  
        emu_DrawVsync();
    
        do_events();
        Sound_Update_VBL();
        running=0;  
#ifdef HAS_SDL
        SDL_Delay(20);
#endif

             
        hbl=0;
        //Generate vsync interrupt
#ifndef USE_FAME_CORE
        vsyncpend=1; recalc_int=1;
#else
        Interrupt(AUTOINT4, 4);
#endif  
        //Do fdc spinup
        if (fdc_motor){
          if (delay_fdc_motor>150) {
            fdc_status &= ~0x80;
            delay_fdc_motor=0;
            fdc_motor=0;
          }
          else delay_fdc_motor++;
        }
      }
    }


    //Recalculate interrupts?
#ifndef USE_FAME_CORE
    if (recalc_int==1)
#endif
    {
      int mfp_int;
      mfp_int=0;
      if (6>GetI()) {
        //Mfp interrupt
        {
          int n, number;
          uint16 imr, ipr, isr, irr;
          int in_request;
          //Find in_request and in_service
          imr = (mfp_imra<<8)+mfp_imrb;
          ipr = (mfp_ipra<<8)+mfp_iprb;
          irr = imr & ipr;
          isr = (mfp_isra<<8) + mfp_isrb;
          //in_request higher than in_service?
          if (irr>isr) {
            //Find highest set bit
            for (in_request = 15; in_request > 0; in_request--) {
              if (irr & 0x8000) break;
              irr <<= 1;
            }
            isr = 1 << in_request;
            //Set interrupt in service bits in MFP
            if (mfp_ivr & 0x8) {
              mfp_isra |= isr >> 8;
              mfp_isrb |= isr;
            }else{
              mfp_isra &= (~isr) >> 8;
              mfp_isrb &= ~isr;
            }
            //Clear interrupt pending bits in MFP
            mfp_ipra &= ~(isr >> 8);
            mfp_iprb &= ~isr;
            //Pass interrupt to cpu
            number = in_request | (mfp_ivr & 0xf0);
            Interrupt(number, 6);
            mfp_int=1;
          }
        }
      }
#ifndef USE_FAME_CORE
      if (!mfp_int){
        if (vsyncpend==1 && 4>GetI()) {
          Interrupt(AUTOINT4, 4);
          vsyncpend=0;
        }else if (hsyncpend==1 && 2>GetI()) {
          Interrupt(AUTOINT2, 2);
          hsyncpend=0;
        }        
      }
      
      if (!vsyncpend&&!hsyncpend) recalc_int=0;
#endif      
    }

  }

}






