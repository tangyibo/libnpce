#include "utils/HttpURL.h"
#include "utils/TimerClock.h"
#include "utils/UtilFun.h"
#include "regexapi/regexhelper.h"
#include "HtmlTimeParser.h"
#include "HtmlSourceParser.h"
#include "HtmlCleaner.h"
#include "JaccardSimilarity.hpp"
#include <unistd.h>
#include <cstring>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <assert.h>
#include "HtmlContentParser.h"

using namespace std;
using namespace htmlcxx;

typedef struct
{
	const char *tag;		//tag name in html language
	int rm_tag_label;		//indicate tag that need erase tag label
	int rm_tag_node;		//indicate tag that need remove itself and it's child
	int is_content;		//indicate tag that maybe contain passage content
	int is_nextline;		//indicate tag that need insert '\n' to next line
}HtmlTagContext;

typedef enum {
	eTag_RM_TAG=10,
	eTag_RM_NODE,
	eTag_IS_CTT,
	eTAG_IS_NTL,
}HtmlTagOpt;

#define MAGIC_CHARACTER "Ю"
#define ARRAY_LENGTH(arr)  ((int)sizeof(arr)/(int)sizeof(HtmlTagContext))

HtmlTagContext g_tagNameTable[]={
		//tag        remove label     remove node    is content      is next line
		{"a",			0,					0,					1,						0		},
		{"abbr",		1,					0,					1,						0		},
		{"acronym",		1,					0,					1,						0		},
		{"address",		1,					0,					1,						0		},
		{"applet",		0,					1,					0,						0		},
		{"area",		0,					1,					0,						0		},
		{"article",		1,					0,					1,						0		},
		{"aside",		1,					0,					1,						0		},
		{"audio",		0,					1,					0,						0		},
		{"b",			1,					0,					1,						0		},
		{"base",		1,					0,					0,						0		},
		{"basefont",	1,					0,					0,						0		},
		{"bdi",			1,					0,					1,						0		},
		{"bdo",			0,					0,					1,						0		},
		{"big",			1,					0,					1,						0		},
		{"blockquote",	1,					0,					1,						0		},
		{"body",		0,					0,					1,						0		},
		{"br",			0,					0,					0,						1		},
		{"button",		1,					0,					0,						0		},
		{"canvas",		1,					0,					1,						0		},
		{"caption",		1,					0,					1,						0		},
		{"center",		1,					0,					1,						0		},
		{"cite",		1,					0,					1,						0		},
		{"code",		1,					0,					1,						0		},
		{"col",			1,					0,					1,						0		},
		{"colgroup",	1,					0,					1,						0		},
		{"command",		0,					1,					0,						0		},
		{"datalist",	0,					1,					0,						0		},
		{"dd",			1,					0,					1,						0		},
		{"del",			1,					0,					1,						0		},
		{"details",		1,					0,					1,						0		},
		{"dfn",			1,					0,					1,						0		},
		{"dialog",		1,					0,					1,						0		},
		{"dir",			1,					0,					1,						0		},
		{"div",			0,					0,					1,						0		},
		{"dl",			1,					0,					1,						0		},
		{"dt",			1,					0,					1,						0		},
		{"em",			1,					0,					1,						0		},
		{"embed",		1,					0,					0,						0		},
		{"fieldset",	1,					0,					1,						0		},
		{"figcaption",	1,					0,					1,						0		},
		{"figure",		1,					0,					1,						0		},
		{"font",		1,					0,					1,						0		},
		{"footer",		1,					0,					0,						0		},
		{"form",		0,					1,					0,						0		},
		{"frame",		1,					0,					0,						0		},
		{"frameset",	0,					1,					0,						0		},
		{"h1",			0,					0,					1,						1		},
		{"h2",			0,					0,					1,						1		},
		{"h3",			0,					0,					1,						1		},
		{"h4",			0,					0,					1,						1		},
		{"h5",			0,					0,					1,						1		},
		{"h6",			0,					0,					1,						1		},
		{"head",		0,					0,					0,						0		},
		{"header",		0,					0,					0,						0		},
		{"hr",			0,					0,					0,						1		},
		{"html",		0,					0,					0,						0		},
		{"i",			1,					0,					1,						0		},
		{"iframe",		1,					0,					0,						0		},
		{"img",			0,					0,					0,						0		},
		{"input",		1,					0,					0,						0		},
		{"ins",			1,					0,					1,						0		},
		{"kbd",			1,					0,					1,						0		},
		{"kbd",			1,					0,					1,						0		},
		{"keygen",		1,					0,					0,						0		},
		{"label",		1,					0,					1,						0		},
		{"legend",		1,					0,					0,						0		},
		{"li",			0,					0,					1,						1		},
		{"link",		1,					0,					0,						0		},
		{"main",		1,					0,					1,						0		},
		{"map",			0,					1,					0,						0		},
		{"mark",		1,					0,					1,						0		},
		{"menu",		0,					1,					0,						0		},
		{"menuitem",	1,					0,					0,						0		},
		{"meta",		0,					0,					0,						0		},
		{"meter",		1,					0,					1,						0		},
		{"nav",			1,					0,					0,						0		},
		{"noframes",	0,					1,					0,						0		},
		{"noscript",	0,					1,					0,						0		},
		{"object",		0,					1,					0,						0		},
		{"ol",			1,					0,					1,						0		},
		{"optgroup",	0,					1,					0,						0		},
		{"option",		0,					1,					0,						0		},
		{"output",		0,					1,					0,						0		},
		{"p",			1,					0,					1,						1		},
		{"param",		1,					0,					0,						0		},
		{"pre",			1,					0,					1,						0		},
		{"progress",	0,					1,					0,						0		},
		{"q",			1,					0,					1,						0		},
		{"rp",			1,					0,					1,						0		},
		{"rt",			1,					0,					1,						0		},
		{"ruby",		1,					0,					1,						0		},
		{"s",			1,					0,					1,						0		},
		{"samp",		1,					0,					1,						0		},
		{"script",		0,					1,					0,						0		},
		{"section",		1,					0,					0,						0		},
		{"select",		0,					1,					0,						0		},
		{"small",		1,					0,					1,						0		},
		{"source",		1,					1,					0,						0		},
		{"span",		1,					0,					1,						0		},
		{"strike",		1,					0,					1,						0		},
		{"strong",		1,					0,					1,						0		},
		{"style",		0,					1,					0,						0		},
		{"sub",			1,					0,					1,						0		},
		{"summary",		1,					0,					1,						0		},
		{"sup",			1,					0,					1,						0		},
		{"table",		0,					0,					1,						0		},
		{"tbody",		1,					0,					1,						0		},
		{"td",			0,					0,					1,						0		},
		{"textarea",	0,					1,					0,						0		},
		{"tfoot",		1,					0,					1,						0		},
		{"th",			1,					0,					1,						0		},
		{"thead",		1,					0,					1,						0		},
		{"time",		1,					0,					1,						0		},
		{"title",		0,					0,					0,						0		},
		{"tr",			1,					0,					1,						0		},
		{"track",		1,					0,					0,						0		},
		{"tt",			1,					0,					1,						0		},
		{"u",			1,					0,					1,						0		},
		{"ul",			1,					0,					1,						0		},
		{"var",			1,					0,					1,						0		},
		{"video",		0,					1,					0,						0		},
		{"wbr",			1,					0,					1,						0		},
};

typedef struct sMaxTagContext
{
	//int text_len;
	int block_score;
	tree<HTML::Node>::iterator tag_it;
}MaxTagContext;

bool str_nocase_equal(const string left,const string right)
{
	if(left.empty() || right.empty())
		return false;

	if(0==strcasecmp(left.c_str(),right.c_str()) )
		return true;

	return false;
}

bool is_str_in(const string str,HtmlTagOpt opt)
{
	for(int i=0;i<ARRAY_LENGTH(g_tagNameTable);++i)
	{
		if(str_nocase_equal(str,g_tagNameTable[i].tag))
		{
			switch(opt)
			{
			case eTag_RM_TAG: return (1==g_tagNameTable[i].rm_tag_label);
			case eTag_RM_NODE: return (1==g_tagNameTable[i].rm_tag_node);
			case eTag_IS_CTT: return (1==g_tagNameTable[i].is_content);
			case eTAG_IS_NTL: return (1==g_tagNameTable[i].is_nextline);
			default: return false;
			}
		}
	}

	return false;
}

bool is_remove_tag(const string str)
{
	return is_str_in(str,eTag_RM_TAG);
}

bool is_remove_node(const string str)
{
	return is_str_in(str,eTag_RM_NODE);
}

bool is_content_tag(const string str)
{
	return is_str_in(str,eTag_IS_CTT);
}

bool is_nextline_tag(const string str)
{
	return is_str_in(str,eTAG_IS_NTL);
}

bool is_title_wrap_tag(const string str)
{
	const char *wraptags[]={"h1","h2","h3","strong","div","td"};
	for(size_t i=0;i<sizeof(wraptags)/sizeof(char *);++i)
	{
		if(str_nocase_equal(wraptags[i],str))
			return true;
	}

	return false;
}

bool is_content_wrap_tag(const string str)
{
	const char *wraptags[]={"div","td","table","body"};

	for(size_t i=0;i<sizeof(wraptags)/sizeof(char *);++i)
	{
		if(str_nocase_equal(wraptags[i],str))
			return true;
	}

	return false;
}

bool is_source_tag_class(const string str)
{
	const char *tagclass[]={"source","a_source","art_source","article_source",
				"cite","src","art_src","from","color-a-1"/*qq.com*/,"writer"/*sohu.com*/,
				"ne_article_source"/*163.com*/,"post-user"/*weixin.qq.com*/};
	for(size_t i=0;i<sizeof(tagclass)/sizeof(char *);++i)
	{
		if(str_nocase_equal(tagclass[i],str))
			return true;
	}
	
	return false;
}

bool is_copyright_text(const string text)
{
	const char *textarr[]={"版权所有","版权均属","未经本网授权","本站郑重声明","免责声明"};
	for(size_t i=0;i<sizeof(textarr)/sizeof(char *);++i)
	{
		if(string::npos!=text.find(textarr[i]))
			return true;
	}

	return false;
}

bool is_maybe_contain_source(const string text,string &src)
{
        static const char *textarr[]={
#include "source.dict"
	};

        for(size_t i=0;i<sizeof(textarr)/sizeof(char *);++i)
        {    
                if(string::npos!=text.find(textarr[i]))
		{
			src=textarr[i];
                        return true;
		}
        }    

        return false;
	
}

string get_tag_sub_tree_text(tree<HTML::Node> &dom,tree<HTML::Node>::iterator it)
{
	string tmpstr="";
	try{
		tree<HTML::Node>::sibling_iterator it_begin=it;
		tree<HTML::Node>::sibling_iterator it_ends=dom.next_sibling(it);
		tree<HTML::Node> linktree=dom.subtree(it_begin,it_ends);
		for(tree<HTML::Node>::iterator iter=linktree.begin();iter!=linktree.end();++iter)
		{
			if(!iter->isComment() && !iter->isTag())
			{
				string txt=iter->text();
				Utils::Trim(txt);
				tmpstr.append(txt);
			}
		}
	}catch(...){}
							
	return tmpstr;
}
//////////////////////////////////////////////////////////////////////////////
CContentParser::CContentParser(const int type)
:mType(type)
,mMarkedImg(false)
,mLinkAnchor(true)
,mTidyHtml(false)
,mSetContentFilter(false)
{
}

CContentParser::~CContentParser()
{
	Close();
}

void CContentParser::Clear()
{
	mResult.title.clear();
	mResult.content.clear();
	mResult.pubtime=0;
	mResult.source.clear();
	mResult.imglist.clear();
	mResult.linklist.clear();

	mErrorMsg.clear();
}

int CContentParser::LoadFilterFile(const char *filename)
{
	if(NULL!=filename && mFilterContent.LoadFiles(filename))
	{
		mSetContentFilter=true;
		return 0;
	}
	else
	{
		mSetContentFilter=false;
		return -1;
	}
}

int CContentParser::ParseTime(const string text,int min/*=0*/)
{
	CTimeParser tp;
	time_t pt=0;

	if(text.empty())
		return 0;

	if(tp.TimeParser(text,min,pt))
	{
		return pt;
	}

	return min;
}

void CContentParser::ParseSource(string &text)
{
	CSourceParser sp;
	string source;

	if(!mResult.source.empty())
		return;

	if(text.empty())
		return;

	if(sp.SourceParser(text,source))
	{
		mResult.source=source;
	}
	else
	{
		mResult.source="";
	}
}

bool CContentParser::PreProcess(string &src,const string url)
{
	/*
	*该函数负责：
	*(1)抽取发布时间、来源信息
	*(2)清理无用的HTML标签
	*/
	if(src.size()==0 )
		return false;

	Clear();
	string space="&nbsp;";

	//Utils::Save2File("orginal.txt",src);

	if(mTidyHtml)
	{	
		string errmsg;
		if(!CHtmlCleaner::HtmlTidy(src,errmsg))
		{
			mErrorMsg=errmsg;
			return false;
		}
	}

	std::remove (src.begin(), src.end(), '\n');
 	std::remove (src.begin(), src.end(), '\r');
 	std::remove (src.begin(), src.end(), '\t');
 	Utils::ReplaceAll(src,space," ");
	
	if(IS_PUBTIME(mType) || IS_SOURCE(mType))
	{
		string text=CHtmlCleaner::HtmlEraseTags(src);
		//std::cout<<"================"<<std::endl<<text<<"===================="<<std::endl;
		if(!text.empty())
		{
			if(IS_SOURCE(mType))
			{
				ParseSource(text);
			}

			if(IS_PUBTIME(mType))
			{
				int pt=ParseTime(url);
				mResult.pubtime=ParseTime(text,pt);
			}
		}

		htmlcxx::HTML::ParserDom domParser;
		tree<HTML::Node> dom1 = domParser.parseTree(src);
		if(dom1.size()==0)
		{
			mErrorMsg="Parse DOM failed,DOM size equal 0 !";
			return false;
		}

		tree<HTML::Node>::iterator it = dom1.begin();
		tree<HTML::Node>::iterator end = dom1.end();
		for(it = dom1.begin(); it != end; ++it)
		{
			if(it->isComment())
			{
				continue;
			}
			else if(it->isTag())
			{
				it->parseAttributes();
				
				//handle <meta> tag information
				if(str_nocase_equal(it->tagName(),"meta"))
				{
					map<string,string> tagattr=it->attributes();
					map<string,string>::iterator mapit_name=tagattr.find("name");
					map<string,string>::iterator mapit_prop=tagattr.find("property");
					map<string,string>::iterator mapit_cont=tagattr.find("content");

					map<string,string>::iterator mapit_key=tagattr.end();
					if(mapit_name!=tagattr.end()) mapit_key=mapit_name;
					if(mapit_prop!=tagattr.end()) mapit_key=mapit_prop;
					if(mapit_key!=tagattr.end() && mapit_cont!=tagattr.end())
					{
						string keystr=mapit_key->second;
						string valstr=mapit_cont->second;

						//===============publish time==============
						if(IS_PUBTIME(mType))
						{
							if(keystr.find("time")!=string::npos || keystr.find("date")!=string::npos)
							{
								mResult.pubtime=ParseTime(valstr,mResult.pubtime);
							}
						}

						//============passage source================
						//1.针对新浪网站的来源抽取
						if(IS_SOURCE(mType) && url.find(".sina.com")!=string::npos)
						{
							if(keystr=="mediaid" && !valstr.empty())
								mResult.source=valstr;
						}
						//2.通用的网站来源抽取
						if(IS_SOURCE(mType) && mResult.source.empty())
						{
							if(keystr=="source" || keystr=="site")
							{
								if(string::npos==valstr.find("http"))
								{
									string::size_type pos=string::npos;
									do{
										pos=valstr.find_first_of(":%!?<>[]{}@&*");
										if(string::npos!=pos)
											valstr=valstr.substr(pos+1);
									}while(string::npos!=pos);

									pos=valstr.find_first_not_of(" \t\n\r&[]");
									if(pos>8)
										valstr=valstr.substr(0,pos);

									string keystr="来源：";
									pos=valstr.find(keystr);	
									if(pos!=string::npos)
									{
										valstr.erase(pos,pos+keystr.length());
									}

									if(valstr.length()>4 && valstr.length()<64)
										mResult.source=valstr;
								}
							}
						}
					}
				}
				else
				{
					map<string,string> tagattr=it->attributes();
					map<string,string> ::iterator it_id=tagattr.find("id");
					map<string,string> ::iterator it_cls=tagattr.find("class");
					string str_tag_cls;
					string str_tag_id;
					if(it_cls!=tagattr.end())
						str_tag_cls=it_cls->second;
					if(it_id!=tagattr.end())
						str_tag_id=it_id->second;
					
					//handle for title
					if(IS_TITLE(mType) && str_nocase_equal(it->tagName(),"p") 
						&& str_tag_cls=="title")
						mResult.title=get_tag_sub_tree_text(dom1,it);

					//handle for source
					if(IS_SOURCE(mType) /*&&  mResult.source.empty()*/)
					{
						bool maybe_source_class=!str_tag_cls.empty() && is_source_tag_class(str_tag_cls);
						bool maybe_source_id=!str_tag_id.empty() && is_source_tag_class(str_tag_id);
						if(maybe_source_class || maybe_source_id)
						{
							string tmpsrc=get_tag_sub_tree_text(dom1,it);
							if(tmpsrc.length()>0)
							{
								const string keyarr[4]={"来源：","来源:","来源","转自："};
								for(int i=0;i<4;++i)
								{
									string::size_type pos=tmpsrc.find(keyarr[i]);
									if(pos!=string::npos) 
									{
										tmpsrc=tmpsrc.substr(pos+keyarr[i].size());
										break;
									}
								}
							
								string::size_type pos=tmpsrc.find_first_of(" ");
								if(pos!=string::npos)
									tmpsrc=tmpsrc.substr(0,pos);

								if(tmpsrc.length()>4 && tmpsrc.length()<128)
									mResult.source=tmpsrc;
							}
						}
						
					}
				}
			}
		}
	}

	char pattern[1024];
	src=RegexAPI::RegexHelper::ReplaceAll(src.c_str(),"<br[^>]*>","\n");
	src=RegexAPI::RegexHelper::ReplaceAll(src.c_str(),"</?p(>|[\\s]+[^>]*>)","\n");
	src=RegexAPI::RegexHelper::ReplaceAll(src.c_str(),"</?center(>|[\\s]+[^>]*>)","\n");
	for(int i=0;i<ARRAY_LENGTH(g_tagNameTable);++i)
	{
		if(g_tagNameTable[i].rm_tag_label)
		{
			if(strlen(g_tagNameTable[i].tag)<3)
			{
				sprintf(pattern,"</?%s(>|[\\s]+[^>]*>)",g_tagNameTable[i].tag);
			}
			else
			{
				sprintf(pattern,"</?%s[^>]*>",g_tagNameTable[i].tag);
			}

			src=RegexAPI::RegexHelper::ReplaceAll(src.c_str(),pattern,"");
		}
	}

	//Utils::Save2File("html.txt",src);

	tree<HTML::Node> dom2 = mDomParser.parseTree(src);
	if(dom2.size()==0)
	{
		mErrorMsg="Parse DOM failed,DOM size equal 0 !";
		return false;
	}

	tree<HTML::Node>::iterator it = dom2.begin();
	tree<HTML::Node>::iterator end = dom2.end();
	for(it = dom2.begin(); it != end; )
	{
		if(it->isComment())
		{
			it=dom2.erase(it);
			continue;
		}
		else if(it->isTag())
		{
			//remove or ease tag
			if( is_remove_node(it->tagName()) )
			{
				dom2.erase_children(it);
				it=dom2.erase(it);
				continue;
			}
			else if(it.number_of_children()<=0)
			{
				it=dom2.erase(it);
				continue;
			}
			else ++it;
		}
		else
		{
			++it;
		}
	}

	//cout<<"================="<<dom2<<endl<<"=================="<<endl;	
	return true;
}

void CContentParser::AfterProcess(string &text)
{
	Utils::Trim(text);
	Utils::CleanupStr(text);
	mFilterContent.FilterContent(text);
}

bool CContentParser::Open(std::string &src,const std::string &url)
{
	if(!PreProcess(src,url))
		return false;

	JaccardSimilarity sim;
	CHttpURL convert(url);
	tree<HTML::Node> dom = mDomParser.getTree();
	if(dom.size()==0)
	{
		mErrorMsg="Parse DOM failed,DOM size equal 0 !";
		return false;
	}

	bool get_title=false;
	string page_title="";
	string maybe_title="";
	double maybe_title_threshold=0.;
	tree<HTML::Node>::iterator maybe_title_it=dom.end();

	tree<HTML::Node>::iterator it = dom.begin();
	tree<HTML::Node>::iterator parent;
	tree<HTML::Node>::iterator max_node_it=dom.end();
	tree<HTML::Node>::iterator second_node_it=dom.end();
	int max_node_score=0;
	int second_node_score=0;

	for(it = dom.begin(); it != dom.end(); ++it)
	{
		if(it->isTag())
		{
			//handle <meta> tag information
			if(str_nocase_equal(it->tagName(),"meta"))
			{
				it->parseAttributes();
				map<string,string> tagattr=it->attributes();
				map<string,string>::iterator mapit_name=tagattr.find("name");
				map<string,string>::iterator mapit_prop=tagattr.find("property");
				map<string,string>::iterator mapit_cont=tagattr.find("content");

				map<string,string>::iterator mapit_key=tagattr.end();
				if(mapit_name!=tagattr.end()) mapit_key=mapit_name;
				if(mapit_prop!=tagattr.end()) mapit_key=mapit_prop;
				if(mapit_key!=tagattr.end() && mapit_cont!=tagattr.end())
				{
					string keystr=mapit_key->second;
					string valstr=mapit_cont->second;

					//passage title
					if(IS_TITLE(mType))
					{
						if(keystr=="title")
						{
							mResult.title=valstr;
							get_title=true;
						}
					}
				}
			}
		}
		else if(!it->isComment())
		{
			parent=dom.parent(it);
			if(NULL==parent.node)
				continue;

			if(parent->isTag())
			{
				string text=it->text();
				Utils::Trim(text);

				if(str_nocase_equal(parent->tagName(),"title") && !get_title && IS_TITLE(mType))
				{
					page_title=text;

					string::size_type pos=text.find_first_of("_|-");
					if(pos>=20)  text=text.substr(0,pos);
					
					if(text.length()>16 && text.length()>mResult.title.length())
					{
						mResult.title=text;
						get_title=false;
					}
				}
				else if(is_title_wrap_tag(parent->tagName()) && !get_title  &&  IS_TITLE(mType))
				{
					if(!text.empty() && text.size()>16 )
					{
						if(page_title.size()>16)
						{
							double threshold=sim.CalculateTextSimilarity(page_title,text);
							if(threshold>maybe_title_threshold)
							{
								maybe_title_threshold=threshold;
								string::size_type pos=text.find("\n");
								if(pos!=string::npos)
									text=text.substr(0,pos);

								maybe_title=text;
								maybe_title_it=parent;
								//if(text.find("　")==string::npos)
								//	get_title=true;
							}
						}else if("h1"==parent->tagName())
						{
							maybe_title=text;
							maybe_title_it=parent;
						}
					}
				}
				else if(str_nocase_equal(parent->tagName(),"a") && IS_SOURCE(mType))
				{
					if(!mResult.source.empty() && text.find(mResult.source)!=string::npos && text.size()<mResult.source.length()*3)
					{
						parent->parseAttributes();
                        			if(parent->attribute("href").first)
                        			{
                            				string src_url=parent->attribute("href").second;
                            				Utils::Trim(src_url);
                            				if(!src_url.empty() && src_url.find("javascript")==string::npos)
                            				{
								mResult.srcurl=src_url;
							}
						}
					}
				}

				if( is_content_tag(parent->tagName()))
				{
					parent->parseAttributes();

					map<string,string> tagattr=parent->attributes();
					map<string,string> ::iterator it_id=tagattr.find("id");
					map<string,string> ::iterator it_cls=tagattr.find("class");
					map<string,string> ::iterator it_style=tagattr.find("style");
					string str_tag_id;
					string str_tag_cls;
					string str_tag_style;
					int score=0;

					if(it_id!=tagattr.end())
						str_tag_id=it_id->second;
					if(it_cls!=tagattr.end())
						str_tag_cls=it_cls->second;
					if(it_style!=tagattr.end())
						str_tag_style=it_style->second;

					if(!str_tag_cls.empty() )
					{
						if( str_tag_style.find("display:none")==string::npos )
						{
							if(RegexAPI::RegexHelper::IsMatch(str_tag_cls.c_str(),
								"(comment|shengming|bar|hidden|copyright|meta|footer|footnote)"))
							{
								score-=text.size()*1.2;
							}
							else if(RegexAPI::RegexHelper::IsMatch(str_tag_cls.c_str(),
								"((^|\\s)(post|hentry|detail|context|entry[-]?(content|text|body)?|article[-]?(content|text|body)?)(\\s|$))"))
							{
								score+=text.size()*0.5;
							}
						}
						else
						{
							score=-100;
						}
			
					}

					if(!str_tag_id.empty())
					{
						if(str_tag_style.find("display:none")==string::npos)
						{
							if(RegexAPI::RegexHelper::IsMatch(str_tag_id.c_str(),
								"(comment|shengming|bar|hidden|copyright|meta|footer|footnote)"))
							{
								score-=text.size()*1.2;
							}
							else if(RegexAPI::RegexHelper::IsMatch(str_tag_id.c_str(),
								"^(post|hentry|detail|context|entry[-]?(content|text|body)?|article[-]?(content|text|body)?)$"))
							{
								score+=text.size()*0.5;
							}
						}
						else
						{
							score=-100;
						}

						if(IS_SOURCE(mType) && url.find("sohu.com")!=string::npos)
						{
							if(str_tag_id=="sourceOrganization" && mResult.source.empty())
								mResult.source=text;
							if(str_tag_id=="isBasedOnUrl" && !mResult.source.empty())
								mResult.srcurl=text;
						}
					}

					if(text.size()>0)
					{
						if(!is_copyright_text(text))
						{
							if(text.size()>10)
                                				score+=text.size();

							if(str_nocase_equal(parent->tagName(),"li")
								|| str_nocase_equal(parent->tagName(),"a"))
								score-=text.size()*0.9;

							if( str_nocase_equal(parent->tagName(),"h2")
								|| str_nocase_equal(parent->tagName(),"h3")
								|| str_nocase_equal(parent->tagName(),"h4")
								|| str_nocase_equal(parent->tagName(),"h5")
								|| str_nocase_equal(parent->tagName(),"h6"))
							{
								score+=text.size()*0.5;
							}
						}
						else
						{
							score-=text.size();
						}

						if( max_node_score<score && text.size()>128)
						{
							second_node_it=max_node_it;
							second_node_score=max_node_score;
						
							max_node_it=parent;
							max_node_score=score;
						}

					}
				}
			}
		}
	}//for

	if(NULL==max_node_it.node || max_node_it==dom.end())
	{
		mErrorMsg="Extract error when locate max text block,error information : max_len_it.node==NULL";
		return false;
	}

	if(NULL!=second_node_it.node && second_node_it!=dom.end() && max_node_it->offset()-second_node_it->offset()<4096)
	{
		tree<HTML::Node>::iterator_base position_it=second_node_it;
		if(second_node_it!=dom.end() && dom.is_in_subtree(position_it,max_node_it,dom.next_sibling(max_node_it)))
			second_node_it=dom.end();
		else if(dom.depth(second_node_it)<dom.depth(max_node_it))
			max_node_it=second_node_it;
		else if(dom.parent(second_node_it)==dom.parent(max_node_it))
			max_node_it=dom.parent(max_node_it);
	}

	tree<HTML::Node>::iterator subit=max_node_it;
	while(NULL!=subit.node)
	{
		assert(subit->isTag());
		string tagname=subit->tagName();

		if(is_content_wrap_tag(tagname) && subit->length()>=512)
			break;
		else if(subit->length()>=1024)
			break;
		else
		{
			try{subit=dom.parent(subit);}
			catch(...){break;}
		}
	}

	if(subit.node==NULL || subit.number_of_children()<=0)
	{
		mErrorMsg="Extract error when find max text block node, error information : subit.node==NULL";
		return false;
	}


	tree<HTML::Node> subtree;
	try
	{
		tree<HTML::Node>::sibling_iterator start=subit;
		tree<HTML::Node>::sibling_iterator ends=dom.next_sibling(subit);
		if(start==ends)
		{
			mErrorMsg="Extract error when generate text block tree, error information : start==ends";
			return false;
		}

		subtree=dom.subtree(start,ends);
	}
	catch(...)
	{
		mErrorMsg="Extract error when generate text block tree, error information : subtree=dom.subtree(start,ends)";
		return false;
	}

	for(it=subtree.begin();it!=subtree.end();++it)
	{
		if(it->isComment())
		{
			continue;
		}
		else if(it->isTag())
		{
			if( IS_IMAGES(mType) && str_nocase_equal(it->tagName(),"img"))
			{
				it->parseAttributes();
				if(it->attribute("src").first)
				{
					string src_url=it->attribute("src").second;
					string alt=it->attribute("alt").first?it->attribute("alt").second:"";
					Utils::Trim(src_url);
					if(!src_url.empty())
					{
						string absurl=convert.toAbsolute(src_url);

						pair<string,string> item;
						item.first=absurl;
						item.second=alt;
						mResult.imglist.push_back(item);

						if(mMarkedImg)
						{
							string imgpos="{IMG:"+Utils::Utils::Int2String(mResult.imglist.size())+"}\n";
							if(mResult.content[mResult.content.length()-1]!='\n')
								mResult.content.append("\n"+imgpos);
							else
								mResult.content.append(imgpos);
						}
						else
						{
							mResult.content.append("\n");
						}
					}
				}
				else if(it->attribute("data-src").first)
				{
					string src_url=it->attribute("data-src").second;
					if(!src_url.empty() && src_url.length()<512)
			           	{
                                        	pair<string,string> item;
                                             	item.first=src_url;
                                             	item.second="";
                                             	mResult.imglist.push_back(item);
 
                                             	if(mMarkedImg)
                                             	{
                                                	string imgpos="{IMG:"+Utils::Int2String(mResult.imglist.size())+"}\n";
                                                 	if(mResult.content[mResult.content.length()-1]!='\n')
                                                        	mResult.content.append("\n"+imgpos);
                                                     	else
                                                        	mResult.content.append(imgpos);
                                              	}
                                              	else
                                              	{
                                                	mResult.content.append("\n");
                                              	}
                                         }
				}
			}
			else if( IS_LINKS(mType) && str_nocase_equal(it->tagName(),"a"))
			{
				it->parseAttributes();
				if(it->attribute("href").first)
				{
					string src_url=it->attribute("href").second;
					Utils::Trim(src_url);
					if(!src.empty())
					{
						string absurl=convert.toAbsolute(src_url);
						string anchor="";

						if(mLinkAnchor)
						{
							anchor=get_tag_sub_tree_text(dom,it);
						}

						pair<string,string> item;
						item.first=absurl;
						item.second=anchor;
						mResult.linklist.push_back(item);
					}
				}
			}
			else
			{
				it->parseAttributes();

				map<string,string> tagattr=it->attributes();
				map<string,string> ::iterator it_id=tagattr.find("id");
				map<string,string> ::iterator it_cls=tagattr.find("class");
				string str_tag_id;
				string str_tag_cls;

				if(it_id!=tagattr.end())
					str_tag_id=it_id->second;
				if(it_cls!=tagattr.end())
					str_tag_cls=it_cls->second;

				if(!str_tag_cls.empty())
				{
					if(RegexAPI::RegexHelper::IsMatch(str_tag_cls.c_str(),
						"(comment|shengming|bar|scroll|copyright|meta|footer|footnote)"))
					{
						dom.erase_children(it);
					}
				}

				if(!str_tag_id.empty())
				{
					if(RegexAPI::RegexHelper::IsMatch(str_tag_id.c_str(),
					"(comment|shengming|bar|scroll|copyright|meta|footer|footnote)"))
					{
						dom.erase_children(it);
					}
				}
			}
		}
		else
		{
			parent=dom.parent(it);
			if(NULL==parent.node)
				continue;

			if( str_nocase_equal(parent->tagName(),"a"))
			{
				tree<HTML::Node>::iterator pparent=dom.parent(parent);
				if(NULL!=pparent.node && str_nocase_equal(pparent->tagName(),"li"))
					continue;
			}

			string text=it->text();
			Utils::TrimEx(text);
			bool nextline=is_nextline_tag(parent->tagName());
			if(nextline)
			{
				if(NULL==parent.node->prev_sibling)
				{
					if(mResult.content[mResult.content.length()-1]!='\n')
						mResult.content.append("\n");
				}
			}

			if(is_content_tag(parent->tagName()) && text.size()>0)
			{
				mResult.content.append(text);
			}

			if(nextline)
			{
				if(NULL==it.node->next_sibling)
				{
					if(mResult.content[mResult.content.length()-1]!='\n')
						mResult.content.append("\n");
				}
			}
		}

	}

	if(maybe_title_it!=dom.end() && maybe_title_it->offset()<max_node_it->offset() && !maybe_title.empty())
	{
		mResult.title=maybe_title;		
	}

	if(IS_SOURCE(mType) && mResult.source.empty())
	{
		tree<HTML::Node>::iterator start=maybe_title_it;
		tree<HTML::Node>::iterator stop=max_node_it;//subit;
	
		if(start!=stop && start!=dom.end() && stop!=dom.end() && start->offset() < stop->offset())
		{
			try{
				//tree<HTML::Node> subtree=dom.subtree(start,stop);
				//for(it=subtree.begin();it!=subtree.end();++it)
				for(it=start;it!=stop;++it)
				{
					if(it->isComment())
						continue;
					else if(it->isTag() && str_nocase_equal(it->tagName(),"a"))
					{
						string src="";
						string text=get_tag_sub_tree_text(subtree,it);
						if(is_maybe_contain_source(text,mResult.source))
						{
							it->parseAttributes();
							if(it->attribute("href").first)
							{
								string maybeurl=it->attribute("href").second;
								if(maybeurl.find("http")!=string::npos)
									mResult.srcurl=maybeurl;
							}
					
							break;
						}
					}
					else
					{
                                                string src="";
                                                string text=it->text();
                                                if(text.length()<1024 && is_maybe_contain_source(text,mResult.source))
							break;

					}
				}
			}catch(...){}
		}	
	}

	dom.clear();

	AfterProcess(mResult.content);

	Utils::HandleHtmlEscapeChars(mResult.title);
	Utils::HandleHtmlEscapeChars(mResult.content);

	return true;
}

void CContentParser::Close()
{
	Clear();
}
