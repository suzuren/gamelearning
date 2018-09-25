
#ifndef _RAND_TABLE_H__
#define _RAND_TABLE_H__

#include <vector>
#include <time.h>

class RandTable
{
public:
	RandTable();
	~RandTable();

	//	添加某一个类型及其概率进入圆桌
	void AddIntoRoundTable(int iType, unsigned int iProbability);

	//	通过随机值获取某一个类型
	int  GetTypeFromRoundTable(unsigned int iRand);

	void Reset();
private:
	struct TROUNT_TABLE_ELEMENT
	{
		unsigned int iStartPos;
		unsigned int iEndPos;
		int  Type;
	};
	std::vector<TROUNT_TABLE_ELEMENT> m_vtTableElement;
	unsigned int 	m_iNowStartPos;
	unsigned int 	m_iTypeNum;
};

//随机数生成类

class RandGen
{	
private:
	unsigned int m_Seed[3];
	static const unsigned int Max32BitLong = 0xFFFFFFFFLU;
public:
	static const unsigned int RandomMax = Max32BitLong;

	RandGen(const unsigned int p_Seed = 0)
	{
		Reset(p_Seed);
	}
	//ReSeed the random number generator
	//种子处理
	void Reset(const unsigned int p_Seed = 0)
	{
		m_Seed[0] = (p_Seed ^ 0xFEA09B9DLU) & 0xFFFFFFFELU;
		m_Seed[0] ^= (((m_Seed[0] << 7) & Max32BitLong) ^ m_Seed[0]) >> 31;

		m_Seed[1] = (p_Seed ^ 0x9C129511LU) & 0xFFFFFFF8LU;
		m_Seed[1] ^= (((m_Seed[1] << 2) & Max32BitLong) ^ m_Seed[1]) >> 29;

		m_Seed[2] = (p_Seed ^ 0x2512CFB8LU) & 0xFFFFFFF0LU;
		m_Seed[2] ^= (((m_Seed[2] << 9) & Max32BitLong) ^ m_Seed[2]) >> 28;

		RandUInt();
	}

	//Returns an unsigned integer from 0..RandomMax
	//0~RandMax uint 随机数
	unsigned int RandUInt()
	{
		m_Seed[0] = (((m_Seed[0] & 0xFFFFFFFELU) << 24) & Max32BitLong)	^ ((m_Seed[0] ^ ((m_Seed[0] << 7) & Max32BitLong)) >> 7);

		m_Seed[1] = (((m_Seed[1] & 0xFFFFFFF8LU) << 7) & Max32BitLong)	^ ((m_Seed[1] ^ ((m_Seed[1] << 2) & Max32BitLong)) >> 22);

		m_Seed[2] = (((m_Seed[2] & 0xFFFFFFF0LU) << 11) & Max32BitLong)	^ ((m_Seed[2] ^ ((m_Seed[2] << 9) & Max32BitLong)) >> 17);

		return (m_Seed[0] ^ m_Seed[1] ^ m_Seed[2]);
	}
	//Returns a double in [0.0, 1.0]
	//返回0.0~1.0之间的双精度浮点
	double RandDouble()
	{
		return static_cast<double>(RandUInt()) / (static_cast<double>(RandomMax) );
	}	
	int	   GetRand(int nStart, int nEnd);	
	int	   RandRange(int nMax, int nMin);
	bool   RandRatio(unsigned int uRatio, unsigned int uRatioMax = 10000);
		
};

extern	RandGen	g_RandGen;


#include <iostream>

void test_randTable();


#endif //_RAND_TABLE_H__

