/*
 * HttpURL.cpp
 *
 *  Created: 2015年11月24日
 *   Author: tang
 */
#include <algorithm>
#include <string.h>
#include "HttpURL.h"
//#include "UrlCoder.h"

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

CHttpURL::CHttpURL(const std::string baseUrl)
:m_baseUrl(baseUrl)
{
	const char *key="://";
	std::string::size_type pos;
	pos=m_baseUrl.find(key);
	if(std::string::npos!=pos)
	{
		std::string str=m_baseUrl.substr(0,pos+strlen(key)-1);
		if(!str.empty())
			m_list.push_back(str);

		std::string fullpath=m_baseUrl.substr(pos+strlen(key));
		split(fullpath,"/",m_list);
	}
	else
	{
		split(m_baseUrl,"/",m_list);
	}

}

CHttpURL::~CHttpURL()
{
	m_baseUrl.clear();
	m_list.clear();
}

const std::string CHttpURL::toAbsolute(const std::string refUrl)
{
	std::string strurl=refUrl;
	//std::transform(strurl.begin(), strurl.end(), strurl.begin(),::tolower); 
	if(std::string::npos!=strurl.find("://") )
		return strurl;

	if('/'==strurl[0] && '/'==strurl[1] && m_list.size()>=2)
	{
		std::string absUrl=m_list[0];
		strurl=strurl.substr(1);
		absUrl+=strurl;
		return absUrl;
	}
	else if('/'==strurl[0] && '/'!=strurl[1] && m_list.size()>2)
	{
		std::string absUrl=m_list[0];
		absUrl+="/";
		absUrl+=m_list[1];
		absUrl+=strurl;

		return absUrl;
	}

	std::vector<std::string> sections;
	split(strurl,"/",sections);
	std::vector<std::string> list=m_list;

	std::vector<std::string>::iterator it;
	for(it=sections.begin();it!=sections.end();)
	{
		if(0==strcasecmp((*it).c_str(),".."))
		{
			if (list.size()>2) list.pop_back();
#ifdef WIN32
			it=sections.erase(it);
#else
			sections.erase(it);
#endif
		}
		else if(0==strcasecmp((*it).c_str(),"."))
		{
#ifdef WIN32
			it = sections.erase(it);
#else
			sections.erase(it);
#endif
		}
		else
		{
			++it;
		}

	}

	std::string absUrl;
	if(!IsLastCharSlash() && list.size()>2)
		list.pop_back();
	for(it=list.begin();it!=list.end();++it)
	{
		absUrl.append((*it));
		absUrl.append("/");
	}

	std::vector<std::string>::iterator last_it=sections.end();
	last_it--;
	for(it=sections.begin();it!=sections.end();++it)
	{
		absUrl.append(*it);
	
		if(it!=last_it)
			absUrl.append("/");
	}

	return absUrl;
}

const std::string CHttpURL::FileName(std::string &url)
{
	std::string httpurl=url;
	std::string::size_type pos=httpurl.find(":");
	if(pos!=std::string::npos)
		httpurl=httpurl.substr(pos+1);

	pos=httpurl.find_last_of('/');
	if(pos!=std::string::npos)
			httpurl=httpurl.substr(pos+1);

	pos=httpurl.find_last_of('?');
	if(pos!=std::string::npos)
			httpurl=httpurl.substr(0,pos);

	return httpurl;
}

void CHttpURL::split(std::string str,std::string pattern,std::vector<std::string> &result,bool urlcode/*=false*/)
{
	str+=pattern;

	int size=str.size();

	for(int i=0; i<size; i++)
	{
		int pos = str.find(pattern, i);
		if (pos < size)
		{
			std::string s = str.substr(i, pos - i);
			Trim(s);
			if (!s.empty())
			{
				//if(urlcode)
				//{
				//	std::string enstr = UrlCode::UrlEncode(s);
				//	result.push_back(enstr);
				//}
				//else
				{
					result.push_back(s);
				}
			}

			i = pos + pattern.size() - 1;
		}
	}

}

bool CHttpURL::IsLastCharSlash()
{
	if('/'==m_baseUrl[m_baseUrl.length()-1])
		return true;

	return false;
}

void CHttpURL::Trim(std::string &str)
{
	std::string::size_type s = str.find_first_not_of(" \t\n\r ");
	if (s != std::string::npos)
		str = str.substr(s);

	std::string::size_type e = str.find_last_not_of(" \t\n\r ");
	if (e != std::string::npos)
		str = str.substr(0,e+1);
}
