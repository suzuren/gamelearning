
#ifndef __NETWORK_MGR_H_
#define __NETWORK_MGR_H_

#include "network_task.h"

#include <vector>

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
	std::shared_ptr<CNetworkTask>  m_sptrNetTaskOper;



	std::shared_ptr<AsyncNetCallBack> m_sptrAsyncNetTaskCallBack;
private:
	void	DispatchNetworkRequest();

	bool StartTask();
	bool StartWrap();

public:
	bool	Init();
	void    ShutDown();
	void    OnNetworkTick();
	void	SetAsyncNetTaskCallBack(std::shared_ptr<AsyncNetCallBack> sptrAsyncNetCallBack);

public:
	void	TestNetwork();
	void	TestNetworkTaskSendData(int contextid, struct packet_buffer & data);

};


#endif


