

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
		// ���յĳ��ȴ��ڻ��ߵ��ڰ��ĳ��ȣ����Խ�����
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
		// û�а�ͷ��,��������
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
	// ����������Ĵ�С
	return pkglen;
}