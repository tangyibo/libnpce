/*
 * HtmlAuthorParser.h
 *
 *  Created: 2016年8月22日
 *   Author: tang
 */

#ifndef SRC_HTML_SOURCE_PARSER_H_
#define SRC_HTML_SOURCE_PARSER_H_
#include <string>
using std::string;

#define REGEX_SOURCE_PARSER_NUM_COUNT 17

class CSourceParser
{
public:
	CSourceParser();
	virtual ~CSourceParser();

	bool SourceParser (string &data, string& source);

private:
	string m_strSourceRegexp[REGEX_SOURCE_PARSER_NUM_COUNT];
};

#endif /* SRC_HTMLAUTHORPARSER_H_ */
