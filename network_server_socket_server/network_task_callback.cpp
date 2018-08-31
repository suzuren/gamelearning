
#include "network_task_callback.h"
#include "network_oper_define.h"

#include "network_mgr.h"

#include <arpa/inet.h>

bool CNetworkTaskCallBack::OnProcessNetworkEvent(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	if (sptrRequest == nullptr)
	{
		return false;
	}
	if (sptrRequest->eventid == NETWORK_NOTIFY_CLOSED)
	{
		return OnNetworkNotifyClosed(sptrRequest);
	}
	if (sptrRequest->eventid == NETWORK_NOTIFY_ACCENT)
	{
		return OnNetworkNotifyAccept(sptrRequest);
	}
	if (sptrRequest->eventid == NETWORK_NOTIFY_READED)
	{
		return OnNetworkNotifyReaded(sptrRequest);
	}
	if (sptrRequest->eventid == NETWORK_NOTIFY_OPENED)
	{
		return OnNetworkNotifyOpened(sptrRequest);
	}
	if (sptrRequest->eventid == NETWORK_NOTIFY_EXITED)
	{
		return OnNetworkNotifyExited(sptrRequest);
	}
	return false;
}

bool CNetworkTaskCallBack::OnNetworkNotifyClosed(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	printf("Task OnNetworkNotifyClosed - eventid:%d,contextid:%d\n", sptrRequest->eventid, sptrRequest->contextid);

	return false;
}

bool CNetworkTaskCallBack::OnNetworkNotifyAccept(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	printf("Task OnNetworkNotifyAccept - eventid:%d,contextid:%d,handle:%llu,header.handler:%llu\n", sptrRequest->eventid, sptrRequest->contextid, sptrRequest->handle, sptrRequest->data.header.handler);

	return false;
}

bool CNetworkTaskCallBack::OnNetworkNotifyReaded(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	printf("Task OnNetworkNotifyReaded - eventid:%d,contextid:%d,handle:%llu\n", sptrRequest->eventid, sptrRequest->contextid, sptrRequest->handle);

	struct packet_buffer & data = sptrRequest->data;
	if (data.header.command <= NETWORK_EVENT_MIN || data.header.command >= NETWORK_EVENT_MAX)
	{
		return false;
	}
	if (data.header.command == NETWORK_EVENT_TEST)
	{
		return NetworkedReadedOnTest(sptrRequest->contextid, data);
	}

	return false;
}

bool CNetworkTaskCallBack::OnNetworkNotifyOpened(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	printf("Task OnNetworkNotifyOpened - eventid:%d,contextid:%d,handle:%llu,buffer:%s\n", sptrRequest->eventid, sptrRequest->contextid, sptrRequest->handle, sptrRequest->data.buffer);

	return false;
}

bool CNetworkTaskCallBack::OnNetworkNotifyExited(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	printf("Task OnNetworkNotifyExited - eventid:%d,contextid:%d,handle:%llu,buffer:%s\n", sptrRequest->eventid, sptrRequest->contextid, sptrRequest->handle, sptrRequest->data.buffer);

	return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool CNetworkTaskCallBack::NetworkedReadedOnTest(int contextid, struct packet_buffer & data)
{
	printf("Task NetworkedReadedOnTest - handler:%llu,command:%d,length:%d,buffer:%s\n", data.header.handler, data.header.command, data.header.length, data.buffer);


	return true;
}

