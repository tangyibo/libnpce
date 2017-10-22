#!/bin/sh
#Date : 2016-11-22
#Author: tang

moduler=httpd_npce_app

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib

mkdir log  >/dev/null 2>&1
ulimit -c unlimited

echo -n `date +'%Y-%m-%d %H:%M:%S'`
echo "----Current directory is " $PWD

while :
do
	running=$(ps -ef |grep "$moduler" | grep -v "grep")
        if [ -z "$running" ] ; then
               	echo -n `date +'%Y-%m-%d %H:%M:%S'`
               	echo "----[$moduler] service was not started."
               	echo -n `date +'%Y-%m-%d %H:%M:%S'`
               	echo "----Starting [$moduler] service ."
               	mv core.* log  >/dev/null 2>&1
               	nohup ./$moduler >>log/server.log --port 7654 --threads 20 --log-level 0  2>&1 &
	fi

        sleep 1

done
