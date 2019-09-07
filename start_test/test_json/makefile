

TARGET := test
CC := g++ -Wall -g
CPPFLAGS := -DNDEBUG -g
CFLAGS := -g
CXXFLAGS := -Wall -D_FILE_OFFSET_BITS=64 -std=c++17 -g
LDFLAGS := -lpthread -lrt -lm -g
CPP_SOURCES := $(wildcard *.cpp)

C_SOURCES := $(wildcard *.c)

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
	rm -f *.o *.so core.* $(TARGET)

