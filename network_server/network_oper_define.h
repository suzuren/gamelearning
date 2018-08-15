
#ifndef __NETWORK_OPER_DEFINE_H_
#define __NETWORK_OPER_DEFINE_H_

// ---------------------------------------------------------------------------------------


#define CHECK_CONNECT_TIME (20*1000)

// ---------------------------------------------------------------------------------------

#define MAX_WORK_THREAD_COUNT 3
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

enum emNETWORK_EVENT_TYPE
{
	NETWORK_EVENT_MIN,
	NETWORK_EVENT_TEST,
	// ...
	// ...
	// ...
	// ...
	NETWORK_EVENT_MAX,
};

// ---------------------------------------------------------------------------------------


#endif


