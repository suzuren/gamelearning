
#include <atomic>

#include "network_daemonize.h"
#include "network_task_callback.h"
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
	CNetworkMgr::Instance().Init();
	//CNetworkMgr::Instance().TestNetwork();
	
	//unsigned long long	ulStartTime = GetMillisecond();

	while (g_run)
	{
		CNetworkMgr::Instance().OnNetworkTick();
	}
	CNetworkMgr::Instance().ShutDown();
	std::cout << "network server shutdown." << std::endl;
	return 0;
}

