
#ifndef __DB_MYSQL_H__
#define __DB_MYSQL_H__

#define MAX_ERROR_MESSAGE_SIZE 512

static struct tag_db_exception db_exception_sent;
static struct tag_db_exception db_exception_stmt;

struct tag_db_exception
{
	unsigned int	code;
	char			error[MAX_ERROR_MESSAGE_SIZE];
	char			state[MAX_ERROR_MESSAGE_SIZE];
	char			message[MAX_ERROR_MESSAGE_SIZE];
	tag_db_exception()
	{
		init();
	}
	void init()
	{
		code = 0;
		memset(error, 0, sizeof(error));
		memset(state, 0, sizeof(state));
		memset(message, 0, sizeof(message));
	}
};

void mysql_set_error_sentence(MYSQL* mysql, unsigned int code, int len, const char* msg);
void mysql_set_error_statement(MYSQL_STMT* stmt, unsigned int code, int len, const char* msg);



#endif



