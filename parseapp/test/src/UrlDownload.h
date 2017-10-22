/*
 * CUrlDownload.h
 *
 *  Created on: 2015-5-11
 *      Author: tang
 */

#ifndef CURLDOWNLOAD_H_
#define CURLDOWNLOAD_H_
#include <string>
using std::string;

#define TIME_INFINIT  	-1

/*
 * 基于libcurl的URL文件下载
 */
class CUrlDownload
{
public:
	CUrlDownload();
	virtual ~CUrlDownload();

	bool Download2Mem(const string &url ,string &content,const bool bToUtf8=true, const size_t timeout_s=TIME_INFINIT);
	bool Download2File(const string &strURL, const string &filename, const size_t timeout_s = TIME_INFINIT);

	const std::string LastErrorMsg() { return mErrorMsg;}
	void SetErrorMsg(const char* format, ...);

protected:
	static int WriteMemoryCallback(char *data, size_t size, size_t nmemb, void *stream);
	static size_t WriteFileCallback(void *ptr, size_t size, size_t nmemb, void* userdata);
	static int CodeConv(string& strSrc);

private:
	string mErrorMsg;
};

#endif /* CURLDOWNLOAD_H_ */
