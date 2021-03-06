#!/bin/bash
root_dir=$(cd `dirname $0`; pwd)
# echo "root_dir:" ${root_dir}

echo ""
echo `pwd`
rm -rf ./project/Debug/ ./project/x64/ ./project/.vs ./project/project.opensdf ./project/project.sdf ./project/project.VC.db .vs

cd ./pbc
./clean.sh
cd ${root_dir}


cd ./cpp_17_stl_cook_book
./clear.sh
cd $root_dir

cd ./http_libevent
./clear.sh
cd $root_dir

cd ./websocket_libpp
./clear.sh
cd $root_dir

cd ./test_boost_lib
./clear.sh
cd $root_dir

clean_dir=("./3rd/templog/" "GeneralHashFunctions_-_C" "GeneralHashFunctions_-_CPP" "helloworld" "MultiplayerBook" "inicfg"
 "test" "proxy_epoll" "referlib" "cpp11" "ThreadPool" "declare" "http" "webservice" "websocket" "candlua" "card" 
 "lua-arc4random" "lua-cjson" "lua-5.3.4-src" "luasql" "NetSelect" "NetPoll" "NetEpoll" "ConnectSkynet" "ConnnectSkynet_sproto"
 "big_little_endian"
 "logger" "loggercpp" "loggersafe" "demo_php_connect_server" "NetHttp" "timer" "luafunc" "test_curl" "post_mgr" "http_client"
 "test_string" "mysql_connect" "mysql_client" "database_server" "cpp_new_features" "test_jemalloc" "test_lib" "virtual_destructor"
 "network_server" "socket-server" "network_server_socket_server" "memorypoll" "3rd/lua" "test_skynet/load_config" 
 "test_skynet/daemon" "test_skynet/databuffer_hashid" "test_skynet/harbor" "test_skynet/skynet_handle" "test_skynet/skynet_mq"
 "test_skynet/skynet_timer" "test_skynet/skynet_module" "test_skynet/snlua_bootstrap" "test_skynet/skynet_lua" 
 "test_skynet/handlemap" "test_skynet/openssl_tls" "test_skynet/skynet_socket"
 "3rd/hiredis" "redis_server" "chess_chinese" "chess_gobang" "chess_weiqi" "cpp11datalit" "cpp_template" "cpp_operator" "cpp_serialize"
 "area_of_interst" "coroutine" "tencent_libco")

clean_len=${#clean_dir[@]}

for ((i=0;i<${clean_len};i++))
do
	clear_sub=${clean_dir[$i]}
	cd ${clear_sub}
    echo ""
    echo `pwd`
    make clean
    cd ${root_dir}
done

cd $root_dir

