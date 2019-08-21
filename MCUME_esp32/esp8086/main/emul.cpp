#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
extern "C" {
#include "emuapi.h"
}
#include "emu.h"
#include "ps2.h"



uint8_t * RAM;
uint8_t * LORAM; //LORAM[NATIVE_RAM];






#define PALMULT8(x)  ((x)<<5)
#define RGBVAL16(r,g,b)  ( (((r>>3)&0x1f)<<11) | (((g>>2)&0x3f)<<5) | (((b>>3)&0x1f)<<0) )





#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"



struct i8253_s {
  uint16_t chandata[3];
  uint8_t accessmode[3];
  uint8_t bytetoggle[3];
  uint32_t effectivedata[3];
  float chanfreq[3];
  uint8_t active[3];
  uint16_t counter[3];
};

extern struct i8253_s i8253;

void doirq(uint8_t irqnum);
void write86(uint32_t addr32, uint8_t value);
uint8_t read86(uint32_t addr32);

uint8_t curkey = 0;



uint8_t VRAM[16384];


uint32_t updateaddrs[64];
uint8_t updatepos = 0;

void VRAM_write(uint32_t addr32, uint8_t value) {

  if (addr32 < 16384) {
    VRAM[addr32] = value;

//printf("%d %d\n",addr32,value);      
  }
}

uint8_t VRAM_read(uint32_t addr32) {

  if (addr32 < 16384) return VRAM[addr32];
  return 0;
}

uint8_t ansicolor[16] = { 0, 4, 2, 6, 1, 5, 3, 7, 0, 4, 2, 6, 1, 5, 3, 7 };
uint16_t palettecga[16], palettevga[256];


void installPalette(void) {
  palettecga[0] = RGBVAL16(0, 0, 0);
  palettecga[1] = RGBVAL16(0, 0, 0xAA);
  palettecga[2] = RGBVAL16(0, 0xAA, 0);
  palettecga[3] = RGBVAL16(0, 0xAA, 0xAA);
  palettecga[4] = RGBVAL16(0xAA, 0, 0);
  palettecga[5] = RGBVAL16(0xAA, 0, 0xAA);
  palettecga[6] = RGBVAL16(0xAA, 0x55, 0);
  palettecga[7] = RGBVAL16(0xAA, 0xAA, 0xAA);
  palettecga[8] = RGBVAL16(0x55, 0x55, 0x55);
  palettecga[9] = RGBVAL16(0x55, 0x55, 0xFF);
  palettecga[10] = RGBVAL16(0x55, 0xFF, 0x55);
  palettecga[11] = RGBVAL16(0x55, 0xFF, 0xFF);
  palettecga[12] = RGBVAL16(0xFF, 0x55, 0x55);
  palettecga[13] = RGBVAL16(0xFF, 0x55, 0xFF);
  palettecga[14] = RGBVAL16(0xFF, 0xFF, 0x55);
  palettecga[15] = RGBVAL16(0xFF, 0xFF, 0xFF);
  palettevga[0] = RGBVAL16(0, 0, 0);
  palettevga[1] = RGBVAL16(0, 0, 169);
  palettevga[2] = RGBVAL16(0, 169, 0);
  palettevga[3] = RGBVAL16(0, 169, 169);
  palettevga[4] = RGBVAL16(169, 0, 0);
  palettevga[5] = RGBVAL16(169, 0, 169);
  palettevga[6] = RGBVAL16(169, 169, 0);
  palettevga[7] = RGBVAL16(169, 169, 169);
  palettevga[8] = RGBVAL16(0, 0, 84);
  palettevga[9] = RGBVAL16(0, 0, 255);
  palettevga[10] = RGBVAL16(0, 169, 84);
  palettevga[11] = RGBVAL16(0, 169, 255);
  palettevga[12] = RGBVAL16(169, 0, 84);
  palettevga[13] = RGBVAL16(169, 0, 255);
  palettevga[14] = RGBVAL16(169, 169, 84);
  palettevga[15] = RGBVAL16(169, 169, 255);
  palettevga[16] = RGBVAL16(0, 84, 0);
  palettevga[17] = RGBVAL16(0, 84, 169);
  palettevga[18] = RGBVAL16(0, 255, 0);
  palettevga[19] = RGBVAL16(0, 255, 169);
  palettevga[20] = RGBVAL16(169, 84, 0);
  palettevga[21] = RGBVAL16(169, 84, 169);
  palettevga[22] = RGBVAL16(169, 255, 0);
  palettevga[23] = RGBVAL16(169, 255, 169);
  palettevga[24] = RGBVAL16(0, 84, 84);
  palettevga[25] = RGBVAL16(0, 84, 255);
  palettevga[26] = RGBVAL16(0, 255, 84);
  palettevga[27] = RGBVAL16(0, 255, 255);
  palettevga[28] = RGBVAL16(169, 84, 84);
  palettevga[29] = RGBVAL16(169, 84, 255);
  palettevga[30] = RGBVAL16(169, 255, 84);
  palettevga[31] = RGBVAL16(169, 255, 255);
  palettevga[32] = RGBVAL16(84, 0, 0);
  palettevga[33] = RGBVAL16(84, 0, 169);
  palettevga[34] = RGBVAL16(84, 169, 0);
  palettevga[35] = RGBVAL16(84, 169, 169);
  palettevga[36] = RGBVAL16(255, 0, 0);
  palettevga[37] = RGBVAL16(255, 0, 169);
  palettevga[38] = RGBVAL16(255, 169, 0);
  palettevga[39] = RGBVAL16(255, 169, 169);
  palettevga[40] = RGBVAL16(84, 0, 84);
  palettevga[41] = RGBVAL16(84, 0, 255);
  palettevga[42] = RGBVAL16(84, 169, 84);
  palettevga[43] = RGBVAL16(84, 169, 255);
  palettevga[44] = RGBVAL16(255, 0, 84);
  palettevga[45] = RGBVAL16(255, 0, 255);
  palettevga[46] = RGBVAL16(255, 169, 84);
  palettevga[47] = RGBVAL16(255, 169, 255);
  palettevga[48] = RGBVAL16(84, 84, 0);
  palettevga[49] = RGBVAL16(84, 84, 169);
  palettevga[50] = RGBVAL16(84, 255, 0);
  palettevga[51] = RGBVAL16(84, 255, 169);
  palettevga[52] = RGBVAL16(255, 84, 0);
  palettevga[53] = RGBVAL16(255, 84, 169);
  palettevga[54] = RGBVAL16(255, 255, 0);
  palettevga[55] = RGBVAL16(255, 255, 169);
  palettevga[56] = RGBVAL16(84, 84, 84);
  palettevga[57] = RGBVAL16(84, 84, 255);
  palettevga[58] = RGBVAL16(84, 255, 84);
  palettevga[59] = RGBVAL16(84, 255, 255);
  palettevga[60] = RGBVAL16(255, 84, 84);
  palettevga[61] = RGBVAL16(255, 84, 255);
  palettevga[62] = RGBVAL16(255, 255, 84);
  palettevga[63] = RGBVAL16(255, 255, 255);
  palettevga[64] = RGBVAL16(255, 125, 125);
  palettevga[65] = RGBVAL16(255, 157, 125);
  palettevga[66] = RGBVAL16(255, 190, 125);
  palettevga[67] = RGBVAL16(255, 222, 125);
  palettevga[68] = RGBVAL16(255, 255, 125);
  palettevga[69] = RGBVAL16(222, 255, 125);
  palettevga[70] = RGBVAL16(190, 255, 125);
  palettevga[71] = RGBVAL16(157, 255, 125);
  palettevga[72] = RGBVAL16(125, 255, 125);
  palettevga[73] = RGBVAL16(125, 255, 157);
  palettevga[74] = RGBVAL16(125, 255, 190);
  palettevga[75] = RGBVAL16(125, 255, 222);
  palettevga[76] = RGBVAL16(125, 255, 255);
  palettevga[77] = RGBVAL16(125, 222, 255);
  palettevga[78] = RGBVAL16(125, 190, 255);
  palettevga[79] = RGBVAL16(125, 157, 255);
  palettevga[80] = RGBVAL16(182, 182, 255);
  palettevga[81] = RGBVAL16(198, 182, 255);
  palettevga[82] = RGBVAL16(218, 182, 255);
  palettevga[83] = RGBVAL16(234, 182, 255);
  palettevga[84] = RGBVAL16(255, 182, 255);
  palettevga[85] = RGBVAL16(255, 182, 234);
  palettevga[86] = RGBVAL16(255, 182, 218);
  palettevga[87] = RGBVAL16(255, 182, 198);
  palettevga[88] = RGBVAL16(255, 182, 182);
  palettevga[89] = RGBVAL16(255, 198, 182);
  palettevga[90] = RGBVAL16(255, 218, 182);
  palettevga[91] = RGBVAL16(255, 234, 182);
  palettevga[92] = RGBVAL16(255, 255, 182);
  palettevga[93] = RGBVAL16(234, 255, 182);
  palettevga[94] = RGBVAL16(218, 255, 182);
  palettevga[95] = RGBVAL16(198, 255, 182);
  palettevga[96] = RGBVAL16(182, 255, 182);
  palettevga[97] = RGBVAL16(182, 255, 198);
  palettevga[98] = RGBVAL16(182, 255, 218);
  palettevga[99] = RGBVAL16(182, 255, 234);
  palettevga[100] = RGBVAL16(182, 255, 255);
  palettevga[101] = RGBVAL16(182, 234, 255);
  palettevga[102] = RGBVAL16(182, 218, 255);
  palettevga[103] = RGBVAL16(182, 198, 255);
  palettevga[104] = RGBVAL16(0, 0, 113);
  palettevga[105] = RGBVAL16(28, 0, 113);
  palettevga[106] = RGBVAL16(56, 0, 113);
  palettevga[107] = RGBVAL16(84, 0, 113);
  palettevga[108] = RGBVAL16(113, 0, 113);
  palettevga[109] = RGBVAL16(113, 0, 84);
  palettevga[110] = RGBVAL16(113, 0, 56);
  palettevga[111] = RGBVAL16(113, 0, 28);
  palettevga[112] = RGBVAL16(113, 0, 0);
  palettevga[113] = RGBVAL16(113, 28, 0);
  palettevga[114] = RGBVAL16(113, 56, 0);
  palettevga[115] = RGBVAL16(113, 84, 0);
  palettevga[116] = RGBVAL16(113, 113, 0);
  palettevga[117] = RGBVAL16(84, 113, 0);
  palettevga[118] = RGBVAL16(56, 113, 0);
  palettevga[119] = RGBVAL16(28, 113, 0);
  palettevga[120] = RGBVAL16(0, 113, 0);
  palettevga[121] = RGBVAL16(0, 113, 28);
  palettevga[122] = RGBVAL16(0, 113, 56);
  palettevga[123] = RGBVAL16(0, 113, 84);
  palettevga[124] = RGBVAL16(0, 113, 113);
  palettevga[125] = RGBVAL16(0, 84, 113);
  palettevga[126] = RGBVAL16(0, 56, 113);
  palettevga[127] = RGBVAL16(0, 28, 113);
  palettevga[128] = RGBVAL16(56, 56, 113);
  palettevga[129] = RGBVAL16(68, 56, 113);
  palettevga[130] = RGBVAL16(84, 56, 113);
  palettevga[131] = RGBVAL16(97, 56, 113);
  palettevga[132] = RGBVAL16(113, 56, 113);
  palettevga[133] = RGBVAL16(113, 56, 97);
  palettevga[134] = RGBVAL16(113, 56, 84);
  palettevga[135] = RGBVAL16(113, 56, 68);
  palettevga[136] = RGBVAL16(113, 56, 56);
  palettevga[137] = RGBVAL16(113, 68, 56);
  palettevga[138] = RGBVAL16(113, 84, 56);
  palettevga[139] = RGBVAL16(113, 97, 56);
  palettevga[140] = RGBVAL16(113, 113, 56);
  palettevga[141] = RGBVAL16(97, 113, 56);
  palettevga[142] = RGBVAL16(84, 113, 56);
  palettevga[143] = RGBVAL16(68, 113, 56);
  palettevga[144] = RGBVAL16(56, 113, 56);
  palettevga[145] = RGBVAL16(56, 113, 68);
  palettevga[146] = RGBVAL16(56, 113, 84);
  palettevga[147] = RGBVAL16(56, 113, 97);
  palettevga[148] = RGBVAL16(56, 113, 113);
  palettevga[149] = RGBVAL16(56, 97, 113);
  palettevga[150] = RGBVAL16(56, 84, 113);
  palettevga[151] = RGBVAL16(56, 68, 113);
  palettevga[152] = RGBVAL16(80, 80, 113);
  palettevga[153] = RGBVAL16(89, 80, 113);
  palettevga[154] = RGBVAL16(97, 80, 113);
  palettevga[155] = RGBVAL16(105, 80, 113);
  palettevga[156] = RGBVAL16(113, 80, 113);
  palettevga[157] = RGBVAL16(113, 80, 105);
  palettevga[158] = RGBVAL16(113, 80, 97);
  palettevga[159] = RGBVAL16(113, 80, 89);
  palettevga[160] = RGBVAL16(113, 80, 80);
  palettevga[161] = RGBVAL16(113, 89, 80);
  palettevga[162] = RGBVAL16(113, 97, 80);
  palettevga[163] = RGBVAL16(113, 105, 80);
  palettevga[164] = RGBVAL16(113, 113, 80);
  palettevga[165] = RGBVAL16(105, 113, 80);
  palettevga[166] = RGBVAL16(97, 113, 80);
  palettevga[167] = RGBVAL16(89, 113, 80);
  palettevga[168] = RGBVAL16(80, 113, 80);
  palettevga[169] = RGBVAL16(80, 113, 89);
  palettevga[170] = RGBVAL16(80, 113, 97);
  palettevga[171] = RGBVAL16(80, 113, 105);
  palettevga[172] = RGBVAL16(80, 113, 113);
  palettevga[173] = RGBVAL16(80, 105, 113);
  palettevga[174] = RGBVAL16(80, 97, 113);
  palettevga[175] = RGBVAL16(80, 89, 113);
  palettevga[176] = RGBVAL16(0, 0, 64);
  palettevga[177] = RGBVAL16(16, 0, 64);
  palettevga[178] = RGBVAL16(32, 0, 64);
  palettevga[179] = RGBVAL16(48, 0, 64);
  palettevga[180] = RGBVAL16(64, 0, 64);
  palettevga[181] = RGBVAL16(64, 0, 48);
  palettevga[182] = RGBVAL16(64, 0, 32);
  palettevga[183] = RGBVAL16(64, 0, 16);
  palettevga[184] = RGBVAL16(64, 0, 0);
  palettevga[185] = RGBVAL16(64, 16, 0);
  palettevga[186] = RGBVAL16(64, 32, 0);
  palettevga[187] = RGBVAL16(64, 48, 0);
  palettevga[188] = RGBVAL16(64, 64, 0);
  palettevga[189] = RGBVAL16(48, 64, 0);
  palettevga[190] = RGBVAL16(32, 64, 0);
  palettevga[191] = RGBVAL16(16, 64, 0);
  palettevga[192] = RGBVAL16(0, 64, 0);
  palettevga[193] = RGBVAL16(0, 64, 16);
  palettevga[194] = RGBVAL16(0, 64, 32);
  palettevga[195] = RGBVAL16(0, 64, 48);
  palettevga[196] = RGBVAL16(0, 64, 64);
  palettevga[197] = RGBVAL16(0, 48, 64);
  palettevga[198] = RGBVAL16(0, 32, 64);
  palettevga[199] = RGBVAL16(0, 16, 64);
  palettevga[200] = RGBVAL16(32, 32, 64);
  palettevga[201] = RGBVAL16(40, 32, 64);
  palettevga[202] = RGBVAL16(48, 32, 64);
  palettevga[203] = RGBVAL16(56, 32, 64);
  palettevga[204] = RGBVAL16(64, 32, 64);
  palettevga[205] = RGBVAL16(64, 32, 56);
  palettevga[206] = RGBVAL16(64, 32, 48);
  palettevga[207] = RGBVAL16(64, 32, 40);
  palettevga[208] = RGBVAL16(64, 32, 32);
  palettevga[209] = RGBVAL16(64, 40, 32);
  palettevga[210] = RGBVAL16(64, 48, 32);
  palettevga[211] = RGBVAL16(64, 56, 32);
  palettevga[212] = RGBVAL16(64, 64, 32);
  palettevga[213] = RGBVAL16(56, 64, 32);
  palettevga[214] = RGBVAL16(48, 64, 32);
  palettevga[215] = RGBVAL16(40, 64, 32);
  palettevga[216] = RGBVAL16(32, 64, 32);
  palettevga[217] = RGBVAL16(32, 64, 40);
  palettevga[218] = RGBVAL16(32, 64, 48);
  palettevga[219] = RGBVAL16(32, 64, 56);
  palettevga[220] = RGBVAL16(32, 64, 64);
  palettevga[221] = RGBVAL16(32, 56, 64);
  palettevga[222] = RGBVAL16(32, 48, 64);
  palettevga[223] = RGBVAL16(32, 40, 64);
  palettevga[224] = RGBVAL16(44, 44, 64);
  palettevga[225] = RGBVAL16(48, 44, 64);
  palettevga[226] = RGBVAL16(52, 44, 64);
  palettevga[227] = RGBVAL16(60, 44, 64);
  palettevga[228] = RGBVAL16(64, 44, 64);
  palettevga[229] = RGBVAL16(64, 44, 60);
  palettevga[230] = RGBVAL16(64, 44, 52);
  palettevga[231] = RGBVAL16(64, 44, 48);
  palettevga[232] = RGBVAL16(64, 44, 44);
  palettevga[233] = RGBVAL16(64, 48, 44);
  palettevga[234] = RGBVAL16(64, 52, 44);
  palettevga[235] = RGBVAL16(64, 60, 44);
  palettevga[236] = RGBVAL16(64, 64, 44);
  palettevga[237] = RGBVAL16(60, 64, 44);
  palettevga[238] = RGBVAL16(52, 64, 44);
  palettevga[239] = RGBVAL16(48, 64, 44);
  palettevga[240] = RGBVAL16(44, 64, 44);
  palettevga[241] = RGBVAL16(44, 64, 48);
  palettevga[242] = RGBVAL16(44, 64, 52);
  palettevga[243] = RGBVAL16(44, 64, 60);
  palettevga[244] = RGBVAL16(44, 64, 64);
  palettevga[245] = RGBVAL16(44, 60, 64);
  palettevga[246] = RGBVAL16(44, 52, 64);
  palettevga[247] = RGBVAL16(44, 48, 64);
  palettevga[248] = RGBVAL16(0, 0, 0);
  palettevga[249] = RGBVAL16(0, 0, 0);
  palettevga[250] = RGBVAL16(0, 0, 0);
  palettevga[251] = RGBVAL16(0, 0, 0);
  palettevga[252] = RGBVAL16(0, 0, 0);
  palettevga[253] = RGBVAL16(0, 0, 0);
  palettevga[254] = RGBVAL16(0, 0, 0);
  palettevga[255] = RGBVAL16(0, 0, 0);
}

extern uint8_t vidmode, portram[0x400];



#include "font.h"


static unsigned short line[320];


void drawscreentext80(void) {
	uint16_t row, col, y, x, xpos;
	uint16_t fontdata;

	for (y=0; y<(25*8); y++) 
	{
		row = y>>3;
		uint8_t * vrampt=&VRAM[160*row];
		xpos = 0;
		for (col=0; col<80; col++) 
		{
			uint8_t bold, attrib, cc, bg, fg;
			cc = *vrampt++;
			attrib = *vrampt++;
			bg = (attrib >> 4) & 7;
			fg = attrib & 0x0F;
			//if (y == 0) {printf("0x%02X",cc);}
			fontdata = ROM_READ(font, ((uint32_t)cc << 3) + (y&0x7));
			for (x=0; x<4; x++) 
			{
				if (fontdata & 1) {
				  line[xpos++] = palettecga[fg];
				  //tft.drawPixel(realx++, realy, palettecga[fg]);
				} else {
				  line[xpos++] = palettecga[bg];
				  //tft.drawPixel(realx++, realy, palettecga[bg]);
				}
				fontdata >>= 2;
			}
		} 
		//if (y == 0) {printf("\n");}

		emu_DrawLine16(&line[0], 320, 200, y);       
	}
}

void drawscreenlorescga(void) {
  //uint16_t row;
	uint16_t y, x, xpos;
	uint8_t intensity, usepal;
	uint16_t color;
	usepal = (portram[0x3D9]>>5) & 1;
	intensity = ( (portram[0x3D9]>>4) & 1) << 3;

	for (y=0; y<(25*8); y++) 
	{
		//row = y>>3;
		//uint8_t * vrampt=&VRAM[160*row];
		xpos = 0;
    for (x=0; x<320; x++) 
    {
      int ychar = y;// >> 1;
      int xchar = x;// >> 1;
      uint8_t curchar = VRAM[((ychar & 1) * 8192 + (ychar >> 1) * 80 + (xchar >> 2))];
      switch (xchar & 3) {
        case 3: color = curchar & 3; break;
        case 2: color = (curchar >> 2) & 3; break;
        case 1: color = (curchar >> 4) & 3; break;
        case 0: color = (curchar >> 6) & 3; break;
      }
      color = (color << 1) + usepal + intensity;
      if (color == (usepal + intensity)) color = 0; 
      line[xpos++] = palettecga[color];     
    } 


    /*
		for (col=0; col<80; col++) 
		{
			uint8_t value = *vrampt++;
			for (i=0; i<4; i++) 
			{
				uint8_t packedcolor;
				packedcolor = value >> 6;
				if (!packedcolor) color = 0; else color = packedcolor * 2 + usepal + intensity;
				line[xpos++] = palettecga[color];
				value <<= 2;
			}    	
		} 
    */
		//if (y == 0) {printf("\n");}

		emu_DrawLine16(&line[0], 320, 200, y);       
	}
}


void drawscreenhirescga(void) {
  uint16_t y, x, xpos;
  uint16_t color;
  for (y=0; y<(25*8); y++) 
  {
    xpos = 0;
    for (x=0; x<320; x++) 
    {
      int ychar = y >> 1;
      int xchar = x;
      uint8_t curchar = VRAM[((ychar & 1) * 8192 + (ychar >> 1) * 80 + (xchar >> 3))];
      color = ((curchar >> (7-(x&7))) & 1) ? 15 : 0;
      line[xpos++] = palettecga[color];     
    } 

    emu_DrawLine16(&line[0], 320, 200, y);       
  }
}


void updatescreen() {
  switch (vidmode & 0x7F) {
    case 0:
    case 1:
      //drawtext40(origaddr, value);
      break;
    case 2:
    case 3:
    //case 7:
      drawscreentext80();
      break;
    case 4:
      drawscreenlorescga();
      break;
    case 5:
      drawscreenlorescga(); //change to BW function later
      break;
    case 6:
      drawscreenhirescga();
      break;
  }
//printf("%d\n",vidmode & 0x7F);      
}


void mac_Init(void)
{
  RAM = (uint8_t*) heap_caps_malloc(RAM_SIZE,MALLOC_CAP_SPIRAM); //malloc (MEMSIZE);
  if (!RAM)printf("SPI RAM malloc failed\n"); 

  LORAM = (uint8_t*)malloc(NATIVE_RAM);
  if (!LORAM)printf("LORAM malloc failed\n"); 


  installPalette();
  init8253();
  reset86();
  init8259();
}


static uint8_t nbkeys=0;
static uint8_t kcnt=0;
static int toggle=1;

//static char * seq="DIR\r";
static char * seq="CAT.EXE\r";
//static char * seq="PRINCE.BAT\r";


static int hk=0;
static int k=0;
static bool isMouse = true;
static int prev_key = 0; 
static int prev_j = 0; 

static void keyevent(int keysym, int isdown)
{
	uint8_t scancode = translatescancode(keysym);
	if (scancode != 0xFF) {
		portram[0x60] = scancode;
		if (!isdown) portram[0x60] |= 0x80;
		portram[0x64] |= 2;
		doirq(1);
		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
}

void mac_Input(int bClick) 
{
	hk = emu_ReadI2CKeyboard();
	k = emu_ReadKeys(); 

	if (nbkeys == 0) {
		if (bClick & MASK_JOY2_BTN) {
		  	nbkeys = strlen(seq);   
		  	kcnt=0;
		}  
	}
	else {
		char k = seq[kcnt];
		if (k == 13) keyevent(0xFF0D,toggle);
		else keyevent(k,toggle);
		//setKey(ascii2scan[k],toggle);
		if (!toggle) {
		  	kcnt++;
		  	nbkeys--;
		  	toggle = true;
		}
		else {
		  	toggle = false; 
		}
	}	   
}





void mac_Step(void)
{
	exec86(8000);
	updatescreen();

	//hk = emu_ReadI2CKeyboard();
	if ( (hk != 0) && (hk != prev_key) ) {
		printf("press %d\n", hk); 
		prev_key = hk;
		keyevent ( hk, 0 );
	} else{
		if (prev_key) {
		    printf("release %d\n", prev_key); 
		    keyevent ( prev_key, 1 );
		    prev_key = 0;
		} 
	} 


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
    //if (j != prev_j) IkbdJoystickChange(joynum,j);
    prev_j = j;

}




void mac_Start(char * filename)
{
    initDisk(filename);
}


