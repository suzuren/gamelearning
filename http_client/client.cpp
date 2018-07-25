
#include <vector>
#include <string>
#include <sstream>

#include "http_mgr.h"


int main(int argc, char const *argv[])
{
	CHttpMgr::Instance().Init();
	int iLoopCount = 20;
	while (true)
	{
		//usleep(100);
		
		if (iLoopCount>0)
		{
			iLoopCount--;
			CHttpMgr::Instance().PostData("register", "mac_address=FC-AA-14-D3-A4-E7");
			//break;
		}
		CHttpMgr::Instance().OnHttpTick();
	}
	CHttpMgr::Instance().ShutDown();
}



