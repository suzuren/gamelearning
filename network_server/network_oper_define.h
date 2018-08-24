
#ifndef __NETWORK_OPER_DEFINE_H_
#define __NETWORK_OPER_DEFINE_H_

#include <string>
#include <memory.h>
#include <netinet/in.h>

#include "stream_decoder.h"
#include "pack_proto_define.h"


// ---------------------------------------------------------------------------------------


#define CHECK_CONNECT_TIME (20*1000)

// ---------------------------------------------------------------------------------------

#pragma  pack(1)

struct packet_header
{
	unsigned int    identity;
	int				command;
	int				length;
	packet_header()
	{
		identity = 0;
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


