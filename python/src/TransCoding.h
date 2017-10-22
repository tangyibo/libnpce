///////////////////////////////////////////////////////////////////////////////////
//
//		Author		:	stong.yang@gmail.com
//		CreationDate:	2010-9-10
//		Function	:	采用码表方式,编码转换,速度快准确易用
//		UpdateLog	:	1.Created on 2010-9-10
//		
///////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_TRANSCODING_H_
#define _INCLUDE_TRANSCODING_H_

class CTransCoding
{
public:
	//Gbk转换成Utf-8
	int GbkToUtf8(char *pSents,int iLen,char *pRetSents,int &iRetLen);
	//Utf-8转换成Gbk
	int Utf8ToGbk(char *pSents,int iLen,char *pRetSents,int &iRetLen);
public:
	//Big5转换成Utf-8
	int Big5ToUtf8(char *pSents,int iLen,char *pRetSents,int &iRetLen);
	//Utf-8转换成Big5
	int Utf8ToBig5(char *pSents,int iLen,char *pRetSents,int &iRetLen);
public:
	//日文转换成Utf-8
	int JapanToUtf8(char *pSents,int iLen,char *pRetSents,int &iRetLen);
	//Utf-8转换成日文
	int Utf8ToJapan(char *pSents,int iLen,char *pRetSents,int &iRetLen);
public:
	//韩语转换成Utf-8
	int KoreaToUtf8(char *pSents,int iLen,char *pRetSents,int &iRetLen);
	int Utf8ToKorea(char *pSents,int iLen,char *pRetSents,int &iRetLen);
private:
	//Unicode值转换成UTF8编码
	int UnicodeToUtf8(int iWordValue,char *pResult,int &iRetLen);
	//UTF8编码转换成Unicode值
	int Utf8ToUnicode(char *pWord,int iLen,int &iWordValue);

};


#endif //_INCLUDE_TRANSCODING_H_