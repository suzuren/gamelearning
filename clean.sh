#!/bin/bash
root_dir=$(cd `dirname $0`; pwd)
# echo "root_dir:" ${root_dir}

clean_dir=("./3rd/templog/" "GeneralHashFunctions_-_C" "GeneralHashFunctions_-_CPP" "helloworld" "./MultiplayerBook/" "proxy_epoll")

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

