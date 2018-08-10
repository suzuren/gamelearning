
#ifndef __MYSQL_MGR_H_
#define __MYSQL_MGR_H_

#include "mysql.hpp"
#include "data_table.hpp"

#include "mysql_oper_define.h"


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
	
protected:
	unsigned long long	GetMillisecond();
	void OnCheckConnect();


	
public:
	bool	Init();
	void    ShutDown();
	void    OnMysqlTick();
public:
	void	TestMysql();
	void	TestMysql_One();
	void	TestMysql_Two();
	void	TestMysql_Three();

};


#endif


