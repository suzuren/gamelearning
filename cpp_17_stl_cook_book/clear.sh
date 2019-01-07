#!/bin/bash

sub_root_dir=$(cd `dirname $0`; pwd)
echo "sub_root_dir:" ${sub_root_dir}

clean_file=("./build/CMakeFiles" "./build/cmake_install.cmake" "./build/CMakeCache.txt" "./build/Makefile" "./build/testCpp")
clean_dir=("./test" "./code")

clean_file_len=${#clean_file[@]}
clean_dir_len=${#clean_dir[@]}

for ((i=0;i<${clean_dir_len};i++))
do
	clear_sub_dir=${clean_dir[$i]}
	cd ${clear_sub_dir}
    echo ""
    echo `pwd`
    
	for ((j=0;j<${clean_file_len};j++))
	do
		clear_sub_file=${clean_file[$j]}
		#echo "clear_sub_file:" ${clear_sub_file}
		rm -rf ${clear_sub_file}
	done

    cd ${sub_root_dir}

done


