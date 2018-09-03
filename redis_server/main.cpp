
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
	
	unsigned long long	ulStartTime = GetMillisecond();

	CRedisMgr::Instance().Init();
	bool bStartConnect = CRedisMgr::Instance().Connect();
	printf("main - bStartConnect:%d\n", bStartConnect);

	while (bStartConnect)
	{
		CRedisMgr::Instance().OnTick();
		if (ulStartTime + 1000 <= GetMillisecond())
		{
			ulStartTime = GetMillisecond();
			CRedisMgr::Instance().Test();
		}
		if (ulStartTime + 1000 <= GetMillisecond())
		{
			ulStartTime = GetMillisecond();
		}
		
	}
	CRedisMgr::Instance().ShutDown();

	printf("redis server shutdown ...\n");
	return 0;
}

