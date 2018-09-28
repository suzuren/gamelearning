
static const int  MAX_COUNT = 14;                                 // 手牌最大数目

static const int  MAX_WEAVE = 5;								  //最大组合
static const int  MAX_INDEX = 34;								  //最大索引
static const int  MAX_OPER_COUNT = 80;								  //最大操作类型

																	  // 扑克数值掩码
#define	MASK_COLOR			0xF0	//花色掩码
#define	MASK_VALUE			0x0F	//数值掩码

																	  //特殊牌标记
enum emPOKER_FLAG
{
	emPOKER_COLOR_WAN = 0,
	emPOKER_COLOR_SUO = 1,
	emPOKER_COLOR_TONG = 2,
	emPOKER_COLOR_ZI = 3,


	emPOKER_WAN1 = 0x01,
	emPOKER_SUO1 = 0x11,
	emPOKER_TONG1 = 0x21,
	emPOKER_DONG = 0x31,
	emPOKER_NAN = 0x32,
	emPOKER_XI = 0x33,
	emPOKER_BEI = 0x34,
	emPOKER_ZHONG = 0x35,
	emPOKER_FA = 0x36,
	emPOKER_BAI = 0x37,

};

typedef unsigned char		BYTE;
typedef unsigned int		uint32;
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef int					int32;

//////////////////////////////////////////////////////////////////////////////////
enum emMAJIANG_TYPE
{
	MAJIANG_TYPE_NULL = 0,
	MAJIANG_TYPE_CHANGSHA = 1,	// 长沙麻将
	MAJIANG_TYPE_ZHUANZHUAN = 2,	// 转转麻将
	MAJIANG_TYPE_NINGXIANG = 3,	// 宁乡麻将
	MAJIANG_TYPE_HONGZHONG = 4,	// 红中麻将
	MAJIANG_TYPE_GUOBIAO = 5,	// 国标麻将
	MAJIANG_TYPE_TWO_PEOPLE = 6,	// 二人麻将
};
//动作定义

//动作标志
enum emACTION_TYPE
{
	ACTION_NULL			= 0,	 // 没有类型
	ACTION_EAT_LEFT		= 1 << 1,	 // 左吃类型
	ACTION_EAT_CENTER	= 1 << 2,  // 中吃类型
	ACTION_EAT_RIGHT	= 1 << 3,  // 右吃类型
	ACTION_PENG			= 1 << 4,  // 碰牌类型
	ACTION_GANG			= 1 << 5,  // 杠牌类型
	ACTION_LISTEN		= 1 << 6,  // 听牌类型
	ACTION_CHI_HU		= 1 << 7,  // 吃胡类型
	ACTION_QIPAI_HU		= 1 << 8,  // 起手胡
	ACTION_NEED_TAIL	= 1 << 9,  // 要海底
	ACTION_GANG_TING	= 1 << 10, // 杠听(仅长沙麻将)

};

void test_majiang();


/*
[roomid:2,tableid:1,size:64,i:00,card:0x01,index:00
[roomid:2,tableid:1,size:64,i:01,card:0x01,index:00
[roomid:2,tableid:1,size:64,i:02,card:0x01,index:00
[roomid:2,tableid:1,size:64,i:03,card:0x01,index:00

[roomid:2,tableid:1,size:64,i:04,card:0x02,index:01
[roomid:2,tableid:1,size:64,i:05,card:0x02,index:01
[roomid:2,tableid:1,size:64,i:06,card:0x02,index:01
[roomid:2,tableid:1,size:64,i:07,card:0x02,index:01

[roomid:2,tableid:1,size:64,i:08,card:0x03,index:02
[roomid:2,tableid:1,size:64,i:09,card:0x03,index:02
[roomid:2,tableid:1,size:64,i:10,card:0x03,index:02
[roomid:2,tableid:1,size:64,i:11,card:0x03,index:02

[roomid:2,tableid:1,size:64,i:12,card:0x04,index:03
[roomid:2,tableid:1,size:64,i:13,card:0x04,index:03
[roomid:2,tableid:1,size:64,i:14,card:0x04,index:03
[roomid:2,tableid:1,size:64,i:15,card:0x04,index:03

[roomid:2,tableid:1,size:64,i:16,card:0x05,index:04
[roomid:2,tableid:1,size:64,i:17,card:0x05,index:04
[roomid:2,tableid:1,size:64,i:18,card:0x05,index:04
[roomid:2,tableid:1,size:64,i:19,card:0x05,index:04

[roomid:2,tableid:1,size:64,i:20,card:0x06,index:05
[roomid:2,tableid:1,size:64,i:21,card:0x06,index:05
[roomid:2,tableid:1,size:64,i:22,card:0x06,index:05
[roomid:2,tableid:1,size:64,i:23,card:0x06,index:05

[roomid:2,tableid:1,size:64,i:24,card:0x07,index:06
[roomid:2,tableid:1,size:64,i:25,card:0x07,index:06
[roomid:2,tableid:1,size:64,i:26,card:0x07,index:06
[roomid:2,tableid:1,size:64,i:27,card:0x07,index:06

[roomid:2,tableid:1,size:64,i:28,card:0x08,index:07
[roomid:2,tableid:1,size:64,i:29,card:0x08,index:07
[roomid:2,tableid:1,size:64,i:30,card:0x08,index:07
[roomid:2,tableid:1,size:64,i:31,card:0x08,index:07

[roomid:2,tableid:1,size:64,i:32,card:0x09,index:08
[roomid:2,tableid:1,size:64,i:33,card:0x09,index:08
[roomid:2,tableid:1,size:64,i:34,card:0x09,index:08
[roomid:2,tableid:1,size:64,i:35,card:0x09,index:08

[roomid:2,tableid:1,size:64,i:36,card:0x31,index:27
[roomid:2,tableid:1,size:64,i:37,card:0x32,index:28
[roomid:2,tableid:1,size:64,i:38,card:0x33,index:29
[roomid:2,tableid:1,size:64,i:39,card:0x34,index:30
[roomid:2,tableid:1,size:64,i:40,card:0x35,index:31
[roomid:2,tableid:1,size:64,i:41,card:0x36,index:32
[roomid:2,tableid:1,size:64,i:42,card:0x37,index:33

[roomid:2,tableid:1,size:64,i:43,card:0x31,index:27
[roomid:2,tableid:1,size:64,i:44,card:0x32,index:28
[roomid:2,tableid:1,size:64,i:45,card:0x33,index:29
[roomid:2,tableid:1,size:64,i:46,card:0x34,index:30
[roomid:2,tableid:1,size:64,i:47,card:0x35,index:31
[roomid:2,tableid:1,size:64,i:48,card:0x36,index:32
[roomid:2,tableid:1,size:64,i:49,card:0x37,index:33

[roomid:2,tableid:1,size:64,i:50,card:0x31,index:27
[roomid:2,tableid:1,size:64,i:51,card:0x32,index:28
[roomid:2,tableid:1,size:64,i:52,card:0x33,index:29
[roomid:2,tableid:1,size:64,i:53,card:0x34,index:30
[roomid:2,tableid:1,size:64,i:54,card:0x35,index:31
[roomid:2,tableid:1,size:64,i:55,card:0x36,index:32
[roomid:2,tableid:1,size:64,i:56,card:0x37,index:33

[roomid:2,tableid:1,size:64,i:57,card:0x31,index:27
[roomid:2,tableid:1,size:64,i:58,card:0x32,index:28
[roomid:2,tableid:1,size:64,i:59,card:0x33,index:29
[roomid:2,tableid:1,size:64,i:60,card:0x34,index:30
[roomid:2,tableid:1,size:64,i:61,card:0x35,index:31
[roomid:2,tableid:1,size:64,i:62,card:0x36,index:32
[roomid:2,tableid:1,size:64,i:63,card:0x37,index:33

[初始化牌池:m_poolCards:64,m_cbMustLeft:0]

BYTE    CGameMaJiangTable::PopCardFromPool()
{
BYTE card = 0;
if(m_poolCards.size() > 0)
{
card = m_poolCards.back();
m_poolCards.pop_back();
}
return card;
}
                 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 
strHandCardIndex:2 2 1 0 2 0 1 0 1 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  1  1  1  1  0  0

*/
