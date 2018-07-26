

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
}

bool mysql_open()
{
	DB_CONTENT->mysql = mysql_init(DB_CONTENT->mysql);
	if (DB_CONTENT->mysql == NULL)
	{
		mysql_set_error_sentence(DB_CONTENT->mysql, 0, int len, "mysql_init()");
		return false;
	}
	mysql_options(m_pMySQL, MYSQL_OPT_CONNECT_TIMEOUT, (char*)&m_uConnectTimeout)
	if ()
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL, 0, "mysql_options(MYSQL_OPT_CONNECT_TIMEOUT)");
#endif
	}

	return true;
}
