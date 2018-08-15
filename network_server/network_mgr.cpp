
#include "network_mgr.h"

// ---------------------------------------------------------------------------------------

void CNetworkMgr::DispatchNetworkEvent()
{
	int count = 0;
	for (int i = 0; i < MAX_WORK_THREAD_COUNT; ++i)
	{
		if (m_sptrNetAsyncOper[i] == nullptr)
		{
			continue;
		}
		do
		{
			auto sptrResponse = m_sptrNetAsyncOper[i]->GetAsyncExecuteResult();
			DispatchNetworkCallBack(std::move(sptrResponse));
			if (++count >= 128)
			{
				sptrResponse = nullptr;
				break;
			}
		} while (true);
	}
}

void CNetworkMgr::DispatchNetworkCallBack(std::shared_ptr<struct tagEventResponse> sptrResponse)
{
	if (m_sptrAsyncNetworkCallBack != nullptr)
	{
		m_sptrAsyncNetCallBack->OnProcessNetworkEvent(sptrResponse);
	}
}

// ---------------------------------------------------------------------------------------

bool CNetworkMgr::Init()
{
	m_listenfd = socket_bind(IPADDR, PORT);
	m_epfd = epoll_create(MAX_SOCKET_CONNECT);
	SetSocketEvents(m_epfd, m_listenfd, EPOLL_CTL_ADD);

	return true;
}

void CNetworkMgr::ShutDown()
{
}

void CNetworkMgr::SetAsyncNetCallBack(std::shared_ptr<AsyncNetCallBack> sptrAsyncNetCallBack)
{
	if (sptrAsyncNetCallBack != nullptr)
	{
		m_sptrAsyncNetCallBack = sptrAsyncNetCallBack;
	}
}

void CNetworkMgr::OnDisposeEvents()
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
				int sock_err = 0;
				int sock_err_len = sizeof(sock_err);
				int sockopt_ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &sock_err, (socklen_t*)&sock_err_len);

				//HangupNotify();
			}
			if (ev && EPOLLIN)
			{
				//InputNotify();
			}
			if (ev && EPOLLOUT)
			{
				//OutputNotify();
			}
		}
	}

}

void CNetworkMgr::OnNetworkTick()
{
	OnDisposeEvents();
	DispatchNetworkEvent();
}

void CNetworkMgr::TestNetwork()
{

}


