

#include "rand_table.h"
#include <assert.h>

RandGen	g_RandGen	= RandGen((unsigned)time(NULL));


RandTable::RandTable()
{
	Reset();
}

RandTable::~RandTable()
{

}

//	添加某一个类型及其概率进入圆桌
void RandTable::AddIntoRoundTable(int iType, unsigned int iProbability)
{
	if(iProbability == 0)
	{
		return;
	}
	TROUNT_TABLE_ELEMENT stTableElem;
	stTableElem.iStartPos = m_iNowStartPos;
	stTableElem.iEndPos = m_iNowStartPos + iProbability;
	stTableElem.Type	= iType;
	m_vtTableElement.push_back(stTableElem);
	m_iNowStartPos += iProbability;
	m_iTypeNum++;
}

//	通过随机值获取某一个类型
int RandTable::GetTypeFromRoundTable(unsigned int iRand)
{
	if (m_iNowStartPos == 0)
	{
		return 0;
	}
	if (iRand > m_iNowStartPos)
	{
		iRand = iRand%m_iNowStartPos;
	}
	for(unsigned int i = 0; i < m_iTypeNum; i++)
	{
		TROUNT_TABLE_ELEMENT stTableElem = m_vtTableElement[i];
		if (iRand >= stTableElem.iStartPos && iRand < stTableElem.iEndPos)
		{
			return stTableElem.Type;
		}
	}	
	return 0;	//	返回错误值
}

void RandTable::Reset()
{
	m_vtTableElement.clear();
	m_iNowStartPos = 0;
	m_iTypeNum = 0;
}


int RandGen::GetRand(int nStart, int nEnd)
{
	return (int)((nEnd-nStart)*RandDouble()) + nStart;
}
int  RandGen::RandRange(int nMax, int nMin)
{
	if (nMax == nMin)
	{
		return nMax;
	}
	if(nMax < nMin)
	{
		int nTemp = nMax;
		nMax = nMin;
		nMin = nTemp;
	}
	return ((RandUInt()% (nMax - nMin + 1)) + nMin);
}
bool   RandGen::RandRatio(unsigned int uRatio, unsigned int uRatioMax)
{
	if (uRatio == 0)
	{
		return false;
	}
	return (uRatio >= (unsigned int)RandRange(uRatioMax,0));
}



void test_randTable()
{
	//-----------------------------------------------------------------------
	//rand
	unsigned char cbArCardColor[] = { 0x00,0x10,0x20,0x30 };
	unsigned char cbArCardValue[] = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D };
	int iArLenCardColor = sizeof(cbArCardColor) / sizeof(cbArCardColor[0]);
	int iArLenCardValue = sizeof(cbArCardValue) / sizeof(cbArCardValue[0]);

	std::cout << "test_randTable - rand ..." << std::endl;
	std::cout << "test_randTable - iArCardColor:" << iArLenCardColor << ",iArLenCardValue:" << iArLenCardValue << std::endl;
	for (int i = 0; i < 10; i++)
	{
		int iRColor = g_RandGen.RandRange(0, iArLenCardColor);
		int iRValue = g_RandGen.RandRange(0, iArLenCardValue);
		int iUColor = g_RandGen.RandUInt() % (iArLenCardColor);
		int iUValue = g_RandGen.RandUInt() % (iArLenCardValue);
		std::cout << "test_randTable - iRColor:" << iRColor << ",iRValue:" << iRValue << ",iUColor:" << iUColor << ",iUValue:" << iUValue << std::endl;

	}
	for (int i = 0; i < 1000; i++)
	{
		unsigned int iUValue = g_RandGen.RandUInt() % (30); // 0-29 rand

		std::cout << "test_randTable - iUValue:" << iUValue << std::endl;

	}

}

