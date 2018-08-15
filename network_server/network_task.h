
#ifndef __NETWORK_TASK_H_
#define __NETWORK_TASK_H_


#include <mutex>
#include <thread>
#include <string>
#include <queue>
#include <memory.h>
#include <functional>
#include <atomic>
#include <iostream>

#include "mysql.hpp"
#include "data_table.hpp"

#include "mysql_oper_define.h"

class CNetworkTask
{
public:
	CNetworkTask();
	~CNetworkTask();

private:
	std::atomic_bool m_bRunFlag;
	std::mutex m_queue_mutex_request;
	std::queue< std::shared_ptr<struct tagEventRequest> > m_queueRequest;
	std::mutex m_queue_mutex_response;
	std::queue< std::shared_ptr<struct tagEventResponse> > m_queueResponse;
	std::thread m_workThread;
private:
	void AddEventRequest(std::shared_ptr<struct tagEventRequest> sptrRequest);
	void AddEventResponse(std::shared_ptr<struct tagEventResponse> sptrResponse);

private:
	static void runThreadFunction(CNetworkTask *pTask);

public:
	bool Init();
	bool Start();
	bool ShutDown();
	std::shared_ptr<struct tagEventRequest> MallocEventRequest(int eventid, int callback);
	void AsyncExecute(std::shared_ptr<struct tagEventRequest>);
	bool OnProcessEvent(std::shared_ptr<struct tagEventRequest> sptrRequest);
	std::shared_ptr<struct tagEventResponse> GetAsyncExecuteResult();
};






#endif


