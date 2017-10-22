#include "HtmlTimeParser.h"
#include "regexapi/regexhelper.h"
#include <stdlib.h>
#include <string>
#include <iostream>
#include <algorithm>
using namespace std;

static int g_vecRegexpIndex[REGEX_TIME_PARSER_NUM_COUNT][10] = {
		{12,1, 4, 6,8,10,12},    	//匹配11 Jan 2011 22:23:24  {count,day,month,year,hour,minute,second}
		{12,1, 3, 6,8,10,12},     	//匹配Jan 11 2011 22:23:24
		{10,1, 4, 6,8,10},        	//匹配11 Jan 2011 22:23
		{10,1, 3, 6,8,10},        	//匹配Jan 11 2011 22:23
		{10,1, 3, 6,8,10},      	//匹配Jan 11 22:23:24

		{6,1, 4, 6},          		//匹配11 Jan 2011
		{6,1, 3, 6},          		//匹配Jan 11 2011
		{8,1, 3, 6,8},        		//匹配Jan 11 22:23
		{4,1, 3},             		//匹配Jan 11
		{4,1, 4},            		//匹配11 Jan

		{12,1, 3, 5, 8, 10, 12},    //匹配2011年1月2日 22:23:24
		{12,1, 3, 5, 8, 10, 12},    //匹配2011年1月2日 22时(点)23分24秒
		{10,1, 3, 5, 8, 10},        //匹配2011年1月2日 22:23
		{5,1, 3, 5},                //匹配2011年1月2日
		{11,1, 3, 5,7,9,11},        //匹配2011-12-23 22:23:24

		{9,1, 3, 5,7,9},           //匹配2011-12-23 22:23
		{11,1, 3, 5,7,9,11},       //匹配18-12-2011 22:23:24
		{9,1, 3, 5,7,9},           //匹配18-12-2011 22:23
		{5,1, 3, 5},               //匹配2011-12-23
		{5,1, 3, 5},               //匹配18-12-2011


		{12,1, 4, 6,8,10,12},    	//匹配11 Jan 2011 22:23:24
		{12,1, 3, 6,8,10,12},     	//匹配Jan 11 2011 22:23:24
		{10,1, 4, 6,8,10},        	//匹配11 Jan 2011 22:23
		{10,1, 3, 6,8,10},        	//匹配Jan 11 2011 22:23
		{10,1, 3, 6,8,10},      	//匹配Jan 11 22:23:24

		{6,1, 4, 6},           		//匹配11 Jan 2011
		{6,1, 3, 6},          		//匹配Jan 11 2011
		{8,1, 3, 6,8},        		//匹配Jan 11 22:23
		{4,1, 3},            		//匹配Jan 11
		{4,1, 4},             		//匹配11 Jan

		{11,1, 3, 5, 7, 9, 11},    //匹配2011年1月2日22:23:24
		{12,1, 3, 5, 7, 9, 11},    //匹配2011年1月2日22时(点)23分24秒
		{9,1, 3, 5, 7, 9},        //匹配2011年1月2日22:23
};

/////////////////////////////////////////////////////////////////
CTimeParser::CTimeParser()
{
	m_iMinLIineNum=5000;   ///获取最小有效时间文本行  常量(对复杂文本而已，需要进行分割从而逐行匹配，这里指对一个文本最多获取行数)
	m_iMinYear= 1970;       ///获取最小有效时间年份 常量

	/**< 时区正则串个数 */
	m_vecTimeZoneRegexpNum = 2;

	/**< am pm 正则匹配串 */
	m_strAmPmRegexp = "^.*(am|AM|pm|PM)";

	/**< 时区正则串静态变量数组信息 */
	m_strTimeZoneRegexp[0] = "^.*(GMT|UTC)(\\s?)(\\+|-)?(\\d{1,2})?";
	m_strTimeZoneRegexp[1] = "^.*(\\+|-)(\\d{2}00)";

	m_strTimeRegexp[0] = "^.*?(\\d{1,2})(th)?(\\s)(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)(\\D?\\s?)(\\d{2,4})(\\D)(\\d{1,2})(\\D)(\\d{1,2})(\\D)(\\d{1,2})"; //匹配11 Jan 2011 22:23:24
	m_strTimeRegexp[1] = "^.*?(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)(\\D)(\\d{1,2})(th)?(\\D?\\s?)(\\d{2,4})(\\D)(\\d{1,2})(\\D)(\\d{1,2})(\\D)(\\d{1,2})"; //匹配Jan 11 2011 22:23:24
	m_strTimeRegexp[2] = "^.*?(\\d{1,2})(th)?(\\s)(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)(\\D?\\s?)(\\d{2,4})(\\D)(\\d{1,2})(\\D)(\\d{1,2})"; //匹配11 Jan 2011 22:23
	m_strTimeRegexp[3] = "^.*?(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)(\\D)(\\d{1,2})(th)?(\\D?\\s?)(\\d{2,4})(\\D)(\\d{1,2})(\\D)(\\d{1,2})";  //匹配Jan 11 2011 22:23
	m_strTimeRegexp[4] = "^.*?(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)(\\D)(\\d{1,2})(th)?(\\D?\\s?)(\\d{1,2})(\\D)(\\d{1,2})(\\D)(\\d{1,2})";  //匹配Jan 11 22:23:24

	m_strTimeRegexp[5] = "^.*?(\\d{1,2})(th)?(\\s)(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)(\\D?\\s?)(\\d{2,4})(th)?";  //匹配11 Jan 2011
	m_strTimeRegexp[6] = "^.*?(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)(\\D)(\\d{1,2})(th)?(\\D?\\s?)(\\d{4})";    //匹配Jan 11 2011
	m_strTimeRegexp[7] = "^.*?(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)(\\D)(\\d{1,2})(th)?(\\D?\\s?)(\\d{1,2})(\\D)(\\d{1,2})"; //匹配Jan 11 22:23
	m_strTimeRegexp[8] = "^.*?(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)(\\D)(\\d{1,2})(th)?";   //匹配Jan 11
	m_strTimeRegexp[9] = "^.*?(\\d{1,2})(th)?(\\D)(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)";   //匹配11 Jan

	m_strTimeRegexp[10] = "^.*?(\\d{2}|\\d{4})\\s?(年)\\s?(\\d{1,2})\\s?(月)\\s?(\\d{1,2})\\s?(日)(\\s)(\\d{1,2})(\\D)(\\d{1,2})(\\D)(\\d{1,2})"; //匹配2011年1月2日 22:23:24
	m_strTimeRegexp[11] = "^.*?(\\d{2}|\\d{4})\\s?(年)\\s?(\\d{1,2})\\s?(月)\\s?(\\d{1,2})\\s?(日)(\\s)(\\d{1,2})(点|时)\\s?(\\d{1,2})\\s?(分)\\s?(\\d{1,2})\\s?(秒)?"; //匹配2011年1月2日 22时(点)23分24秒
	m_strTimeRegexp[12] = "^.*?(\\d{2}|\\d{4})\\s?(年)\\s?(\\d{1,2})\\s?(月)\\s?(\\d{1,2})\\s?(日)(\\s)(\\d{1,2})(\\D)(\\d{1,2})"; //匹配2011年1月2日 22:23
	m_strTimeRegexp[13] = "^.*?(\\d{2}|\\d{4})\\s?(年)\\s?(\\d{1,2})\\s?(月)\\s?(\\d{1,2})\\s?(日)"; //匹配2011年1月2日 //7
	m_strTimeRegexp[14] = "^.*?(\\d{2,4})(\\D)(\\d{1,2})(\\D)(\\d{1,2})(\\s)(\\d{1,2})(\\D)(\\d{1,2})(\\D)(\\d{1,2})"; //匹配2011-12-23 22:23:24 //3

	m_strTimeRegexp[15] = "^.*?(\\d{2,4})(\\D)(\\d{1,2})(\\D)(\\d{1,2})(\\s)(\\d{1,2})(\\D)(\\d{1,2})";  //匹配2011-12-23 22:23 //5
	m_strTimeRegexp[16] = "^.*?(\\d{1,2})(\\D)(\\d{1,2})(\\D)(\\d{4})(\\s)(\\d{1,2})(\\D)(\\d{1,2})(\\D)(\\d{1,2})";  //匹配18-12-2011 22:23:24 //4
	m_strTimeRegexp[17] = "^.*?(\\d{1,2})(\\D)(\\d{1,2})(\\D)(\\d{4})(\\s)(\\d{1,2})(\\D)(\\d{1,2})";  //匹配18-12-2011 22:23 //6
	m_strTimeRegexp[18] = "^.*?(\\d{2,4})(\\D)(\\d{1,2})(\\D)(\\d{1,2})"; //匹配2011-12-23 //8
	m_strTimeRegexp[19] = "^.*?(\\d{1,2})(\\s|\\.|-|/)(\\d{1,2})(\\s|\\.|-|/)(\\d{4})";  //匹配18-12-2011 //9

	m_strTimeRegexp[20] = "^.*?(\\d{1,2})(th)?(\\s)(January|February|March|April|May|June|July|August|September|October|November|December)(\\D?\\s?)(\\d{2,4})(\\D)(\\d{1,2})(\\D)(\\d{1,2})(\\D)(\\d{1,2})"; //匹配11 Jan 2011 22:23:24
	m_strTimeRegexp[21] = "^.*?(January|February|March|April|May|June|July|August|September|October|November|December)(\\D)(\\d{1,2})(th)?(\\D?\\s?)(\\d{2,4})(\\D)(\\d{1,2})(\\D)(\\d{1,2})(\\D)(\\d{1,2})"; //匹配Jan 11 2011 22:23:24
	m_strTimeRegexp[22] = "^.*?(\\d{1,2})(th)?(\\s)(January|February|March|April|May|June|July|August|September|October|November|December)(\\D?\\s?)(\\d{2,4})(\\D)(\\d{1,2})(\\D)(\\d{1,2})"; //匹配11 Jan 2011 22:23
	m_strTimeRegexp[23] = "^.*?(January|February|March|April|May|June|July|August|September|October|November|December)(\\D)(\\d{1,2})(th)?(\\D?\\s?)(\\d{2,4})(\\D)(\\d{1,2})(\\D)(\\d{1,2})";  //匹配Jan 11 2011 22:23
	m_strTimeRegexp[24] = "^.*?(January|February|March|April|May|June|July|August|September|October|November|December)(\\D)(\\d{1,2})(th)?(\\D?\\s?)(\\d{1,2})(\\D)(\\d{1,2})(\\D)(\\d{1,2})";  //匹配Jan 11 22:23:24

	m_strTimeRegexp[25] = "^.*?(\\d{1,2})(th)?(\\s)(January|February|March|April|May|June|July|August|September|October|November|December)(\\D?\\s?)(\\d{2,4})(th)?";  //匹配11 Jan 2011
	m_strTimeRegexp[26] = "^.*?(January|February|March|April|May|June|July|August|September|October|November|December)(\\D)(\\d{1,2})(th)?(\\D?\\s?)(\\d{4})";    //匹配Jan 11 2011
	m_strTimeRegexp[27] = "^.*?(January|February|March|April|May|June|July|August|September|October|November|December)(\\D)(\\d{1,2})(th)?(\\D?\\s?)(\\d{1,2})(\\D)(\\d{1,2})"; //匹配Jan 11 22:23
	m_strTimeRegexp[28] = "^.*?(January|February|March|April|May|June|July|August|September|October|November|December)(\\D)(\\d{1,2})(th)?";   //匹配Jan 11
	m_strTimeRegexp[29] = "^.*?(\\d{1,2})(th)?(\\D)(January|February|March|April|May|June|July|August|September|October|November|December)";   //匹配11 Jan

	m_strTimeRegexp[30] = "^.*?(\\d{2}|\\d{4})(年)(\\d{1,2})(月)(\\d{1,2})(日)(\\d{1,2})(\\D)(\\d{1,2})(\\D)(\\d{1,2})"; //匹配2011年1月2日22:23:24
	m_strTimeRegexp[31] = "^.*?(\\d{2}|\\d{4})(年)(\\d{1,2})(月)(\\d{1,2})(日)(\\d{1,2})(点|时)(\\d{1,2})(分)(\\d{1,2})(秒)?"; //匹配2011年1月2日22时(点)23分24秒
	m_strTimeRegexp[32] = "^.*?(\\d{2}|\\d{4})(年)(\\d{1,2})(月)(\\d{1,2})(日)(\\d{1,2})(\\D)(\\d{1,2})"; //匹配2011年1月2日22:23
}

CTimeParser::~CTimeParser()
{

}

void CTimeParser::SetMinYear(int year)
{
        time_t newtime = time(NULL);
        /**< 获取当前时间并解析 */
        struct tm *now = localtime(&newtime);
        if(year>=1970 && year<=1900+now->tm_year )
                m_iMinYear=year;
}

string CTimeParser::GetMonth(string monthString)
{
	//Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec (January|February|March |April |May|
	//June|July|August|September|October|November|December)
	if (monthString == "Jan" || monthString == "January")  return "1";	
	if (monthString == "Feb" || monthString == "February")  return "2";
	if (monthString == "Mar" || monthString == "March")  return "3";
	if (monthString == "Apr" || monthString == "April")  return "4";
	if (monthString == "May" )  return "5";
	if (monthString == "Jun" || monthString == "June")  return "6";
	if (monthString == "Jul" || monthString == "July")  return "7";
	if (monthString == "Aug" || monthString == "August")  return "8";
	if (monthString == "Sep" || monthString == "September")  return "9";
	if (monthString == "Oct" || monthString == "October")  return "10";
	if (monthString == "Nov" || monthString == "November")  return "11";
	if (monthString == "Dec" || monthString == "December")  return "12";

	return "0";
}

bool CTimeParser::GetTime(vector<string> &result,int curser,string&tempyear,string&tempmonth,
		string&tempday,string &temphour,string &tempminute,string &tempsecond)
{
	if ((int)result.size()<g_vecRegexpIndex[curser][0])
	{
		std::cout<<"CTimeParser::GetTime():Regex may be error!"<<std::endl;
		return false;
	}

	switch(curser)
	{
	case 0:
		tempyear   = result[g_vecRegexpIndex[curser][3]];
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][2]]);
		tempday    = result[g_vecRegexpIndex[curser][1]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		tempsecond = result[g_vecRegexpIndex[curser][6]];
		break;
	case 1:
		tempyear   = result[g_vecRegexpIndex[curser][3]];
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][1]]);
		tempday    = result[g_vecRegexpIndex[curser][2]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		tempsecond = result[g_vecRegexpIndex[curser][6]];
		break;
	case 2:
		tempyear   = result[g_vecRegexpIndex[curser][3]];
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][2]]);
		tempday    = result[g_vecRegexpIndex[curser][1]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		break;
	case 3:
		tempyear   = result[g_vecRegexpIndex[curser][3]];
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][1]]);
		tempday    = result[g_vecRegexpIndex[curser][2]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		break;
	case 4:
		tempyear   = "0";
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][1]]);
		tempday    = result[g_vecRegexpIndex[curser][2]];
		temphour   = result[g_vecRegexpIndex[curser][3]];
		tempminute = result[g_vecRegexpIndex[curser][4]];
		tempsecond = result[g_vecRegexpIndex[curser][5]];
		break;
	case 5:
		tempyear   = result[g_vecRegexpIndex[curser][3]];
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][2]]);
		tempday    = result[g_vecRegexpIndex[curser][1]];
		break;
	case 6:
		tempyear   = result[g_vecRegexpIndex[curser][3]];
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][1]]);
		tempday    = result[g_vecRegexpIndex[curser][2]];
		break;	
	case 7:
		tempyear   = "0";
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][1]]);
		tempday    = result[g_vecRegexpIndex[curser][2]];
		temphour   = result[g_vecRegexpIndex[curser][3]];
		tempminute = result[g_vecRegexpIndex[curser][4]];
		break;
	case 8:
		tempyear   = "0";
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][1]]);
		tempday    = result[g_vecRegexpIndex[curser][2]];
		break;
	case 9:
		tempyear   = "0";
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][2]]);
		tempday    = result[g_vecRegexpIndex[curser][1]];
		break;

	case 10:
		tempyear   = result[g_vecRegexpIndex[curser][1]];
		tempmonth  = result[g_vecRegexpIndex[curser][2]];
		tempday    = result[g_vecRegexpIndex[curser][3]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		tempsecond = result[g_vecRegexpIndex[curser][6]];
		break;
	case 11:
		tempyear   = result[g_vecRegexpIndex[curser][1]];
		tempmonth  = result[g_vecRegexpIndex[curser][2]];
		tempday    = result[g_vecRegexpIndex[curser][3]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		if( result.size() >12 )
			tempsecond = result[g_vecRegexpIndex[curser][6]];
		break;
	case 12:
		tempyear   = result[g_vecRegexpIndex[curser][1]];
		tempmonth  = result[g_vecRegexpIndex[curser][2]];
		tempday    = result[g_vecRegexpIndex[curser][3]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		break;
	case 13:
		tempyear   = result[g_vecRegexpIndex[curser][1]];
		tempmonth  = result[g_vecRegexpIndex[curser][2]];
		tempday    = result[g_vecRegexpIndex[curser][3]];
		break;
	case 14:
		tempyear   = result[g_vecRegexpIndex[curser][1]];
		tempmonth  = result[g_vecRegexpIndex[curser][2]];
		tempday    = result[g_vecRegexpIndex[curser][3]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		tempsecond = result[g_vecRegexpIndex[curser][6]];
		break;
	case 15:
		tempyear   = result[g_vecRegexpIndex[curser][1]];
		tempmonth  = result[g_vecRegexpIndex[curser][2]];
		tempday    = result[g_vecRegexpIndex[curser][3]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		break;
	case 16:
		tempyear   = result[g_vecRegexpIndex[curser][3]];
		tempmonth  = result[g_vecRegexpIndex[curser][2]];
		tempday    = result[g_vecRegexpIndex[curser][1]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		tempsecond = result[g_vecRegexpIndex[curser][6]];
		break;
	case 17:
		tempyear   = result[g_vecRegexpIndex[curser][3]];
		tempmonth  = result[g_vecRegexpIndex[curser][2]];
		tempday    = result[g_vecRegexpIndex[curser][1]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		break;
	case 18:
		tempyear   = result[g_vecRegexpIndex[curser][1]];
		tempmonth  = result[g_vecRegexpIndex[curser][2]];
		tempday    = result[g_vecRegexpIndex[curser][3]];
		break;
	case 19:
		tempyear   = result[g_vecRegexpIndex[curser][3]];
		tempmonth  = result[g_vecRegexpIndex[curser][2]];
		tempday    = result[g_vecRegexpIndex[curser][1]];
		//temphour   = result[g_vecRegexpIndex[curser][4]];
		break;
	case 20:
		tempyear   = result[g_vecRegexpIndex[curser][3]];
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][2]]);
		tempday    = result[g_vecRegexpIndex[curser][1]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		tempsecond = result[g_vecRegexpIndex[curser][6]];
		break;
	case 21:
		tempyear   = result[g_vecRegexpIndex[curser][3]];
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][1]]);
		tempday    = result[g_vecRegexpIndex[curser][2]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		tempsecond = result[g_vecRegexpIndex[curser][6]];
		break;
	case 22:
		tempyear   = result[g_vecRegexpIndex[curser][3]];
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][2]]);
		tempday    = result[g_vecRegexpIndex[curser][1]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		break;
	case 23:
		tempyear   = result[g_vecRegexpIndex[curser][3]];
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][1]]);
		tempday    = result[g_vecRegexpIndex[curser][2]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		break;
	case 24:
		tempyear   = "0";
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][1]]);
		tempday    = result[g_vecRegexpIndex[curser][2]];
		temphour   = result[g_vecRegexpIndex[curser][3]];
		tempminute = result[g_vecRegexpIndex[curser][4]];
		tempsecond = result[g_vecRegexpIndex[curser][5]];
		break;
	case 25:
		tempyear   = result[g_vecRegexpIndex[curser][3]];
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][2]]);
		tempday    = result[g_vecRegexpIndex[curser][1]];
		break;
	case 26:
		tempyear   = result[g_vecRegexpIndex[curser][3]];
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][1]]);
		tempday    = result[g_vecRegexpIndex[curser][2]];
		break;	
	case 27:
		tempyear   = "0";
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][1]]);
		tempday    = result[g_vecRegexpIndex[curser][2]];
		temphour   = result[g_vecRegexpIndex[curser][3]];
		tempminute = result[g_vecRegexpIndex[curser][4]];
		break;
	case 28:
		tempyear   = "0";
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][1]]);
		tempday    = result[g_vecRegexpIndex[curser][2]];
		break;
	case 29:
		tempyear   = "0";
		tempmonth  = GetMonth(result[g_vecRegexpIndex[curser][2]]);
		tempday    = result[g_vecRegexpIndex[curser][1]];
		break;
	case 30:
		tempyear   = result[g_vecRegexpIndex[curser][1]];
		tempmonth  = result[g_vecRegexpIndex[curser][2]];
		tempday    = result[g_vecRegexpIndex[curser][3]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		tempsecond = result[g_vecRegexpIndex[curser][6]];
		break;
	case 31:
		tempyear   = result[g_vecRegexpIndex[curser][1]];
		tempmonth  = result[g_vecRegexpIndex[curser][2]];
		tempday    = result[g_vecRegexpIndex[curser][3]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		tempsecond = result[g_vecRegexpIndex[curser][6]];
		break;
	case 32:
		tempyear   = result[g_vecRegexpIndex[curser][1]];
		tempmonth  = result[g_vecRegexpIndex[curser][2]];
		tempday    = result[g_vecRegexpIndex[curser][3]];
		temphour   = result[g_vecRegexpIndex[curser][4]];
		tempminute = result[g_vecRegexpIndex[curser][5]];
		tempsecond = "0";
		break;
	}

	if (curser == 7 || curser == 8 || curser == 9)
	{
		if( atoi(tempmonth.c_str()) > 12 && atoi(tempday.c_str()) <=31)
		{
			string tmp = tempday;
			tempday = tempmonth;
			tempmonth = tmp;
		}
	}
	return true;
}


time_t CTimeParser::maketime(int year, int mon, int mday, int hour, int min, int sec, int isdst)
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

vector<string> CTimeParser::SplitString(const string& s, const string& delim, const bool keep_empty)
{
	vector<string> result;
	result.clear();
	if (delim.empty())
	{
		result.push_back(s);
		return result;
	}

	string::const_iterator substart = s.begin(), subend;
	while (true)
	{
		subend = search(substart, s.end(), delim.begin(), delim.end());
		string temp(substart, subend);
		if (keep_empty || !temp.empty())
		{
			///如果得到的值不为空，这个地方主要考虑//
			if (temp !="")
			{
				result.push_back(temp);
				if (result.size()==m_iMinLIineNum)
				{
					return result;
				}
			}

		}
		if (subend == s.end())
		{
			break;
		}
		substart = subend + delim.size();
	}

	return result;
}

void CTimeParser::MakeTimeAmPmFlag(string &subString,int& hour)
{
	vector<string> apmResult;
	if (RegexAPI::RegexHelper::Match(subString.c_str(),m_strAmPmRegexp.c_str(),apmResult)>0)
	{
		if (apmResult[0] == "pm" || apmResult[0] == "PM")
		{
			hour = hour +12;
			hour = hour>24?hour-24:hour;
		}

	}
}

void CTimeParser::MakeTimeZone(string &subString)
{
/*	vector<string> timeZoneResult;
	for (size_t tZoneFlag = 0;tZoneFlag!=m_vecTimeZoneRegexpNum;tZoneFlag++)
	{
		if (RegexAPI::RegexHelper::Match(subString.c_str(),m_strTimeZoneRegexp[tZoneFlag].c_str(),timeZoneResult)>0)
		{
			//for (size_t m=1;m!=timeZoneResult.size();m++)
			//{
			// 	cout<<timeZoneResult[m]<<" ";
			//}
			//cout<<endl;
			break;
		}
	}
*/
}

bool CTimeParser::MakeValidJudgement(int& year,int& month,int& day,int&hour,int&minute,
		int&second,int localyear,string&tempyear,string&tempmonth,string&tempday,string temphour,
		string tempminute,string tempsecond)
{
	/**< 年信息处理，如果为0，置为当前年份，小于100，加上2000 */
	year = atoi(tempyear.c_str());
	year = (year==0) ?localyear: year;
	year = (year<100) ? (year+2000) : year;

	month = atoi(tempmonth.c_str());
	day = atoi(tempday.c_str());
	hour = atoi(temphour.c_str());
	minute = atoi(tempminute.c_str());
	second = atoi(tempsecond.c_str());

	/**< 有效性判断 */
	if (year<=localyear && year>=m_iMinYear &&
			month>=1 && month<=12 &&
			day>=1 && day<=31 &&
			hour >= 0 && hour<=24 &&
			minute>=0 && minute <=60 &&
			second>=0 &&second <=60)
	{
		return true;
	}

	return false;
}

bool CTimeParser::TimeParser (const string &data,const time_t min, time_t& resultTime)
{
	int regexNum;
	time_t newtime = time(NULL);
	/**< 获取当前时间并解析 */
	struct tm *local = localtime(&newtime);
	int default_year = local->tm_year + 1900, default_month = local->tm_mon + 1, default_day = local->tm_mday;
	int default_hour = 0, default_minute = 0, default_second = 0;
	//int timezone = 0, timezone_offset = 0;
	//int apmFlag = 0;
	string tempyear,tempmonth,tempday,temphour,tempminute,tempsecond;

	/**< 剖析匹配的字符串 */
	vector<string> vData;
	vData = SplitString(data,"\n");
	vector<string> result;
	size_t j;

	//std::cout<<"============="<<std::endl<<data<<std::endl<<"======================"<<std::endl;

	/**< 循环匹配 */
	for (j=0;j!=vData.size();j++)
	{
		for (regexNum=0; regexNum!=REGEX_TIME_PARSER_NUM_COUNT; regexNum++)
		{
			result.clear();
			string matchstr=RegexAPI::RegexHelper::Match(vData[j].c_str(),m_strTimeRegexp[regexNum].c_str());
			if (!matchstr.empty() && RegexAPI::RegexHelper::RegexSplit(matchstr.c_str(),m_strTimeRegexp[regexNum].c_str(),result)>0)
			{
				//findflag = true;
				//string::size_type subStrBegin = vData[j].find(result[0]);
				//std::cout<<m_strTimeRegexp[regexNum]<<" "<<vData[j]<<std::endl;

				if(GetTime(result,regexNum,tempyear,tempmonth,tempday,temphour,tempminute,tempsecond))
				{
					int year = default_year, month = default_month, day = default_day;
					int hour = default_hour, minute = default_minute, second = default_second;
					if (MakeValidJudgement(year,month,day,hour,minute,second,default_year,tempyear,
							tempmonth,tempday,temphour,tempminute,tempsecond))
					{
						//if (subStrBegin!=string::npos)
						//{
						//	string subString = vData[j].substr(subStrBegin + result[0].size());
						//	MakeTimeAmPmFlag(subString,hour);
						//	MakeTimeZone(subString);
						//}
						resultTime = maketime(year,month,day,hour,minute,second);

						if(resultTime>min)
							return true;
					} 
				}
				else
				{
					std::cout<<m_strTimeRegexp[regexNum]<<" "<<vData[j]<<std::endl;
				}
			}
		}
	}	

	resultTime=min;
	return false;
}

