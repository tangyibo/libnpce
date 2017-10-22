#include "appnews.h"
#include "jsoncpp/json.h"
#include "htmlextract.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>

bool appnews_title_sohu(const string &xml,NpceResult &result)
{
	string::size_type start=xml.find("<title>");
	if(start==string::npos)
		return false;
	
	string::size_type end=xml.find("</title>",start);
	if(end==string::npos)
		return false;

	string strTitle=xml.substr(start+7,end-start-7);
        start=strTitle.find("<![CDATA[");
        if(0==start)
        {
                 strTitle=strTitle.substr(start+9);
        }
        end=strTitle.find("]]>",start);
        if(end==strTitle.length()-3)
        {
                strTitle=strTitle.substr(0,end);
        }
	result.title=strTitle;
	return true;
}

bool appnews_source_sohu(const string &xml,NpceResult &result)
{
         string::size_type start=xml.find("<from>");
         if(start==string::npos)
                 return false;
 
         string::size_type end=xml.find("</from>",start);
         if(end==string::npos)
                 return false;
 
         result.source=xml.substr(start+6,end-start-6);
                 
         return true;
}

bool appnews_pubtime_sohu(const string &xml,NpceResult &result)
{
	string::size_type start=xml.find("<time>");
	if(start==string::npos)
		return false;
	
	string::size_type end=xml.find("</time>",start);
	if(end==string::npos)
		return false;

	string str=xml.substr(start+6,end-start-6);
	int year=0,month=0,day=0,hour=0,minute=0,second=0;
	sscanf(str.c_str(),"%d/%d/%d %d:%d",&year,&month,&day,&hour,&minute);
	result.pubtime=Utils::MakeTime(year,month,day,hour,minute,second);

	return true;
}

bool appnews_content_sohu(const string &xml,const bool img,const bool flg,NpceResult &result)
{
	string::size_type start=xml.find("<content>");
	if(start==string::npos)
		return false;

	string::size_type end=xml.find("</content>",start);
	if(end==string::npos)
		return false;

	string strContent=xml.substr(start+9,end-start-9);
	start=strContent.find("<![CDATA[");
	if(0==start)
	{
		strContent=strContent.substr(start+9);
	}
	end=strContent.find("]]>");
	if(end==strContent.length()-3)
	{
		strContent=strContent.substr(0,end);
	}

	return html_content_context_extract(strContent,img,flg,result);
	//result.content=strContent;
}

int parse_appnews_sohu(const string &src,const string newsid,const bool img,const bool flg,NpceResult &result)
{
	result.srcurl="";

	appnews_title_sohu(src,result);
	appnews_pubtime_sohu(src,result);
	appnews_source_sohu(src,result);
	appnews_content_sohu(src,img,flg,result);
	result.content=Utils::ReplaceAll(result.content,"本内容由搜狗推荐引擎推荐, 点击查看原文","");
	return 0;
}
