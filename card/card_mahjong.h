
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

//����ά��
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))


//��������
#define MAX_WEAVE					4									//������
#define MAX_INDEX					34									//�������
#define MAX_COUNT					14									//�����Ŀ
#define MAX_REPERTORY				108									//�����
#define MAX_HUA_CARD				0									//���Ƹ���

//���ڲ����ת����������ƿ��Դ����������ʹ�ã�����Ϊ����������������ΪMAX_INDEX. ע:����滻����������,������.
#define	INDEX_REPLACE_CARD					MAX_INDEX

#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����


//������־
#define WIK_NULL					0x00								//û������
#define WIK_LEFT					0x01								//�������
#define WIK_CENTER					0x02								//�г�����
#define WIK_RIGHT					0x04								//�ҳ�����
#define WIK_PENG					0x08								//��������
#define WIK_GANG					0x10								//��������
#define WIK_LISTEN					0x20								//��������
#define WIK_CHI_HU					0x40								//�Ժ�����
#define WIK_REPLACE					0x80								//�����滻


//��������
struct tagKindItem
{
	BYTE							cbWeaveKind;						//�������
	BYTE							cbCenterCard;						//�����˿�
	BYTE							cbValidIndex[3];					//ʵ���˿�����
};

//�������
struct tagWeaveItem
{
	BYTE							cbWeaveKind;						//�������
	BYTE							cbCenterCard;						//�����˿�
	BYTE							cbPublicCard;						//������־
	WORD							wProvideUser;						//��Ӧ�û�
	BYTE							cbCardData[4];						//
};

//��������
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//�����˿�
	bool                            bMagicEye;                          //�����Ƿ�������
	BYTE							cbWeaveKind[MAX_WEAVE];				//�������
	BYTE							cbCenterCard[MAX_WEAVE];			//�����˿�
	BYTE                            cbCardData[MAX_WEAVE][4];           //ʵ���˿�
};

//��Ч�ж�
bool IsValidCard(BYTE cbCardData)
{
	BYTE cbValue=(cbCardData&MASK_VALUE);
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
	return (((cbValue>=1)&&(cbValue<=9)&&(cbColor<=2))||((cbValue>=1)&&(cbValue<=0x0f)&&(cbColor==3)));
}

//�˿���Ŀ
BYTE GetCardCount(const BYTE cbCardIndex[MAX_INDEX])
{
	//��Ŀͳ��
	BYTE cbCardCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++) 
	{
		cbCardCount+=cbCardIndex[i];
	}
	return cbCardCount;
}

//�˿�ת��
BYTE SwitchToCardData(BYTE cbCardIndex)
{
	assert(cbCardIndex<MAX_INDEX);
	if( cbCardIndex < 27 )
	{
		return ((cbCardIndex/9)<<4)|(cbCardIndex%9+1);
	}
	else return (0x30|(cbCardIndex-27+1));
}

//�˿�ת��
BYTE SwitchToCardIndex(BYTE cbCardData)
{
	ASSERT(IsValidCard(cbCardData));
	return ((cbCardData&MASK_COLOR)>>4)*9+(cbCardData&MASK_VALUE)-1;
}

//��ȡ���
BYTE GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4])
{
	//����˿�
	switch (cbWeaveKind)
	{
	case WIK_LEFT:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard+1;
			cbCardBuffer[2]=cbCenterCard+2;

			return 3;
		}
	case WIK_RIGHT:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard-2;
			cbCardBuffer[1]=cbCenterCard-1;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_CENTER:	//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard-1;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard+1;

			return 3;
		}
	case WIK_PENG:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_GANG:		//���Ʋ���
		{
			//���ñ���
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

//����˵��
typedef std::vector<tagAnalyseItem> CAnalyseItemArray;

//�����˿�
bool AnalyseCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CAnalyseItemArray & AnalyseItemArray,BYTE cbMagicIndex)
{
	BYTE m_cbMagicIndex = cbMagicIndex;
	//������Ŀ
	BYTE cbCardCount=GetCardCount(cbCardIndex);

	//Ч����Ŀ
	assert((cbCardCount>=2)&&(cbCardCount<=MAX_COUNT)&&((cbCardCount-2)%3==0));
	if ((cbCardCount<2)||(cbCardCount>MAX_COUNT)||((cbCardCount-2)%3!=0))
		return false;

	//��������
	BYTE cbKindItemCount=0;
	tagKindItem KindItem[27*2+28];
	ZeroMemory(KindItem,sizeof(KindItem));

	//�����ж�
	BYTE cbLessKindItem=(cbCardCount-2)/3;
	assert((cbLessKindItem+cbWeaveCount)==MAX_WEAVE);

	printf("AnalyseCard - cbCardCount:%d,cbLessKindItem:%d\n",cbCardCount,cbLessKindItem);

	//�����ж�
	if (cbLessKindItem==0)  // �� cbCardCount == 2 Ҳ����ֻʣ��������Ծ��ܺ��� cbLessKindItem==0
	{
		//Ч�����
		assert((cbCardCount==2)&&(cbWeaveCount==MAX_WEAVE));

		//�����ж�
		for (BYTE i=0;i<MAX_INDEX-MAX_HUA_CARD;i++)
		{
			if (cbCardIndex[i]==2 || ( m_cbMagicIndex != MAX_INDEX && i != m_cbMagicIndex && cbCardIndex[m_cbMagicIndex]+cbCardIndex[i]==2 ) )
			{
				//��������
				tagAnalyseItem AnalyseItem;
				ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

				//���ý��
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

				//������
				AnalyseItemArray.push_back(AnalyseItem);

				return true;
			}
		}

		return false;
	}

	//��ַ���
	BYTE cbMagicCardIndex[MAX_INDEX];
	CopyMemory(cbMagicCardIndex,cbCardIndex,sizeof(cbMagicCardIndex));
	//����в���
	BYTE cbMagicCardCount = 0;
	if( m_cbMagicIndex != MAX_INDEX )
	{
		cbMagicCardCount = cbCardIndex[m_cbMagicIndex];
		//��������д����ƣ������������ת��
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
			//ͬ���ж�
			//����ǲ���,���Ҳ�����С��3,�򲻽������
			if( cbMagicCardIndex[i] >= 3 || ( cbMagicCardIndex[i]+cbMagicCardCount >= 3 &&( ( INDEX_REPLACE_CARD!=MAX_INDEX && i != INDEX_REPLACE_CARD ) || ( INDEX_REPLACE_CARD==MAX_INDEX && i != m_cbMagicIndex ) ) ))
			{
				int nTempIndex = cbMagicCardIndex[i];
				do
				{
					ASSERT( cbKindItemCount < CountArray(KindItem) );
					BYTE cbIndex = i;
					BYTE cbCenterCard = SwitchToCardData(i);
					//����ǲ����Ҳ����д�����,�򻻳ɴ�����
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

					//����ǲ���,���˳�
					if( i == INDEX_REPLACE_CARD || (i == m_cbMagicIndex && INDEX_REPLACE_CARD == MAX_INDEX) )
						break;

					nTempIndex -= 3;
					//����պô���ȫ�������˳�
					if( nTempIndex == 0 ) break;

				}while( nTempIndex+cbMagicCardCount >= 3 );
			}

			//�����ж�
			if ((i<(MAX_INDEX-MAX_HUA_CARD-9))&&((i%9)<7))
			{
				//ֻҪ������������3��˳���������������ڵ���3,��������
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

	//��Ϸ���
	if (cbKindItemCount>=cbLessKindItem)
	{
		//��������
		BYTE cbCardIndexTemp[MAX_INDEX];
		ZeroMemory(cbCardIndexTemp,sizeof(cbCardIndexTemp));

		//��������
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

		//��ʼ���
		do
		{
			//���ñ���
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

			//�����ж�
			bool bEnoughCard=true;
			
			for (BYTE i=0;i<cbLessKindItem*3;i++)
			{
				//�����ж�
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

			//�����ж�
			if (bEnoughCard==true)
			{
				//�����ж�
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

				//�������
				if (cbCardEye!=0)
				{
					//��������
					tagAnalyseItem AnalyseItem;
					ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

					//�������
					for (BYTE i=0;i<cbWeaveCount;i++)
					{
						AnalyseItem.cbWeaveKind[i]=WeaveItem[i].cbWeaveKind;
						AnalyseItem.cbCenterCard[i]=WeaveItem[i].cbCenterCard;
						GetWeaveCard( WeaveItem[i].cbWeaveKind,WeaveItem[i].cbCenterCard,AnalyseItem.cbCardData[i] );
					}

					//��������
					for (BYTE i=0;i<cbLessKindItem;i++) 
					{
						AnalyseItem.cbWeaveKind[i+cbWeaveCount]=pKindItem[i]->cbWeaveKind;
						AnalyseItem.cbCenterCard[i+cbWeaveCount]=pKindItem[i]->cbCenterCard;
						AnalyseItem.cbCardData[cbWeaveCount+i][0] = SwitchToCardData(pKindItem[i]->cbValidIndex[0]);
						AnalyseItem.cbCardData[cbWeaveCount+i][1] = SwitchToCardData(pKindItem[i]->cbValidIndex[1]);
						AnalyseItem.cbCardData[cbWeaveCount+i][2] = SwitchToCardData(pKindItem[i]->cbValidIndex[2]);
					}

					//��������
					AnalyseItem.cbCardEye=cbCardEye;
					AnalyseItem.bMagicEye = bMagicEye;

					//������
					AnalyseItemArray.push_back(AnalyseItem);
				}
			}

			//��������
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


//�˿�ת��
BYTE SwitchToCardIndex(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX])
{
	//���ñ���
	ZeroMemory(cbCardIndex,sizeof(BYTE)*MAX_INDEX);

	//ת���˿�
	for (BYTE i=0;i<cbCardCount;i++)
	{
		ASSERT(IsValidCard(cbCardData[i]));
		cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
	}

	return cbCardCount;
}

//�˿�����
const BYTE m_cbCardDataArray[MAX_REPERTORY]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	
};

//�����˿�
void RandCardData(BYTE cbCardData[], BYTE cbMaxCount)
{
	//����׼��
	BYTE cbCardDataTemp[CountArray(m_cbCardDataArray)];
	CopyMemory(cbCardDataTemp,m_cbCardDataArray,sizeof(m_cbCardDataArray));

	//�����˿�
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
	//BYTE m_cbRepertoryCard[MAX_REPERTORY];	//����˿�
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

	//�����˿�
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


