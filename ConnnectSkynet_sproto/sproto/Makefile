.PHONY : all win clean

all : linux clientsproto lpeg.so 
win : sproto.dll

# For Linux
linux:
	make sproto.so "DLLFLAGS = -shared -fPIC"
# For Mac OS
macosx:
	make sproto.so "DLLFLAGS = -bundle -undefined dynamic_lookup"

sproto.so : sproto.c lsproto.c
	env gcc -O2 -Wall $(LUA_INC) $(DLLFLAGS) -o $@ $^

sproto.dll : sproto.c lsproto.c
	gcc -O2 -Wall --shared -o $@ $^ -I/usr/local/include -L/usr/local/bin -llua53

clientsproto: testsproto.c sproto.c
	gcc -g -O2 -Wall $^ -o $@

lpeg.so : ../lpeg/lpcap.c ../lpeg/lpcode.c ../lpeg/lpprint.c ../lpeg/lptree.c ../lpeg/lpvm.c
	gcc -g -O2 -Wall -I/usr/local/lua/include -fPIC --shared -I../lpeg $^ -o $@ 

clean :
	rm -f sproto.so sproto.dll clientsproto lpeg.so
