
//-------------------------------------------------------------------------------------------


#include <stdio.h>
#include <time.h>
#include "GameLogic.h"

//-------------------------------------------------------------------------------------------
// Minimum and maximum macros
#define __max(a,b) (((a) > (b)) ? (a) : (b))
#define __min(a,b) (((a) < (b)) ? (a) : (b))
//-------------------------------------------------------------------------------------------

CGameLogic m_GameLogic;	//游戏逻辑

//规则变量
bool							m_bGameRule;						//规则标志
tagGameRuleInfo					m_GameRuleInfo;						//游戏规则

//时间状态
DWORD							m_dwLastClock;						//上次时间
WORD							m_wUseClock[2];						//使用时间
WORD							m_wLeftClock[2];					//剩余时间
WORD							m_wTimeOutCount[2];					//超时次数

//用户状态
WORD							m_wBlackUser;						//黑棋玩家
WORD							m_wCurrentUser;						//当前用户
WORD							m_cbBegStatus[2];					//请求状态
BYTE							m_cbPeaceCount[2];					//求和次数
BYTE							m_cbRegretCount[2];					//悔棋次数

//-------------------------------------------------------------------------------------------

#define RECTIFY_TIME				3										//调整时间
#define RULE_STEP_TIME				30										//规则步时
#define RULE_LIMIT_TIME				1800									//规则限时
#define RULE_TIME_OUT_COUNT			3										//规则超时

//-------------------------------------------------------------------------------------------

//无效数值
#define INVALID_BYTE				((BYTE)(0xFF))						//无效数值
#define INVALID_WORD				((WORD)(0xFFFF))					//无效数值
#define INVALID_DWORD				((DWORD)(0xFFFFFFFF))				//无效数值

//无效数值
#define INVALID_TABLE				INVALID_WORD						//无效桌子
#define INVALID_CHAIR				INVALID_WORD						//无效椅子

//-------------------------------------------------------------------------------------------

void Init()
{
	//规则变量
	m_bGameRule = false;
	m_GameRuleInfo.cbDirections = TRUE;
	m_GameRuleInfo.cbRegretFlag = TRUE;
	m_GameRuleInfo.cbGameMode = GM_MATCH;
	m_GameRuleInfo.wRuleStepTime = RULE_STEP_TIME;
	m_GameRuleInfo.wRuleLimitTime = RULE_LIMIT_TIME;
	m_GameRuleInfo.wRuleTimeOutCount = RULE_TIME_OUT_COUNT;

	//时间状态
	m_dwLastClock = 0L;
	memset(m_wUseClock, 0, sizeof(m_wUseClock));
	memset(m_wLeftClock, 0, sizeof(m_wLeftClock));
	memset(m_wTimeOutCount, 0, sizeof(m_wTimeOutCount));

	//用户状态
	m_wBlackUser = 0;
	m_wCurrentUser = INVALID_CHAIR;
	memset(m_cbBegStatus, 0, sizeof(m_cbBegStatus));
	memset(m_cbPeaceCount, 0, sizeof(m_cbPeaceCount));
	memset(m_cbRegretCount, 0, sizeof(m_cbRegretCount));

	//组件变量

	return;
}


//移动棋子
bool OnUserMoveChess(WORD wChairID, BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos)
{
	//效验参数
	ASSERT((cbXSourcePos<9) && (cbYSourcePos<10));
	ASSERT((cbXTargetPos<9) && (cbYTargetPos<10));
	if ((cbXSourcePos >= 9) || (cbYSourcePos >= 10))
	{
		printf("OnUserMoveChess - error - 0\n");
		return false;
	}
	if ((cbXTargetPos >= 9) || (cbYTargetPos >= 10))
	{
		printf("OnUserMoveChess - error - 1\n");
		return false;
	}

	//点棋判断
	BYTE cbUserColor = (wChairID == m_wBlackUser) ? BLACK_CHESS : WHITE_CHESS;
	const tagChessItem * pSourceChessItem = m_GameLogic.GetChessItem(cbXSourcePos, cbYSourcePos);
	ASSERT((pSourceChessItem != NULL) && (pSourceChessItem->cbColor == cbUserColor));
	if ((pSourceChessItem == NULL) || (pSourceChessItem->cbColor != cbUserColor))
	{
		printf("OnUserMoveChess - error - 2\n");
		return false;
	}

	//移动棋子
	if (m_GameLogic.IsWalkLegality(pSourceChessItem, cbXTargetPos, cbYTargetPos) == false)
	{
		printf("OnUserMoveChess - error - 3\n");
		return false;
	}
	if (m_GameLogic.MoveChess(cbXSourcePos, cbYSourcePos, cbXTargetPos, cbYTargetPos) == false)
	{
		printf("OnUserMoveChess - error - 4\n");
		return false;
	}

	//结束判断
	bool bGameEnd = m_GameLogic.IsGameFinish((m_wCurrentUser == m_wBlackUser) ? WHITE_CHESS : BLACK_CHESS);

	//计算时间
	DWORD dwNowClock = (DWORD)time(NULL);
	WORD wUseClock = (WORD)(dwNowClock - m_dwLastClock);
	m_wUseClock[m_wCurrentUser] += (WORD)(dwNowClock - m_dwLastClock);
	if (m_wLeftClock[m_wCurrentUser] >= RECTIFY_TIME)
	{
		m_wLeftClock[m_wCurrentUser] -= __min(wUseClock, m_wLeftClock[m_wCurrentUser] - RECTIFY_TIME);
	}


	//设置变量
	m_dwLastClock = dwNowClock;
	m_wCurrentUser = (m_wCurrentUser + 1) % GAME_PLAYER;


	//构造数据
	CMD_S_MoveChess MoveChess;
	MoveChess.cbXSourcePos = cbXSourcePos;
	MoveChess.cbYSourcePos = cbYSourcePos;
	MoveChess.cbXTargetPos = cbXTargetPos;
	MoveChess.cbYTargetPos = cbYTargetPos;
	MoveChess.wCurrentUser = m_wCurrentUser;
	memcpy(MoveChess.wUseClock, m_wUseClock, sizeof(MoveChess.wUseClock));
	memcpy(MoveChess.wLeftClock, m_wLeftClock, sizeof(MoveChess.wLeftClock));

	//发送消息

	//结束游戏
	if (bGameEnd == true)
	{
		
	}
	printf("OnUserMoveChess - bGameEnd:%d\n", bGameEnd);
	return true;
}

bool OnEventGameStart()
{
	//时间变量
	m_wCurrentUser = m_wBlackUser;
	m_dwLastClock = (DWORD)time(NULL);
	m_wLeftClock[0] = m_wLeftClock[1] = m_GameRuleInfo.wRuleLimitTime;
	m_wTimeOutCount[0] = m_wTimeOutCount[1] = m_GameRuleInfo.wRuleTimeOutCount;

	//重置棋盘
	m_GameLogic.ResetChessBorad();
	return true;
}

void UpdateChess()
{
	// 炮居中
	WORD wChairID = 0;
	BYTE cbXSourcePos = 1;
	BYTE cbYSourcePos = 7;
	BYTE cbXTargetPos = 4;
	BYTE cbYTargetPos = 7;
	OnUserMoveChess(wChairID, cbXSourcePos, cbYSourcePos, cbXTargetPos, cbYTargetPos);

	// 上马
	wChairID = 1;
	cbXSourcePos = 0;
	cbYSourcePos = 7;
	cbXTargetPos = 4;
	cbYTargetPos = 7;
	OnUserMoveChess(wChairID, cbXSourcePos, cbYSourcePos, cbXTargetPos, cbYTargetPos);

}

int main(int argc, const char** argv)
{
	printf("hello world!\n");

	Init();
	OnEventGameStart();

	UpdateChess();

	return 0;
}



