/*
 * Read/Write memory macros - little endian
 */
#ifdef BYTES_SWAP
#define ReadB(address) *(uint8*)((uint32)(address)^1)
#define WriteB(address,value) *(uint8*)((uint32)(address)^1)=value 
#define ReadW(addr) *(uint16*)(addr) 
#define WriteW(addr,value) *(uint16*)(addr)=value 
#define ReadL(address) ((*(uint16*)(address))<<16)|(*(uint16*)(address+2)) 
#define WriteL(address,value) WriteW(address + 2, value); WriteW(address, value>> 16)
#define ReadSL(addr) (*(uint16*)(addr))|((*(uint16*)(addr+2))<<16)
#else
#define ReadB(address) *((int8 *) (address))
#define WriteB(address,value) *((int8 *) (address)) = (value)
//#define ReadW(addr) ((*(uint16 *)(addr) << 8) | (*(uint16 *)(addr) >> 8))
//#define WriteW(addr,value) *((int16 *)(addr)) = ((((uint16)(value)) << 8) | (((uint16)(value)) >> 8))
#define ReadW(addr) *(uint16*)(addr) 
#define WriteW(addr,value) *(uint16*)(addr)=value 
//#define ReadL(address) ((uint16) ReadW(address) << 16) | (uint16) ReadW((address) + 2)
//#define WriteL(address,value) WriteW((address) + 2, value); WriteW(address, (value) >> 16)
#define ReadL(addr) *(uint32*)(addr) 
#define WriteL(addr,value) *(uint32*)(addr)=value 
//#define ReadSL(addr) ((uint16)ReadW(addr))|((((uint16)ReadW(addr+2)))<<16)
#define ReadSL(addr) ((uint16)ReadW(addr+2))|((((uint16)ReadW(addr)))<<16)
#endif

int MemInit(void);
void MemQuit(void);
void MemClean(void);
void MemReInit(void);

extern unsigned short int TosVersion; 
extern short TosCountry;
void TOS_FixRom(uint8 *TosAddress);
