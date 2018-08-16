
#ifndef __MYSQL_MGR_H_
#define __MYSQL_MGR_H_

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
	std::shared_ptr<CNetworkTask>  m_sptrNetAsyncOper;

protected:
	std::shared_ptr<AsyncNetCallBack> m_sptrAsyncNetCallBack;
private:
	void	DispatchNetworkEvent();
	void	DispatchNetworkCallBack(std::shared_ptr<struct tagEventResponse> sptrResponse);

public:
	bool	Init();
	void    ShutDown();
	void    OnNetworkTick();
	void	SetAsyncNetCallBack(std::shared_ptr<AsyncNetCallBack> sptrAsyncNetCallBack);

public:
	void	TestNetwork();
};


#endif


