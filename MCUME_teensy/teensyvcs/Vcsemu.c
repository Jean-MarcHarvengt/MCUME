#include "options.h"
#include "vcsemu.h"
#include "types.h"
#include "vmachine.h"

#include "emuapi.h"

/****************************************************************************
* Local macros / typedefs
****************************************************************************/

/****************************************************************************
* Global data
****************************************************************************/

/****************************************************************************
* Imported procedures
****************************************************************************/
extern void mainloop(void);

/****************************************************************************
* Local procedures
****************************************************************************/

/****************************************************************************
* Exported procedures
****************************************************************************/
void vcs_Init(void)
{
  init_machine();	
  init_hardware();
  tv_on();
}


void vcs_Start(char * filename)
{
  int size = emu_LoadFile(filename, (char *)theCart, 16384); 
  

  if (size > 16384)
    size = 16384;
	
  rom_size = size;
  if (size == 2048)
  {
    memcpy (&theCart[2048], &theCart[0], 2048);
    rom_size = 4096;
  }
  else if (size < 2048)
  {
    theCart[0x0ffc] = 0x00;
    theCart[0x0ffd] = 0xf0;
    rom_size = 4096;
  }
  
  init_hardware();
  init_banking(); 
}


void vcs_Step(void)
{
  //emu_printf("s");
  mainloop();
}






