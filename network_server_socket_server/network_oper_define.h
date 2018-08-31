
#ifndef __NETWORK_OPER_DEFINE_H_
#define __NETWORK_OPER_DEFINE_H_

#include <string>
#include <memory.h>
#include <netinet/in.h>

// ---------------------------------------------------------------------------------------

#define IPADDR						"127.0.0.1"
#define PORT						9876

// ---------------------------------------------------------------------------------------

#define SOCKET_TCP_BUFFER			65535
#define MAX_RECV_BUFFER_SIZE		(SOCKET_TCP_BUFFER*5)

// ---------------------------------------------------------------------------------------
#pragma  pack(1)

struct packet_header
{
	int					command;
	int					length;
	unsigned long long	handler; // 接受事件的 handler
	packet_header()
	{
		handler = 0;
		command = 0;
		length = 0;
	}
};

#define PACKET_HEADER_SIZE          (sizeof(struct packet_header))
#define PACKET_MAX_DATA_SIZE		(SOCKET_TCP_BUFFER - PACKET_HEADER_SIZE)

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

#pragma pack()

struct tagEventRequest
{
	int	eventid;
	int contextid;
	unsigned long long handle; // 触发事件的 handler
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
	NETWORK_NOTIFY_ERROR,
	NETWORK_NOTIFY_CLOSED,
	NETWORK_NOTIFY_ACCENT,
	NETWORK_NOTIFY_CONNECT,
	NETWORK_NOTIFY_OPENED,
	NETWORK_NOTIFY_EXITED,
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


