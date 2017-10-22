#include "appnews.h"
#include "jsoncpp/json.h"
#include "htmlextract.h"
#include "utils.h"

int parse_appnews_netease(const string &src,const string newsid,const bool img,const bool flg,NpceResult &result)
{
	result.srcurl="";

	if(newsid.empty())
	{
		g_strLastErrorMsg="Invalid newsid for [netease app] ";
		return -1;
	}

	Json::Reader reader;
	Json::Value root;
	if(reader.parse(src.c_str(),src.c_str()+src.size(),root))
	{
		Json::Value rootDoc=root[newsid];
		if(rootDoc.isNull() || rootDoc.type()!=Json::objectValue)
		{
			g_strLastErrorMsg="Parse json failed for [netease app] when parse content";
			return -1;
		}

		const Json::Value titleValue=rootDoc["title"];
		const Json::Value pubtimeValue=rootDoc["ptime"];
		const Json::Value sourceValue=rootDoc["source"];
		const Json::Value contentValue=rootDoc["body"];

		if(!titleValue.isNull()&& titleValue.isString())
		{
			result.title=titleValue.asString();
		}

		if(!pubtimeValue.isNull() && pubtimeValue.isString())
		{
			string str=pubtimeValue.asString();
			int year=0,month=0,day=0,hour=0,minute=0,second=0;
			sscanf(str.c_str(),"%d-%d-%d %d:%d:%d",&year,&month,&day,&hour,&minute,&second);
			result.pubtime=Utils::MakeTime(year,month,day,hour,minute,second);
		}

		if(!sourceValue.isNull() && sourceValue.isString())
		{
			result.source=sourceValue.asString();
		}

		if(!contentValue.isNull() && contentValue.isString())
		{
			string content=contentValue.asString();
			html_content_context_extract(content,img,flg,result);
		}

		return 0;
	}

	g_strLastErrorMsg="Parse json failed for [netease app] when call parse()";
	return -1;
}
