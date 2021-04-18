#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#include "crc.h"

#define PACKAGE_SIZE 128

int main()
{
	int sfp, nfp, num = 0;
	int recbyte = 0;
	struct sockaddr_in s_add, c_add;
	int sin_size;
	unsigned short portnum = 2333;

	u16 crc_result;

	char buffer[PACKAGE_SIZE] = {0};

	printf("Hello, welcome to my server!\r\n");

	if (-1 == (sfp = socket(AF_INET, SOCK_STREAM, 0)))
	{
		printf("socket fail! \r\n");
		return -1;
	}
	printf("socket ok!\r\n");

	bzero(&s_add, sizeof(struct sockaddr_in));
	s_add.sin_family = AF_INET;
	s_add.sin_addr.s_addr = htonl(INADDR_ANY);
	s_add.sin_port = htons(portnum);

	if (-1 == bind(sfp, (struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
	{
		printf("bind fail!\r\n");
		return -1;
	}
	printf("bind ok!\r\n");

	if (-1 == listen(sfp, 5))
	{
		printf("listen fail !\r\n");
		return -1;
	}
	printf("listen ok\r\n");

	int fileHandler = 0;
	if ((fileHandler = open("/tmp/ico.ico", O_WRONLY | O_CREAT | O_TRUNC), 0777) < 0)
	{
		printf("File open error.\n");
		return -1;
	}

	sin_size = sizeof(struct sockaddr_in);
	nfp = accept(sfp, (struct sockaddr *)(&c_add), &sin_size);
	if (-1 == nfp)
	{
		printf("accept fail!\r\n");
		return -1;
	}
	printf("accept ok!\r\nServer start get connect from %#x : %#x\r\n",
		   ntohl(c_add.sin_addr.s_addr), ntohs(c_add.sin_port));

	int count = 0;
	while (1)
	{
		int byteN = 0;
		if (-1 == (recbyte = read(nfp, buffer, PACKAGE_SIZE)))
		{
			printf("read data fail !\r\n");
			return -1;
		}
		count++;
		printf("[%d]\t%d bytes recived.\n", count, recbyte);
		if (recbyte == 0)
		{
			printf("EOF\n");
			break;
		}
		crc_result=crc16(buffer + 3, PACKAGE_SIZE - 3, crc_16_MODBUS);
		printf("CRC: %u\n",crc_result);
		unsigned char crc_char[2];
		crc_char[0]=buffer[1];
		crc_char[1]=buffer[2];
		if ((crc_char[0]==(crc_result >> 8 ))&&(crc_char[1]==(crc_result & 0xFF)))
		{
			printf("Data Verified.\n");
		}
		else
		{
			printf("Transmission failed.\n");
			return -1;
		}

		byteN = write(fileHandler, buffer + 3, buffer[0]);
		printf("\t%d bytes of valid data.\n", buffer[0]);
		printf("\t%d bytes written to file.\n", byteN);
		
	}

	close(fileHandler);
	close(nfp);
	close(sfp);

	return 0;
}