
//-------------------------------------------------------------------------------------------


#include <stdio.h>
#include <time.h>
#include "GameLogic.h"

//-------------------------------------------------------------------------------------------

#define GAME_CLOCK					600										//游戏局时


CGameLogic m_GameLogic;	//游戏逻辑

						//规则变量
	bool							m_bRestrict;						//是否禁手
	bool							m_bTradeUser;						//是否对换
	bool							m_bDoubleChess;						//是否双打

																		//时间变量
	DWORD							m_dwLastClock;						//上次时间
	WORD							m_wLeftClock[2];					//剩余时间

																		//用户状态
	WORD							m_wBlackUser;						//黑棋玩家
	WORD							m_wCurrentUser;						//当前用户
	WORD							m_cbBegStatus[2];					//请求状态
	BYTE							m_cbPeaceCount[2];					//求和次数
	BYTE							m_cbRegretCount[2];					//悔棋次数


void Init()
{
	//时间变量
	m_dwLastClock = 0L;
	memset(m_wLeftClock, 0, sizeof(m_wLeftClock));

	//用户状态
	m_wCurrentUser = INVALID_CHAIR;
	memset(m_cbBegStatus, 0, sizeof(m_cbBegStatus));
	memset(m_cbPeaceCount, 0, sizeof(m_cbPeaceCount));
	memset(m_cbRegretCount, 0, sizeof(m_cbRegretCount));

	return;
}


//放棋事件
bool OnUserPlaceChess(WORD wChairID, BYTE cbXPos, BYTE cbYPos)
{

	//效验用户
	ASSERT(m_wCurrentUser == wChairID);
	if (m_wCurrentUser != wChairID) return false;

	//放置棋子
	BYTE cbColor = (wChairID == m_wBlackUser) ? BLACK_CHESS : WHITE_CHESS;
	if (m_GameLogic.PlaceChess(cbXPos, cbYPos, cbColor) == 0) return false;

	//计算时间
	DWORD dwNowClock = (DWORD)time(NULL);
	WORD wUseClock = (WORD)(dwNowClock - m_dwLastClock);
	m_dwLastClock = dwNowClock;
	m_wLeftClock[m_wCurrentUser] -= __min(wUseClock, m_wLeftClock[m_wCurrentUser]);

	//结束判断
	bool bGameEnd = false;
	WORD wWinUser = INVALID_CHAIR;
	if (m_GameLogic.WinEstimate(cbXPos, cbYPos) == true)
	{
		bGameEnd = true;
		wWinUser = wChairID;
	}
	else if ((m_GameLogic.GetStepCount(BLACK_CHESS) + m_GameLogic.GetStepCount(WHITE_CHESS)) == 225)
	{
		bGameEnd = true;
		wWinUser = INVALID_CHAIR;
	}
	else
	{
		m_wCurrentUser = (m_wCurrentUser + 1) % GAME_PLAYER;
	}

	//构造数据
	CMD_S_PlaceChess PlaceChess;
	PlaceChess.cbXPos = cbXPos;
	PlaceChess.cbYPos = cbYPos;
	PlaceChess.wPlaceUser = wChairID;
	PlaceChess.wCurrentUser = m_wCurrentUser;
	memcpy(PlaceChess.wLeftClock, m_wLeftClock, sizeof(PlaceChess.wLeftClock));


	printf("OnUserPlaceChess - bGameEnd:%d,wWinUser:%d\n", bGameEnd, wWinUser);

	//结束游戏
	//if (bGameEnd == true) OnEventGameEnd(wWinUser, NULL, GER_NORMAL);

	return true;
}

bool OnEventGameStart()
{
	//设置变量
	m_wCurrentUser = m_wBlackUser;
	m_dwLastClock = (DWORD)time(NULL);
	m_wLeftClock[0] = m_wLeftClock[1] = GAME_CLOCK;


	//设置棋盘
	m_GameLogic.InitGameLogic();


	return true;
}

void UpdateChess()
{

}

int main(int argc, const char** argv)
{
	printf("hello world!\n");

	Init();
	OnEventGameStart();

	UpdateChess();

	return 0;
}



