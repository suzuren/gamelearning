
MYSQL_INC = /usr/include/mysql
MYSQL_LIB = /usr/lib64/mysql

MYSQL_INC = /www/server/mysql/include/
MYSQL_LIB = /www/server/mysql/lib/

TARGET := dbServer
CC := g++ -Wall
CPPFLAGS := -DNDEBUG
CFLAGS :=
CXXFLAGS := -Wall -I$(MYSQL_INC) -D_FILE_OFFSET_BITS=64 -std=c++17
LDFLAGS := -L$(MYSQL_LIB) -lpthread -lrt -lm -lmysqlclient
CPP_SOURCES := $(wildcard *.cpp)
CPP_SOURCES += $(wildcard ../hlib/*.cpp)
CPP_SOURCES += $(wildcard ../hlib/json/*.cpp)

C_SOURCES := $(wildcard *.c)
C_SOURCES += $(wildcard ../hlib/*.c)

CPPOBJS := $(CPP_SOURCES:.cpp=.o)
COBJS := $(C_SOURCES:.c=.o)

ifeq ($(MAKECMDGOALS),embedded)
CPPFLAGS += -DEMBEDDED
endif
embedded:all
all:$(TARGET)

$(TARGET) : $(COBJS) $(CPPOBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(COBJS) $(CPPOBJS)
install:
clean:
	rm -f *.o *.so *.pid core.* $(TARGET)
