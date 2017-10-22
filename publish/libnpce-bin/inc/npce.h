/**
 * 功能：新闻文章正文抽取组件
 * 作者：tangyibo
 * 时间：2016-08-20
 * 说明：本组件可将UTF-8编码格式的HTML源码进行正文抽取、图片抽取、链接抽取，如果不是UTF-8格式，
 * 需由用户先进行编码格式转换后才可使用。
 */
#ifndef _NPCE_API_H_
#define _NPCE_API_H_
#include <time.h>
#include <string>
#include <vector>
#include <utility>
using std::string;
using std::vector;
using std::pair;

#define DLL_NPCE_PUBLIC   __attribute__ ((visibility("default")))
#define NPCE_API extern "C" DLL_NPCE_PUBLIC

typedef void npce;

/*定义正文抽取的元素的类型*/
enum eExtractType
{
	NPCE_ET_TITLE=(1<<0),	/** 标题 */
	NPCE_ET_CONTENT=(1<<1),	/** 正文 */
	NPCE_ET_PUBTIME=(1<<2),	/** 时间 */
	NPCE_ET_SOURCE=(1<<3),	/** 来源 */
	NPCE_ET_IMAGES=(1<<4),	/** 正文中的所有图片链接 */
	NPCE_ET_LINKS=(1<<5),	/** 正文中的所有链接 */
};

/* 定义抽取全部信息的宏*/
#define NPCE_OPT_ALL  (NPCE_ET_TITLE|NPCE_ET_CONTENT|\
		NPCE_ET_PUBTIME|NPCE_ET_SOURCE|NPCE_ET_IMAGES|\
		NPCE_ET_LINKS)

#define IS_TITLE(type) ((type & NPCE_ET_TITLE) > 0)
#define IS_CONTENT(type) ((type & NPCE_ET_CONTENT) > 0)
#define IS_PUBTIME(type) ((type & NPCE_ET_PUBTIME) > 0)
#define IS_SOURCE(type) ((type & NPCE_ET_SOURCE) > 0)
#define IS_IMAGES(type) ((type & NPCE_ET_IMAGES) > 0)
#define IS_LINKS(type) ((type & NPCE_ET_LINKS) > 0)

/* 函数返回值的错误码 */
typedef enum
{
	NPCE_CODE_UNKOWN_ERROR=-1,		//未知错误
	NPCE_CODE_OK = 0,			//执行成功

	NPCE_CODE_BAD_FUNC_PARAM,		//无效的参数
	NPCE_CODE_INVALID_OPTION,		//无效的OPTION选项
	NPCE_CODE_OUT_OF_MEMORY,		//内存溢出错误
	NPCE_CODE_TIDY_HTML_FAILED,		//HTML整理失败
	NPCE_CODE_INVALID_HTML_DOM,		//无效的HTML结构
}NpceCode;

/*参数类型定义，用于函数npce_setopt和npce_getopt；两个
 * 函数第二个参数的值。*/
typedef enum
{
	NPCEOPT_MARKED_IMG=1,	//设置是否在正文中显示图片位置
	NPCEOPT_LINK_ANCHOR, 	//定义是否抽取正文链接的锚文本
	NPCEOPT_FILE_FILTER,	//设置过滤文件的全路径，每条过滤文本独占一行
	NPCEOPT_HTML_TIDY,	//定义在抽取HTML信息前进行HTML整理
}NpceOpt;

/*正文抽取的结果返回结构*/
typedef struct sResultValues
{
	string title;
	string content;
	time_t pubtime;
	string source;
	string srcurl;
	vector< pair<string,string> > imglist;
	vector< pair<string,string> > linklist;
}NpceResult;

/****************************************************************/
/* 函数名：npce*  npce_open(const int type=NPCE_ET_TITLE|NPCE_ET_CONTENT);
 * 功能：创建一个NPCE句柄
 * @param: type [in] ,整型，要抽取的正文元素类型
 * @return:成功返回句柄（非零），失败返回NULL
 * */
NPCE_API npce*  npce_open(const int type=NPCE_ET_TITLE|NPCE_ET_CONTENT);

/*函数名：int npce_setopt(npce * h, NpceOpt opt, ...);
 *功能：设置选项参数，详见NpceOpt的定义注释部分
 *@param: h [in] ,npce类型，句柄
 *@param: opt [in] , NpceOpt , 参数类型
 *@return:成功返回0,其他为失败。
 **/
NPCE_API NpceCode npce_setopt(npce * h, NpceOpt opt, ...);

/* 函数名：int npce_getopt(npce * h, NpceOpt opt, ...);
  *功能：获取选项参数，详见NpceOpt的定义注释部分
  *@param: h [in] ,npce类型，句柄
  *@param: opt [in] , NpceOpt , 参数类型
  *@return:成功返回0，其他为失败。
  **/
NPCE_API NpceCode npce_getopt(npce * h, NpceOpt opt, ...);

/* 函数名：int npce_perform(npce * h,const char *html,const char *url);
 *功能：执行正文抽取
 *@param: h [in] ,npce类型，句柄
 *@param: html [in] , 字符串，要抽取的HTML源码,需为UTF-8编码
 *@param: url [in] , 字符串, HTML源码的URL绝对路径
 *@return:成功返回0，其他为失败。
 **/
NPCE_API NpceCode npce_perform(npce * h,const char *html,const char *url);

/* 函数名：int npce_result(npce * h,NpceResult &result);
 *功能：当npce_perform执行成功时获取正文结果
 *@param: h [in] ,npce类型，句柄
 *@param: result [out] , NpceResult,正文结果
 *@return:成功返回0，其他为失败。
 **/
NPCE_API NpceCode npce_result(npce * h,NpceResult &result);

/* 函数名：string npce_error(npce * h);
 *功能：获取最后一次的错误信息
 *@param: h [in] ,npce类型，句柄、
 *@return:返回错误的字符串信息
 **/   
NPCE_API string npce_error(npce * h);

/*函数名：void npce_close(npce* h);
 *功能: 关闭NPCE句柄，释放内存空间
 *说明：请勿多次释放
 *@param: h [in] ,npce类型，句柄、
 **/ 
NPCE_API void npce_close(npce* h);

/***********************************************************/

/* 函数名：string npce_clean_htmltags(const char *html);
 *功能：清理HTML中的标签，以获取HTML的纯文本
 *@param: html [in] , 字符串，HTML的源码字符串
 *@return:成功返回清理后的纯文本字符串，失败返回空
 **/
NPCE_API string npce_clean_htmltags(const char *html);

/*函数名：int npce_html_links_wich_anchor(const char *html,const char *url,vector<TwoValueItem> &links);
 *功能：通过给定的HTML源字符串，抽取其中所有<a>标签的链接和对应的锚文本
 *@param: html [in] ,字符串，HTML源码字符串
 *@param: url  [in] ,字符串，HTML源码的URL绝对路径
 *@param: links [out], 二值数组，返回<link,anchor>对数组值
 *@return :成功返回0，其他为失败。
 * */
NPCE_API NpceCode npce_html_links_wich_anchor(const char *html,const char *url,vector< pair<string,string> > &links);

/*函数名：int npce_search_html_links(const char *html,const char *url,vector<string> &links);
 *功能：通过给定的HTML源字符串，抽取其中所有<a>标签的链接
 *@param: html [in] ,字符串，HTML源码字符串
 *@param: url  [in] ,字符串，HTML源码的URL绝对路径
 *@param: links [out], 字符串数组，返回得到的链接数组值
 *@return :成功返回0，其他为失败。
 * */
NPCE_API NpceCode npce_search_html_links(const char *html,const char *url,vector<string> &links);

/*函数名：int npce_search_iframe_links(const char *html,const char *url,vector<string> &links);
 *功能：通过给定的HTML源字符串，抽取其中所有的<ifram>标签内的链接
 * @param: html [in] ,字符串，HTML源码字符串
 * @param: url  [in] ,字符串，HTML源码的URL绝对路径
 * @param: links [out], 字符串数组，返回得到的链接数组值
 * @return :成功返回0，其他为失败。
 */
NPCE_API NpceCode npce_search_iframe_links(const char *html,const char *url,vector<string> &links);

/*函数名：int npce_html_links_wich_anchor_filter(const char *html,const char *url,vector<TwoValueItem> &links);
 * 功能：通过给定的HTML源字符串，抽取其中符合正则的链接和对应的锚文本
 * @param: html [in] ,字符串，HTML源码字符串
 * @param: url  [in] ,字符串，HTML源码的URL绝对路径
 * @param: regex [in] ,字符串，过滤link链接的正则表达式
 * @param: links [out], 二值数组，返回经链接正则过滤后的<link,anchor>对数组值
 * @return :成功返回0，其他为失败。
 * */
NPCE_API NpceCode npce_html_links_wich_anchor_filter(const char *html,const char *url,const char *regex,vector< pair<string,string> > &links);

/*函数名：int npce_search_html_links_filter(const char *html,const char *url,vector<string> &links);
 * 功能：通过给定的HTML源字符串，抽取其中符合正则的链接和对应的锚文本
 * @param: html [in] ,字符串，HTML源码字符串
 * @param: url  [in] ,字符串，HTML源码的URL绝对路径
 * @param: regex [in] ,字符串，过滤link链接的正则表达式
 * @param: links [out], 字符串数组，返回经正则过滤后的链接对数组值
 * @return :成功返回0，其他为失败。
 * */
NPCE_API NpceCode npce_search_html_links_filter(const char *html,const char *url,const char *regex,vector<string> &links);

#endif /* _NPCE_API_H_ */
