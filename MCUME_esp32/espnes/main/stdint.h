#define uint8_t unsigned char
#define int8_t char
#define uint16_t unsigned short
#define int16_t short
#define uint32_t unsigned long
#define int32_t long
#define uint64_t unsigned long long
#define int64_t long long

#ifndef __BOOL_DEFINED
	#define bool uint8_t
	#define true 1
	#define false 0
#endif
