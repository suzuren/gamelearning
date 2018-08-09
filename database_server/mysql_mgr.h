
#ifndef __HTTP_MGR_H_
#define __HTTP_MGR_H_

#include "mysql.hpp"
#include "data_table.hpp"
#include <iostream>

#define CHECK_CONNECT_TIME (20*1000)

enum DB_INDEX_TYPE
{
	DB_INDEX_TYPE_ACCOUNT = 0,
	DB_INDEX_TYPE_RECORD,
	DB_INDEX_TYPE_TREASURE,
	DB_INDEX_TYPE_MAX,
};

struct tagDataBaseConfig
{
	int port;
	int timeout;
	std::string host;
	std::string user;
	std::string password;
	std::string database;
	tagDataBaseConfig()
	{
		init();
	}
	void init()
	{
		port = 0;
		timeout = 0;
		host.clear();
		user.clear();
		password.clear();
		database.clear();
	}
	void operator=(const tagDataBaseConfig & conf)
	{
		port = conf.port;
		timeout = conf.timeout;
		host = conf.host;
		user = conf.user;
		password = conf.password;
		database = conf.database;
	}
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

};


#endif


