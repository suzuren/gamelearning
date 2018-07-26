
#include <mysql.h>

#include "db_mysql.h"

struct tag_db_connect
{
	unsigned long flag;
	unsigned int port;
	char	host[32];
	char	user[32];
	char	pass[32];
	char	base[32];
	char	unix[32];

	tag_db_connect()
	{
		init();
	}
	void init()
	{
		flag = 0;
		port = 0;
		memset(host, 0, sizeof(host));
		memset(user, 0, sizeof(user));
		memset(pass, 0, sizeof(pass));
		memset(base, 0, sizeof(base));
		memset(unix, 0, sizeof(unix));
	}
};

struct tag_db_info
{
	MYSQL*	mysql;
	bool    bconnected;
	bool    breconnect;
	int     timeout;
	tag_db_info()
	{
		init();
	}
	void init()
	{
		mysql = NULL;
		bconnected = false;
		breconnect = false;
		timeout = 0;
	}
};

static struct tag_db_connect * DB_CONNECT = NULL;
static struct tag_db_content * DB_CONTENT = NULL;



bool mysql_init();

bool mysql_open();
