/*
 * HtmlLinksParser.h
 *
 *  Created: 2016年8月23日
 *   Author: tang
 */

#ifndef SRC_HTMLLINKSPARSER_H_
#define SRC_HTMLLINKSPARSER_H_
#include "npce.h"
using std::pair;

class CHtmlLinksParser
{
public:
	static NpceCode ParseHtmlLinks(const char *html,const char *url,bool anchor,vector< pair<string,string> > &results);

	static NpceCode ParseHtmlIframeLinks(const char *html,const char *url,vector<string> &links);

	static NpceCode ParseHtmlLinksFilter(const char *html,const char *url,const char *regex,bool anchor,vector< pair<string,string> > &results);
};

#endif /* SRC_HTMLLINKSPARSER_H_ */
