
#ifndef __CURL_MGR_H__
#define __CURL_MGR_H__


#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>


#include <curl/curl.h>

#include "singleton.h"

#include "logger.h"

using namespace std;


#define MAX_CURL_COUNT (512)

#define CURL_MAX_WAIT_MSECS 30*1000

struct sm_url_info_t
{
	CURL* url;
	int   index;
	unsigned long long btime;
	std::string strData;
	sm_url_info_t()
	{
		Reset();
	}
	void Reset()
	{
		url = NULL;
		index = -1;
		btime = 0;
		strData.clear();
	}
};

class CRobotPostMgr : public AutoDeleteSingleton<CRobotPostMgr>
{
public:
	CRobotPostMgr() {}
	~CRobotPostMgr() {}

	bool	Init();
	void	ShutDown();
public:

	void InitCurl();
	void SubCurl(int index);
	int AddCurl(CURL* curl);
	void RemoveTimeOutCurl();

	int PostData(const std::string &url, const std::string &data);


	void UpdataCurl();
	int GetCurlCount(){	return m_iCurlCount;}
	CURL* GetCurlEasy();

public:
	std::vector<CURL *> m_vecFreeCurlPool;
	int m_iCurlCount;

	sm_url_info_t  m_urlInfo[MAX_CURL_COUNT];

	CURLM* m_multiHandle;
};


static size_t post_data_req_reply(void *ptr, size_t size, size_t nmemb, void *stream)
{
	sm_url_info_t *pInfo = (sm_url_info_t *) stream;
	if (pInfo == NULL)
	{
		return 0;
	}
	//cout<<*str<<endl;
	pInfo->strData.append((char*)ptr, size*nmemb);

	printf("2 robotpostdata - m_iCurlCount:%d,index:%d,curl:%p,data:%s\r\n", CRobotPostMgr::Instance().GetCurlCount(), pInfo->index, pInfo->url, pInfo->strData.c_str());

	//LOG_DEBUG("robotpostdata - m_iCurlCount:%d,index:%d,curl:%p,data:%s\r\n", CRobotPostMgr::Instance().GetCurlCount(), pInfo->index, pInfo->url, pInfo->strData.c_str());

	CRobotPostMgr::Instance().SubCurl(pInfo->index);


	return size*nmemb;
}

#endif // _ROBOT_POST_MGR_H__





