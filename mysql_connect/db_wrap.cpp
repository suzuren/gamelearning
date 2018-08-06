
#include "db_wrap.h"
#include "db_mysql.h"

static struct tag_db_connect * DB_CONNECT = NULL;
static struct tag_db_content * DB_CONTENT = NULL;


bool db_mysql_init()
{
	DB_CONNECT = new (struct tag_db_connect);
	DB_CONNECT->init();

	sprintf(DB_CONNECT->host, "%s", "127.0.0.1");
	sprintf(DB_CONNECT->user, "%s", "root");
	sprintf(DB_CONNECT->pass, "%s", "game123456");
	sprintf(DB_CONNECT->base, "%s", "chess");

	DB_CONTENT = new (struct tag_db_content);
	DB_CONTENT->init();
	DB_CONTENT->reconnect = true;
	DB_CONTENT->timeout = 10;
	sprintf(DB_CONTENT->charset, "%s", "utf8");

	mysql_server_init(0, NULL, NULL);
	return true;
}

bool db_mysql_open()
{
	int ret = 0;
	MYSQL* pmysql = NULL;
	char charset[32] = { 0 };
	int timeout = DB_CONTENT->timeout;
	bool reconnect = DB_CONTENT->reconnect;	
	strcpy(charset, DB_CONTENT->charset);	
	pmysql = mysql_init(pmysql);
	if (pmysql == NULL)
	{
		mysql_set_error_sentence(pmysql, 0, "mysql_init()");
		return false;
	}
	DB_CONTENT->mysql = pmysql;
	ret = mysql_options(pmysql, MYSQL_OPT_CONNECT_TIMEOUT, (char*)&timeout);
	if (ret != 0)
	{
		mysql_set_error_sentence(pmysql, 0, "mysql_options(MYSQL_OPT_CONNECT_TIMEOUT)");
		return false;
	}
	ret = mysql_options(pmysql, MYSQL_OPT_READ_TIMEOUT, (char*)&timeout);
	if (ret != 0)
	{
		mysql_set_error_sentence(pmysql, 0, "mysql_options(MYSQL_OPT_CONNECT_TIMEOUT)");
		return false;
	}
	ret = mysql_options(pmysql, MYSQL_OPT_WRITE_TIMEOUT, (char*)&timeout);
	if (ret != 0)
	{
		mysql_set_error_sentence(pmysql, 0, "mysql_options(MYSQL_OPT_CONNECT_TIMEOUT)");
		return false;
	}
	ret = mysql_options(pmysql, MYSQL_OPT_RECONNECT, &reconnect);
	if (ret != 0)
	{
		mysql_set_error_sentence(pmysql, 0, "mysql_options(MYSQL_OPT_RECONNECT)");
	}
	ret = mysql_options(pmysql, MYSQL_SET_CHARSET_NAME, charset);
	if (ret != 0)
	{
		mysql_set_error_sentence(pmysql, 0, "mysql_options(MYSQL_SET_CHARSET_NAME)");
	}
	
	return true;
}

void db_mysql_close()
{
	if (DB_CONTENT != NULL && DB_CONTENT->mysql == NULL)
	{
		return;
	}
	DB_CONTENT->connected = false;
	mysql_close(DB_CONTENT->mysql);
	DB_CONTENT->mysql = NULL;
}

bool  db_mysql_ping()
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return false;
	}
	int ret = mysql_ping(pmysql);
	if (ret == 0)
	{
		return true;
	}
	static char message[128];
	memset(message,0,sizeof(message));
	sprintf(message, "mysql_ping(%d)", ret);
	mysql_set_error_sentence(pmysql, 0, message);
	return false;
}

bool db_mysql_multi_sentence(bool bmore)
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return false;
	}
	int ret = mysql_set_server_option(pmysql, bmore ? MYSQL_OPTION_MULTI_STATEMENTS_ON : MYSQL_OPTION_MULTI_STATEMENTS_OFF);
	if (ret != 0)
	{
		mysql_set_error_sentence(pmysql, 0, "mysql_set_server_option()");
		return false;
	}
	return true;
}

bool db_mysql_set_connect_character(const char * charset)
{
	if (charset == NULL || *charset == '\0' || db_mysql_ping() == false)
	{
		return false;
	}
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return false;
	}
	int ret = mysql_set_character_set(pmysql, charset);
	if (ret != 0)
	{
		mysql_set_error_sentence(pmysql, 0, "mysql_set_character_set()");
		return false;
	}
	return true;
}

bool	db_mysql_set_current_character(const char * charset)
{
	if (charset == NULL)
	{
		return false;
	}
	db_mysql_cmd("SET names '%s';SET CHARACTER_SET '%s'", charset, charset);
	if (db_mysql_execute(true))
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

const char*	db_mysql_get_connect_character()
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return NULL;
	}
	if (db_mysql_ping() == false)
	{
		return NULL;
	}
	return mysql_character_set_name(pmysql);
}

bool db_mysql_get_connect_character_info()
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return false;
	}
	MY_CHARSET_INFO info;
	memset(&info, 0, sizeof(info));
	mysql_get_character_set_info(pmysql, &info);
	printf("db_mysql_get_connect_character_info - 字符集信息:\n");
	printf("db_mysql_get_connect_character_info - 字符集名: %s\n", info.name);
	printf("db_mysql_get_connect_character_info - 校对名: %s\n", info.csname);
	printf("db_mysql_get_connect_character_info - 注释: %s\n", info.comment);
	printf("db_mysql_get_connect_character_info - 目录: %s\n", info.dir);
	printf("db_mysql_get_connect_character_info - 多字节字符最小长度: %d\n", info.mbminlen);
	printf("db_mysql_get_connect_character_info - 多字节字符最大长度: %d\n", info.mbmaxlen);
	return true;
}

bool db_mysql_get_server_info()
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return false;
	}
	if (db_mysql_ping() == false)
	{
		return false;
	}
	printf("db_mysql_get_server_info - mysql_get_host_info(%s)\n", mysql_get_host_info(pmysql));
	printf("db_mysql_get_server_info - 以整数形式返回服务器的版本号(%ld)\n", mysql_get_server_version(pmysql));
	printf("db_mysql_get_server_info - 返回服务器的版本号(%s)\n", mysql_get_server_info(pmysql));
	printf("db_mysql_get_server_info - 以字符串形式返回服务器状态(%s)\n", mysql_stat(pmysql));
	printf("db_mysql_get_server_info - 最后查询信息(%s)\n", mysql_info(pmysql));
	return true;
}

bool db_mysql_get_client_info()
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return false;
	}
	printf("db_mysql_get_client_info - 返回连接所使用的协议版本。(%u)\n", mysql_get_proto_info(pmysql));
	printf("db_mysql_get_client_info - 以字符串形式返回客户端版本信息。(%s)\n", mysql_get_client_info());
	printf("db_mysql_get_client_info - 以整数形式返回客户端版本信息(%ld)\n", mysql_get_client_version());
	return true;
}

MYSQL_RES * db_mysql_show_processes_list()
{
	MYSQL_RES * presult = NULL;
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return presult;
	}
	if (db_mysql_ping() == false)
	{
		return presult;
	}
	presult = mysql_list_processes(pmysql);
	if (presult == NULL)
	{
		//CR_COMMANDS_OUT_OF_SYNC	以不恰当的顺序执行了命令。
		//CR_SERVER_GONE_ERROR		MySQL服务器不可用。
		//CR_SERVER_LOST			在查询过程中，与服务器的连接丢失。
		//CR_UNKNOWN_ERROR			出现未知错误。
	}
	return presult;
}

unsigned long db_mysql_get_connect_threadid()
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return 0;
	}
	if (db_mysql_ping() == false)
	{
		return 0;
	}
	return mysql_thread_id(pmysql);
}
unsigned int db_mysql_get_last_warning_count()
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return 0;
	}
	return mysql_warning_count(pmysql);
}

const char*	db_mysql_get_execute_info()
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return NULL;
	}
	return mysql_info(pmysql);
}



bool db_mysql_change_connect_info(const char* user, const char* pass, const char* db)
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return false;
	}
	my_bool	bret = mysql_change_user(pmysql, user, pass, db);
	if (bret == 0)
	{
		return true;
	}
	return false;
}

bool db_mysql_select_database(const char* database)
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return false;
	}
	if (database ==NULL || *database =='\0' || db_mysql_ping()==false)
	{
		return false;
	}
	return (mysql_select_db(pmysql, database) == 0);
}

bool db_mysql_create_database(const char* database, const char* charset, const char* collation, bool exist)
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return false;
	}
	if (database == NULL || *database == '\0' || db_mysql_ping() == false)
	{
		return false;
	}
	char command[1024] = { 0 };
	sprintf(command, "CREATE DATABASE");
	if (exist)
	{
		sprintf(command,"%s IF NOT EXISTS", command);
	}
	sprintf(command,"%s %s", command, database);
	if (charset != NULL)
	{
		sprintf(command, "%s CHARACTER SET %s", command,charset);
	}
	if (collation != NULL)
	{
		sprintf(command,"%s %sCOLLATE %s", command, (charset&& *charset) ? "," : "", collation);
	}
	return (mysql_query(pmysql, command) == 0);
}

bool db_mysql_drop_database(const char* database, bool exist)
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return false;
	}
	if (database == NULL || *database == '\0' || db_mysql_ping() == false)
	{
		return false;
	}
	char command[1024] = { 0 };
	sprintf(command,"DROP DATABASE");
	if (exist)
	{
		sprintf(command, "%s IF EXISTS", command);
	}
	sprintf(command,"%s %s", command, database);
	return (mysql_query(pmysql, command) == 0);
}

bool db_mysql_alter_database(const char* database, const char* charset, const char* collation)
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return false;
	}
	if (database == NULL || *database == '\0' || db_mysql_ping() == false)
	{
		return false;
	}
	if (charset == NULL && collation == NULL)
	{
		return false;
	}
	if (db_mysql_ping() == false)
	{
		return false;
	}
	char command[1024] = { 0 };
	sprintf(command,"ALTER DATABASE");
	sprintf(command,"%s %s", command, database);
	sprintf(command,"%s CHARACTER SET %s", command, charset);
	sprintf(command,"%s %sCOLLATE %s", command, (charset && *charset) ? "," : "", collation);
	return (mysql_query(pmysql, command) == 0);
}

bool db_mysql_exists_table(const char* ptable)
{
	if (ptable == NULL)
	{
		return false;
	}
	db_mysql_cmd("SHOW TABLES LIKE '%s'", ptable);
	if (db_mysql_execute(true))
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

bool db_mysql_create_table(const char* ptable, bool temp, bool exist, const char*definition, ...)
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return false;
	}
	if (ptable == NULL || *ptable == '\0')
	{
		return false;
	}
	if (definition == NULL || *definition == '\0')
	{
		return false;
	}
	if (db_mysql_ping() == false)
	{
		return false;
	}
	char command[1024] = { 0 };
	if (temp)
	{
		sprintf(command,"CREATE TEMPORARY TABLE");
	}
	else
	{
		sprintf(command, "CREATE TABLE");
	}
	if (exist)
	{
		sprintf(command, "%s IF NOT EXISTS", command);
	}
	sprintf(command,"%s %s (", command, ptable);
	char format[512] = { 0 };
	va_list	argptr;
	va_start(argptr, definition);
	sprintf(format, definition, argptr);
	va_end(argptr);
	sprintf(command,"%s %s);", command, format);
	return (mysql_query(pmysql, command) == 0);
}

bool db_mysql_drop_table(bool temp, bool exist, const char*ptable, ...)
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return false;
	}
	if (ptable == NULL || *ptable == '\0')
	{
		return false;
	}
	if (db_mysql_ping() == false)
	{
		return false;
	}
	char command[1024] = { 0 };
	if (temp)
	{
		sprintf(command, "DROP TEMPORARY TABLE");
	}
	else
	{
		sprintf(command, "DROP TABLE");
	}
	if (exist)
	{
		sprintf(command, "%s IF EXISTS", command);
	}
	char format[512] = { 0 };
	va_list	argptr;
	va_start(argptr, ptable);
	sprintf(format, ptable, argptr);
	va_end(argptr);
	sprintf(command, "%s %s", command, format);
	return (mysql_query(pmysql, command) == 0);
}

bool db_mysql_alter_table(const char* ptable, const char* cmd, ...)
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return false;
	}
	if (ptable == NULL || *ptable == '\0')
	{
		return false;
	}
	if (cmd == NULL || *cmd == '\0')
	{
		return false;
	}
	if (db_mysql_ping() == false)
	{
		return false;
	}
	char command[1024] = { 0 };
	sprintf(command, "ALTER TABLE %s", ptable);
	char format[512] = { 0 };
	va_list	argptr;
	va_start(argptr, cmd);
	sprintf(format, cmd, argptr);
	va_end(argptr);
	sprintf(command, "%s %s", command, format);
	return (mysql_query(pmysql, command) == 0);
}

bool db_mysql_rename_table(const char* ptable, const char* pnewname)
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return false;
	}
	if (ptable == NULL || *ptable == '\0')
	{
		return false;
	}
	if (pnewname == NULL || *pnewname == '\0')
	{
		return false;
	}
	if (db_mysql_ping() == false)
	{
		return false;
	}
	char command[1024] = { 0 };
	sprintf(command,"RENAME TABLE %s TO %s", ptable, pnewname);
	return (mysql_query(pmysql, command) == 0);
}

MYSQL_RES* db_mysql_show_database(const char* wild)
{
	MYSQL_RES * presult = NULL;
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return presult;
	}
	if (db_mysql_ping() == false)
	{
		return presult;
	}
	presult = mysql_list_dbs(pmysql, wild);
	if (presult == NULL)
	{
		mysql_set_error_sentence(pmysql, 0, "mysql_list_dbs()");
		return presult;
	}
	return presult;
}
MYSQL_RES* db_mysql_show_tables(const char* wild)
{
	MYSQL_RES * presult = NULL;
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return presult;
	}
	if (db_mysql_ping() == false)
	{
		return presult;
	}
	/*SHOW tables [LIKE wild]*/
	presult = mysql_list_tables(pmysql, wild);
	if (presult == NULL)
	{
		mysql_set_error_sentence(pmysql, 0, "mysql_list_tables()");
		return presult;
	}
	return presult;
}

MYSQL_RES* db_mysql_show_fields(const char* ptable, const char* wild)
{
	MYSQL_RES * presult = NULL;
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return presult;
	}
	if (ptable == NULL || *ptable == '\0')
	{
		return presult;
	}
	if (db_mysql_ping() == false)
	{
		return presult;
	}
	char command[2 * 1024] = { 0 };
	sprintf(command, "SHOW COLUMNS FROM %s", ptable);
	if (wild != NULL)
	{
		sprintf(command, "%s %s", command, wild);
	}
	if (mysql_query(pmysql, command) != 0)
	{
		mysql_set_error_sentence(pmysql, 0, "mysql_query()");
		return presult;
	}
	presult = mysql_store_result(pmysql);
	if (pmysql == NULL)
	{
		mysql_set_error_sentence(pmysql, 0, "mysql_store_result()");
		return presult;
	}
	return presult;
}

int	db_mysql_cmd(const char*pcmd, ...)
{
	if (pcmd == NULL)
	{
		return 0;
	}
	DB_CONTENT->recmd();
	char * pcommand = DB_CONTENT->command;
	va_list	argptr;
	va_start(argptr, pcmd);
	int length = sprintf(pcommand, pcmd, argptr);
	va_end(argptr);
	DB_CONTENT->length = length;
	return length;
}

bool db_mysql_execute(bool real)
{
	int length = DB_CONTENT->length;
	char * pcommand = DB_CONTENT->command;
	MYSQL * pmysql = DB_CONTENT->mysql;

	if (pmysql == NULL || length <= 0 || pcommand[0] == '\0' || db_mysql_ping() == false)
	{
		return false;
	}
	//clear
	if (pmysql != NULL)
	{
		MYSQL_RES* presult = NULL;
		do
		{
			presult = mysql_store_result(pmysql);
			if (presult != NULL)
			{
				mysql_free_result(presult);
			}
		} while (mysql_next_result(pmysql) == 0);
	}

	int ret = -1;
	if (real)
	{
		// 可用于包含二进制数据的查询
		ret = mysql_real_query(pmysql, pcommand, length);
	}
	else
	{
		// 不能用于包含二进制数据的查询
		ret = mysql_query(pmysql, pcommand);
	}
	if (ret != 0)
	{
		static char message[128];
		memset(message, 0, sizeof(message));
		sprintf(message, "db_mysql_execute(%d)", real);
		mysql_set_error_sentence(pmysql, 0, message);
		return false;
	}
	return true;
}

my_ulonglong db_mysql_get_insert_increment()
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return 0;
	}
	return	mysql_insert_id(pmysql);
}

my_ulonglong db_mysql_get_affected_rows()
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return 0;
	}
	return mysql_affected_rows(pmysql);
}

unsigned int db_mysql_get_result_fieldcount()
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return 0;
	}
	return mysql_field_count(pmysql);
}

MYSQL_RES* db_mysql_get_result()
{
	MYSQL_RES* presult = NULL;
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return presult;
	}
	if (mysql_field_count(pmysql) == 0)
	{
		mysql_set_error_sentence(pmysql, 0, "mysql_field_count()");
		return presult;
	}
	presult = mysql_store_result(pmysql);
	if (presult == NULL)
	{
		mysql_set_error_sentence(pmysql, 0, "mysql_store_result()");
		return presult;
	}
	return presult;
}

bool db_mysql_result_more()
{
	MYSQL * pmysql = DB_CONTENT->mysql;
	if (pmysql == NULL)
	{
		return false;
	}
	if (mysql_more_results(pmysql) == 1)
	{
		return false;
	}
	return (mysql_next_result(pmysql) == 0);
}
