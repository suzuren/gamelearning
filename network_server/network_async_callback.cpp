
#include "network_async_callback.h"

bool CNetworkAsyncCallBack::OnProcessDataBaseEvent(std::shared_ptr<struct tagEventResponse> sptrResponse)
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

bool CNetworkAsyncCallBack::EventCallBackOnTest(std::shared_ptr<struct tagEventResponse> sptrResponse)
{
	return true;
}

