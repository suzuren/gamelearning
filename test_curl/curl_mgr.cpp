

#include "curl_mgr.h"



bool CRobotPostMgr::Init()
{
	m_iCurlCount = 0;

	//for (int index = 0; index < m_ithreadCount; index++)
	//{
	//	pthread_t p_thread;
	//	pthread_create(&p_thread, 0, runthreadfunc, this);
	//	m_pthreadPool.push_back(p_thread);
	//}

	InitCurl();

	return true;
}

void CRobotPostMgr::InitCurl()
{
	curl_global_init(CURL_GLOBAL_ALL);
	m_multiHandle = curl_multi_init();
}

void CRobotPostMgr::SubCurl(int index)
{
	if (m_urlInfo[index].url != NULL)
	{
		curl_multi_remove_handle(m_multiHandle, m_urlInfo[index].url);
		curl_easy_cleanup(m_urlInfo[index].url);
		m_urlInfo[index].Reset();
		m_iCurlCount--;
	}
}


int CRobotPostMgr::AddCurl(CURL* curl)
{
	if (m_iCurlCount == MAX_CURL_COUNT)
	{
		return -1;
	}
	if (curl != NULL )
	{
		for (int i = 0; i < MAX_CURL_COUNT; i++)
		{
			if (m_urlInfo[i].url == NULL)
			{
				m_urlInfo[i].url = curl;
				m_urlInfo[i].index = i;
				m_urlInfo[i].btime = time(NULL);
				m_iCurlCount++;

				return i;
			}
		}
	}
	return -1;
}


void CRobotPostMgr::RemoveTimeOutCurl()
{
	unsigned long long uSysTime = time(NULL);

	for (int index = 0; index < MAX_CURL_COUNT; index++)
	{
		unsigned long long uOldTime = m_urlInfo[index].btime;
		unsigned long long uEndTame = (uOldTime + (60 * 1000));
		if (uEndTame < uSysTime)
		{
			continue;
		}
		if (m_urlInfo[index].url != NULL)
		{
			curl_multi_remove_handle(m_multiHandle, m_urlInfo[index].url);
			curl_easy_cleanup(m_urlInfo[index].url);
			m_urlInfo[index].Reset();
			m_iCurlCount--;
		}
	}
}

void CRobotPostMgr::ShutDown()
{
	curl_multi_cleanup(m_multiHandle);
	for (int index = 0; index < MAX_CURL_COUNT; index++)
	{
		if (m_urlInfo[index].url != NULL)
		{
			curl_multi_remove_handle(m_multiHandle, m_urlInfo[index].url);
			curl_easy_cleanup(m_urlInfo[index].url);
			m_urlInfo[index].Reset();
			m_iCurlCount--;
		}
	}
}

int CRobotPostMgr::PostData(const std::string &url, const std::string &data)
{
	CURL* curl = curl_easy_init();

	int iIndex = AddCurl(curl);

	LOG_DEBUG("robotpostdata - m_iCurlCount:%d,iIndex:%d,curl:%s", m_iCurlCount, iIndex, url.c_str());

	if (iIndex == -1)
	{
		return -1;
	}

	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, post_data_req_reply);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&(m_urlInfo[iIndex]));

	int ret = curl_multi_add_handle(m_multiHandle, curl);
	if (ret != CURLM_OK)
	{
		LOG_DEBUG("failed to add handle - url:%s,data:%s", url.c_str(), data.c_str());
		return -1;
	}

	//int still_running = 0;
	//CURLMcode curlm_code = CURLM_CALL_MULTI_PERFORM;
	//while (CURLM_CALL_MULTI_PERFORM == curlm_code)
	//{
	//	curlm_code = curl_multi_perform(m_multiHandle, &still_running);
	//}
	//if (curlm_code != CURLM_OK)
	//{
	//	LOG_DEBUG( "code:%d,msg:%s", curlm_code, curl_multi_strerror(curlm_code));
	//	return -1;
	//}

	return 1;
}


void CRobotPostMgr::UpdataCurl()
{

	if (m_iCurlCount == 0)
	{
		return;
	}

	RemoveTimeOutCurl();

	int still_running = 0;

	CURLMcode mc = curl_multi_perform(m_multiHandle, &still_running);

	LOG_DEBUG("curl_multi_perform mc:%d,still_running:%d", mc, still_running);

	if (still_running)
	{
		int maxfd = -1;
		fd_set fdread, fdwrite,fdexcep;
		FD_ZERO(&fdread);FD_ZERO(&fdwrite);	FD_ZERO(&fdexcep);
		CURLMcode mc = curl_multi_fdset(m_multiHandle, &fdread, &fdwrite, &fdexcep, &maxfd);
		if (!mc)
		{
			LOG_DEBUG("error - curl_multi_fdset mc:%d",mc);
		}
		long curl_timeo;
		mc = curl_multi_timeout(m_multiHandle, &curl_timeo);
		if (!mc)
		{
			LOG_DEBUG("error - curl_multi_timeout mc:%d", mc);
		}
		int rc = -2;
		struct timeval timeout;

		LOG_DEBUG("mc:%d,maxfd:%d,rc:%d,tv_sec:%ld,tv_usec:%ld,curl_timeo:%ld", mc, maxfd, rc, timeout.tv_sec, timeout.tv_usec, curl_timeo);

		if (maxfd == -1)
		{
			sleep((unsigned int)curl_timeo / 1000);
		}
		else
		{
			
			timeout.tv_sec = curl_timeo / 1000;
			timeout.tv_usec = (curl_timeo % 1000) * 1000;

			rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);


			switch (rc)
			{
			case -1:
				break;
			case 0:
				break;
			default:
				while (1)
				{
					int ret = curl_multi_perform(m_multiHandle, &still_running);
					if (ret != CURLM_CALL_MULTI_PERFORM)
					{
						break;
					}
				}
				break;
			}
		}


	}
	
}

