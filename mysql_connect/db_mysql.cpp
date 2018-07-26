
#include "db_mysql.h"

void mysql_set_error_sentence(MYSQL* mysql, unsigned int code,int len, const char* msg)
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

void mysql_set_error_statement(MYSQL_STMT* stmt, unsigned int code, int len, const char* msg)
{
	db_exception_stmt.init();
	db_exception_stmt.code = code;
	if (msg != NULL)
	{
		strcpy(db_exception_stmt.message, msg);
	}
	if (mysql != NULL)
	{
		if (code != 0)
		{
			db_exception_stmt.code = mysql_stmt_errno(stmt);
		}
		strcpy(db_exception_stmt.error, mysql_stmt_error(stmt));
		strcpy(db_exception_stmt.state, mysql_stmt_sqlstate(stmt));
	}
}

