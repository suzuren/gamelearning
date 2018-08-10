
#include "mysql_task.h"
#include <iomanip>


void CMysqlTask::runThreadFunction(CMysqlTask *pTask)
{
	while (pTask->m_bRunFlag == true)
	{
		if (pTask->m_queueRequest.empty() == false)
		{
			std::unique_lock<std::mutex> lock_front(pTask->m_queue_mutex_request);
			std::shared_ptr<struct tagEventRequest> sptrRequest = pTask->m_queueRequest.front();
			lock_front.unlock();
			if (sptrRequest != nullptr)
			{
				if (pTask->OnProcessEvent(sptrRequest) == false)
				{
					bool bDataBaseConnected = pTask->m_dbAsyncOper.connected();
					if (bDataBaseConnected)
					{
						pTask->StartAsyncConnect();
						continue;
					}
					else
					{
						// wtite error log
					}
				}
				else
				{
					// success
				}
			}
			std::unique_lock<std::mutex> lock_pop(pTask->m_queue_mutex_request);
			pTask->m_queueRequest.pop();
			lock_pop.unlock();
		}
		else
		{
			unsigned int msec = 1000;
			struct timespec tm;
			tm.tv_sec = msec / 1000;
			tm.tv_nsec = msec % 1000 * 1000000;
			nanosleep(&tm, 0);
		}
	}
}


CMysqlTask::CMysqlTask()
{

}

CMysqlTask::~CMysqlTask()
{
	m_workThread.join();
}


bool CMysqlTask::Init()
{
	m_bRunFlag = true;
	return true;
}

bool CMysqlTask::StartAsyncConnect()
{
	struct tagDataBaseConfig & config = m_dbConfig;
	m_dbAsyncOper.connect(config.host.data(), config.port, config.user.data(), config.password.data(), config.database.data(), config.timeout);
	if (m_dbAsyncOper.connected() == false)
	{
		return false;
	}
	return true;
}

void CMysqlTask::AddEventRequest(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	if (m_bRunFlag && sptrRequest != nullptr)
	{
		std::unique_lock<std::mutex> lock_front(m_queue_mutex_request);
		m_queueRequest.push(sptrRequest);
		lock_front.unlock();
	}
}

void CMysqlTask::AddEventRequest(std::shared_ptr<struct tagEventResponse> sptrResponse)
{
	if (m_bRunFlag && sptrResponse != nullptr)
	{
		std::unique_lock<std::mutex> lock_front(m_queue_mutex_response);
		m_queueResponse.push(sptrResponse);
		lock_front.unlock();
	}
}

bool CMysqlTask::Start()
{
	m_bRunFlag = true;
	auto startfunc = std::bind(runThreadFunction, this);
	std::thread worker_thread(startfunc);
	m_workThread = std::move(worker_thread);

	return true;
}

bool CMysqlTask::OnProcessEvent(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	if (sptrRequest == nullptr)
	{
		return false;
	}
	if (sptrRequest->callback == DATABASE_CALL_BACK_NULL)
	{
		m_dbAsyncOper.execute(sptrRequest->strsql);
		return true;
	}

	auto sptrResponse = std::make_shared<struct tagEventResponse>();
	sptrResponse->eventid = sptrRequest->eventid;
	sptrResponse->callback = sptrRequest->callback;
	memcpy(sptrResponse->params, sptrRequest->params, sizeof(sptrResponse->params));

	if (sptrRequest->callback == DATABASE_CALL_BACK_QUERY_FIELDS)
	{
		AddEventRequest(sptrResponse);
		return true;
	}
	if (sptrRequest->callback == DATABASE_CALL_BACK_AFFECTED_ROWS)
	{
		AddEventRequest(sptrResponse);
		return true;
	}

	return false;
}
