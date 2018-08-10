
#ifndef __MYSQL_ASYNC_CALLBACK_H_
#define __MYSQL_ASYNC_CALLBACK_H_

#include "mysql_mgr.h"

class CMysqlAsyncCallBack : public AsyncDBCallBack
{
public:
	virtual bool OnProcessDBEvent(std::shared_ptr<struct tagEventResponse> sptrResponse)
	{
		return false;
	}

};


#endif


