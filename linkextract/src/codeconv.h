/**
 * 功能：编码识别与转换
 * 作者：tangyibo
 * 时间：2016-010-20
 * 说明：本组件融合了编码的识别与编码转换库，
 */
#ifndef _CODE_CONV_H_
#define _CODE_CONV_H_
#include <string>

#define DLL_CODECONV_PUBLIC   __attribute__ ((visibility("default")))
#define CODECONV_API extern "C" DLL_CODECONV_PUBLIC

CODECONV_API  int codeconv_code_detect(const std::string text,std::string &charset);

CODECONV_API  int  codeconv_text_convert(std::string &text,const std::string dest_charset="utf-8");

CODECONV_API std::string codeconv_last_errmsg();

#endif /* _CODE_CONV_H_ */
