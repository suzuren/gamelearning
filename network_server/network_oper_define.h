
#ifndef __NETWORK_OPER_DEFINE_H_
#define __NETWORK_OPER_DEFINE_H_

#include <string>
#include <memory.h>
#include "stream_decoder.h"
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
	int contextid;
	int fd;
	struct packet_buffer data;
	tagEventResponse()
	{
		init();
	}
	void init()
	{
		eventid = 0;
		contextid = 0;
		fd = -1;
		data.init();
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


