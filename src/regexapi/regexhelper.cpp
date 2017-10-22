#include "regexhelper.h"
#include "boost/regex.hpp"

namespace RegexAPI
{

RegexHelper::RegexHelper()
{
}

RegexHelper::~RegexHelper()
{
}

bool RegexHelper::IsMatch(const char* input,const char* pattern)
{
    boost::regex reg( pattern , boost::regex::perl|boost::regex::icase);
    bool ret = boost::regex_search( input , reg);
    return ret;
}

std::string RegexHelper::Match(const char* input,const char* pattern,int group)
{
    if(group < 0)group = 0;
    boost::cmatch mat;
    boost::regex reg( pattern , boost::regex::perl|boost::regex::icase);
    bool success = boost::regex_search( input, mat, reg);
    if(success){
        if(mat[group].matched){
            return std::string(mat[group]);
        }
    }
    return std::string("");
}

int RegexHelper::Match(const char* input,const char* pattern,std::vector<std::string>& results)
{
    boost::cmatch mat;
    boost::regex reg( pattern , boost::regex::perl|boost::regex::icase );
    bool success =boost::regex_search( input, mat, reg);
    int total = 0;
    if(success)
	{ //如果匹配成功
        //cout << "match success" << endl;
        //显示所有子串
        for(boost::cmatch::iterator itr=mat.begin(); itr!=mat.end(); ++itr){
            //       指向子串对应首位置        指向子串对应尾位置          子串内容
            //cout << itr->first-szStr << ' ' << itr->second-szStr << ' ' << *itr << endl;
            results.push_back(std::string(*itr));
            total++ ;
        }
    }
    return total;
}

int RegexHelper::Matches(const char* input,const char* pattern)
{
    boost::regex reg( pattern, boost::regex::perl|boost::regex::icase); //查找字符串里的数字
    boost::cregex_iterator itrBegin = make_regex_iterator(input,reg); //(szStr, szStr+strlen(szStr), reg);
    boost::cregex_iterator itrEnd;
    int total = 0;
    for(boost::cregex_iterator itr=itrBegin; itr!=itrEnd; ++itr){
        //cout << (*itr)[0].first-szStr << ' ' << (*itr)[0].second-szStr << ' ' << *itr << endl;
        total++;
    }
    return total;

}

int RegexHelper::Matches(const char* input,const char* pattern,std::vector<std::string>& results,int group)
{
    if(group < 0)group = 0;
    boost::regex reg( pattern, boost::regex::perl|boost::regex::icase); //查找字符串里的数字
    boost::cregex_iterator itrBegin = make_regex_iterator(input,reg); //(szStr, szStr+strlen(szStr), reg);
    boost::cregex_iterator itrEnd;
    int total = 0;
    for(boost::cregex_iterator itr=itrBegin; itr!=itrEnd; ++itr){
        //cout << (*itr)[0].first-szStr << ' ' << (*itr)[0].second-szStr << ' ' << *itr << endl;
        results.push_back(std::string((*itr)[group]));
        total++;
    }
    return total;
}

std::string RegexHelper::ReplaceFirst(const char* input,const char* pattern,const char* repValue)
{
    //( 1 )   ((  3  )  2 )((  5 )4)(    6    )
    //(/w+)://((/w+/.)*/w+)((//w*)*)(//w+/./w+)?
    //^协议://网址(x.x...x)/路径(n个/字串)/网页文件(xxx.xxx)
    //const char *szReg = "(\\w+)://((\\w+\\.)*\\w+)((/\\w*)*)(/\\w+\\.\\w+)?";
    //const char *szStr = "http://www.cppprog.com/2009/0112/48.html";
    //repValue = ""
    boost::regex reg( pattern , boost::regex::perl|boost::regex::icase);
    std::string sret = boost::regex_replace( std::string(input), reg, std::string(repValue));
    return sret;
}

std::string RegexHelper::ReplaceAll(const char* input,const char* pattern,const char* repValue)
{
    //string s1 = "(<)|(>)|(&)";
    //string s2 = "(?1<)(?2>)(?3&)";
    boost::regex reg( pattern , boost::regex::perl|boost::regex::icase);
    std::string sret = boost::regex_replace( std::string(input), reg, std::string(repValue), boost::match_default | boost::format_all);
    return sret;
}

int RegexHelper::Split(const char* input,const char* pattern,std::vector<std::string>& results)
{
    boost::regex reg(pattern, boost::regex::perl|boost::regex::icase);  //按/符拆分字符串
    boost::cregex_token_iterator itrBegin = make_regex_token_iterator(input,reg,-1); //使用-1参数时拆分，使用其它数字时表示取第几个子串，可使用数组取多个串
    boost::cregex_token_iterator itrEnd;
    int total = 0;
    for(boost::cregex_token_iterator itr=itrBegin; itr!=itrEnd; ++itr){
        //cout << *itr << endl;
        results.push_back(std::string(*itr));
        total++;
    }
    return total;
}

int RegexHelper::Split(const char* input,const char* pattern,std::vector<int>& subs,std::vector<std::string>& results)
{
    boost::regex reg(pattern, boost::regex::perl|boost::regex::icase);  //取/的前一字符和后一字符（这个字符串形象貌似有点邪恶-_-）
    boost::cregex_token_iterator itrBegin = make_regex_token_iterator(input,reg,subs); //使用-1参数时拆分，使用其它数字时表示取第几个子串，可使用数组取多个串
    boost::cregex_token_iterator itrEnd;
    int total = 0;
    for(boost::cregex_token_iterator itr=itrBegin; itr!=itrEnd; ++itr){
        //cout << *itr << endl;
        results.push_back(std::string(*itr));
        total++;
    }
    return total;
}

int RegexHelper::RegexSplit(const char* input,const char* pattern,std::vector<std::string>& results)
{
	//提取子串
	boost::cmatch mat;
	boost::regex reg(pattern, boost::regex::perl|boost::regex::icase);
	int total = 0;;

	bool success=boost::regex_match( input, mat, reg);
	if(success) //如果匹配成功
	{
		for(boost::cmatch::iterator itr=mat.begin(); itr!=mat.end(); ++itr)
		{
			//    指向子串对应首位置        指向子串对应尾位置          子串内容
			//cout << itr->first-szStr << ' ' << itr->second-szStr << ' ' << *itr << endl;
			results.push_back(std::string(*itr));
			total++;
		}
	}

	return total;
}

}
