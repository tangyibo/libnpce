#include "parseapp.h"
#include "utils.h"
#include "appnews.h"
#include <string>
using namespace std;

string g_strLastErrorMsg="";

PARSEAPP_API 
string parseapp_app_last_error()
{
	return g_strLastErrorMsg;
}

PARSEAPP_API 
AppTypeID parseapp_app_context(const string murl,string &iurl,string &newsid,ArticleFileType &format)
{
	if(!murl.empty())
	{
		newsid.clear();

		string prefix_sohu="http://3g.k.sohu.com/t/n";
		string prefix_netease="http://3g.163.com/news/";

		
		if(Utils::StartsWith(murl,prefix_sohu))
		{
			newsid=murl.substr(prefix_sohu.length());
			if(!newsid.empty())
			{
				iurl="http://api.k.sohu.com/api/news/v4/article.go?newsId="+newsid;
				format=eFileType_xml;
				return eApp_Sohu;
			}
	
			g_strLastErrorMsg="Get newsid for [sohu app] failed!";
		}
		else if(Utils::StartsWith(murl,prefix_netease))
		{
			string::size_type start_pos=murl.find_last_of("/");
			string::size_type stop_pos=murl.find(".html");
			if(stop_pos>start_pos)
			{
				newsid=murl.substr(start_pos+1,stop_pos-start_pos-1);
				if(!newsid.empty())
				{
					iurl="http://c.m.163.com/nc/article/";
					iurl.append(newsid);
					iurl.append("/full.html");
					format=eFileType_json;
					return eApp_Netease;
				}
			}
	
			 g_strLastErrorMsg="Get newsid for [netease app] failed!";
		}
		else
		{
			//.....................
		}
	}

	return eApp_NULL;
}

PARSEAPP_API
int parseapp_app_parse_content(const AppTypeID app, const string &src,const string newsid,
	const bool img,const bool flg,const ArticleFileType format,NpceResult &result)
{
	switch(app)
	{
	case eApp_Sohu:
	{
		if(format!=eFileType_xml)
		{
			g_strLastErrorMsg="Invalid format for [sohu app]";
			return -1;
		}
		else
		{
			return parse_appnews_sohu(src,newsid,img,flg,result);
		}

		break;
	}
	case eApp_Netease:
	{
		if(format!=eFileType_json)
		{
		        g_strLastErrorMsg="Invalid format for [sohu app]";
                        return -1;
                }
                else
	        {
			return parse_appnews_netease(src,newsid,img,flg,result);
	        }
            
		break;
	}
	default:
		break;
	}


	return -1;
}
