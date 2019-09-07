#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"


#include <iostream>
#include <string>
#include <sstream>
#include <memory.h>

rapidjson::Document		docUserInfo;

void GetUserInfo(int uid, rapidjson::Value& valueUserInfo)
{
	rapidjson::Document::AllocatorType& docUserInfo_allocator = docUserInfo.GetAllocator();

	valueUserInfo.PushBack(uid, docUserInfo_allocator);

	for (int i = 0; i < 3; i++)
	{
		valueUserInfo.PushBack(i, docUserInfo_allocator);
	}
}


void GetAllUserInfo(rapidjson::Value& valueAllUserInfo)
{
	//����һ��������
	rapidjson::Document::AllocatorType& docUserInfo_allocator = docUserInfo.GetAllocator();

	for (int i = 30; i < 35; i++)
	{
		rapidjson::Value valueUserInfo(rapidjson::Type::kArrayType);
		GetUserInfo(i, valueUserInfo);

		std::stringstream ss_uid;
		ss_uid.clear();
		ss_uid.str("");
		ss_uid << "uid_" << i;

		rapidjson::Value valueUID(rapidjson::Type::kStringType);
		valueUID.SetString(ss_uid.str().data(), ss_uid.str().size(), docUserInfo_allocator);

		valueAllUserInfo.AddMember(valueUID, valueUserInfo, docUserInfo_allocator);
	}
}



int test_json()
{
	docUserInfo.SetObject();

	rapidjson::Value valueAllUserInfo(rapidjson::Type::kObjectType);
	GetAllUserInfo(valueAllUserInfo);

	rapidjson::Document::AllocatorType& docUserInfo_allocator = docUserInfo.GetAllocator();

	rapidjson::Value valueStepIndex(rapidjson::Type::kObjectType);

	valueStepIndex.AddMember("user_info", valueAllUserInfo, docUserInfo_allocator);

	int iStepIndex = 0;
	std::stringstream ss_step;
	ss_step.clear();
	ss_step.str("");
	ss_step << "step_" << iStepIndex++;

	rapidjson::Value valueStep(rapidjson::Type::kStringType);
	valueStep.SetString(ss_step.str().data(), ss_step.str().size(), docUserInfo_allocator);

	docUserInfo.AddMember(valueStep, valueStepIndex, docUserInfo_allocator);


	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	docUserInfo.Accept(writer);
	// Output {"project":"rapidjson","stars":11}
	std::cout << buffer.GetString() << std::endl;

	return 0;
}

void test_value_set_int64()
{
	long long lValue = 100000;
	rapidjson::Value value_int64(rapidjson::Type::kNumberType);
	value_int64.SetInt64(lValue);

	rapidjson::StringBuffer buffer_int64;
	rapidjson::Writer<rapidjson::StringBuffer> writer_int64(buffer_int64);
	value_int64.Accept(writer_int64);
	std::cout << buffer_int64.GetString() << std::endl;
}


typedef unsigned char		BYTE;
typedef long long			int64;
typedef unsigned long long	uint64;

#define MAX_HU_CARD_TYPE_AMOUNT	66	//��������



#define	INVALID_VALUE		(0xFF)				//��Ч��ֵ
#define ZEOR_VALUE			(0)					//0ֵ

#define GAME_PLAY_COUNT			4				//��Ϸ����

//�����Ƶ�����
// �齫���� ��(9x4) + ��(9x4) + ��(9x4) + ��(7x4) + ��(8x4) + ����(4x4) = 184
#define MAX_MEN_CARD_COUNT		184				// ����齫������ Ŀǰ BYTE ���ͱ�ʾ�����Ϊ255

// �齫������ ��(9) + ��(9) + ��(9) + ��(7) + ��(8) + ����(4) = 46
#define MAX_SINGLE_CARD_COUNT	46


#define MAX_HAND_CARD_AMOUNT	14				//�����������
#define MAX_OUT_CARD_AMOUNT		160				//��������
#define MAX_FLOWER_CARD_AMOUNT	8				//��������
#define MAX_MEN_CARD_AMOUNT		68				//��ǽ����(��������ǰ���17����)
#define MAX_OPERATE_CARD_COUNT	4				//������������
#define MAX_CHI_COUNT			3				//���������
#define MAX_PENG_COUNT			3				//���������
#define MAX_GANG_COUNT			4				//���������
#define MAX_ACTION_COUNT		5				//��������
#define MAX_EDGE_COUNT			4				//����ı�
#define MAX_LAYER_COUNT			2				//ÿ������

// ������ʾ
typedef struct tagHuCardTips_t
{
	BYTE	cbHuCard;
	BYTE	cbArrHuType[MAX_HU_CARD_TYPE_AMOUNT];										//��������
	int64	lArrHuScore[MAX_HU_CARD_TYPE_AMOUNT];										//���Ʒ���
	tagHuCardTips_t()
	{
		Init();
	}
	void Init()
	{
		cbHuCard = INVALID_VALUE;
		memset(cbArrHuType, INVALID_VALUE, sizeof(cbArrHuType));
		memset(lArrHuScore, 0, sizeof(lArrHuScore));
	}
	void operator=(const struct tagHuCardTips_t& info)
	{
		cbHuCard = info.cbHuCard;
		for (int i = 0; i < MAX_HU_CARD_TYPE_AMOUNT; i++)
		{
			cbArrHuType[i] = info.cbArrHuType[i];
			lArrHuScore[i] = info.lArrHuScore[i];
		}
	}
}tagHuCardTips;


// ������ʾ
typedef struct tagTingCardTips_t
{
	BYTE cbOutCard;
	tagHuCardTips HuCardTips[MAX_SINGLE_CARD_COUNT];
	tagTingCardTips_t()
	{
		Init();
	}
	void Init()
	{
		cbOutCard = INVALID_VALUE;
		for (int i = 0; i < MAX_HAND_CARD_AMOUNT; i++)
		{
			HuCardTips[i].Init();
		}
	}
	void operator=(const struct tagTingCardTips_t& info)
	{
		cbOutCard = info.cbOutCard;
		for (int i = 0; i < MAX_SINGLE_CARD_COUNT; i++)
		{
			HuCardTips[i] = info.HuCardTips[i];
		}
	}
}tagTingCardTips;


void test_TingCardTips()
{
	rapidjson::Document		docOperate;
	docOperate.SetObject();

	rapidjson::Document::AllocatorType& docOperate_allocator = docOperate.GetAllocator();


	tagTingCardTips TingCardTips[MAX_HAND_CARD_AMOUNT];								//������ʾ

	// ������ʾ
	rapidjson::Value valueCPGNotify_ting_card_tips(rapidjson::Type::kObjectType);
	for (int i = 0; i < MAX_HAND_CARD_AMOUNT; i++)
	{
		tagTingCardTips& tempTingCardTips = TingCardTips[i];
		//if (tempTingCardTips.cbOutCard == INVALID_VALUE)
		//{
		//	break;
		//}
		rapidjson::Value valueCPGNotify_ting_card_tips_index(rapidjson::Type::kObjectType);

		rapidjson::Value valueCPGNotify_ting_card_tips_index_out_card(rapidjson::Type::kNumberType);
		valueCPGNotify_ting_card_tips_index_out_card.SetInt(tempTingCardTips.cbOutCard);
		valueCPGNotify_ting_card_tips_index.AddMember("out_card", valueCPGNotify_ting_card_tips_index_out_card, docOperate_allocator);

		rapidjson::Value valueCPGNotify_ting_card_tips_index_hu_card_tips(rapidjson::Type::kObjectType);

		for (int j = 0; j < MAX_SINGLE_CARD_COUNT; j++)
		{
			tagHuCardTips& tempHuCardTips = tempTingCardTips.HuCardTips[j];
			//if (tempHuCardTips.cbHuCard == INVALID_VALUE)
			//{
			//	break;
			//}
			//else
			{
				rapidjson::Value valueCPGNotify_ting_card_tips_index_hu_card_tips_hu_card(rapidjson::Type::kNumberType);
				valueCPGNotify_ting_card_tips_index_hu_card_tips_hu_card.SetInt(tempHuCardTips.cbHuCard);
				valueCPGNotify_ting_card_tips_index_hu_card_tips.AddMember("hu_card", valueCPGNotify_ting_card_tips_index_hu_card_tips_hu_card, docOperate_allocator);

				rapidjson::Value valueCPGNotify_ting_card_tips_index_hu_card_tips_hu_type(rapidjson::Type::kArrayType);
				rapidjson::Value valueCPGNotify_ting_card_tips_index_hu_card_tips_hu_score(rapidjson::Type::kArrayType);
				for (int k = 0; k < MAX_HU_CARD_TYPE_AMOUNT; k++)
				{
					BYTE cbHuType = tempHuCardTips.cbArrHuType[k];
					int64_t lHuScore = tempHuCardTips.lArrHuScore[k];
					//if (cbHuType == INVALID_VALUE)
					//{
					//	break;
					//}
					//else
					{
						rapidjson::Value value_hu_score_int64(rapidjson::Type::kNumberType);
						value_hu_score_int64.SetInt64(lHuScore);
						valueCPGNotify_ting_card_tips_index_hu_card_tips_hu_type.PushBack(cbHuType, docOperate_allocator);
						valueCPGNotify_ting_card_tips_index_hu_card_tips_hu_score.PushBack(value_hu_score_int64, docOperate_allocator);
					}
				}
				valueCPGNotify_ting_card_tips_index_hu_card_tips.AddMember("hu_type", valueCPGNotify_ting_card_tips_index_hu_card_tips_hu_type, docOperate_allocator);
				valueCPGNotify_ting_card_tips_index_hu_card_tips.AddMember("hu_score", valueCPGNotify_ting_card_tips_index_hu_card_tips_hu_score, docOperate_allocator);
			}
			valueCPGNotify_ting_card_tips_index.AddMember("hu_card_tips", valueCPGNotify_ting_card_tips_index_hu_card_tips, docOperate_allocator);
		}

		std::stringstream ss_ting_card_tips_index;
		ss_ting_card_tips_index << "ting_card_tips_index_" << i;
		rapidjson::Value value_ting_card_tips_index(rapidjson::Type::kStringType);
		value_ting_card_tips_index.SetString(ss_ting_card_tips_index.str().data(), ss_ting_card_tips_index.str().size(), docOperate_allocator);
		valueCPGNotify_gang_data.AddMember(value_ting_card_tips_index, valueCPGNotify_ting_card_tips_index, docOperate_allocator);
	}

	docOperate.AddMember("value_game_step", valueCPGNotify_ting_card_tips, docOperate_allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	docOperate.Accept(writer);
	std::cout << buffer.GetString() << std::endl;
}


