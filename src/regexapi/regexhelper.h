/*---------------------------------------------------------------
* 功能：基于boost的正则表达式API封装
*---------------------------------------------------------------*/
#ifndef REGEX_HELPER_H_INCLUDE
#define REGEX_HELPER_H_INCLUDE
#include<string>
#include<vector>

namespace RegexAPI
{

class RegexHelper
{
public:
    RegexHelper();
    virtual ~RegexHelper();
    /*
    * 是否包含匹配字符串
    * @param: input 输入字符串
    * @param：pattern 正则表达式
    */
    static bool IsMatch(const char* input,const char* pattern);
    /*
    * 获取首个匹配字符串或其字串
    * @param: input 输入字符串
    * @param：pattern 正则表达式
    * @param：group 子捕获组
    */
    static std::string Match(const char* input,const char* pattern,int group = 0);
    /*
    * 获取首个匹配字符串所有捕获组
    * @param: input 输入字符串
    * @param：pattern 正则表达式
    * @param: results 输出的字符串数组
    */
    static int Match(const char* input,const char* pattern,std::vector<std::string>& results);
    /*
    * 匹配字符串数目
    * @param: input 输入字符串
    * @param：pattern 正则表达式
    */
    static int Matches(const char* input,const char* pattern);
    /*
    * 输出所有匹配字符串或其捕获组
    * @param: input 输入字符串
    * @param：pattern 正则表达式
    * @param: results 输出的字符串数组
    * @param：group 捕获组
    */
    static int Matches(const char* input,const char* pattern,std::vector<std::string>& results,int group = 0);
    /*
    * 替换首个匹配字符串
    * @param: input 输入字符串
    * @param：pattern 正则表达式
    * @param：repValue 被替换值，可以是捕获组的组合
    */
    static std::string ReplaceFirst(const char* input,const char* pattern,const char* repValue);
    /*
    * 替换所有匹配字符串
    * @param: input 输入字符串
    * @param：pattern 正则表达式
    * @param：repValue 被替换值，可以是捕获组的组合
    */
    static std::string ReplaceAll(const char* input,const char* pattern,const char* repValue);
    /*
    * 分割字符串并输出结果
    * @param: input 输入字符串
    * @param：pattern 正则表达式
    * @param: results 输出的字符串数组
    */
    static int Split(const char* input,const char* pattern,std::vector<std::string>& results);
    /*
    * 分割字符串并根据捕获组输出
    * @param: input 输入字符串
    * @param：pattern 正则表达式
    * @param：subs 捕获组
    * @param: results 输出的字符串数组
    */
    static int Split(const char* input,const char* pattern,std::vector<int>& subs,std::vector<std::string>& results);

    /*
    * 提取出正则表达式括号对应的子串
    * @param: input 输入字符串
    * @param：pattern 正则表达式
    * @param: results 输出的子串数组
    */
    static int RegexSplit(const char* input,const char* pattern,std::vector<std::string>& results);
protected:
private:
};

}
#endif // REGEX_HELPER_H_INCLUDE
