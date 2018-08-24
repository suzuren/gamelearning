
#include <atomic>

#include "network_oper_define.h"

#include "network_task_callback.h"
#include "network_wrap_callback.h"
#include "network_daemonize.h"
#include "network_mgr.h"


int main(int argc, const char** argv)
{
	printf("test network server ...\n");

	//signal(SIGUSR2, reload);
	//signal(SIGUSR1, shutdown);
	//daemonize();
	//int pid = check_pid("network.pid");
	//if (pid)
	//{
	//	fprintf(stderr, "network server is already running, pid = %d.\n", pid);
	//	return 1;
	//}
	//pid = write_pid("network.pid");
	//if (pid == 0)
	//{
	//	return 1;
	//}

	GenCoreDumpFile((uint32_t)(1024UL * 1024 * 1024 * 2));


	CNetworkMgr::Instance().SetAsyncNetTaskCallBack(std::make_shared<CNetworkTaskCallBack>());
	CNetworkMgr::Instance().SetAsyncNetWrapCallBack(std::make_shared<CNetworkWrapCallBack>());
	CNetworkMgr::Instance().Init();
	//CNetworkMgr::Instance().TestNetwork();
	
	bool bStartConnect = false;
	unsigned long long	ulStartTime = GetMillisecond();

	while (g_run)
	{
		CNetworkMgr::Instance().OnNetworkTick();
		if (!bStartConnect && ulStartTime + 1000 <= GetMillisecond())
		{
			CNetworkMgr::Instance().TestNetworkConnect();
			bStartConnect = true;
			ulStartTime = GetMillisecond();
		}
		if (bStartConnect && ulStartTime + 3000 <= GetMillisecond())
		{
			CNetworkMgr::Instance().TestNetworkSendData();

			ulStartTime = GetMillisecond();
		}
	}
	CNetworkMgr::Instance().ShutDown();
	std::cout << "network server shutdown." << std::endl;
	return 0;
}

