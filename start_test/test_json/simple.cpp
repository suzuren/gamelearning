#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"


#include <iostream>
#include <string>
#include <sstream>
#include <memory.h>

typedef unsigned char		BYTE;
#define MAX_HAND_CARD_AMOUNT 14
#define	INVALID_VALUE		(0xFF)				//无效数值

#define MAX_CHI_COUNT			3				//吃牌最大数
#define MAX_PENG_COUNT			3				//碰牌最大数
#define MAX_GANG_COUNT			4				//杠牌最大数
#define MAX_ACTION_COUNT		5				//吃碰杠数
#define MAX_EDGE_COUNT			4				//码成四边
#define MAX_LAYER_COUNT			2				//每墩两层

typedef struct tagGameBlingLog_t
{
	rapidjson::Document		docOperate;				// 用户操作
	tagGameBlingLog_t()
	{
		Init();
	}
	void Init()
	{
		docOperate.SetObject(); // 把 rapidjson::Document 设置为一个空对象 即为 {}
	}
}Game_BlingLog;

Game_BlingLog						m_tagBlingLog;				// 牌局日志
int m_iGameStepIndex = 0;
int   m_cbPlayerCount = 4;
 
struct TCPGStruct
{
	BYTE  cbType;					//吃碰杠类型
	BYTE  cbMeChairID;				//吃碰杠玩家位置
	BYTE  cbBeChairID;				//被吃碰杠玩家位置
	BYTE  cbOutCard;				//别人出的牌
	BYTE  cbData[4];			//吃碰杠牌数据
	TCPGStruct()
	{
		Init();
	}
	void Init()
	{
		cbType = 1;
		cbMeChairID = INVALID_VALUE;
		cbBeChairID = INVALID_VALUE;
		cbOutCard = INVALID_VALUE;
		memset(cbData, INVALID_VALUE, sizeof(cbData));
	}
};


void WriteRecord_GetUserHandCard(BYTE cbChairID, rapidjson::Value & valueHandCard)
{
	//生成一个分配器
	rapidjson::Document::AllocatorType& docOperate_allocator = m_tagBlingLog.docOperate.GetAllocator();
	BYTE cbHandCard[MAX_HAND_CARD_AMOUNT] = {0};
	int iHandCardCount = MAX_HAND_CARD_AMOUNT;// m_GameData.CopyOneHandCard(cbHandCard, cbChairID);
	for (int i = 0; i < iHandCardCount; i++)
	{
		BYTE cbCardData = cbHandCard[i];
		valueHandCard.PushBack(cbCardData, docOperate_allocator);
	}
}


void WriteRecord_GetAllUserHandCard(rapidjson::Value & valueAllHandCard)
{
	//生成一个分配器
	rapidjson::Document::AllocatorType& docOperate_allocator = m_tagBlingLog.docOperate.GetAllocator();
	for (BYTE i = 0; i < m_cbPlayerCount; i++)
	{

		rapidjson::Value valueHandCard(rapidjson::Type::kArrayType);
		WriteRecord_GetUserHandCard(i, valueHandCard);

		std::stringstream ss_chairid;
		ss_chairid.clear();
		ss_chairid.str("");
		ss_chairid << "chairid_" << i;
		valueAllHandCard.AddMember(rapidjson::Value::StringRefType(ss_chairid.str().data()), valueHandCard, docOperate_allocator);
	}
}



void WriteRecord_GetUserCPGData(BYTE cbChairID, rapidjson::Value& valueUserCPGData)
{
	rapidjson::Document::AllocatorType& docOperate_allocator = m_tagBlingLog.docOperate.GetAllocator();

	TCPGStruct tagCPGData[MAX_ACTION_COUNT];
	int iCPGCount = MAX_ACTION_COUNT;// = m_GameData.CopyCPGData(cbChairID, tagCPGData);

	for (int j = 0; j < iCPGCount; j++)
	{
		rapidjson::Value valueUserCPGDataIndex(rapidjson::Type::kObjectType);

		rapidjson::Value valueCPGType(rapidjson::Type::kNumberType);
		valueCPGType.SetInt(tagCPGData[j].cbType);
		valueUserCPGDataIndex.AddMember("type", valueCPGType, docOperate_allocator);

		rapidjson::Value valueCPGMeChairID(rapidjson::Type::kNumberType);
		valueCPGMeChairID.SetInt(tagCPGData[j].cbMeChairID);
		valueUserCPGDataIndex.AddMember("me_chairid", valueCPGMeChairID, docOperate_allocator);

		rapidjson::Value valueCPGOutCard(rapidjson::Type::kNumberType);
		valueCPGOutCard.SetInt(tagCPGData[j].cbOutCard);
		valueUserCPGDataIndex.AddMember("out_card", valueCPGOutCard, docOperate_allocator);

		rapidjson::Value valueCPGData(rapidjson::Type::kArrayType);
		for (int k = 0; k < MAX_GANG_COUNT; k++)
		{
			BYTE cbCardData = tagCPGData[j].cbData[k];
			//if (cbCardData != INVALID_VALUE)
			{
				valueCPGData.PushBack(cbCardData, docOperate_allocator);
			}
		}
		valueUserCPGDataIndex.AddMember("data", valueCPGData, docOperate_allocator);

		std::stringstream ss_cpg_index;
		ss_cpg_index.clear();
		ss_cpg_index.str("");
		ss_cpg_index << "cpg_index_" << j;
		valueUserCPGData.AddMember(rapidjson::Value::StringRefType(ss_cpg_index.str().data()), valueUserCPGDataIndex, docOperate_allocator);
	}
}

void WriteRecord_GetAllUserCPGData(rapidjson::Value &valueAllCPGData)
{
	rapidjson::Document::AllocatorType& docOperate_allocator = m_tagBlingLog.docOperate.GetAllocator();

	rapidjson::Value valueStep(rapidjson::Type::kObjectType);

	rapidjson::Value valueName(rapidjson::Type::kStringType);
	std::string strName = "cpg_data";
	valueName.SetString(strName.data(), strName.size(), docOperate_allocator);
	valueStep.AddMember("step_name", valueName, docOperate_allocator);

	for (int i = 0; i < m_cbPlayerCount; ++i)
	{
		rapidjson::Value valueUserCPGData(rapidjson::Type::kObjectType);
		WriteRecord_GetUserCPGData(i, valueUserCPGData);

		std::stringstream ss_chairid;
		ss_chairid.clear();
		ss_chairid.str("");
		ss_chairid << "chairid_" << i;
		valueAllCPGData.AddMember(rapidjson::Value::StringRefType(ss_chairid.str().data()), valueUserCPGData, docOperate_allocator);
	}
}

void WriteRecord_GameStart()
{
	rapidjson::Document::AllocatorType& docOperate_allocator = m_tagBlingLog.docOperate.GetAllocator();

	rapidjson::Value valueAllHandCard(rapidjson::Type::kObjectType);
	//WriteRecord_GetAllUserHandCard(valueAllHandCard);

	for (BYTE i = 0; i < m_cbPlayerCount; i++)
	{

		rapidjson::Value valueHandCard(rapidjson::Type::kArrayType);
		WriteRecord_GetUserHandCard(i, valueHandCard);

		//std::stringstream ss_chairid;
		//ss_chairid.clear();
		//ss_chairid.str("");
		//ss_chairid << "chairid_" << i;
		char arrCh[32] = { 0 };
		sprintf(arrCh,"chairid_%d",i);
		//valueAllHandCard.AddMember(rapidjson::Value::StringRefType(ss_chairid.str().data()), valueHandCard, docOperate_allocator);
		valueAllHandCard.AddMember(arrCh, valueHandCard, docOperate_allocator);
	}

	rapidjson::Value valueAllCPGData(rapidjson::Type::kObjectType);
	WriteRecord_GetAllUserCPGData(valueAllCPGData);

	rapidjson::Value valueGameStepIndex(rapidjson::Type::kObjectType);

	valueGameStepIndex.AddMember("hand_card", valueAllHandCard, docOperate_allocator);
	//valueGameStepIndex.AddMember("cpg_card", valueAllCPGData, docOperate_allocator);

	std::stringstream ss_game_step;
	ss_game_step.clear();
	ss_game_step.str("");
	ss_game_step << "game_step_" << m_iGameStepIndex++;
	m_tagBlingLog.docOperate.AddMember(rapidjson::Value::StringRefType(ss_game_step.str().data()), valueGameStepIndex, docOperate_allocator);

}


int main(int argc, char const *argv[])
{
	WriteRecord_GameStart();

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	m_tagBlingLog.docOperate.Accept(writer);
	// Output {"project":"rapidjson","stars":11}
	std::cout << buffer.GetString() << std::endl;

	return 0;
}



