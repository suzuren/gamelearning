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
		// http ��ͷ���Ȳ��������ݰ�ͷ����� "Content-Length: " ���ȡ body ��С��ÿ�� http ���ݰ������� "\r\n\r\n" ���������������� body ����
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
					// û��������������Ҫ��������
					iPacketSize = 0;
				}				
			}
			else
			{
				// ��ͷ���ô���,���߷������ݰ�����
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
					// û��������������Ҫ��������
					iPacketSize = 0;
				}
			}
			else
			{
				// ��ͷ���ô���,���߷������ݰ�����
				iPacketSize = -2;
			}
		}
		else if (type == DECODE_TYPE_HTTP)
		{
			// http ��ͷ���Ȳ��������ݰ�ͷ�����"Content-Length: "���ȡ body ��С��ÿ��http���ݰ�������"\r\n\r\n"����������������body����
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
		// ���ʹ���
		iDataSize = 0;
	}
	else if(iHeaderSize < 0)
	{
		// ���ʹ���
		iDataSize = iHeaderSize;
	}
	else if (iHeaderSize>len)
	{
		// ͷ������
		iDataSize = 0;
	}
	else
	{
		iPacketSize = GetAidePacketSize(type, data, len);
		if (iPacketSize == 0)
		{
			// �����ݲ���
			iDataSize = 0;
		}
		else if (iPacketSize < 0)
		{
			// �����ݴ���
			iDataSize = iPacketSize;
		}
		else
		{
			// �������ݰ���С
			iDataSize = iPacketSize;
		}
	}
	return iDataSize;
}


#endif // ___DECODE_AIDE_H___

