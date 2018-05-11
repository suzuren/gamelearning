
#include <stdio.h>

#include <memory.h>

#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))

struct stGangCardResult
{
	unsigned char	cbCardCount;						//扑克数目
	unsigned char	cbCardData[4];						//扑克数据

	stGangCardResult()
	{
		//Init();
	}
	void Init()
	{
		cbCardCount = 0;
		for (int i = 0; i < 4; i++)
		{
			cbCardData[i] = 0;
		}
	}
};

void tets_memory_zero(stGangCardResult & GangCardResult)
{
	//ZeroMemory(&GangCardResult, sizeof(GangCardResult));
	GangCardResult.Init();
	printf("cbCardCount:%d,cbCardData:%d %d %d %d\n",	GangCardResult.cbCardCount, GangCardResult.cbCardData[0], GangCardResult.cbCardData[1], GangCardResult.cbCardData[2], GangCardResult.cbCardData[3]);
}

void func1();

int main(int argc, const char** argv)
{
	printf("hello test!\n");

	stGangCardResult GangCardResult;

	tets_memory_zero(GangCardResult);

	func1();

	return 0;
}







void func1()
{
	printf("hello world!\n");
}

