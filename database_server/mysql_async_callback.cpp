
#include "mysql_async_callback.h"

bool CMysqlAsyncCallBack::OnProcessDataBaseEvent(std::shared_ptr<struct tagEventResponse> sptrResponse)
{
	if (sptrResponse == nullptr)
	{
		return false;
	}
	if (sptrResponse->eventid == MYSQL_DATABASE_EVENT_TEST)
	{
		return EventCallBackOnTest(sptrResponse);
	}
	//else if ()
	//{
	//}
	return false;
}

bool CMysqlAsyncCallBack::EventCallBackOnTest(std::shared_ptr<struct tagEventResponse> sptrResponse)
{
	std::cout << "EventCallBackOnTest - " << " callback:" << sptrResponse->callback << " eventid:" << sptrResponse->eventid << " params: " << sptrResponse->params[0] << " affected_rows:" << sptrResponse->affected_rows << std::endl;
	auto sptr_result = sptrResponse->sptrResult;
	if (sptr_result != nullptr)
	{
		for (auto& row : *sptr_result)
		{
			std::cout << "----------------------------------------------------------" << std::endl;
			std::cout << "0 - " << std::get<int>(row[0]) << std::endl;
			std::cout << "1 - " << std::get<std::string>(row[1]) << std::endl;
			std::cout << "2 - " << std::get<int64_t>(row[2]) << std::endl;
			std::cout << "3 - " << std::get<int>(row[3]) << std::endl;
			std::cout << "4 - " << std::get<int>(row[4]) << std::endl;
			std::cout << "5 - " << std::get<int>(row[5]) << std::endl;
			std::cout << "6 - " << std::get<int64_t>(row[6]) << std::endl;
			std::cout << "7 - " << std::get<int64_t>(row[7]) << std::endl;
			std::cout << "8 - " << std::get<std::string>(row[8]) << std::endl;
			std::cout << "9 - " << std::get<int>(row[9]) << std::endl;
		}
	}
	else
	{
		return false;
	}
	return true;
}

