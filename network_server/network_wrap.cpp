
#include "network_wrap.h"
#include <iomanip>


int CNetworkWrap::OnDisposeEvents()
{
	memset(m_events, 0, sizeof(m_events));
	int nfds = socket_wait(m_epfd, m_events, 64, 0);
	if (nfds > 0)
	{
		for (int i = 0; i < nfds; i++)
		{
			int fd = m_events[i].data.fd;
			int ev = m_events[i].events;

			if (ev && (EPOLLHUP | EPOLLERR))
			{
				//printf("Wrap OnDisposeEvents - Hangup fd:%d,errno:%d,EALREADY:%d,EINPROGRESS:%d\n", fd, errno, EALREADY, EINPROGRESS);
				if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR || errno == EALREADY || errno == EINPROGRESS)
				{
					//continue;
				}
				else
				{
					printf("Wrap OnDisposeEvents - Hangup fd:%d,errno:%d,EALREADY:%d,EINPROGRESS:%d\n", fd, errno, EALREADY, EINPROGRESS);
					HangupNotify(fd);
				}
			}
			else if (ev && EPOLLIN)
			{
				printf("Wrap OnDisposeEvents - Input fd:%d,errno:%d\n", fd, errno);

				InputNotify(fd);
				SetSocketEvents(m_epfd, fd, EPOLL_CTL_MOD);
			}
			else if (ev && EPOLLOUT)
			{
				printf("Wrap OnDisposeEvents - Output fd:%d,errno:%d\n", fd, errno);

				OutputNotify(fd);
				SetSocketEvents(m_epfd, fd, EPOLL_CTL_MOD);
			}
		}
	}
	return nfds;
}

int CNetworkWrap::OnSendQueueData()
{
	if (m_queueSend.empty() == true)
	{
		return -1;
	}
	if (m_slength > 0)
	{
		return 0;
	}
	std::unique_lock<std::mutex> lock_front(m_queue_mutex_send);
	auto sptrData = m_queueSend.front();
	m_queueSend.pop();
	lock_front.unlock();
	if (sptrData == nullptr)
	{
		return -1;
	}
	m_alength = 0;
	m_slength = PACKET_HEADER_SIZE + sptrData->header.length;
	memcpy(m_sbuffer, &(*sptrData), m_slength);

	return 1;
}

int CNetworkWrap::SendBuffer()
{
	if (m_slength>0)
	{
		int slen = write(m_clientfd, m_sbuffer + m_alength, m_slength);
		m_slength -= slen;
		m_alength += slen;
		return slen;
	}
	return -1;
}

int CNetworkWrap::HangupNotify(int fd)
{
	int sock_err = 0;
	int sock_err_len = sizeof(sock_err);
	int sockopt_ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &sock_err, (socklen_t*)&sock_err_len);
	if (sockopt_ret == 0)
	{
		//success
	}
	else
	{
		//flaited
	}

	auto sptrRequest = std::make_shared<struct tagEventRequest>();
	sptrRequest->init();
	sptrRequest->eventid = NETWORK_NOTIFY_CLOSED;
	sptrRequest->contextid = fd;
	auto spAddr = m_peerfd.find(fd);
	if (spAddr != m_peerfd.end() && spAddr->second != nullptr)
	{
		memcpy(&sptrRequest->address, &(*(spAddr->second)), sizeof(sptrRequest->address));
	}
	AddEventRequest(std::move(sptrRequest));
	close(fd);
	if (spAddr != m_peerfd.end())
	{
		m_peerfd.erase(spAddr);
	}
	return 1;
}

int CNetworkWrap::InputNotify(int fd)
{
	int maxCharCount = sizeof(m_rbuffer) - m_rlength;
	int nread = read(fd, m_rbuffer + m_rlength, maxCharCount);
	//printf("Wrap InputNotify - nread:%d,fd:%d,errno:%d\n", nread, fd, errno);

	if (nread == 0)
	{
		return -1;
	}
	else if (nread < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR || errno == EMSGSIZE || errno == EALREADY || errno == EINPROGRESS)
		{
			printf("Wrap InputNotify - nread:%d,fd:%d,errno:%d\n", nread, fd, errno);

			return 0;
		}
		else
		{
			//close(fd);
			HangupNotify(fd);
			return -1;
		}
		return -1;
	}
	else
	{
		m_rlength += nread;
		do
		{
			int size = ParsePacket(m_rbuffer, m_rlength);
			if (size < 0)
			{
				// error
			}
			if (size > 0)
			{
				struct packet_header * ptr = (struct packet_header *)m_rbuffer;
				struct packet_buffer pack;
				pack.header.identity = ptr->identity;
				pack.header.command = ptr->command;
				pack.header.length = ptr->length;
				memcpy(pack.buffer, m_rbuffer + GetPacketHeaderLength(), ptr->length);
				
				m_rlength -= size;
				memcpy(m_rbuffer, m_rbuffer + size, m_rlength);

				//
				auto sptrRequest = std::make_shared<struct tagEventRequest>();
				sptrRequest->init();
				sptrRequest->eventid = NETWORK_NOTIFY_READED;
				sptrRequest->contextid = fd;
				auto spAddr = m_peerfd.find(fd);
				if (spAddr != m_peerfd.end() && spAddr->second != nullptr)
				{
					memcpy(&sptrRequest->address, &(*(spAddr->second)), sizeof(sptrRequest->address));
				}
				sptrRequest->data = std::move(pack);
				AddEventRequest(std::move(sptrRequest));
			}
		} while (m_rlength >= GetPacketHeaderLength());
		return 1;
	}
	return -1;
}

int CNetworkWrap::OutputNotify(int fd)
{
	SetStatus(NET_WRAP_SOCKET_STATUS_CONNECTED);
	printf("wrap OutputNotify - fd:%d,errno:%d \n", fd, errno);

	auto sptrRequest = std::make_shared<struct tagEventRequest>();
	sptrRequest->init();
	sptrRequest->eventid = NETWORK_NOTIFY_CONNECT;
	sptrRequest->contextid = fd;
	auto spAddr = m_peerfd.find(fd);
	if (spAddr != m_peerfd.end() && spAddr->second != nullptr)
	{
		memcpy(&sptrRequest->address, &(*(spAddr->second)), sizeof(sptrRequest->address));
	}
	AddEventRequest(std::move(sptrRequest));
	return 1;
}

bool CNetworkWrap::SocketConnect()
{
	m_bRunFlag = true;
	m_epfd = epoll_create(64);
	if (m_epfd == -1)
	{
		return false;
	}
	int ret = socket_connect(m_strIP.data(), m_port, &m_clientfd);
	if (ret == -1)
	{
		return false;
	}
	bool flag = SetSocketEvents(m_epfd, m_clientfd, EPOLL_CTL_ADD);
	if (flag == false)
	{
		return false;
	}
	return true;
}

void CNetworkWrap::runThreadFunction(CNetworkWrap *pTask)
{
	bool bConnect = pTask->SocketConnect();
	printf("Wrap connect - ip:%s,port:%d,clientfd:%d,bConnect:%d", pTask->GetIP().data(), pTask->GetPort(), pTask->GetClientFd(), bConnect);
	if (bConnect == true)
	{
		while (pTask != nullptr && pTask->m_bRunFlag == true)
		{
			if (pTask->OnDisposeEvents() > 0)
			{
				//break;
			}
			if (pTask->GetStatus() == NET_WRAP_SOCKET_STATUS_CONNECTED)
			{
				pTask->OnSendQueueData();
				pTask->SendBuffer();
			}
			else
			{
				thread_sleep(1);
			}
		}
	}
}


CNetworkWrap::CNetworkWrap()
{

}

CNetworkWrap::~CNetworkWrap()
{

}

bool CNetworkWrap::Init()
{
	m_bRunFlag = true;
	m_rlength = 0;
	memset(m_rbuffer, 0, sizeof(m_rbuffer));

	m_slength = 0;
	m_alength = 0;
	memset(m_sbuffer, 0, sizeof(m_sbuffer));

	m_status = 0;
	return true;
}

bool CNetworkWrap::Start(std::string ip,int port)
{
	m_port = port;
	m_strIP = ip;

	auto startfunc = std::bind(runThreadFunction, this);
	std::thread worker_thread(startfunc);
	m_workThread = std::move(worker_thread);

	return true;
}

bool CNetworkWrap::ShutDown()
{
	m_bRunFlag = false;
	m_workThread.join();
	return true;
}

void CNetworkWrap::AddEventRequest(std::shared_ptr<struct tagEventRequest> sptrRequest)
{
	if (m_bRunFlag && sptrRequest != nullptr)
	{
		std::unique_lock<std::mutex> lock_front(m_queue_mutex_request);
		m_queueRequest.push(sptrRequest);
		lock_front.unlock();
	}
}

std::shared_ptr<struct tagEventRequest> CNetworkWrap::GetEventRequest()
{
	if (m_queueRequest.empty() == false)
	{
		std::unique_lock<std::mutex> lock_front(m_queue_mutex_request);
		auto sptrRequest = m_queueRequest.front();
		m_queueRequest.pop();
		lock_front.unlock();
		return sptrRequest;
	}
	return nullptr;
}

std::shared_ptr<struct tagEventRequest> CNetworkWrap::GetAsyncRequest()
{
	return GetEventRequest();
}

bool CNetworkWrap::SendData(std::shared_ptr<struct packet_buffer> sptrData)
{
	if (sptrData !=  nullptr)
	{
		std::unique_lock<std::mutex> lock_front(m_queue_mutex_send);
		m_queueSend.push(sptrData);
		lock_front.unlock();
	}
	return true;
}

