
LDFLAGS = -lstdc++ -std=c++11
MYSQL_INC = -I/usr/include/mysql

all: client
clean:
	rm -f *.o *.pid client
db_wrap: db_wrap.cpp
	g++ -I/usr/include/mysql -L/usr/lib64/mysql -ansi -pedantic -Wall -c db_wrap.cpp

