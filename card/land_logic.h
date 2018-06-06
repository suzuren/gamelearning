
#include <stdio.h>
#include <memory.h>

typedef unsigned char		BYTE;
static const int  MAX_LAND_COUNT = 20;                                    // 最大数目

//逻辑类型
enum emLAND_CARD_TYPE
{
	CT_ERROR = 0,                                 //错误类型
	CT_SINGLE,                                    //单牌类型
	CT_DOUBLE,                                    //对牌类型
	CT_THREE,                                     //三条类型
	CT_SINGLE_LINE,                               //单连类型
	CT_DOUBLE_LINE,                               //对连类型
	CT_THREE_LINE,                                //三连类型
	CT_THREE_TAKE_ONE,                            //三带一单
	CT_THREE_TAKE_TWO,                            //三带一对
	CT_FOUR_TAKE_ONE,                             //四带两单
	CT_FOUR_TAKE_TWO,                             //四带两对
	CT_BOMB_CARD,                                 //炸弹类型
	CT_MISSILE_CARD,                              //火箭类型
};


//分析结构
struct tagAnalyseResult {
	BYTE cbBlockCount[4];                    //扑克数目
	BYTE cbCardData[4][MAX_LAND_COUNT];        //扑克数据
};

// 扑克数值掩码
#define	MASK_COLOR	    0xF0	//花色掩码
#define	MASK_VALUE	    0x0F	//数值掩码


//获取扑克数值
BYTE    GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
//获取扑克花色
BYTE    GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

//逻辑数值
BYTE GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE cbCardColor = GetCardColor(cbCardData);
	BYTE cbCardValue = GetCardValue(cbCardData);

	//转换数值
	if (cbCardColor == 0x40) return cbCardValue + 2;
	return (cbCardValue <= 2) ? (cbCardValue + 13) : cbCardValue;
}

void AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult &AnalyseResult)
{
	//设置结果
	memset(&AnalyseResult, 0, sizeof(AnalyseResult));

	//扑克分析
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		//变量定义
		BYTE cbSameCount = 1;
		BYTE cbLogicValue = GetCardLogicValue(cbCardData[i]);

		//搜索同牌
		for (BYTE j = i + 1; j < cbCardCount; j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbCardData[j]) != cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		//设置结果
		BYTE cbIndex = AnalyseResult.cbBlockCount[cbSameCount - 1]++;
		for (BYTE j = 0; j < cbSameCount; j++)
			AnalyseResult.cbCardData[cbSameCount - 1][cbIndex * cbSameCount + j] = cbCardData[i + j];

		//设置索引
		i += cbSameCount - 1;
	}
	return;
}

//获取类型
BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	//简单牌型
	switch (cbCardCount) {
	case 0:    //空牌
	{
		return CT_ERROR;
	}
	case 1: //单牌
	{
		return CT_SINGLE;
	}
	case 2:    //对牌火箭
	{
		//牌型判断
		if ((cbCardData[0] == 0x4F) && (cbCardData[1] == 0x4E)) return CT_MISSILE_CARD;
		if (GetCardLogicValue(cbCardData[0]) == GetCardLogicValue(cbCardData[1])) return CT_DOUBLE;

		return CT_ERROR;
	}
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

	//四牌判断
	if (AnalyseResult.cbBlockCount[3] > 0)
	{
		//牌型判断
		if ((AnalyseResult.cbBlockCount[3] == 1) && (cbCardCount == 4)) return CT_BOMB_CARD;
		if ((AnalyseResult.cbBlockCount[3] == 1) && (cbCardCount == 6)) return CT_FOUR_TAKE_ONE;
		if ((AnalyseResult.cbBlockCount[3] == 1) && (cbCardCount == 8) &&
			(AnalyseResult.cbBlockCount[1] == 2))
			return CT_FOUR_TAKE_TWO;


		return CT_ERROR;
	}

	//三牌判断
	if (AnalyseResult.cbBlockCount[2] > 0)
	{
		//连牌判断
		if (AnalyseResult.cbBlockCount[2] > 1)
		{
			//变量定义
			BYTE cbCardData = AnalyseResult.cbCardData[2][0];
			BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

			//错误过虑
			if (cbFirstLogicValue >= 15) return CT_ERROR;

			//连牌判断
			for (BYTE i = 1; i < AnalyseResult.cbBlockCount[2]; i++) {
				BYTE cbCardData = AnalyseResult.cbCardData[2][i * 3];
				if (cbFirstLogicValue != (GetCardLogicValue(cbCardData) + i)) return CT_ERROR;
			}
		}
		else if (cbCardCount == 3) return CT_THREE;

		//牌形判断
		if (AnalyseResult.cbBlockCount[2] * 3 == cbCardCount) return CT_THREE_LINE;
		if (AnalyseResult.cbBlockCount[2] * 4 == cbCardCount) return CT_THREE_TAKE_ONE;
		if ((AnalyseResult.cbBlockCount[2] * 5 == cbCardCount) &&
			(AnalyseResult.cbBlockCount[1] == AnalyseResult.cbBlockCount[2]))
			return CT_THREE_TAKE_TWO;

		return CT_ERROR;
	}

	//两张类型
	if (AnalyseResult.cbBlockCount[1] >= 3) {
		//变量定义
		BYTE cbCardData = AnalyseResult.cbCardData[1][0];
		BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue >= 15) return CT_ERROR;

		//连牌判断
		for (BYTE i = 1; i < AnalyseResult.cbBlockCount[1]; i++) {
			BYTE cbCardData = AnalyseResult.cbCardData[1][i * 2];
			if (cbFirstLogicValue != (GetCardLogicValue(cbCardData) + i)) return CT_ERROR;
		}

		//二连判断
		if ((AnalyseResult.cbBlockCount[1] * 2) == cbCardCount) return CT_DOUBLE_LINE;

		return CT_ERROR;
	}

	//单张判断
	if ((AnalyseResult.cbBlockCount[0] >= 5) && (AnalyseResult.cbBlockCount[0] == cbCardCount))
	{
		//变量定义
		BYTE cbCardData = AnalyseResult.cbCardData[0][0];
		BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue >= 15) return CT_ERROR;

		//连牌判断
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
	//获取类型
	BYTE cbNextType = GetCardType(cbNextCard, cbNextCount);
	BYTE cbFirstType = GetCardType(cbFirstCard, cbFirstCount);

	printf("CompareCard - cbFirstCount:%d,cbNextCount:%d,cbFirstType:%d,cbNextType:%d.\n", cbFirstCount, cbNextCount, cbFirstType, cbNextType);
	//		CompareCard - cbFirstCount:2, cbNextCount : 4, cbFirstType : 12, cbNextType : 11.
	//类型判断
	if (cbNextType == CT_ERROR) return false;
	if (cbNextType == CT_MISSILE_CARD) return true;
	if (cbFirstType == CT_MISSILE_CARD) return false;

	//炸弹判断
	if ((cbFirstType != CT_BOMB_CARD) && (cbNextType == CT_BOMB_CARD)) return true;
	if ((cbFirstType == CT_BOMB_CARD) && (cbNextType != CT_BOMB_CARD)) return false;

	printf("CompareCard 2 - cbFirstCount:%d,cbNextCount:%d,cbFirstType:%d,cbNextType:%d.\n", cbFirstCount, cbNextCount, cbFirstType, cbNextType);


	//规则判断
	if ((cbFirstType != cbNextType) || (cbFirstCount != cbNextCount)) return false;

	//开始对比
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
		//获取数值
		BYTE cbNextLogicValue = GetCardLogicValue(cbNextCard[0]);
		BYTE cbFirstLogicValue = GetCardLogicValue(cbFirstCard[0]);

		//对比扑克
		return cbNextLogicValue > cbFirstLogicValue;
	}
	case CT_THREE_TAKE_ONE:
	case CT_THREE_TAKE_TWO:
	{
		//分析扑克
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;
		AnalysebCardData(cbNextCard, cbNextCount, NextResult);
		AnalysebCardData(cbFirstCard, cbFirstCount, FirstResult);

		//获取数值
		BYTE cbNextLogicValue = GetCardLogicValue(NextResult.cbCardData[2][0]);
		BYTE cbFirstLogicValue = GetCardLogicValue(FirstResult.cbCardData[2][0]);

		//对比扑克
		return cbNextLogicValue > cbFirstLogicValue;
	}
	case CT_FOUR_TAKE_ONE:
	case CT_FOUR_TAKE_TWO:
	{
		//分析扑克
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;
		AnalysebCardData(cbNextCard, cbNextCount, NextResult);
		AnalysebCardData(cbFirstCard, cbFirstCount, FirstResult);

		//获取数值
		BYTE cbNextLogicValue = GetCardLogicValue(NextResult.cbCardData[3][0]);
		BYTE cbFirstLogicValue = GetCardLogicValue(FirstResult.cbCardData[3][0]);

		//对比扑克
		return cbNextLogicValue > cbFirstLogicValue;
	}
	}

	return false;
}