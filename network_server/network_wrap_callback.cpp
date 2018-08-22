
#include "network_wrap_callback.h"

bool CNetworkWrapCallBack::OnProcessNetworkEvent(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	if (sptrRequest == nullptr)
	{
		return false;
	}
	if (sptrRequest->eventid == NETWORK_NOTIFY_CLOSED)
	{
		return OnNetworkNotifyClosed(sptrRequest);
	}
	if (sptrRequest->eventid == NETWORK_NOTIFY_CONNECT)
	{
		return OnNetworkNotifyConnect(sptrRequest);
	}
	if (sptrRequest->eventid == NETWORK_NOTIFY_READED)
	{
		return OnNetworkNotifyReaded(sptrRequest);
	}
	return false;
}

bool CNetworkWrapCallBack::OnNetworkNotifyClosed(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	printf("Wrap OnNetworkNotifyClosed - eventid:%d,contextid:%d,addr:%s\n", sptrRequest->eventid, sptrRequest->contextid, inet_ntoa(sptrRequest->address.sin_addr));

	return false;
}

bool CNetworkWrapCallBack::OnNetworkNotifyConnect(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	printf("Wrap OnNetworkNotifyConnect - eventid:%d,contextid:%d,addr:%s\n", sptrRequest->eventid, sptrRequest->contextid, inet_ntoa(sptrRequest->address.sin_addr));

	return false;
}

bool CNetworkWrapCallBack::OnNetworkNotifyReaded(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	printf("Wrap OnNetworkNotifyReaded - eventid:%d,contextid:%d,addr:%s\n", sptrRequest->eventid, sptrRequest->contextid, inet_ntoa(sptrRequest->address.sin_addr));

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


bool CNetworkWrapCallBack::NetworkedReadedOnTest(struct packet_buffer & data)
{
	printf("Wrap NetworkedReadedOnTest - identity:%d,command:%d,length:%d,buffer:%s\n", data.header.identity, data.header.command, data.header.length, data.buffer);


	return true;
}

