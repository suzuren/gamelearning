
#include "mysql_task.h"
#include <iomanip>



void CNetworkTask::runThreadFunction(CNetworkTask *pTask)
{
	while (pTask != nullptr &&pTask->m_bRunFlag == true)
	{
		if (pTask->m_queueRequest.empty() == false)
		{

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


CNetworkTask::CNetworkTask()
{

}

CNetworkTask::~CNetworkTask()
{

}

bool CNetworkTask::Init()
{
	m_bRunFlag = true;
	return true;
}



void CNetworkTask::AddEventRequest(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	if (m_bRunFlag && sptrRequest != nullptr)
	{
		std::unique_lock<std::mutex> lock_front(m_queue_mutex_request);
		m_queueRequest.push(sptrRequest);
		lock_front.unlock();
	}
}

void CNetworkTask::AddEventResponse(std::shared_ptr<struct tagEventResponse> sptrResponse)
{
	if (m_bRunFlag && sptrResponse != nullptr)
	{
		std::unique_lock<std::mutex> lock_front(m_queue_mutex_response);
		m_queueResponse.push(sptrResponse);
		lock_front.unlock();
	}
}

bool CNetworkTask::Start()
{
	m_bRunFlag = true;
	auto startfunc = std::bind(runThreadFunction, this);
	std::thread worker_thread(startfunc);
	m_workThread = std::move(worker_thread);

	return true;
}

bool CNetworkTask::ShutDown()
{
	m_bRunFlag = false;
	std::cout << "CNetworkTask::ShutDown - pid:" << m_workThread.get_id() << std::endl;
	m_workThread.join();
	return true;
}

std::shared_ptr<struct tagEventRequest> CNetworkTask::MallocEventRequest(int eventid, int callback)
{
	auto sptrRequest = std::make_shared<struct tagEventRequest>();
	sptrRequest->eventid = eventid;
	sptrRequest->callback = callback;
	return sptrRequest;
}

void CNetworkTask::AsyncExecute(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	AddEventRequest(sptrRequest);
}




bool CNetworkTask::OnProcessEvent(std::shared_ptr<struct tagEventRequest> sptrRequest)
{

	return false;
}

std::shared_ptr<struct tagEventResponse> CNetworkTask::GetAsyncExecuteResult()
{
	return nullptr;
}
