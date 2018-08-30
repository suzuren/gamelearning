
#ifndef __NETWORK_OPER_DEFINE_H_
#define __NETWORK_OPER_DEFINE_H_

#include <string>
#include <memory.h>
#include <netinet/in.h>

// ---------------------------------------------------------------------------------------


#define CHECK_CONNECT_TIME (20*1000)

// ---------------------------------------------------------------------------------------


#define MAX_TIME_OUT		(30*1000)
#define MAX_SOCKET_CONNECT	10240



#define IPADDR				"127.0.0.1"
#define PORT				9876
// ---------------------------------------------------------------------------------------

#define MAX_EVENT_PARAM_COUNT 3

struct packet_header
{
	unsigned long long	handler;
	int					command;
	int					length;
	packet_header()
	{
		handler = 0;
		command = 0;
		length = 0;
	}
};


#define MAX_PACKRT_BUFFER   (65535*5)
#define SOCKET_TCP_BUFFER   65535


#define MAX_PACKRT_BUFFER   (65535*5)
#define SOCKET_TCP_BUFFER   65535

#define PACKET_MAX_SIZE             4096*4


#define PACKET_MAX_SIZE             4096*4
#define PACKET_HEADER_SIZE          (sizeof(struct packet_header))
#define PACKET_MAX_DATA_SIZE		(PACKET_MAX_SIZE - PACKET_HEADER_SIZE)


struct packet_buffer
{
	packet_header header;
	char buffer[PACKET_MAX_DATA_SIZE];
	packet_buffer()
	{
		init();
	}
	void init()
	{
		memset(buffer, 0, sizeof(buffer));
	}
};

struct tagEventRequest
{
	int	eventid;
	int contextid;
	struct packet_buffer data;
	tagEventRequest()
	{
		init();
	}
	void init()
	{
		eventid = 0;
		contextid = -1;
		data.init();
	}
};

// ---------------------------------------------------------------------------------------
enum NETWORK_NOTIFY_TYPE
{
	NETWORK_NOTIFY_INITIO,
	NETWORK_NOTIFY_CLOSED,
	NETWORK_NOTIFY_ACCENT,
	NETWORK_NOTIFY_CONNECT,
	NETWORK_NOTIFY_READED,
	NETWORK_NOTIFY_SENDED,
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


