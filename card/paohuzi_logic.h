
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <vector>

namespace game_paohuzi
{
	//@brief  ASSERT
#define ASSERT(EXPR)\
do{\
  if (!(EXPR)){assert(false);}\
}while(0);

#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))

	//����ά��
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

	typedef unsigned char		BYTE;



#define PAOHUZI_GAME_PLAYER			3									//��Ϸ����

	//��ֵ����
#define MAX_WEAVE					7									//������
#define MAX_INDEX					20									//�������
#define MAX_COUNT					21									//�����Ŀ
#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

//��������
#define ACK_NULL					0x00								//��
#define ACK_TI						0x01								//��
#define ACK_PAO						0x02								//��
#define ACK_WEI						0x04								//��
#define ACK_CHI						0x08								//��
#define ACK_CHI_EX					0x10								//��
#define ACK_PENG					0x20								//��
#define ACK_CHIHU					0x40								//��

//��������
#define CK_NULL						0x00								//��Ч����
#define CK_XXD						0x01								//СС���
#define CK_XDD						0x02								//С����
#define CK_EQS						0x04								//����ʮ��
#define CK_LEFT						0x10								//�������
#define CK_CENTER					0x20								//���ж���
#define CK_RIGHT					0x40								//���Ҷ���

//////////////////////////////////////////////////////////////////////////

//�������
	struct tagWeaveItem
	{
		BYTE							cbWeaveKind;						//�������
		BYTE							cbCardCount;						//�˿���Ŀ
		BYTE							cbCenterCard;						//�����˿�
		BYTE							cbCardList[4];						//�˿��б�
	};

	//������Ϣ
	struct tagChiCardInfo
	{
		BYTE							cbChiKind;							//��������
		BYTE							cbCenterCard;						//�����˿�
		BYTE							cbResultCount;						//�����Ŀ
		BYTE							cbCardData[3][3];					//�������
		BYTE							cbChiCardData[3];					//��������(���ݳ������ͺ�������)
	};

	//��������
	struct tagAnalyseItem
	{
		BYTE							cbCardEye;							//�����˿�
		BYTE							cbHuXiCount;						//��Ϣ��Ŀ
		BYTE							cbWeaveCount;						//�����Ŀ
		tagWeaveItem					WeaveItemArray[MAX_WEAVE];			//����˿�
	};

	//������Ϣ
	struct tagHuCardInfo
	{
		BYTE							cbCardEye;							//�����˿�
		BYTE							cbHuXiCount;						//��Ϣ��Ŀ
		BYTE							cbWeaveCount;						//�����Ŀ
		tagWeaveItem					WeaveItemArray[MAX_WEAVE];			//����˿�
	};


	//��Ч�ж�
	bool IsValidCard(BYTE cbCardData)
	{
		BYTE cbValue = (cbCardData & MASK_VALUE);
		BYTE cbColor = (cbCardData & MASK_COLOR) >> 4;
		return ((cbValue >= 1) && (cbValue <= 10) && (cbColor <= 1));
	}

	//�˿�ת��
	BYTE SwitchToCardData(BYTE cbCardIndex)
	{
		ASSERT(cbCardIndex < MAX_INDEX);
		return ((cbCardIndex / 10) << 4) | (cbCardIndex % 10 + 1);
	}

	//�˿�ת��
	BYTE SwitchToCardIndex(BYTE cbCardData)
	{
		ASSERT(IsValidCard(cbCardData));
		return ((cbCardData & MASK_COLOR) >> 4) * 10 + (cbCardData & MASK_VALUE) - 1;
	}

	//�˿�ת��
	BYTE SwitchToCardData(BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[], BYTE bMaxCount)
	{
		//ת���˿�
		BYTE bPosition = 0;
		for (BYTE i = 0; i < MAX_INDEX; i++)
		{
			BYTE cbIndex = (i % 2) * 10 + i / 2;
			if (cbCardIndex[cbIndex] != 0)
			{
				for (BYTE j = 0; j < cbCardIndex[cbIndex]; j++)
				{
					ASSERT(bPosition < bMaxCount);
					cbCardData[bPosition++] = SwitchToCardData(cbIndex);
				}
			}
		}

		return bPosition;
	}

	//�˿�ת��
	BYTE SwitchToCardIndex(BYTE cbCardData[], BYTE bCardCount, BYTE cbCardIndex[MAX_INDEX])
	{
		//���ñ���
		ZeroMemory(cbCardIndex, sizeof(BYTE) * MAX_INDEX);

		//ת���˿�
		for (BYTE i = 0; i < bCardCount; i++)
		{
			ASSERT(IsValidCard(cbCardData[i]));
			cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
		}

		return bCardCount;
	}

	//�Ƿ�����
	bool IsWeiPengCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
	{
		//Ч���˿�
		ASSERT(cbCurrentCard != 0);
		if (cbCurrentCard == 0) return false;

		//ת������
		BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);

		//�����ж�
		return (cbCardIndex[cbCurrentIndex] == 2) ? true : false;
	}

	//��ȡ����
	BYTE TakeOutChiCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard, BYTE cbResultCard[3])
	{
		//Ч���˿�
		ASSERT(cbCurrentCard != 0);
		if (cbCurrentCard == 0) return 0;

		//��������
		BYTE cbFirstIndex = 0;
		BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);

		//��С���
		BYTE cbReverseIndex = (cbCurrentIndex + 10) % MAX_INDEX;
		if ((cbCardIndex[cbCurrentIndex] >= 2) && (cbCardIndex[cbReverseIndex] >= 1) && (cbCardIndex[cbReverseIndex] != 3))
		{
			//ɾ���˿�
			cbCardIndex[cbCurrentIndex]--;
			cbCardIndex[cbCurrentIndex]--;
			cbCardIndex[cbReverseIndex]--;

			//���ý��
			cbResultCard[0] = cbCurrentCard;
			cbResultCard[1] = cbCurrentCard;
			cbResultCard[2] = SwitchToCardData(cbReverseIndex);

			return ((cbCurrentCard & MASK_COLOR) == 0x00) ? CK_XXD : CK_XDD;
		}

		//��С���
		if (cbCardIndex[cbReverseIndex] == 2)
		{
			//ɾ���˿�
			cbCardIndex[cbCurrentIndex]--;
			cbCardIndex[cbReverseIndex] -= 2;

			//���ý��
			cbResultCard[0] = cbCurrentCard;
			cbResultCard[1] = SwitchToCardData(cbReverseIndex);
			cbResultCard[2] = SwitchToCardData(cbReverseIndex);

			return ((cbCurrentCard & MASK_COLOR) == 0x00) ? CK_XDD : CK_XXD;
		}

		//����ʮ��
		BYTE bCardValue = (cbCurrentCard & MASK_VALUE);
		if ((bCardValue == 0x02) || (bCardValue == 0x07) || (bCardValue == 0x0A))
		{
			//��������
			BYTE cbExcursion[] = { 1,6,9 };
			BYTE cbInceptIndex = ((cbCurrentCard & MASK_COLOR) == 0x00) ? 0 : 10;

			//�����ж�
			BYTE i = 0;
			for (; i < CountArray(cbExcursion); i++)
			{
				BYTE cbIndex = cbInceptIndex + cbExcursion[i];
				if ((cbCardIndex[cbIndex] == 0) || (cbCardIndex[cbIndex] == 3)) break;
			}

			//�ɹ��ж�
			if (i == CountArray(cbExcursion))
			{
				//ɾ���˿�
				cbCardIndex[cbInceptIndex + cbExcursion[0]]--;
				cbCardIndex[cbInceptIndex + cbExcursion[1]]--;
				cbCardIndex[cbInceptIndex + cbExcursion[2]]--;

				//���ý��
				cbResultCard[0] = SwitchToCardData(cbInceptIndex + cbExcursion[0]);
				cbResultCard[1] = SwitchToCardData(cbInceptIndex + cbExcursion[1]);
				cbResultCard[2] = SwitchToCardData(cbInceptIndex + cbExcursion[2]);

				return CK_EQS;
			}
		}

		//˳���ж�
		BYTE cbExcursion[3] = { 0,1,2 };
		for (BYTE i = 0; i < CountArray(cbExcursion); i++)
		{
			BYTE cbValueIndex = cbCurrentIndex % 10;
			if ((cbValueIndex >= cbExcursion[i]) && ((cbValueIndex - cbExcursion[i]) <= 7))
			{
				//�����ж�
				cbFirstIndex = cbCurrentIndex - cbExcursion[i];
				if ((cbCardIndex[cbFirstIndex] == 0) || (cbCardIndex[cbFirstIndex] == 3)) continue;
				if ((cbCardIndex[cbFirstIndex + 1] == 0) || (cbCardIndex[cbFirstIndex + 1] == 3)) continue;
				if ((cbCardIndex[cbFirstIndex + 2] == 0) || (cbCardIndex[cbFirstIndex + 2] == 3)) continue;

				//ɾ���˿�
				cbCardIndex[cbFirstIndex]--;
				cbCardIndex[cbFirstIndex + 1]--;
				cbCardIndex[cbFirstIndex + 2]--;

				//���ý��
				cbResultCard[0] = SwitchToCardData(cbFirstIndex);
				cbResultCard[1] = SwitchToCardData(cbFirstIndex + 1);
				cbResultCard[2] = SwitchToCardData(cbFirstIndex + 2);

				BYTE cbChiKind[3] = { CK_LEFT,CK_CENTER,CK_RIGHT };
				return cbChiKind[i];
			}
		}

		return CK_NULL;
	}

	//�����ж�
	BYTE GetActionChiCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard, tagChiCardInfo ChiCardInfo[6])
	{
		//Ч���˿�
		ASSERT(cbCurrentCard != 0);
		if (cbCurrentCard == 0) return 0;

		//��������
		BYTE cbChiCardCount = 0;
		BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);

		//�����ж�
		if (cbCardIndex[cbCurrentIndex] >= 3) return cbChiCardCount;

		//��С���
		BYTE cbReverseIndex = (cbCurrentIndex + 10) % MAX_INDEX;
		if ((cbCardIndex[cbCurrentIndex] >= 1) && (cbCardIndex[cbReverseIndex] >= 1) && (cbCardIndex[cbReverseIndex] != 3))
		{
			//�����˿�
			BYTE cbCardIndexTemp[MAX_INDEX];
			memcpy(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

			//ɾ���˿�
			cbCardIndexTemp[cbCurrentIndex]--;
			cbCardIndexTemp[cbReverseIndex]--;

			//��ȡ�ж�
			BYTE cbResultCount = 1;
			while (cbCardIndexTemp[cbCurrentIndex] > 0)
			{
				BYTE* pcbResult = ChiCardInfo[cbChiCardCount].cbCardData[cbResultCount];
				if (TakeOutChiCard(cbCardIndexTemp, cbCurrentCard, pcbResult) != CK_NULL) cbResultCount++;
				else break;
			}

			//���ý��
			if (cbCardIndexTemp[cbCurrentIndex] == 0)
			{
				ChiCardInfo[cbChiCardCount].cbCenterCard = cbCurrentCard;
				ChiCardInfo[cbChiCardCount].cbResultCount = cbResultCount;
				ChiCardInfo[cbChiCardCount].cbCardData[0][0] = cbCurrentCard;
				ChiCardInfo[cbChiCardCount].cbCardData[0][1] = cbCurrentCard;
				ChiCardInfo[cbChiCardCount].cbCardData[0][2] = SwitchToCardData(cbReverseIndex);
				ChiCardInfo[cbChiCardCount++].cbChiKind = ((cbCurrentCard & MASK_COLOR) == 0x00) ? CK_XXD : CK_XDD;
			}
		}

		//��С���
		if (cbCardIndex[cbReverseIndex] == 2)
		{
			//�����˿�
			BYTE cbCardIndexTemp[MAX_INDEX];
			memcpy(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

			//ɾ���˿�
			cbCardIndexTemp[cbReverseIndex] -= 2;

			//��ȡ�ж�
			BYTE cbResultCount = 1;
			while (cbCardIndexTemp[cbCurrentIndex] > 0)
			{
				BYTE* pcbResult = ChiCardInfo[cbChiCardCount].cbCardData[cbResultCount];
				if (TakeOutChiCard(cbCardIndexTemp, cbCurrentCard, pcbResult) != CK_NULL) cbResultCount++;
				else break;
			}

			//���ý��
			if (cbCardIndexTemp[cbCurrentIndex] == 0)
			{
				ChiCardInfo[cbChiCardCount].cbCenterCard = cbCurrentCard;
				ChiCardInfo[cbChiCardCount].cbResultCount = cbResultCount;
				ChiCardInfo[cbChiCardCount].cbCardData[0][0] = cbCurrentCard;
				ChiCardInfo[cbChiCardCount].cbCardData[0][1] = SwitchToCardData(cbReverseIndex);
				ChiCardInfo[cbChiCardCount].cbCardData[0][2] = SwitchToCardData(cbReverseIndex);
				ChiCardInfo[cbChiCardCount++].cbChiKind = ((cbCurrentCard & MASK_COLOR) == 0x00) ? CK_XDD : CK_XXD;
			}
		}

		//����ʮ��
		BYTE bCardValue = cbCurrentCard & MASK_VALUE;
		if ((bCardValue == 0x02) || (bCardValue == 0x07) || (bCardValue == 0x0A))
		{
			//��������
			BYTE cbExcursion[] = { 1,6,9 };
			BYTE cbInceptIndex = ((cbCurrentCard & MASK_COLOR) == 0x00) ? 0 : 10;

			//�����ж�
			BYTE i = 0;
			for (; i < CountArray(cbExcursion); i++)
			{
				BYTE cbIndex = cbInceptIndex + cbExcursion[i];
				if ((cbIndex != cbCurrentIndex) && ((cbCardIndex[cbIndex] == 0) || (cbCardIndex[cbIndex] == 3))) break;
			}

			//��ȡ�ж�
			if (i == CountArray(cbExcursion))
			{
				//�����˿�
				BYTE cbCardIndexTemp[MAX_INDEX];
				memcpy(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

				//ɾ���˿�
				for (BYTE j = 0; j < CountArray(cbExcursion); j++)
				{
					BYTE cbIndex = cbInceptIndex + cbExcursion[j];
					if (cbIndex != cbCurrentIndex) cbCardIndexTemp[cbIndex]--;
				}

				//��ȡ�ж�
				BYTE cbResultCount = 1;
				while (cbCardIndexTemp[cbCurrentIndex] > 0)
				{
					BYTE* pcbResult = ChiCardInfo[cbChiCardCount].cbCardData[cbResultCount];
					if (TakeOutChiCard(cbCardIndexTemp, cbCurrentCard, pcbResult) != CK_NULL) cbResultCount++;
					else break;
				}

				//���ý��
				if (cbCardIndexTemp[cbCurrentIndex] == 0)
				{
					ChiCardInfo[cbChiCardCount].cbChiKind = CK_EQS;
					ChiCardInfo[cbChiCardCount].cbCenterCard = cbCurrentCard;
					ChiCardInfo[cbChiCardCount].cbResultCount = cbResultCount;
					ChiCardInfo[cbChiCardCount].cbCardData[0][0] = SwitchToCardData(cbInceptIndex + cbExcursion[0]);
					ChiCardInfo[cbChiCardCount].cbCardData[0][1] = SwitchToCardData(cbInceptIndex + cbExcursion[1]);
					ChiCardInfo[cbChiCardCount++].cbCardData[0][2] = SwitchToCardData(cbInceptIndex + cbExcursion[2]);
				}
			}
		}

		//˳������
		BYTE cbExcursion[3] = { 0,1,2 };
		for (BYTE i = 0; i < CountArray(cbExcursion); i++)
		{
			BYTE cbValueIndex = cbCurrentIndex % 10;
			if ((cbValueIndex >= cbExcursion[i]) && ((cbValueIndex - cbExcursion[i]) <= 7))
			{
				//��������
				BYTE cbFirstIndex = cbCurrentIndex - cbExcursion[i];

				//�����ж�
				BYTE j = 0;
				for (; j < 3; j++)
				{
					BYTE cbIndex = cbFirstIndex + j;
					if ((cbIndex != cbCurrentIndex) && ((cbCardIndex[cbIndex] == 0) || (cbCardIndex[cbIndex] == 3))) break;
				}

				//��ȡ�ж�
				if (j == CountArray(cbExcursion))
				{
					//�����˿�
					BYTE cbCardIndexTemp[MAX_INDEX];
					memcpy(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

					//ɾ���˿�
					for (BYTE j = 0; j < 3; j++)
					{
						BYTE cbIndex = cbFirstIndex + j;
						if (cbIndex != cbCurrentIndex) cbCardIndexTemp[cbIndex]--;
					}

					//��ȡ�ж�
					BYTE cbResultCount = 1;
					while (cbCardIndexTemp[cbCurrentIndex] > 0)
					{
						BYTE* pcbResult = ChiCardInfo[cbChiCardCount].cbCardData[cbResultCount];
						if (TakeOutChiCard(cbCardIndexTemp, cbCurrentCard, pcbResult) != CK_NULL) cbResultCount++;
						else break;
					}

					//���ý��
					if (cbCardIndexTemp[cbCurrentIndex] == 0)
					{
						BYTE cbChiKind[3] = { CK_LEFT,CK_CENTER,CK_RIGHT };
						ChiCardInfo[cbChiCardCount].cbChiKind = cbChiKind[i];
						ChiCardInfo[cbChiCardCount].cbCenterCard = cbCurrentCard;
						ChiCardInfo[cbChiCardCount].cbResultCount = cbResultCount;
						ChiCardInfo[cbChiCardCount].cbCardData[0][0] = SwitchToCardData(cbFirstIndex);
						ChiCardInfo[cbChiCardCount].cbCardData[0][1] = SwitchToCardData(cbFirstIndex + 1);
						ChiCardInfo[cbChiCardCount++].cbCardData[0][2] = SwitchToCardData(cbFirstIndex + 2);
					}
				}
			}
		}

		return cbChiCardCount;
	}


	//��ȡ��Ϣ
	BYTE GetWeaveHuXi(const tagWeaveItem& WeaveItem)
	{
		//�����Ϣ
		switch (WeaveItem.cbWeaveKind)
		{
		case ACK_TI:	//��
		{
			return ((WeaveItem.cbCardList[0] & MASK_COLOR) == 0x10) ? 12 : 9;
		}
		case ACK_PAO:	//��
		{
			return ((WeaveItem.cbCardList[0] & MASK_COLOR) == 0x10) ? 9 : 6;
		}
		case ACK_WEI:	//��
		{
			return ((WeaveItem.cbCardList[0] & MASK_COLOR) == 0x10) ? 6 : 3;
		}
		case ACK_PENG:	//��
		{
			return ((WeaveItem.cbCardList[0] & MASK_COLOR) == 0x10) ? 3 : 1;
		}
		case ACK_CHI:	//��
		{
			//��ȡ��ֵ
			BYTE cbValue1 = WeaveItem.cbCardList[0] & MASK_VALUE;
			BYTE cbValue2 = WeaveItem.cbCardList[1] & MASK_VALUE;
			BYTE cbValue3 = WeaveItem.cbCardList[2] & MASK_VALUE;

			//һ������
			if ((cbValue1 == 1) && (cbValue2 == 2) && (cbValue3 == 3)) return ((WeaveItem.cbCardList[0] & MASK_COLOR) == 0x10) ? 6 : 3;

			//����ʮ��
			if ((cbValue1 == 2) && (cbValue2 == 7) && (cbValue3 == 10)) return ((WeaveItem.cbCardList[0] & MASK_COLOR) == 0x10) ? 6 : 3;

			return 0;
		}
		}

		return 0;
	}

	//�����˿�
	bool AnalyseCard(BYTE cbCardIndex[MAX_INDEX], std::vector<tagAnalyseItem>& AnalyseItemArray)
	{
		//��������
		BYTE cbWeaveItemCount = 0;
		tagWeaveItem WeaveItem[76];
		ZeroMemory(WeaveItem, sizeof(WeaveItem));

		//��Ŀͳ��
		BYTE cbCardCount = 0;
		for (BYTE i = 0; i < MAX_INDEX; i++) cbCardCount += cbCardIndex[i];

		//��Ŀ�ж�
		if (cbCardCount == 0) return true;
		if ((cbCardCount % 3 != 0) && ((cbCardCount + 1) % 3 != 0)) return false;

		//�������
		BYTE bLessWeavItem = cbCardCount / 3;
		bool bNeedCardEye = ((cbCardCount + 1) % 3 == 0);

		//�����ж�
		if ((bLessWeavItem == 0) && (bNeedCardEye == true))
		{
			//�����ж�
			for (BYTE i = 0; i < MAX_INDEX; i++)
			{
				if (cbCardIndex[i] == 2)
				{
					//��������
					tagAnalyseItem AnalyseItem;
					ZeroMemory(&AnalyseItem, sizeof(AnalyseItem));

					//���ý��
					AnalyseItem.cbHuXiCount = 0;
					AnalyseItem.cbWeaveCount = 0;
					AnalyseItem.cbCardEye = SwitchToCardData(i);

					//������
					AnalyseItemArray.push_back(AnalyseItem);

					return true;
				}
			}

			return false;
		}

		//��ִ���
		for (BYTE i = 0; i < MAX_INDEX; i++)
		{
			//��������
			if (cbCardIndex[i] == 0) continue;

			//��������
			BYTE cbCardData = SwitchToCardData(i);

			//��С���
			if ((cbCardIndex[i] == 2) && (cbCardIndex[(i + 10) % MAX_INDEX] >= 1))
			{
				WeaveItem[cbWeaveItemCount].cbCardCount = 3;
				WeaveItem[cbWeaveItemCount].cbWeaveKind = ACK_CHI;
				WeaveItem[cbWeaveItemCount].cbCenterCard = cbCardData;
				WeaveItem[cbWeaveItemCount].cbCardList[0] = cbCardData;
				WeaveItem[cbWeaveItemCount].cbCardList[1] = cbCardData;
				WeaveItem[cbWeaveItemCount++].cbCardList[2] = (cbCardData + 16) % 32;
			}

			//��С���
			if ((cbCardIndex[i] >= 1) && (cbCardIndex[(i + 10) % MAX_INDEX] == 2))
			{
				WeaveItem[cbWeaveItemCount].cbCardCount = 3;
				WeaveItem[cbWeaveItemCount].cbWeaveKind = ACK_CHI;
				WeaveItem[cbWeaveItemCount].cbCenterCard = cbCardData;
				WeaveItem[cbWeaveItemCount].cbCardList[0] = cbCardData;
				WeaveItem[cbWeaveItemCount].cbCardList[1] = (cbCardData + 16) % 32;
				WeaveItem[cbWeaveItemCount++].cbCardList[2] = (cbCardData + 16) % 32;
			}

			//����ʮ��
			if ((cbCardData & MASK_VALUE) == 0x02)
			{
				for (BYTE j = 1; j <= cbCardIndex[i]; j++)
				{
					if ((cbCardIndex[i + 5] >= j) && (cbCardIndex[i + 8] >= j))
					{
						WeaveItem[cbWeaveItemCount].cbCardCount = 3;
						WeaveItem[cbWeaveItemCount].cbWeaveKind = ACK_CHI;
						WeaveItem[cbWeaveItemCount].cbCenterCard = cbCardData;
						WeaveItem[cbWeaveItemCount].cbCardList[0] = cbCardData;
						WeaveItem[cbWeaveItemCount].cbCardList[1] = cbCardData + 5;
						WeaveItem[cbWeaveItemCount++].cbCardList[2] = cbCardData + 8;
					}
				}
			}

			//˳���ж�
			if ((i < (MAX_INDEX - 2)) && (cbCardIndex[i] > 0) && ((i % 10) <= 7))
			{
				for (BYTE j = 1; j <= cbCardIndex[i]; j++)
				{
					if ((cbCardIndex[i + 1] >= j) && (cbCardIndex[i + 2] >= j))
					{
						WeaveItem[cbWeaveItemCount].cbCardCount = 3;
						WeaveItem[cbWeaveItemCount].cbWeaveKind = ACK_CHI;
						WeaveItem[cbWeaveItemCount].cbCenterCard = cbCardData;
						WeaveItem[cbWeaveItemCount].cbCardList[0] = cbCardData;
						WeaveItem[cbWeaveItemCount].cbCardList[1] = cbCardData + 1;
						WeaveItem[cbWeaveItemCount++].cbCardList[2] = cbCardData + 2;
					}
				}
			}
		}

		//��Ϸ���
		if (cbWeaveItemCount >= bLessWeavItem)
		{
			//��������
			BYTE cbCardIndexTemp[MAX_INDEX];
			ZeroMemory(cbCardIndexTemp, sizeof(cbCardIndexTemp));

			//��������
			BYTE cbIndex[MAX_WEAVE] = { 0,1,2,3,4,5,6 };
			tagWeaveItem* pWeaveItem[CountArray(cbIndex)];

			//��ʼ���
			do
			{
				//���ñ���
				memcpy(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));
				for (BYTE i = 0; i < bLessWeavItem; i++) pWeaveItem[i] = &WeaveItem[cbIndex[i]];

				//�����ж�
				bool bEnoughCard = true;
				for (BYTE i = 0; i < bLessWeavItem * 3; i++)
				{
					//�����ж�
					BYTE cbIndex = SwitchToCardIndex(pWeaveItem[i / 3]->cbCardList[i % 3]);
					if (cbCardIndexTemp[cbIndex] == 0)
					{
						bEnoughCard = false;
						break;
					}
					else cbCardIndexTemp[cbIndex]--;
				}

				//�����ж�
				if (bEnoughCard == true)
				{
					//�����ж�
					BYTE cbCardEye = 0;
					if (bNeedCardEye == true)
					{
						for (BYTE i = 0; i < MAX_INDEX; i++)
						{
							if (cbCardIndexTemp[i] == 2)
							{
								cbCardEye = SwitchToCardData(i);
								break;
							}
						}
					}

					//�������
					if ((bNeedCardEye == false) || (cbCardEye != 0))
					{
						//��������
						tagAnalyseItem AnalyseItem;
						ZeroMemory(&AnalyseItem, sizeof(AnalyseItem));

						//���ý��
						AnalyseItem.cbHuXiCount = 0;
						AnalyseItem.cbCardEye = cbCardEye;
						AnalyseItem.cbWeaveCount = bLessWeavItem;

						//�������
						for (BYTE i = 0; i < bLessWeavItem; i++)
						{
							AnalyseItem.WeaveItemArray[i] = *pWeaveItem[i];
							AnalyseItem.cbHuXiCount += GetWeaveHuXi(AnalyseItem.WeaveItemArray[i]);
						}

						//������
						AnalyseItemArray.push_back(AnalyseItem);
					}
				}

				//��������
				if (cbIndex[bLessWeavItem - 1] == (cbWeaveItemCount - 1))
				{
					BYTE i = bLessWeavItem - 1;
					for (; i > 0; i--)
					{
						if ((cbIndex[i - 1] + 1) != cbIndex[i])
						{
							BYTE cbNewIndex = cbIndex[i - 1];
							for (BYTE j = (i - 1); j < bLessWeavItem; j++) cbIndex[j] = cbNewIndex + j - i + 2;
							break;
						}
					}
					if (i == 0) break;
				}
				else cbIndex[bLessWeavItem - 1]++;

			} while (true);

			return (AnalyseItemArray.size() > 0);
		}

		return false;
	}


	//���ƽ��
	bool GetHuCardInfo(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard, BYTE cbHuXiWeave, tagHuCardInfo& HuCardInfo)
	{
		//��������
		//static CAnalyseItemArray AnalyseItemArray;
		std::vector<tagAnalyseItem> AnalyseItemArray;

		//�����˿�
		BYTE cbCardIndexTemp[MAX_INDEX];
		memcpy(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

		//���ý��
		ZeroMemory(&HuCardInfo, sizeof(HuCardInfo));

		//��ȡ����
		if ((cbCurrentCard != 0) && (IsWeiPengCard(cbCardIndexTemp, cbCurrentCard) == true))
		{

			//�жϺ���	
			cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;
			AnalyseItemArray.clear();
			if (AnalyseCard(cbCardIndexTemp, AnalyseItemArray) == true)
			{
				//Ѱ������
				BYTE cbHuXiCard = 0;
				int nBestItem = -1;
				for (int i = 0; i < (int)AnalyseItemArray.size(); i++)
				{
					//��ȡ����
					tagAnalyseItem* pAnalyseItem = &AnalyseItemArray[i];

					//��Ϣ����
					if (pAnalyseItem->cbHuXiCount >= cbHuXiCard)
					{
						nBestItem = i;
						cbHuXiCard = pAnalyseItem->cbHuXiCount;
					}
				}
				HuCardInfo.cbHuXiCount += cbHuXiCard;

				//���ý��
				if (nBestItem >= 0)
				{
					//��ȡ����
					tagAnalyseItem* pAnalyseItem = &AnalyseItemArray[nBestItem];

					//���ñ���
					HuCardInfo.cbCardEye = pAnalyseItem->cbCardEye;

					//�������
					for (BYTE i = 0; i < pAnalyseItem->cbWeaveCount; i++)
					{
						BYTE cbIndex = HuCardInfo.cbWeaveCount++;
						HuCardInfo.WeaveItemArray[cbIndex] = pAnalyseItem->WeaveItemArray[i];
					}
				}

				if ((HuCardInfo.cbHuXiCount + cbHuXiWeave) >= 15)
					return true;
			}

			//�������
			BYTE cbIndex = HuCardInfo.cbWeaveCount++;
			HuCardInfo.WeaveItemArray[cbIndex].cbCardCount = 3;
			HuCardInfo.WeaveItemArray[cbIndex].cbWeaveKind = ACK_PENG;
			HuCardInfo.WeaveItemArray[cbIndex].cbCenterCard = cbCurrentCard;
			HuCardInfo.WeaveItemArray[cbIndex].cbCardList[0] = cbCurrentCard;
			HuCardInfo.WeaveItemArray[cbIndex].cbCardList[1] = cbCurrentCard;
			HuCardInfo.WeaveItemArray[cbIndex].cbCardList[2] = cbCurrentCard;

			//ɾ���˿�
			cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)] = 0;

			//���ú�Ϣ
			HuCardInfo.cbHuXiCount += GetWeaveHuXi(HuCardInfo.WeaveItemArray[cbIndex]);
		}
		else if (cbCurrentCard != 0) cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;

		//��ȡ����
		for (BYTE i = 0; i < MAX_INDEX; i++)
		{
			if (cbCardIndexTemp[i] == 3)
			{
				//�����˿�
				cbCardIndexTemp[i] = 0;

				//�������
				BYTE cbCardData = SwitchToCardData(i);
				BYTE cbIndex = HuCardInfo.cbWeaveCount++;
				HuCardInfo.WeaveItemArray[cbIndex].cbCardCount = 3;
				HuCardInfo.WeaveItemArray[cbIndex].cbWeaveKind = ACK_WEI;
				HuCardInfo.WeaveItemArray[cbIndex].cbCenterCard = cbCardData;
				HuCardInfo.WeaveItemArray[cbIndex].cbCardList[0] = cbCardData;
				HuCardInfo.WeaveItemArray[cbIndex].cbCardList[1] = cbCardData;
				HuCardInfo.WeaveItemArray[cbIndex].cbCardList[2] = cbCardData;

				//���ú�Ϣ
				HuCardInfo.cbHuXiCount += GetWeaveHuXi(HuCardInfo.WeaveItemArray[cbIndex]);
			}
		}

		//�����˿�
		AnalyseItemArray.clear();
		if (AnalyseCard(cbCardIndexTemp, AnalyseItemArray) == false) return false;

		//Ѱ������
		BYTE cbHuXiCard = 0;
		int nBestItem = -1;
		for (int i = 0; i < (int)AnalyseItemArray.size(); i++)
		{
			//��ȡ����
			tagAnalyseItem* pAnalyseItem = &AnalyseItemArray[i];

			//��Ϣ����
			if (pAnalyseItem->cbHuXiCount >= cbHuXiCard)
			{
				nBestItem = i;
				cbHuXiCard = pAnalyseItem->cbHuXiCount;
			}
		}
		HuCardInfo.cbHuXiCount += cbHuXiCard;

		//���ý��
		if (nBestItem >= 0)
		{
			//��ȡ����
			tagAnalyseItem* pAnalyseItem = &AnalyseItemArray[nBestItem];

			//���ñ���
			HuCardInfo.cbCardEye = pAnalyseItem->cbCardEye;

			//�������
			for (BYTE i = 0; i < pAnalyseItem->cbWeaveCount; i++)
			{
				BYTE cbIndex = HuCardInfo.cbWeaveCount++;
				HuCardInfo.WeaveItemArray[cbIndex] = pAnalyseItem->WeaveItemArray[i];
			}
		}

		return ((HuCardInfo.cbHuXiCount + cbHuXiWeave) >= 15);
	}


	void TestActionChiCard()
	{
		//��ȡ����
		tagChiCardInfo ChiCardInfo[6];
		BYTE m_cbCardIndex[MAX_INDEX] = { 0 };
		BYTE m_cbOutCardData = 0x12;
		BYTE cbChiCardCount = GetActionChiCard(m_cbCardIndex, m_cbOutCardData, ChiCardInfo);

		tagHuCardInfo HuCardInfo;
		BYTE cbCenterCard = 0x12;
		BYTE m_bUserHuXiCount = 0x12;

		bool bIsHuCard = GetHuCardInfo(m_cbCardIndex, cbCenterCard, m_bUserHuXiCount, HuCardInfo);

		printf("cbChiCardCount:%d,bIsHuCard:%d\n", cbChiCardCount, bIsHuCard);

	}

};