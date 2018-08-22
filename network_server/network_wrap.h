
#ifndef __NETWORK_WRAP_H_
#define __NETWORK_WRAP_H_


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

class CNetworkWrap
{
public:
	CNetworkWrap();
	~CNetworkWrap();

private:
	std::thread m_workThread;
	std::atomic_bool m_bRunFlag;
	std::mutex m_queue_mutex_request;
	std::queue< std::shared_ptr<struct tagEventRequest> > m_queueRequest;

	std::mutex m_queue_mutex_send;
	std::queue< std::shared_ptr<struct tagEventRequest> > m_queueSend;

	

	int m_port;
	std::string m_strIP;

	int m_epfd;
	int m_clientfd;
	struct epoll_event m_events[MAX_SOCKET_CONNECT];
	std::map<int, std::shared_ptr<struct sockaddr_in> > m_peerfd;

	int  m_rlength;
	char m_rbuffer[MAX_PACKRT_BUFFER];

	int m_slength;
	char m_sbuffer[MAX_PACKRT_BUFFER];


private:
	static void runThreadFunction(CNetworkWrap *pTask);
	void AddEventRequest(std::shared_ptr<struct tagEventRequest> sptrRequest);
	std::shared_ptr<struct tagEventRequest> GetEventRequest();
	bool SocketConnect();
	int OnDisposeEvents();
	

	int HangupNotify(int fd);
	int InputNotify(int fd);
	int OutputNotify(int fd);


public:
	bool Init();
	bool Start(std::string ip, int port);
	bool ShutDown();
	std::shared_ptr<struct tagEventRequest> GetAsyncRequest();
	int GetClientFd();
	bool SendData(std::shared_ptr<struct packet_buffer> sptrData);

};


#endif


