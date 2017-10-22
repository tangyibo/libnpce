/*
 * HtmlCleaner.cpp
 *
 *  Created: 2016年8月23日
 *   Author: tang
 */

#include "HtmlCleaner.h"
#include "tidy/tidy.h"
#include "tidy/tidybuffio.h"
#include "HtmlContentParser.h"
#include "utils/UtilFun.h"

using namespace std;
using namespace htmlcxx;

bool CHtmlCleaner::HtmlTidy(std::string &src,string &errmsg)
{
	TidyBuffer output = {0};
	TidyBuffer errbuf = {0};
	int rc = -1;
	Bool ok;
	errmsg.clear();

	TidyDoc tdoc = tidyCreate();                     // Initialize "document"
	ok = tidyOptSetBool( tdoc, TidyXhtmlOut, yes );  // Convert to XHTML
	if ( ok )
		rc = tidySetErrorBuffer( tdoc, &errbuf );      // Capture diagnostics
	if ( rc >= 0 )
		rc = tidyParseString( tdoc, src.c_str() );           // Parse the input
	if ( rc >= 0 )
		rc = tidyCleanAndRepair( tdoc );               // Tidy it up!
	if ( rc >= 0 )
		rc = tidyRunDiagnostics( tdoc );               // Kvetch
	if ( rc > 1 )                                    // If error, force output.
		rc = ( tidyOptSetBool(tdoc, TidyForceOutput, yes) ? rc : -1 );
	if ( rc >= 0 )
		rc = tidySaveBuffer( tdoc, &output );          // Pretty Print

	if ( rc < 0 )
	{
		errmsg=Utils::StringFormat("an error occurred when tidy html code ,error code :%d",rc);
	}
	else
	{
		src.assign((char *)output.bp,output.size);
	}

	tidyBufFree( &output );
	tidyBufFree( &errbuf );
	tidyRelease( tdoc );

	if(rc>=0)
		return true;

	return false;
}

string CHtmlCleaner::HtmlEraseTags(const string &src)
{
	string result="";
	if(src.size()==0 )
		return result;

	HTML::ParserDom parser;
	tree<HTML::Node> dom = parser.parseTree(src);
	if(dom.size()==0)
		return result;

	tree<HTML::Node>::iterator it = dom.begin();
	tree<HTML::Node>::iterator end = dom.end();

	for(it = dom.begin(); it != end;)
	{
		if(it->isComment())
		{
			it=dom.erase(it);
			continue;
		}
		else if(it->isTag())
		{
			if( is_remove_node(it->tagName()))
			{
				//dom.erase_children(it);
				it=dom.erase(it);
				continue;
			}

			if( is_nextline_tag(it->tagName()))
			{
				if(!result.empty())
					result+="\n";
			}

			++it;
		}
		else
		{
			string text=it->text();
			if(!text.empty())
			{
				result+=" ";
				result+=text;
			}

			++it;
		}
	}


	Utils::CleanupStr(result);
	Utils::ReplaceAll(result,"&nbsp;"," ");
	return result;
}
