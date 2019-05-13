#!/bin/bash

rm -rf CMakeFiles CMakeCache.txt cmake_install.cmake Makefile

cmake ./
while getopts rR opt
do
	case $opt in
		r)
			make clean
			;;
		R)
			make clean
			;;
		*)
			;;
	esac
done
make -j 4
