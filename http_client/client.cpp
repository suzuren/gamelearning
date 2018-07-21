
#include <vector>
#include <string>
#include <sstream>

#include "http_mgr.h"


int main(int argc, char const *argv[])
{
	CHttpMgr::Instance().Init();
	CHttpMgr::Instance().PostData("register", "mac_address=FC-AA-14-D3-A4-E8");
	while (true)
	{
		CHttpMgr::Instance().UpdateSocketStatus();
		CHttpMgr::Instance().UpdateSocketData();
		//usleep(100);
	}
}



