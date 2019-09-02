


#ifndef ___SOCKET_AIDE_H___
#define ___SOCKET_AIDE_H___

#include <memory.h>
#include <queue>
#include <unordered_map>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/message.h>

#include "protocol/network_packet.h"

#define HANDLE_MSG_TYPE_GATE			1
#define HANDLE_MSG_TYPE_GAME			2
#define HANDLE_MSG_TYPE_LOGON			3
#define HANDLE_MSG_TYPE_WORLD			4
#define HANDLE_MSG_TYPE_DATABASE		5
#define HANDLE_MSG_TYPE_HTTP			6

#define AIDE_MAX_NET_INFO				128

#ifndef AIDE_PARSE_MSG_FROM_ARRAY
#define AIDE_PARSE_MSG_FROM_ARRAY(msg)								\
	if(pPackData == NULL)											\
	{																\
		LOG_ERROR("unpack fail data")								\
		return 0;													\
	}																\
	if (msg.ParseFromArray(pPackData, uPackSize) == false)			\
	{																\
		LOG_ERROR("unpack fail rest")								\
		return 0;													\
	}
#endif // AIDE_PARSE_MSG_FROM_ARRAY

#define GET_SERVER_TYPE(uSrcServerID) ((unsigned short)(((unsigned int)(uSrcServerID))/100))

typedef struct tagAideListenInfo_t
{
	unsigned int		handle;
	int					detype;			// 解码类型
	int					iSocketID;		// 监听的 SocketID
	int					state;			// 是否已经加入 epoll 管理（ 0 未加入 1 已经加入）
	std::string			strIP;			// 监听的 IP
	unsigned int		uPort;			// 监听的端口
	tagAideListenInfo_t()
	{
		Init();
	}
	void Init()
	{
		handle			= 0;
		detype			= 0;
		iSocketID		= -1;
		state			= 0;
		strIP.clear();
		uPort			= 0;
	}
	void operator=(const struct tagAideListenInfo_t& info)
	{
		handle			= info.handle;
		detype			= info.detype;
		iSocketID		= info.iSocketID;
		state			= info.state;
		strIP			= info.strIP;
		uPort			= info.uPort;
	}
}Aide_ListenInfo;

typedef struct tagAideAcceptInfo_t
{
	unsigned int		handle;
	int					detype;		// 解码类型
	int					iSocketID;	// 接受客户端连接的SocketID
	int					state;		// 是否已经加入 epoll 管理（ 0 未加入 1 已经加入）
	std::string			strNet;		// 客户端的 (IP:端口)形式 如 （113.87.130.231:45626）
	unsigned long long	ltime;		// 最后通信时间
	tagAideAcceptInfo_t()
	{
		Init();
	}
	void Init()
	{
		handle			= 0;
		detype			= 0;
		iSocketID		= -1;
		state			= 0;
		strNet.clear();
		ltime			= 0;
	}
	void operator=(const struct tagAideAcceptInfo_t & info)
	{
		handle			= info.handle;
		detype			= info.detype;
		iSocketID		= info.iSocketID;
		state			= info.state;
		strNet			= info.strNet;
		ltime			= info.ltime;
	}
}Aide_AcceptInfo;


typedef struct tagAideConnectInfo_t
{
	unsigned int		handle;
	int					detype;		// 解码类型
	int					iSocketID;	// 与服务器所连接的 SocketID
	int					state;		// 是否已经加入 epoll 管理（ 0 未加入 1 已经加入）
	std::string			strIP;		// 连接服务器的 IP
	unsigned int		uPort;		// 连接服务器的端口
	unsigned long long	ltime;		// 最后通信时间
	tagAideConnectInfo_t()
	{
		Init();
	}
	void Init()
	{
		handle			= 0;
		detype			= 0;
		iSocketID		= -1;
		state			= 0;
		strIP.clear();
		uPort			= 0;
		ltime			= 0;
	}
	void operator=(const struct tagAideConnectInfo_t & info)
	{
		handle			= info.handle;
		detype			= info.detype;
		iSocketID		= info.iSocketID;
		state			= info.state;
		strIP			= info.strIP;
		uPort			= info.uPort;
		ltime			= info.ltime;
	}
}Aide_ConnectInfo;

typedef struct tagAideNetworkState_t
{
	int					type;						// 数据类型 包括 连接 关闭 错误
	int					id;							// 通过 SocketID 来的数据
	int					source;						// 状态源
	int					ud;							// 网络状态
	char				buffer[AIDE_MAX_NET_INFO];	// 网络信息，不同状态有不同信息
	tagAideNetworkState_t()
	{
		Init();
	}
	void Init()
	{
		type			= -1;
		id				= -1;
		source			= 0;
		ud				= 0;
		memset(buffer, 0, AIDE_MAX_NET_INFO);
	}
	void operator=(const struct tagAideNetworkState_t & info)
	{
		type			= info.type;
		id				= info.id;
		source			= info.source;
		ud				= info.ud;
		memcpy(buffer, info.buffer, AIDE_MAX_NET_INFO);
	}
}Aide_NetworkState;

unsigned long long	socket_aide_time();


typedef struct tagAideNetworkData_t
{
	int					type;			// 数据类型
	int					id;				// 通过 iSocketID 来的数据
	int					source;			// 数据源，对于 m_umAideClients 就是监听的 source，对于 m_umAideConnects 就是 连接的 source
	int					ud;				// 数据大小
	void*				data;			// 数据地址
	tagAideNetworkData_t()
	{
		Init();
	}
	void Init()
	{
		type			= -1;
		id				= -1;
		source			= 0;
		ud				= 0;
		data			= NULL;
	}
	void operator=(const struct tagAideNetworkData_t & info)
	{
		type			= info.type;
		id				= info.id;
		source			= info.source;
		ud				= info.ud;
		data			= info.data;
	}
}Aide_NetworkData;

class IHanldeMessageSink
{
public:
	virtual int OnOpenListen(Aide_ListenInfo* pListenInfo) = 0;
	virtual int OnOpenClient(Aide_AcceptInfo* pAcceptInfo) = 0;
	virtual int OnOpenConnect(Aide_ConnectInfo* pConnectInfo) = 0;
public:
	virtual int OnExceptionListen(Aide_ListenInfo* pListenInfo) = 0;
	virtual int OnExceptionClient(Aide_AcceptInfo* pAcceptInfo) = 0;
	virtual int OnExceptionConnect(Aide_ConnectInfo* pConnectInfo) = 0;
public:
	virtual int OnRecvMessage(int iSocketID, unsigned short uHeadSize, const unsigned char* pHeadData, unsigned short uPackSize, const unsigned char* pPackData) = 0;
};

class CSocketAideMgr
{
private:
	const CSocketAideMgr & operator=(const CSocketAideMgr &);
public:
	static  CSocketAideMgr & Instance()
	{
		static  CSocketAideMgr  s_SingleObject;
		return  s_SingleObject;
	}
public:
	CSocketAideMgr() {}
    ~CSocketAideMgr() {}
	
public:
	// -----------------------

	bool	AideInit();
	void	AideUpdate();
	void	AideShutDown();
	bool 	AideRegisterHandle(int type, IHanldeMessageSink* pSink);

	// -----------------------

	int		AideConnect(int source,int detype, const char* ip, int port);
	int		AideListen(int source, int detype, const char* ip, int port);

public:
	// -----------------------
	// 把收到的状态放入网络状态队列
	void	PushNetworkState(int type, int id, int source, char* data, int ud);
	// 把收到的数据放入网络数据队列
	void	PushNetworkData(int type, int id, int source, char* data, int ud);

protected:
	// -----------------------
	// 解析收到的状态
	bool	ParseNetworkSatet();
	// 解析打开连接
	bool	ParseConnectState(Aide_NetworkState& tempNetData);
	// 解析客户端连接数据
	bool	ParseAcceptState(Aide_NetworkState& tempNetData);
	// 解析异常连接数据
	bool	ParseExceptionState(Aide_NetworkState& tempNetData);
	// -----------------------
	// 解析收到的数据
	bool	ParseNetworkData();
	// 解析数据包
	bool	ParsePacketData(Aide_NetworkData& tempNetData);
	// 解析收到客户端的数据
	bool	ParseClientNetworkData(int id, unsigned char* pDataBuff, int& iDataSize, int& iReadSize);
	// 解析收到服务器的数据
	bool	ParseConnectNetworkData(int id, unsigned char* pDataBuff, int& iDataSize, int& iReadSize);
	// -----------------------

public:
	bool	UpdateMessageHandle(int id, int handle);
	int		SendMessage(int handle,int iSocketID, int size, char* buffer);

	int		SendServerProtoMessage(int iSocketID, const google::protobuf::Message* msg, unsigned short type, unsigned short cmd, unsigned short uin, int iClientID);
private:
	std::unordered_map<int, Aide_ListenInfo  *>					m_umAideListens;			// 监听的信息
	std::unordered_map<int, Aide_AcceptInfo  *>					m_umAideClients;			// 接受的连接
	std::unordered_map<int, Aide_ConnectInfo *>					m_umAideConnects;			// 发起的连接

	pthread_t													m_pSocketAideThreadID;		// Socket助手线程ID

	pthread_mutex_t												m_MutexAideNetworkState;	// 这个锁只用于网络状态包队列
	std::queue<Aide_NetworkState>								m_QueueAideNetworkState;	// 网络状态包队列

	pthread_mutex_t												m_MutexAideNetworkData;		// 这个锁只用于网络数据包队列
	std::queue<Aide_NetworkData>								m_QueueAideNetworkData;		// 网络数据包队列

	unsigned long long											m_LastUpdateTime;			// 最后检查更新时间

	std::queue<Aide_AcceptInfo *>								m_QueueClinetsFree;			// 接受连接空闲对象
	std::queue<Aide_ConnectInfo*>								m_QueueConnectsFree;		// 发起连接空闲对象
	std::queue<incomplete_packet *>								m_QueueIncompleteFree;		// 未完整包空闲结构
	std::unordered_map<int, incomplete_packet *>				m_umIncompletePacket;		// 未接收完全数据包

	std::unordered_map<int, IHanldeMessageSink*>				m_umHandleMessageRecvSink;	// 消息处理类
};

#endif // ___SOCKET_AIDE_H___



