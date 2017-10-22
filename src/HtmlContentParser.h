#ifndef SRC_CNEWSEXTRACT_H_
#define SRC_CNEWSEXTRACT_H_
#include "npce.h"
#include "htmlcxx/ParserDom.h"
#include "HtmlContentFilter.h"

class  CContentParser
{
public:
	CContentParser(const int type);
	virtual ~CContentParser();

	bool Open(string &src,const string &url);
	void Close();

	int LoadFilterFile(const char *filename);
	const string FilterFile() { return mFilterContent.GetFilterFile();}

	void MarkedImages(bool marked) { mMarkedImg=marked; }
	const bool MarkedImages() const { return mMarkedImg; }

	void LinkAnchor(bool flag) { mLinkAnchor=flag;}
	const bool LinkAnchor() const { return mLinkAnchor; }
	
	void TidyHtml(bool tidy) { mTidyHtml=tidy; }
	const bool TidyHeml () const { return mTidyHtml; }

	const string LastError() { return mErrorMsg;}

private:

	void Clear();

	bool PreProcess(string &src,const string url);
	void AfterProcess(string &text);

	int ParseTime(const string text,int min=0);
	void ParseSource(string &text);

public:
	NpceResult mResult;

private:
	int mType;
	bool mMarkedImg;
	bool mLinkAnchor;
	bool mTidyHtml;

	string mErrorMsg;

	htmlcxx::HTML::ParserDom mDomParser;

	bool mSetContentFilter;
	CContentFilter mFilterContent;
};

bool str_nocase_equal(const string left,const string right);
bool is_remove_tag(const string str);
bool is_remove_node(const string str);
bool is_content_tag(const string str);
bool is_nextline_tag(const string str);

#endif /* SRC_CNEWSEXTRACT_H_ */
