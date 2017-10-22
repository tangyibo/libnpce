#include "npce.h"
#include "parseapp.h"
#include "UrlDownload.h"
#include "cJSON.h"
#include "codeRecogntion.h"
#include "TransCoding.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <Python.h>


static int g_tidy=0;

static std::string UTC2String(const time_t &tmr)
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

int CodeConv(string& strSrc)
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

string  reporterrmsg(const string errmsg)
{
	cJSON *json = cJSON_CreateObject();
	cJSON_AddItemToObject(json,"status",cJSON_CreateNumber(-1));
        cJSON_AddItemToObject(json,"errmsg",cJSON_CreateString(errmsg.c_str()));

        char *strJson = cJSON_Print(json);
        cJSON_Delete(json);

        return string(strJson);
}

string  extract(const char *url,const char *html)
{
	string errmsg="Success";
	NpceResult result;
	bool bIsOk=false;

	if(NULL==url || NULL==html)
		return reporterrmsg("URL or HTML is empty!");
	
	if(strlen(url)<1 || strlen(html)<1)
		return reporterrmsg("Invalid URL or HTML string length");
	
	string strUrl(url);
	string strHtml(html);

	if(0!=CodeConv(strUrl) || 0!=CodeConv(strHtml))
	{
		return reporterrmsg("Code convert failed!");
	}


	string iurl;
	string newsid;
	ArticleFileType fmt;
	AppTypeID aid=parseapp_app_context(strUrl,iurl,newsid,fmt);
	if(aid!=eApp_NULL)
	{
		CUrlDownload loader;
		long http_code=500;
		size_t tmout=60;
		string fmtpage;
		if(loader.Download2Mem(iurl,fmtpage,http_code,true,false,tmout))
		{
			if(0==parseapp_app_parse_content(aid,fmtpage,newsid,true,true,fmt,result))
			{
				bIsOk=true;
			}
			else
				errmsg=parseapp_app_last_error();
		}
	}
	
	if(eApp_NULL==aid || false==bIsOk)
	{
		npce* h=NULL;
		h=npce_open(NPCE_ET_TITLE|NPCE_ET_CONTENT|NPCE_ET_PUBTIME|NPCE_ET_SOURCE|NPCE_ET_IMAGES);
		if(NULL==h)
			return NULL;

		npce_setopt(h,NPCEOPT_MARKED_IMG,1);
		npce_setopt(h,NPCEOPT_LINK_ANCHOR,0);	
		npce_setopt(h,NPCEOPT_HTML_TIDY,g_tidy);
		if(0==npce_perform(h,strHtml.c_str(),strUrl.c_str()))
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
	
	cJSON *json = cJSON_CreateObject();
         if(bIsOk)
         {
                 cJSON_AddItemToObject(json,"status",cJSON_CreateNumber(0));
                 cJSON_AddItemToObject(json,"errmsg",cJSON_CreateString(errmsg.c_str()));

                 cJSON *docTmp = cJSON_CreateObject();
                 cJSON_AddItemToObject(json,"doc",docTmp);
                 cJSON_AddItemToObject(docTmp,"url",cJSON_CreateString(url));
                 cJSON_AddItemToObject(docTmp,"title",cJSON_CreateString(result.title.c_str()));
                 cJSON_AddItemToObject(docTmp,"pubtime",cJSON_CreateString(UTC2String(result.pubtime).c_str()));
                 cJSON_AddItemToObject(docTmp,"source",cJSON_CreateString(result.source.c_str()));
                 cJSON_AddItemToObject(docTmp,"content",cJSON_CreateString(result.content.c_str()));
 
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
	else
	{
		 cJSON_AddItemToObject(json,"status",cJSON_CreateNumber(-1));
                 cJSON_AddItemToObject(json,"errmsg",cJSON_CreateString(errmsg.c_str()));
	}
	
	char *strJson = cJSON_Print(json);
	cJSON_Delete(json);

	return string(strJson);
}

PyObject* wrap_extract(PyObject* self, PyObject* args)
{
	char * url;
	char * page;

	if(!PyArg_ParseTuple(args, "s|s", &url,&page))
		return NULL;

	string ret=extract(url,page);
	return Py_BuildValue("s", ret.c_str());
}	

static PyMethodDef libgolaxynpceMethods[] =
{
  {"extract", wrap_extract, METH_VARARGS},
  {NULL, NULL}
};

extern "C" __attribute__ ((visibility("default")))
void initlibgolaxynpce()
{
  PyObject* m;
  m = Py_InitModule("libgolaxynpce", libgolaxynpceMethods);
}

