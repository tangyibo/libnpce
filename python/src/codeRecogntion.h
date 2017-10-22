/**
	该类主要是做编码识别来使用的，同时提供一下编码相关的接口
*/
#ifndef H_CODERECOGNTION_H
#define H_CODERECOGNTION_H

enum eCodeType 
{
	CODE_TYPE_UNKNOWN,//type unknown
	CODE_TYPE_ASCII,//ASCII
	CODE_TYPE_GB,//GB2312,GBK,GB10380
	CODE_TYPE_UTF8,//UTF-8
	CODE_TYPE_BIG5//BIG5
};


class CCodeRecogtion
{
public:
	CCodeRecogtion();
	~CCodeRecogtion();
public:
	/**
		功能：识别一段文本的编码，只识别GBK和utf8 和BIG5其他的编码返回不知道
		参数: sLine 字符串 nLen 表示串的长度
		返回值: 表示文本的类型
	*/
	static eCodeType getTextType(const char* sLine, int nLen);
	/**
		功能: utf8编码的字符串提取1个uft8的汉字或字符，并返回占用的字节
		可以用此函数做文字解析使用
		参数：sLine表示文本 nUnicode[out] 1个汉字的unicode码值
		返回值: 表示1个汉字或者字符占用的字节数
	*/
	static int utf8ToUnicode(const char* sLine, int& nUnicode);
private:
	//范围是 双字节 ch 0xA1-0xF7 cl 0xA1-0xFE
	static bool isGB2312(unsigned char ch, unsigned char cl);
	//big编码 ch 0xA1-0xFE cl 0x40-0x7E（64-126）|| 0xA1－0xFE（161-254）
	static bool isBig5(unsigned char ch, unsigned char cl);
	//gbk编码 ch 0x81  0xfe cl  0x40-0x7e || 0x80-0xfe
	static bool isGBK(unsigned char ch, unsigned char cl);
	static bool isAscii(int c);
	//utf8编码识别方法 网上搜的
	static bool isUtf8(const char* line, int len);
};
#endif//H_CODERECOGNTION_H
