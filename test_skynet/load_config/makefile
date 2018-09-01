
LUA_INC := ../../3rd/lua
LUA_LIB ?= ../../3rd/lua/liblua.a

testLoadConfig : loadConfig.c
	gcc -g -Wall -o $@ $^ -lm -ldl -I$(LUA_INC) $(LUA_LIB)

clean:
	rm -rf testLoadConfig
