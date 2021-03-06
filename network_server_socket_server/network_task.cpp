
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
	m_port = 0;
	m_strIP.clear();
	m_listenid = -1;
	m_connectid = -1;
	m_hlisten = 200;
	m_hstart = 201;
	m_hconnect = 202;

	m_rlength = 0;
	memset(m_rbuffer, 0, sizeof(m_rbuffer));
	return true;
}

bool CNetworkTask::Start(std::string ip,int port)
{
	m_port = port;
	m_strIP = ip;
	if (m_socketserver != NULL)
	{
		return false;
	}
	m_socketserver = socket_server_create();
	if (m_socketserver == NULL)
	{
		return false;
	}
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

bool CNetworkTask::Connect(std::string ip, int port)
{
	if (m_connectid != -1)
	{
		return false;
	}
	if (m_socketserver == NULL)
	{
		return false;
	}
	if (m_sptrWorkThread == nullptr)
	{
		return false;
	}
	if (ip.empty() || port == 0)
	{
		return false;
	}
	//const char * ptrIP = "127.0.0.1";
	m_connectid = socket_server_connect(m_socketserver, m_hconnect, ip.data(), port);
	printf("Task connect - ip:%s,port:%d,connectid:%d\n", ip.data(), port, m_connectid);
	if (m_connectid == -1)
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
	if (m_sptrWorkThread != nullptr)
	{
		m_sptrWorkThread->join();
	}
	if (m_socketserver != NULL)
	{
		free(m_socketserver);
	}
	m_sptrWorkThread = nullptr;
	return true;
}

bool CNetworkTask::SendData(const void * buffer, int size)
{
	if (buffer == NULL || size == 0)
	{
		return false;
	}
	if (m_socketserver == NULL || m_connectid == -1)
	{
		return false;
	}
	int64_t wb_size = socket_server_send(m_socketserver, m_connectid, buffer, size);
	if (wb_size == -1)
	{
		return false;
	}
	return wb_size >= 0;
}

std::shared_ptr<struct tagEventRequest> CNetworkTask::GetAsyncRequest()
{
	return GetEventRequest();
}

// ----------------------------------------------------------------------------------------------------------

bool CNetworkTask::SocketListen()
{
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
			switch (type)
			{
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
	//printf("Task exit(%lu) [id=%d] size=%d\n", result.opaque, result.id, result.ud);
	auto sptrRequest = std::make_shared<struct tagEventRequest>();
	sptrRequest->eventid = NETWORK_NOTIFY_EXITED;
	sptrRequest->contextid = result.id;
	sptrRequest->handle = result.opaque;
	AddEventRequest(std::move(sptrRequest));
	return 0;
}
int CNetworkTask::NotifyData(struct socket_message & result)
{
	//printf("Task message(%lu) [id=%d] size=%d\n", result.opaque, result.id, result.ud);

	memcpy(m_rbuffer + m_rlength, result.data, result.ud);
	m_rlength += result.ud;
	free(result.data);

	do
	{
		if (m_rlength < (int)PACKET_HEADER_SIZE)
		{
			return 1;
		}
		struct packet_header * ptr = (struct packet_header *)m_rbuffer;
		int size = ptr->length;
		if (size > m_rlength)
		{
			return -1;
		}
		struct packet_buffer pack;
		if (size <= (int)sizeof(struct packet_buffer))
		{
			memcpy(&pack, m_rbuffer, size);
		}
		m_rlength -= size;
		memcpy(m_rbuffer, m_rbuffer + size, m_rlength);
		//send
		auto sptrRequest = std::make_shared<struct tagEventRequest>();
		sptrRequest->eventid = NETWORK_NOTIFY_READED;
		sptrRequest->contextid = result.id;
		sptrRequest->handle = result.opaque;
		sptrRequest->data = std::move(pack);
		AddEventRequest(std::move(sptrRequest));
	} while (true);

	return 0;
}
int CNetworkTask::NotifyClose(struct socket_message & result)
{
	//printf("Task close(%lu) [id=%d]\n", result.opaque, result.id);
	auto sptrRequest = std::make_shared<struct tagEventRequest>();
	sptrRequest->eventid = NETWORK_NOTIFY_CLOSED;
	sptrRequest->contextid = result.id;
	sptrRequest->handle = result.opaque;
	AddEventRequest(std::move(sptrRequest));
	return 0;
}
int CNetworkTask::NotifyOpen(struct socket_message & result)
{
	//printf("Task open(%lu) [id=%d] %s\n", result.opaque, result.id, result.data);
	auto sptrRequest = std::make_shared<struct tagEventRequest>();
	sptrRequest->eventid = NETWORK_NOTIFY_OPENED;
	sptrRequest->contextid = result.id;
	sptrRequest->handle = result.opaque;
	strcpy(sptrRequest->data.buffer, result.data);

	AddEventRequest(std::move(sptrRequest));
	return 0;
}
int CNetworkTask::NotifyError(struct socket_message & result)
{
	//printf("Task error(%lu) [id=%d]\n", result.opaque, result.id);
	auto sptrRequest = std::make_shared<struct tagEventRequest>();
	sptrRequest->eventid = NETWORK_NOTIFY_ERROR;
	sptrRequest->contextid = result.id;
	sptrRequest->handle = result.opaque;
	AddEventRequest(std::move(sptrRequest));
	return 0;
}
int CNetworkTask::NotifyAccept(struct socket_message & result)
{
	//printf("Task accept(%lu) [id=%d %s] from [%d]\n", result.opaque, result.ud, result.data, result.id);

	struct socket_server *ss = m_socketserver;
	socket_server_start(ss, result.opaque, result.ud);
	
	auto sptrRequest = std::make_shared<struct tagEventRequest>();
	sptrRequest->eventid = NETWORK_NOTIFY_ACCENT;
	sptrRequest->contextid = result.id;
	sptrRequest->handle = result.opaque;
	//sptrRequest->data.header.handler = result.ud;	
	AddEventRequest(std::move(sptrRequest));

	return 0;
}

// ----------------------------------------------------------------------------------------------------------

void CNetworkTask::AddEventRequest(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	if (sptrRequest != nullptr)
	{
		m_queue_mutex_request.lock();
		m_queueRequest.push(sptrRequest);
		m_queue_mutex_request.unlock();
	}
}

std::shared_ptr<struct tagEventRequest> CNetworkTask::GetEventRequest()
{
	std::shared_ptr<struct tagEventRequest> sptrRequest = nullptr;

	m_queue_mutex_request.lock();
	if (m_queueRequest.empty() == false)
	{
		sptrRequest = m_queueRequest.front();
		m_queueRequest.pop();
	}
	m_queue_mutex_request.unlock();
	return sptrRequest;
}

// ----------------------------------------------------------------------------------------------------------


