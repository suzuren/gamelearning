#!/bin/sh
cur_exe_path=$(pwd)
echo "start_dbServer staring"
cd "/home/surea/depend/database_server"
./dbServer
echo "start_dbServer end"
cd ${cur_exe_path}
