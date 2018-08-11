
#ifndef __MYSQL_OPER_DEFINE_H_
#define __MYSQL_OPER_DEFINE_H_

// ---------------------------------------------------------------------------------------


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


// ---------------------------------------------------------------------------------------

enum emMYSQL_DATABASE_CALL_BACK
{
	DATABASE_CALL_BACK_NULL = 0,		// 不回调
	DATABASE_CALL_BACK_QUERY_FIELDS,	// 查询结果集
	DATABASE_CALL_BACK_AFFECTED_ROWS,	// 影响行数
	DATABASE_CALL_BACK_MAX,
};

#define MAX_EVENT_PARAM_COUNT 3

struct tagEventRequest
{
	int	eventid;
	int callback;
	long long params[MAX_EVENT_PARAM_COUNT];
	std::string strsql;
	tagEventRequest()
	{
		init();
	}
	void init()
	{
		eventid = 0;
		callback = 0;
		memset(params, 0, sizeof(params));
		strsql.clear();
	}
};

struct tagEventResponse
{
	int	eventid;
	int callback;
	long long params[MAX_EVENT_PARAM_COUNT];
	unsigned int affected_rows;
	std::shared_ptr<db::data_table> sptrResult;
	tagEventResponse()
	{
		init();
	}
	void init()
	{
		eventid = 0;
		callback = 0;
		memset(params, 0, sizeof(params));
		affected_rows = 0;
		sptrResult = nullptr;
	}
};

// ---------------------------------------------------------------------------------------

enum emMYSQL_DATABASE_EVENT_TYPE
{
	MYSQL_DATABASE_EVENT_MIN,
	MYSQL_DATABASE_EVENT_TEST,
	// ...
	// ...
	// ...
	// ...
	MYSQL_DATABASE_EVENT_MAX,
};

// ---------------------------------------------------------------------------------------


#endif


