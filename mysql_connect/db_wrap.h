

#ifndef __DB_WRAP_H__
#define __DB_WRAP_H__

#include <memory.h>
#include <stdio.h>

#include <mysql.h>
#include <errmsg.h>
#include <stdarg.h>

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
	void recmd()
	{
		length = 0;
		memset(command, 0, sizeof(command));
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

bool db_mysql_init();
bool db_mysql_open();
void db_mysql_close();
bool  db_mysql_ping();
bool db_mysql_multi_sentence(bool bmore);

bool db_mysql_set_connect_character(const char * charset);
bool db_mysql_set_current_character(const char * charset);
const char*	db_mysql_get_connect_character();
bool db_mysql_get_connect_character_info();

bool db_mysql_get_server_info();
bool db_mysql_get_client_info();

MYSQL_RES * db_mysql_show_processes_list();
unsigned long db_mysql_get_connect_threadid();
unsigned int db_mysql_get_last_warning_count();
const char*	db_mysql_get_execute_info();

bool db_mysql_change_connect_info(const char* user, const char* pass, const char* db);

bool db_mysql_select_database(const char* database);
bool db_mysql_create_database(const char* database, const char* charset, const char* collation, bool exist);
bool db_mysql_drop_database(const char* database, bool exist);
bool db_mysql_alter_database(const char* database, const char* charset, const char* collation);

bool db_mysql_exists_table(const char* ptable);
bool db_mysql_create_table(const char* ptable, bool temp, bool exist, const char*definition, ...);
bool db_mysql_drop_table(bool temp, bool exist, const char*ptable, ...);
bool db_mysql_alter_table(const char* ptable, const char* cmd, ...);
bool db_mysql_rename_table(const char* ptable, const char* pnewname);

MYSQL_RES* db_mysql_show_database(const char* wild);
MYSQL_RES* db_mysql_show_tables(const char* wild);
MYSQL_RES* db_mysql_show_fields(const char* ptable, const char* wild);

int	db_mysql_cmd(const char*pcmd, ...);
bool db_mysql_execute(bool real);

my_ulonglong db_mysql_get_insert_increment();
my_ulonglong db_mysql_get_affected_rows();
unsigned int db_mysql_get_result_fieldcount();

MYSQL_RES* db_mysql_get_result();
bool db_mysql_result_more();


#endif



