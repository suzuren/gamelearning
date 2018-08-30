
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

	struct socket_server * m_socketserver;

	std::shared_ptr<std::thread> m_sptrWorkThread;

	std::atomic_bool m_bRunFlag;
	std::mutex m_queue_mutex_request;
	std::queue< std::shared_ptr<struct tagEventRequest> > m_queueRequest;

	int m_port;
	std::string m_strIP;
	int m_listenid;
	uintptr_t m_hlisten;
	uintptr_t m_hstart;

private:
	static void runThreadFunction(CNetworkTask *pTask);

	void AddEventRequest(std::shared_ptr<struct tagEventRequest> sptrRequest);
	std::shared_ptr<struct tagEventRequest> GetEventRequest();
	bool SocketListen();
	int OnDisposeEvents();


	int HangupNotify(int fd);
	int AcceptNotify(int fd);
	int InputNotify(int fd);
	int OutputNotify(int fd);

public:
	bool Init();
	bool Start(std::string ip, int port);
	bool ShutDown();
	std::shared_ptr<struct tagEventRequest> GetAsyncRequest();
	bool SendData(std::shared_ptr<struct tagTaskSendData> sptrData);

	int GetPort() { return m_port; }
	std::string GetIP() { return m_strIP; }


};


#endif


