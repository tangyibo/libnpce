#include "ConstorAPI.h"
#include "UrlDownload.h"
#include <iostream>
#include <time.h>
#include <stdio.h>

using namespace std;
using namespace nsConstor;

int main(int argc,char *argv[])
{
	if(argc<2)
	{
		cout<<"Usage:"<<argv[0]<<" [url]"<<endl;
		return 1;
	}

	string url=argv[1];
	string html;
	CUrlDownload loader;

	if(!loader.Download2Mem(url,html,true,10))
	{
		cout<<"Download error"<<endl;
		return 1;
	}

	CONSTOR *h= Constor_Open(CONSTOR_NEWS);
	if(NULL==h)
	{
		cout<<"Constor_Open() failed!"<<endl;
		return 1;
	}

	bool ret=Constor_PerformBuf(h,html,true,1, CONSTOR_TITLE|CONSTOR_PUBLISHTIME|CONSTOR_SOURCE|CONSTOR_CONTENT);
	if(ret)
	{
		std::vector<Extr_Result> result;
		Constor_GetResult(h,result);
		Extr_Result res=result[0];
	
		cout<<"Title:"<<res.strTitle<<endl;
		cout<<"Pubtime:"<<res.DataTime.nYear<<"-"<<res.DataTime.nMonth<<"-"<<
			res.DataTime.nDay<<" "<<res.DataTime.nHour<<":"<<res.DataTime.nMinute<<":"<<res.DataTime.nSecond<<endl;
		cout<<"Source:"<<res.strSource<<endl;
		cout<<"Content:"<<endl<<res.strContent<<endl;
	}
	else
	{
		cout<<"Consor Extract failed!"<<endl;
	}

	Constor_Close(h);
}

