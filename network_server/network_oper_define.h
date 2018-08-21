
#ifndef __NETWORK_OPER_DEFINE_H_
#define __NETWORK_OPER_DEFINE_H_

#include <string>
#include <memory.h>
#include <netinet/in.h>
#include "stream_decoder.h"

// ---------------------------------------------------------------------------------------


#define CHECK_CONNECT_TIME (20*1000)

// ---------------------------------------------------------------------------------------

#define MAX_EVENT_PARAM_COUNT 3

struct tagEventRequest
{
	int	eventid;
	int contextid;
	struct sockaddr_in address;
	struct packet_buffer data;
	tagEventRequest()
	{
		init();
	}
	void init()
	{
		eventid = 0;
		contextid = -1;
		memset(&address, 0, sizeof(struct sockaddr_in));
		data.init();
	}
};

// ---------------------------------------------------------------------------------------
enum NETWORK_NOTIFY_TYPE
{
	NETWORK_NOTIFY_CLOSED,
	NETWORK_NOTIFY_ACCENT,
	NETWORK_NOTIFY_READED,
};

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


