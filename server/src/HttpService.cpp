/*
 * HttpService.cpp
 *
 *  Created: 2016年10月21日
 *   Author: tang
 */

#include "HttpService.h"
#include "logging.hpp"
#include "strutils.hpp"
#include <iostream>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "npce.h"
#include "parseapp.h"
#include "UrlDownload.h"
#include "cJSON.h"

#define POST_LEN 1024

std::string UTC2String(const time_t &tmr)
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

	return std::string(szFormat);
}

int CHttpService::Service(struct mg_connection* conn)
{
	std::string str_post_data;

	const struct mg_request_info *ri = mg_get_request_info(conn);

	if (!strcmp(ri->uri, "/npce"))
	{
		int len;
		char buffer[POST_LEN];

		while ((len = mg_read(conn, buffer, sizeof(buffer) - 1)) > 0)
		{
			buffer[len] = 0;
			str_post_data += buffer;
		}

		DEBUG_LOG("CDATA: %s (length=%d)", str_post_data.c_str(),str_post_data.size());
		if (str_post_data.size() == 0)
		{
			WARNING_LOG("Input request is empty");
			ErrorResponse(conn, kEmptyStringError);
			return 0;
		}

		char *url = new char[str_post_data.size() + 1];
		if(NULL==url)
		{
			ERROR_LOG("Failed to allocate memory!");
                        ErrorResponse(conn, kAllocateMemoryError);
                        return 0;
		}

		char get_image[10];
		char flg_image[10];

		mg_get_var(str_post_data.c_str(), str_post_data.size(), "img", get_image,sizeof(get_image));
		mg_get_var(str_post_data.c_str(), str_post_data.size(), "flg", flg_image,sizeof(flg_image));
		mg_get_var(str_post_data.c_str(), str_post_data.size(), "url", url,str_post_data.size());

		if (strlen(url) == 0)
                {
                        WARNING_LOG("Input URL is empty");
                        ErrorResponse(conn, kEmptyStringError);
                        delete []url;
                        return 0;
                }

		char *decode_url=new char [2*strlen(url) + 1];
	 	if(NULL==decode_url)
                {
                        ERROR_LOG("Failed to allocate memory!");
                        ErrorResponse(conn, kAllocateMemoryError);
			delete []url;
                        return 0;
                }

		if(mg_url_decode(url,strlen(url),decode_url,2*strlen(url),1)<=0)
                {
                        ERROR_LOG("Failed to decode url parameter!");
                        ErrorResponse(conn, kDecodeUrlError);
			delete []url;
			delete []decode_url;
                        return 0;
                }
		
		std::string strUrl(url);
		int img=0;
		int flg=0;

		delete []decode_url;
		delete []url;
	
		// validation check
		if (strUrl.empty())
		{
			WARNING_LOG("Input URL is empty");
			ErrorResponse(conn, kEmptyStringError);
			return 0;
		}

		DEBUG_LOG("Input url is: %s", strUrl.c_str());

		if(strlen(get_image)>0 && 0==strcasecmp(get_image,"1"))
		{
			img=1;
		}
		if(strlen(flg_image)>0 && 0==strcasecmp(flg_image,"1"))
		{
			flg=1;
		}
		DEBUG_LOG("Input img is: %s,flg is : %s", get_image,flg_image);

		string strResult;
		CHttpService::Handler(strUrl,img,flg,strResult);

		TRACE_LOG("Analysis is done .[%s]",str_post_data.c_str());

		strResult = "HTTP/1.1 200 OK\r\n\r\n" + strResult;
		mg_printf(conn, "%s", strResult.c_str());
	}

	return 1;
}

void CHttpService::ErrorResponse(struct mg_connection* conn,const enum ErrorCodes& code)
{
	switch (code)
	{
	case kEmptyStringError:
	{
		std::string response = "HTTP/1.1 400 EMPTY SENTENCE\r\n\r\n";
		mg_printf(conn, "%s", response.c_str());
		break;
	}
	case kEncodingError:
	{
		// Input sentence is not clear
		std::string response = "HTTP/1.1 400 ENCODING NOT IN UTF8\r\n\r\n";
		mg_printf(conn, "%s", response.c_str());
		break;
	}
	case kDecodeUrlError:
	{
		// Failed the xml validation check
		std::string response = "HTTP/1.1 400 URL ENCODE INVALIDE\r\n\r\n";
		response += "Failed to decode url";
		mg_printf(conn, "%s", response.c_str());
		break;
	}
	case kAllocateMemoryError:
	{
		std::string response = "HTTP/1.1 400 URL DATA TOO LONG\r\n\r\n";
		response += "Url parameter data size should small than 1024";
		mg_printf(conn, "%s", response.c_str());
		break;
	}
	default:
	{
		ERROR_LOG("Unkown error catch");
		break;
	}
	}
}

void CHttpService::Handler(std::string url,int img,int flg,std::string &ret)
{
	CUrlDownload loader;
	string html;
	bool bIsOk=false;
	string errmsg="Success";
	vector< pair<string,string> >::iterator it;
	NpceResult result;


	string iurl;
	string newsid;
	ArticleFileType fmt;
	AppTypeID aid=parseapp_app_context(url,iurl,newsid,fmt);
	if(aid!=eApp_NULL)
	{
		long http_code=500;
		if(loader.Download2Mem(iurl,html,http_code,false,true,50))
		{
			if(0==parseapp_app_parse_content(aid,html,newsid,img,flg,fmt,result))	
				bIsOk=true;
			else
				errmsg=parseapp_app_last_error();
		}
		else
		{
			errmsg="Download url ["+url+"] error!";
		}
	}
	else
	{
		bool use_mobile_useraget=false;
		string zaker_prefix="http://iphone.myzaker.com/l.php?l=";
		if(utils::startswith(url,zaker_prefix ))
			use_mobile_useraget=true;

		long http_code=501;
		if(loader.Download2Mem(url,html,http_code,use_mobile_useraget,true,50) && 200==http_code)
		{
			npce* h=NULL;
			if(1==img)
			{
				h=npce_open(NPCE_ET_TITLE|NPCE_ET_CONTENT|NPCE_ET_PUBTIME|NPCE_ET_SOURCE|NPCE_ET_IMAGES);
			}
			else
			{
				h=npce_open(NPCE_ET_TITLE|NPCE_ET_CONTENT|NPCE_ET_PUBTIME|NPCE_ET_SOURCE);
			}

			if(h)
			{
				npce_setopt(h,NPCEOPT_MARKED_IMG,flg);
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
			errmsg="Download url ["+url+"] error,or Response http code:"+utils::to_str(http_code);
		}
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
		
		if(1==img)
		{
			cJSON *imgeArr = cJSON_CreateArray();
			cJSON_AddItemToObject(docTmp,"images",imgeArr);

			int index=0;
			vector< pair<string,string> >::iterator it;
			for(it=result.imglist.begin();it!=result.imglist.end();++it)
			{
				cJSON *imgTmp = cJSON_CreateObject();
				cJSON_AddItemToArray(imgeArr,imgTmp);
				cJSON_AddItemToObject(imgTmp,"img_id",cJSON_CreateNumber(++index));
				cJSON_AddItemToObject(imgTmp,"img_src",cJSON_CreateString(it->first.c_str()));
			}
		}	
	}
	else
	{
		cJSON_AddItemToObject(json,"status",cJSON_CreateNumber(-1));
		cJSON_AddItemToObject(json,"errmsg",cJSON_CreateString(errmsg.c_str()));
	}

	//print to client
	//char *strJson=cJSON_PrintUnformatted(json);
	char *strJson = cJSON_Print(json);
	cJSON_Delete(json);
	ret.append(strJson);
	free(strJson);
}
