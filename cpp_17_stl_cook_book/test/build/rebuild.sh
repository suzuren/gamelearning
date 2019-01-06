#!/bin/bash

rm -f CMakeCache.txt

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
