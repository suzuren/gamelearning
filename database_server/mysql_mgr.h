
#ifndef __MYSQL_MGR_H_
#define __MYSQL_MGR_H_

#include "mysql.hpp"
#include "data_table.hpp"

#include "mysql_oper_define.h"
#include "mysql_task.h"

#include <iostream>


class AsyncDBCallBack
{
public:
	virtual bool OnProcessDBEvent(std::shared_ptr<struct tagEventResponse> sptrResponse) = 0;
};

class CMysqlMgr
{
protected:
	CMysqlMgr() {}
	~CMysqlMgr()	{}
private:
	const CMysqlMgr & operator=(const CMysqlMgr &);
public:
	static  CMysqlMgr & Instance()
	{
		static  CMysqlMgr  s_SingleObj;
		return  s_SingleObj;
	}
private:
	unsigned long long m_lLastCheckTime;
	struct tagDataBaseConfig m_dbConfig[DB_INDEX_TYPE_MAX];
	db::mysql m_dbSyncOper[DB_INDEX_TYPE_MAX];
	std::shared_ptr<CMysqlTask>  m_sptrDBAsyncOper[DB_INDEX_TYPE_MAX];
protected:
	std::shared_ptr<AsyncDBCallBack> m_sptrAsyncDBCallBack;
private:
	bool InitDatabaseConfigure();
	bool ConnectSyncOper();
	bool ConnectAsyncOper();
	unsigned long long	GetMillisecond();
	std::string FormatToString(const char* fmt, ...);
	void OnCheckConnect();
	void	DispatchDataBaseEvent();

public:
	bool	Init();
	void    ShutDown();
	void    OnMysqlTick();
	void	SetAsyncDBCallBack(std::shared_ptr<AsyncDBCallBack> sptrAsyncDBCallBack);
	void	DispatchDataBaseCallBack(std::shared_ptr<struct tagEventResponse> sptrResponse);


public:
	void	TestMysql();
	void	TestMysql_One();
	void	TestMysql_Two();
	void	TestMysql_Three();
	void	TestMysql_Four();

};


#endif


