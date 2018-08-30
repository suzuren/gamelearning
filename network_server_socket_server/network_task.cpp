
#include "network_task.h"

#include <iomanip>

// ----------------------------------------------------------------------------------------------------------

CNetworkTask::CNetworkTask()
{
}

CNetworkTask::~CNetworkTask()
{
}

bool CNetworkTask::Init()
{
	m_sptrWorkThread = nullptr;
	m_socketserver = NULL;
	m_listenid = -1;
	m_hlisten = 200;
	m_hstart = 201;
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
	if (m_socketserver != NULL)
	{
		socket_server_exit(m_socketserver);
	}

	//if (m_socketserver != NULL)
	//{
	//	free(m_socketserver);
	//}
	//if (m_sptrWorkThread != nullptr)
	//{
	//	m_sptrWorkThread->join();
	//	m_sptrWorkThread = nullptr;
	//}

	return true;
}

bool CNetworkTask::SendData(std::shared_ptr<struct tagTaskSendData> sptrData)
{
	if (sptrData != nullptr)
	{
	}
	return true;
}

std::shared_ptr<struct tagEventRequest> CNetworkTask::GetAsyncRequest()
{
	return GetEventRequest();
}

// ----------------------------------------------------------------------------------------------------------

bool CNetworkTask::SocketListen()
{
	if (m_socketserver != NULL)
	{
		return false;
	}
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

void * CNetworkTask::runThreadFunction(CNetworkTask *pTask)
{
	bool bListen = pTask->SocketListen();
	printf("Task listen - ip:%s,port:%d,bListen:%d\n", pTask->GetIP().data(), pTask->GetPort(), bListen);
	if (bListen == true && pTask != nullptr)
	{
		struct socket_server *ss = pTask->m_socketserver;
		struct socket_message result;
		for (;;)
		{
			int type = socket_server_poll(ss, &result, NULL);
			// DO NOT use any ctrl command (socket_server_close , etc. ) in this thread.
			switch (type) {
			case SOCKET_EXIT:
				pTask->NotifyExit(result);
				return NULL;
			case SOCKET_DATA:
				pTask->NotifyData(result);
				break;
			case SOCKET_CLOSE:
				pTask->NotifyClose(result);
				break;
			case SOCKET_OPEN:
				pTask->NotifyOpen(result);
				break;
			case SOCKET_ERROR:
				pTask->NotifyError(result);
				break;
			case SOCKET_ACCEPT:
				pTask->NotifyAccept(result);
				break;
			}
		}

	}
	return NULL;
}

int CNetworkTask::NotifyExit(struct socket_message & result)
{
	printf("Task exit(%lu) [id=%d] size=%d\n", result.opaque, result.id, result.ud);
	return 0;
}
int CNetworkTask::NotifyData(struct socket_message & result)
{
	printf("Task message(%lu) [id=%d] size=%d\n", result.opaque, result.id, result.ud);
	free(result.data);
	return 0;
}
int CNetworkTask::NotifyClose(struct socket_message & result)
{
	printf("Task close(%lu) [id=%d]\n", result.opaque, result.id);
	return 0;
}
int CNetworkTask::NotifyOpen(struct socket_message & result)
{
	printf("Task open(%lu) [id=%d] %s\n", result.opaque, result.id, result.data);
	return 0;
}
int CNetworkTask::NotifyError(struct socket_message & result)
{
	printf("Task error(%lu) [id=%d]\n", result.opaque, result.id);
	return 0;
}
int CNetworkTask::NotifyAccept(struct socket_message & result)
{
	printf("Task accept(%lu) [id=%d %s] from [%d]\n", result.opaque, result.ud, result.data, result.id);
	struct socket_server *ss = m_socketserver;
	socket_server_start(ss, result.opaque, result.ud);
	return 0;
}
// ----------------------------------------------------------------------------------------------------------

void CNetworkTask::AddEventRequest(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
}

std::shared_ptr<struct tagEventRequest> CNetworkTask::GetEventRequest()
{
	return nullptr;
}

// ----------------------------------------------------------------------------------------------------------


