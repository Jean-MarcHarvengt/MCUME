
//#define ALL_IN_RAM  1
//#define PSRAM_FAKE  1


#ifdef ALL_IN_RAM

#define MEMSIZE  0x00040000L /* default memsize 256 Kb split RAM */
#define RAM1BASE 0x00000000L
#define RAM1SIZE 0x00020000L
#define RAM2BASE RAM1SIZE
extern uint8    *mem1base;

#else

#ifdef PSRAM_FAKE
#define MEMSIZE  0x00040000L /* default memsize 256 Kb split RAM */
#define RAM1BASE 0x00000000L
#define RAM1SIZE 0x00020000L
#define RAM2BASE RAM1SIZE
extern uint8    *mem1base;
#else
//#define MEMSIZE  0x00080000L /* default memsize 512 Kb split PSRAM+RAM */
#define MEMSIZE  0x00100000L /* default memsize 1024 Kb split PSRAM+RAM */
#define RAM1BASE 0x00000000L
//#define RAM1SIZE 0x00030000L
#define RAM1SIZE 0x000B0000L
#define RAM2BASE RAM1SIZE
#endif

#endif
extern uint8    *mem2base;
extern uint8	*rombase;

/*
 * Read/Write memory macros - little endian
 */
#ifdef ALL_IN_RAM

#define ReadB(address) *(uint8*)((uint32)(address+mem1base)^1)
#define WriteB(address,value) *(uint8*)((uint32)(address+mem1base)^1)=value
#define ReadW(addr) *(uint16*)(addr+mem1base)
#define WriteW(addr,value) *(uint16*)(addr+mem1base)=value 
#define ReadL(address) ((*(uint16*)(address+mem1base))<<16)|(*(uint16*)(address+mem1base+2))
#define WriteL(address,value) WriteW(address + 2, value); WriteW(address, value>> 16)
#define ReadSL(addr) (*(uint16*)(addr+mem1base))|((*(uint16*)(addr+mem1base+2))<<16)

#else

extern unsigned char ram_readb(int address);
extern void ram_writeb(int address, unsigned char val);
extern unsigned short ram_readw(int address);
extern void ram_writew(int address, unsigned short val);

#define ReadBB(address) (address<RAM1SIZE?ram_readb(address):*(address-RAM1SIZE+mem2base))
#define WriteBB(address,value) (address<RAM1SIZE?ram_writeb(address,value):(void)(*(address-RAM1SIZE+mem2base)=value)) 
#define ReadB(address) (address<RAM1SIZE?ram_readb(address^1):*(uint8*)((uint32)(address-RAM1SIZE+mem2base)^1))
#define WriteB(address,value) (address<RAM1SIZE?ram_writeb(address^1,value):(void)(*(uint8*)((uint32)(address-RAM1SIZE+mem2base)^1)=value)) 
#define ReadW(address) (address<RAM1SIZE?ram_readw(address):*(uint16*)(address-RAM1SIZE+mem2base))
#define WriteW(address,value) (address<RAM1SIZE?ram_writew(address,value):(void)(*(uint16*)(address-RAM1SIZE+mem2base)=value))  
#define ReadL(address) (address<RAM1SIZE?(ram_readw(address)<<16)|(ram_readw(address+2)):((*(uint16*)(address-RAM1SIZE+mem2base))<<16)|(*(uint16*)(address-RAM1SIZE+mem2base+2))) 
#define WriteL(address,value) WriteW(address + 2, value); WriteW(address, value>> 16)
#define ReadSL(address) (address<RAM1SIZE?(ram_readw(address))|((ram_readw(address+2))<<16):(*(uint16*)(address-RAM1SIZE+mem2base))|((*(uint16*)(address-RAM1SIZE+mem2base+2))<<16)) 

/*
#define ReadBB(address) (address<RAM1SIZE?*(mem1base+address):*(address-RAM1SIZE+mem2base))
#define WriteBB(address,value) (address<RAM1SIZE?*(mem1base+address)=value:*(address-RAM1SIZE+mem2base)=value) 
#define ReadB(address) (address<RAM1SIZE?*(uint8*)((uint32)(mem1base+address)^1):*(uint8*)((uint32)(address-RAM1SIZE+mem2base)^1))
#define WriteB(address,value) (address<RAM1SIZE?*(uint8*)((uint32)(mem1base+address)^1)=value:*(uint8*)((uint32)(address-RAM1SIZE+mem2base)^1)=value) 
#define ReadW(address) (address<RAM1SIZE?*(uint16*)(mem1base+address):*(uint16*)(address-RAM1SIZE+mem2base))
#define WriteW(address,value) (address<RAM1SIZE?*(uint16*)(mem1base+address)=value:*(uint16*)(address-RAM1SIZE+mem2base)=value)  
#define ReadL(address) (address<RAM1SIZE?((*(uint16*)(mem1base+address))<<16)|(*(uint16*)(mem1base+address+2)):((*(uint16*)(address-RAM1SIZE+mem2base))<<16)|(*(uint16*)(address-RAM1SIZE+mem2base+2))) 
#define WriteL(address,value) WriteW(address + 2, value); WriteW(address, value>> 16)
#define ReadSL(address) (address<RAM1SIZE?(*(uint16*)(mem1base+address))|((*(uint16*)(mem1base+address+2))<<16):(*(uint16*)(address-RAM1SIZE+mem2base))|((*(uint16*)(address-RAM1SIZE+mem2base+2))<<16)) 
*/
#endif


int MemInit(void);
void MemQuit(void);
void MemClean(void);
void MemReInit(void);

extern unsigned short int TosVersion; 
extern short TosCountry;
void TOS_FixRom(uint8 *TosAddress);
