#!/bin/sh
#Date:2016-09-29
#Author:tang

protector=pronpced.sh

rm server.txt >/dev/null 2>&1 
rm spid.txt >/dev/null 2>&1

nohup $PWD/$protector >> running.log 2>&1 &
echo $! > spid.txt
