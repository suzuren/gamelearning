
#ifndef __MYSQL_MGR_H_
#define __MYSQL_MGR_H_

#include "epoll_socket.h"
#include "network_oper_define.h"
#include "network_task.h"



class AsyncNetCallBack
{
public:
	virtual bool OnProcessNetworkEvent(std::shared_ptr<struct tagEventResponse> sptrResponse) = 0;
};

class CNetworkMgr
{
protected:
	CNetworkMgr() {}
	~CNetworkMgr()	{}
private:
	const CNetworkMgr & operator=(const CNetworkMgr &);
public:
	static  CNetworkMgr & Instance()
	{
		static  CNetworkMgr  s_SingleObj;
		return  s_SingleObj;
	}
private:
	int m_epfd;
	int m_listenfd;
	struct epoll_event m_events[MAX_SOCKET_CONNECT];
	std::shared_ptr<CNetworkTask>  m_sptrNetAsyncOper[MAX_WORK_THREAD_COUNT];

protected:
	std::shared_ptr<AsyncDBCallBack> m_sptrAsyncNetCallBack;
private:
	void	DispatchNetworkEvent();
	void	DispatchNetworkCallBack(std::shared_ptr<struct tagEventResponse> sptrResponse);
	void	SetAsyncNetCallBack(std::shared_ptr<AsyncNetCallBack> sptrAsyncNetCallBack);

public:
	bool	Init();
	void    ShutDown();
	void	OnDisposeEvents();
	void    OnNetworkTick();

public:
	void	TestNetwork();
};


#endif


