
#include "network_wrap.h"

#include "stream_decoder.h"

#include "network_oper_define.h"

#include "pack_proto_define.h"

#include <iomanip>

int CNetworkWrap::OnDisposeEvents()
{
	memset(m_events, 0, sizeof(m_events));
	int nfds = socket_wait(m_epfd, m_events, 64, 0);

	//printf("Wrap OnDisposeEvents - all nfds:%d,errno:%d\n", nfds, errno);

	if (nfds > 0)
	{
		for (int i = 0; i < nfds; i++)
		{
			int fd = m_events[i].data.fd;
			int ev = m_events[i].events;

			//if (ev && (EPOLLHUP | EPOLLERR))
			//{
			//	//printf("Wrap OnDisposeEvents - Hangup fd:%d,errno:%d,EALREADY:%d,EINPROGRESS:%d\n", fd, errno, EALREADY, EINPROGRESS);
			//	if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR || errno == EALREADY || errno == EINPROGRESS)
			//	{
			//		//continue;
			//	}
			//	else
			//	{
			//		printf("Wrap OnDisposeEvents - Hangup fd:%d,errno:%d,EALREADY:%d,EINPROGRESS:%d\n", fd, errno, EALREADY, EINPROGRESS);
			//		HangupNotify(fd);
			//	}
			//	continue;
			//}
			if (ev && EPOLLOUT)
			{
				printf("Wrap OnDisposeEvents - Output fd:%d,errno:%d\n", fd, errno);

				OutputNotify(fd);
				SetSocketEvents(m_epfd, fd, EPOLL_CTL_MOD, EPOLLIN);
				continue;
			}
			if (ev && EPOLLIN)
			{
				//printf("Wrap OnDisposeEvents - Input fd:%d,errno:%d\n", fd, errno);

				InputNotify(fd);
				SetSocketEvents(m_epfd, fd, EPOLL_CTL_MOD, EPOLLIN);
				continue;
			}
		}
	}
	return nfds;
}

int CNetworkWrap::OnSendQueueData()
{
	std::unique_lock<std::mutex> lock_empty(m_queue_mutex_send);
	bool bIsEmpty = m_queueSend.empty();
	lock_empty.unlock();
	//printf("1 Wrap OnSendQueueData - m_alength:%d,m_slength:%d,bIsEmpty:%d\n", m_alength, m_slength, bIsEmpty);

	if (bIsEmpty == true)
	{
		return -1;
	}
	//printf("2 Wrap OnSendQueueData - m_alength:%d,m_slength:%d\n", m_alength, m_slength);

	if (m_slength > 0)
	{
		return 0;
	}
	//printf("3 Wrap OnSendQueueData - m_alength:%d,m_slength:%d\n", m_alength, m_slength);

	std::unique_lock<std::mutex> lock_front(m_queue_mutex_send);
	auto sptrData = m_queueSend.front();
	m_queueSend.pop();
	lock_front.unlock();
	if (sptrData == nullptr)
	{
		return -1;
	}
	//printf("4 Wrap OnSendQueueData - m_alength:%d,m_slength:%d\n", m_alength, m_slength);

	m_alength = 0;
	m_slength = sptrData->length;
	memcpy(m_sbuffer, sptrData->buffer, sptrData->length);

	printf("Wrap OnSendQueueData - m_alength:%d,m_slength:%d\n", m_alength, m_slength);

	return 1;
}

int CNetworkWrap::SendBuffer()
{
	if (m_slength>0)
	{
		int old_slength = m_slength;
		int slen = write(m_clientfd, m_sbuffer + m_alength, m_slength);
		m_slength -= slen;
		m_alength += slen;

		printf("Wrap SendBuffer - m_alength:%d,m_slength:%d,old_slength:%d,slen:%d\n",
			m_alength, m_slength, old_slength, slen);

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
	//printf("wrap OutputNotify - fd:%d,errno:%d \n", fd, errno);

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
	bool flag = SetSocketEvents(m_epfd, m_clientfd, EPOLL_CTL_ADD, EPOLLOUT);
	if (flag == false)
	{
		return false;
	}
	return true;
}

void CNetworkWrap::runThreadFunction(CNetworkWrap *pTask)
{
	bool bConnect = pTask->SocketConnect();
	printf("Wrap connect - ip:%s,port:%d,clientfd:%d,bConnect:%d\n", pTask->GetIP().data(), pTask->GetPort(), pTask->GetClientFd(), bConnect);
	if (bConnect == true)
	{
		while (pTask != nullptr && pTask->m_bRunFlag == true)
		{
			if (pTask->OnDisposeEvents() > 0)
			{
				//break;
			}
			//int nfds = pTask->OnDisposeEvents();
			//printf("Wrap run - nfds:%d,status:%d\n", nfds, pTask->GetStatus());
			if (pTask->GetStatus() == NET_WRAP_SOCKET_STATUS_CONNECTED)
			{
				pTask->OnSendQueueData();
				pTask->SendBuffer();
			}
			//else
			//{
			//	thread_sleep(1);
			//}
		}
	}
	printf("CNetworkWrap::runThreadFunction exit ... \n");
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

	m_sptrWorkThread = nullptr;
	return true;
}

bool CNetworkWrap::Start(std::string ip,int port)
{
	m_port = port;
	m_strIP = ip;

	auto startfunc = std::bind(runThreadFunction, this);
	//std::thread worker_thread(startfunc);
	//m_workThread = std::move(worker_thread);
	m_sptrWorkThread = std::make_shared<std::thread>(startfunc);
	if (m_sptrWorkThread == nullptr)
	{
		return false;
	}
	return true;
}

bool CNetworkWrap::ShutDown()
{
	m_bRunFlag = false;
	//m_workThread.join();
	if (m_sptrWorkThread != nullptr)
	{
		m_sptrWorkThread->join();
		m_sptrWorkThread = nullptr;
	}
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

bool CNetworkWrap::SendData(std::shared_ptr<struct tagSendData> sptrData)
{
	if (sptrData !=  nullptr)
	{
		std::unique_lock<std::mutex> lock_front(m_queue_mutex_send);
		m_queueSend.push(sptrData);
		lock_front.unlock();
	}
	return true;
}

bool CNetworkWrap::SendDataTest()
{
	packet_buffer data;

	//data.header.identity = 335;
	//data.header.command = NETWORK_EVENT_TEST;
	//strcpy(data.buffer, "hello world!");
	//data.header.length = PACKET_HEADER_SIZE + strlen(data.buffer);

	//std::shared_ptr<pakcet_buffer> sptrData = std::make_shared<pakcet_buffer>(data);

	//sptrData->header.identity = 335;
	//sptrData->header.command = NETWORK_EVENT_TEST;
	//strcpy(sptrData->buffer, "hello world!");
	//sptrData->header.length = PACKET_HEADER_SIZE + strlen(sptrData->buffer);
	//SendData(sptrData);
	return true;
}
