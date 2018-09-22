

#include "show_hand_logic.h"


namespace game_show_hand
{

//#define    MAGIC_A_CARD                                //A是否可变换成7

//扑克数据
    BYTE CShowHandLogic::m_cbCardListData[MAX_SHOWHAND_POKER] =
            {
                    0x01, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,                                    //方块 A - K
                    0x11, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,                                    //梅花 A - K
                    0x21, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,                                    //红桃 A - K
                    0x31, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,                                    //黑桃 A - K
            };
//构造函数
    CShowHandLogic::CShowHandLogic() {
    }

//析构函数
    CShowHandLogic::~CShowHandLogic() {
    }

//获取类型
    BYTE CShowHandLogic::GetCardGenre(BYTE cbCardData[], BYTE cbCardCount) {
		SortCardList(cbCardData, cbCardCount);
        //简单牌形
        switch (cbCardCount)
        {
            case 1: //单牌
            {
                return CT_SINGLE;
            }
            case 2: //对牌
            {
                return (GetCardLogicValue(cbCardData[0]) == GetCardLogicValue(cbCardData[1])) ? CT_ONE_DOUBLE
                                                                                              : CT_SINGLE;
            }
            case 3: //三张
            {
                if(GetCardLogicValue(cbCardData[0]) == GetCardLogicValue(cbCardData[2])) 
                    return CT_THREE_TIAO;
                if(GetCardLogicValue(cbCardData[0]) == GetCardLogicValue(cbCardData[1]) ||
                   GetCardLogicValue(cbCardData[1]) == GetCardLogicValue(cbCardData[2])){
                    return CT_ONE_DOUBLE;
                }
                return CT_SINGLE;                   
            } 
            
        }
        //五条类型
        if (cbCardCount >= 4) {
            //变量定义
            bool cbSameColor = true, bLineCard = true;
            BYTE cbFirstColor = GetCardColor(cbCardData[0]);
            BYTE cbFirstValue = GetCardLogicValue(cbCardData[0]);

            BYTE i = 1;
            //牌形分析
            for (i = 1; i < cbCardCount; i++) {
                //数据分析
                if (GetCardColor(cbCardData[i]) != cbFirstColor) cbSameColor = false;
                if (cbFirstValue != (GetCardLogicValue(cbCardData[i]) + i)) bLineCard = false;

                //结束判断
                if ((cbSameColor == false) && (bLineCard == false)) break;
            }

//#ifdef    MAGIC_A_CARD
//            //特殊
//            if (!bLineCard && GetCardValue(cbCardData[0]) == 1) {
//                //A J 10 9 8特殊
//                BYTE cbMinusCount = cbCardCount == 4 ? 3 : 2;
//                for (i = 1; i < cbCardCount; i++) {
//                    if ((cbFirstValue - cbMinusCount) != (GetCardLogicValue(cbCardData[i]) + i)) break;
//                }
//                if (i == cbCardCount) bLineCard = true;
//            }
//#endif

            //顺子类型
            if ((cbSameColor == false) && (bLineCard == true)) return CT_SHUN_ZI;

            //同花类型
            if ((cbSameColor == true) && (bLineCard == false)) return CT_TONG_HUA;

            //同花顺类型
            if ((cbSameColor == true) && (bLineCard == true)) return CT_TONG_HUA_SHUN;
        }

        //扑克分析
        tagAnalyseResult AnalyseResult;
        AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

        //四条类型
        if (AnalyseResult.cbFourCount == 1) return CT_TIE_ZHI;

        //两对类型
        if (AnalyseResult.cbDoubleCount == 2) return CT_TWO_DOUBLE;

        //对牌类型
        if ((AnalyseResult.cbDoubleCount == 1) && (AnalyseResult.cbThreeCount == 0)) return CT_ONE_DOUBLE;

        //葫芦类型
        if (AnalyseResult.cbThreeCount == 1) return (AnalyseResult.cbDoubleCount == 1) ? CT_HU_LU : CT_THREE_TIAO;

        return CT_SINGLE;
    }

//排列扑克
    void CShowHandLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount) {
        //转换数值
        if(cbCardCount < 2)
            return;
        BYTE cbLogicValue[MAX_COUNT];
        for (BYTE i = 0; i < cbCardCount; i++) {
            cbLogicValue[i] = GetCardLogicValue(cbCardData[i]);
        }

        //排序操作
        bool bSorted = true;
        BYTE cbTempData, bLast = cbCardCount - 1;
        do {
            bSorted = true;
            for (BYTE i = 0; i < bLast; i++) {
                if ((cbLogicValue[i] < cbLogicValue[i + 1]) ||
                    ((cbLogicValue[i] == cbLogicValue[i + 1]) && (cbCardData[i] < cbCardData[i + 1]))) {
                    //交换位置
                    cbTempData = cbCardData[i];
                    cbCardData[i] = cbCardData[i + 1];
                    cbCardData[i + 1] = cbTempData;
                    cbTempData = cbLogicValue[i];
                    cbLogicValue[i] = cbLogicValue[i + 1];
                    cbLogicValue[i + 1] = cbTempData;
                    bSorted = false;
                }
            }
            bLast--;
        } while (bSorted == false);

        return;
    }
    //获取牌型值
    BYTE CShowHandLogic::GetCardGenreValue(BYTE cbCardData[], BYTE cbCardCount)
    {
        //获取类型
        BYTE cbGenre = GetCardGenre(cbCardData, cbCardCount);
        BYTE retValue = 0;
        //类型对比
        switch(cbGenre) {
            case CT_SINGLE:            //单牌
            {
                for(BYTE i = 0; i < cbCardCount; i++){
                    BYTE cbValue = GetCardLogicValue(cbCardData[i]);
                    if(cbValue > retValue){
                        retValue = cbValue;
                    }
                }
                return retValue;
            }
            case CT_HU_LU:              //葫芦
            case CT_TIE_ZHI:            //铁支
            case CT_ONE_DOUBLE:         //对子
            case CT_TWO_DOUBLE:         //两对
            case CT_THREE_TIAO:         //三条
            {
                //分析扑克
                tagAnalyseResult AnalyseResult;
                 AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);
                //四条数值
                if (AnalyseResult.cbFourCount > 0) {
                    retValue = AnalyseResult.cbFourLogicVolue[0];
                    return retValue;
                }
                //三条数值
                if (AnalyseResult.cbThreeCount > 0) {
                    retValue = AnalyseResult.cbThreeLogicVolue[0];
                    return retValue;
                }
                //对子数值
                for (BYTE i = 0; i < AnalyseResult.cbDoubleCount; i++) {
                    BYTE cbValue = AnalyseResult.cbDoubleLogicVolue[i];
                    if (cbValue > retValue){
                        retValue = cbValue;
                    }
                    if( i == AnalyseResult.cbDoubleCount-1){
                        return retValue;
                    }
                }
                //散牌数值
                for (BYTE i = 0; i < AnalyseResult.cbSignedCount; i++) {
                    BYTE cbValue = AnalyseResult.cbSignedLogicVolue[i];
                    if ( cbValue > retValue){
                        retValue = cbValue;
                    }
                    if(i == AnalyseResult.cbSignedCount-1){
                        return retValue;
                    }
                }
                break;
            }
            case CT_SHUN_ZI:         //顺子
            case CT_TONG_HUA:        //同花
            case CT_TONG_HUA_SHUN:   //同花顺
            {
                //数值判断
                BYTE cbValue = GetCardLogicValue(cbCardData[0]);

//#ifdef    MAGIC_A_CARD
//                //特殊列:A J 10 9 8
//                if (cbGenre != CT_TONG_HUA) {
//                    BYTE cbMinusCount = cbCardCount == 5 ? 3 : 4;
//                    if (GetCardValue(cbCardData[0]) == 1 &&
//                        GetCardLogicValue(cbCardData[0]) - cbMinusCount == GetCardLogicValue(cbCardData[1]))
//                        cbValue = GetCardLogicValue(cbCardData[1]);
//                }
//#endif
                retValue = cbValue;
                return retValue;
            }
        }


        return retValue;
    }
    //是否匹配做牌牌型
    bool CShowHandLogic::IsMatchedCardType(BYTE cbCardData[],BYTE count,bool type5,bool type3)
    {
        SortCardList(cbCardData,MAX_COUNT);
        uint8 cardType5 = GetCardGenre(cbCardData,MAX_COUNT);
		if (type5 && cardType5 < CT_ONE_DOUBLE)
		{
			return false;
		}
        uint8 tmp[3];
        memcpy(tmp,cbCardData,3);
        SortCardList(tmp,3);			    
        uint8 cardType3 = GetCardGenre(tmp,3);
		if (type3 && cardType3 < CT_ONE_DOUBLE)
		{
			return false;
		}
	    return true;      
    }

	//随机牌牌型
	bool CShowHandLogic::GetRandCardType(BYTE cbCardData[], BYTE count, bool & type5, bool & type3)
	{
		SortCardList(cbCardData, MAX_COUNT);
		uint8 cardType5 = GetCardGenre(cbCardData, MAX_COUNT);
		if (cardType5 < CT_ONE_DOUBLE) {
			type5 = false;
		}
		else {
			type5 = true;
		}
		uint8 tmp[3];
		memcpy(tmp, cbCardData, 3);
		SortCardList(tmp, 3);
		uint8 cardType3 = GetCardGenre(tmp, 3);
		if (cardType3 < CT_ONE_DOUBLE)
		{
			type3 = false;
		}
		else {
			type3 = true;
		}
		return true;
	}

    bool CShowHandLogic::CanPlayFirstCardValue(BYTE cbHandCardData[],BYTE cbCardCount)
    {
        bool  sameColor = false; // 同色
        bool  duizi     = false; // 对子
        bool  lianshun  = false; // 连顺          
        
        if(GetCardColor(cbHandCardData[0]) == GetCardColor(cbHandCardData[1])){
            sameColor = true;
        }
        int32 values[2];
        values[0] = GetCardLogicValue(cbHandCardData[0]);
        values[1] = GetCardLogicValue(cbHandCardData[1]);
        if(values[0] == values[1]){
            duizi = true;
        }
        if(abs(values[0]-values[1]) == 1){
            lianshun = true;
        }
        if(sameColor || duizi || lianshun)
            return true;
        
        return false;           
    }
    //洗牌换牌
    void CShowHandLogic::GetLeftCard(BYTE cbCardBuffer[],BYTE cbBufferCount,vector<uint8>& leftCards)
    {
        BYTE datalen = getArrayLen(m_cbCardListData);
        BYTE cbCardData[datalen];
        memcpy(cbCardData, m_cbCardListData, sizeof(m_cbCardListData));
        for(uint16 i=0;i<datalen;++i)
        {
            for(uint16 j=0;j<cbBufferCount;++j){
                if(cbCardData[i] == cbCardBuffer[j])
                {
                    cbCardData[i] = 0;
                }
            }              
        }
        for(uint16 i=0;i<datalen;++i)
        {
            if(cbCardData[i] != 0)
            {
                leftCards.push_back(cbCardData[i]);
            }                          
        }           
    }    
    //混乱扑克
    void CShowHandLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount) {

        //混乱准备
		BYTE datalen = MAX_SHOWHAND_POKER;// getArrayLen(m_cbCardListData);
        BYTE cbCardData[MAX_SHOWHAND_POKER];
        memcpy(cbCardData, m_cbCardListData, sizeof(m_cbCardListData));

        //混乱扑克
        BYTE cbRandCount = 0, cbPosition = 0;
        do
        {
            //cbPosition=(BYTE)(g_RandGen.RandUInt()%(datalen-cbRandCount));
            cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
            cbCardData[cbPosition]=cbCardData[datalen-cbRandCount];
        } while (cbRandCount<cbBufferCount);

        return;
    }

//逻辑数值
    BYTE CShowHandLogic::GetCardLogicValue(BYTE cbCardData) {
        //转换数值
        BYTE cbCardValue = GetCardValue(cbCardData);
        BYTE cbLogicValue = (cbCardValue <= 2) ? (cbCardValue + 13) : cbCardValue;

        return cbLogicValue;
    }

//对比扑克
    bool CShowHandLogic::CompareCard(BYTE cbFirstCardData[], BYTE cbNextCardData[], BYTE cbCardCount) {
        //获取类型
        BYTE cbNextGenre = GetCardGenre(cbNextCardData, cbCardCount);
        BYTE cbFirstGenre = GetCardGenre(cbFirstCardData, cbCardCount);

		printf("show_hand_server CompareCard - cbFirstGenre:%d,cbNextGenre:%d\n", cbFirstGenre,cbNextGenre);


        //类型判断
		if (cbFirstGenre != cbNextGenre)
		{
			return (cbFirstGenre > cbNextGenre);
		}

        //类型对比
        switch (cbFirstGenre)
		{
            case CT_SINGLE:            //单牌
            {
                //对比数值
                for (BYTE i = 0; i < cbCardCount; i++) {
                    BYTE cbNextValue = GetCardLogicValue(cbNextCardData[i]);
                    BYTE cbFirstValue = GetCardLogicValue(cbFirstCardData[i]);
                    if (cbFirstValue != cbNextValue) return cbFirstValue > cbNextValue;
                }

                //对比花色
                return GetCardColor(cbFirstCardData[0]) > GetCardColor(cbNextCardData[0]);
            }
            case CT_HU_LU:              //葫芦
            case CT_TIE_ZHI:            //铁支
            case CT_ONE_DOUBLE:         //对子
            case CT_TWO_DOUBLE:         //两对
            case CT_THREE_TIAO:         //三条
            {
                //分析扑克
                tagAnalyseResult AnalyseResultNext;
                tagAnalyseResult AnalyseResultFirst;
                AnalysebCardData(cbNextCardData, cbCardCount, AnalyseResultNext);
                AnalysebCardData(cbFirstCardData, cbCardCount, AnalyseResultFirst);

				printf("show_hand_server AnalysebCardData - cbFourCount:%d,cbThreeCount:%d,cbDoubleCount:%d,cbSignedCount:%d,\n",
					AnalyseResultFirst.cbFourCount, AnalyseResultFirst.cbThreeCount,AnalyseResultFirst.cbDoubleCount, AnalyseResultFirst.cbSignedCount);

                //四条数值
                if (AnalyseResultFirst.cbFourCount > 0) {
                    BYTE cbNextValue = AnalyseResultNext.cbFourLogicVolue[0];
                    BYTE cbFirstValue = AnalyseResultFirst.cbFourLogicVolue[0];
                    return cbFirstValue > cbNextValue;
                }

                //三条数值
                if (AnalyseResultFirst.cbThreeCount > 0) {
                    BYTE cbNextValue = AnalyseResultNext.cbThreeLogicVolue[0];
                    BYTE cbFirstValue = AnalyseResultFirst.cbThreeLogicVolue[0];
                    return cbFirstValue > cbNextValue;
                }

                //对子数值
                for (BYTE i = 0; i < AnalyseResultFirst.cbDoubleCount; i++) {
                    BYTE cbNextValue = AnalyseResultNext.cbDoubleLogicVolue[i];
                    BYTE cbFirstValue = AnalyseResultFirst.cbDoubleLogicVolue[i];
                    if (cbFirstValue != cbNextValue) return cbFirstValue > cbNextValue;
                }

                //散牌数值
                for (BYTE i = 0; i < AnalyseResultFirst.cbSignedCount; i++) {
                    BYTE cbNextValue = AnalyseResultNext.cbSignedLogicVolue[i];
                    BYTE cbFirstValue = AnalyseResultFirst.cbSignedLogicVolue[i];
                    if (cbFirstValue != cbNextValue) return cbFirstValue > cbNextValue;
                }

                //对子花色
                if (AnalyseResultFirst.cbDoubleCount > 0)
				{
                    BYTE cbNextColor = GetCardColor(AnalyseResultNext.cbDoubleCardData[0]);
                    BYTE cbFirstColor = GetCardColor(AnalyseResultFirst.cbDoubleCardData[0]);

					printf("show_hand_server CompareCard - cbColor:0x%02X 0x%02X,cbDoubleCardData:0x%02X 0x%02X\n", 
						cbFirstColor,cbNextColor, AnalyseResultFirst.cbDoubleCardData[0], AnalyseResultNext.cbDoubleCardData[0]);
					return cbFirstColor > cbNextColor;
                }

                //散牌花色
                if (AnalyseResultFirst.cbSignedCount > 0) {
                    BYTE cbNextColor = GetCardColor(AnalyseResultNext.cbSignedCardData[0]);
                    BYTE cbFirstColor = GetCardColor(AnalyseResultFirst.cbSignedCardData[0]);
                    return cbFirstColor > cbNextColor;
                }

                break;
            }
            case CT_SHUN_ZI:        //顺子
            case CT_TONG_HUA:        //同花
            case CT_TONG_HUA_SHUN:    //同花顺
            {

//#ifdef    MAGIC_A_CARD
//                //特殊列:A J 10 9 8
//                if (cbFirstGenre != CT_TONG_HUA) {
//                    BYTE cbMinusCount = cbCardCount == 5 ? 3 : 4;
//                    if (GetCardValue(cbNextCardData[0]) == 1 &&
//                        GetCardLogicValue(cbNextCardData[0]) - cbMinusCount == GetCardLogicValue(cbNextCardData[1]))
//                        cbNextValue = GetCardLogicValue(cbNextCardData[1]);
//                    if (GetCardValue(cbFirstCardData[0]) == 1 &&
//                        GetCardLogicValue(cbFirstCardData[0]) - cbMinusCount == GetCardLogicValue(cbFirstCardData[1]))
//                        cbFirstValue = GetCardLogicValue(cbFirstCardData[1]);
//                }
//#endif
//数值判断
				//BYTE cbNextValue = GetCardLogicValue(cbNextCardData[0]);
				//BYTE cbFirstValue = GetCardLogicValue(cbFirstCardData[0]);
				//if (cbFirstValue != cbNextValue) return (cbFirstValue > cbNextValue);

				for (BYTE i = 0; i < cbCardCount; i++)
				{
					BYTE cbNextValue = GetCardLogicValue(cbNextCardData[i]);
					BYTE cbFirstValue = GetCardLogicValue(cbFirstCardData[i]);
					if (cbFirstValue != cbNextValue) return cbFirstValue > cbNextValue;
				}
                //花色判断
                BYTE cbNextColor = GetCardColor(cbNextCardData[0]);
                BYTE cbFirstColor = GetCardColor(cbFirstCardData[0]);

                return (cbFirstColor > cbNextColor);
            }
        }


        return false;
    }


//分析扑克
    void CShowHandLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult &AnalyseResult) {
        //设置结果
        memset(&AnalyseResult,0, sizeof(AnalyseResult));

        //扑克分析
        for (BYTE i = 0; i < cbCardCount; i++) {
            //变量定义
            BYTE cbSameCount = 1;
            BYTE cbSameCardData[4] = {cbCardData[i], 0, 0, 0};
            BYTE cbLogicValue = GetCardLogicValue(cbCardData[i]);

            //获取同牌
            for (int j = i + 1; j < cbCardCount; j++) {
                //逻辑对比
                if (GetCardLogicValue(cbCardData[j]) != cbLogicValue) break;

                //设置扑克
                cbSameCardData[cbSameCount++] = cbCardData[j];
            }

            //保存结果
            switch (cbSameCount) {
                case 1:        //单张
                {
                    AnalyseResult.cbSignedLogicVolue[AnalyseResult.cbSignedCount] = cbLogicValue;
                    memcpy(&AnalyseResult.cbSignedCardData[(AnalyseResult.cbSignedCount++) * cbSameCount],
                               cbSameCardData, cbSameCount);
                    break;
                }
                case 2:        //两张
                {
                    AnalyseResult.cbDoubleLogicVolue[AnalyseResult.cbDoubleCount] = cbLogicValue;
                    memcpy(&AnalyseResult.cbDoubleCardData[(AnalyseResult.cbDoubleCount++) * cbSameCount],
                               cbSameCardData, cbSameCount);
                    break;
                }
                case 3:        //三张
                {
                    AnalyseResult.cbThreeLogicVolue[AnalyseResult.cbThreeCount] = cbLogicValue;
                    memcpy(&AnalyseResult.cbThreeCardData[(AnalyseResult.cbThreeCount++) * cbSameCount],
                               cbSameCardData, cbSameCount);
                    break;
                }
                case 4:        //四张
                {
                    AnalyseResult.cbFourLogicVolue[AnalyseResult.cbFourCount] = cbLogicValue;
                    memcpy(&AnalyseResult.cbFourCardData[(AnalyseResult.cbFourCount++) * cbSameCount],
                               cbSameCardData, cbSameCount);
                    break;
                }
            }

            //设置递增
            i += cbSameCount - 1;
        }

        return;
    }


	//	    0x01, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,                                    //方块 A - K
	//		0x11, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,                                    //梅花 A - K
	//		0x21, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,                                    //红桃 A - K
	//		0x31, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,                                    //黑桃 A - K


	void CShowHandLogic::testShowHand()
	{
		unsigned char cbFirstCardData[] = { 0x31,0x0B,0x1A,0x29,0x19};
		unsigned char cbNextCardData[]  = { 0x11,0x3B,0x0A,0x39,0x09 };

		bool bCompareCard = CompareCard(cbNextCardData, cbFirstCardData, 5);

		printf("show_hand_server CompareCard - bCompareCard:%d\n", bCompareCard);

	}

};
