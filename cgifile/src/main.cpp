/*
 * examples.cpp
 *
 *  Created: 2016年8月17日
 *   Author: tang
 */
#include "npce.h"
#include "UrlDownload.h"
#include "base64.h"
#include <iostream>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cJSON.h"

using namespace std;

string UTC2String(const time_t &tmr)
{
	struct tm *t=localtime(&tmr);
	char szFormat[128];
	sprintf(szFormat,"%d-%d-%d %d:%d:%d",
			t->tm_year+1900 ,
			t->tm_mon+1,
			t->tm_mday,
			t->tm_hour,
			t->tm_min,
			t->tm_sec);

	return string(szFormat);
}

int main(int argc,char *argv[])
{
	if(argc<2)
	{
		cout<<"Usage:"<<argv[0]<<" [url->base64 code]"<<endl;
		return 1;
	}
	
	string url=Base64::decode64(argv[1]);
	CUrlDownload loader;
	string html;
	bool bIsOk=false;
	string errmsg="Sucesss";
	vector< pair<string,string> >::iterator it;
	NpceResult result;

	if(loader.Download2Mem(url,html,true,50))
	{
		npce* h=npce_open(NPCE_ET_TITLE|NPCE_ET_CONTENT|NPCE_ET_PUBTIME|NPCE_ET_SOURCE);
		if(h)
		{
			npce_setopt(h,NPCEOPT_MARKED_IMG,0);
			npce_setopt(h,NPCEOPT_LINK_ANCHOR,0);
			//npce_setopt(h,NPCEOPT_FILE_FILTER,"filter.txt");
		}

		if(h && 0==npce_perform(h,html.c_str(),url.c_str()))
		{
			if(0==npce_result(h,result))
			{
				bIsOk=true;
			}

			npce_close(h);
		}
		else
		{
			errmsg=npce_error(h);
		}

	}
	else
	{
		errmsg="Download url ["+url+"] error!";
	}

	//generate json format result
	cJSON *json = cJSON_CreateObject();
	if(bIsOk)
	{
		cJSON_AddItemToObject(json,"status",cJSON_CreateNumber(0));
		cJSON_AddItemToObject(json,"errmsg",cJSON_CreateString(errmsg.c_str()));

		cJSON *docTmp = cJSON_CreateObject();
		cJSON_AddItemToObject(json,"doc",docTmp);
		cJSON_AddItemToObject(docTmp,"url",cJSON_CreateString(url.c_str()));
		cJSON_AddItemToObject(docTmp,"title",cJSON_CreateString(result.title.c_str()));
		cJSON_AddItemToObject(docTmp,"pubtime",cJSON_CreateString(UTC2String(result.pubtime).c_str()));
		cJSON_AddItemToObject(docTmp,"source",cJSON_CreateString(result.source.c_str()));
		cJSON_AddItemToObject(docTmp,"content",cJSON_CreateString(result.content.c_str()));
	}
	else
	{
		cJSON_AddItemToObject(json,"status",cJSON_CreateNumber(-1));
		cJSON_AddItemToObject(json,"errmsg",cJSON_CreateString(errmsg.c_str()));
	}

	//print to client
	char *strJson = cJSON_Print(json);
	cJSON_Delete(json);
	//cout<<"Content-Length: "<<(int)strlen(strJson)<<"\r\n";
	//cout <<"Content-Type: application/json;charset=utf-8\r\n\r\n";
	cout<<strJson<<endl;
	free(strJson);


	return 0;
}



