
CPPFLAGS = -O3
LDFLAGS = -lrt -lpthread -lmysqlclient -lstdc++ -std=c++11
MYSQL_INC = /usr/include/mysql
MYSQL_LIB = /usr/lib64/mysql

all : client db_mysql.so db_wrap.so client.so

client: db_mysql.so db_wrap.so client.so
	g++ -g -o client db_mysql.so db_wrap.so client.so

db_mysql.so : db_mysql.cpp
	g++ -g -c -o db_mysql.so db_mysql.cpp $(LDFLAGS) -I$(MYSQL_INC) -L$(MYSQL_LIB) -lmysqlclient

db_wrap.so : db_wrap.cpp
	g++ -g -c -o db_wrap.so db_wrap.cpp db_mysql.so $(LDFLAGS) -I$(MYSQL_INC) -L$(MYSQL_LIB) -lmysqlclient

client.so : client.cpp
	g++ -g -o client.so client.cpp db_wrap.so db_mysql.so $(LDFLAGS) -I$(MYSQL_INC) -L$(MYSQL_LIB) -lmysqlclient

clean:
	rm -f *.o *.so *.pid client

