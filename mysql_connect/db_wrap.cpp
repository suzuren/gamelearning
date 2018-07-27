
#include "db_wrap.h"
#include "db_mysql.h"

bool mysql_init()
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

}

bool mysql_open()
{
	DB_CONTENT->mysql = mysql_init(DB_CONTENT->mysql);
	if (DB_CONTENT->mysql == NULL)
	{
		mysql_set_error_sentence(DB_CONTENT->mysql, 0, "mysql_init()");
		return false;
	}
	int ret = mysql_options(DB_CONTENT->mysql, MYSQL_OPT_CONNECT_TIMEOUT, (char*)&DB_CONTENT->timeout);
	if (ret != 0)
	{
		mysql_set_error_sentence(DB_CONTENT->mysql, 0, "mysql_options(MYSQL_OPT_CONNECT_TIMEOUT)");
		return false;
	}
	unsigned int timeout = 10;
	ret = mysql_options(DB_CONTENT->mysql, MYSQL_OPT_READ_TIMEOUT, (char*)&timeout);
	if (ret != 0)
	{
		mysql_set_error_sentence(DB_CONTENT->mysql, 0, "mysql_options(MYSQL_OPT_CONNECT_TIMEOUT)");
		return false;
	}
	ret = mysql_options(DB_CONTENT->mysql, MYSQL_OPT_WRITE_TIMEOUT, (char*)&timeout);
	if (ret != 0)
	{
		mysql_set_error_sentence(DB_CONTENT->mysql, 0, "mysql_options(MYSQL_OPT_CONNECT_TIMEOUT)");
		return false;
	}
	ret = mysql_options(DB_CONTENT->mysql, MYSQL_OPT_RECONNECT, &DB_CONTENT->reconnect);
	if (ret != 0)
	{
		mysql_set_error_sentence(DB_CONTENT->mysql, 0, "mysql_options(MYSQL_OPT_RECONNECT)");
	}
	if (!DB_CONTENT->charset[0])
	{
		sprintf(DB_CONTENT->charset, "%s", "utf8");
	}
	ret = mysql_options(DB_CONTENT->mysql, MYSQL_SET_CHARSET_NAME, DB_CONTENT->charset);
	if (ret != 0)
	{
		mysql_set_error_sentence(DB_CONTENT->mysql, 0, "mysql_options(MYSQL_SET_CHARSET_NAME)");
	}
	return true;
}
