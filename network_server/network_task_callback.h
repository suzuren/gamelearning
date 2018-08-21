
#ifndef __NETWORK_TASK_CALLBACK_H_
#define __NETWORK_TASK_CALLBACK_H_

#include "network_mgr.h"

class CNetworkTaskCallBack : public AsyncNetCallBack
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


