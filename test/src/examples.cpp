/*
 * examples.cpp
 *
 *  Created: 2016年8月17日
 *   Author: tang
 */
#include "npce.h"
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
	string html;
	vector< pair<string,string> >::iterator it;

	if(loader.Download2Mem(url,html,true,50))
	{
		npce* h=npce_open(NPCE_ET_TITLE|NPCE_ET_CONTENT|NPCE_ET_PUBTIME|NPCE_ET_SOURCE|NPCE_ET_IMAGES);
		if(h)
		{
			npce_setopt(h,NPCEOPT_MARKED_IMG,1);
			npce_setopt(h,NPCEOPT_LINK_ANCHOR,0);
			npce_setopt(h,NPCEOPT_HTML_TIDY,0);
			npce_setopt(h,NPCEOPT_FILE_FILTER,"filter.txt");
		}

		//cout<<html<<endl<<"=============="<<endl;
		if(h && 0==npce_perform(h,html.c_str(),url.c_str()))
		{
			NpceResult result;
			if(0==npce_result(h,result))
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

			npce_close(h);
		}
		else
		{
				cout<<"Extract error:"<<npce_error(h)<<endl;
		}

	}
	else
		cout<<"Download error!"<<endl;

	return 0;
}



