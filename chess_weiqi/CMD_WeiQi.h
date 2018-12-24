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


//��Ϸ����
#define GAME_GENRE_SCORE				0x0001								//��ֵ����
#define GAME_GENRE_GOLD					0x0002								//�������
#define GAME_GENRE_MATCH				0x0004								//��������
#define GAME_GENRE_EDUCATE				0x0008								//ѵ������


#define ASSERT(EXPR)\
do{\
  if (!(EXPR)){assert(false);}\
}while(0);

//����ά��
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

//��Ϸ����
#define GAME_GENRE_SCORE				0x0001								//��ֵ����
#define GAME_GENRE_GOLD					0x0002								//�������
#define GAME_GENRE_MATCH				0x0004								//��������
#define GAME_GENRE_EDUCATE				0x0008								//ѵ������



//��Ч��ֵ
#define INVALID_BYTE				((BYTE)(0xFF))						//��Ч��ֵ
#define INVALID_WORD				((WORD)(0xFFFF))					//��Ч��ֵ
#define INVALID_DWORD				((DWORD)(0xFFFFFFFF))				//��Ч��ֵ

//��Ч��ֵ
#define INVALID_TABLE				INVALID_WORD						//��Ч����
#define INVALID_CHAIR				INVALID_WORD						//��Ч����

//-------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID						205									//��Ϸ I D
#define GAME_PLAYER					2									//��Ϸ����
#define GAME_NAME					("Χ����Ϸ")					//��Ϸ����
#define GAME_GENRE					GAME_GENRE_SCORE					//��Ϸ����

//////////////////////////////////////////////////////////////////////////
//��Ϸ����

//������ɫ
#define NO_CHESS					0									//û������
#define BLACK_CHESS					1									//��ɫ����
#define WHITE_CHESS					2									//��ɫ����

//����״̬
#define GU_WAIT_PEACE				0x01								//�ȴ����
#define GU_WAIT_REGRET				0x02								//�ȴ�����
#define GU_WAIT_CHESS_COUNT			0x04								//�ȴ���Ŀ

//ʧ��ԭ��
#define FR_RULE_LIMIT				1									//��������
#define FR_COUNT_LIMIT				2									//��������
#define FR_PLAYER_OPPOSE			3									//��ҷ���

//�Ծ�ģʽ
#define GM_MATCH					0									//����ģʽ
#define GM_FRIEND					1									//����ģʽ

//////////////////////////////////////////////////////////////////////////

//����ṹ
struct tagGameRuleInfo
{
	BYTE							cbGameMode;							//�Ծ�ģʽ
	BYTE							cbDirections;						//��ʾ��־
	BYTE							cbRegretFlag;						//��������
	WORD							wRuleStepTime;						//��Ϸ��ʱ
	WORD							wRuleLimitTime;						//�޶�ʱ��
	WORD							wRuleTimeOutCount;					//��ʱ����
};

//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_PLACE_CHESS			101									//��������
#define SUB_S_PASS_CHESS			102									//��������
#define SUB_S_REGRET_REQ			103									//��������
#define SUB_S_REGRET_FAILE			104									//����ʧ��
#define SUB_S_REGRET_RESULT			105									//������
#define SUB_S_PEACE_REQ				106									//��������
#define SUB_S_PEACE_ANSWER			107									//����Ӧ��
#define SUB_S_GAME_END				108									//��Ϸ����
#define SUB_S_CHESS_MANUAL			109									//������Ϣ
#define SUB_S_REQ_SET_RULE			110									//��������
#define SUB_S_REQ_AGREE_RULE		111									//����ͬ��
#define SUB_S_REQ_CHESS_COUNT		112									//�����Ŀ
#define SUB_S_UNAGREE_CHESS_COUNT	113									//������Ŀ

//��Ϸ״̬
struct CMD_S_StatusFree
{
	WORD							wBlackUser;							//�������
	BYTE							cbGameRule;							//�����־
	tagGameRuleInfo					GameRuleInfo;						//��Ϸ����
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	WORD							wBlackUser;							//�������
	WORD				 			wCurrentUser;						//��ǰ���
	WORD							wTimeOutCount;						//��ʱ����
	WORD							wUseClock[2];						//ʹ��ʱ��
	WORD							wLeftClock[2];						//��ʱʱ��
	WORD							cbBegStatus[2];						//����״̬
	tagGameRuleInfo					GameRuleInfo;						//��Ϸ����
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	WORD							wBlackUser;							//�������
	tagGameRuleInfo					GameRuleInfo;						//��Ϸ����
};

//��������
struct CMD_S_PlaceChess
{
	BYTE							cbXPos;								//����λ��
	BYTE							cbYPos;								//����λ��
	WORD				 			wPlaceUser;							//�������
	WORD				 			wCurrentUser;						//��ǰ���
	WORD							wUseClock[2];						//ʹ��ʱ��
	WORD							wLeftClock[2];						//��ʱʱ��
};

//��������
struct CMD_S_PassChess
{
	WORD				 			wPassUser;							//�������
	WORD				 			wCurrentUser;						//��ǰ���
	WORD							wUseClock[2];						//ʹ��ʱ��
	WORD							wLeftClock[2];						//��ʱʱ��
};

//����ʧ��
struct CMD_S_RegretFaile
{
	BYTE							cbFaileReason;						//ʧ��ԭ��
};

//������
struct CMD_S_RegretResult
{
	WORD							wRegretUser;						//�������
	WORD							wCurrentUser;						//��ǰ���
	WORD							wRegretCount;						//������Ŀ
	WORD							wUseClock[2];						//ʹ��ʱ��
	WORD							wLeftClock[2];						//��ʱʱ��
};

//��Ϸ����
struct CMD_S_GameEnd
{
	WORD							wWinUser;							//ʤ�����
	LONG							lUserScore[2];						//�û�����
};

//��������
struct CMD_S_Req_SetRult
{
	tagGameRuleInfo					GameRuleInfo;						//��Ϸ����
};

//����ͬ��
struct CMD_S_Req_AgreeRult
{
	tagGameRuleInfo					GameRuleInfo;						//��Ϸ����
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_PLACE_CHESS			1									//��������
#define SUB_C_PASS_CHESS			2									//��������
#define SUB_C_TIME_OUT				3									//���峬ʱ
#define SUB_C_REGRET_REQ			4									//��������
#define SUB_C_REGRET_ANSWER			5									//����Ӧ��
#define SUB_C_PEACE_REQ				6									//��������
#define SUB_C_PEACE_ANSWER			7									//����Ӧ��
#define SUB_C_GIVEUP_REQ			8									//��������
#define SUB_C_REQ_CHESS_COUNT		9									//�����Ŀ
#define SUB_C_UNAGREE_CHESS_COUNT	10									//�ܾ���Ŀ

//��������
struct CMD_C_PlaceChess
{
	BYTE							cbXPos;								//����λ��
	BYTE							cbYPos;								//����λ��
};

//������Ӧ
struct CMD_C_RegretAnswer
{
	BYTE							cbApprove;							//ͬ���־
};

//������Ӧ
struct CMD_C_PeaceAnswer
{
	BYTE							cbApprove;							//ͬ���־
};

//���ƹ���
struct CMD_C_CustomizeRult
{
	tagGameRuleInfo					GameRuleInfo;						//��Ϸ����
};

//////////////////////////////////////////////////////////////////////////

#endif