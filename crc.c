#include "crc.h"

const CRC_16 crc_16_MODBUS = {0x8005,0xffff,0x0000,TRUE,TRUE};

u8 reverse8(u8 data)
{
    u8 i;
    u8 temp=0;
    for(i=0;i<8;i++)
        temp |= ((data>>i) & 0x01)<<(7-i);
    return temp;
}

u16 reverse16(u16 data)
{
    u8 i;
    u16 temp=0;
    for(i=0;i<16;i++)
        temp |= ((data>>i) & 0x0001)<<(15-i);
    return temp;
}

u16 crc16(u8 *addr, int num,CRC_16 type)  
{  
    u8 data;
    u16 crc = type.InitValue;					//初始值
    int i;  
    for (; num > 0; num--)               
    {  
        data = *addr++;
        if(type.InputReverse == TRUE)
            data = reverse8(data);				//字节反转
        crc = crc ^ (data<<8) ;					//与crc初始值高8位异或 
        for (i = 0; i < 8; i++)					//循环8位 
        {  
            if (crc & 0x8000)					//左移移出的位为1，左移后与多项式异或
                crc = (crc << 1) ^ type.poly;    
            else		                        //否则直接左移
                crc <<= 1;                  
        }
    }
    if(type.OutputReverse == TRUE)              //满足条件，反转
        crc = reverse16(crc);
    crc = crc^type.xor;	                        //最后返与结果异或值异或
    return(crc);                                //返回最终校验值
}
