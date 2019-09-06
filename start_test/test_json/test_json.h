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
	//生成一个分配器
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

}

