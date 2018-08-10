

#include "mysql_mgr.h"
#include "mysql_async_callback.h"

int main()
{
	CMysqlMgr::Instance().Init();
	CMysqlMgr::Instance().TestMysql();
	CMysqlMgr::Instance().SetAsyncDBCallBack(std::make_shared<CMysqlAsyncCallBack>());

	while (true)
	{
		CMysqlMgr::Instance().OnMysqlTick();
	}

	return 0;
}

