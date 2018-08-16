

#include "stream_decoder.h"


int GetPacketHeaderLength()
{
	return PACKET_HEADER_SIZE;
}

int GetPacketDataLength(char * data, int len)
{
	struct packet_header * ptr = (struct packet_header *)data;
	int sz = ptr->length + PACKET_HEADER_SIZE;
	if (len >= sz)
	{
		return sz;
	}
	return 0;
}

int ParsePacket(char * data, int len)
{
	if (data == nullptr || len <= 0)
	{
		return -1;
	}
	if (len < GetPacketHeaderLength())
	{
		return 0;
	}
	int pkglen = GetPacketDataLength(data, len);
	if (pkglen <= 0)
	{
		return 0;
	}
	if (pkglen > SOCKET_TCP_BUFFER)
	{
		return -1;
	}
	return pkglen;
}