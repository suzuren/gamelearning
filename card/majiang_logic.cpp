
#include "majiang_logic.h"

#include <stdio.h>
//扑克转换
BYTE SwitchToCardData(BYTE cbCardIndex)
{
	return ((cbCardIndex / 9) << 4) | (cbCardIndex % 9 + 1);
}

//扑克转换
BYTE SwitchToCardIndex(BYTE cbCardData)
{
	////计算位置
	return ((cbCardData&MASK_COLOR) >> 4) * 9 + (cbCardData&MASK_VALUE) - 1;
}

BYTE    GetAIOutCard(uint16 chairID)
{
	BYTE outCard = 0xff;
	//优先1,9边张
	for (uint8 j = 0; j < MAX_INDEX; ++j)
	{
		if (j % 9 == 0 || j % 9 == 8)
		{
			printf("majiang - GetAIOutCard - j:%d,mod9:%d\n",j, j % 9);
		}
	}
	return outCard;
}


BYTE    GetAIOutCard_1(uint16 chairID)
{
	for (int i = 0; i<3; i++)
	{
		//if (m_cbCardIndex[wMeChairID][i * 9] == 1)
		//{
		//	if ((m_cbCardIndex[wMeChairID][i * 9 + 1] == 0)
		//		&& (m_cbCardIndex[wMeChairID][i * 9 + 2] == 0))
		//	{
		//		return m_GameLogic.SwitchToCardData(i * 9);
		//	}
		//}
		//if (m_cbCardIndex[wMeChairID][i * 9 + 8] == 1)
		//{
		//	if ((m_cbCardIndex[wMeChairID][i * 9 + 7] == 0)
		//		&& (m_cbCardIndex[wMeChairID][i * 9 + 6] == 0))
		//	{
		//		return m_GameLogic.SwitchToCardData(i * 9 + 8);
		//	}
		//}

		printf("majiang - GetAIOutCard_1 - i:%d,mult:%d - %d\n", i, i * 9, i * 9 + 8);

	}
	return 0;
}
//majiang - GetAIOutCard_1 - i : 0, mult : 0 - 8
//majiang - GetAIOutCard_1 - i : 1, mult : 9 - 17
//majiang - GetAIOutCard_1 - i : 2, mult : 18 - 26

BYTE    GetAIOutCard_2(uint16 chairID)
{
	//                     0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 
	BYTE cbCardIndex = { 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 2, 3, 1, 0 };
	for (int i = 0; i<27; i++)
	{
		if (cbCardIndex[i] == 1)
		{
			//if ((i % 9) == 0)
			//{
			//	printf("majiang - GetAIOutCard_2 - i:%d, mod9:%d, and3:%d\n", i, i % 9, i + 3);

			//	i += 3;
			//	continue;
			//}
			if ((i + 1) % 9 == 0)
			{
				printf("majiang - GetAIOutCard_2 - i:%d, mod9:%d\n", i, (i+1) % 9);

				continue;
			}
			if (cbCardIndex[i + 1] == 0 && cbCardIndex[i - 1] == 0)
			{
				printf("majiang - GetAIOutCard_2 - i:%d\n", i);

				//return SwitchToCardData(i);
			}
			i += 1;
		}


	}


	return 0;
}


BYTE    CGameMaJiangTable::GetAIOutCard(uint16 chairID)
{
	//                   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 
	BYTE cbCardIndex = { 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 2, 3, 1, 0 };

	BYTE outCard = 0xff;
	//有风打风
	for (int i = 27; i<27 + 7; i++)
	{
		if (cbCardIndex[i] == 1)
		{
			outCard = SwitchToCardData(i);
			return outCard;
		}
	}
	//打19孤牌
	for (int i = 0; i<1; i++)
	{
		if (cbCardIndex[i * 9] == 1)
		{
			if ((cbCardIndex[i * 9 + 1] == 0) && (cbCardIndex[i * 9 + 2] == 0))
			{
				outCard = SwitchToCardData(i * 9);
				return outCard;
			}
		}
		if (cbCardIndex[i * 9 + 8] == 1)
		{
			if ((cbCardIndex[i * 9 + 7] == 0) && (cbCardIndex[i * 9 + 6] == 0))
			{
				outCard = SwitchToCardData(i * 9 + 8);
				return outCard;
			}
		}
	}

	//打普通卡张
	for (int i = 0; i<9; i++)
	{
		if (i == 0 || i == 8)
		{
			continue;
		}
		if (cbCardIndex[i] == 1)
		{
			if (cbCardIndex[i + 1] == 0 && cbCardIndex[i - 1] == 0)
			{
				outCard = SwitchToCardData(i);
				return outCard;
			}
		}
	}

	//单张不靠
	for (uint8 j = 0; j<MAX_INDEX; ++j)
	{
		if (cbCardIndex[j] == 1 && cbCardIndex[j + 1] == 0)
		{//单张
			outCard = SwitchToCardData(j);
			return outCard;
		}
		else
		{
			j += 2;
		}
	}

	for (int i = 0; i<9; i++)
	{
		if (i == 0)
		{
			if (cbCardIndex[i + 1] != 0 && cbCardIndex[i + 2] == 0)
			{
				outCard = SwitchToCardData(i);
				return outCard;
			}
			continue;
		}
		if (i == 8)
		{
			if (cbCardIndex[i - 1] != 0 && cbCardIndex[i - 2] == 0)
			{
				outCard = SwitchToCardData(i);
				return outCard;
			}
			continue;
		}
		if (cbCardIndex[i] == 1)
		{
			if (cbCardIndex[i + 1] != 0 && cbCardIndex[i - 1] == 0)
			{
				outCard = SwitchToCardData(i);
				return outCard;
			}
		}
	}

	vector vecFengCardCount;

	for (int i = 27; i<27 + 7; i++)
	{
		if (cbCardIndex[i] == 2)
		{
			outCard = SwitchToCardData(i);
			return outCard;
		}
	}

	// 打单张
	for (uint8 j = 0; j<MAX_INDEX; ++j)
	{
		if (cbCardIndex[j] == 1)
		{//单张
			outCard = SwitchToCardData(j);
			return outCard;
		}
	}
	//for(uint8 j=0;j<MAX_INDEX;++j){
	//    if(cbCardIndex[j] > 0 && !m_gameLogic.IsKingCardIndex(j)){
	//        outCard=m_gameLogic.SwitchToCardData(j);
	//    }
	//}
	if (outCard == 0xff)
	{
		for (uint8 j = 0; j<MAX_INDEX; ++j) {
			if (cbCardIndex[j] > 0) {
				outCard = m_gameLogic.SwitchToCardData(j);
			}
		}
	}
	return outCard;
}


void test_majiang()
{
	//GetAIOutCard(0);
	//GetAIOutCard_1(0);
	GetAIOutCard_2(0);
}