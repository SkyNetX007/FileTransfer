#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include "crc.h"

#define PACKAGE_SIZE 128

int main(int argc, char **argv)
{
	printf("Program started.\n");
	//arg1: IP, arg2: fileName
	int cfd;
	int recbyte;
	int sin_size;
	char buffer[PACKAGE_SIZE] = {0};

	struct sockaddr_in s_add, c_add;
	unsigned short portnum = 2333;

	printf("Hello,welcome to client!\r\n");
	if (argc != 3)
	{
		printf("usage: filetransferClient [ip] [file]\n");
		return -1;
	}

	if (-1 == (cfd = socket(AF_INET, SOCK_STREAM, 0)))
	{
		printf("socket fail! \r\n");
		return -1;
	}
	printf("socket ok!\r\n");

	bzero(&s_add, sizeof(struct sockaddr_in));
	s_add.sin_family = AF_INET;
	s_add.sin_addr.s_addr = inet_addr(argv[1]);
	s_add.sin_port = htons(portnum);
	printf("s_addr = %#x ,port : %#x\r\n", s_add.sin_addr.s_addr, s_add.sin_port);

	if (-1 == connect(cfd, (struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
	{
		printf("connect fail !\r\n");
		return -1;
	}
	printf("connect ok!\r\n");

	int fileHandler = 0;
	if ((fileHandler = open(argv[2], O_RDONLY)) < 0)
	{
		printf("File open error.\n");
		return -1;
	}

	int readBitN = 0;
	int count = 0;
	u16 crc_result;
	while ((readBitN = read(fileHandler, buffer + 3, PACKAGE_SIZE - 3)))
	{
		count++;
		buffer[0] = readBitN;
		crc_result=crc16(buffer + 3, PACKAGE_SIZE - 3, crc_16_MODBUS);
		unsigned char crc_char[2];
		crc_char[0]=crc_result >> 8;
		crc_char[1]=crc_result & 0xFF;
		buffer[1]=crc_char[0];
		buffer[2]=crc_char[1];
		printf("[%d]\t%d bytes of valid data sent.\n", count, buffer[0]);
		printf("CRC: %u\n", crc_result);
		send(cfd, buffer, PACKAGE_SIZE, 0);
	}

	close(cfd);
	close(fileHandler);

	printf("File send completed.\n");
	return 0;
}
