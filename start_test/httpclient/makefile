

TARGET := testHttp
CC := g++ -Wall
CPPFLAGS := -DNDEBUG
CFLAGS :=
CXXFLAGS := -Wall -D_FILE_OFFSET_BITS=64 -std=c++17
LDFLAGS := -lpthread -lrt -lm
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

