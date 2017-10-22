#!/bin/sh
eval `./process_cgi.sh $*`
echo "Content-Type: application/json;charset=utf-8"
echo       #此处必须有输出一个空行,所有 CGI 程序都有此规定  

#echo "url="${FORM_url}

if [ -z ${FORM_url} ];then
	echo "{\"status\":-1,\"errmsg\":\"No arg src\"}"
	exit 1
fi

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:./lib
./npcecurl "${FORM_url}"
