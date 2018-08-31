
#ifndef __NETWORK_TASK_H_
#define __NETWORK_TASK_H_


#include <mutex>
#include <thread>
#include <string>
#include <queue>
#include <memory.h>
#include <functional>
#include <atomic>
#include <iostream>
#include <map>
#include<utility>

#include "network_oper_define.h"
#include "./lib_socketserver/socket_server.hpp"

struct tagTaskSendData
{
	int fd;
	int borad;
	int	length;
	char buffer[PACKET_MAX_SIZE];
	tagTaskSendData()
	{
		init();
	}
	void init()
	{
		fd = -1;
		borad = 0;
		length = 0;
		memset(buffer, 0, sizeof(buffer));
	}
};

class CNetworkTask
{
public:
	CNetworkTask();
	~CNetworkTask();

private:
	std::shared_ptr<std::thread> m_sptrWorkThread;
	std::mutex m_queue_mutex_request;
	std::queue< std::shared_ptr<struct tagEventRequest> > m_queueRequest;

	struct socket_server * m_socketserver;
	int m_port;
	std::string m_strIP;
	int m_listenid;
	uintptr_t m_hlisten;
	uintptr_t m_hstart;

	int  m_rlength;
	char m_rbuffer[MAX_RECV_BUFFER_SIZE];

private:
	bool SocketListen();
	static void * runThreadFunction(CNetworkTask *pTask);

	int NotifyExit(struct socket_message & result);
	int NotifyData(struct socket_message & result);
	int NotifyClose(struct socket_message & result);
	int NotifyOpen(struct socket_message & result);
	int NotifyError(struct socket_message & result);
	int NotifyAccept(struct socket_message & result);

	// ----------------------------------------------------------------------------------------------------------
	void AddEventRequest(std::shared_ptr<struct tagEventRequest> sptrRequest);
	std::shared_ptr<struct tagEventRequest> GetEventRequest();

	// ----------------------------------------------------------------------------------------------------------
public:
	int GetPort() { return m_port; }
	std::string GetIP() { return m_strIP; }

public:
	bool Init();
	bool Start(std::string ip, int port);
	bool ShutDown();
	bool SendData(std::shared_ptr<struct tagTaskSendData> sptrData);
	std::shared_ptr<struct tagEventRequest> GetAsyncRequest();
};


#endif


