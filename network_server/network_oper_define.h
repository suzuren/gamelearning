
#ifndef __NETWORK_OPER_DEFINE_H_
#define __NETWORK_OPER_DEFINE_H_

#include <string>
#include <memory.h>
// ---------------------------------------------------------------------------------------


#define CHECK_CONNECT_TIME (20*1000)

// ---------------------------------------------------------------------------------------

#define MAX_EVENT_PARAM_COUNT 3

struct tagEventRequest
{
	int	eventid;
	int callback;
	long long params[MAX_EVENT_PARAM_COUNT];
	tagEventRequest()
	{
		init();
	}
	void init()
	{
		eventid = 0;
		callback = 0;
		memset(params, 0, sizeof(params));
	}
};

struct tagEventResponse
{
	int	eventid;
	int callback;
	long long params[MAX_EVENT_PARAM_COUNT];
	tagEventResponse()
	{
		init();
	}
	void init()
	{
		eventid = 0;
		callback = 0;
		memset(params, 0, sizeof(params));
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


