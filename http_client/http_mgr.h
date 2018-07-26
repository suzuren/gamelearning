
#ifndef __HTTP_MGR_H_
#define __HTTP_MGR_H_

#include <utility>
#include <map>
#include <vector>
#include <string>
#include <queue>
#include <iostream>

#include "http_socket.h"

#define IPADDRESS "47.94.250.154"
#define PORT 38018

enum emSOCKET_STATUS
{
	SOCKET_STATUS_INIT = 0,

	SOCKET_STATUS_CONNECTING,
	SOCKET_STATUS_CONNECT_FAILURE,
	SOCKET_STATUS_CONNECTED,

	SOCKET_STATUS_WRITE,
	SOCKET_STATUS_WRITEING,
	SOCKET_STATUS_WRITEED,

	SOCKET_STATUS_READ,
	SOCKET_STATUS_READING,
	SOCKET_STATUS_READED,
	SOCKET_STATUS_READ_EXCEPTION,

	SOCKET_STATUS_WAIT,
	SOCKET_STATUS_CLOSE,
	SOCKET_STATUS_ERROR,
	SOCKET_STATUS_TIME_OUT,
	SOCKET_STATUS_EXCEPTION
};


//static const int SOCKET_STATUS_INIT = 0;
//static const int SOCKET_STATUS_CONNECTING = 1;
//static const int SOCKET_STATUS_CONNECT_FAILURE = 2;
//static const int SOCKET_STATUS_CONNECTED = 3;
//
//static const int SOCKET_STATUS_WRITE = 1;
//static const int SOCKET_STATUS_WRITEING = 2;
//static const int SOCKET_STATUS_WRITEED = 3;
//
//static const int SOCKET_STATUS_READ = 1;
//static const int SOCKET_STATUS_READING = 2;
//static const int SOCKET_STATUS_READED = 3;
//static const int SOCKET_STATUS_READEXCEPTION = 4;
//
//static const int SOCKET_STATUS_WAIT = 1;
//static const int SOCKET_STATUS_CLOSE = 2;
//static const int SOCKET_STATUS_ERROR = 3;
//static const int SOCKET_STATUS_TIME_OUT = 4;

//#pragma  pack(1)

struct tagsocket_connect
{
	struct epoll_event events[MAX_SOCKET_CONNECT];
	int epfd;
	void reevents()
	{
		memset(events, 0, sizeof(events));
	}
};

struct tagpost_data
{
	int roomid;
	int tableid;
	std::string identify;
	std::string api;
	std::string body;
	std::string content;
	tagpost_data()
	{
		Init();
	}
	void Init()
	{
		roomid = -1;
		tableid = -1;
		identify.clear();
		api.clear();
		body.clear();
		content.clear();
	}
};

struct tagsocket_info
{
	int fd;
	int astatus;
	int cstatus;
	int rstatus;
	int wstatus;
	int rlen;
	int wlen;
	int flen;
	unsigned long long btime;
	struct tagpost_data * ptrpost;
	char rbuffer[SOCKET_TCP_BUFFER];
	char wbuffer[SOCKET_TCP_BUFFER];
	tagsocket_info()
	{
		Init();
	}
	void Init()
	{
		fd = -1;
		astatus = SOCKET_STATUS_INIT;
		cstatus = SOCKET_STATUS_INIT;
		rstatus = SOCKET_STATUS_INIT;
		wstatus = SOCKET_STATUS_INIT;
		rlen = 0;
		wlen = 0;
		flen = 0;
		btime = 0;
		ptrpost = NULL;
		memset(rbuffer, 0, sizeof(rbuffer));
		memset(wbuffer, 0, sizeof(wbuffer));
	}
};


//#pragma pack()

class CHttpMgr
{
protected:
	CHttpMgr() {}
	~CHttpMgr()	{}
private:
	const CHttpMgr & operator=(const CHttpMgr &);
public:
	static  CHttpMgr & Instance()
	{
		static  CHttpMgr  s_SingleObj;
		return  s_SingleObj;
	}
private:
	struct tagsocket_connect m_tagconnect;
	std::map<int,struct tagsocket_info *> m_mpsocket;
	std::vector<struct tagsocket_info *> m_vecsocket_storage;
	std::queue<struct tagpost_data *> m_queue_post;
	std::vector<struct tagpost_data *> m_vecpost_storage;
	int		m_iNewSocketCount;
	int		m_iNewPostDataCount;
private:
	struct  tagsocket_info * GetSocketTarget();
	void    FreeSocketTarget(struct tagsocket_info ** psocket);
	void	AddSocketTarget(struct tagsocket_info * psocket);

	struct  tagpost_data * GetPostDataTarget();
	void    FreePostDataTarget(struct tagpost_data ** ptrpost);
	void    AddPostDataTarget(struct tagpost_data * ptrpost);
private:
	bool	SetSocketAStatus(int fd, int status);
	bool	SetSocketCStatus(int fd, int status);
	bool	SetSocketRStatus(int fd, int status);
	bool	SetSocketWStatus(int fd, int status);

	int		GetSocketAStatus(int fd);
	int		GetSocketCStatus(int fd);
	int		GetSocketRStatus(int fd);
	int		GetSocketWStatus(int fd);
private:
	struct  tagsocket_info *	AddSocketEpoll(int fd);
	void	response_parser(std::string & strdata, std::string & str_content);
	int		AnalysisSocketData(int fd);
	int		ReadSocketData(int fd);
	int		WriteSocketData(int fd);
public:
	int     UpdateSendPostData();
	void	UpdateSocketStatus();
	void	UpdateSocketData();
	void    UpdateCheckTimeOut();
	
public:
	bool	Init();
	int		PostData(const char * api, const char * body);
	void    OnHttpTick();
	void    ShutDown();
};


#endif


