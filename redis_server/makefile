
TARGET := testRedis
CC := g++ -Wall
CPPFLAGS := -DNDEBUG
CFLAGS :=
CXXFLAGS := -Wall -D_FILE_OFFSET_BITS=64 -std=c++11 -I../3rd/hiredis/
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
	$(CC) $(LDFLAGS) -o $(TARGET) $(COBJS) $(CPPOBJS) ../3rd/hiredis/libhiredis.a
install:
clean:
	rm -f *.o *.so *.a *.pid core.* $(TARGET)
