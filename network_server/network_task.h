
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

#include "epoll_socket.h"
#include "stream_decoder.h"

#include "network_oper_define.h"

class CNetworkTask
{
public:
	CNetworkTask();
	~CNetworkTask();

private:
	std::atomic_bool m_bRunFlag;
	std::mutex m_queue_mutex_request;
	std::queue< std::shared_ptr<struct tagEventRequest> > m_queueRequest;
	std::thread m_workThread;

	int m_port;
	std::string m_strIP;
	int m_epfd;
	int m_listenfd;
	struct epoll_event m_events[MAX_SOCKET_CONNECT];
	std::map<int, std::shared_ptr<struct sockaddr_in> > m_peerfd;

	int  m_rlength;
	char m_rbuffer[MAX_PACKRT_BUFFER];

private:
	static void runThreadFunction(CNetworkTask *pTask);
	void AddEventRequest(std::shared_ptr<struct tagEventRequest> sptrRequest);
	std::shared_ptr<struct tagEventRequest> GetEventRequest();
	bool SocketListen();
	int OnDisposeEvents();
	

	int HangupNotify(int fd);
	int AcceptNotify(int fd);
	int InputNotify(int fd);

public:
	bool Init();
	bool Start(std::string ip, int port);
	bool ShutDown();
	std::shared_ptr<struct tagEventRequest> GetAsyncRequest();
	int GetListenFd(){ return m_listenfd; }
	int GetPort() { return m_port; }
	std::string GetIP() { return m_strIP; }


};


#endif


