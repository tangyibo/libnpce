/*
 * HtmlContentFilter.h
 *
 *  Created: 2016年8月23日
 *   Author: tang
 */

#ifndef SRC_HTMLCONTENTFILTER_H_
#define SRC_HTMLCONTENTFILTER_H_
#include <vector>
#include <string>

class CContentFilter
{
public:
	CContentFilter();
	virtual ~CContentFilter();

public:
	bool LoadFiles(const char *filename);
	const std::string GetFilterFile()  { return mstrFilePath; }
	void FilterContent(std::string &str);
	void Clearup();

private:
	std::vector<std::string> mFilterLists;
	std::string mstrFilePath;
};

#endif /* SRC_HTMLCONTENTFILTER_H_ */
