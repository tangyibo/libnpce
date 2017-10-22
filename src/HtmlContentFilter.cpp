/*
 * HtmlContentFilter.cpp
 *
 *  Created: 2016年8月23日
 *   Author: tang
 */

#include "HtmlContentFilter.h"
#include "utils/UtilFun.h"
#include <fstream>
#include <algorithm>

using namespace std;

CContentFilter::CContentFilter()
:mFilterLists()
{

}

CContentFilter::~CContentFilter()
{
	Clearup();
}

bool CContentFilter::LoadFiles(const char *filename)
{
	Clearup();
	bool ret=false;

	ifstream fin(filename);
	if(fin.is_open())
	{
		string line;
		while(getline(fin, line))
		{
			Utils::Trim(line);
			if(!line.empty())
			{
				mFilterLists.push_back(line);
				ret=true;
			}
		}
		fin.close();
	}

	mstrFilePath=filename;

	return ret;
}

void CContentFilter::FilterContent(std::string &str)
{
	if(str.empty() || mFilterLists.size()<=0)
		return;

	std::vector<std::string>::const_iterator cit;
	for(cit=mFilterLists.begin();cit!=mFilterLists.end();++cit)
	{
		size_t pos=str.find(*cit);
		if(string::npos!=pos)
		{
			str.erase(pos,pos+(*cit).size());
		}
	}

	string space="&nbsp;";
	Utils::ReplaceAll(str,space," ");
}

void CContentFilter::Clearup()
{
	mFilterLists.clear();
}
