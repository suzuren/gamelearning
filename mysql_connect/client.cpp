
#include <vector>
#include <string>
#include <sstream>
#include"db_wrap.h"



int main(int argc, char const *argv[])
{
	printf("test_mysql ... \n");

	bool flag = false;
	flag = db_mysql_init();
	flag = db_mysql_open();
	if (flag == false)
	{
		mysql_get_error_message();
		return -1;
	}
	printf("mysql connect success.\n");
	db_mysql_close();
	printf("mysql connect shutting down.\n");
	return 0;
}



