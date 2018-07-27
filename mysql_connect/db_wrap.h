

#ifndef __DB_WRAP_H__
#define __DB_WRAP_H__

#include <memory.h>
#include <stdio.h>

#include <mysql.h>
#include <errmsg.h>

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

struct tag_db_content
{
	MYSQL*	mysql;
	bool    connected;
	bool    reconnect;
	int     timeout;
	char	charset[256];
	int		length;
	char	command[16 * 1024];
	tag_db_content()
	{
		init();
	}
	void init()
	{
		mysql = NULL;
		connected = false;
		reconnect = false;
		timeout = 0;
		memset(charset, 0, sizeof(charset));
		length = 0;
		memset(command, 0, sizeof(command));
	}
};

static struct tag_db_connect * DB_CONNECT = NULL;
static struct tag_db_content * DB_CONTENT = NULL;



bool mysql_init();

bool mysql_open();



#endif



