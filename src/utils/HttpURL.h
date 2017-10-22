/*
 * HttpURL.h
 *
 *  Created: 2015年11月24日
 *   Author: tang
 */

#ifndef SRC_HTTPURL_H_
#define SRC_HTTPURL_H_
#include <string>
#include <vector>

class CHttpURL
{
public:
	CHttpURL(const std::string baseUrl);
	virtual ~CHttpURL();

	const std::string toAbsolute(std::string refUrl);
	static const std::string FileName(std::string &url);
	static void Trim(std::string &str);

private:
	static void split(std::string str,std::string pattern,std::vector<std::string> &result,bool urlcode=false);
	bool IsLastCharSlash();

private:
	std::string m_baseUrl;
	std::vector<std::string> m_list;
};

#endif /* SRC_HTTPURL_H_ */
