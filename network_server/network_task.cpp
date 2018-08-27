
#include "network_task.h"

#include <iomanip>


int CNetworkTask::OnDisposeEvents()
{
	//printf("listen Task OnDisposeEvents - Input fd:%d,errno:%d\n", 3, errno);

	memset(m_events, 0, sizeof(m_events));
	int nfds = socket_wait(m_epfd, m_events, MAX_SOCKET_CONNECT, 0);

	//printf("Task OnDisposeEvents - all nfds:%d,errno:%d\n", nfds, errno);

	if (nfds > 0)
	{
		for (int i = 0; i < nfds; i++)
		{
			int fd = m_events[i].data.fd;
			int ev = m_events[i].events;
			if (fd == m_listenfd)
			{
				//if (ev & EPOLLERR)
				//{
				//	printf("listen Task OnDisposeEvents - Error fd:%d,errno:%d\n", fd, errno);
				//	//HangupNotify(fd);
				//	continue;
				//}
				//if (ev & EPOLLHUP)
				//{
				//	printf("listen Task OnDisposeEvents - Hangup fd:%d,errno:%d\n", fd, errno);
				//	//HangupNotify(fd);
				//	continue;
				//}
				if (ev & EPOLLIN)
				{
					//printf("listen Task OnDisposeEvents - Input fd:%d,errno:%d\n", fd, errno);
					AcceptNotify(fd);
					SetSocketEvents(m_epfd, fd, EPOLL_CTL_MOD, EPOLLERR | EPOLLHUP | EPOLLIN);
					//InputNotify(fd);
					//SetSocketEvents(m_epfd, fd, EPOLL_CTL_MOD);
					continue;
				}
				//if (ev && EPOLLOUT)
				//{
				//	printf("listen Task OnDisposeEvents - Output fd:%d,errno:%d\n", fd, errno);

				//	//OutputNotify();
				//	continue;
				//}
			}
			else
			{
				if (ev & EPOLLERR)
				{
					printf("client Task OnDisposeEvents - Error fd:%d,errno:%d\n", fd, errno);
					HangupNotify(fd);
					continue;
				}
				if (ev & EPOLLHUP)
				{
					printf("client Task OnDisposeEvents - Hangup fd:%d,errno:%d\n", fd, errno);
					HangupNotify(fd);
					continue;
				}
				if (ev & EPOLLIN)
				{
					//printf("client Task OnDisposeEvents - input fd:%d,errno:%d\n", fd, errno);

					InputNotify(fd);
					SetSocketEvents(m_epfd, fd, EPOLL_CTL_MOD, EPOLLERR | EPOLLHUP | EPOLLIN);
					continue;
				}
				if (ev & EPOLLOUT)
				{
					printf("client Task OnDisposeEvents - output fd:%d,errno:%d\n", fd, errno);

					OutputNotify(fd);
					continue;
				}
			}
		}
	}
	return nfds;
}

int CNetworkTask::OnSendQueueData()
{
	m_queue_mutex_send.lock();
	bool bIsEmpty = m_queueSend.empty();
	m_queue_mutex_send.unlock();
	//printf("1 Task OnSendQueueData - m_alength:%d,m_slength:%d,bIsEmpty:%d\n", m_alength, m_slength, bIsEmpty);

	if (bIsEmpty == true)
	{
		return -1;
	}
	//printf("2 Task OnSendQueueData - m_alength:%d,m_slength:%d\n", m_alength, m_slength);

	if (m_slength > 0)
	{
		return 0;
	}
	//printf("3 Task OnSendQueueData - m_alength:%d,m_slength:%d\n", m_alength, m_slength);

	m_queue_mutex_send.lock();
	auto sptrData = m_queueSend.front();
	m_queueSend.pop();
	m_queue_mutex_send.unlock();
	if (sptrData == nullptr)
	{
		return -1;
	}
	//printf("4 Task OnSendQueueData - m_alength:%d,m_slength:%d\n", m_alength, m_slength);

	m_alength = 0;
	m_slength = sptrData->length;
	m_sfd = sptrData->fd;
	memcpy(m_sbuffer, sptrData->buffer, sptrData->length);

	printf("Task OnSendQueueData - m_alength:%d,m_slength:%d,m_sfd:%d\n", m_alength, m_slength, m_sfd);

	return 1;
}

int CNetworkTask::SendBuffer()
{
	if (m_slength <= 0 || m_sfd == -1)
	{
		return 0;
	}
	//int old_slength = m_slength;
	int slen = write(m_sfd, m_sbuffer + m_alength, m_slength);
	m_slength -= slen;
	m_alength += slen;

	if (m_slength <= 0)
	{
		m_sfd = -1;
	}

	return slen;
}



int CNetworkTask::AcceptNotify(int fd)
{
	socklen_t client_addr_len = 0;
	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(struct sockaddr_in));
	int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len);
	//printf("CNetworkTask::AcceptNotify - fd:%d,client_fd:%d\n", fd, client_fd);

	if (client_fd == -1)
	{
		//printf("CNetworkTask::AcceptNotify - accept errno:%d,- %s.\n", errno,strerror(errno));
		return -1;
	}
	else
	{
		bool flag = false;
		do
		{
			flag = SetSocketNonblock(client_fd);
			if (flag == false)
			{
				break;
			}
			
			flag = SetSocketEvents(m_epfd, client_fd, EPOLL_CTL_ADD, EPOLLERR | EPOLLHUP | EPOLLIN);
			if (flag == false)
			{
				break;
			}
			auto spAddr = std::make_shared<struct sockaddr_in>(client_addr);
			//std::pair< std::map<int, std::shared_ptr<struct sockaddr_in>>::iterator, bool > pairRet;
			auto pairRet = m_peerfd.insert(std::make_pair(client_fd, spAddr));
			flag = pairRet.second;
			if (flag == false)
			{
				break;
			}
			auto sptrRequest = std::make_shared<struct tagEventRequest>();
			sptrRequest->init();
			sptrRequest->eventid = NETWORK_NOTIFY_ACCENT;
			sptrRequest->contextid = client_fd;
			memcpy(&sptrRequest->address, &client_addr,sizeof(sptrRequest->address));
			AddEventRequest(std::move(sptrRequest));
			return 1;
		} while (false);
		close(client_fd);
		return -1;
	}
}

int CNetworkTask::HangupNotify(int fd)
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

int CNetworkTask::InputNotify(int fd)
{
	int maxCharCount = sizeof(m_rbuffer) - m_rlength;
	int nread = read(fd, m_rbuffer + m_rlength, maxCharCount);

	//printf("Task InputNotify - nread:%d,m_rlength:%d\n", nread, m_rlength);

	if (nread == 0)
	{
		HangupNotify(fd);
		return -1;
	}
	else if (nread < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR || errno == EMSGSIZE)
		{
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
		//printf("Task InputNotify - nread:%d,m_rlength:%d\n", nread, m_rlength);

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

int CNetworkTask::OutputNotify(int fd)
{
	return 1;
}

bool CNetworkTask::SocketListen()
{
	m_bRunFlag = true;
	m_epfd = epoll_create(MAX_SOCKET_CONNECT);
	if (m_epfd == -1)
	{
		return false;
	}
	m_listenfd = socket_bind(m_strIP.data(), m_port);
	if (m_listenfd == -1)
	{
		return false;
	}
	bool flag = SetSocketEvents(m_epfd, m_listenfd, EPOLL_CTL_ADD, EPOLLERR | EPOLLHUP | EPOLLIN);
	if (flag == false)
	{
		return false;
	}
	return true;
}

void CNetworkTask::runThreadFunction(CNetworkTask *pTask)
{
	bool bListen = pTask->SocketListen();
	printf("Task listen - ip:%s,port:%d,listenfd:%d,bListen:%d\n", pTask->GetIP().data(), pTask->GetPort(), pTask->GetListenFd(), bListen);
	if (bListen == true)
	{
		while (pTask != nullptr && pTask->m_bRunFlag == true)
		{
			if (pTask->OnDisposeEvents() > 0)
			{

			}
			pTask->OnSendQueueData();
			pTask->SendBuffer();

			//else
			//{
			//	thread_sleep(1);
			//}
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
	m_rlength = 0;
	memset(m_rbuffer, 0, sizeof(m_rbuffer));

	m_sfd = -1;
	m_slength = 0;
	m_alength = 0;
	memset(m_sbuffer, 0, sizeof(m_sbuffer));

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
	//std::thread worker_thread(startfunc);
	//m_workThread = std::move(worker_thread);
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
	//m_workThread.join();
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
		m_queue_mutex_send.lock();
		m_queueSend.push(sptrData);
		m_queue_mutex_send.unlock();
	}
	return true;
}