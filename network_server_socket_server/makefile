

SOCKET_SERVER_PATH=./lib_socketserver
LIB_SOCKET_SERVER=./lib_socketserver/libsocket_server.a

TARGET := netServer
CC := g++ -Wall
CPPFLAGS := -DNDEBUG
CFLAGS :=
CXXFLAGS := -I$(SOCKET_SERVER_PATH) -Wall -D_FILE_OFFSET_BITS=64 -std=c++11
LDFLAGS := -lpthread -lrt -lm
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
	make -C $(SOCKET_SERVER_PATH)
	$(CC) $(LDFLAGS) -o $(TARGET) $(COBJS) $(CPPOBJS) -L$(SOCKET_SERVER_PATH) $(LIB_SOCKET_SERVER)
install:
clean:
	make -C $(SOCKET_SERVER_PATH) clean
	rm -f *.o *.so *.a *.pid core.* $(TARGET)
