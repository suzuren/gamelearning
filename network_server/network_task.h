
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
#include "pack_proto_define.h"

#include "network_oper_define.h"


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

	std::atomic_bool m_bRunFlag;
	std::mutex m_queue_mutex_request;
	std::queue< std::shared_ptr<struct tagEventRequest> > m_queueRequest;

	std::mutex m_queue_mutex_send;
	std::queue< std::shared_ptr<struct tagTaskSendData> > m_queueSend;

	int m_port;
	std::string m_strIP;

	int m_epfd;
	int m_listenfd;
	struct epoll_event m_events[MAX_SOCKET_CONNECT];
	std::map<int, std::shared_ptr<struct sockaddr_in> > m_peerfd;

	int  m_rlength;
	char m_rbuffer[MAX_PACKRT_BUFFER];

	int m_sfd;
	int m_slength;
	int m_alength;
	char m_sbuffer[MAX_PACKRT_BUFFER];

private:
	static void runThreadFunction(CNetworkTask *pTask);

	void AddEventRequest(std::shared_ptr<struct tagEventRequest> sptrRequest);
	std::shared_ptr<struct tagEventRequest> GetEventRequest();
	bool SocketListen();
	int OnDisposeEvents();
	int OnSendQueueData();
	int SendBuffer();


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

	int GetListenFd(){ return m_listenfd; }
	int GetPort() { return m_port; }
	std::string GetIP() { return m_strIP; }


};


#endif


