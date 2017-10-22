/*
 * HtmlCleaner.h
 *
 *  Created: 2016年8月23日
 *   Author: tang
 */

#ifndef SRC_HTMLCLEANER_H_
#define SRC_HTMLCLEANER_H_
#include <string>
using std::string;

class CHtmlCleaner
{
public:
	static bool HtmlTidy(string &src,string &errmsg);
	static string HtmlEraseTags(const string &src);
};

#endif /* SRC_HTMLCLEANER_H_ */
