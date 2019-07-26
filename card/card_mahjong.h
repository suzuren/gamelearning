
#ifndef _CARD_MAHJONG_H_
#define _CARD_MAHJONG_H_

#include <assert.h>
#include <memory.h>
#include <vector>

typedef unsigned char		BYTE;
typedef unsigned short      WORD;



#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))
#define CopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))

#define ASSERT assert

//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))


//常量定义
#define MAX_WEAVE					4									//最大组合
#define MAX_INDEX					34									//最大索引
#define MAX_COUNT					14									//最大数目
#define MAX_REPERTORY				108									//最大库存
#define MAX_HUA_CARD				0									//花牌个数

//用于财神的转换，如果有牌可以代替财神本身牌使用，则设为该牌索引，否则设为MAX_INDEX. 注:如果替换牌是序数牌,将出错.
#define	INDEX_REPLACE_CARD					MAX_INDEX

#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码


//动作标志
#define WIK_NULL					0x00								//没有类型
#define WIK_LEFT					0x01								//左吃类型
#define WIK_CENTER					0x02								//中吃类型
#define WIK_RIGHT					0x04								//右吃类型
#define WIK_PENG					0x08								//碰牌类型
#define WIK_GANG					0x10								//杠牌类型
#define WIK_LISTEN					0x20								//吃牌类型
#define WIK_CHI_HU					0x40								//吃胡类型
#define WIK_REPLACE					0x80								//花牌替换


//类型子项
struct tagKindItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbValidIndex[3];					//实际扑克索引
};

//组合子项
struct tagWeaveItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbPublicCard;						//公开标志
	WORD							wProvideUser;						//供应用户
	BYTE							cbCardData[4];						//
};

//分析子项
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//牌眼扑克
	bool                            bMagicEye;                          //牌眼是否是王霸
	BYTE							cbWeaveKind[MAX_WEAVE];				//组合类型
	BYTE							cbCenterCard[MAX_WEAVE];			//中心扑克
	BYTE                            cbCardData[MAX_WEAVE][4];           //实际扑克
};

//有效判断
bool IsValidCard(BYTE cbCardData)
{
	BYTE cbValue=(cbCardData&MASK_VALUE);
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
	return (((cbValue>=1)&&(cbValue<=9)&&(cbColor<=2))||((cbValue>=1)&&(cbValue<=0x0f)&&(cbColor==3)));
}

//扑克数目
BYTE GetCardCount(const BYTE cbCardIndex[MAX_INDEX])
{
	//数目统计
	BYTE cbCardCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++) 
	{
		cbCardCount+=cbCardIndex[i];
	}
	return cbCardCount;
}

//扑克转换
BYTE SwitchToCardData(BYTE cbCardIndex)
{
	assert(cbCardIndex<MAX_INDEX);
	if( cbCardIndex < 27 )
	{
		return ((cbCardIndex/9)<<4)|(cbCardIndex%9+1);
	}
	else return (0x30|(cbCardIndex-27+1));
}

//扑克转换
BYTE SwitchToCardIndex(BYTE cbCardData)
{
	ASSERT(IsValidCard(cbCardData));
	return ((cbCardData&MASK_COLOR)>>4)*9+(cbCardData&MASK_VALUE)-1;
}

//获取组合
BYTE GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4])
{
	//组合扑克
	switch (cbWeaveKind)
	{
	case WIK_LEFT:		//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard+1;
			cbCardBuffer[2]=cbCenterCard+2;

			return 3;
		}
	case WIK_RIGHT:		//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard-2;
			cbCardBuffer[1]=cbCenterCard-1;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_CENTER:	//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard-1;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard+1;

			return 3;
		}
	case WIK_PENG:		//碰牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_GANG:		//杠牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;
			cbCardBuffer[3]=cbCenterCard;

			return 4;
		}
	default:
		{
			ASSERT(0);
		}
	}

	return 0;
}

//数组说明
typedef std::vector<tagAnalyseItem> CAnalyseItemArray;

//分析扑克
bool AnalyseCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CAnalyseItemArray & AnalyseItemArray,BYTE cbMagicIndex)
{
	BYTE m_cbMagicIndex = cbMagicIndex;
	//计算数目
	BYTE cbCardCount=GetCardCount(cbCardIndex);

	//效验数目
	assert((cbCardCount>=2)&&(cbCardCount<=MAX_COUNT)&&((cbCardCount-2)%3==0));
	if ((cbCardCount<2)||(cbCardCount>MAX_COUNT)||((cbCardCount-2)%3!=0))
		return false;

	//变量定义
	BYTE cbKindItemCount=0;
	tagKindItem KindItem[27*2+28];
	ZeroMemory(KindItem,sizeof(KindItem));

	//需求判断
	BYTE cbLessKindItem=(cbCardCount-2)/3;
	assert((cbLessKindItem+cbWeaveCount)==MAX_WEAVE);

	printf("AnalyseCard - cbCardCount:%d,cbLessKindItem:%d\n",cbCardCount,cbLessKindItem);

	//单吊判断
	if (cbLessKindItem==0)  // 当 cbCardCount == 2 也就是只剩下牌眼配对就能胡牌 cbLessKindItem==0
	{
		//效验参数
		assert((cbCardCount==2)&&(cbWeaveCount==MAX_WEAVE));

		//牌眼判断
		for (BYTE i=0;i<MAX_INDEX-MAX_HUA_CARD;i++)
		{
			if (cbCardIndex[i]==2 || ( m_cbMagicIndex != MAX_INDEX && i != m_cbMagicIndex && cbCardIndex[m_cbMagicIndex]+cbCardIndex[i]==2 ) )
			{
				//变量定义
				tagAnalyseItem AnalyseItem;
				ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

				//设置结果
				for (BYTE j=0;j<cbWeaveCount;j++)
				{
					AnalyseItem.cbWeaveKind[j]=WeaveItem[j].cbWeaveKind;
					AnalyseItem.cbCenterCard[j]=WeaveItem[j].cbCenterCard;
					CopyMemory( AnalyseItem.cbCardData[j],WeaveItem[j].cbCardData,sizeof(WeaveItem[j].cbCardData) );
				}
				AnalyseItem.cbCardEye=SwitchToCardData(i);
				if( cbCardIndex[i] < 2 || i == m_cbMagicIndex )
				{
					AnalyseItem.bMagicEye = true;
				}
				else
				{
					AnalyseItem.bMagicEye = false;
				}

				//插入结果
				AnalyseItemArray.push_back(AnalyseItem);

				return true;
			}
		}

		return false;
	}

	//拆分分析
	BYTE cbMagicCardIndex[MAX_INDEX];
	CopyMemory(cbMagicCardIndex,cbCardIndex,sizeof(cbMagicCardIndex));
	//如果有财神
	BYTE cbMagicCardCount = 0;
	if( m_cbMagicIndex != MAX_INDEX )
	{
		cbMagicCardCount = cbCardIndex[m_cbMagicIndex];
		//如果财神有代替牌，财神与代替牌转换
		if( INDEX_REPLACE_CARD != MAX_INDEX )
		{
			cbMagicCardIndex[m_cbMagicIndex] = cbMagicCardIndex[INDEX_REPLACE_CARD];
			//cbMagicCardIndex[INDEX_REPLACE_CARD] = cbMagicCardCount;
		}
	}
	if (cbCardCount>=3)
	{
		for (BYTE i=0;i<MAX_INDEX-MAX_HUA_CARD;i++)
		{
			//同牌判断
			//如果是财神,并且财神数小于3,则不进行组合
			if( cbMagicCardIndex[i] >= 3 || ( cbMagicCardIndex[i]+cbMagicCardCount >= 3 &&( ( INDEX_REPLACE_CARD!=MAX_INDEX && i != INDEX_REPLACE_CARD ) || ( INDEX_REPLACE_CARD==MAX_INDEX && i != m_cbMagicIndex ) ) ))
			{
				int nTempIndex = cbMagicCardIndex[i];
				do
				{
					ASSERT( cbKindItemCount < CountArray(KindItem) );
					BYTE cbIndex = i;
					BYTE cbCenterCard = SwitchToCardData(i);
					//如果是财神且财神有代替牌,则换成代替牌
					if( i == m_cbMagicIndex && INDEX_REPLACE_CARD != MAX_INDEX )
					{
						cbIndex = INDEX_REPLACE_CARD;
						cbCenterCard = SwitchToCardData(INDEX_REPLACE_CARD);
					}
					KindItem[cbKindItemCount].cbWeaveKind=WIK_PENG;
					KindItem[cbKindItemCount].cbCenterCard=cbCenterCard;
					KindItem[cbKindItemCount].cbValidIndex[0] = nTempIndex>0?cbIndex:m_cbMagicIndex;
					KindItem[cbKindItemCount].cbValidIndex[1] = nTempIndex>1?cbIndex:m_cbMagicIndex;
					KindItem[cbKindItemCount].cbValidIndex[2] = nTempIndex>2?cbIndex:m_cbMagicIndex;
					cbKindItemCount++;

					//如果是财神,则退出
					if( i == INDEX_REPLACE_CARD || (i == m_cbMagicIndex && INDEX_REPLACE_CARD == MAX_INDEX) )
						break;

					nTempIndex -= 3;
					//如果刚好搭配全部，则退出
					if( nTempIndex == 0 ) break;

				}while( nTempIndex+cbMagicCardCount >= 3 );
			}

			//连牌判断
			if ((i<(MAX_INDEX-MAX_HUA_CARD-9))&&((i%9)<7))
			{
				//只要财神牌数加上3个顺序索引的牌数大于等于3,则进行组合
				if( cbMagicCardCount+cbMagicCardIndex[i]+cbMagicCardIndex[i+1]+cbMagicCardIndex[i+2] >= 3 )
				{
					BYTE cbIndex[3] = { cbMagicCardIndex[i],cbMagicCardIndex[i+1],cbMagicCardIndex[i+2] };
					int nMagicCountTemp = cbMagicCardCount;
					BYTE cbValidIndex[3];
					while( nMagicCountTemp+cbIndex[0]+cbIndex[1]+cbIndex[2] >= 3 )
					{
						for( BYTE j = 0; j < CountArray(cbIndex); j++ )
						{
							if( cbIndex[j] > 0 ) 
							{
								cbIndex[j]--;
								cbValidIndex[j] = (i+j==m_cbMagicIndex&&INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j;
							}
							else
							{
								nMagicCountTemp--;
								cbValidIndex[j] = m_cbMagicIndex;
							}
						}
						if( nMagicCountTemp >= 0 )
						{
							ASSERT( cbKindItemCount < CountArray(KindItem) );
							KindItem[cbKindItemCount].cbWeaveKind=WIK_LEFT;
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							CopyMemory( KindItem[cbKindItemCount].cbValidIndex,cbValidIndex,sizeof(cbValidIndex) );
							cbKindItemCount++;
						}
						else
						{
							break;
						}
					}
				}
			}
		}
	}

	//组合分析
	if (cbKindItemCount>=cbLessKindItem)
	{
		//变量定义
		BYTE cbCardIndexTemp[MAX_INDEX];
		ZeroMemory(cbCardIndexTemp,sizeof(cbCardIndexTemp));

		//变量定义
		BYTE cbIndex[MAX_WEAVE];
		for( BYTE i = 0; i < CountArray(cbIndex); i++ )
		{
			cbIndex[i] = i;
		}
		tagKindItem * pKindItem[MAX_WEAVE];
		ZeroMemory(&pKindItem,sizeof(pKindItem));
		tagKindItem KindItemTemp[CountArray(KindItem)];
		if( m_cbMagicIndex != MAX_INDEX )
		{
			CopyMemory( KindItemTemp,KindItem,sizeof(KindItem) );
		}

		//开始组合
		do
		{
			//设置变量
			CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));
			cbMagicCardCount = 0;
			if( m_cbMagicIndex != MAX_INDEX )
			{
				CopyMemory( KindItem,KindItemTemp,sizeof(KindItem) );
			}

			for (BYTE i=0;i<cbLessKindItem;i++)
			{
				pKindItem[i]=&KindItem[cbIndex[i]];
			}

			//数量判断
			bool bEnoughCard=true;
			
			for (BYTE i=0;i<cbLessKindItem*3;i++)
			{
				//存在判断
				BYTE cbCardIndex=pKindItem[i/3]->cbValidIndex[i%3]; 
				if (cbCardIndexTemp[cbCardIndex]==0)
				{
					if( m_cbMagicIndex != MAX_INDEX && cbCardIndexTemp[m_cbMagicIndex] > 0 )
					{
						cbCardIndexTemp[m_cbMagicIndex]--;
						pKindItem[i/3]->cbValidIndex[i%3] = m_cbMagicIndex;
					}
					else
					{
						bEnoughCard=false;
						break;
					}
				}
				else cbCardIndexTemp[cbCardIndex]--;
			}

			//胡牌判断
			if (bEnoughCard==true)
			{
				//牌眼判断
				BYTE cbCardEye=0;
				bool bMagicEye = false;
				if( GetCardCount(cbCardIndexTemp) == 2 )
				{
					for (BYTE i=0;i<MAX_INDEX-MAX_HUA_CARD;i++)
					{
						if (cbCardIndexTemp[i]==2)
						{
							cbCardEye=SwitchToCardData(i);
							if( m_cbMagicIndex != MAX_INDEX && i == m_cbMagicIndex ) bMagicEye = true;
							break;
						}
						else if( i!=m_cbMagicIndex && m_cbMagicIndex != MAX_INDEX && cbCardIndexTemp[i]+cbCardIndexTemp[m_cbMagicIndex]==2 )
						{
							cbCardEye = SwitchToCardData(i);
							bMagicEye = true;
						}
					}
				}

				//组合类型
				if (cbCardEye!=0)
				{
					//变量定义
					tagAnalyseItem AnalyseItem;
					ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

					//设置组合
					for (BYTE i=0;i<cbWeaveCount;i++)
					{
						AnalyseItem.cbWeaveKind[i]=WeaveItem[i].cbWeaveKind;
						AnalyseItem.cbCenterCard[i]=WeaveItem[i].cbCenterCard;
						GetWeaveCard( WeaveItem[i].cbWeaveKind,WeaveItem[i].cbCenterCard,AnalyseItem.cbCardData[i] );
					}

					//设置牌型
					for (BYTE i=0;i<cbLessKindItem;i++) 
					{
						AnalyseItem.cbWeaveKind[i+cbWeaveCount]=pKindItem[i]->cbWeaveKind;
						AnalyseItem.cbCenterCard[i+cbWeaveCount]=pKindItem[i]->cbCenterCard;
						AnalyseItem.cbCardData[cbWeaveCount+i][0] = SwitchToCardData(pKindItem[i]->cbValidIndex[0]);
						AnalyseItem.cbCardData[cbWeaveCount+i][1] = SwitchToCardData(pKindItem[i]->cbValidIndex[1]);
						AnalyseItem.cbCardData[cbWeaveCount+i][2] = SwitchToCardData(pKindItem[i]->cbValidIndex[2]);
					}

					//设置牌眼
					AnalyseItem.cbCardEye=cbCardEye;
					AnalyseItem.bMagicEye = bMagicEye;

					//插入结果
					AnalyseItemArray.push_back(AnalyseItem);
				}
			}

			//设置索引
			if (cbIndex[cbLessKindItem-1]==(cbKindItemCount-1))
			{
				BYTE i=0;
				for (i=cbLessKindItem-1;i>0;i--)
				{
					if ((cbIndex[i-1]+1)!=cbIndex[i])
					{
						BYTE cbNewIndex=cbIndex[i-1];
						for (BYTE j=(i-1);j<cbLessKindItem;j++) 
							cbIndex[j]=cbNewIndex+j-i+2;
						break;
					}
				}
				if (i==0)
					break;
			}
			else
				cbIndex[cbLessKindItem-1]++;
		} while (true);

	}

	return (AnalyseItemArray.size()>0);
}


//扑克转换
BYTE SwitchToCardIndex(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX])
{
	//设置变量
	ZeroMemory(cbCardIndex,sizeof(BYTE)*MAX_INDEX);

	//转换扑克
	for (BYTE i=0;i<cbCardCount;i++)
	{
		ASSERT(IsValidCard(cbCardData[i]));
		cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
	}

	return cbCardCount;
}

//扑克数据
const BYTE m_cbCardDataArray[MAX_REPERTORY]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
	
};

//混乱扑克
void RandCardData(BYTE cbCardData[], BYTE cbMaxCount)
{
	//混乱准备
	BYTE cbCardDataTemp[CountArray(m_cbCardDataArray)];
	CopyMemory(cbCardDataTemp,m_cbCardDataArray,sizeof(m_cbCardDataArray));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbMaxCount-cbRandCount);
		cbCardData[cbRandCount++]=cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition]=cbCardDataTemp[cbMaxCount-cbRandCount];
	} while (cbRandCount<cbMaxCount);

	return;
}

void test_card_mahjong()
{
	printf("hello card_mahjong!\n");

	BYTE cbCardIndex[MAX_INDEX] = {0};
	tagWeaveItem WeaveItem[MAX_WEAVE];
	BYTE cbWeaveCount = 0;
	CAnalyseItemArray AnalyseItemArray;

	//BYTE m_cbLeftCardCount = 0;
	//BYTE m_cbRepertoryCard[MAX_REPERTORY];	//库存扑克
	//m_cbLeftCardCount=CountArray(m_cbRepertoryCard);
	//RandCardData(m_cbRepertoryCard,CountArray(m_cbRepertoryCard));
	//SwitchToCardIndex(&m_cbRepertoryCard[m_cbLeftCardCount],MAX_COUNT-1,cbCardIndex);

	BYTE cbCardData[MAX_COUNT] = {0x01,0x02,0x03,0x11,0x11,0x11,0x21,0x22,0x23,0x28,0x28,0x28,0x09,0x09};
	//BYTE cbCardData[MAX_COUNT] ={0x01,0x02,0x03, 0x01,0x02,0x03, 0x01,0x02,0x03, 0x01,0x02,0x03, 0x09,0x09};

	for(int index = 0; index<MAX_COUNT; index++)
	{
		BYTE cbTempCardData  = cbCardData[index];
		BYTE cbTempCardIndex = SwitchToCardIndex(cbTempCardData);
		cbCardIndex[cbTempCardIndex]++;

		//cbCardIndex[SwitchToCardIndex(cbCardData[index])]++;
		printf("index:%02d,cbTempCardIndex:%02d,cbCardData:0x%02X\n",index,cbTempCardIndex,cbTempCardData);
	}

	printf("\n\n\n");

	for(int index = 0; index<MAX_INDEX; index++)
	{
		BYTE cbTempCardCount = cbCardIndex[index];
		BYTE cbTempCardData  = SwitchToCardData(index);
		if(cbTempCardCount==0)
		{
			continue;
		}
		printf("index:%02d,cbCardCount:%d,cbCardData:0x%02X\n",index,cbTempCardCount,cbTempCardData);
	}

	//分析扑克
	BYTE cbMagicIndex = MAX_INDEX;
	AnalyseCard(cbCardIndex, WeaveItem, cbWeaveCount, AnalyseItemArray, cbMagicIndex);

	printf("AnalyseItemArray.size:%u\n",AnalyseItemArray.size());
	for(unsigned int index = 0; index<AnalyseItemArray.size(); index++)
	{
		tagAnalyseItem & AnalyseItem = AnalyseItemArray[index];

		printf("index:%d,cbCardEye:0x%02X,bMagicEye:%d\n",index,AnalyseItem.cbCardEye,AnalyseItem.bMagicEye);

		for(int j=0; j<MAX_WEAVE; j++)
		{
			printf("j:%d,cbWeaveKind:0x%02X,cbCenterCard:0x%02X,cbCardData:0x%02X 0x%02X 0x%02X 0x%02X\n",
				j,AnalyseItem.cbWeaveKind[j],AnalyseItem.cbCenterCard[j],AnalyseItem.cbCardData[j][0],AnalyseItem.cbCardData[j][1],AnalyseItem.cbCardData[j][2],AnalyseItem.cbCardData[j][3]);
		}
		printf("\n\n\n");
		//printf("index:%d,cbCardEye:%0x02X,bMagicEye:%d\n",index,AnalyseItem.cbCardEye,AnalyseItem.bMagicEye);

	}
}

#endif


