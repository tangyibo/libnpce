/*
 * HtmlAuthorParser.cpp
 *
 *  Created: 2016年8月22日
 *   Author: tang
 */

#include "HtmlSourceParser.h"
#include "utils/UtilFun.h"
#include <iostream>

CSourceParser::CSourceParser()
{
	m_strSourceRegexp[0] =" 来源 ";
	m_strSourceRegexp[1] ="新闻来源：";
	m_strSourceRegexp[2] =" 来源:";
	m_strSourceRegexp[3] =" 来源：";
	m_strSourceRegexp[4] =" 来源-";
	m_strSourceRegexp[5] =" 来源/";
	m_strSourceRegexp[6] =" 稿源：";
	m_strSourceRegexp[7]=" [来源] ";

	m_strSourceRegexp[8] ="来源：";
        m_strSourceRegexp[9] ="来源:";
	m_strSourceRegexp[10] ="来源/";
	m_strSourceRegexp[11]="稿源：";
	m_strSourceRegexp[12] ="来源 ：";
        m_strSourceRegexp[13] ="来源 ";
	m_strSourceRegexp[14] ="稿源：";
        m_strSourceRegexp[15] ="来源 ：";
	m_strSourceRegexp[16]="[来源]：";
}

CSourceParser::~CSourceParser()
{
}

bool CSourceParser::SourceParser (string &data, string& source)
{
	if(data.empty())
		return false;

	//std::cout<<data<<std::endl<<"===================="<<std::endl;
	Utils::ReplaceAll(data,"图片来源","图片");
	Utils::ReplaceAll(data,"资金来源","资金");
	Utils::ReplaceAll(data,"来源标题","原标题");
	Utils::ReplaceAll(data,"注明来源","注明");
	Utils::ReplaceAll(data,"数据来源","数据");
	Utils::ReplaceAll(data,"信息来源","信息");

	string tmp;
	for(int i=0;i<REGEX_SOURCE_PARSER_NUM_COUNT;++i)
	{
		tmp.clear();
		string::size_type start=data.find(m_strSourceRegexp[i]);
		if(string::npos==start)
			continue;

		tmp=data.substr(start+m_strSourceRegexp[i].length());
		if(!tmp.empty() )
		{
			const char *stopwords=" \t\n\r&[(])";
			string::size_type pos=tmp.find_first_not_of(stopwords);
			if(pos>8)
				continue;

			if(string::npos!=pos)
			{
				tmp=tmp.substr(pos);
			}

			if(tmp.empty() )
				continue;

			string::size_type stop=tmp.find_first_of(stopwords);
			if(string::npos!=stop)
			{
				source=tmp.substr(0,stop);
				string keystr[4]={"（","）","【","】"};
				for(int i=0;i<4;++i)
				{
					pos=source.find(keystr[i]);
					if(pos!=string::npos)
						source.erase(pos,pos+keystr[i].length());
				}
		
				bool invalid=false;
				string invalidstr[9]={"下一页","分享到","编辑","作者","发布","选择","字号","字体","标题"};
				for(int i=0;i<9;++i)
				{
					pos=source.find(invalidstr[i]);
                                        if(pos!=string::npos)
					{
						invalid=true;
						break;
					}
				}
				if(invalid)
					continue;

				Utils::Trim(source);

				if(string::npos==source.find_first_of(":#?!@%()'\"\n\r") &&
				 	!source.empty() && source.length()>=4 &&source.length()<=32)
				{
					return true;
				}
			}
		}
	}

	return false;
}
