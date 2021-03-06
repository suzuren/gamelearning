#!/bin/bash
root_dir=$(cd `dirname $0`; pwd)
# echo "root_dir:" ${root_dir}

echo ""
echo `pwd`
rm -rf ./project/Debug/ ./project/x64/ ./project/.vs ./project/project.opensdf ./project/project.sdf ./project/project.VC.db

clean_dir=("./" "./binding/lua" "./binding/lua53")

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

