
#include "network_task_callback.h"
#include "network_oper_define.h"

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
	return false;
}

bool CNetworkTaskCallBack::OnNetworkNotifyClosed(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	printf("Task OnNetworkNotifyClosed - eventid:%d,contextid:%d,addr:%s\n", sptrRequest->eventid, sptrRequest->contextid, inet_ntoa(sptrRequest->address.sin_addr));

	return false;
}

bool CNetworkTaskCallBack::OnNetworkNotifyAccept(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	printf("Task OnNetworkNotifyAccept - eventid:%d,contextid:%d,addr:%s\n", sptrRequest->eventid, sptrRequest->contextid, inet_ntoa(sptrRequest->address.sin_addr));

	return false;
}

bool CNetworkTaskCallBack::OnNetworkNotifyReaded(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	printf("Task OnNetworkNotifyReaded - eventid:%d,contextid:%d,addr:%s\n", sptrRequest->eventid, sptrRequest->contextid, inet_ntoa(sptrRequest->address.sin_addr));

	struct packet_buffer & data = sptrRequest->data;
	if (data.header.command <= NETWORK_EVENT_MIN || data.header.command >= NETWORK_EVENT_MAX)
	{
		return false;
	}
	if (data.header.command == NETWORK_EVENT_TEST)
	{
		return NetworkedReadedOnTest(data);
	}

	return false;
}


bool CNetworkTaskCallBack::NetworkedReadedOnTest(struct packet_buffer & data)
{
	printf("Task NetworkedReadedOnTest - identity:%d,command:%d,length:%d,buffer:%s\n", data.header.identity, data.header.command, data.header.length, data.buffer);


	return true;
}

