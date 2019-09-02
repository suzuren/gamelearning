
#ifndef __HTTP_MGR_H_
#define __HTTP_MGR_H_

#include"../socketaide/socket_aide.h"

#include<string>
#include<map>
#include<stdlib.h>
#include <queue>
#include <unordered_map>

#define MAX_READ_HTTP_DATA 512
#define MAX_BODY_HTTP_DATA 128

#define MAX_HTTP_DATA_FREE		1024

//错误代码  具体含义
//000  请求成功
//- 01  用户帐号余额不足
//- 02  用户帐号不存在
//- 03  用户帐号密码不正确
//- 04  请求参数错误
//- 12  其它错误
//- 110  IP 被限制

typedef struct tagPostData_t
{
	int id;
	int state;
	unsigned int type;
	std::string accounts;
	std::string code;

	int rszie;
	char rbuffer[MAX_READ_HTTP_DATA];

	int bodylen;
	char body[MAX_BODY_HTTP_DATA];

	bool success;

	tagPostData_t()
	{
		Init();
	}

	void Init()
	{
		id = -1;
		state = 0;
		rszie = 0;
		memset(rbuffer, 0, sizeof(rbuffer));

		bodylen = 0;
		memset(body, 0, sizeof(body));

		success = false;
	}

	void operator=(const tagPostData_t& tmpData)
	{
		id = tmpData.id;
		state = tmpData.state;
		type = tmpData.type;
		accounts = tmpData.accounts;
		code = tmpData.code;

		rszie = tmpData.rszie;
		memcpy(rbuffer, tmpData.rbuffer, tmpData.rszie);

		bodylen = tmpData.bodylen;
		memcpy(body, tmpData.body, tmpData.bodylen);

		success = tmpData.success;
	}
}tagPostData;


class CHandleHttpMsg : public IHanldeMessageSink
{
public:
	virtual int OnOpenListen(Aide_ListenInfo* pListenInfo);
	virtual int OnOpenClient(Aide_AcceptInfo* pAcceptInfo);
	virtual int OnOpenConnect(Aide_ConnectInfo* pConnectInfo);

	virtual int OnExceptionListen(Aide_ListenInfo* pListenInfo);
	virtual int OnExceptionClient(Aide_AcceptInfo* pAcceptInfo);
	virtual int OnExceptionConnect(Aide_ConnectInfo* pConnectInfo);

	virtual int OnRecvMessage(int iSocketID, unsigned short uHeadSize, const unsigned char* pHeadData, unsigned short uPackSize, const unsigned char* pPackData);

protected:

};


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
		static  CHttpMgr  s_SingleObject;
		return  s_SingleObject;
	}
private:

	pthread_t m_pthread;
	std::unordered_map<int, tagPostData *>	m_mpPostData;
	std::queue<tagPostData *>		m_QueueDataFree; // 接受连接空闲对象

protected:

	bool	ParseBodyData(tagPostData* pPostData);

public:

	bool	HttpInit();
	int		HttpSetData(tagPostData & paramData);
	void    OnHttpTick();
	void    HttpShutDown();
	bool	GetDataByHID(int id, tagPostData* pPostData);
	void	RemoveDataByHID(int id);

	int		HttpPostData(int id);
	bool	ParseExceptionState(int id);
	bool	ParsePacketData(int id, unsigned short size, unsigned char* data);
};


#endif


