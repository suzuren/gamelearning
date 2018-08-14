

#include "network_mgr.h"
#include "network_async_callback.h"
#include "network_daemonize.h"


int main(int argc, const char** argv)
{
	printf("test network server ...\n");

	signal(SIGUSR2, reload);
	signal(SIGUSR1, shutdown);
	daemonize();
	int pid = check_pid("network.pid");
	if (pid)
	{
		fprintf(stderr, "network server is already running, pid = %d.\n", pid);
		return 1;
	}
	pid = write_pid("network.pid");
	if (pid == 0)
	{
		return 1;
	}
	CNetworkMgr::Instance().Init();
	CNetworkMgr::Instance().SetAsyncDBCallBack(std::make_shared<CNetworkAsyncCallBack>());
	CNetworkMgr::Instance().TestMysql();

	while (g_run)
	{
		CNetworkMgr::Instance().OnNetworkTick();
	}
	CNetworkMgr::Instance().ShutDown();
	std::cout << "network server shutdown." << std::endl;
	return 0;
}

