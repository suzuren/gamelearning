
#ifndef __MYSQL_ASYNC_CALLBACK_H_
#define __MYSQL_ASYNC_CALLBACK_H_

#include "network_mgr.h"

class CNetworkAsyncCallBack : public AsyncNetCallBack
{
public:
	virtual bool OnProcessNetworkEvent(std::shared_ptr<struct tagEventRequest> sptrRequest);

private:
	bool EventCallBackOnTest(std::shared_ptr<struct tagEventRequest> sptrRequest);
};


#endif


