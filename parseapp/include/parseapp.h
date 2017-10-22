#ifndef _PARSE_APP_H_
#define _PARSE_APP_H_
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::pair;

#define DLL_PARSEAPP_PUBLIC   __attribute__ ((visibility("default")))
#define PARSEAPP_API extern "C" DLL_PARSEAPP_PUBLIC

/* app client type id */
typedef enum _APP_TYPE_ID_
{
	eApp_NULL=-1,

	eApp_Tencent,
	eApp_Sohu,
	eApp_Netease,
	eApp_Zaker,
	eApp_BBC,
	eApp_NewsAsia,
	eApp_IFeng,
	eApp_Sina,
	eApp_TouTiao,
	eApp_CCTV,
	eApp_Surge,
	
	eAppType_Count,
}AppTypeID;

/*article file format type*/
typedef enum _APP_FILE_TYPE_
{
	eFileType_html=0,
	eFileType_json,
	eFileType_xml,
}ArticleFileType;

#ifndef _NPCE_API_H_
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
#endif 

/*
 *功能：根据APP的移动端URL信息，计算出APP的ID和接口URL地址信息及接口格式
 *参数：@param murl,[input]APP移动端的URL
 * @param iurl,[output]接口的URL地址
 * @param format,[output]接口的文本格式
 *返回：APP的ID，当返回值为eApp_NULL时标识接口不支持该APP的抽取
 */
PARSEAPP_API AppTypeID parseapp_app_context(const string murl,string &iurl,string &newsid,ArticleFileType &format);

/*
 *功能：抽取APP页面正文相关信息
 *参数：@param,app,[input]APP的ID编号，由函数parseapp_app_context()获取的返回值。
 *@param,src,[input]根据parseapp_app_context()函数下载的接口地址文本源代码
 *@param,format,[input]根据parseapp_app_context()函数获取的文本格式
 *@param,result,[output]抽取的正文相关的信息
 *返回：成功返回0，非零标识失败
 */
PARSEAPP_API int parseapp_app_parse_content(const AppTypeID app, const string &src,const string newsid,const bool img,const bool flg,const ArticleFileType format,
	NpceResult &result);

/*
 *功能：返回最后一次执行的错误文本信息
 *返回：错误文本信息
 */
PARSEAPP_API string parseapp_app_last_error();

#endif
