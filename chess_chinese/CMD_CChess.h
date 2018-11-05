#ifndef CMD_CHESS_HEAD_FILE
#define CMD_CHESS_HEAD_FILE

/*

白 cbColor - 2 cbUserIndex:1

0     1     2     3     4     5     6     7     8
┏━━┯━━┯━━┯━━┯━━┯━━┯━━┯━━┓0
┃	  │    │	  │    │	  │	│	  │    ┃
┠──┼──┼──┼──┼──┼──┼──┼──┨1
┃	  │    │	  │    │	  │	│	  │    ┃
┠──┼──┼──┼──┼──┼──┼──┼──┨2
┃	  │    │	  │    │	  │	│	  │    ┃
┠──┼──┼──┼──┼──┼──┼──┼──┨3
┃	  │    │	  │    │	  │	│	  │    ┃
┠──┼──┼──┼──┼──┼──┼──┼──┨4
┃	                                            ┃
┃	       楚河                    汉界         ┃
┃	                                            ┃
┠──┼──┼──┼──┼──┼──┼──┼──┨5
┃	  │    │	  │    │	  │	│	  │    ┃
┠──┼──┼──┼──┼──┼──┼──┼──┨6
┃	  │    │	  │    │	  │	│	  │    ┃
┠──┼──┼──┼──┼──┼──┼──┼──┨7
┃	  │    │	  │    │	  │	│	  │    ┃
┠──┼──┼──┼──┼──┼──┼──┼──┨8
┃	  │    │	  │    │	  │	│	  │    ┃
┗━━┷━━┷━━┷━━┷━━┷━━┷━━┷━━┛9
0      1     2     3     4     5     6     7    8

黑 cbColor - 1 cbUserIndex:0

*/

/*


ResetChessBorad 主线棋子 - cbUserIndex:0,nIndex:0,cbXPos:0, cbYPos:9, cbChessID:00, cbChess:5, cbChessName:车, cbColor:1
ResetChessBorad 主线棋子 - cbUserIndex:0,nIndex:1,cbXPos:1, cbYPos:9, cbChessID:01, cbChess:4, cbChessName:马, cbColor:1
ResetChessBorad 主线棋子 - cbUserIndex:0,nIndex:2,cbXPos:2, cbYPos:9, cbChessID:02, cbChess:3, cbChessName:象, cbColor:1
ResetChessBorad 主线棋子 - cbUserIndex:0,nIndex:3,cbXPos:3, cbYPos:9, cbChessID:03, cbChess:2, cbChessName:士, cbColor:1
ResetChessBorad 主线棋子 - cbUserIndex:0,nIndex:4,cbXPos:4, cbYPos:9, cbChessID:04, cbChess:1, cbChessName:将, cbColor:1
ResetChessBorad 主线棋子 - cbUserIndex:0,nIndex:5,cbXPos:5, cbYPos:9, cbChessID:05, cbChess:2, cbChessName:士, cbColor:1
ResetChessBorad 主线棋子 - cbUserIndex:0,nIndex:6,cbXPos:6, cbYPos:9, cbChessID:06, cbChess:3, cbChessName:象, cbColor:1
ResetChessBorad 主线棋子 - cbUserIndex:0,nIndex:7,cbXPos:7, cbYPos:9, cbChessID:07, cbChess:4, cbChessName:马, cbColor:1
ResetChessBorad 主线棋子 - cbUserIndex:0,nIndex:8,cbXPos:8, cbYPos:9, cbChessID:08, cbChess:5, cbChessName:车, cbColor:1


ResetChessBorad 兵的棋子 - cbUserIndex:0,nIndex:0,cbXPos:0, cbYPos:6, cbChessID:09, cbChess:7, cbChessName:兵, cbColor:1
ResetChessBorad 兵的棋子 - cbUserIndex:0,nIndex:1,cbXPos:2, cbYPos:6, cbChessID:10, cbChess:7, cbChessName:兵, cbColor:1
ResetChessBorad 兵的棋子 - cbUserIndex:0,nIndex:2,cbXPos:4, cbYPos:6, cbChessID:11, cbChess:7, cbChessName:兵, cbColor:1
ResetChessBorad 兵的棋子 - cbUserIndex:0,nIndex:3,cbXPos:6, cbYPos:6, cbChessID:12, cbChess:7, cbChessName:兵, cbColor:1
ResetChessBorad 兵的棋子 - cbUserIndex:0,nIndex:4,cbXPos:8, cbYPos:6, cbChessID:13, cbChess:7, cbChessName:兵, cbColor:1


ResetChessBorad 炮的棋子 - cbUserIndex:0,nIndex:0,cbXPos:1, cbYPos:7, cbChessID:14, cbChess:6, cbChessName:炮, cbColor:1
ResetChessBorad 炮的棋子 - cbUserIndex:0,nIndex:1,cbXPos:7, cbYPos:7, cbChessID:15, cbChess:6, cbChessName:炮, cbColor:1


ResetChessBorad 主线棋子 - cbUserIndex:1,nIndex:0,cbXPos:0, cbYPos:0, cbChessID:00, cbChess:5, cbChessName:车, cbColor:2
ResetChessBorad 主线棋子 - cbUserIndex:1,nIndex:1,cbXPos:1, cbYPos:0, cbChessID:01, cbChess:4, cbChessName:马, cbColor:2
ResetChessBorad 主线棋子 - cbUserIndex:1,nIndex:2,cbXPos:2, cbYPos:0, cbChessID:02, cbChess:3, cbChessName:象, cbColor:2
ResetChessBorad 主线棋子 - cbUserIndex:1,nIndex:3,cbXPos:3, cbYPos:0, cbChessID:03, cbChess:2, cbChessName:士, cbColor:2
ResetChessBorad 主线棋子 - cbUserIndex:1,nIndex:4,cbXPos:4, cbYPos:0, cbChessID:04, cbChess:1, cbChessName:将, cbColor:2
ResetChessBorad 主线棋子 - cbUserIndex:1,nIndex:5,cbXPos:5, cbYPos:0, cbChessID:05, cbChess:2, cbChessName:士, cbColor:2
ResetChessBorad 主线棋子 - cbUserIndex:1,nIndex:6,cbXPos:6, cbYPos:0, cbChessID:06, cbChess:3, cbChessName:象, cbColor:2
ResetChessBorad 主线棋子 - cbUserIndex:1,nIndex:7,cbXPos:7, cbYPos:0, cbChessID:07, cbChess:4, cbChessName:马, cbColor:2
ResetChessBorad 主线棋子 - cbUserIndex:1,nIndex:8,cbXPos:8, cbYPos:0, cbChessID:08, cbChess:5, cbChessName:车, cbColor:2


ResetChessBorad 兵的棋子 - cbUserIndex:1,nIndex:0,cbXPos:0, cbYPos:3, cbChessID:09, cbChess:7, cbChessName:兵, cbColor:2
ResetChessBorad 兵的棋子 - cbUserIndex:1,nIndex:1,cbXPos:2, cbYPos:3, cbChessID:10, cbChess:7, cbChessName:兵, cbColor:2
ResetChessBorad 兵的棋子 - cbUserIndex:1,nIndex:2,cbXPos:4, cbYPos:3, cbChessID:11, cbChess:7, cbChessName:兵, cbColor:2
ResetChessBorad 兵的棋子 - cbUserIndex:1,nIndex:3,cbXPos:6, cbYPos:3, cbChessID:12, cbChess:7, cbChessName:兵, cbColor:2
ResetChessBorad 兵的棋子 - cbUserIndex:1,nIndex:4,cbXPos:8, cbYPos:3, cbChessID:13, cbChess:7, cbChessName:兵, cbColor:2


ResetChessBorad 炮的棋子 - cbUserIndex:1,nIndex:0,cbXPos:1, cbYPos:2, cbChessID:14, cbChess:6, cbChessName:炮, cbColor:2
ResetChessBorad 炮的棋子 - cbUserIndex:1,nIndex:1,cbXPos:7, cbYPos:2, cbChessID:15, cbChess:6, cbChessName:炮, cbColor:2


*/

#include <assert.h>
#include <string>

#define ASSERT(EXPR)\
do{\
  if (!(EXPR)){assert(false);}\
}while(0);

//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))


#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

//游戏类型
#define GAME_GENRE_SCORE				0x0001								//点值类型
#define GAME_GENRE_GOLD					0x0002								//金币类型
#define GAME_GENRE_MATCH				0x0004								//比赛类型
#define GAME_GENRE_EDUCATE				0x0008								//训练类型

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef short SHORT;
typedef long LONG;

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						501									//游戏 I D
#define GAME_PLAYER					2									//游戏人数
#define GAME_NAME					("中国象棋")						//游戏名字
#define GAME_GENRE					GAME_GENRE_SCORE        			//游戏类型

//////////////////////////////////////////////////////////////////////////
//游戏定义

//棋子颜色
#define NO_CHESS					0									//没有棋子
#define BLACK_CHESS					1									//黑色棋子
#define WHITE_CHESS					2									//白色棋子

//棋子定义
#define CHESS_GENERAL				1									//将
#define CHESS_KAVASS				2									//士
#define CHESS_ELEPHANT				3									//象
#define CHESS_HORSE					4									//马
#define CHESS_ROOK					5									//车
#define CHESS_CANNON				6									//炮
#define CHESS_SOLDIER				7									//兵

static const std::string g_chess_name[] = {"","将","士","象","马","车","炮","兵"};

//请求状态
#define GU_WAIT_PEACE				0x01								//等待求和
#define GU_WAIT_REGRET				0x02								//等待悔棋

//失败原因
#define FR_RULE_LIMIT				1									//规则限制
#define FR_COUNT_LIMIT				2									//次数限制
#define FR_PLAYER_OPPOSE			3									//玩家反对

//对局模式
#define GM_MATCH					0									//比赛模式
#define GM_FRIEND					1									//友谊模式

//////////////////////////////////////////////////////////////////////////

//棋子定义
struct tagChessItem
{
	BYTE							cbXPos;								//棋子坐标
	BYTE							cbYPos;								//棋子坐标
	BYTE							cbColor;							//棋子颜色
	BYTE							cbChess;							//棋子权位
	BYTE							cbChessID;							//棋子标识
};

//棋谱结构
struct tagChessManual
{
	//移动棋子
	BYTE							cbSourceID;							//棋子标识
	BYTE							cbSourceColor;						//棋子颜色
	BYTE							cbSourceChess;						//棋子权位
	BYTE							cbXSourceChessPos;					//棋子位置
	BYTE							cbYSourceChessPos;					//棋子位置
	BYTE							cbXTargetChessPos;					//棋子位置
	BYTE							cbYTargetChessPos;					//棋子位置

	//关联棋子
	BYTE							cbUniteID;							//棋子标识
	BYTE							cbUniteChess;						//棋子权位
};

//规则结构
struct tagGameRuleInfo
{
	BYTE							cbGameMode;							//对局模式
	BYTE							cbDirections;						//提示标志
	BYTE							cbRegretFlag;						//允许悔棋
	WORD							wRuleStepTime;						//游戏步时
	WORD							wRuleLimitTime;						//限定时间
	WORD							wRuleTimeOutCount;					//超时次数
};

//////////////////////////////////////////////////////////////////////////

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_MOVE_CHESS			101									//移动棋子
#define SUB_S_REGRET_REQ			102									//悔棋请求
#define SUB_S_REGRET_FAILE			103									//悔棋失败
#define SUB_S_REGRET_RESULT			104									//悔棋结果
#define SUB_S_PEACE_REQ				105									//和棋请求
#define SUB_S_PEACE_ANSWER			106									//和棋应答
#define SUB_S_GAME_END				107									//游戏结束
#define SUB_S_CHESS_MANUAL			108									//棋谱信息
#define SUB_S_REQ_SET_RULE			109									//请求设置
#define SUB_S_REQ_AGREE_RULE		110									//请求同意

//游戏状态
struct CMD_S_StatusFree
{
	WORD							wBlackUser;							//黑棋玩家
	BYTE							cbGameRule;							//规则标志
	tagGameRuleInfo					GameRuleInfo;						//游戏规则
};

//游戏状态
struct CMD_S_StatusPlay
{
	WORD							wGameClock;							//局时时间
	WORD							wBlackUser;							//黑棋玩家
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wTimeOutCount;						//超时次数
	WORD							wUseClock[2];						//使用时间
	WORD							wLeftClock[2];						//局时时间
	WORD							cbBegStatus[2];						//请求状态
	WORD							wChessStepCount[2];					//棋子步数
	tagChessItem					ChessItemArray[2][16];				//棋子信息
	tagGameRuleInfo					GameRuleInfo;						//游戏规则
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wBlackUser;							//黑棋玩家
	tagGameRuleInfo					GameRuleInfo;						//游戏规则
};

//放置棋子
struct CMD_S_MoveChess
{
	BYTE							cbXSourcePos;						//棋子位置
	BYTE							cbYSourcePos;						//棋子位置
	BYTE							cbXTargetPos;						//棋子位置
	BYTE							cbYTargetPos;						//棋子位置
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wUseClock[2];						//使用时间
	WORD							wLeftClock[2];						//局时时间
};

//悔棋失败
struct CMD_S_RegretFaile
{
	BYTE							cbFaileReason;						//失败原因
};

//悔棋结果
struct CMD_S_RegretResult
{
	WORD							wRegretUser;						//悔棋玩家
	WORD							wCurrentUser;						//当前玩家
	BYTE							cbRegretCount;						//悔棋数目
	WORD							wUseClock[2];						//使用时间
	WORD							wLeftClock[2];						//局时时间
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG							lGameTax;							//游戏税收
	WORD							wWinUser;							//胜利玩家
	LONG							lUserScore[2];						//用户积分
};

//请求设置
struct CMD_S_Req_SetRult
{
	tagGameRuleInfo					GameRuleInfo;						//游戏规则
};

//请求同意
struct CMD_S_Req_AgreeRult
{
	tagGameRuleInfo					GameRuleInfo;						//游戏规则
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_MOVE_CHESS			1									//移动棋子
#define SUB_C_TIME_OUT				2									//走棋超时
#define SUB_C_REGRET_REQ			3									//悔棋请求
#define SUB_C_REGRET_ANSWER			4									//悔棋应答
#define SUB_C_PEACE_REQ				5									//和棋请求
#define SUB_C_PEACE_ANSWER			6									//和棋应答
#define SUB_C_GIVEUP_REQ			7									//认输请求

//移动棋子
struct CMD_C_MoveChess
{
	BYTE							cbXSourcePos;						//棋子位置
	BYTE							cbYSourcePos;						//棋子位置
	BYTE							cbXTargetPos;						//棋子位置
	BYTE							cbYTargetPos;						//棋子位置
};

//悔棋响应
struct CMD_C_RegretAnswer
{
	BYTE							cbApprove;							//同意标志
};

//和棋响应
struct CMD_C_PeaceAnswer
{
	BYTE							cbApprove;							//同意标志
};

//定制规则
struct CMD_C_CustomizeRult
{
	tagGameRuleInfo					GameRuleInfo;						//游戏规则
};

//////////////////////////////////////////////////////////////////////////

#endif