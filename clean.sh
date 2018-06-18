#!/bin/bash
root_dir=$(cd `dirname $0`; pwd)
# echo "root_dir:" ${root_dir}

echo ""
echo `pwd`
rm -rf ./project/Debug/ ./project/x64/ ./project/.vs ./project/project.opensdf ./project/project.sdf ./project/project.VC.db

cd ./pbc
./clean.sh
cd ${root_dir}

clean_dir=("./3rd/templog/" "GeneralHashFunctions_-_C" "GeneralHashFunctions_-_CPP" "helloworld" "MultiplayerBook" "inicfg"
 "test" "proxy_epoll" "referlib" "cpp11" "ThreadPool" "declare" "http" "webservice" "websocket" "candlua" "card" 
 "lua-arc4random" "lua-cjson" "lua-5.3.4-src" "luasql" "NetSelect" "NetPoll" "NetEpoll" "ConnectSkynet" "big_little_endian"
 "logger")

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
cd $rootdir

