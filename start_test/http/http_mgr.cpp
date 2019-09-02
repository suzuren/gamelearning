

#include "http_mgr.h"
#include "support_algorithm.h"

#include <stdio.h>
#include <assert.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <pthread.h>

#include "framework/logger.h"
// ---------------------------------------------------------------------

static char * socket_hosttoip(const char * phost)
{
	static char buffer[32];
	memset(buffer, 0, sizeof(buffer));
	if (phost == NULL)
	{
		return NULL;
	}
	struct hostent *phostent = gethostbyname(phost);
	if (phostent == NULL)
	{
		return NULL;
	}
	switch (phostent->h_addrtype)
	{
	case AF_INET:
	case AF_INET6:
	{
		const char * ret_inet = inet_ntop(phostent->h_addrtype, phostent->h_addr, buffer, sizeof(buffer));
		if (ret_inet != NULL)
		{
			return buffer;
		}
	}break;
	default:
	{
		//
	}
	break;
	}
	return NULL;
}

char * http_build_post_data(int * datelen,const char * api, const char * host, const char * body)
{
	char buffer[1024] = { 0 };
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, "POST "); strcat(buffer, api); strcat(buffer, " HTTP/1.1\r\n");
	strcat(buffer, "Host: "); strcat(buffer, host); strcat(buffer, "\r\n");
	strcat(buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
	strcat(buffer, "Content-Length: ");	strcat(buffer, itoa_parser(strlen(body), 10));	strcat(buffer, "\r\n");
	strcat(buffer, "Connection: Keep-Alive\r\n\r\n");
	strcat(buffer, body);
	int size = (int)strlen(buffer);
	char * pdata = (char *)network_malloc(size);
	memcpy(pdata, buffer, size);
	*datelen = size;
	return pdata;
}


// ---------------------------------------------------------------------

bool CHttpMgr::GetDataByHID(int id, tagPostData * pPostData)
{
	bool ret = false;

	auto iter = m_mpPostData.find(id);
	if (iter != m_mpPostData.end())
	{
		ret = true;
		pPostData = iter->second;
	}

	return ret;
}

void	CHttpMgr::RemoveDataByHID(int id)
{
	auto iter = m_mpPostData.find(id);
	if (iter != m_mpPostData.end())
	{
		tagPostData * pPostData = iter->second;
		m_mpPostData.erase(iter);
		if (pPostData != NULL)
		{
			pPostData->Init();
			if (m_QueueDataFree.size() < MAX_HTTP_DATA_FREE)
			{
				m_QueueDataFree.emplace(pPostData);
			}
			else
			{
				delete pPostData;
				pPostData = NULL;
			}
		}
	}
}


// ----------------------------------------------------------------------------------------

int CHandleHttpMsg::OnOpenListen(Aide_ListenInfo* pListenInfo)
{
	LOG_DEBUG("open");
	return 0;
}
int CHandleHttpMsg::OnOpenClient(Aide_AcceptInfo* pAcceptInfo)
{
	LOG_DEBUG("open");
	return 0;
}
int CHandleHttpMsg::OnOpenConnect(Aide_ConnectInfo* pConnectInfo)
{
	LOG_DEBUG("open");
	CHttpMgr::Instance().HttpPostData(pConnectInfo->iSocketID);

	return 0;
}

int CHandleHttpMsg::OnExceptionListen(Aide_ListenInfo* pListenInfo)
{
	LOG_DEBUG("exeception");
	return 0;
}
int CHandleHttpMsg::OnExceptionClient(Aide_AcceptInfo* pAcceptInfo)
{
	LOG_DEBUG("exeception");
	return 0;
}
int CHandleHttpMsg::OnExceptionConnect(Aide_ConnectInfo* pConnectInfo)
{
	LOG_DEBUG("exeception");
	CHttpMgr::Instance().ParseExceptionState(pConnectInfo->iSocketID);
	return 0;
}

int CHandleHttpMsg::OnRecvMessage(int iSocketID, unsigned short uHeadSize, const unsigned char* pHeadData, unsigned short uPackSize, const unsigned char* pPackData)
{
	LOG_DEBUG("recv");

	CHttpMgr::Instance().ParsePacketData(iSocketID, uHeadSize, (unsigned char*)pHeadData);

	return 0;
}

// ----------------------------------------------------------------------------------------

bool CHttpMgr::HttpInit()
{
	CSocketAideMgr::Instance().AideRegisterHandle(HANDLE_MSG_TYPE_HTTP, new CHandleHttpMsg());

	for (int idx = 0; idx < MAX_HTTP_DATA_FREE; idx++)
	{
		tagPostData* pPostData = new tagPostData();
		if (pPostData != NULL)
		{
			pPostData->Init();
			m_QueueDataFree.emplace(pPostData);
		}
	}
	return true;
}

int	CHttpMgr::HttpSetData(tagPostData & paramData)
{
	int port = 80;
	char* ip = socket_hosttoip("service.winic.org");
	if (ip == NULL)
	{
		return -2;
	}
	int id = CSocketAideMgr::Instance().AideConnect(HANDLE_MSG_TYPE_HTTP, DECODE_TYPE_HTTP, ip, port);
	
	if (id > 0)
	{
		tagPostData* pPostData = NULL;
		while (pPostData == NULL && m_QueueDataFree.empty() == false)
		{
			pPostData = m_QueueDataFree.front();
			m_QueueDataFree.pop();
		}
		if (pPostData == NULL)
		{
			pPostData = new tagPostData();
		}
		if (pPostData != NULL)
		{
			pPostData->id = id;
			pPostData->type = paramData.type;
			pPostData->accounts = paramData.accounts;
			pPostData->code = paramData.code;
			m_mpPostData.insert_or_assign(pPostData->id, pPostData);
			//auto iter = m_mpPostData.find(pPostData->id);
			//if (iter != m_mpPostData.end())
			//{
			//	iter->second = pPostData;
			//}
			//else
			//{
			//	//m_mpPostData.insert(std::make_pair(pPostData->id, pPostData));
			//	m_mpPostData.emplace(std::make_pair(pPostData->id, pPostData));
			//}
		}
	}
	return id;
}
// http://service.winic.org:8009/sys_port/gateway/index.asp

// http://service.winic.org/sys_port/gateway/index.asp


void    CHttpMgr::OnHttpTick()
{
	
}


void    CHttpMgr::HttpShutDown()
{	
	while (m_QueueDataFree.empty() == false)
	{
		tagPostData * pPostData = m_QueueDataFree.front();
		m_QueueDataFree.pop();
		if (pPostData != NULL)
		{
			delete pPostData;
			pPostData = NULL;
		}
	}
}

bool CHttpMgr::ParseExceptionState(int id)
{
	bool ret = false;

	ret = true;

	//tagPostData* pPostData = NULL;
	//auto iter = m_mpPostData.find(id);
	//if (iter != m_mpPostData.end())
	//{
	//	pPostData = iter->second;
	//	m_mpPostData.erase(iter);
	//	pPostData->state = 0;
	//	ret = true;
	//}

	//if (ret == true && pPostData != NULL)
	//{
	//	// 缓存对象
	//	pPostData->Init();
	//	if (m_QueueDataFree.size() < MAX_HTTP_DATA_FREE)
	//	{
	//		m_QueueDataFree.emplace(pPostData);
	//	}
	//	else
	//	{
	//		delete pPostData;
	//		pPostData = NULL;
	//	}
	//}

	if (ret == false)
	{
		LOG_ERROR("exce_state - id:%d",id);
	}

	return ret;
}

bool CHttpMgr::ParsePacketData(int id, unsigned short size, unsigned char* data)
{
	bool ret = false;

	// 把上次接收的包没读完的取出
	auto iter = m_mpPostData.find(id);
	if (iter != m_mpPostData.end())
	{
		ret = true;
		tagPostData * pPostData = iter->second;
		if (pPostData != NULL && pPostData->rszie + size < MAX_READ_HTTP_DATA)
		{
			ret = true;
			memcpy(pPostData->rbuffer + pPostData->rszie, data, size);
			pPostData->rszie += size;
			bool bParseSuccess = ParseBodyData(pPostData);
			if (bParseSuccess)
			{
				LOG_DEBUG("ParseBodyData - hid:%d,rszie:%d,\nrbuffer:-%s-,\nbodylen:%d,\nbody:-%s-\n", id, pPostData->rszie, pPostData->rbuffer, pPostData->bodylen, pPostData->body);
			}
		}
	}

	return ret;
}

int	CHttpMgr::HttpPostData(int fd)
{
	tagPostData* pPostData = NULL;
	auto iter = m_mpPostData.find(fd);
	if (iter != m_mpPostData.end())
	{
		pPostData = iter->second;
		pPostData->state = 1;
	}
	if (pPostData == NULL)
	{
		LOG_ERROR("connect_error - fd:%d", fd);
		return 0;
	}
	const char* api = "/sys_port/gateway/index.asp";
	const char* host = "service.winic.org";
	char body[1024] = { 0 };
	const char* id = "taranazi";
	const char* pwd = "GAN123pengjin";
	const char* to = "13380353070";
	//const char *content = "hello,your verify code is [ 333333 ]  - XX GAME!";
	char content[512] = { 0 };
	sprintf(content, "hello,your verify code is [ %s ]  - XX GAME!", pPostData->code.data());
	sprintf(body, "id=%s&pwd=%s&to=%s&content=%s&time=", id, pwd, pPostData->accounts.data(), content);

	int datalen;
	char* pdata = http_build_post_data(&datalen, api, host, body);
	LOG_DEBUG("datalen:%d,pdata:\n-%s-\n", datalen, pdata);
	int handle = HANDLE_MSG_TYPE_HTTP;
	CSocketAideMgr::Instance().SendMessage(handle, fd, datalen, pdata);
	//network_socket_send(333, id, pdata, datalen);

	return 1;
}

bool	CHttpMgr::ParseBodyData(tagPostData* pPostData)
{
	bool ret = false;

	char* Content_Length_str = (char*)"Content-Length: ";
	int Content_Length_size = (int)strlen(Content_Length_str);
	int Content_Length_nextval[Content_Length_size];
	kmp_get_nextval(Content_Length_str, Content_Length_nextval);
	int Content_Length_pos = kmp_index(pPostData->rbuffer, Content_Length_str, 0, Content_Length_nextval);

	int Content_Length_end = Content_Length_pos + Content_Length_size;
	if (Content_Length_pos != -1 && (Content_Length_end + 3) < pPostData->rszie)
	{
		char* Content_Length_ptr = pPostData->rbuffer + Content_Length_end;
		char Content_Length_buf[3] = { 0 };
		int Content_Length_index = 0;
		for (int index = 0; index < 3; index++)
		{
			char ch = (*(Content_Length_ptr + index));
			if (isdigit(ch) == 1)
			{
				Content_Length_buf[Content_Length_index++] = ch;
			}
			else
			{
				break;
			}
		}
		int Content_Length = atoi(Content_Length_buf);
		//atoi_parser(&Content_Length, Content_Length_buf, Content_Length_index);
		if (Content_Length > 0 && Content_Length < MAX_BODY_HTTP_DATA)
		{
			char* change_line_str = (char*)"\r\n\r\n";
			int change_line_size = (int)strlen(change_line_str);
			int change_line_nextval[change_line_size];
			kmp_get_nextval(change_line_str, change_line_nextval);
			int change_line_pos = kmp_index(pPostData->rbuffer, change_line_str, 0, change_line_nextval);
			int change_line_end = change_line_pos + change_line_size;
			if (change_line_pos != -1 && (change_line_end + Content_Length) == pPostData->rszie)
			{
				char* change_line_ptr = pPostData->rbuffer + change_line_end;
				memcpy(pPostData->body, change_line_ptr, Content_Length);
				pPostData->bodylen = Content_Length;
				pPostData->success = true;
				ret = true;
			}
		}
	}
	return ret;
}
