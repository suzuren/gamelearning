
#include "network_async_callback.h"

bool CNetworkAsyncCallBack::OnProcessNetworkEvent(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	if (sptrRequest == nullptr)
	{
		return false;
	}
	if (sptrRequest->eventid == NETWORK_NOTIFY_ACCENT)
	{
		return EventCallBackOnTest(sptrRequest);
	}
	//else if ()
	//{
	//}
	return false;
}

bool CNetworkAsyncCallBack::EventCallBackOnTest(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	return true;
}

