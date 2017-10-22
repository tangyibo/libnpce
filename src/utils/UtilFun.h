/*
 * UtilFun.h
 *
 *  Created: 2016年8月17日
 *   Author: tang
 */

#ifndef SRC_UTILS_UTILFUN_H_
#define SRC_UTILS_UTILFUN_H_
#include <string>
#include <algorithm>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
using std::string;

class Utils
{
public:
	static void Trim(string &str)
	{
		string::size_type s = str.find_first_not_of(" \t\n\r");
		if (s != string::npos)
			str = str.substr(s);

		string::size_type e = str.find_last_not_of(" \t\n\r");
		if (e != string::npos)
			str = str.substr(0,e+1);
		
		//TrimEx(str);
	}

	static void TrimEx(string &str)
        {
		string::size_type s = str.find_first_not_of(" \t\r");
		if (s != string::npos)
			str = str.substr(s);
		
		string::size_type e = str.find_last_not_of(" \t\n\r");
		if (e != string::npos)
			str = str.substr(0,e+1);
        }

	static string Int2String(const int val)
	{
		char buf[128];
		sprintf(buf,"%d",val);
		return buf;
	}

	static string StringFormat(const char* fmt, ...)
	{
		int size = 256;
		std::string str;
		va_list ap;
		while (1)
		{
			str.resize(size);
			va_start(ap, fmt);
			int n = vsnprintf((char *)str.c_str(), size, fmt, ap);
			va_end(ap);
			if (n > -1 && n < size)
			{
				str.resize(n);
				return str;
			}
			if (n > -1)
				size = n + 1;
			else
				size *= 2;
		}
		return str;
	}

	static void CleanupStr(string &text)
	{
		string::iterator iter=text.begin();
		string::iterator it_begin,it_end;
		while(iter!=text.end())
		{
			if((*iter=='\n' || *iter=='\t' ) && iter!=text.end()-1)
			{
				it_begin=iter;
				it_end=it_begin++;
				while( it_end!=text.end() && (*it_end=='\n' ||*it_end==' ' || *it_end=='\t') )
					it_end++;

				if(it_end>it_begin)
					text.erase(it_begin,it_end);

				iter=it_begin;
			}
			else
				iter++;
		}

	}

	static string& ReplaceAll(string&  str,const string&  old_value,const string& new_value)
	{
		while(true)
		{
			string::size_type   pos(0);
			if(   (pos=str.find(old_value))!=string::npos   )
				str.replace(pos,old_value.length(),new_value);
			else   break;
		}
		return   str;
	}

	static void Save2File(const char *filename,const string content)
	{
		FILE *file=fopen(filename , "wb");
		if(file)
		{
			int ret=fwrite(content.data(),content.length(),1,file);
			assert(1==ret);

			fclose(file);
		}
	}

	static void HandleHtmlEscapeChars(string &src)
	{
         	int escape_count=26;
         	static string escape_string_dict[26][2]={
		#include "escape_chars.dict"
		};
         	for(int i=0;i<escape_count;++i)
         	{
                 	Utils::ReplaceAll(src,escape_string_dict[i][0],escape_string_dict[i][1]);
         	}
	}
};



#endif /* SRC_UTILS_UTILFUN_H_ */
