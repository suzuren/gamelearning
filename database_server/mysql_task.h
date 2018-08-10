
#ifndef __MYSQL_TASK_H_
#define __MYSQL_TASK_H_


#include <mutex>
#include <thread>
#include <string>
#include <queue>
#include <memory.h>
#include <functional>

#include "mysql.hpp"
#include "data_table.hpp"

#include "mysql_oper_define.h"

class CMysqlTask
{
public:
	CMysqlTask();
	~CMysqlTask();

private:
	bool m_bRunFlag;
	std::mutex m_queue_mutex_request;
	std::queue< std::shared_ptr<struct tagEventRequest> > m_queueRequest;
	std::mutex m_queue_mutex_response;
	std::queue< std::shared_ptr<struct tagEventResponse> > m_queueResponse;
	std::thread m_workThread;

private:
	struct tagDataBaseConfig m_dbConfig;
	db::mysql m_dbAsyncOper;
private:
	void AddEventRequest(std::shared_ptr<struct tagEventRequest> sptrRequest);
	void AddEventRequest(std::shared_ptr<struct tagEventResponse> sptrResponse);

private:
	static void runThreadFunction(CMysqlTask *pTask);

public:
	bool Init();
	bool Start();
	void SetDatabaseConfigure(struct tagDataBaseConfig & dbConfig);
	bool StartAsyncConnect();
	std::shared_ptr<struct tagEventRequest> MallocEventRequest(int eventid, int callback);
	void AsyncExecute(std::shared_ptr<struct tagEventRequest>);
	bool OnProcessEvent(std::shared_ptr<struct tagEventRequest> sptrRequest);
	std::shared_ptr<struct tagEventResponse> GetAsyncExecuteResult();
};






#endif


