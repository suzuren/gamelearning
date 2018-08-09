

#include "mysql_mgr.h"

int main()
{
	CMysqlMgr::Instance().Init();
	CMysqlMgr::Instance().TestMysql();


	return 0;
}

