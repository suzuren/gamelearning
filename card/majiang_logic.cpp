
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



void test_majiang()
{
	//GetAIOutCard(0);
	GetAIOutCard_1(0);
}