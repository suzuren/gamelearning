
//#include "mysql.h"

struct tag_db_info
{
	//MYSQL*	pmysql;
	bool    bconnected;
	bool    breconnect;
	int     itimeout;
	tag_db_info()
	{
		Init();
	}
	void Init()
	{
		//pmysql = NULL;
		bconnected = false;
		breconnect = false;
		itimeout = 0;
	}
};

bool mysql_open(const char*host, const char*user, const char*passwd, const char*db, unsigned int port, const char*unixSocket, unsigned long clientFlag);
