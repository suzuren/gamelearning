


#ifndef SERVER_SHOW_HAND_LOGIC_H
#define SERVER_SHOW_HAND_LOGIC_H

#include<vector>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>



using namespace std;

namespace game_show_hand
{
#define GAME_PLAYER					2									//游戏人数

//宏定义
	typedef unsigned char		BYTE;
	typedef unsigned int		uint32;
	typedef unsigned char		uint8;
	typedef unsigned short		uint16;
	typedef int					int32;

static const int MAX_COUNT          = 5;  //最大数目
static const int MAX_SHOWHAND_POKER = 28; //梭哈牌数

static const int MAX_CARD_VALUE = 14;									//最大牌值
static const int MAX_CARD_COLOR = 4;									//最大颜色

//扑克类型
enum emLAND_CARD_TYPE
{
    CT_SINGLE       = 1,                                    //单牌类型
    CT_ONE_DOUBLE,                                          //对子类型
    CT_TWO_DOUBLE,                                          //两对类型
    CT_THREE_TIAO,                                          //三条类型
    CT_SHUN_ZI,                                             //顺子类型
    CT_TONG_HUA,                                            //同花类型
    CT_HU_LU,                                               //葫芦类型（三带二）
    CT_TIE_ZHI,                                             //铁支类型(有四个一样的牌 比如四个8加一个1)
    CT_TONG_HUA_SHUN,                                       //同花顺型
};


//发牌概率索引
enum emSHOWHAND_CARD_TYPE_PRO_INDEX
{
	ShowHand_Pro_Index_TongHuaShun = 0,
	ShowHand_Pro_Index_TieZhi,
	ShowHand_Pro_Index_HuLu,
	ShowHand_Pro_Index_TongHua,
	ShowHand_Pro_Index_ShunZi,
	ShowHand_Pro_Index_ThreeTiao,
	ShowHand_Pro_Index_TwoDouble,
	ShowHand_Pro_Index_OneDouble,
	ShowHand_Pro_Index_Single,
	ShowHand_Pro_Index_MAX,
};

//分析结构
struct tagAnalyseResult
{
    BYTE 							cbFourCount;						//四张数目
    BYTE 							cbThreeCount;						//三张数目
    BYTE 							cbDoubleCount;						//两张数目
    BYTE							cbSignedCount;						//单张数目
    BYTE 							cbFourLogicVolue[1];				//四张列表
    BYTE 							cbThreeLogicVolue[1];				//三张列表
    BYTE 							cbDoubleLogicVolue[2];				//两张列表
    BYTE 							cbSignedLogicVolue[5];				//单张列表
    BYTE							cbFourCardData[MAX_COUNT];			//四张列表
    BYTE							cbThreeCardData[MAX_COUNT];			//三张列表
    BYTE							cbDoubleCardData[MAX_COUNT];		//两张列表
    BYTE							cbSignedCardData[MAX_COUNT];		//单张数目
};

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CShowHandLogic
{
    //变量定义
private:
    static BYTE						m_cbCardListData[MAX_SHOWHAND_POKER];				//扑克定义

    //函数定义
public:
    //构造函数
    CShowHandLogic();
    //析构函数
    virtual ~CShowHandLogic();

	// 扑克数值掩码
#define	MASK_COLOR	    0xF0	//花色掩码
#define	MASK_VALUE	    0x0F	//数值掩码

	//获取扑克数值
	BYTE    GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//获取扑克花色
	BYTE    GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }
	template <class T>
	int getArrayLen(T& array)
	{
		return (sizeof(array) / sizeof(array[0]));
	}

    //类型函数
public:
    //获取类型
    BYTE    GetCardGenre(BYTE cbCardData[], BYTE cbCardCount);
    //获取牌型值
    BYTE    GetCardGenreValue(BYTE cbCardData[], BYTE cbCardCount);
    //是否匹配做牌牌型
    bool    IsMatchedCardType(BYTE cbCardData[],BYTE count,bool type5,bool type3);
	//随机牌牌型
	bool	GetRandCardType(BYTE cbCardData[], BYTE count, bool & type5, bool & type3);
    bool    CanPlayFirstCardValue(BYTE cbHandCardData[],BYTE cbCardCount);

    //洗牌换牌
    void    GetLeftCard(BYTE cbCardBuffer[],BYTE cbBufferCount,vector<uint8>& leftCards);
    
    
    //控制函数
public:
    //排列扑克
    void SortCardList(BYTE cbCardData[], BYTE cbCardCount);
    //混乱扑克
    void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);

	void RandCardListEx(BYTE cbCardBuffer[], BYTE cbBufferCount)
	{
		//BYTE datalen = cbBufferCount;
		BYTE cbRandCount = 0, cbPosition = 0;
		do
		{
			cbPosition = 1;// g_RandGen.RandRange(0, cbBufferCount - 1);
			BYTE tempCard = cbCardBuffer[cbRandCount];
			cbCardBuffer[cbRandCount] = cbCardBuffer[cbPosition];
			cbCardBuffer[cbPosition] = tempCard;
			cbRandCount++;
		} while (cbRandCount<cbBufferCount);
		return;
	}

    //功能函数
public:
    //逻辑数值
    BYTE GetCardLogicValue(BYTE cbCardData);
    //对比扑克
    bool CompareCard(BYTE cbFirstCardData[], BYTE cbNextCardData[], BYTE cbCardCount);
    //分析扑克
    void AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult);

	void testShowHand();
};

};


#endif //SERVER_SHOW_HAND_LOGIC_H
