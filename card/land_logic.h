
#include <stdio.h>
#include <memory.h>

typedef unsigned char		BYTE;
static const int  MAX_LAND_COUNT = 20;                                    // �����Ŀ

//�߼�����
enum emLAND_CARD_TYPE
{
	CT_ERROR = 0,                                 //��������
	CT_SINGLE,                                    //��������
	CT_DOUBLE,                                    //��������
	CT_THREE,                                     //��������
	CT_SINGLE_LINE,                               //��������
	CT_DOUBLE_LINE,                               //��������
	CT_THREE_LINE,                                //��������
	CT_THREE_TAKE_ONE,                            //����һ��
	CT_THREE_TAKE_TWO,                            //����һ��
	CT_FOUR_TAKE_ONE,                             //�Ĵ�����
	CT_FOUR_TAKE_TWO,                             //�Ĵ�����
	CT_BOMB_CARD,                                 //ը������
	CT_MISSILE_CARD,                              //�������
};


//�����ṹ
struct tagAnalyseResult {
	BYTE cbBlockCount[4];                    //�˿���Ŀ
	BYTE cbCardData[4][MAX_LAND_COUNT];        //�˿�����
};

// �˿���ֵ����
#define	MASK_COLOR	    0xF0	//��ɫ����
#define	MASK_VALUE	    0x0F	//��ֵ����

//��������
#define ST_ORDER                    1                                    //��С����
#define ST_COUNT                    2                                    //��Ŀ����
#define ST_CUSTOM                   3                                    //�Զ�����


//��ȡ�˿���ֵ
BYTE    GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
//��ȡ�˿˻�ɫ
BYTE    GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

//�߼���ֵ
BYTE GetCardLogicValue(BYTE cbCardData)
{
	//�˿�����
	BYTE cbCardColor = GetCardColor(cbCardData);
	BYTE cbCardValue = GetCardValue(cbCardData);

	//ת����ֵ
	if (cbCardColor == 0x40) return cbCardValue + 2;
	return (cbCardValue <= 2) ? (cbCardValue + 13) : cbCardValue;
}

template <class T>
int getArrayLen(T& array)
{
	return (sizeof(array) / sizeof(array[0]));
}

void AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult &AnalyseResult)
{
	//���ý��
	memset(&AnalyseResult, 0, sizeof(AnalyseResult));

	//�˿˷���
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		//��������
		BYTE cbSameCount = 1;
		BYTE cbLogicValue = GetCardLogicValue(cbCardData[i]);

		//����ͬ��
		for (BYTE j = i + 1; j < cbCardCount; j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(cbCardData[j]) != cbLogicValue) break;

			//���ñ���
			cbSameCount++;
		}

		//���ý��
		BYTE cbIndex = AnalyseResult.cbBlockCount[cbSameCount - 1]++;
		for (BYTE j = 0; j < cbSameCount; j++)
			AnalyseResult.cbCardData[cbSameCount - 1][cbIndex * cbSameCount + j] = cbCardData[i + j];

		//��������
		i += cbSameCount - 1;
	}
	return;
}


void SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//��Ŀ����
	if (cbCardCount == 0) return;
	if (cbSortType == ST_CUSTOM) return;

	//ת����ֵ
	BYTE cbSortValue[MAX_LAND_COUNT];
	for (BYTE i = 0; i < cbCardCount; i++) cbSortValue[i] = GetCardLogicValue(cbCardData[i]);

	//�������
	bool bSorted = true;
	BYTE cbSwitchData = 0, cbLast = cbCardCount - 1;
	do {
		bSorted = true;
		for (BYTE i = 0; i < cbLast; i++) {
			if ((cbSortValue[i] < cbSortValue[i + 1]) ||
				((cbSortValue[i] == cbSortValue[i + 1]) && (cbCardData[i] < cbCardData[i + 1]))) {
				//���ñ�־
				bSorted = false;

				//�˿�����
				cbSwitchData = cbCardData[i];
				cbCardData[i] = cbCardData[i + 1];
				cbCardData[i + 1] = cbSwitchData;

				//����Ȩλ
				cbSwitchData = cbSortValue[i];
				cbSortValue[i] = cbSortValue[i + 1];
				cbSortValue[i + 1] = cbSwitchData;
			}
		}
		cbLast--;
	} while (bSorted == false);

	//��Ŀ����
	if (cbSortType == ST_COUNT) {
		//��������
		BYTE cbCardIndex = 0;

		//�����˿�
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(&cbCardData[cbCardIndex], cbCardCount - cbCardIndex, AnalyseResult);

		//��ȡ�˿�
		for (BYTE i = 0; i < getArrayLen(AnalyseResult.cbBlockCount); i++) {
			//�����˿�
			BYTE cbIndex = getArrayLen(AnalyseResult.cbBlockCount) - i - 1;
			memcpy(&cbCardData[cbCardIndex], AnalyseResult.cbCardData[cbIndex],
				AnalyseResult.cbBlockCount[cbIndex] * (cbIndex + 1) * sizeof(BYTE));

			//��������
			cbCardIndex += AnalyseResult.cbBlockCount[cbIndex] * (cbIndex + 1) * sizeof(BYTE);
		}
	}

	return;
}



//��ȡ����
BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	//������
	switch (cbCardCount) {
	case 0:    //����
	{
		return CT_ERROR;
	}
	case 1: //����
	{
		return CT_SINGLE;
	}
	case 2:    //���ƻ��
	{
		//�����ж�
		if ((cbCardData[0] == 0x4F) && (cbCardData[1] == 0x4E)) return CT_MISSILE_CARD;
		if (GetCardLogicValue(cbCardData[0]) == GetCardLogicValue(cbCardData[1])) return CT_DOUBLE;

		return CT_ERROR;
	}
	}

	//�����˿�
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

	//�����ж�
	if (AnalyseResult.cbBlockCount[3] > 0)
	{
		//�����ж�
		if ((AnalyseResult.cbBlockCount[3] == 1) && (cbCardCount == 4)) return CT_BOMB_CARD;
		if ((AnalyseResult.cbBlockCount[3] == 1) && (cbCardCount == 6)) return CT_FOUR_TAKE_ONE;
		if ((AnalyseResult.cbBlockCount[3] == 1) && (cbCardCount == 8) &&
			(AnalyseResult.cbBlockCount[1] == 2))
			return CT_FOUR_TAKE_TWO;


		return CT_ERROR;
	}
	//printf("GetCardType cbBlockCount:%d %d %d %d,cbCardCount:%d\n",
	//	AnalyseResult.cbBlockCount[0],AnalyseResult.cbBlockCount[1], AnalyseResult.cbBlockCount[2],AnalyseResult.cbBlockCount[3], cbCardCount);

	//�����ж�
	if (AnalyseResult.cbBlockCount[2] > 0)
	{
		//�����ж�
		if (AnalyseResult.cbBlockCount[2] > 1)
		{
			//��������
			BYTE cbCardData = AnalyseResult.cbCardData[2][0];
			BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

			//�������
			if (cbFirstLogicValue >= 15) return CT_ERROR;

			//�����ж�
			for (BYTE i = 1; i < AnalyseResult.cbBlockCount[2]; i++)
			{
				BYTE cbCardData = AnalyseResult.cbCardData[2][i * 3];
				if (cbFirstLogicValue != (GetCardLogicValue(cbCardData) + i))
				{
					//printf("GetCardType - i:%d,cbCardData:0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X	0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X \n",
					//	i, AnalyseResult.cbCardData[2][0], AnalyseResult.cbCardData[2][1], AnalyseResult.cbCardData[2][2], AnalyseResult.cbCardData[2][3], AnalyseResult.cbCardData[2][4], AnalyseResult.cbCardData[2][5], AnalyseResult.cbCardData[2][6], AnalyseResult.cbCardData[2][7], AnalyseResult.cbCardData[2][8], AnalyseResult.cbCardData[2][9], AnalyseResult.cbCardData[2][10], AnalyseResult.cbCardData[2][11], AnalyseResult.cbCardData[2][12], AnalyseResult.cbCardData[2][13], AnalyseResult.cbCardData[2][14], AnalyseResult.cbCardData[2][15], AnalyseResult.cbCardData[2][16], AnalyseResult.cbCardData[2][17], AnalyseResult.cbCardData[2][18], AnalyseResult.cbCardData[2][19]);
					//printf("GetCardType - cbFirstLogicValue:%d,cbCardCount:%d\n", cbFirstLogicValue, GetCardLogicValue(cbCardData) +i);

					return CT_ERROR;
				}
			}
		}
		else if (cbCardCount == 3)
		{
			return CT_THREE;
		}

		//�����ж�
		if (AnalyseResult.cbBlockCount[2] * 3 == cbCardCount) return CT_THREE_LINE;
		if (AnalyseResult.cbBlockCount[2] * 4 == cbCardCount) return CT_THREE_TAKE_ONE;
		if ((AnalyseResult.cbBlockCount[2] * 5 == cbCardCount) && (AnalyseResult.cbBlockCount[1] == AnalyseResult.cbBlockCount[2]))
		{
			return CT_THREE_TAKE_TWO;
		}

		return CT_ERROR;
	}

	//��������
	if (AnalyseResult.cbBlockCount[1] >= 3)
	{
		//��������
		BYTE cbCardData = AnalyseResult.cbCardData[1][0];
		BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

		//�������
		if (cbFirstLogicValue >= 15) return CT_ERROR;

		//�����ж�
		for (BYTE i = 1; i < AnalyseResult.cbBlockCount[1]; i++)
		{
			BYTE cbCardData = AnalyseResult.cbCardData[1][i * 2];
			if (cbFirstLogicValue != (GetCardLogicValue(cbCardData) + i)) return CT_ERROR;
		}

		//�����ж�
		if ((AnalyseResult.cbBlockCount[1] * 2) == cbCardCount) return CT_DOUBLE_LINE;

		return CT_ERROR;
	}

	//�����ж�
	if ((AnalyseResult.cbBlockCount[0] >= 5) && (AnalyseResult.cbBlockCount[0] == cbCardCount))
	{
		//��������
		BYTE cbCardData = AnalyseResult.cbCardData[0][0];
		BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

		//�������
		if (cbFirstLogicValue >= 15) return CT_ERROR;

		//�����ж�
		for (BYTE i = 1; i < AnalyseResult.cbBlockCount[0]; i++) {
			BYTE cbCardData = AnalyseResult.cbCardData[0][i];
			if (cbFirstLogicValue != (GetCardLogicValue(cbCardData) + i)) return CT_ERROR;
		}

		return CT_SINGLE_LINE;
	}


	return CT_ERROR;
}





bool CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount,	BYTE cbNextCount)
{
	//��ȡ����
	BYTE cbNextType = GetCardType(cbNextCard, cbNextCount);
	BYTE cbFirstType = GetCardType(cbFirstCard, cbFirstCount);

	//printf("CompareCard - cbFirstCount:%d,cbNextCount:%d,cbFirstType:%d,cbNextType:%d.\n", cbFirstCount, cbNextCount, cbFirstType, cbNextType);
	//		CompareCard - cbFirstCount:2, cbNextCount : 4, cbFirstType : 12, cbNextType : 11.
	//�����ж�
	if (cbNextType == CT_ERROR) return false;
	if (cbNextType == CT_MISSILE_CARD) return true;
	if (cbFirstType == CT_MISSILE_CARD) return false;

	//ը���ж�
	if ((cbFirstType != CT_BOMB_CARD) && (cbNextType == CT_BOMB_CARD)) return true;
	if ((cbFirstType == CT_BOMB_CARD) && (cbNextType != CT_BOMB_CARD)) return false;

	//printf("CompareCard 2 - cbFirstCount:%d,cbNextCount:%d,cbFirstType:%d,cbNextType:%d.\n", cbFirstCount, cbNextCount, cbFirstType, cbNextType);


	//�����ж�
	if ((cbFirstType != cbNextType) || (cbFirstCount != cbNextCount)) return false;

	//��ʼ�Ա�
	switch (cbNextType)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_THREE:
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
	case CT_THREE_LINE:
	case CT_BOMB_CARD:
	{
		//��ȡ��ֵ
		BYTE cbNextLogicValue = GetCardLogicValue(cbNextCard[0]);
		BYTE cbFirstLogicValue = GetCardLogicValue(cbFirstCard[0]);

		//�Ա��˿�
		return cbNextLogicValue > cbFirstLogicValue;
	}
	case CT_THREE_TAKE_ONE:
	case CT_THREE_TAKE_TWO:
	{
		//�����˿�
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;
		AnalysebCardData(cbNextCard, cbNextCount, NextResult);
		AnalysebCardData(cbFirstCard, cbFirstCount, FirstResult);

		//��ȡ��ֵ
		BYTE cbNextLogicValue = GetCardLogicValue(NextResult.cbCardData[2][0]);
		BYTE cbFirstLogicValue = GetCardLogicValue(FirstResult.cbCardData[2][0]);

		//�Ա��˿�
		return cbNextLogicValue > cbFirstLogicValue;
	}
	case CT_FOUR_TAKE_ONE:
	case CT_FOUR_TAKE_TWO:
	{
		//�����˿�
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;
		AnalysebCardData(cbNextCard, cbNextCount, NextResult);
		AnalysebCardData(cbFirstCard, cbFirstCount, FirstResult);

		//��ȡ��ֵ
		BYTE cbNextLogicValue = GetCardLogicValue(NextResult.cbCardData[3][0]);
		BYTE cbFirstLogicValue = GetCardLogicValue(FirstResult.cbCardData[3][0]);

		//�Ա��˿�
		return cbNextLogicValue > cbFirstLogicValue;
	}
	}

	return false;
}



//enum emLAND_CARD_TYPE
//{
//	CT_ERROR = 0,                                 //��������  0
//	CT_SINGLE,                                    //��������  1
//	CT_DOUBLE,                                    //��������  2
//	CT_THREE,                                     //��������  3
//	CT_SINGLE_LINE,                               //��������  4
//	CT_DOUBLE_LINE,                               //��������  5
//	CT_THREE_LINE,                                //��������  6
//	CT_THREE_TAKE_ONE,                            //����һ��  7
//	CT_THREE_TAKE_TWO,                            //����һ��  8
//	CT_FOUR_TAKE_ONE,                             //�Ĵ�����  9
//	CT_FOUR_TAKE_TWO,                             //�Ĵ�����  10
//	CT_BOMB_CARD,                                 //ը������  11
//	CT_MISSILE_CARD,                              //�������  12
//};

void testLandServer()
{
	//land server

	//unsigned char turnCardData[] = { 0x4f,0x4e };
	//unsigned char turnCardCount = 2;

	//unsigned char cardData[] = { 0x3b,0x2b,0x1b,0x0b };
	//unsigned char cardCount = 4;

	bool bIsRet = false;// CompareCard(turnCardData, cardData, turnCardCount, cardCount);

						//printf("land_server CompareCard - bIsRet:%d\n", bIsRet);

						//unsigned char cbCardData[] = { 0x03,0x13,0x23,0x04,0x14,0x24 };
						//unsigned char cbCardCount = 6;

						//unsigned char cbCardData[] = { 0x03,0x13,0x23,0x04,0x14,0x24,0x05,0x15,0x25,0x1C,0x1D,0x1B };
						//unsigned char cbCardCount = 12;

						//unsigned char cbCardData[] = { 0x3b,0x2b,0x1b,0x0b };

						//unsigned char cbCardData[] = { 0x03,0x13 }; // 2

						//unsigned char cbCardData[] = { 0x03,0x13,0x23 }; // 3

						//unsigned char cbCardData[] = { 0x03,0x13,0x23,0x14 }; // 7

						//unsigned char cbCardData[] = { 0x03,0x13,0x23,0x14,0x24 }; // 8

						//unsigned char cbCardData[] = { 0x15,0x25,0x35, 0x14,0x24,0x34,0x03,0x13,0x23, }; // 6

						//unsigned char cbCardData[] = { 0x14,0x24,0x34,0x03,0x13,0x23,0x05,0x06 }; // 7

						//unsigned char cbCardData[] = { 0x14,0x24,0x34,0x03,0x13,0x23,0x05,0x15,0x06,0x16 }; // 8

	unsigned char cbCardData[] = { 0x03,0x13,0x23,0x14,0x24,0x34,0x05,0x15,0x06,0x16 }; // 8

	SortCardList(cbCardData, sizeof(cbCardData), ST_ORDER);

	BYTE cbCardType = GetCardType(cbCardData, sizeof(cbCardData));

	printf("land_server CompareCard - bIsRet:%d,cbCardType:%d\n", bIsRet, cbCardType);

}




