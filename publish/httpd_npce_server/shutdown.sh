#!/bin/sh
if [ -f "spid.txt" ]; then
	kill -9 $(cat spid.txt)
fi

pid=$( pgrep httpd_npce)
kill -9 $pid

