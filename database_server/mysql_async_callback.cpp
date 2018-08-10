
#include "mysql_async_callback.h"

bool CMysqlAsyncCallBack::OnProcessDBEvent(std::shared_ptr<struct tagEventResponse> sptrResponse)
{
	if (sptrResponse == nullptr)
	{
		return false;
	}
	if (sptrResponse->eventid == MYSQL_DATABASE_EVENT_TEST)
	{
		return EventCallBackOnTest(sptrResponse);
	}
	else if (1)
	{

	}
	return false;
}

bool CMysqlAsyncCallBack::EventCallBackOnTest(std::shared_ptr<struct tagEventResponse> sptrResponse)
{
	std::cout << sptrResponse->callback << sptrResponse->eventid << sptrResponse->params[0] << std::endl;
	return true;
}

