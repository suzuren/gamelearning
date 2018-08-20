
#ifndef __STREAM_DECODER_
#define __STREAM_DECODER_
#include <memory.h>

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
#define SOCKET_TCP_BUFFER   65535
#define MAX_PACKRT_BUFFER   (65535*5)

#define PACKET_MAX_SIZE             4096*4
#define PACKET_HEADER_SIZE          sizeof(struct packet_header)
#define PACKET_MAX_DATA_SIZE		(PACKET_MAX_SIZE - PACKET_HEADER_SIZE)

struct packet_buffer
{
	packet_header header;
	unsigned char buffer[PACKET_MAX_DATA_SIZE];
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

int GetPacketHeaderLength();

int ParsePacket(char * data, int len);

#endif


