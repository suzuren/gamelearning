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

// core function
// mysql_ping()
// mysql_set_server_option()
// mysql_set_character_set()
// mysql_get_server_version()
// mysql_stat()
// mysql_real_query()
// mysql_list_processes()
// mysql_change_user()
// db_mysql_select_database
// 
// 
// 
// 
// 

int main(int argc, char const *argv[])
{
	printf("test_mysql ... \n");
	GenCoreDumpFile((unsigned int)(1024UL * 1024 * 1024 * 2));

	bool flag = false;
	flag = db_mysql_init();
	flag = db_mysql_open();

	//flag = db_mysql_multi_sentence(true);
	//flag = db_mysql_set_connect_character("utf8");
	//flag = db_mysql_set_current_character("utf8");
	const char * pcharset = db_mysql_get_connect_character();
	printf("db_mysql_get_connect_character - pcharset:%s\n", pcharset);
	flag = db_mysql_get_connect_character_info();

	flag = db_mysql_get_server_info();
	flag = db_mysql_get_client_info();

	//MYSQL_RES * presult = db_mysql_show_processes_list();
	unsigned long threadid = db_mysql_get_connect_threadid();
	printf("db_mysql_get_connect_threadid - threadid:%ld\n", threadid);
	unsigned int count = db_mysql_get_last_warning_count();
	printf("db_mysql_get_last_warning_count - count:%d\n", count);
	const char * pinfo = db_mysql_get_execute_info();
	printf("db_mysql_get_execute_info - pinfo:%s\n", pinfo);
	//flag = db_mysql_change_connect_info("root", "game123456", "chess_sysdata");
	//flag = db_mysql_select_database("chess_sysdata");
	//printf("db_mysql_select_database - flag:%d\n", flag);
	//flag = db_mysql_create_database("testdb", "utf8", NULL, true);
	printf("db_mysql_create_database - flag:%d\n", flag);


	printf("mysql connect success.\n");
	db_mysql_close();
	printf("mysql connect shutting down.\n");
	return 0;
}



