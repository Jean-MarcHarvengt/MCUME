
/****************************************************************************
* Module name : oddemu
* Description : Library SMSEMU
* Author      : J-M Harvengt 
* History     : 14-07-97 Creation
****************************************************************************/
#include "vmachine.h"
#include "vdc.h"
#include "oddemu.h"

#include "emuapi.h"

/****************************************************************************
* Local macros / typedefs
****************************************************************************/
#define MAX_ROM_SIZE 8192 //16384

/****************************************************************************
* Local procedures definition
****************************************************************************/


/****************************************************************************
* Local data
****************************************************************************/
#define MAXC 1024
static char * bios=0;   //[MAXC];
static char * scshot=0; //[MAXC]; 
static char * lorom=0;  //[MAX_ROM_SIZE];


/****************************************************************************
* Global data
****************************************************************************/

/****************************************************************************
* Local procedures
****************************************************************************/

/****************************************************************************
* Exported procedures
****************************************************************************/
void odd_Init(void)
{
	if (bios == 0) bios = (char *)emu_Malloc(MAXC);
	if (scshot == 0) scshot = (char *)emu_Malloc(MAXC);
	if (lorom == 0) lorom = (char *)emu_Malloc(MAX_ROM_SIZE);
	
	app_data.debug = 0;
	app_data.stick[0] = 0;
	app_data.stick[1] = 1;
	app_data.bank = 0;
  app_data.limit = 1;
  app_data.sound_en = 1;
	app_data.speed = 100;
	app_data.wsize = 1;
	app_data.fullscreen = 0;
	app_data.scanlines = 0;
	app_data.voice = 1;
	app_data.window_title = "O2EM v";
	app_data.svolume = 200;
	app_data.vvolume = 100;
	app_data.filter = 0;
	app_data.exrom = 0;
	app_data.crc = 0;
	app_data.scshot = scshot;
	app_data.euro = 0;
	app_data.openb = 0;
}


static void load_bios( char *biosname)
{
	static char s[MAXC+10];
	unsigned long crc;

    emu_LoadFile(biosname,&rom1[0],1024);

	memcpy(&rom2[0],&rom1[0],1024);
	memcpy(&rom3[0],&rom1[0],1024);
	memcpy(&rom4[0],&rom1[0],1024);

	crc = crc32_buf(rom1,1024);

	if (crc==0x8016A315)
		emu_printf("Odyssey2 bios ROM loaded\n");
	else if (crc==0xE20A9F41)
		emu_printf("Videopac+ G7400 bios ROM loaded\n");
	else
		emu_printf("Bios ROM loaded (unknown version)\n");
}


static void load_cart(char *file)
{
	long l;

    l = emu_LoadFile(file,&lorom[0],MAX_ROM_SIZE);

	app_data.crc = crc32_buf(lorom, l); // crc32_file(file);

	if (app_data.crc == 0xAFB23F89) app_data.exrom = 1;  /* Musician */
	if (app_data.crc == 0x3BFEF56B) app_data.exrom = 1;  /* Four in 1 Row! */

	if (((app_data.crc == 0x975AB8DA) || (app_data.crc == 0xE246A812)) && (!app_data.debug)) {
		emu_printf("Error: file is an incomplete ROM dump\n");
	}

	if (l < 2049) {
		app_data.bank=1;
		memcpy(&rom1[1024],&lorom[0],l);
		memcpy(&rom1[3072],&rom1[2048],1024); /* simulate missing A10 */
		rom=rom1;
		emu_printf("2K");
	} else if (l == 3072) {
		app_data.bank=1;
		memcpy(&rom1[1024],&lorom[0],3072);
		rom=rom1;
		emu_printf("3K");
	} else if (l == 4096) {
		if (app_data.exrom) {
			app_data.bank=1;
		    memcpy(&extROM[0],&lorom[0],1024);
		    memcpy(&rom1[1024],&lorom[1024],3072);
			rom=rom1;
			emu_printf("3K EXROM");
		} else {
			app_data.bank=2;
		    memcpy(&rom1[1024],&lorom[0],2048);
		    memcpy(&rom2[1024],&lorom[2048],2048);
			memcpy(&rom1[3072],&rom1[2048],1024); /* simulate missing A10 */
			memcpy(&rom2[3072],&rom2[2048],1024); /* simulate missing A10 */
			rom=rom2;
			emu_printf("4K");
		}
	} else if (l == 6144) {
		app_data.bank=2;
	    memcpy(&rom1[1024],&lorom[0],3072);
	    memcpy(&rom2[1024],&lorom[3072],3072);
		rom=rom2;
		emu_printf("6K");
	} else if (l == 8192) {
		app_data.bank=3;
	    memcpy(&rom1[1024],&lorom[0],2048);
	    memcpy(&rom2[1024],&lorom[2048],2048);
	    memcpy(&rom3[1024],&lorom[2*2048],2048);
	    memcpy(&rom4[1024],&lorom[3*2048],2048);
		memcpy(&rom1[3072],&rom1[2048],1024); /* simulate missing A10 */
		memcpy(&rom2[3072],&rom2[2048],1024); /* simulate missing A10 */
		memcpy(&rom3[3072],&rom3[2048],1024); /* simulate missing A10 */
		memcpy(&rom4[3072],&rom4[2048],1024); /* simulate missing A10 */
		rom=rom4;
		emu_printf("8K");
	} else if (l == 12288) {
		app_data.bank=3;
	    memcpy(&rom1[1024],&lorom[0],3072);
	    memcpy(&rom2[1024],&lorom[3072],3072);
	    memcpy(&rom3[1024],&lorom[2*3072],3072);
	    memcpy(&rom4[1024],&lorom[3*3072],3072);
		rom=rom4;
		emu_printf("12K");
	} else {
		emu_printf("Invalid ROM size");
	}
	if ((rom1[1024+12]=='O') && (rom1[1024+13]=='P') && (rom1[1024+14]=='N') && (rom1[1024+15]=='B')) app_data.openb=1;
	orom=rom;
	//printf("CRC: %08lX\n",app_data.crc);
}

void odd_Start(char * filename)
{
	load_bios("o2rom.bin");
	load_cart(filename);
	init_display();
	init_cpu();
	init_system();
}


//static void snd_Mixer(short *  stream, int len )
//{
//  audio_process(stream,  len>>2);
//}

void odd_Stop(void)
{
	close_audio();
	close_display();
}




void odd_Step(void)
{
  run();

//emu_printf("s");  
	emu_DrawScreen((unsigned char *)getGBuf()+BORDERW/2, BMPW-BORDERW, BMPH, BMPW);
  emu_DrawVsync();
}






