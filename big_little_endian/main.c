
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

int main(int argc, const char** argv)
{
	bool bStorageMode = CheckStorageModeEx();
	printf("hello world - bStorageMode:%d\n", bStorageMode);
	return 0;
}

