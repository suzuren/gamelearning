
#include "land_logic.h"
//const BYTE    CLandLogic::m_cbCardData[FULL_POKER_COUNT] =
//{
//	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,    //方块 A - K
//	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,    //梅花 A - K
//	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,    //红桃 A - K
//	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,    //黑桃 A - K
//	0x4E, 0x4F,
//};
int main(int argc, const char** argv)
{
	printf("hello world!\n");

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

	return 0;
}

//enum emLAND_CARD_TYPE
//{
//	CT_ERROR = 0,                                 //错误类型  0
//	CT_SINGLE,                                    //单牌类型  1
//	CT_DOUBLE,                                    //对牌类型  2
//	CT_THREE,                                     //三条类型  3
//	CT_SINGLE_LINE,                               //单连类型  4
//	CT_DOUBLE_LINE,                               //对连类型  5
//	CT_THREE_LINE,                                //三连类型  6
//	CT_THREE_TAKE_ONE,                            //三带一单  7
//	CT_THREE_TAKE_TWO,                            //三带一对  8
//	CT_FOUR_TAKE_ONE,                             //四带两单  9
//	CT_FOUR_TAKE_TWO,                             //四带两对  10
//	CT_BOMB_CARD,                                 //炸弹类型  11
//	CT_MISSILE_CARD,                              //火箭类型  12
//};