
CPPFLAGS = -O3
LDFLAGS = -lrt -lpthread -lmysqlclient -lstdc++ -std=c++11
MYSQL_INC = /usr/include/mysql
MYSQL_LIB = /usr/lib64/mysql


client: db_wrap.so client.so
	g++ -g -o client db_wrap.so client.so

db_wrap.so : db_wrap.cpp
	g++ -g -c -o db_wrap.so db_wrap.cpp $(LDFLAGS) -I$(MYSQL_INC) -L$(MYSQL_LIB)

client.so: client.cpp
	g++ -g -c -o client.so client.cpp db_wrap.so $(LDFLAGS) -I$(MYSQL_INC) -L$(MYSQL_LIB)

clean:
	rm -f *.o *.so *.pid client

