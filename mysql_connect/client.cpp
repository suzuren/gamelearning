#include <signal.h>
#include <sys/file.h>
#include <sys/resource.h>

#include <vector>
#include <string>
#include <sstream>
#include"db_wrap.h"


int GenCoreDumpFile(size_t size)
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
	printf("test_mysql ... \n");
	GenCoreDumpFile((unsigned int)(1024UL * 1024 * 1024 * 2));

	bool flag = false;
	flag = db_mysql_init();
	flag = db_mysql_open();
	if (flag == false)
	{
		mysql_get_error_message();
		return -1;
	}
	flag = db_mysql_get_connect_character_info();
	flag = db_mysql_get_server_info();
	flag = db_mysql_get_client_info();

	printf("mysql connect success.\n");
	db_mysql_close();
	printf("mysql connect shutting down.\n");
	return 0;
}



