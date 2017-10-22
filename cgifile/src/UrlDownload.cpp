/*
 * CUrlDownload.cpp
 *
 *  Created on: 2015-5-11
 *      Author: tang
 */
#include "UrlDownload.h"
#include "TransCoding.h"
#include "CodeRecogntion.h"
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
	CCodeRecogtion m_Code;
	CTransCoding m_Trans;
	int ret = m_Code.getTextType(strSrc.c_str(),strSrc.length());
	int iResult = 0;
	string strResult;
	char *pResult = NULL;
	switch(ret)
	{
	case CODE_TYPE_GB:
		pResult = new char [2 * strSrc.length() + 1];
		memset(pResult,0,2 * strSrc.length() + 1);
		m_Trans.GbkToUtf8((char *)strSrc.c_str(),strSrc.length(),pResult,iResult);
		strResult = string(pResult);
		delete[] pResult;
		break;
	case CODE_TYPE_BIG5:
		pResult = new char [2 * strSrc.length() + 1];
		memset(pResult,0,2 * strSrc.length() + 1);
		m_Trans.Big5ToUtf8((char *)strSrc.c_str(),strSrc.length(),pResult,iResult);
		strResult = string(pResult);
		delete[] pResult;
		break;
	default:
		strResult = strSrc;
	}
	strSrc = strResult;
	return 0;
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

bool CUrlDownload::Download2Mem(const string &strURL,string &content,const bool bToUtf8/*=true*/,const size_t timeout_s/*=TIME_INFINIT*/ )
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
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:38.0) Gecko/20100101 Firefox/38.0");
	curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1) ;
	curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "gzip");

	if(timeout_s>0)
	{
		curl_easy_setopt(curl_handle,CURLOPT_TIMEOUT ,timeout_s);
	}

	/* get it! ,maybe timeout*/
	ret=curl_easy_perform(curl_handle);
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

bool CUrlDownload::Download2File(const string &strURL, const string &filename, const size_t timeout_s)
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
	curl_easy_setopt(curlCtx, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:38.0) Gecko/20100101 Firefox/38.0");
	curl_easy_setopt(curlCtx, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curlCtx, CURLOPT_ACCEPT_ENCODING, "gzip");

	if (timeout_s>0)
	{
		curl_easy_setopt(curlCtx, CURLOPT_TIMEOUT, timeout_s);
	}

	CURLcode ret = curl_easy_perform(curlCtx);
	if (ret) 
	{
		SetErrorMsg("Failed to download: [%s], Return Code:%d", url.c_str(),ret);
		curl_easy_cleanup(curlCtx);
		fclose(fp);
		remove(filename.c_str());
		return false;
	}

	long res_code = 0;
	curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
	if (!((res_code == 200 || res_code == 201) && ret != CURLE_ABORTED_BY_CALLBACK)) 
	{
		SetErrorMsg("Failed to download: [%s] ,Response code: %ld\n", url.c_str(), res_code);
		curl_easy_cleanup(curlCtx);
		fclose(fp);
		remove(filename.c_str());
		return false;
	}

	curl_easy_cleanup(curlCtx);
	fclose(fp);

	return true;
}
