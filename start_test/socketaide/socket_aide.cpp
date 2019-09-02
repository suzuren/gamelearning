

#include <stdio.h>
#include <assert.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "socketserver/socket_server.h"
#include "socketserver/network_malloc.h"
#include "socket_aide.h"
#include "decode_aide.h"

// ---------------------------------------------------------------------

#include "framework/logger.h"

// ---------------------------------------------------------------------

#define AIDE_SOCKET_TYPE_DATA			1
#define AIDE_SOCKET_TYPE_CONNECT		2
#define AIDE_SOCKET_TYPE_CLOSE			3
#define AIDE_SOCKET_TYPE_ACCEPT			4
#define AIDE_SOCKET_TYPE_ERROR			5
#define AIDE_SOCKET_TYPE_UDP			6
#define AIDE_SOCKET_TYPE_WARNING		7

#define AIDE_MAX_CLIENT_FREE			1024
#define AIDE_MAX_CONNECT_FREE			1024
#define AIDE_MAX_INCOMPLETE_PACKET		4096

// ---------------------------------------------------------------------

//unsigned long long	socket_aide_time();
void socket_aide_init();
void socket_aide_exit();
void socket_aide_free();
int  socket_aide_poll();
void socket_aide_updatetime();

int  socket_aide_send(uint32_t handle, int id, void* buffer, int sz);
int  socket_aide_send_lowpriority(uint32_t handle, int id, void* buffer, int sz);
int  socket_aide_listen(uint32_t handle, const char* host, int port, int backlog);
int  socket_aide_connect(uint32_t handle, const char* host, int port);
int  socket_aide_bind(uint32_t handle, int fd);
void socket_aide_close(uint32_t handle, int id);
void socket_aide_shutdown(uint32_t handle, int id);
void socket_aide_start(uint32_t handle, int id);
void socket_aide_nodelay(uint32_t handle, int id);
struct socket_info* socket_aide_info();

// ---------------------------------------------------------------------

unsigned long long	socket_aide_time()
{
	timespec _spec;
	clock_gettime(CLOCK_REALTIME, &_spec);
	unsigned long long uTick = _spec.tv_sec * 1000 + _spec.tv_nsec / 1000 / 1000;

	return uTick;
}

static struct socket_server * AIDE_SOCKET_SERVER = NULL;

void
socket_aide_init() {
	if (AIDE_SOCKET_SERVER == NULL)
	{
		uint64_t time = socket_aide_time();
		AIDE_SOCKET_SERVER = socket_server_create(time);
	}
}

void
socket_aide_exit() {
	socket_server_exit(AIDE_SOCKET_SERVER);
}

void
socket_aide_free() {
	socket_server_release(AIDE_SOCKET_SERVER);
	AIDE_SOCKET_SERVER = NULL;
}

void
socket_aide_updatetime() {
	uint64_t time = socket_aide_time();
	socket_server_updatetime(AIDE_SOCKET_SERVER, time);
}

int
socket_aide_send(uint32_t handle, int id, void *buffer, int sz) {
	return socket_server_send(AIDE_SOCKET_SERVER, id, buffer, sz);
}

int
socket_aide_send_lowpriority(uint32_t handle, int id, void *buffer, int sz) {
	return socket_server_send_lowpriority(AIDE_SOCKET_SERVER, id, buffer, sz);
}

int
socket_aide_listen(uint32_t handle, const char *host, int port, int backlog) {
	uint32_t source = handle;
	return socket_server_listen(AIDE_SOCKET_SERVER, source, host, port, backlog);
}

int
socket_aide_connect(uint32_t handle, const char *host, int port) {
	uint32_t source = handle;
	return socket_server_connect(AIDE_SOCKET_SERVER, source, host, port);
}

int
socket_aide_bind(uint32_t handle, int fd) {
	uint32_t source = handle;
	return socket_server_bind(AIDE_SOCKET_SERVER, source, fd);
}

void
socket_aide_close(uint32_t handle, int id) {
	uint32_t source = handle;
	socket_server_close(AIDE_SOCKET_SERVER, source, id);
}

void
socket_aide_shutdown(uint32_t handle, int id) {
	uint32_t source = handle;
	socket_server_shutdown(AIDE_SOCKET_SERVER, source, id);
}

void
socket_aide_start(uint32_t handle, int id) {
	uint32_t source = handle;
	socket_server_start(AIDE_SOCKET_SERVER, source, id);
}

void
socket_aide_nodelay(uint32_t handle, int id) {
	socket_server_nodelay(AIDE_SOCKET_SERVER, id);
}

struct socket_info *
	socket_aide_info() {
	return socket_server_info(AIDE_SOCKET_SERVER);
}

// ---------------------------------------------------------------------


// mainloop thread
static void
forward_message(int type, bool padding, struct socket_message * result)
{
	if (type == AIDE_SOCKET_TYPE_CONNECT)
	{
		CSocketAideMgr::Instance().PushNetworkState(type, result->id, (int)result->opaque, result->data, result->ud);
		LOG_DEBUG("CONNECT - open(%lu) [id=%d] %s", result->opaque, result->id, result->data);
	}
	else if (type == AIDE_SOCKET_TYPE_DATA)
	{
		CSocketAideMgr::Instance().PushNetworkData(type,result->id, (int)result->opaque, result->data, result->ud);
		LOG_DEBUG("DATA - type:%d,opaque:%ld, [id=%d] ud=%d", type, result->opaque, result->id, result->ud);
	}
	else if(type == AIDE_SOCKET_TYPE_ACCEPT)
	{
		CSocketAideMgr::Instance().PushNetworkState(type, result->id, (int)result->opaque, result->data, result->ud);
		LOG_DEBUG("ACCEPT - accept(%lu) [id=%d %s] from [%d]", result->opaque, result->ud, result->data, result->id);
	}
	else if (type == AIDE_SOCKET_TYPE_CLOSE)
	{
		CSocketAideMgr::Instance().PushNetworkState(type, result->id, (int)result->opaque, result->data, result->ud);
		LOG_DEBUG("CLOSE - close(%lu) [id=%d]", result->opaque, result->id);
	}
	else if (type == AIDE_SOCKET_TYPE_ERROR)
	{
		CSocketAideMgr::Instance().PushNetworkState(type, result->id, (int)result->opaque, result->data, result->ud);
		LOG_DEBUG("ERROR - error(%lu) [id=%d]\n", result->opaque, result->id);
	}
	else
	{
		LOG_DEBUG("else - else(%lu) [id=%d]\n", result->opaque, result->id);
	}
	LOG_DEBUG("all_type - type:%d,opaque:%ld, [id=%d] ud=%d", type, result->opaque, result->id, result->ud);
}

int
socket_aide_poll() {
	struct socket_server *ss = AIDE_SOCKET_SERVER;
	assert(ss);
	struct socket_message result;
	int more = 1;
	int type = socket_server_poll(ss, &result, &more);
	switch (type) {
	case SOCKET_EXIT:
		return 0;
	case SOCKET_DATA:
		forward_message(AIDE_SOCKET_TYPE_DATA, false, &result);
		break;
	case SOCKET_CLOSE:
		forward_message(AIDE_SOCKET_TYPE_CLOSE, false, &result);
		break;
	case SOCKET_OPEN:
		forward_message(AIDE_SOCKET_TYPE_CONNECT, true, &result);
		break;
	case SOCKET_ERR:
		forward_message(AIDE_SOCKET_TYPE_ERROR, true, &result);
		break;
	case SOCKET_ACCEPT:
		forward_message(AIDE_SOCKET_TYPE_ACCEPT, true, &result);
		break;
	case SOCKET_UDP:
		forward_message(AIDE_SOCKET_TYPE_UDP, false, &result);
		break;
	case SOCKET_WARNING:
		forward_message(AIDE_SOCKET_TYPE_WARNING, false, &result);
		break;
	default:
		fprintf(stderr, "Unknown socket message type %d.", type);
		return -1;
	}
	if (more) {
		return -1;
	}
	return 1;
}

// ---------------------------------------------------------------------

void thread_socket_aide_sleep(unsigned int msec)
{
	struct timespec tm;
	tm.tv_sec = msec / 1000;
	tm.tv_nsec = msec % 1000 * 1000000;
	nanosleep(&tm, 0);
}

static void *
thread_socket_aide_func(void *p)
{
	for (;;)
	{
		int r = socket_aide_poll();
		if (r == 0)
		{
			break;
		}
		thread_socket_aide_sleep(30);
		if (r < 0)
		{
			continue;
		}
	}
	socket_aide_free();
	//std::cout << "socket thread aide exit." << std::endl;
	//thread_socket_aide_sleep(30);
	return NULL;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

bool CSocketAideMgr::AideInit()
{
	m_LastUpdateTime = socket_aide_time();

	socket_aide_init();
	pthread_mutex_init(&m_MutexAideNetworkState, NULL);
	pthread_mutex_init(&m_MutexAideNetworkData, NULL);

	// 预先初始化客户端对象
	for (int idx = 0; idx < AIDE_MAX_CLIENT_FREE; idx++)
	{
		Aide_AcceptInfo * pAcceptInfo = new Aide_AcceptInfo();
		if (pAcceptInfo != NULL)
		{
			pAcceptInfo->Init();
			m_QueueClinetsFree.emplace(pAcceptInfo);
		}
	}

	for (int idx = 0; idx < AIDE_MAX_CONNECT_FREE; idx++)
	{
		Aide_ConnectInfo* pConnectInfo = new Aide_ConnectInfo();
		if (pConnectInfo != NULL)
		{
			pConnectInfo->Init();
			m_QueueConnectsFree.emplace(pConnectInfo);
		}
	}

	pthread_create(&m_pSocketAideThreadID, 0, thread_socket_aide_func, NULL);

	return true;
}

void CSocketAideMgr::AideUpdate()
{
	socket_aide_updatetime();
	unsigned long long lCurTime = socket_aide_time();
	// 有网络状态直接解析
	ParseNetworkSatet();
	// 有网络数据包直接解析
	ParseNetworkData();
	// 每隔5秒检测
	//if (m_LastUpdateTime + 5 * 1000 < lCurTime)
	//{
	//	//LOG_DEBUG("m_LastUpdateTime:%llu,lCurTime:%llu", m_LastUpdateTime, lCurTime);
	//	m_LastUpdateTime = lCurTime;
	//	// 检查发起连接状态
	//	CheckAideConnects(lCurTime);
	//}
}

void CSocketAideMgr::AideShutDown()
{
	//在主线程请求退出
	socket_aide_exit();
	//在线程退出线程循环的时候释放
	//socket_aide_free();
	pthread_mutex_destroy(&m_MutexAideNetworkState);
	pthread_mutex_destroy(&m_MutexAideNetworkData);
	pthread_join(m_pSocketAideThreadID, NULL);
	pthread_cancel(m_pSocketAideThreadID);
	//std::cout << "AideShutDown." << std::endl;
}

bool CSocketAideMgr::AideRegisterHandle(int type, IHanldeMessageSink* pSink)
{
	bool bRegister = false;

	if (type >= 0 && pSink != NULL)
	{
		auto iter = m_umHandleMessageRecvSink.find(type);
		if (iter == m_umHandleMessageRecvSink.end())
		{
			m_umHandleMessageRecvSink.emplace(type, pSink);
			bRegister = true;
		}
	}

	return bRegister;
}

int	 CSocketAideMgr::AideConnect(int source,int detype, const char* ip, int port)
{
	int cid = socket_aide_connect(source, ip, port);
	if (cid != -1)
	{
		Aide_ConnectInfo* pConnectInfo = NULL;
		while (pConnectInfo == NULL && m_QueueConnectsFree.empty() == false)
		{
			pConnectInfo = m_QueueConnectsFree.front();
			m_QueueConnectsFree.pop();
		}
		if (pConnectInfo == NULL)
		{
			pConnectInfo = new Aide_ConnectInfo();
		}

		if (pConnectInfo != NULL)
		{
			pConnectInfo->handle = source;
			pConnectInfo->detype = detype;
			pConnectInfo->iSocketID = cid;
			pConnectInfo->state = 0;
			pConnectInfo->strIP = ip;
			pConnectInfo->uPort = port;
			// 因为是在同一个主线程解析Socket连接状态，所以在这里直接插入，不用担心解析连接的时候找不到这个cid
			auto iter = m_umAideListens.find(cid);
			if (iter == m_umAideListens.end())
			{
				m_umAideConnects.emplace(cid, pConnectInfo);
			}
			else
			{
				cid = -2;
			}
		}
		else
		{
			cid = -3;
		}
	}

	return cid;
}

int	 CSocketAideMgr::AideListen(int source, int detype, const char* ip, int port)
{
	int backlog = 128;
	int lid = socket_aide_listen(source, ip, port, backlog);
	if (lid != -1)
	{
		Aide_ListenInfo* pListenInfo = new Aide_ListenInfo();
		if (pListenInfo != NULL)
		{
			pListenInfo->handle = source;
			pListenInfo->detype = detype;
			pListenInfo->iSocketID = lid;
			pListenInfo->state = 0;
			pListenInfo->strIP = ip;
			pListenInfo->uPort = port;

			auto iter = m_umAideListens.find(lid);
			if (iter == m_umAideListens.end())
			{
				m_umAideListens.emplace(lid, pListenInfo);
				socket_aide_start(source, lid);
			}
			else
			{
				lid = -2;
			}
		}
		else
		{
			lid = -3;
		}		
	}

	return lid;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void CSocketAideMgr::PushNetworkState(int type, int id, int source, char* data, int ud)
{
	pthread_mutex_lock(&m_MutexAideNetworkState);

	Aide_NetworkState tempNetState;

	tempNetState.type = type;
	tempNetState.id = id;
	tempNetState.source = source;
	tempNetState.ud = ud;
	if (data != NULL)
	{
		unsigned int iDataSize = strlen(data);
		if (iDataSize > AIDE_MAX_NET_INFO)
		{
			LOG_ERROR("size_error - type:%d,id:%d,source:%d,ud:%d,iDataSize:%d", type, id, source, ud, iDataSize);
			iDataSize = AIDE_MAX_NET_INFO;
		}
		memcpy(tempNetState.buffer, data, iDataSize);
	}
	else
	{
		LOG_ERROR("state_error - type:%d,id:%d,source:%d,ud:%d", type, id, source, ud);
	}
	m_QueueAideNetworkState.emplace(tempNetState);

	pthread_mutex_unlock(&m_MutexAideNetworkState);
}

void CSocketAideMgr::PushNetworkData(int type,int id, int source, char* data, int ud)
{
	pthread_mutex_lock(&m_MutexAideNetworkData);

	Aide_NetworkData tempNetData;

	tempNetData.type = type;
	tempNetData.id = id;
	tempNetData.source = source;
	tempNetData.ud = ud;
	tempNetData.data = data;

	m_QueueAideNetworkData.emplace(tempNetData);

	pthread_mutex_unlock(&m_MutexAideNetworkData);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------


bool CSocketAideMgr::ParseNetworkSatet()
{
	bool ret = false;

	if (m_QueueAideNetworkState.empty() == false) // 这里不加锁了
	{
		Aide_NetworkState tempNetState;
		pthread_mutex_lock(&m_MutexAideNetworkState);
		tempNetState = m_QueueAideNetworkState.front();
		m_QueueAideNetworkState.pop();
		pthread_mutex_unlock(&m_MutexAideNetworkState);

		if (tempNetState.type == AIDE_SOCKET_TYPE_CONNECT)
		{
			ret = ParseConnectState(tempNetState);
		}
		else if (tempNetState.type == AIDE_SOCKET_TYPE_ACCEPT)
		{
			ret = ParseAcceptState(tempNetState);
		}
		else if (tempNetState.type == AIDE_SOCKET_TYPE_CLOSE || tempNetState.type == AIDE_SOCKET_TYPE_ERROR)
		{
			ret = ParseExceptionState(tempNetState);
		}
		else
		{
			// error
			LOG_ERROR("size_error - type:%d,id:%d,source:%d,ud:%d", tempNetState.type, tempNetState.id, tempNetState.source, tempNetState.ud);
		}
	}

	return ret;
}

bool CSocketAideMgr::ParseConnectState(Aide_NetworkState& tempNetState)
{
	bool ret = false;

	if (ret != true)
	{
		auto iter_clients = m_umAideClients.find(tempNetState.id);
		if (iter_clients != m_umAideClients.end())
		{
			Aide_AcceptInfo* pAcceptInfo = iter_clients->second;
			pAcceptInfo->state = 1;
			pAcceptInfo->ltime = socket_aide_time();

			ret = true;

			auto iter_sink = m_umHandleMessageRecvSink.find(pAcceptInfo->handle);
			if (iter_sink != m_umHandleMessageRecvSink.end())
			{
				IHanldeMessageSink* pSink = iter_sink->second;
				if (pSink != NULL)
				{
					pSink->OnOpenClient(pAcceptInfo);
				}
			}
		}
	}
	if (ret != true)
	{
		auto iter_connects = m_umAideConnects.find(tempNetState.id);
		if (iter_connects != m_umAideConnects.end())
		{
			Aide_ConnectInfo* pConnectInfo = iter_connects->second;
			pConnectInfo->state = 1;
			pConnectInfo->ltime = socket_aide_time();

			ret = true;

			auto iter_sink = m_umHandleMessageRecvSink.find(pConnectInfo->handle);
			if (iter_sink != m_umHandleMessageRecvSink.end())
			{
				IHanldeMessageSink* pSink = iter_sink->second;
				if (pSink != NULL)
				{
					pSink->OnOpenConnect(pConnectInfo);
				}
			}
		}
	}
	if (ret != true)
	{
		auto iter_listens = m_umAideListens.find(tempNetState.id);
		if (iter_listens != m_umAideListens.end())
		{
			Aide_ListenInfo* pListenInfo = iter_listens->second;
			pListenInfo->state = 1;
			ret = true;
			auto iter_sink = m_umHandleMessageRecvSink.find(pListenInfo->handle);
			if (iter_sink != m_umHandleMessageRecvSink.end())
			{
				IHanldeMessageSink* pSink = iter_sink->second;
				if (pSink != NULL)
				{
					pSink->OnOpenListen(pListenInfo);
				}
			}
		}
	}
	if (ret == false)
	{
		LOG_ERROR("connect_error - type:%d,id:%d,source:%d,ud:%d,buffrt:%s",
			tempNetState.type, tempNetState.id, tempNetState.source, tempNetState.ud, tempNetState.buffer);
	}

	return ret;
}

bool CSocketAideMgr::ParseAcceptState(Aide_NetworkState& tempNetState)
{
	bool ret = false;

	Aide_ListenInfo* pListenInfo = NULL;
	auto iter_listens = m_umAideListens.find(tempNetState.id);
	if (iter_listens != m_umAideListens.end())
	{
		pListenInfo = iter_listens->second;
	}

	Aide_AcceptInfo* pAcceptInfo = NULL;
	if (pListenInfo != NULL)
	{
		while (pAcceptInfo == NULL && m_QueueClinetsFree.empty() == false)
		{
			pAcceptInfo = m_QueueClinetsFree.front();
			m_QueueClinetsFree.pop();
		}
		if (pAcceptInfo == NULL)
		{
			pAcceptInfo = new Aide_AcceptInfo();
		}
	}

	if (pAcceptInfo != NULL)
	{
		// result->id - id is listen id
		// result->opaque - opaque is listen handle
		// result->ud - ud is new connection id

		pAcceptInfo->Init();
		pAcceptInfo->handle = tempNetState.source;
		pAcceptInfo->detype = pListenInfo->detype;
		pAcceptInfo->iSocketID = tempNetState.ud;
		pAcceptInfo->state = 0;
		unsigned int iDataSize = strlen(tempNetState.buffer);
		if (iDataSize > AIDE_MAX_NET_INFO)
		{
			iDataSize = AIDE_MAX_NET_INFO;
		}
		pAcceptInfo->strNet.append(tempNetState.buffer, iDataSize);

		auto iter = m_umAideClients.find(pAcceptInfo->iSocketID);
		if (iter == m_umAideClients.end())
		{
			m_umAideClients.emplace(pAcceptInfo->iSocketID, pAcceptInfo);
			ret = true;
		}
		else
		{
			m_umAideClients.insert_or_assign(pAcceptInfo->iSocketID, pAcceptInfo);
			ret = true;
		}
		socket_aide_start(pAcceptInfo->handle, pAcceptInfo->iSocketID);
	}
	if(ret == false)
	{
		LOG_ERROR("accept_state - type:%d,id:%d,source:%d,ud:%d,buffer:%s",
			tempNetState.type, tempNetState.id, tempNetState.source, tempNetState.ud, tempNetState.buffer);
	}
	return ret;
}

bool CSocketAideMgr::ParseExceptionState(Aide_NetworkState& tempNetState)
{
	bool ret = false;

	Aide_AcceptInfo* pAcceptInfo = NULL;

	auto iter_clients = m_umAideClients.find(tempNetState.id);
	if (iter_clients != m_umAideClients.end())
	{
		pAcceptInfo = iter_clients->second;
		m_umAideClients.erase(iter_clients);
		pAcceptInfo->state = 0;
		ret = true;
	}

	if (ret == true && pAcceptInfo != NULL)
	{
		// 通知上层逻辑客户端掉线

		auto iter_sink = m_umHandleMessageRecvSink.find(pAcceptInfo->handle);
		if (iter_sink != m_umHandleMessageRecvSink.end())
		{
			IHanldeMessageSink* pSink = iter_sink->second;
			if (pSink != NULL)
			{
				pSink->OnExceptionClient(pAcceptInfo);
			}
		}

		// 缓存客户端对象
		pAcceptInfo->Init();
		if (m_QueueClinetsFree.size() < AIDE_MAX_CLIENT_FREE)
		{
			m_QueueClinetsFree.emplace(pAcceptInfo);
		}
		else
		{
			delete pAcceptInfo;
			pAcceptInfo = NULL;
		}
	}

	// 发起的连接断线进行重连，不要直接移除
	if (ret != true)
	{
		auto iter_connects = m_umAideConnects.find(tempNetState.id);
		if (iter_connects != m_umAideConnects.end())
		{
			Aide_ConnectInfo* pConnectInfo = iter_connects->second;
			pConnectInfo->state = 0;
			ret = true;

			auto iter_sink = m_umHandleMessageRecvSink.find(pConnectInfo->handle);
			if (iter_sink != m_umHandleMessageRecvSink.end())
			{
				IHanldeMessageSink* pSink = iter_sink->second;
				if (pSink != NULL)
				{
					pSink->OnExceptionConnect(pConnectInfo);
				}
			}
			if (pConnectInfo->handle == HANDLE_MSG_TYPE_HTTP)
			{
				pConnectInfo->Init();
				if (m_QueueConnectsFree.size() < AIDE_MAX_CONNECT_FREE)
				{
					m_QueueConnectsFree.emplace(pConnectInfo);
				}
				else
				{
					delete pConnectInfo;
					pConnectInfo = NULL;
				}
			}
		}
	}

	// 监听发生错误
	if (ret != true)
	{
		auto iter_listens = m_umAideListens.find(tempNetState.id);
		if (iter_listens != m_umAideListens.end())
		{
			Aide_ListenInfo* pListenInfo = iter_listens->second;
			pListenInfo->state = 0;
			ret = true;

			auto iter_sink = m_umHandleMessageRecvSink.find(pListenInfo->handle);
			if (iter_sink != m_umHandleMessageRecvSink.end())
			{
				IHanldeMessageSink* pSink = iter_sink->second;
				if (pSink != NULL)
				{
					pSink->OnExceptionListen(pListenInfo);
				}
			}
		}
	}

	if (ret == false)
	{
		LOG_ERROR("exce_state - type:%d,id:%d,source:%d,ud:%d,buffer:%s",
			tempNetState.type, tempNetState.id, tempNetState.source, tempNetState.ud, tempNetState.buffer);
	}

	return ret;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------


bool CSocketAideMgr::ParseNetworkData()
{
	bool ret = false;
	if (m_QueueAideNetworkData.empty() == false) // 这里不加锁了
	{
		Aide_NetworkData tempNetData;
		pthread_mutex_lock(&m_MutexAideNetworkData);
		tempNetData = m_QueueAideNetworkData.front();
		m_QueueAideNetworkData.pop();
		pthread_mutex_unlock(&m_MutexAideNetworkData);

		if(tempNetData.type == AIDE_SOCKET_TYPE_DATA)
		{
			ret = ParsePacketData(tempNetData);
		}
		else
		{
			// error
			LOG_ERROR("data_type_error - type:%d,id:%d,source:%d,ud:%d", tempNetData.type, tempNetData.id, tempNetData.source, tempNetData.ud);
		}
	}
	else
	{
		// queue is empty
	}

	return ret;
}

bool CSocketAideMgr::ParsePacketData(Aide_NetworkData& tempNetData)
{
	bool ret = false;
	incomplete_packet incomPack;
	incomPack.len = 0;
	// 把上次接收的包没读完的取出
	auto iter_Incomplete = m_umIncompletePacket.find(tempNetData.id);
	if (iter_Incomplete != m_umIncompletePacket.end())
	{
		incomplete_packet* pInpack = iter_Incomplete->second;
		incomPack.len += pInpack->len;
		memcpy(incomPack.buf, pInpack->buf, incomPack.len);
		m_umIncompletePacket.erase(iter_Incomplete);
		if (m_QueueIncompleteFree.size() < AIDE_MAX_INCOMPLETE_PACKET)
		{
			m_QueueIncompleteFree.emplace(pInpack);
		}
		else
		{
			if (pInpack != NULL)
			{
				network_free(pInpack);
				pInpack = NULL;
				LOG_ERROR("Incomplete packet is many - type:%d,id:%d,source:%d,ud:%d",tempNetData.type, tempNetData.id, tempNetData.source, tempNetData.ud);
			}
			LOG_ERROR("m_QueueIncompleteFree.size:%d", m_QueueIncompleteFree.size());
		}
	}

	int iDataSize = 0;
	unsigned char* pDataBuff = NULL;
	int iReadSize = 0;
	int iAllDataSize = 0;
	int iMaxParseCount = 32;
	do
	{
		if (iDataSize > 0 && iReadSize > 0 && pDataBuff != NULL)
		{
			// 把上次未读取的数据前移
			memcpy(incomPack.buf, incomPack.buf + iReadSize, iDataSize);
			incomPack.len = iDataSize;
		}
		if (tempNetData.data != NULL && tempNetData.ud > 0)
		{
			if (tempNetData.ud < (INCOMPLETE_PACKET_MAX_SIZE - incomPack.len))
			{
				memcpy(incomPack.buf + incomPack.len, tempNetData.data, tempNetData.ud);
				incomPack.len += tempNetData.ud;
				tempNetData.ud -= tempNetData.ud;
			}
			else
			{
				// 这次解析超过一个最大包，可能里面又多个数据包，分多次解析出来
				memcpy(incomPack.buf + incomPack.len, tempNetData.data, (INCOMPLETE_PACKET_MAX_SIZE - incomPack.len));
				incomPack.len += (INCOMPLETE_PACKET_MAX_SIZE - incomPack.len);
				tempNetData.ud -= (INCOMPLETE_PACKET_MAX_SIZE - incomPack.len);
				LOG_ERROR("many_packet - type:%d,id:%d,source:%d,ud:%d,incomPack.len:%d",
					tempNetData.type, tempNetData.id, tempNetData.source, tempNetData.ud, incomPack.len);
			}
		}
		iDataSize = incomPack.len;
		pDataBuff = incomPack.buf;
		iReadSize = 0;
		iAllDataSize += iDataSize;
		// 客户端的数据解析出来，转发到具体的服务器
		if (ret != true && iDataSize > 0)
		{
			//LOG_DEBUG("net_client_1 - type:%d,fid:%d,source:%d,ud:%d,iAllDataSize:%d,iDataSize:%d,iReadSize:%d,iMaxParseCount:%d,ret:%d",
			//	tempNetData.type, tempNetData.id, tempNetData.source, tempNetData.ud, iAllDataSize, iDataSize, iReadSize, iMaxParseCount, ret);

			ret = ParseClientNetworkData(tempNetData.id, pDataBuff, iDataSize, iReadSize);

			//LOG_DEBUG("net_client_2 - type:%d,fid:%d,source:%d,ud:%d,iAllDataSize:%d,iDataSize:%d,iReadSize:%d,iMaxParseCount:%d,ret:%d",
			//	tempNetData.type, tempNetData.id, tempNetData.source, tempNetData.ud, iAllDataSize, iDataSize, iReadSize, iMaxParseCount, ret);

		}
		// 服务器的数据解析出来，查看具体操作
		if (ret != true && iDataSize > 0)
		{
			//LOG_DEBUG("net_connect_1 - type:%d,fid:%d,source:%d,ud:%d,iAllDataSize:%d,iDataSize:%d,iReadSize:%d,iMaxParseCount:%d,ret:%d",
			//	tempNetData.type, tempNetData.id, tempNetData.source, tempNetData.ud, iAllDataSize, iDataSize, iReadSize, iMaxParseCount, ret);

			ret = ParseConnectNetworkData(tempNetData.id, pDataBuff, iDataSize, iReadSize);

			//LOG_DEBUG("net_connect_2 - type:%d,fid:%d,source:%d,ud:%d,iAllDataSize:%d,iDataSize:%d,iReadSize:%d,iMaxParseCount:%d,ret:%d",
			//	tempNetData.type, tempNetData.id, tempNetData.source, tempNetData.ud, iAllDataSize, iDataSize, iReadSize, iMaxParseCount, ret);

		}
		// 监听的数据--理论上不会出现这一步，这个时候是数据错误造成

		iMaxParseCount--;
		// 解析一遍都没解析出来的超大数据包就直接丢弃
		if (iDataSize >= INCOMPLETE_PACKET_MAX_SIZE && iReadSize == 0)
		{
			// error
			LOG_ERROR("net_data_error_1 - type:%d,fid:%d,source:%d,ud:%d,iAllDataSize:%d,iDataSize:%d,iReadSize:%d,iMaxParseCount:%d,ret:%d",
				tempNetData.type, tempNetData.id, tempNetData.source, tempNetData.ud, iAllDataSize, iDataSize, iReadSize, iMaxParseCount, ret);
			iReadSize = iDataSize;
			iDataSize = 0;
			break;
		}
		// 解析那么多次还没解析完的数据就不要了，这个链接发送的数据有问题
		if (iMaxParseCount<=0)
		{
			LOG_ERROR("net_data_error_2 - type:%d,fid:%d,source:%d,ud:%d,iAllDataSize:%d,iDataSize:%d,iReadSize:%d,iMaxParseCount:%d,ret:%d",
				tempNetData.type, tempNetData.id, tempNetData.source, tempNetData.ud, iAllDataSize, iDataSize, iReadSize, iMaxParseCount, ret);
			iReadSize = iDataSize;
			iDataSize = 0;
			break;
		}
	} while (tempNetData.ud > 0);
	// 释放空间
	network_free(tempNetData.data);

	LOG_DEBUG("net_data_inpac - type:%d,fid:%d,source:%d,ud:%d,iAllDataSize:%d,iDataSize:%d,iReadSize:%d",
		tempNetData.type, tempNetData.id, tempNetData.source, tempNetData.ud, iAllDataSize, iDataSize, iReadSize);

	if (iDataSize > 0 && iDataSize < INCOMPLETE_PACKET_MAX_SIZE)
	{
		// 获取未完成包存储对象
		incomplete_packet* pPacket = NULL;
		if (m_QueueIncompleteFree.empty() == false)
		{
			pPacket = m_QueueIncompleteFree.front();
			m_QueueIncompleteFree.pop();
		}
		if (pPacket == NULL)
		{
			pPacket = (incomplete_packet*)network_malloc(sizeof(incomplete_packet));
		}
		if (pPacket != NULL)
		{
			pPacket->len = iDataSize;
			memcpy(pPacket->buf, pDataBuff + iReadSize, iDataSize);
			m_umIncompletePacket.emplace(tempNetData.id, pPacket);
		}
		else
		{
			LOG_ERROR("pack_error - type:%d,id:%d,source:%d,ud:%d,iDataSize:%d", tempNetData.type, tempNetData.id, tempNetData.source, tempNetData.ud, iDataSize);
		}
	}
	else
	{
		// error
		if (iDataSize != 0)
		{
			// 数据过长 又不能解析
			LOG_ERROR("data_error - type:%d,id:%d,source:%d,ud:%d,iDataSize:%d", tempNetData.type, tempNetData.id, tempNetData.source, tempNetData.ud, iDataSize);
		}
		else
		{
			// iDataSize == 0 证明已经解析完全
		}
	}
	return ret;
}
bool CSocketAideMgr::ParseClientNetworkData(int id, unsigned char* pDataBuff, int& iDataSize, int& iReadSize)
{
	bool ret = false;
	
	Aide_AcceptInfo* pAcceptInfo = NULL;
	auto iter_clients = m_umAideClients.find(id);
	if (iter_clients != m_umAideClients.end())
	{
		pAcceptInfo = iter_clients->second;
		ret = true;
	}

	while (ret == true && pAcceptInfo != NULL && iDataSize >= 0)
	{
		unsigned char* pInData = (pDataBuff + iReadSize);
		int iPacketSize = AideParsePacketData(pAcceptInfo->detype, pInData, iDataSize);
		if (iPacketSize < 0)
		{
			// 包数据错误，把数据丢弃
			iReadSize += iDataSize;
			iDataSize -= iDataSize;
			break;
		}
		if (iPacketSize == 0)
		{
			// 接收数据不够 退出
			break;
		}
		else
		{
			unsigned short uHeadSize = GetAideHeaderSize(pAcceptInfo->detype, pInData, iDataSize);
			const unsigned char* pHeadData = pInData;
			unsigned short uPackSize = iPacketSize - uHeadSize;
			const unsigned char* pPackData = (pInData + uHeadSize);

			auto iter_sink = m_umHandleMessageRecvSink.find(pAcceptInfo->handle);
			if (iter_sink != m_umHandleMessageRecvSink.end())
			{
				IHanldeMessageSink* pSink = iter_sink->second;
				if (pSink != NULL)
				{
					pSink->OnRecvMessage(pAcceptInfo->iSocketID, uHeadSize, pHeadData, uPackSize, pPackData);
				}
			}

			iReadSize += iPacketSize;
			iDataSize -= iPacketSize;
		}
	}
	return ret;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

bool CSocketAideMgr::ParseConnectNetworkData(int id, unsigned char* pDataBuff, int& iDataSize, int& iReadSize)
{
	bool ret = false;
	Aide_ConnectInfo* pConnectInfo = NULL;

	auto iter_connects = m_umAideConnects.find(id);
	if (iter_connects != m_umAideConnects.end())
	{
		pConnectInfo = iter_connects->second;
		ret = true;
	}

	while (ret == true && pConnectInfo != NULL && iDataSize >= 0)
	{
		unsigned char* pInData = (pDataBuff + iReadSize);
		int iPacketSize = AideParsePacketData(pConnectInfo->detype, pInData, iDataSize);
		if (iPacketSize < 0)
		{
			// 包数据错误，把数据丢弃
			iReadSize += iDataSize;
			iDataSize -= iDataSize;
			break;
		}
		if (iPacketSize == 0)
		{
			// 接收数据不够 退出
			break;
		}
		else
		{
			unsigned short uHeadSize = GetAideHeaderSize(pConnectInfo->detype, pInData, iDataSize);
			const unsigned char* pHeadData = pInData;
			unsigned short uPackSize = iPacketSize - uHeadSize;
			const unsigned char* pPackData = (pInData + uHeadSize);

			auto iter_sink = m_umHandleMessageRecvSink.find(pConnectInfo->handle);
			if (iter_sink != m_umHandleMessageRecvSink.end())
			{
				IHanldeMessageSink* pSink = iter_sink->second;
				if (pSink != NULL)
				{
					pSink->OnRecvMessage(pConnectInfo->iSocketID, uHeadSize, pHeadData, uPackSize, pPackData);
				}
			}

			iReadSize += iPacketSize;
			iDataSize -= iPacketSize;
		}
	}
	return ret;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

bool CSocketAideMgr::UpdateMessageHandle(int id, int handle)
{
	bool ret = false;

	if (ret != true)
	{
		auto iter_clients = m_umAideClients.find(id);
		if (iter_clients != m_umAideClients.end())
		{
			Aide_AcceptInfo* pAcceptInfo = iter_clients->second;
			pAcceptInfo->handle = handle;
			ret = true;
		}
	}
	if (ret != true)
	{
		auto iter_connects = m_umAideConnects.find(id);
		if (iter_connects != m_umAideConnects.end())
		{
			Aide_ConnectInfo* pConnectInfo = iter_connects->second;
			pConnectInfo->handle = handle;
			ret = true;
		}
	}
	if (ret != true)
	{
		auto iter_listens = m_umAideListens.find(id);
		if (iter_listens != m_umAideListens.end())
		{
			Aide_ListenInfo* pListenInfo = iter_listens->second;
			pListenInfo->handle = handle;
		}
	}
	if (ret == false)
	{
		LOG_ERROR("update_handle - id:%d,handle:%d", id, handle);
	}

	return ret;
}

int CSocketAideMgr::SendServerProtoMessage(int iSocketID, const google::protobuf::Message* msg, unsigned short type, unsigned short cmd, unsigned short uin,int iClientID)
{
	int iSendRet = 0;

	auto iter_clients = m_umAideClients.find(iSocketID);
	if (iter_clients != m_umAideClients.end())
	{
		Aide_AcceptInfo * pAcceptInfo = iter_clients->second;
		if (pAcceptInfo != NULL)
		{
			if (pAcceptInfo->iSocketID < 0 || pAcceptInfo->state == 0)
			{
				iSendRet = 1;
			}
			else
			{
				// success
			}
		}
		else
		{
			iSendRet = 2;
		}
	}
	else
	{
		auto iter_connects = m_umAideConnects.find(iSocketID);
		if (iter_connects != m_umAideConnects.end())
		{
			Aide_ConnectInfo* pConnectInfo = iter_connects->second;
			if (pConnectInfo != NULL)
			{
				if (pConnectInfo->iSocketID < 0 || pConnectInfo->state == 0)
				{
					iSendRet = 3;
				}
				else
				{
					// success
				}
			}
			else
			{
				iSendRet = 4;
			}
		}
		else
		{
			iSendRet = 5;
		}
	}


	static server_packet pkt;
	if (iSendRet == 0)
	{
		memset(&pkt, 0, sizeof(pkt));
		pkt.header.type = type;
		pkt.header.cmd = cmd;
		pkt.header.uin = uin;
		pkt.header.cid = iClientID;

		msg->SerializeToArray((void*)pkt.protobuf, SERVER_PACKET_MAX_DATA_SIZE - 1);
		pkt.header.len = msg->ByteSize();
	}
	
	unsigned short packet_size = 0;
	void* pbuffer = NULL;
	if (iSendRet == 0)
	{
		packet_size = pkt.header.len + SERVER_PACKET_HEADER_SIZE;
		if (packet_size > SERVER_PACKET_MAX_SIZE)
		{
			iSendRet = 4;
		}
		else
		{
			pbuffer = network_malloc(packet_size);
			if (pbuffer != NULL)
			{
				memcpy(pbuffer, &(pkt.header), packet_size);				
			}
			else
			{
				iSendRet = 5;
			}
		}
	}

	if (iSendRet == 0)
	{
		uint32_t handle = 0;
		iSendRet = socket_aide_send(handle, iSocketID, pbuffer, (int)packet_size);
	}

	return iSendRet;
}

int CSocketAideMgr::SendMessage(int handle, int iSocketID, int size, char* buffer)
{
	int iSendRet = 0;
	iSendRet = socket_aide_send(handle, iSocketID, buffer, size);
	return iSendRet;
}