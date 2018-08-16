
#include "network_task.h"
#include <iomanip>


int CNetworkTask::OnDisposeEvents()
{
	memset(m_events, 0, sizeof(m_events));
	int nfds = socket_wait(m_epfd, m_events, MAX_SOCKET_CONNECT, 0);
	if (nfds > 0)
	{
		for (int i = 0; i < nfds; i++)
		{
			int fd = m_events[i].data.fd;
			int ev = m_events[i].events;
			if (ev && (EPOLLHUP | EPOLLERR))
			{
				//int sock_err = 0;
				//int sock_err_len = sizeof(sock_err);
				//int sockopt_ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &sock_err, (socklen_t*)&sock_err_len);

				HangupNotify(fd);
			}
			if (fd == m_listenfd)
			{
				AcceptNotify(fd);
			}
			if (ev && EPOLLIN)
			{
				InputNotify(fd);
			}
			if (ev && EPOLLOUT)
			{
				//OutputNotify();
			}
		}
	}
	return nfds;
}

int CNetworkTask::AcceptNotify(int fd)
{
	socklen_t client_addr_len = 0;
	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(struct sockaddr_in));
	int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len);
	if (client_fd == -1)
	{
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
			flag = SetSocketEvents(m_epfd, client_fd, EPOLL_CTL_ADD);
			if (flag == false)
			{
				break;
			}
			auto spAddr = std::make_shared<struct sockaddr_in>(std::move(client_addr));
			//std::pair< std::map<int, std::shared_ptr<struct sockaddr_in>>::iterator, bool > pairRet;
			auto pairRet = m_peerfd.insert(std::make_pair(client_fd, spAddr));
			flag = pairRet.second;
			if (flag == false)
			{
				break;
			}
			return 1;
		} while (false);
		close(client_fd);
		return -1;
	}
}

int CNetworkTask::HangupNotify(int fd)
{
	close(fd);
	//
}

int CNetworkTask::InputNotify(int fd)
{
	int maxCharCount = sizeof(m_rbuffer) - m_rlength;
	int nread = read(fd, m_rbuffer + m_rlength, maxCharCount);
	if (nread == 0)
	{
		return -1;
	}
	else if (nread < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
		{
			return 0;
		}
		else
		{
			close(fd);
			return -1;
		}
		return -1;
	}
	else
	{
		SetSocketEvents(m_epfd, fd, EPOLL_CTL_MOD);
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
			}
			return 1;
		} while (m_rlength >= GetPacketHeaderLength());
	}
	return -1;
}


void CNetworkTask::runThreadFunction(CNetworkTask *pTask)
{
	while (pTask != nullptr && pTask->m_bRunFlag == true)
	{
		if (pTask->OnDisposeEvents() > 0)
		{

		}
		else
		{
			thread_sleep(30);
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
	m_rlength = 0;
	memset(m_rbuffer, 0, sizeof(m_rbuffer));
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

bool CNetworkTask::Start(std::string ip,int port)
{
	m_bRunFlag = true;
	m_epfd = epoll_create(MAX_SOCKET_CONNECT);
	if (m_epfd == -1)
	{
		return false;
	}
	m_listenfd = socket_bind(ip.data(), port);
	if (m_listenfd == -1)
	{
		return false;
	}
	bool flag = SetSocketEvents(m_epfd, m_listenfd, EPOLL_CTL_ADD);
	if (flag == false)
	{
		return false;
	}
	auto startfunc = std::bind(runThreadFunction, this);
	std::thread worker_thread(startfunc);
	m_workThread = std::move(worker_thread);

	return true;
}

bool CNetworkTask::ShutDown()
{
	m_bRunFlag = false;
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
