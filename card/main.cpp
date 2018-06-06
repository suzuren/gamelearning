
#include "land_logic.h"

int main(int argc, const char** argv)
{
	printf("hello world!\n");

	//land server

	unsigned char turnCardData[] = { 0x4f,0x4e };
	unsigned char turnCardCount = 2;

	unsigned char cardData[] = { 0x3b,0x2b,0x1b,0x0b };
	unsigned char cardCount = 4;

	bool bIsRet = CompareCard(turnCardData, cardData, turnCardCount, cardCount);

	printf("land_server CompareCard - bIsRet:%d\n", bIsRet);

	return 0;
}

