
#include <time.h>
#include <stdio.h>

#include "hiredis_mgr.h"

unsigned long long	GetMillisecond()
{
	timespec _spec;
	clock_gettime(CLOCK_MONOTONIC, &_spec);
	unsigned long long millisecond = _spec.tv_sec * 1000 + _spec.tv_nsec / 1000 / 1000;
	return millisecond;
}

int main(int argc, const char** argv)
{
	printf("redis server start ...\n");
	

	bool bStartConnect = false;
	unsigned long long	ulStartTime = GetMillisecond();

	CNetworkMgr::Instance().Init();

	while (true)
	{
		CNetworkMgr::Instance().OnNetworkTick();

		if (!bStartConnect && ulStartTime + 1000 <= GetMillisecond())
		{
			CNetworkMgr::Instance().Connect();
			bStartConnect = true;
			ulStartTime = GetMillisecond();
		}
		if (bStartConnect && ulStartTime + 1000 <= GetMillisecond())
		{
			ulStartTime = GetMillisecond();
		}
	}
	CNetworkMgr::Instance().ShutDown();

	printf("redis server shutdown ...\n");
	return 0;
}

