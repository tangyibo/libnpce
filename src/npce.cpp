#include "npce.h"
#include <stdarg.h>
#include <stdlib.h>
#include "HtmlContentParser.h"
#include "HtmlCleaner.h"
#include "HtmlLinksParser.h"

NPCE_API npce* npce_open(const int type)
{
	CContentParser *ptr=new CContentParser(type);
	return ptr;
}

NPCE_API NpceCode npce_setopt(npce * h, NpceOpt opt, ...)
{
	NpceCode ret=NPCE_CODE_UNKOWN_ERROR;
	CContentParser *ptr=reinterpret_cast<CContentParser *>(h);
	if( ptr)
	{
		va_list arg;
		va_start(arg, opt);

		switch(opt)
		{
		case NPCEOPT_MARKED_IMG:
		{
			int val=va_arg(arg, int);
			ptr->MarkedImages(1==val);
			ret=NPCE_CODE_OK;
			break;
		}
		case NPCEOPT_LINK_ANCHOR:
		{
			int val=va_arg(arg, int);
			ptr->LinkAnchor(1==val);
			ret=NPCE_CODE_OK;
			break;
		}
		case NPCEOPT_FILE_FILTER:
		{
			const char *pram_vp=va_arg(arg, const char *);
			int val=ptr->LoadFilterFile(pram_vp);
			ret=val==0?NPCE_CODE_OK:NPCE_CODE_BAD_FUNC_PARAM;
			break;
		}
		case NPCEOPT_HTML_TIDY:
		{
			int val=va_arg(arg, int);
			ptr->TidyHtml(1==val);
			ret=NPCE_CODE_OK;
			break;
		}
		default:
			ret=NPCE_CODE_INVALID_OPTION;
			break;
		}

		va_end(arg);
	}
	return ret;
}

NPCE_API NpceCode npce_getopt(npce * h, NpceOpt opt, ...)
{
	NpceCode ret=NPCE_CODE_UNKOWN_ERROR;
	CContentParser *ptr=reinterpret_cast<CContentParser *>(h);
	if( ptr)
	{
		va_list arg;
		va_start(arg, opt);
		//void** param_vp;
		int* param_np;
		switch(opt)
		{
		case NPCEOPT_MARKED_IMG:
		{
			param_np = va_arg(arg, int*);
			*param_np=ptr->MarkedImages()?1:0;
			ret=NPCE_CODE_OK;
			break;
		}
		case NPCEOPT_LINK_ANCHOR:
		{
			param_np = va_arg(arg, int*);
			*param_np=ptr->LinkAnchor()?1:0;
			ret=NPCE_CODE_OK;
			break;
		}
		//case NPCEOPT_FILE_FILTER:
		//{
		//	param_vp=va_arg(arg, void**);
		//	*param_vp=(void*)(ptr->FilterFile().c_str());
		//	ret=NPCE_CODE_OK;
		//	break;
		//}
		default:
			ret=NPCE_CODE_INVALID_OPTION;
			break;
		}

		va_end(arg);
	}
	return ret;
}

NPCE_API NpceCode npce_perform(npce * h,const char *html,const char *url)
{
	CContentParser *ptr=reinterpret_cast<CContentParser *>(h);
	string src(html);
	if(ptr && ptr->Open(src,url))
	{
#if 0
		string::size_type pos=ptr->mResult.content.find_first_of("><");
		if(pos!=string::npos) 
		{
			ptr->TidyHtml(true);

			if(ptr->Open(src,url))
				return NPCE_CODE_OK;
		}
#else
		return NPCE_CODE_OK;
#endif
	}

	return NPCE_CODE_UNKOWN_ERROR;
}

NPCE_API NpceCode npce_result(npce * h,NpceResult &result)
{
	CContentParser *ptr=reinterpret_cast<CContentParser *>(h);
	if(ptr)
	{
		NpceResult& ret=ptr->mResult;
		result.title=ret.title;
		result.source=ret.source;
		result.srcurl=ret.srcurl;
		result.content=ret.content;
		result.pubtime=ret.pubtime;
		result.imglist=ret.imglist;
		result.linklist=ret.linklist;

		return NPCE_CODE_OK;
	}
	return NPCE_CODE_UNKOWN_ERROR;
}

NPCE_API string npce_error(npce * h)
{
	CContentParser *ptr=reinterpret_cast<CContentParser *>(h);
	if(ptr) return ptr->LastError();
	return "";
}

NPCE_API void npce_close(npce* h)
{
	CContentParser *ptr=reinterpret_cast<CContentParser *>(h);
	if(NULL!=ptr)
	{
		ptr->Close();
		delete ptr;
	}
}

/*************************************************************/

NPCE_API string npce_clean_htmltags(const char *html)
{
	return CHtmlCleaner::HtmlEraseTags(html);
}

NPCE_API NpceCode npce_html_links_wich_anchor(const char *html,const char *url,vector< pair<string,string> > &links)
{
	return CHtmlLinksParser::ParseHtmlLinks(html,url,true,links);
}

NPCE_API NpceCode npce_search_html_links(const char *html,const char *url,vector<string> &links)
{
	vector< pair<string,string> > result;
	NpceCode ret=CHtmlLinksParser::ParseHtmlLinks(html,url,false,result);
	if(0==ret && result.size()>0)
	{
		for(vector< pair<string,string> >::const_iterator it=result.begin();it!=result.end();++it)
		{
			links.push_back(it->first);
		}
	}

	return ret;
}

NPCE_API NpceCode npce_search_iframe_links(const char *html,const char *url,vector<string> &links)
{
	return CHtmlLinksParser::ParseHtmlIframeLinks(html,url,links);
}

NPCE_API NpceCode npce_html_links_wich_anchor_filter(const char *html,const char *url,const char *regex,vector< pair<string,string> > &links)
{
	return CHtmlLinksParser::ParseHtmlLinksFilter(html,url,regex,true,links);
}

NPCE_API NpceCode npce_search_html_links_filter(const char *html,const char *url,const char *regex,vector<string> &links)
{
	vector< pair<string,string> > result;
	NpceCode ret=CHtmlLinksParser::ParseHtmlLinksFilter(html,url,regex,false,result);
	if(0==ret && result.size()>0)
	{
		for(vector< pair<string,string> >::const_iterator it=result.begin();it!=result.end();++it)
		{
			links.push_back(it->first);
		}
	}

	return ret;
}
