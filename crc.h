#include <stdio.h>
typedef unsigned char        u8;
typedef unsigned short       u16;
typedef unsigned int         u32;
typedef unsigned long long   u64;

typedef struct
{
	u16 poly;//多项式
	u16 InitValue;//初始值
	u16 xor;//结果异或值
	unsigned char InputReverse;
	unsigned char OutputReverse;
}CRC_16;

#define FALSE                0
#define TRUE                 1

const CRC_16 crc_16_MODBUS;
u16 crc16(u8 *addr, int num,CRC_16 type) ;