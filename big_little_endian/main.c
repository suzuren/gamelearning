
#include <stdio.h>
#include <arpa/inet.h>
// return : true - big endian , false - little endian

bool CheckStorageMode()
{
	int iFlag = 0x12345678;
	char *cByte = (char *)&iFlag;
	return(*cByte == 0x12);
}

bool CheckStorageModeEx()
{
	int iFlag = 0x12345678;
	iFlag = htons(iFlag);
	char *cByte = (char *)&iFlag;
	return(*cByte == 0x12);
}

void BigLittleStorage()
{
	// 两个字节最大能表示65535
	unsigned char buffer[2] = { 0 };
	unsigned short pack_size = 65535;
	// 按照大端存进buffer
	buffer[0] = (pack_size >> 8) & 0xff;
	buffer[1] = pack_size & 0xff;
	printf("big storage -pack_size:%d,buffer:%d,%d\n", pack_size,buffer[0],buffer[1]);

	unsigned int depack_size = (unsigned int)buffer[0] << 8 | (unsigned int)buffer[1];

	printf("big storage decode - depack_size:%d\n", depack_size);

}

int main(int argc, const char** argv)
{
	bool bStorageMode = CheckStorageModeEx();
	printf("hello world - bStorageMode:%d\n", bStorageMode);

	BigLittleStorage();

	return 0;
}

