
#include <atomic>

#include "network_daemonize.h"
#include "network_task_callback.h"
#include "network_mgr.h"


int main(int argc, const char** argv)
{
	printf("test network server ...\n");

	int ret = daemo_start();
	if (!ret)
	{
		return 0;
	}

	CNetworkMgr::Instance().SetAsyncNetTaskCallBack(std::make_shared<CNetworkTaskCallBack>());
	CNetworkMgr::Instance().Init();
	//CNetworkMgr::Instance().TestNetwork();
	
	bool bStartConnect = false;
	unsigned long long	ulStartTime = GetMillisecond();

	while (g_run)
	{
		CNetworkMgr::Instance().OnNetworkTick();

		if (!bStartConnect && ulStartTime + 1000 <= GetMillisecond())
		{
			CNetworkMgr::Instance().Connect();
			bStartConnect = true;
			ulStartTime = GetMillisecond();
			CNetworkMgr::Instance().TestClientSendData();

		}
		if (bStartConnect && ulStartTime + 1 <= GetMillisecond())
		{
			//std::cout << "CNetworkMgr::Instance().TestNetworkSendData();" << std::endl;
			//CNetworkMgr::Instance().TestClientSendData();

			ulStartTime = GetMillisecond();
		}
	}
	CNetworkMgr::Instance().ShutDown();
	std::cout << "network server shutdown." << std::endl;
	return 0;
}

