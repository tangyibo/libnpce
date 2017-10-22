/*
 * examples.cpp
 *
 *  Created: 2016年8月17日
 *   Author: tang
 */
#include "parseapp.h"
#include "UrlDownload.h"
#include <iostream>
#include <time.h>
#include <stdio.h>
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
		cout<<"Usage:"<<argv[0]<<" [url]"<<endl;
		return 1;
	}

	string url=argv[1];
	CUrlDownload loader;
	string iurl;
	string newsid;
	ArticleFileType fmt;
	string html;
	vector< pair<string,string> >::iterator it;

	AppTypeID aid=parseapp_app_context(url,iurl,newsid,fmt);
	if(aid!=eApp_NULL)
	{
		if(loader.Download2Mem(iurl,html,true,50))
		{
			NpceResult result;
			if(0==parseapp_app_parse_content(aid,html,newsid,true,true,fmt,result))	
			{
				//cout<<"URL:"<<url<<endl;
				cout<<"Title:"<<result.title<<endl;
				cout<<"Source:"<<result.source<<endl;
				cout<<"SrcUrl:"<<result.srcurl<<endl;
				cout<<"Pubtime:"<<UTC2String(result.pubtime)<<endl;
				cout<<"Content:"<<endl<<result.content<<endl;
				cout<<"Images:"<<endl;

				int index=0;
				vector< pair<string,string> >::iterator it;
				for(it=result.imglist.begin();it!=result.imglist.end();++it)
				{
					cout<<"["<<++index<<"]"<<it->first<<endl;
				}
			}
			else
			{
				cout<<"Extract error:"<<parseapp_app_last_error()<<endl;
			}
		}
		else
		{
				cout<<"Download error!"<<endl;
		}

	}
	else
		cout<<"Unsupport app url!"<<endl;

	return 0;
}



