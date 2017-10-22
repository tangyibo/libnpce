#ifndef _HTML_TIME_PARSER_H_
#define _HTML_TIME_PARSER_H_
#include <time.h>
#include <string>
#include <vector>
using std::string;
using std::vector;

#define REGEX_TIME_PARSER_NUM_COUNT 33	/**< 时间正则串个数 */

class CTimeParser
{
public:
	CTimeParser();
	virtual ~CTimeParser();

	void SetMinYear(int year);
	const int GetMinYear() { return m_iMinYear; }
	/*! 
	 * \fn bool TimeParser (const string &data, time_t& resultTime)
	 * \brief 时间抽取函数
	 * \n 从特定字符串中进行正则匹配时间正则串，匹配成功后进行特定格式的时间转换
	 * \param[in,out]	data	    待匹配的字符串
	 * \param[in,out]	resultTime  匹配成功转换的时间
	 * \return 成功抽取返回true，失败返回false
	 */
	bool TimeParser (const string &data,const time_t min, time_t& resultTime);

private:
	/*! 
	 * \fn string GetMonth(string monthString)
	 * \brief 获取对应月份函数，
	 * \n 针对特定月份string，得到对应的数字string
	 * \param[in]	monthString	   月份string
	 * \return string
	 */
	string GetMonth(string monthString);

	/*!
	 * \fn bool GetTime(vector<string> &result,int curser,string&tempyear,string&tempmonth,string&tempday,string &temphour,string &tempminute,string &tempsecond)
	 * \brief 时间转换函数
	 * \n 根据正则匹配返回的vector<string>进行提取，该函数一定要保证pcreapi返回的vector内容次序的正确性
	 * \param[in,out]	result	       正则匹配获取的内容
	 * \param[in,out]	tempyear	   提取的年信息
	 * \param[in,out]	tempmonth	   提取的月信息
	 * \param[in,out]	tempday	       提取的天信息
	 * \param[in,out]	temphour	   提取的时信息
	 * \param[in,out]	tempminute	   提取的分信息
	 * \param[in,out]	tempsecond	   提取的秒信息
	 * \return 成功提取返回true，否则返回flase
	 */
	bool GetTime(vector<string> &result,int curser,string&tempyear,string&tempmonth,string&tempday,string &temphour,string &tempminute,string &tempsecond);

	/*!
	 * \fn time_t maketime(int year, int mon, int mday, int hour, int min, int sec, int isdst=1)
	 * \brief 特定时间格式转换函数
	 * \n 给定的数据生成time_t格式的时间结果
	 * \param[in]	year	      提供的年信息
	 * \param[in]	mon	          提供的月信息
	 * \param[in]	mday	      提供的日信息
	 * \param[in]	hour	      提供的时信息
	 * \param[in]	min	          提供的分信息
	 * \param[in]	sec	          提供的秒信息
	 * \param[in]	isdst	      默认为1
	 * \return     time_t
	 */
	time_t maketime(int year, int mon, int mday, int hour, int min, int sec, int isdst=1);

	/*!
	 * \fn vector<string> SplitString(const string& s, const string& delim, const bool keep_empty=true)
	 * \brief 特定子串切分函数
	 * \n 给定一个特定子串，将待操作字符串进行切分成vector<string>
	 * \param[in,out]	s	      待切分的字符串
	 * \param[in,out]	delim	  用于切分的子字符串
	 * \param[in]	keep_empty	  判断信息，默认为true
	 * \return     vector<string>
	 */
	vector<string> SplitString(const string& s, const string& delim, const bool keep_empty=true);

	/*! 
	 * \fn void MakeTimeAmPmFlag(string &subString,int& hour)
	 * \brief am pm 获取函数
	 * \n 主要针对pm进行小时的转换
	 * \param[in,out]	subString	      待处理的的字符串
	 * \param[in,out]	hour	          待处理的小时值
	 * \return     无
	 */
	void MakeTimeAmPmFlag(string &subString,int& hour);

	/*! 
	 * \fn void MakeTimeZone(string &subString)
	 * \brief 获取时区函数
	 * \param[in,out]	subString	      待处理的的字符串
	 * \return     无
	 */
	void MakeTimeZone(string &subString);

	/*! 
	 * \fn bool MakeValidJudgement(int& year,int& month,int& day,int&hour,int&minute,int&second,int localyear,string&tempyear,string&tempmonth,string&tempday,string temphour="0",string tempminute="0",string tempsecond="0")
	 * \brief 时间数据有效性判断函数，主要对其有效性进行判断，以筛除无用时间，比如超过当前时间的时间，月超过12的时间等等
	 * \param[in,out]	year	      待判定的年信息
	 * \param[in,out]	month	      待判定的月信息
	 * \param[in,out]	day	          待判定的日信息
	 * \param[in,out]	hour	      待判定的时信息
	 * \param[in,out]	minute	      待判定的分信息
	 * \param[in,out]	second	      待判定的秒信息
	 * \return     有效返回true，失败返回false
	 */
	bool MakeValidJudgement(int& year,int& month,int& day,int&hour,int&minute,int&second,int localyear,string&tempyear,string&tempmonth,string&tempday,string temphour="0",string tempminute="0",string tempsecond="0");

public:
	string	m_strTimeRegexp[REGEX_TIME_PARSER_NUM_COUNT];
	/**< 时区正则串个数 */
	size_t m_vecTimeZoneRegexpNum ;
	/**< am pm 正则匹配串 */
	string m_strAmPmRegexp ;
	/**< 时区正则串静态变量数组信息 */
	string m_strTimeZoneRegexp[2];
	/**< 时间正则串 抽取内容vector 抽取信息定位静态变量数组信息
	 *\brief 该数组针对pcreapi库而言，每成功匹配的正则串会返回一个特定vector<string>,
	 * 为了快速定位到我们需要提前的信息，可以根据每个正则串定位特定vector的位置信息进行提取
	 */
	size_t  m_iMinLIineNum ;   ///获取最小有效时间文本行  常量(对复杂文本而已，需要进行分割从而逐行匹配，这里指对一个文本最多获取行数)
	int  m_iMinYear ;       ///获取最小有效时间年份 常量
};

#endif
