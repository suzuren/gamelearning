

INC =  -I./ -I../socketserver -I../
DIR = ../../../library/

BINARY = libsocketaide.a
BASEOBJS = $(patsubst ./%.cpp, ./%.o, $(wildcard ./*.cpp))
CXXFLAGS := -std=c++17

.PHONY: all clean

all: $(BINARY)
	cp $(BINARY) $(DIR)

$(BINARY): $(BASEOBJS)
	ar -cvr $@ $^

clean:
	rm -f $(BASEOBJS) $(BINARY) $(DIR)$(BINARY)

%.o: %.cpp
	g++ $(CXXFLAGS) -g -fPIC $(INC) -o $@ -c $<

