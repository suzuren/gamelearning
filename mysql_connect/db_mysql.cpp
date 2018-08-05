
#include "db_mysql.h"


static struct tag_db_exception db_exception_sent;
static struct tag_db_exception db_exception_stmt;


void mysql_set_error_sentence(MYSQL* mysql, unsigned int code, const char* msg)
{
	db_exception_sent.init();
	db_exception_sent.code = code;
	if (msg != NULL)
	{
		strcpy(db_exception_sent.message, msg);
	}
	if (mysql != NULL)
	{
		if (code != 0)
		{
			db_exception_sent.code = mysql_errno(mysql);
		}
		strcpy(db_exception_sent.error, mysql_error(mysql));
		strcpy(db_exception_sent.state, mysql_sqlstate(mysql));
	}
}

void mysql_set_error_statement(MYSQL_STMT* stmt, unsigned int code, const char* msg)
{
	db_exception_stmt.init();
	db_exception_stmt.code = code;
	if (msg != NULL)
	{
		strcpy(db_exception_stmt.message, msg);
	}
	if (stmt != NULL)
	{
		if (code != 0)
		{
			db_exception_stmt.code = mysql_stmt_errno(stmt);
		}
		strcpy(db_exception_stmt.error, mysql_stmt_error(stmt));
		strcpy(db_exception_stmt.state, mysql_stmt_sqlstate(stmt));
	}
}

void mysql_get_error_message()
{
	if (db_exception_sent.code != 0)
	{
		printf("sent error - code:%d\n", db_exception_sent.code);
		printf("sent error - mess:%s\n", db_exception_sent.message);
		printf("sent error - erro:%s\n", db_exception_sent.error);
		printf("sent error - stat:%s\n", db_exception_sent.state);
	}
	if (db_exception_stmt.code != 0)
	{
		printf("stmt error - code:%d\n", db_exception_stmt.code);
		printf("stmt error - mess:%s\n", db_exception_stmt.message);
		printf("stmt error - erro:%s\n", db_exception_stmt.error);
		printf("stmt error - stat:%s\n", db_exception_stmt.state);
	}
}





