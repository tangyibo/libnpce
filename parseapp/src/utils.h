#ifndef _MY_URTILS_H_
#define _MY_URTILS_H_
#include <string>
#include <stdio.h>
#include <time.h>

using std::string;

namespace Utils{

inline bool StartsWith(const string& str, const string& prefix) 
{
	if(prefix.length() > str.length()) 
		return false;

	return 0 == str.compare(0, prefix.length(), prefix);
}

inline void TrimString(string &str)
{
	string::size_type s = str.find_first_not_of(" \t\n\r");
	if (s != string::npos)
		str = str.substr(s);

	string::size_type e = str.find_last_not_of(" \t\n\r");
	if (e != string::npos)
		str = str.substr(0,e+1);
}

inline string Int2String(const int val)
{
	char buf[128];
	sprintf(buf,"%d",val);
	return buf;
}

inline string& ReplaceAll(string&  str,const string&  old_value,const string& new_value)
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

inline time_t MakeTime(int year, int mon, int mday, int hour, int min, int sec, int isdst=1)
{
	struct tm t;
	t.tm_year = year - 1900;
	t.tm_mon = mon - 1;
	t.tm_mday = mday;
	t.tm_hour = hour;
	t.tm_min = min;
	t.tm_sec = sec;
	t.tm_isdst = isdst;
	time_t time = mktime(&t);
	return time;
}

}
#endif
