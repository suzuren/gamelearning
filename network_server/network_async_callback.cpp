
#include "network_async_callback.h"

bool CNetworkAsyncCallBack::OnProcessNetworkEvent(std::shared_ptr<struct tagEventResponse> sptrResponse)
{
	if (sptrResponse == nullptr)
	{
		return false;
	}
	if (sptrResponse->eventid == NETWORK_EVENT_TEST)
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

