
#ifndef __MYSQL_MGR_H_
#define __MYSQL_MGR_H_

#include "mysql.hpp"
#include "data_table.hpp"

#include "mysql_oper_define.h"
#include "mysql_task.h"

#include <iostream>


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
	struct tagDataBaseConfig m_dbConfig[DB_INDEX_TYPE_MAX];
	std::shared_ptr<CMysqlTask>  m_sptrDBAsyncOper[DB_INDEX_TYPE_MAX];
protected:
	std::shared_ptr<AsyncDBCallBack> m_sptrAsyncNetCallBack;
private:

	unsigned long long	GetMillisecond();
	std::string FormatToString(const char* fmt, ...);
	void	DispatchNetworkEvent();

public:
	bool	Init();
	void    ShutDown();
	void    OnNetworkTick();
	void	SetAsyncNetCallBack(std::shared_ptr<AsyncNetCallBack> sptrAsyncNetCallBack);
	void	DispatchNetworkCallBack(std::shared_ptr<struct tagEventResponse> sptrResponse);

public:
	void	TestNetwork();
};


#endif


