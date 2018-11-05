
//-------------------------------------------------------------------------------------------


#include <stdio.h>
#include <time.h>
#include "GameLogic.h"

//-------------------------------------------------------------------------------------------

#define GAME_CLOCK					600										//游戏局时


CGameLogic m_GameLogic;	//游戏逻辑


void Init()
{

	return;
}


//放棋事件
bool OnUserPlaceChess(WORD wChairID, BYTE cbXPos, BYTE cbYPos)
{


	return true;
}

bool OnEventGameStart()
{

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



