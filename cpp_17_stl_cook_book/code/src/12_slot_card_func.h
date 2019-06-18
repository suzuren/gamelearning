
#define MAX_LINE            3           //最大行数
#define MAX_ROW				5           //最大列数
#define MAX_TOTAL           11          //最大总数
#define MAX_JETTON          9           //最大下注

#include <random>
#include <functional>
#include <string>
#include <stdio.h>
#include <stdarg.h>
//0   香蕉
//1   西瓜
//2   柠檬
//3   葡萄
//4   草莓
//5   铃铛
//6   樱桃
//7   bar
//8   万能wild
//9   免费 bonus
//10  777图片

int GetRandi(int min, int max)
{
	std::random_device r;
	std::default_random_engine generator(r());
	std::uniform_int_distribution<int> dis(min, max);
	auto dice = std::bind(dis, generator);
	return dice();
}

std::string FormatToString(const char* fmt, ...)
{
	char chTemp[4096] = { 0 };

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(chTemp, sizeof(chTemp), fmt, ap);
	va_end(ap);

	return chTemp;
}

unsigned int GetRandImageIndex(int row)
{
	//根据权重表计算五条线的图片概率
	unsigned int tempArrWeightCfg[MAX_ROW][MAX_TOTAL] =
	{
		//00 01 02 03 04 05 06 07 08 09 10
		{ 64,64,64,64,64,64,64,64,64,00,00 },//0
		{ 64,64,64,64,64,64,64,64,64,00,00 },//1
		{ 64,64,64,64,64,64,64,64,64,00,00 },//2
		{ 64,64,64,64,64,64,64,64,64,00,00 },//3
		{ 64,64,64,64,64,64,64,64,64,00,00 } //4
	};
	unsigned int nRowSumWeight = 0;
	for (int k = 0; k < MAX_TOTAL; k++)
	{
		nRowSumWeight += tempArrWeightCfg[row][k];
	}
	unsigned int nRandWeight = GetRandi(0, nRowSumWeight);
	unsigned int nRandImageIndex = 0;
	for (int iIndex = 0; iIndex < MAX_TOTAL; iIndex++)
	{
		if (tempArrWeightCfg[row][iIndex] == 0)
		{
			continue;
		}
		if (nRandWeight <= tempArrWeightCfg[row][iIndex])
		{
			nRandImageIndex = iIndex;
			break;
		}
		else
		{
			nRandWeight -= tempArrWeightCfg[row][iIndex];
		}
	}
	return nRandImageIndex;
}

//0   香蕉
//1   西瓜
//2   柠檬
//3   葡萄
//4   草莓
//5   铃铛
//6   樱桃
//7   bar
//8   万能wild
//9   免费 bonus
//10  777图片
long long GetLineMultiple(unsigned int lineImage[MAX_ROW], unsigned int & nImageIndex, int & iLinkCount)
{
	iLinkCount = 0;
	//判断首位是否是万能图片
	if (8 == lineImage[0])
	{
		for (int i = 0; i < MAX_ROW; i++)//计算相同的连线
		{
			//特殊符号
			if (0 == lineImage[i] || lineImage[i] == 9 || lineImage[i] == 10)
			{
				break;
			}
			if (lineImage[i] != 8 && lineImage[0] == 8)
			{
				lineImage[0] = lineImage[i];
			}
			//统计个数
			if (lineImage[i] == lineImage[0] || lineImage[i] == 8)
			{
				iLinkCount += 1;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		//计算相同的连线
		for (int i = 1; i < MAX_ROW; i++)//计算相同的连线
		{
			if (0 == lineImage[i])
			{
				break;
			}
			//统计相连的个数
			if (lineImage[0] == lineImage[i] || (lineImage[i] == 8 && 9 != lineImage[0] && 10 != lineImage[0]))
			{
				iLinkCount += 1;
			}
			else
			{
				break;
			}
		}
	}
	nImageIndex = lineImage[0];
	long long lMultiple = 0;
	// 一共有10种图片，每条线上面的连续图片倍数为下面配置
	long long tempArrMultipleCfg[MAX_TOTAL][MAX_ROW] =
	{
		//0 1 2   3   4
		{ 0,1,3,  10, 75 },// 0
		{ 0,0,3,  10, 85 },// 1
		{ 0,0,15, 40, 250 },// 2
		{ 0,0,25, 50, 400 },// 3
		{ 0,0,30, 70, 550 },// 4
		{ 0,0,35, 80, 650 },// 5
		{ 0,0,45, 100,800 },// 6
		{ 0,0,75, 175,1250 },// 7
		{ 0,0,0,  0,  0 },// 8
		{ 0,0,0,  0,  0 },// 9
		{ 0,0,100,200,1750 } // 10
	};
	if (nImageIndex < MAX_TOTAL && iLinkCount < MAX_ROW)
	{
		lMultiple = tempArrMultipleCfg[nImageIndex][iLinkCount];
	}
	return	lMultiple;
}


bool GetMultipleImageArray()
{
	// 图片为3行5列 按照数字标位为
	// 从 0 开始数
	// 00 03 06 09 12
	// 01 04 07 10 13
	// 02 05 08 11 14
	// 从 1 开始数
	// 01 04 07 10 13
	// 02 05 08 11 14
	// 03 06 09 12 15

	unsigned int tempRandImage[MAX_LINE][MAX_ROW] =
	{
		//0  1  2  3  4
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 }
	};
	//所以9条线表示为
	// 从 0 开始数
	unsigned int tempArrLineCfg[MAX_TOTAL][MAX_ROW] =
	{
		// 0  1  2  3  4
		{ 01,04,07,10,13 },//00
		{ 00,03,06, 9,12 },//01
		{ 02,05, 8,11,14 },//02
		{ 00,04, 8,10,12 },//03
		{ 02,04,06,10,14 },//04
		{ 00,03,07,11,14 },//05
		{ 02,05,07, 9,12 },//06
		{ 01,05,07, 9,13 },//07
		{ 01,03,07,11,13 } //08
	};
	// 从 1 开始数
	//unsigned int tempArrLineCfg[MAX_TOTAL][MAX_ROW] =
	//{
	//	{ 2,5,8,11,14 },//00
	//	{ 1,4,7,10,13 },//01
	//	{ 3,6,9,12,15 },//02
	//	{ 1,5,9,11,13 },//03
	//	{ 3,5,7,11,15 },//04
	//	{ 1,4,8,12,15 },//05
	//	{ 3,6,8,10,13 },//06
	//	{ 2,6,8,10,14 },//07
	//	{ 2,4,8,12,14 }	//08
	//};

	for (int i = 0; i < MAX_ROW; i++)
	{
		for (int j = 0; j < MAX_LINE; j++)
		{
			unsigned int nRandImageIndex = GetRandImageIndex(i);
			tempRandImage[j][i] = nRandImageIndex;
		}
	}

	printf("rand image:\n");
	for (int i = 0; i < MAX_LINE; i++)
	{
		std::string strPrint;
		strPrint += FormatToString("line:%d - ", i);
		for (int j = 0; j < MAX_ROW; j++)
		{
			strPrint += FormatToString("%02d ", tempRandImage[i][j]);
		}
		strPrint += "\n";
		printf("%s", strPrint.data());
	}

	int line = GetRandi(0, MAX_JETTON - 1);
	for (int j = 0; j < MAX_ROW; j++)
	{
		//int l = (tempArrLineCfg[line][j]-1) % MAX_LINE;
		//int r = (tempArrLineCfg[line][j]-1) / MAX_LINE;
		int l = tempArrLineCfg[line][j] % MAX_LINE;
		int r = tempArrLineCfg[line][j] / MAX_LINE;
		//printf("l:%d,r:%d\n", l, r);
		tempRandImage[l][r] = 10;
	}

	printf("line image:%d\n", line);
	for (int i = 0; i < MAX_LINE; i++)
	{
		std::string strPrint;
		strPrint += FormatToString("line:%d - ", i);
		for (int j = 0; j < MAX_ROW; j++)
		{
			strPrint += FormatToString("%02d ", tempRandImage[i][j]);
		}
		strPrint += "\n";
		printf("%s", strPrint.data());
	}

	// 计算 9 条线的倍率
	long long lTotalMultiple = 0;
	int iTotalFreeCount = 0;
	//BONUS奖励免费次数 
	const static int  iFreeCount[MAX_ROW] = { 0,0,3,7,12 };
	printf("calculate multiple\n");
	for (int i = 0; i < MAX_JETTON; i++)
	{
		unsigned int lineImage[MAX_ROW] = { 0 };//顺序的线		
		for (int j = 0; j < MAX_ROW; j++)
		{
			int l = tempArrLineCfg[i][j] % MAX_LINE;
			int r = tempArrLineCfg[i][j] / MAX_LINE;
			lineImage[j] = tempRandImage[l][r];
		}
		//统计线的结果
		unsigned int nImageIndex;
		int iLinkCount;
		long long lMultiple = GetLineMultiple(lineImage, nImageIndex, iLinkCount);
		if (lMultiple > 0 && nImageIndex != 9)
		{
			lTotalMultiple += lMultiple;
		}
		//计算免费bonus次数
		if (nImageIndex == 9 && iLinkCount < MAX_ROW)
		{
			iTotalFreeCount += iFreeCount[iLinkCount];
		}

		std::string strPrint;
		strPrint += FormatToString("line:%d - ", i);
		for (int j = 0; j < MAX_ROW; j++)
		{
			strPrint += FormatToString("%02d ", lineImage[j]);
		}
		strPrint += "   ";
		strPrint += FormatToString("nImageIndex:%02d,", nImageIndex);
		strPrint += FormatToString("iLinkCount:%d,", iLinkCount);
		strPrint += FormatToString("lMultiple:%04d,", lMultiple);
		strPrint += FormatToString("lTotalMultiple:%04d,", lTotalMultiple);
		strPrint += FormatToString("iTotalFreeCount:%02d", iTotalFreeCount);
		strPrint += "\n";
		printf("%s", strPrint.data());
	}

	return true;
}



void test_slot_card_func()
{
	GetMultipleImageArray();

}
