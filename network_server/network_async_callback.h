
#ifndef __MYSQL_ASYNC_CALLBACK_H_
#define __MYSQL_ASYNC_CALLBACK_H_

#include "mysql_mgr.h"

class CMysqlAsyncCallBack : public AsyncNetworkCallBack
{
public:
	virtual bool OnProcessDataBaseEvent(std::shared_ptr<struct tagEventResponse> sptrResponse);

private:
	bool EventCallBackOnTest(std::shared_ptr<struct tagEventResponse> sptrResponse);
};


#endif


