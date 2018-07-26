
#include <vector>
#include <string>
#include <sstream>

#include "http_mgr.h"


#include <signal.h>
#include <sys/file.h>
#include <sys/resource.h>

int GenCoreDumpFile(size_t size = 1024 * 1024 * 32)
{
	struct rlimit flimit;
	flimit.rlim_cur = size;
	flimit.rlim_max = size;
	if (setrlimit(RLIMIT_CORE, &flimit) != 0)
	{
		return errno;
	}

	return 0;
}

int main(int argc, char const *argv[])
{
	GenCoreDumpFile((uint32_t)(1024UL * 1024 * 1024 * 2));
	CHttpMgr::Instance().Init();
	unsigned int iLoopCount = 0xffffffff;
	unsigned long long lLsatPostTime = GetMillisecond();
	printf("iLoopCount:%lld\n", iLoopCount);
	while (true)
	{
		//usleep(100);
		unsigned long long curmillisecond = GetMillisecond();
		if (iLoopCount>0 && lLsatPostTime + 64 < curmillisecond)
		{
			//iLoopCount--;
			CHttpMgr::Instance().PostData("register", "mac_address=FC-AA-14-D3-A4-E7");
			lLsatPostTime = curmillisecond;
			//break;
		}
		CHttpMgr::Instance().OnHttpTick();
	}
	CHttpMgr::Instance().ShutDown();
}



