
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

int GetPacketLen(const char unsigned * data)
{
	static unsigned char buffer[2];
	buffer[0] = 0;
	buffer[1] = 0;
	buffer[0] = data[0];
	buffer[1] = data[1];
	int depack_size = (int)buffer[0] << 8 | (int)buffer[1];

	printf("GetPacketLen - pack_size:%d,buffer:%d,%d\n", depack_size, buffer[0], buffer[1]);

	return depack_size;
}

void BigLittleStorage()
{
	// 两个字节最大能表示65535
	unsigned char buffer[2] = { 0 };
	//unsigned short pack_size = 65535;
	unsigned short pack_size = 6553;
	// 按照大端存进buffer
	buffer[0] = (pack_size >> 8) & 0xff; // 把高位的字节移动低位字节，移动 8 位，也就是移动了 1 字节，与 0xff 相与，保证只取一字节数值
	buffer[1] = pack_size & 0xff; // 与 0xff 相与，保证只取一字节数值，并且取的是最低字节的数值
	printf("big storage - pack_size:%d,>>8:%d - &0xff:%d,,buffer:%d,%d\n", pack_size, (pack_size >> 8),(pack_size & 0xff),buffer[0],buffer[1]);

	static unsigned char data[2];
	data[0] = 0;
	data[1] = 0;

	data[0] = buffer[0];
	data[1] = buffer[1];

	unsigned int depack_size = (unsigned int)data[0] << 8 | (unsigned int)data[1];

	printf("big storage decode - depack_size:%d\n", depack_size);

	GetPacketLen(buffer);
}



int main(int argc, const char** argv)
{
	bool bStorageMode = CheckStorageModeEx();
	printf("hello world - bStorageMode:%d\n", bStorageMode);

	BigLittleStorage();

	return 0;
}

