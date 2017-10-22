/*
 * HttpService.h
 *
 *  Created: 2016年10月21日
 *   Author: tang
 */

#ifndef SRC_HTTPSERVICE_H_
#define SRC_HTTPSERVICE_H_
#include "mongoose.h"
#include <string>

enum ErrorCodes {
  kEmptyStringError = 1,  /*< The input sentence is empty */
  kEncodingError,         /*< Sentence encoding not in UTF-8 */
  kDecodeUrlError,         /*< Failed to decode URL */
  kAllocateMemoryError,  /*< Operater System allocate memory failed */
  kReadXmlError,          /*< Failed to read XML in internal process */
  kWriteXmlError,         /*< Failed to write XML in internal process */
};

class CHttpService
{
public:
	static int Service(struct mg_connection* conn);
	static void ErrorResponse(struct mg_connection* conn,const enum ErrorCodes& code);
	static void Handler(std::string url,int img,int flg,std::string &ret);
};

#endif /* SRC_HTTPSERVICE_H_ */
