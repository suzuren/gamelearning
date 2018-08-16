
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
	std::mutex m_queue_mutex_response;
	std::queue< std::shared_ptr<struct tagEventResponse> > m_queueResponse;
	std::thread m_workThread;

	int m_epfd;
	int m_listenfd;
	struct epoll_event m_events[MAX_SOCKET_CONNECT];
	std::map<int, std::shared_ptr<struct sockaddr_in> > m_peerfd;

	int  m_rlength;
	char m_rbuffer[MAX_PACKRT_BUFFER];
private:
	void AddEventRequest(std::shared_ptr<struct tagEventRequest> sptrRequest);
	void AddEventResponse(std::shared_ptr<struct tagEventResponse> sptrResponse);

private:
	static void runThreadFunction(CNetworkTask *pTask);
	int OnDisposeEvents();

	int AcceptNotify(int fd);
	int InputNotify(int fd);

public:
	bool Init();
	bool Start(std::string ip, int port);
	bool ShutDown();
	std::shared_ptr<struct tagEventRequest> MallocEventRequest(int eventid, int callback);
	void AsyncExecute(std::shared_ptr<struct tagEventRequest>);
	bool OnProcessEvent(std::shared_ptr<struct tagEventRequest> sptrRequest);
	std::shared_ptr<struct tagEventResponse> GetAsyncExecuteResult();
};






#endif


