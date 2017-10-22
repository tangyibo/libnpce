/*!
*    \brief 网页信息抽取接口（新闻、博客、任意网页）
*    \n     
*    \n     调用步骤示例如下：
*    \n     1, 启动抽取: Constor_Open 或者Constor_OpenByBufConfig(可根据需求使用相应的接口)
*    \n     2, 解析网页: Constor_PerformBuf 或者 Constor_PerformFile (可根据需求使用相应的接口)
*    \n     3, 信息抽取,获得抽取结果: Constor_GetResult
*    \n     4, 获取最近一次的出错信息: Constor_GetLastErrMsg
*    \n     5, 释放抽取句柄: Constor_Close
*    \n     
*    \n     注1: 当处理多个文件时，可循环调用以上步骤中的 2-4 
*    \n     注2: 所有输入的文件或者文件的内容均要求是UTF-8格式
*    \n     注3：使用前请参阅《Constor_User_Guide.doc》
*    \date     2014-03-11
*    \author     liqiang(liqiang@software.ict.ac.cn)
*/
#pragma once

#ifndef CONSTORAPI_H_
#define CONSTORAPI_H_

#include <string>
#include <vector>

using namespace std;
namespace nsConstor
{
#define LEVEL_MIN 1
#define LEVEL_1 1                    //不采用优化机制
#define LEVEL_2 2                    //采用节点过滤和关键字后过滤
#define LEVEL_3 3                    //采用字典
#define LEVEL_MAX 4

#define CODE_UTF8 0
#define CODE_GBK  1
#define CODE_OTHER 2


#define CONSTOR_NEWS            0x0001             /**< 新闻 */
#define CONSTOR_BLOG            0x0003             /**< 博客 */
#define CONSTOR_ANYPAGE         0x0005             /**< 任意网页 */




#define CONSTOR_CONTENT          0x0001             /**< 正文 content                对应二进制 0000 0001 */
#define CONSTOR_TITLE            0x0002             /**< 标题 tiltle                    对应二进制 0000 0010 */
#define CONSTOR_PUBLISHTIME      0x0004             /**< 发布时间 PublishTime        对应二进制 0000 0100 */
#define CONSTOR_LINK             0x0008             /**< 正文中的所有 url Link        对应二进制 0000 1000 */
#define CONSTOR_IMG              0x0010             /**< 正文中的所有图片 src 和 alt    对应二进制 0001 0000 */
#define CONSTOR_SOURCE           0x0020             /**< 来源 Source                    对应二进制 0010 0000 */
#define CONSTOR_AUTHOR           0x0040             /**< 作者 Author                    对应二进制 0100 0000 */

	/**< 时间结构体 */
	typedef struct _stConstorDateTime
	{
		unsigned int nYear, nMonth, nDay, nHour, nMinute, nSecond;
	}tConstorDateTime;

	/**< 图片信息结构体 */
	typedef struct _stImgInfo
	{
		std::string strUrl; /**< 图片链接 */
		std::string strAlt; /**< 图片提示文字 */
	}tImgInfo;



	/**< 结果保存结果的结构体 */
	typedef struct Extr_Result
	{
		std::string strTitle;                            /**< 标题 */
		std::string strSource;                           /**< 来源 */
		std::string strAuthor;                           /**< 作者 */
		_stConstorDateTime DataTime;                     /**< 时间 */
		std::vector<_stImgInfo>        vecImgInfo;       /**< 图片链接 */
		std::vector<std::string>    vecstrLinkAddress;   /**< 链接 */
		std::string strContent;                          /**< 正文 */
	}KEY_INFO;


#define CONSTOR_API
#ifndef NULL
#define NULL 0
#endif
	typedef void CONSTOR;

	/*!
	* \fn CONSTOR_API CONSTOR * Constor_Open( int iExtractType, int iOptimizeLevel,  const std::string & sConfigFileName, const std::string & sSourceRegFile = "");
	* \brief     启动抽取，抽取配置以函数参数的形式传入，主要完成抽取参数初始化、读取过滤规则等工作。
	*            配置文件格式参考User_Guide。
	* \param     [in]    iExtractType          输入网页的类型： 新闻 CONSTOR_NEWS； 博客 CONSTOR_BLOG；任意网页 CONSTOR_ANYPAGE
	* \param     [in]   iOptimizeLevel        优化级别：1代表不需要配置文件，2代表需要配置文件3是指只需要配置文件中的定位部分（Start）
	* \param     [in]    sConfigFileName       保存相关的后续处理关键字文件名
	* \param     [in]    sSourceRegFile        抽取作者来源用到的正则式文件
	* \return    抽取的控制句柄
	* \date         2014-03-11
	* \author     liqiang(liqiang@software.ict.ac.cn)
	*/
	CONSTOR_API CONSTOR * Constor_Open(int iExtractType, int iOptimizeLevel = 1, const std::string & sConfigFileName = "", const std::string & sSourceRegFile = "");


	/*!
	* \fn CONSTOR_API CONSTOR * Constor_OpenByBufConfig( int iExtractType, int iOptimizeLevel,  const std::string & sConfigFileName, const std::string & sSourceRegBuf = "");
	* \brief     启动抽取，抽取配置以函数参数的形式传入，主要完成抽取参数初始化、读取过滤规则等工作。
	*            配置文件格式参考User_Guide。
	* \param     [in]    iExtractType          输入网页的类型： 新闻 CONSTOR_NEWS； 博客 CONSTOR_BLOG；任意网页 CONSTOR_ANYPAGE
	* \param     [in]   iOptimizeLevel        优化级别：1代表不需要配置文件，2代表需要配置文件3是指只需要配置文件中的定位部分（Start）
	* \param     [in]    sConfigBuf          保存相关的后续处理关键字，每个关键字用空格隔开。
	* \param     [in]    sSourceRegBuf        抽取作者来源用到的正则式文件流
	* \return    抽取的控制句柄
	* \date         2014-03-11
	* \author     liqiang(liqiang@software.ict.ac.cn)
	*/
	CONSTOR_API CONSTOR * Constor_OpenByBufConfig(int iExtractType, int iOptimizeLevel = 1, const std::string & sConfigBuf = "", const std::string & sSourceRegBuf = "");


	/*!
	* \fn CONSTOR_API CONSTOR * Constor_OpenByBufConfig(  int iExtractType, const std::string & sConfigBuf = "", const std::string & sFilterNodeBuf = "", const std::string & sDictionaryDir = "" );
	* \brief     启动抽取，抽取配置以函数参数的形式传入，主要完成抽取参数初始化、读取过滤规则等工作。
	*            配置文件格式参考User_Guide。【仅供IEAdapter使用】
	* \param     [in]    iExtractType          输入网页的类型： 新闻 CONSTOR_NEWS； 博客 CONSTOR_BLOG；任意网页 CONSTOR_ANYPAGE
	* \param     [in]    sConfigBuf          保存相关的后续处理关键字，每个关键字用空格隔开。
	* \param     [in]    sFilterNodeBuf      保存过滤配置内容
	* \param     [in]    sDictionaryDir        抽取作者来源用到的词典所在目录
	* \return    抽取的控制句柄
	* \date         2014-03-11
	* \author     liqiang(liqiang@software.ict.ac.cn)
	*/
	CONSTOR_API CONSTOR * Constor_OpenByBufConfig(int iExtractType, const std::string & sConfigBuf = "", const std::string & sFilterNodeBuf = "", const std::string & sDictionaryDir = "");
	/*!
	*   CONSTOR_API bool Constor_PerformBuf(  CONSTOR * handler , const std::string & sFileBuf ,bool bIsUTF8 int iResultType =1,  int iKeyInfoType =1);
	* \brief     信息抽取，主要基于网页的DOM树进行抽取操作，获得抽取结果。
	*            iKeyInfoType 为抽取的关键信息组合，每一种关键信息对应iKeyInfoType( 0000 0000 )的一个二进制位。
	*            需要抽取的，将对应的二进制位设置为1，不需要抽取的设置为0。
	*            下面表格左边是高位，右边是低位。可以参照头部的宏定义 CONSTOR_AUTHOR CONSTOR_SOURCE CONSTOR_IMG CONSTOR_LINK CONSTOR_PUBLISHTIME CONSTOR_TITLE CONSTOR_CONTENT
	*            七种关键信息分别为
	*            Author 作者，Source 来源，Img 正文中的所有图片URL，Link 正文中的所有URL，PublishTime 发布时间，Title 标题，Content 正文。
	*            新闻所有关键信息都可以抽取。
	*            博客仅能抽取图片链接、地址链接、发布时间、标题和正文。
	*            任意网页仅能抽取图片链接、地址链接、发布时间、标题和正文。
	*            组合见下表。
	*
	*                     CONSTOR_AUTHOR|CONSTOR_SOURCE|CONSTOR_IMG|CONSTOR_LINK|CONSTOR_PUBLISHTIME|CONSTOR_TITLE|CONSTOR_CONTENT
	*                    ┏------------┳------------┳---------┳------------┳------------------┳------------┳-------------┓
	*                    ┃   Author   ┃   Source   ┃    Img  ┃    Link    ┃    PublishTime   ┃    Title   ┃   Content   ┃
	*                    ┣------------╋------------╋---------╋------------╋------------------╋------------╋-------------┫
	*            新闻    ┃      √     ┃      √     ┃    √    ┃     √      ┃         √        ┃     √      ┃      √      ┃
	*                    ┣------------╋------------╋---------╋------------╋------------------╋------------╋-------------┫
	*            博客    ┃            ┃            ┃    √    ┃     √      ┃         √        ┃     √      ┃      √      ┃
	*                    ┣------------╋------------╋---------╋------------╋------------------╋------------╋-------------┫
	*          任意网页  ┃            ┃            ┃    √    ┃     √      ┃         √        ┃     √      ┃      √      ┃
	*                    ┗------------┻------------┻---------┻------------┻------------------┻------------┻-------------┛
	*
	*            各种类型的网页能抽取的关键信息可以任意组合。
	*            比如需要抽取新闻的Author、Img、Link、Title、Content，那么对应的iKeyInfoType为
	*            iKeyInfoType = CONSTOR_AUTHOR | CONSTOR_IMG | CONSTOR_LINK | CONSTOR_TITLE | CONSTOR_CONTENT
	*
	* \param     [in]    handle        抽取程序的控制句柄
	* \param     [in]    sFileBuf      网页内容在内存中的缓冲区指针
	* \param     [in]    bIsUTF8       编码类型是否为UTF-8
	* \param     [in]    iResultType   输出结果格式：0 保留html标签；1 去掉html标签并格式化输出
	* \param     [in]    iKeyInfoType  关键信息类型组合
	* \return    true：操作成功；false：操作失败
	* \date      2014-03-11
	* \author     liqiang(liqiang@software.ict.ac.cn)
	*/
	CONSTOR_API bool Constor_PerformBuf(CONSTOR * handler, const std::string & sFileBuf, bool bIsUTF8, int iResultType = 1, int iKeyInfoType = 1);


	/*!
	* \fn CONSTOR_API bool Constor_PerformFile( CONSTOR * handler , const std::string & sFileName, bool bIsUTF8, int iResultType =1,  int iKeyInfoType =1 );
	* \brief    功能同Constor_PerformBuf，但是抽取的来源为文件
	* \param    [in]    handler       抽取程序的控制句柄
	* \param    [in]	sFileName     网页文件名
	* \param    [in]    bIsUTF8       编码类型是否为UTF-8
	* \param    [in]	iResultType   输出结果格式：0 保留html标签；1 去掉html标签并格式化输出
	* \param    [in]	iKeyInfoType  关键内容信息类型组合
	* \return   true：操作成功；false：操作失败
	* \date	    2014-03-11
	* \author	liqiang(liqiang@software.ict.ac.cn)
	*/
	CONSTOR_API bool Constor_PerformFile(CONSTOR * handler, const std::string & sFileName, bool bIsUTF8, int iResultType = 1, int iKeyInfoType = 1);


	/*!
	* \fn CONSTOR_API bool Constor_GetResult( CONSTOR * handler , std::string & sXML );
	* \brief    获得XML格式的抽取结果。
	*            返回串格式如下（如果没有某项，则该项为空串）：
	*            <Constor><Source>来源</Source><Author>作者</Author><PublishTime>发布时间</PublishTime>
	*            <Img><ID>0</ID><Src>正文中第一张图片网址</Src><Alt>正文中第一张图片图片Alt内容</Alt><ID>1</ID><Src>正文中第二张图片网址</Src><Alt>正文中第二张图片图片Alt内容</Alt></Img>
	*            <Link><ID>0</ID><Src>正文中第一个链接</Src><ID>1</ID><Src>正文中第二个链接</Src></Link>
	*            <Content>正文</Content></Constor>
	* \param    [in]    handler        抽取程序的控制句柄
	* \param    [out]    sXML        返回xml结构的结果
	* \return    true：操作成功；false：操作失败
	* \date     2014-03-11
	* \author    liqiang(liqiang@software.ict.ac.cn)
	*/
	CONSTOR_API bool Constor_GetResult(CONSTOR * handler, std::string & sXML);

	/*!
	* \fn CONSTOR_API bool Constor_GetResult( CONSTOR * handler , std::vector<Extr_Result>& vecResult);
	* \brief    获得XML格式的抽取结果。
	*            返回串格式如下（如果没有某项，则该项为空串）：
	*            <Constor><Source>来源</Source><Author>作者</Author><PublishTime>发布时间</PublishTime>
	*            <Img><ID>0</ID><Src>正文中第一张图片网址</Src><Alt>正文中第一张图片图片Alt内容</Alt><ID>1</ID><Src>正文中第二张图片网址</Src><Alt>正文中第二张图片图片Alt内容</Alt></Img>
	*            <Link><ID>0</ID><Src>正文中第一个链接</Src><ID>1</ID><Src>正文中第二个链接</Src></Link>
	*            <Content>正文</Content></Constor>
	* \param    [in]    handler        抽取程序的控制句柄
	* \param    [out]   vecResult 返回包含xml结构的vector
	* \return    true：操作成功；false：操作失败
	* \date     2014-03-11
	* \author    liqiang(liqiang@software.ict.ac.cn)
	*/
	CONSTOR_API bool Constor_GetResult(CONSTOR * handler, std::vector<Extr_Result>& vecResult);

	/*!
	*        Constor_GetResult_with_page
	* \brief    获得XML格式的抽取结果，同时将抽取到的内容在原网页中标注出来
	* \param    [in]    handler        抽取程序的控制句柄
	* \param    [out]    sXML        返回xml结构的结果
	* \param    [out]    sEditedPage  返回加入了标注信息的网页源码
	* \return    true 操作成功  false 操作失败
	* \date     2014-03-11
	* \author    liqiang(liqiang@software.ict.ac.cn)
	*/

	CONSTOR_API bool Constor_GetResult_with_page(CONSTOR * handler, std::string & sXML, std::string &sEditedPage);


	/*!
	* \fn CONSTOR_API bool Constor_Close( CONSTOR * & handler )
	* \brief    释放抽取句柄，结束抽取工作
	* \param    [in]    handler    抽取程序的控制句柄
	* \return   true：成功释放抽取句柄；false：释放抽取句柄失败
	* \date	    2014-03-11
	* \author	liqiang(liqiang@software.ict.ac.cn)
	*/
	CONSTOR_API bool Constor_Close(CONSTOR * & handler);


	/*!
	* \fn CONSTOR_API string Constor_GetLastErrMsg()
	* \brief    获取最近一次的出错信息
	* \return   string 出错信息字符串
	* \date     2014-03-11
	* \author   liqiang(liqiang@software.ict.ac.cn)
	*/
	CONSTOR_API std::string Constor_GetLastErrMsg();

	/*!
	* \fn CONSTOR_API string Constor_GetVersion()
	* \brief    获取当前Constor的版本。
	* \return   string 版本编号。
	* \date     2014-03-11
	* \author   liqiang(liqiang@software.ict.ac.cn)
	*/
	CONSTOR_API std::string Constor_GetVersion();
}
#endif
