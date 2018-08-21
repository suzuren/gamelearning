
#ifndef __NETWORK_MGR_H_
#define __NETWORK_MGR_H_

#include "network_oper_define.h"
#include "network_task.h"



class AsyncNetCallBack
{
public:
	virtual bool OnProcessNetworkEvent(std::shared_ptr<struct tagEventRequest> sptrRequest) = 0;
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
	std::shared_ptr<CNetworkTask>  m_sptrNetAsyncOper;

	std::shared_ptr<AsyncNetCallBack> m_sptrAsyncNetCallBack;
private:
	void	DispatchNetworkRequest();
	void	DispatchNetworkCallBack(std::shared_ptr<struct tagEventRequest> sptrRequest);

public:
	bool	Init();
	void    ShutDown();
	void    OnNetworkTick();
	void	SetAsyncNetCallBack(std::shared_ptr<AsyncNetCallBack> sptrAsyncNetCallBack);

public:
	void	TestNetwork();
};


#endif


