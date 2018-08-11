

#include "mysql_mgr.h"
#include "mysql_async_callback.h"
#include "mysql_daemonize.h"


int main(int argc, const char** argv)
{
	printf("test database server ...\n");

	signal(SIGUSR2, reload);
	signal(SIGUSR1, shutdown);
	daemonize();
	int pid = check_pid("database.pid");
	if (pid)
	{
		fprintf(stderr, "database server is already running, pid = %d.\n", pid);
		return 1;
	}
	pid = write_pid("database.pid");
	if (pid == 0)
	{
		return 1;
	}
	CMysqlMgr::Instance().Init();
	CMysqlMgr::Instance().SetAsyncDBCallBack(std::make_shared<CMysqlAsyncCallBack>());
	CMysqlMgr::Instance().TestMysql();

	while (g_run)
	{
		CMysqlMgr::Instance().OnMysqlTick();
	}
	CMysqlMgr::Instance().ShutDown();
	std::cout << "database server shutdown." << std::endl;
	return 0;
}

