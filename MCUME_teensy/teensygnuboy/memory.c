#include "shared.h"


//static uint8 lvram[0x10000];
//uint8 *  vram=&lvram[0];

//byte ***mpatpix;
//byte lpatpix[4096][8][8];

void mem_init(void) {
/*	
	mpatpix = &lpatpix[0];
	int i;
	for(i = 0; i < 4096; i++) {
		mpatpix[i] = &lpatpix[i];
		int j;
		for(j = 0; j < 8; j++) {
        	mpatpix[i][j] = &lpatpix[i][j];
			int k;
			for(k = 0; k < 8; k++) {
        		mpatpix[i][j][k] = &lpatpix[i][j][k];
				mpatpix[i][j][k]=0;
			}
		}	
    } 
 */  
}


int mem_test(void) {
	return 1;
}

void memcpy_rom(int dst, int src, int size) {
	while (size > 0) {   
		write_rom(dst++, read_rom(src++));
		size--;
	}
}
