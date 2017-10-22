/*
 * CUrlDownload.cpp
 *
 *  Created on: 2015-5-11
 *      Author: tang
 */
#include "UrlDownload.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////
int CUrlDownload::WriteMemoryCallback(char *data, size_t size, size_t nmemb, void *stream)
{
	string *pResult = (string*)stream;
	pResult->append(data, size*nmemb);
	return int(size*nmemb);
}

size_t CUrlDownload::WriteFileCallback(void *ptr, size_t size, size_t nmemb, void* userdata)
{
	FILE* stream = (FILE*)userdata;
	if (!stream) 
		return 0;

	size_t written = fwrite((FILE*)ptr, size, nmemb, stream);
	return written;
}

void CUrlDownload::SetErrorMsg(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	char szBuffer[1024];
	vsprintf(szBuffer, format, args);
	va_end(args);

	mErrorMsg.assign(szBuffer);
}

int CUrlDownload::CodeConv(string& strSrc)
{
	return CodeConv(strSrc);
	//return codeconv_text_convert(strSrc);
}

///////////////////////////////////////////////////////////////////////////


CUrlDownload::CUrlDownload()
{
	curl_global_init(CURL_GLOBAL_ALL);
}

CUrlDownload::~CUrlDownload()
{
	curl_global_cleanup();
}

bool CUrlDownload::Download2Mem(const string &strURL,string &content,long &http_code,const bool use_mobile_useraget/*=false*/,const bool bToUtf8/*=true*/,const size_t timeout_s/*=TIME_INFINIT*/ )
{
	CURL *curl_handle=NULL;
	CURLcode ret=CURLE_OK;
	string url = strURL;

	content.clear();

	curl_handle = curl_easy_init();
	if (NULL == curl_handle)
	{
		SetErrorMsg("Failed to download page, error location :  curl_easy_init()");
		return false;
	}
		
	curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &CUrlDownload::WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA,&content);
	if(use_mobile_useraget)
	{
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (iPhone; CPU iPhone OS 9_1 like Mac OS X) AppleWebKit/601.1.46 (KHTML, like Gecko) Version/9.0 Mobile/13B143 Safari/601.1");
	}
	else
	{
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:38.0) Gecko/20100101 Firefox/38.0");
	}
	curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1) ;
	curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "gzip");

	if(timeout_s>0)
	{
		curl_easy_setopt(curl_handle,CURLOPT_TIMEOUT ,timeout_s);
	}

	curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 3); //设置重定向的最大次数
	curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);//设置301、302跳转跟随location

	/* get it! ,maybe timeout*/
#if 1
	ret=curl_easy_perform(curl_handle);
	if(CURLE_OK==ret)
	{
		curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
	}
#else
	int try_count=3;
	do
	{
		ret=curl_easy_perform(curl_handle);
		if(CURLE_OK==ret)
		{
   			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
			if(200==http_code||404==http_code)
				break;
		}
		else 
		{
			try_count--; 
			usleep(5);
		}
	}while(try_count>=0);
#endif

	if(ret!=CURLE_OK)
	{
                 SetErrorMsg("Failed to download url : [%s], Return Code:%d ",url.c_str(),ret);
                 curl_easy_cleanup(curl_handle);
                 return false;
        }
	curl_easy_cleanup(curl_handle);

	if (content.size()>0)
	{
		if(bToUtf8) CodeConv(content);

		return true;
	}

	SetErrorMsg("Download page content is empty!");

	return false;
}

bool CUrlDownload::Download2File(const string &strURL, const string &filename,long &http_code,const bool use_mobile_useraget/*=false*/, const size_t timeout_s)
{
	string url = strURL;
	FILE* fp = fopen(filename.c_str(), "wb");
	if (!fp)
	{
		SetErrorMsg("Failed to Create file %s on the disk", filename.c_str());
		return false;
	}

	CURL* curlCtx = curl_easy_init();
	if (NULL == curlCtx)
	{
		fclose(fp);
		remove(filename.c_str());
		SetErrorMsg("Failed to download Error :  curl_easy_init()");
		return false;
	}

	curl_easy_setopt(curlCtx, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, WriteFileCallback);
	curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);
	if(use_mobile_useraget)
	{
		curl_easy_setopt(curlCtx, CURLOPT_USERAGENT, "Mozilla/5.0 (iPhone; CPU iPhone OS 9_1 like Mac OS X) AppleWebKit/601.1.46 (KHTML, like Gecko) Version/9.0 Mobile/13B143 Safari/601.1");
	}
	else
	{
		curl_easy_setopt(curlCtx, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:38.0) Gecko/20100101 Firefox/38.0");
	}
	curl_easy_setopt(curlCtx, CURLOPT_NOSIGNAL, 1);
	//curl_easy_setopt(curlCtx, CURLOPT_ACCEPT_ENCODING, "gzip");

	if (timeout_s>0)
	{
		curl_easy_setopt(curlCtx, CURLOPT_TIMEOUT, timeout_s);
	}

	CURLcode ret = curl_easy_perform(curlCtx);
	if (CURLE_OK!=ret) 
	{
		SetErrorMsg("Failed to download: [%s], Return Code:%d", url.c_str(),ret);
		curl_easy_cleanup(curlCtx);
		fclose(fp);
		remove(filename.c_str());
		return false;
	}

	curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &http_code);
	if (!((http_code == 200 || http_code == 201) && ret != CURLE_ABORTED_BY_CALLBACK)) 
	{
		SetErrorMsg("Failed to download: [%s] ,Response code: %ld\n", url.c_str(), http_code);
		curl_easy_cleanup(curlCtx);
		fclose(fp);
		remove(filename.c_str());
		return false;
	}

	curl_easy_cleanup(curlCtx);
	fclose(fp);

	return true;
}
