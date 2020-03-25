
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

	//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

	typedef unsigned char		BYTE;



#define PAOHUZI_GAME_PLAYER			3									//游戏人数

	//数值定义
#define MAX_WEAVE					7									//最大组合
#define MAX_INDEX					20									//最大索引
#define MAX_COUNT					21									//最大数目
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//动作定义
#define ACK_NULL					0x00								//空
#define ACK_TI						0x01								//提
#define ACK_PAO						0x02								//跑
#define ACK_WEI						0x04								//偎
#define ACK_CHI						0x08								//吃
#define ACK_CHI_EX					0x10								//吃
#define ACK_PENG					0x20								//碰
#define ACK_CHIHU					0x40								//胡

//吃牌类型
#define CK_NULL						0x00								//无效类型
#define CK_XXD						0x01								//小小大搭
#define CK_XDD						0x02								//小大大搭
#define CK_EQS						0x04								//二七十吃
#define CK_LEFT						0x10								//靠左对齐
#define CK_CENTER					0x20								//居中对齐
#define CK_RIGHT					0x40								//靠右对齐

//////////////////////////////////////////////////////////////////////////

//组合子项
	struct tagWeaveItem
	{
		BYTE							cbWeaveKind;						//组合类型
		BYTE							cbCardCount;						//扑克数目
		BYTE							cbCenterCard;						//中心扑克
		BYTE							cbCardList[4];						//扑克列表
	};

	//吃牌信息
	struct tagChiCardInfo
	{
		BYTE							cbChiKind;							//吃牌类型
		BYTE							cbCenterCard;						//中心扑克
		BYTE							cbResultCount;						//结果数目
		BYTE							cbCardData[3][3];					//吃牌组合
		BYTE							cbChiCardData[3];					//吃牌数据(根据吃牌类型和中心牌)
	};

	//分析子项
	struct tagAnalyseItem
	{
		BYTE							cbCardEye;							//牌眼扑克
		BYTE							cbHuXiCount;						//胡息数目
		BYTE							cbWeaveCount;						//组合数目
		tagWeaveItem					WeaveItemArray[MAX_WEAVE];			//组合扑克
	};

	//胡牌信息
	struct tagHuCardInfo
	{
		BYTE							cbCardEye;							//牌眼扑克
		BYTE							cbHuXiCount;						//胡息数目
		BYTE							cbWeaveCount;						//组合数目
		tagWeaveItem					WeaveItemArray[MAX_WEAVE];			//组合扑克
	};


	//有效判断
	bool IsValidCard(BYTE cbCardData)
	{
		BYTE cbValue = (cbCardData & MASK_VALUE);
		BYTE cbColor = (cbCardData & MASK_COLOR) >> 4;
		return ((cbValue >= 1) && (cbValue <= 10) && (cbColor <= 1));
	}

	//扑克转换
	BYTE SwitchToCardData(BYTE cbCardIndex)
	{
		ASSERT(cbCardIndex < MAX_INDEX);
		return ((cbCardIndex / 10) << 4) | (cbCardIndex % 10 + 1);
	}

	//扑克转换
	BYTE SwitchToCardIndex(BYTE cbCardData)
	{
		ASSERT(IsValidCard(cbCardData));
		return ((cbCardData & MASK_COLOR) >> 4) * 10 + (cbCardData & MASK_VALUE) - 1;
	}

	//扑克转换
	BYTE SwitchToCardData(BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[], BYTE bMaxCount)
	{
		//转换扑克
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

	//扑克转换
	BYTE SwitchToCardIndex(BYTE cbCardData[], BYTE bCardCount, BYTE cbCardIndex[MAX_INDEX])
	{
		//设置变量
		ZeroMemory(cbCardIndex, sizeof(BYTE) * MAX_INDEX);

		//转换扑克
		for (BYTE i = 0; i < bCardCount; i++)
		{
			ASSERT(IsValidCard(cbCardData[i]));
			cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
		}

		return bCardCount;
	}

	//是否偎碰
	bool IsWeiPengCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
	{
		//效验扑克
		ASSERT(cbCurrentCard != 0);
		if (cbCurrentCard == 0) return false;

		//转换索引
		BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);

		//跑偎判断
		return (cbCardIndex[cbCurrentIndex] == 2) ? true : false;
	}

	//提取吃牌
	BYTE TakeOutChiCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard, BYTE cbResultCard[3])
	{
		//效验扑克
		ASSERT(cbCurrentCard != 0);
		if (cbCurrentCard == 0) return 0;

		//变量定义
		BYTE cbFirstIndex = 0;
		BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);

		//大小搭吃
		BYTE cbReverseIndex = (cbCurrentIndex + 10) % MAX_INDEX;
		if ((cbCardIndex[cbCurrentIndex] >= 2) && (cbCardIndex[cbReverseIndex] >= 1) && (cbCardIndex[cbReverseIndex] != 3))
		{
			//删除扑克
			cbCardIndex[cbCurrentIndex]--;
			cbCardIndex[cbCurrentIndex]--;
			cbCardIndex[cbReverseIndex]--;

			//设置结果
			cbResultCard[0] = cbCurrentCard;
			cbResultCard[1] = cbCurrentCard;
			cbResultCard[2] = SwitchToCardData(cbReverseIndex);

			return ((cbCurrentCard & MASK_COLOR) == 0x00) ? CK_XXD : CK_XDD;
		}

		//大小搭吃
		if (cbCardIndex[cbReverseIndex] == 2)
		{
			//删除扑克
			cbCardIndex[cbCurrentIndex]--;
			cbCardIndex[cbReverseIndex] -= 2;

			//设置结果
			cbResultCard[0] = cbCurrentCard;
			cbResultCard[1] = SwitchToCardData(cbReverseIndex);
			cbResultCard[2] = SwitchToCardData(cbReverseIndex);

			return ((cbCurrentCard & MASK_COLOR) == 0x00) ? CK_XDD : CK_XXD;
		}

		//二七十吃
		BYTE bCardValue = (cbCurrentCard & MASK_VALUE);
		if ((bCardValue == 0x02) || (bCardValue == 0x07) || (bCardValue == 0x0A))
		{
			//变量定义
			BYTE cbExcursion[] = { 1,6,9 };
			BYTE cbInceptIndex = ((cbCurrentCard & MASK_COLOR) == 0x00) ? 0 : 10;

			//类型判断
			BYTE i = 0;
			for (; i < CountArray(cbExcursion); i++)
			{
				BYTE cbIndex = cbInceptIndex + cbExcursion[i];
				if ((cbCardIndex[cbIndex] == 0) || (cbCardIndex[cbIndex] == 3)) break;
			}

			//成功判断
			if (i == CountArray(cbExcursion))
			{
				//删除扑克
				cbCardIndex[cbInceptIndex + cbExcursion[0]]--;
				cbCardIndex[cbInceptIndex + cbExcursion[1]]--;
				cbCardIndex[cbInceptIndex + cbExcursion[2]]--;

				//设置结果
				cbResultCard[0] = SwitchToCardData(cbInceptIndex + cbExcursion[0]);
				cbResultCard[1] = SwitchToCardData(cbInceptIndex + cbExcursion[1]);
				cbResultCard[2] = SwitchToCardData(cbInceptIndex + cbExcursion[2]);

				return CK_EQS;
			}
		}

		//顺子判断
		BYTE cbExcursion[3] = { 0,1,2 };
		for (BYTE i = 0; i < CountArray(cbExcursion); i++)
		{
			BYTE cbValueIndex = cbCurrentIndex % 10;
			if ((cbValueIndex >= cbExcursion[i]) && ((cbValueIndex - cbExcursion[i]) <= 7))
			{
				//吃牌判断
				cbFirstIndex = cbCurrentIndex - cbExcursion[i];
				if ((cbCardIndex[cbFirstIndex] == 0) || (cbCardIndex[cbFirstIndex] == 3)) continue;
				if ((cbCardIndex[cbFirstIndex + 1] == 0) || (cbCardIndex[cbFirstIndex + 1] == 3)) continue;
				if ((cbCardIndex[cbFirstIndex + 2] == 0) || (cbCardIndex[cbFirstIndex + 2] == 3)) continue;

				//删除扑克
				cbCardIndex[cbFirstIndex]--;
				cbCardIndex[cbFirstIndex + 1]--;
				cbCardIndex[cbFirstIndex + 2]--;

				//设置结果
				cbResultCard[0] = SwitchToCardData(cbFirstIndex);
				cbResultCard[1] = SwitchToCardData(cbFirstIndex + 1);
				cbResultCard[2] = SwitchToCardData(cbFirstIndex + 2);

				BYTE cbChiKind[3] = { CK_LEFT,CK_CENTER,CK_RIGHT };
				return cbChiKind[i];
			}
		}

		return CK_NULL;
	}

	//吃牌判断
	BYTE GetActionChiCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard, tagChiCardInfo ChiCardInfo[6])
	{
		//效验扑克
		ASSERT(cbCurrentCard != 0);
		if (cbCurrentCard == 0) return 0;

		//变量定义
		BYTE cbChiCardCount = 0;
		BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);

		//三牌判断
		if (cbCardIndex[cbCurrentIndex] >= 3) return cbChiCardCount;

		//大小搭吃
		BYTE cbReverseIndex = (cbCurrentIndex + 10) % MAX_INDEX;
		if ((cbCardIndex[cbCurrentIndex] >= 1) && (cbCardIndex[cbReverseIndex] >= 1) && (cbCardIndex[cbReverseIndex] != 3))
		{
			//构造扑克
			BYTE cbCardIndexTemp[MAX_INDEX];
			memcpy(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

			//删除扑克
			cbCardIndexTemp[cbCurrentIndex]--;
			cbCardIndexTemp[cbReverseIndex]--;

			//提取判断
			BYTE cbResultCount = 1;
			while (cbCardIndexTemp[cbCurrentIndex] > 0)
			{
				BYTE* pcbResult = ChiCardInfo[cbChiCardCount].cbCardData[cbResultCount];
				if (TakeOutChiCard(cbCardIndexTemp, cbCurrentCard, pcbResult) != CK_NULL) cbResultCount++;
				else break;
			}

			//设置结果
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

		//大小搭吃
		if (cbCardIndex[cbReverseIndex] == 2)
		{
			//构造扑克
			BYTE cbCardIndexTemp[MAX_INDEX];
			memcpy(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

			//删除扑克
			cbCardIndexTemp[cbReverseIndex] -= 2;

			//提取判断
			BYTE cbResultCount = 1;
			while (cbCardIndexTemp[cbCurrentIndex] > 0)
			{
				BYTE* pcbResult = ChiCardInfo[cbChiCardCount].cbCardData[cbResultCount];
				if (TakeOutChiCard(cbCardIndexTemp, cbCurrentCard, pcbResult) != CK_NULL) cbResultCount++;
				else break;
			}

			//设置结果
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

		//二七十吃
		BYTE bCardValue = cbCurrentCard & MASK_VALUE;
		if ((bCardValue == 0x02) || (bCardValue == 0x07) || (bCardValue == 0x0A))
		{
			//变量定义
			BYTE cbExcursion[] = { 1,6,9 };
			BYTE cbInceptIndex = ((cbCurrentCard & MASK_COLOR) == 0x00) ? 0 : 10;

			//类型判断
			BYTE i = 0;
			for (; i < CountArray(cbExcursion); i++)
			{
				BYTE cbIndex = cbInceptIndex + cbExcursion[i];
				if ((cbIndex != cbCurrentIndex) && ((cbCardIndex[cbIndex] == 0) || (cbCardIndex[cbIndex] == 3))) break;
			}

			//提取判断
			if (i == CountArray(cbExcursion))
			{
				//构造扑克
				BYTE cbCardIndexTemp[MAX_INDEX];
				memcpy(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

				//删除扑克
				for (BYTE j = 0; j < CountArray(cbExcursion); j++)
				{
					BYTE cbIndex = cbInceptIndex + cbExcursion[j];
					if (cbIndex != cbCurrentIndex) cbCardIndexTemp[cbIndex]--;
				}

				//提取判断
				BYTE cbResultCount = 1;
				while (cbCardIndexTemp[cbCurrentIndex] > 0)
				{
					BYTE* pcbResult = ChiCardInfo[cbChiCardCount].cbCardData[cbResultCount];
					if (TakeOutChiCard(cbCardIndexTemp, cbCurrentCard, pcbResult) != CK_NULL) cbResultCount++;
					else break;
				}

				//设置结果
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

		//顺子类型
		BYTE cbExcursion[3] = { 0,1,2 };
		for (BYTE i = 0; i < CountArray(cbExcursion); i++)
		{
			BYTE cbValueIndex = cbCurrentIndex % 10;
			if ((cbValueIndex >= cbExcursion[i]) && ((cbValueIndex - cbExcursion[i]) <= 7))
			{
				//索引定义
				BYTE cbFirstIndex = cbCurrentIndex - cbExcursion[i];

				//吃牌判断
				BYTE j = 0;
				for (; j < 3; j++)
				{
					BYTE cbIndex = cbFirstIndex + j;
					if ((cbIndex != cbCurrentIndex) && ((cbCardIndex[cbIndex] == 0) || (cbCardIndex[cbIndex] == 3))) break;
				}

				//提取判断
				if (j == CountArray(cbExcursion))
				{
					//构造扑克
					BYTE cbCardIndexTemp[MAX_INDEX];
					memcpy(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

					//删除扑克
					for (BYTE j = 0; j < 3; j++)
					{
						BYTE cbIndex = cbFirstIndex + j;
						if (cbIndex != cbCurrentIndex) cbCardIndexTemp[cbIndex]--;
					}

					//提取判断
					BYTE cbResultCount = 1;
					while (cbCardIndexTemp[cbCurrentIndex] > 0)
					{
						BYTE* pcbResult = ChiCardInfo[cbChiCardCount].cbCardData[cbResultCount];
						if (TakeOutChiCard(cbCardIndexTemp, cbCurrentCard, pcbResult) != CK_NULL) cbResultCount++;
						else break;
					}

					//设置结果
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


	//获取胡息
	BYTE GetWeaveHuXi(const tagWeaveItem& WeaveItem)
	{
		//计算胡息
		switch (WeaveItem.cbWeaveKind)
		{
		case ACK_TI:	//提
		{
			return ((WeaveItem.cbCardList[0] & MASK_COLOR) == 0x10) ? 12 : 9;
		}
		case ACK_PAO:	//跑
		{
			return ((WeaveItem.cbCardList[0] & MASK_COLOR) == 0x10) ? 9 : 6;
		}
		case ACK_WEI:	//偎
		{
			return ((WeaveItem.cbCardList[0] & MASK_COLOR) == 0x10) ? 6 : 3;
		}
		case ACK_PENG:	//碰
		{
			return ((WeaveItem.cbCardList[0] & MASK_COLOR) == 0x10) ? 3 : 1;
		}
		case ACK_CHI:	//吃
		{
			//获取数值
			BYTE cbValue1 = WeaveItem.cbCardList[0] & MASK_VALUE;
			BYTE cbValue2 = WeaveItem.cbCardList[1] & MASK_VALUE;
			BYTE cbValue3 = WeaveItem.cbCardList[2] & MASK_VALUE;

			//一二三吃
			if ((cbValue1 == 1) && (cbValue2 == 2) && (cbValue3 == 3)) return ((WeaveItem.cbCardList[0] & MASK_COLOR) == 0x10) ? 6 : 3;

			//二七十吃
			if ((cbValue1 == 2) && (cbValue2 == 7) && (cbValue3 == 10)) return ((WeaveItem.cbCardList[0] & MASK_COLOR) == 0x10) ? 6 : 3;

			return 0;
		}
		}

		return 0;
	}

	//分析扑克
	bool AnalyseCard(BYTE cbCardIndex[MAX_INDEX], std::vector<tagAnalyseItem>& AnalyseItemArray)
	{
		//变量定义
		BYTE cbWeaveItemCount = 0;
		tagWeaveItem WeaveItem[76];
		ZeroMemory(WeaveItem, sizeof(WeaveItem));

		//数目统计
		BYTE cbCardCount = 0;
		for (BYTE i = 0; i < MAX_INDEX; i++) cbCardCount += cbCardIndex[i];

		//数目判断
		if (cbCardCount == 0) return true;
		if ((cbCardCount % 3 != 0) && ((cbCardCount + 1) % 3 != 0)) return false;

		//需求计算
		BYTE bLessWeavItem = cbCardCount / 3;
		bool bNeedCardEye = ((cbCardCount + 1) % 3 == 0);

		//单吊判断
		if ((bLessWeavItem == 0) && (bNeedCardEye == true))
		{
			//牌眼判断
			for (BYTE i = 0; i < MAX_INDEX; i++)
			{
				if (cbCardIndex[i] == 2)
				{
					//变量定义
					tagAnalyseItem AnalyseItem;
					ZeroMemory(&AnalyseItem, sizeof(AnalyseItem));

					//设置结果
					AnalyseItem.cbHuXiCount = 0;
					AnalyseItem.cbWeaveCount = 0;
					AnalyseItem.cbCardEye = SwitchToCardData(i);

					//插入结果
					AnalyseItemArray.push_back(AnalyseItem);

					return true;
				}
			}

			return false;
		}

		//拆分处理
		for (BYTE i = 0; i < MAX_INDEX; i++)
		{
			//分析过虑
			if (cbCardIndex[i] == 0) continue;

			//变量定义
			BYTE cbCardData = SwitchToCardData(i);

			//大小搭吃
			if ((cbCardIndex[i] == 2) && (cbCardIndex[(i + 10) % MAX_INDEX] >= 1))
			{
				WeaveItem[cbWeaveItemCount].cbCardCount = 3;
				WeaveItem[cbWeaveItemCount].cbWeaveKind = ACK_CHI;
				WeaveItem[cbWeaveItemCount].cbCenterCard = cbCardData;
				WeaveItem[cbWeaveItemCount].cbCardList[0] = cbCardData;
				WeaveItem[cbWeaveItemCount].cbCardList[1] = cbCardData;
				WeaveItem[cbWeaveItemCount++].cbCardList[2] = (cbCardData + 16) % 32;
			}

			//大小搭吃
			if ((cbCardIndex[i] >= 1) && (cbCardIndex[(i + 10) % MAX_INDEX] == 2))
			{
				WeaveItem[cbWeaveItemCount].cbCardCount = 3;
				WeaveItem[cbWeaveItemCount].cbWeaveKind = ACK_CHI;
				WeaveItem[cbWeaveItemCount].cbCenterCard = cbCardData;
				WeaveItem[cbWeaveItemCount].cbCardList[0] = cbCardData;
				WeaveItem[cbWeaveItemCount].cbCardList[1] = (cbCardData + 16) % 32;
				WeaveItem[cbWeaveItemCount++].cbCardList[2] = (cbCardData + 16) % 32;
			}

			//二七十吃
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

			//顺子判断
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

		//组合分析
		if (cbWeaveItemCount >= bLessWeavItem)
		{
			//变量定义
			BYTE cbCardIndexTemp[MAX_INDEX];
			ZeroMemory(cbCardIndexTemp, sizeof(cbCardIndexTemp));

			//变量定义
			BYTE cbIndex[MAX_WEAVE] = { 0,1,2,3,4,5,6 };
			tagWeaveItem* pWeaveItem[CountArray(cbIndex)];

			//开始组合
			do
			{
				//设置变量
				memcpy(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));
				for (BYTE i = 0; i < bLessWeavItem; i++) pWeaveItem[i] = &WeaveItem[cbIndex[i]];

				//数量判断
				bool bEnoughCard = true;
				for (BYTE i = 0; i < bLessWeavItem * 3; i++)
				{
					//存在判断
					BYTE cbIndex = SwitchToCardIndex(pWeaveItem[i / 3]->cbCardList[i % 3]);
					if (cbCardIndexTemp[cbIndex] == 0)
					{
						bEnoughCard = false;
						break;
					}
					else cbCardIndexTemp[cbIndex]--;
				}

				//胡牌判断
				if (bEnoughCard == true)
				{
					//牌眼判断
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

					//组合类型
					if ((bNeedCardEye == false) || (cbCardEye != 0))
					{
						//变量定义
						tagAnalyseItem AnalyseItem;
						ZeroMemory(&AnalyseItem, sizeof(AnalyseItem));

						//设置结果
						AnalyseItem.cbHuXiCount = 0;
						AnalyseItem.cbCardEye = cbCardEye;
						AnalyseItem.cbWeaveCount = bLessWeavItem;

						//设置组合
						for (BYTE i = 0; i < bLessWeavItem; i++)
						{
							AnalyseItem.WeaveItemArray[i] = *pWeaveItem[i];
							AnalyseItem.cbHuXiCount += GetWeaveHuXi(AnalyseItem.WeaveItemArray[i]);
						}

						//插入结果
						AnalyseItemArray.push_back(AnalyseItem);
					}
				}

				//设置索引
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


	//胡牌结果
	bool GetHuCardInfo(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard, BYTE cbHuXiWeave, tagHuCardInfo& HuCardInfo)
	{
		//变量定义
		//static CAnalyseItemArray AnalyseItemArray;
		std::vector<tagAnalyseItem> AnalyseItemArray;

		//构造扑克
		BYTE cbCardIndexTemp[MAX_INDEX];
		memcpy(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

		//设置结果
		ZeroMemory(&HuCardInfo, sizeof(HuCardInfo));

		//提取碰牌
		if ((cbCurrentCard != 0) && (IsWeiPengCard(cbCardIndexTemp, cbCurrentCard) == true))
		{

			//判断胡牌	
			cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;
			AnalyseItemArray.clear();
			if (AnalyseCard(cbCardIndexTemp, AnalyseItemArray) == true)
			{
				//寻找最优
				BYTE cbHuXiCard = 0;
				int nBestItem = -1;
				for (int i = 0; i < (int)AnalyseItemArray.size(); i++)
				{
					//获取子项
					tagAnalyseItem* pAnalyseItem = &AnalyseItemArray[i];

					//胡息分析
					if (pAnalyseItem->cbHuXiCount >= cbHuXiCard)
					{
						nBestItem = i;
						cbHuXiCard = pAnalyseItem->cbHuXiCount;
					}
				}
				HuCardInfo.cbHuXiCount += cbHuXiCard;

				//设置结果
				if (nBestItem >= 0)
				{
					//获取子项
					tagAnalyseItem* pAnalyseItem = &AnalyseItemArray[nBestItem];

					//设置变量
					HuCardInfo.cbCardEye = pAnalyseItem->cbCardEye;

					//设置组合
					for (BYTE i = 0; i < pAnalyseItem->cbWeaveCount; i++)
					{
						BYTE cbIndex = HuCardInfo.cbWeaveCount++;
						HuCardInfo.WeaveItemArray[cbIndex] = pAnalyseItem->WeaveItemArray[i];
					}
				}

				if ((HuCardInfo.cbHuXiCount + cbHuXiWeave) >= 15)
					return true;
			}

			//构造组合
			BYTE cbIndex = HuCardInfo.cbWeaveCount++;
			HuCardInfo.WeaveItemArray[cbIndex].cbCardCount = 3;
			HuCardInfo.WeaveItemArray[cbIndex].cbWeaveKind = ACK_PENG;
			HuCardInfo.WeaveItemArray[cbIndex].cbCenterCard = cbCurrentCard;
			HuCardInfo.WeaveItemArray[cbIndex].cbCardList[0] = cbCurrentCard;
			HuCardInfo.WeaveItemArray[cbIndex].cbCardList[1] = cbCurrentCard;
			HuCardInfo.WeaveItemArray[cbIndex].cbCardList[2] = cbCurrentCard;

			//删除扑克
			cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)] = 0;

			//设置胡息
			HuCardInfo.cbHuXiCount += GetWeaveHuXi(HuCardInfo.WeaveItemArray[cbIndex]);
		}
		else if (cbCurrentCard != 0) cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;

		//提取三牌
		for (BYTE i = 0; i < MAX_INDEX; i++)
		{
			if (cbCardIndexTemp[i] == 3)
			{
				//设置扑克
				cbCardIndexTemp[i] = 0;

				//设置组合
				BYTE cbCardData = SwitchToCardData(i);
				BYTE cbIndex = HuCardInfo.cbWeaveCount++;
				HuCardInfo.WeaveItemArray[cbIndex].cbCardCount = 3;
				HuCardInfo.WeaveItemArray[cbIndex].cbWeaveKind = ACK_WEI;
				HuCardInfo.WeaveItemArray[cbIndex].cbCenterCard = cbCardData;
				HuCardInfo.WeaveItemArray[cbIndex].cbCardList[0] = cbCardData;
				HuCardInfo.WeaveItemArray[cbIndex].cbCardList[1] = cbCardData;
				HuCardInfo.WeaveItemArray[cbIndex].cbCardList[2] = cbCardData;

				//设置胡息
				HuCardInfo.cbHuXiCount += GetWeaveHuXi(HuCardInfo.WeaveItemArray[cbIndex]);
			}
		}

		//分析扑克
		AnalyseItemArray.clear();
		if (AnalyseCard(cbCardIndexTemp, AnalyseItemArray) == false) return false;

		//寻找最优
		BYTE cbHuXiCard = 0;
		int nBestItem = -1;
		for (int i = 0; i < (int)AnalyseItemArray.size(); i++)
		{
			//获取子项
			tagAnalyseItem* pAnalyseItem = &AnalyseItemArray[i];

			//胡息分析
			if (pAnalyseItem->cbHuXiCount >= cbHuXiCard)
			{
				nBestItem = i;
				cbHuXiCard = pAnalyseItem->cbHuXiCount;
			}
		}
		HuCardInfo.cbHuXiCount += cbHuXiCard;

		//设置结果
		if (nBestItem >= 0)
		{
			//获取子项
			tagAnalyseItem* pAnalyseItem = &AnalyseItemArray[nBestItem];

			//设置变量
			HuCardInfo.cbCardEye = pAnalyseItem->cbCardEye;

			//设置组合
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
		//提取吃牌
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