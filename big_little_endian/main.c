
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
	// �����ֽ�����ܱ�ʾ65535
	unsigned char buffer[2] = { 0 };
	//unsigned short pack_size = 65535;
	unsigned short pack_size = 6553;
	// ���մ�˴��buffer
	buffer[0] = (pack_size >> 8) & 0xff; // �Ѹ�λ���ֽ��ƶ���λ�ֽڣ��ƶ� 8 λ��Ҳ�����ƶ��� 1 �ֽڣ��� 0xff ���룬��ֻ֤ȡһ�ֽ���ֵ
	buffer[1] = pack_size & 0xff; // �� 0xff ���룬��ֻ֤ȡһ�ֽ���ֵ������ȡ��������ֽڵ���ֵ
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

