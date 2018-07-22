
#ifndef __HTTP_MGR_H_
#define __HTTP_MGR_H_

#include <utility>
#include <map>
#include <vector>
#include <string>

#include "http_socket.h"

#define MAX_SOCKET_CONNECT	1024
#define SOCKET_TCP_BUFFER   65535

//enum emSOCKET_STATUS
//{
//	SOCKET_STATUS_INIT = 0,
//	SOCKET_STATUS_CONNECTING = 1,
//	SOCKET_STATUS_CONNECT_FAILURE = 2,
//	SOCKET_STATUS_CONNECTED = 3,
//	SOCKET_STATUS_WRITE = 4,
//	SOCKET_STATUS_WRITEING = 5,
//	SOCKET_STATUS_WRITEED = 6,
//	SOCKET_STATUS_READ = 7,
//	SOCKET_STATUS_READING = 8,
//	SOCKET_STATUS_READED = 9,
//	SOCKET_STATUS_WAIT = 10,
//	SOCKET_STATUS_CLOSE = 11,
//	SOCKET_STATUS_ERROR = 12,
//	SOCKET_STATUS_TIME_OUT = 13
//};

enum emSOCKET_STATUS
{
	SOCKET_STATUS_INIT = 0,
	SOCKET_STATUS_WAIT = 1,
	SOCKET_STATUS_CLOSE = 2,
	SOCKET_STATUS_ERROR = 3,
	SOCKET_STATUS_TIME_OUT = 4
};
enum emSOCKET_STATUS_CONNECT
{
	SOCKET_STATUS_CINIT = 0,
	SOCKET_STATUS_CONNECTING = 1,
	SOCKET_STATUS_CONNECT_FAILURE = 2,
	SOCKET_STATUS_CONNECTED = 3,
};
enum emSOCKET_STATUS_WRITE
{
	SOCKET_STATUS_WINIT = 0,
	SOCKET_STATUS_WRITE = 1,
	SOCKET_STATUS_WRITEING = 2,
	SOCKET_STATUS_WRITEED = 3,
};
enum emSOCKET_STATUS_READ
{
	SOCKET_STATUS_RINIT = 0,
	SOCKET_STATUS_READ = 1,
	SOCKET_STATUS_READING = 2,
	SOCKET_STATUS_READED = 3,
};

//static const int SOCKET_STATUS_INIT = 0;
//static const int SOCKET_STATUS_CONNECTING = 1;
//static const int SOCKET_STATUS_CONNECT_FAILURE = 2;
//static const int SOCKET_STATUS_CONNECTED = 3;
//static const int SOCKET_STATUS_WRITE = 4;
//static const int SOCKET_STATUS_WRITEING = 5;
//static const int SOCKET_STATUS_WRITEED = 6;
//static const int SOCKET_STATUS_READ = 7;
//static const int SOCKET_STATUS_READING = 8;
//static const int SOCKET_STATUS_READED = 9;
//static const int SOCKET_STATUS_WAIT = 10;
//static const int SOCKET_STATUS_CLOSE = 11;
//static const int SOCKET_STATUS_ERROR = 12;
//static const int SOCKET_STATUS_TIME_OUT = 13;

#pragma  pack(1)

struct tagsocket_connect
{
	struct epoll_event events[MAX_SOCKET_CONNECT];
	int epfd;
	void reevents()
	{
		memset(events, 0, sizeof(events));
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
	char rbuffer[SOCKET_TCP_BUFFER];
	char wbuffer[SOCKET_TCP_BUFFER];
	std::string content;
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
		memset(rbuffer, 0, sizeof(rbuffer));
		memset(wbuffer, 0, sizeof(wbuffer));
		content.clear();
	}
};
#pragma pack()
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
private:
	struct tagsocket_info * GetSocketTarget();
	void	AddSocketTarget(struct tagsocket_info * psocket);
	struct tagsocket_info *	AddSocketEpoll(int fd);

	void	response_parser(std::string & strdata, std::string & str_content);

	bool	SetSocketAStatus(int fd, int status);
	bool	SetSocketCStatus(int fd, int status);
	bool	SetSocketRStatus(int fd, int status);
	bool	SetSocketWStatus(int fd, int status);

	int		GetSocketAStatus(int fd);
	int		GetSocketCStatus(int fd);
	int		GetSocketRStatus(int fd);
	int		GetSocketWStatus(int fd);

	int		AnalysisSocketData(int fd);
	int		ReadSocketData(int fd);
	int		WriteSocketData(int fd);

public:
	bool	Init();
	int		PostData(const char * api, const char * body);
	void	UpdateSocketStatus();
	void	UpdateSocketData();
	
};


#endif


