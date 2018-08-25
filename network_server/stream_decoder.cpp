

#include "stream_decoder.h"

//#include "pack_proto_define.h"
#include "network_oper_define.h"

int GetPacketHeaderLength()
{
	return PACKET_HEADER_SIZE;
}

int GetPacketDataLength(char * data, int len)
{
	struct packet_header * ptr = (struct packet_header *)data;
	int sz = ptr->length;// +PACKET_HEADER_SIZE;
	if (len >= sz)
	{
		// 接收的长度大于或者等于包的长度，可以解析了
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
		// 没有包头长,继续接收
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
	// 返回这个包的大小
	return pkglen;
}