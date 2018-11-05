#ifndef CMD_WEIQI_HEAD_FILE
#define CMD_WEIQI_HEAD_FILE

//-------------------------------------------------------------------------------------------

#include <vector>
#include <memory.h>
#include <assert.h>
#include <string>

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef short SHORT;
typedef long LONG;


//游戏类型
#define GAME_GENRE_SCORE				0x0001								//点值类型
#define GAME_GENRE_GOLD					0x0002								//金币类型
#define GAME_GENRE_MATCH				0x0004								//比赛类型
#define GAME_GENRE_EDUCATE				0x0008								//训练类型


#define ASSERT(EXPR)\
do{\
  if (!(EXPR)){assert(false);}\
}while(0);

//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

// Minimum and maximum macros
#define __max(a,b) (((a) > (b)) ? (a) : (b))
#define __min(a,b) (((a) < (b)) ? (a) : (b))

typedef unsigned long long DWORD_PTR;


#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))

#define EqualMemory(Destination,Source,Length) (!memcmp((Destination),(Source),(Length)))
#define MoveMemory(Destination,Source,Length) memmove((Destination),(Source),(Length))
#define CopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define FillMemory(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))


#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

//游戏类型
#define GAME_GENRE_SCORE				0x0001								//点值类型
#define GAME_GENRE_GOLD					0x0002								//金币类型
#define GAME_GENRE_MATCH				0x0004								//比赛类型
#define GAME_GENRE_EDUCATE				0x0008								//训练类型



//无效数值
#define INVALID_BYTE				((BYTE)(0xFF))						//无效数值
#define INVALID_WORD				((WORD)(0xFFFF))					//无效数值
#define INVALID_DWORD				((DWORD)(0xFFFFFFFF))				//无效数值

//无效数值
#define INVALID_TABLE				INVALID_WORD						//无效桌子
#define INVALID_CHAIR				INVALID_WORD						//无效椅子

//-------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						205									//游戏 I D
#define GAME_PLAYER					2									//游戏人数
#define GAME_NAME					("围棋游戏")					//游戏名字
#define GAME_GENRE					GAME_GENRE_SCORE					//游戏类型

//////////////////////////////////////////////////////////////////////////
//游戏定义

//棋子颜色
#define NO_CHESS					0									//没有棋子
#define BLACK_CHESS					1									//黑色棋子
#define WHITE_CHESS					2									//白色棋子

//请求状态
#define GU_WAIT_PEACE				0x01								//等待求和
#define GU_WAIT_REGRET				0x02								//等待悔棋
#define GU_WAIT_CHESS_COUNT			0x04								//等待点目

//失败原因
#define FR_RULE_LIMIT				1									//规则限制
#define FR_COUNT_LIMIT				2									//次数限制
#define FR_PLAYER_OPPOSE			3									//玩家反对

//对局模式
#define GM_MATCH					0									//比赛模式
#define GM_FRIEND					1									//友谊模式

//////////////////////////////////////////////////////////////////////////

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
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_PLACE_CHESS			101									//放置棋子
#define SUB_S_PASS_CHESS			102									//放弃下棋
#define SUB_S_REGRET_REQ			103									//悔棋请求
#define SUB_S_REGRET_FAILE			104									//悔棋失败
#define SUB_S_REGRET_RESULT			105									//悔棋结果
#define SUB_S_PEACE_REQ				106									//和棋请求
#define SUB_S_PEACE_ANSWER			107									//和棋应答
#define SUB_S_GAME_END				108									//游戏结束
#define SUB_S_CHESS_MANUAL			109									//棋谱信息
#define SUB_S_REQ_SET_RULE			110									//请求设置
#define SUB_S_REQ_AGREE_RULE		111									//请求同意
#define SUB_S_REQ_CHESS_COUNT		112									//请求点目
#define SUB_S_UNAGREE_CHESS_COUNT	113									//决绝点目

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
	WORD							wBlackUser;							//黑棋玩家
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wTimeOutCount;						//超时次数
	WORD							wUseClock[2];						//使用时间
	WORD							wLeftClock[2];						//局时时间
	WORD							cbBegStatus[2];						//请求状态
	tagGameRuleInfo					GameRuleInfo;						//游戏规则
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wBlackUser;							//黑棋玩家
	tagGameRuleInfo					GameRuleInfo;						//游戏规则
};

//放置棋子
struct CMD_S_PlaceChess
{
	BYTE							cbXPos;								//棋子位置
	BYTE							cbYPos;								//棋子位置
	WORD				 			wPlaceUser;							//放棋玩家
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wUseClock[2];						//使用时间
	WORD							wLeftClock[2];						//局时时间
};

//放弃下棋
struct CMD_S_PassChess
{
	WORD				 			wPassUser;							//放棋玩家
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
	WORD							wRegretCount;						//悔棋数目
	WORD							wUseClock[2];						//使用时间
	WORD							wLeftClock[2];						//局时时间
};

//游戏结束
struct CMD_S_GameEnd
{
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

#define SUB_C_PLACE_CHESS			1									//放置棋子
#define SUB_C_PASS_CHESS			2									//放弃下棋
#define SUB_C_TIME_OUT				3									//走棋超时
#define SUB_C_REGRET_REQ			4									//悔棋请求
#define SUB_C_REGRET_ANSWER			5									//悔棋应答
#define SUB_C_PEACE_REQ				6									//和棋请求
#define SUB_C_PEACE_ANSWER			7									//和棋应答
#define SUB_C_GIVEUP_REQ			8									//认输请求
#define SUB_C_REQ_CHESS_COUNT		9									//请求点目
#define SUB_C_UNAGREE_CHESS_COUNT	10									//拒绝点目

//放置棋子
struct CMD_C_PlaceChess
{
	BYTE							cbXPos;								//棋子位置
	BYTE							cbYPos;								//棋子位置
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