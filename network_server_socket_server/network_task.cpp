
#include "network_task.h"

#include <iomanip>


int CNetworkTask::OnDisposeEvents()
{
	return 0;
}

int CNetworkTask::AcceptNotify(int fd)
{
	return 0;
}

int CNetworkTask::HangupNotify(int fd)
{
	return 1;
}

int CNetworkTask::InputNotify(int fd)
{
	return -1;
}

int CNetworkTask::OutputNotify(int fd)
{
	return 1;
}

bool CNetworkTask::SocketListen()
{
	if (m_socketserver != NULL)
	{
		return false;
	}
	m_hlisten = 200;
	m_hstart = 201;
	m_socketserver = socket_server_create();
	if (m_socketserver == NULL)
	{
		return false;
	}
	m_listenid = socket_server_listen(m_socketserver, m_hlisten, m_strIP.c_str(), m_port, 32);
	if (m_listenid == -1)
	{
		return false;
	}
	socket_server_start(m_socketserver, m_hstart, m_listenid);

	return true;
}

void CNetworkTask::runThreadFunction(CNetworkTask *pTask)
{
	bool bListen = pTask->SocketListen();
	printf("Task listen - ip:%s,port:%d,bListen:%d\n", pTask->GetIP().data(), pTask->GetPort(), bListen);
	if (bListen == true)
	{
		while (pTask != nullptr && pTask->m_bRunFlag == true)
		{
			pTask->OnDisposeEvents();
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
	m_sptrWorkThread = nullptr;
	m_bRunFlag = true;

	m_socketserver = NULL;

	m_listenid = -1;
	m_hlisten = 0;
	m_hstart = 0;

	return true;
}

bool CNetworkTask::Start(std::string ip,int port)
{
	m_port = port;
	m_strIP = ip;
	if (m_sptrWorkThread != nullptr)
	{
		return false;
	}
	auto startfunc = std::bind(runThreadFunction, this);
	m_sptrWorkThread = std::make_shared<std::thread>(startfunc);
	if (m_sptrWorkThread == nullptr)
	{
		return false;
	}
	return true;
}

bool CNetworkTask::ShutDown()
{
	m_bRunFlag = false;
	if (m_sptrWorkThread != nullptr)
	{
		m_sptrWorkThread->join();
		m_sptrWorkThread = nullptr;
	}
	return true;
}

void CNetworkTask::AddEventRequest(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	if (m_bRunFlag && sptrRequest != nullptr)
	{
		m_queue_mutex_request.lock();
		m_queueRequest.push(sptrRequest);
		m_queue_mutex_request.unlock();
	}
}

std::shared_ptr<struct tagEventRequest> CNetworkTask::GetEventRequest()
{
	if (m_queueRequest.empty() == false)
	{
		m_queue_mutex_request.lock();
		auto sptrRequest = m_queueRequest.front();
		m_queueRequest.pop();
		m_queue_mutex_request.unlock();
		return sptrRequest;
	}
	return nullptr;
}

std::shared_ptr<struct tagEventRequest> CNetworkTask::GetAsyncRequest()
{
	return GetEventRequest();
}

bool CNetworkTask::SendData(std::shared_ptr<struct tagTaskSendData> sptrData)
{
	if (sptrData != nullptr)
	{
	}
	return true;
}