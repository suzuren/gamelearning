
TARGET := test
CC := g++ -Wall
CPPFLAGS := -DNDEBUG
CFLAGS := -std=gnu11
CXXFLAGS := -Wall -I./ -D_FILE_OFFSET_BITS=64 -std=c++11
LDFLAGS := -pthread -ldl -lrt -lm
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
	$(CC) $(LDFLAGS) -o $(TARGET) $(COBJS) $(CPPOBJS) /root/code/jemalloc/lib/libjemalloc_pic.a
install:
clean:
	rm -rf logs_* *.o *.so $(TARGET)