#ifndef ___DECODE_AIDE_H___
#define ___DECODE_AIDE_H___

//#include "protocol/network_packet.h"

int GetAideHeaderSize(int type, const unsigned char* data, int len)
{
	int iHeaderSize = 0;
	if (type == DECODE_TYPE_CLIENT)
	{
		iHeaderSize = CLIENT_PACKET_HEADER_SIZE;
	}
	else if (type == DECODE_TYPE_SERVER)
	{
		iHeaderSize = SERVER_PACKET_HEADER_SIZE;
	}
	else if (type == DECODE_TYPE_HTTP)
	{
		// http 包头长度不定，根据包头里面的 "Content-Length: " 项获取 body 大小，每个 http 数据包都会有 "\r\n\r\n" ，接下来就是他的 body 数据
		iHeaderSize = len;
	}
	else
	{
		// error
		iHeaderSize = -1;
	}
	return iHeaderSize;
}

int GetAidePacketSize(int type,const unsigned char* data, int len)
{
	int iPacketSize = 0;
	if (data != NULL && len > 0)
	{
		if (type == DECODE_TYPE_CLIENT)
		{
			client_header* pClientHeader = (client_header*)(data);
			int iClientPacketSize = pClientHeader->len + CLIENT_PACKET_HEADER_SIZE;
			if (iClientPacketSize >0 && iClientPacketSize < CLIENT_PACKET_MAX_SIZE)
			{
				if (len >= iClientPacketSize)
				{
					iPacketSize = iClientPacketSize;
				}
				else
				{
					// 没接收完整包，需要继续接收
					iPacketSize = 0;
				}				
			}
			else
			{
				// 包头设置错误,或者发送数据包过大
				iPacketSize = -2;
			}
		}
		else if (type == DECODE_TYPE_SERVER)
		{
			server_header* pServerHeader = (server_header*)(data);
			int iServerPacketSize = pServerHeader->len + SERVER_PACKET_HEADER_SIZE;
			if (iServerPacketSize > 0 && iServerPacketSize < SERVER_PACKET_MAX_SIZE)
			{
				if (len >= iServerPacketSize)
				{
					iPacketSize = iServerPacketSize;
				}
				else
				{
					// 没接收完整包，需要继续接收
					iPacketSize = 0;
				}
			}
			else
			{
				// 包头设置错误,或者发送数据包过大
				iPacketSize = -2;
			}
		}
		else if (type == DECODE_TYPE_HTTP)
		{
			// http 包头长度不定，根据包头里面的"Content-Length: "项获取 body 大小，每个http数据包都会有"\r\n\r\n"，接下来就是他的body数据
			iPacketSize = len;
		}
		else
		{
			// error
			iPacketSize = -3;
		}
	}
	else
	{
		// error
		iPacketSize = -4;
	}
	return iPacketSize;
}


int AideParsePacketData(int type, const unsigned char* data, int len)
{
	int iHeaderSize = 0;
	int iPacketSize = 0;
	int iDataSize = 0;
	iHeaderSize = GetAideHeaderSize(type, data, len);
	if (iHeaderSize == 0)
	{
		// 类型错误
		iDataSize = 0;
	}
	else if(iHeaderSize < 0)
	{
		// 类型错误
		iDataSize = iHeaderSize;
	}
	else if (iHeaderSize>len)
	{
		// 头部不足
		iDataSize = 0;
	}
	else
	{
		iPacketSize = GetAidePacketSize(type, data, len);
		if (iPacketSize == 0)
		{
			// 包数据不足
			iDataSize = 0;
		}
		else if (iPacketSize < 0)
		{
			// 包数据错误
			iDataSize = iPacketSize;
		}
		else
		{
			// 完整数据包大小
			iDataSize = iPacketSize;
		}
	}
	return iDataSize;
}


#endif // ___DECODE_AIDE_H___

