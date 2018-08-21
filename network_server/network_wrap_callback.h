
#ifndef __NETWORK_WRAP_CALLBACK_H_
#define __NETWORK_WRAP_CALLBACK_H_

#include "network_mgr.h"

class CNetworkWrapCallBack : public AsyncNetCallBack
{
public:
	virtual bool OnProcessNetworkEvent(std::shared_ptr<struct tagEventRequest> sptrRequest);

private:
	bool OnNetworkNotifyClosed(std::shared_ptr<struct tagEventRequest> sptrRequest);

	bool OnNetworkNotifyAccept(std::shared_ptr<struct tagEventRequest> sptrRequest);

	bool OnNetworkNotifyReaded(std::shared_ptr<struct tagEventRequest> sptrRequest);
private:

	bool NetworkedReadedOnTest(struct packet_buffer & data);
};


#endif


