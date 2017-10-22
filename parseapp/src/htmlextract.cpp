#include "htmlextract.h"
#include "htmlcxx/ParserDom.h"
#include "utils.h"

using namespace std;
using namespace htmlcxx;

bool html_content_context_extract(const string &src,bool img,bool flg,NpceResult &result)
{
	result.content="";
	if(src.size()==0 )
		return true;

	HTML::ParserDom parser;
	tree<HTML::Node> dom = parser.parseTree(src);
	if(dom.size()==0)
		return false;

	tree<HTML::Node>::iterator it = dom.begin();
	tree<HTML::Node>::iterator end = dom.end();

	for(it = dom.begin(); it != end;++it)
	{
		if(it->isComment())
		{
			dom.erase_children(it);
			continue;
		}
		else if(it->isTag())
		{
			if((0==strcasecmp(it->tagName().c_str(),"script") ||
				0==strcasecmp(it->tagName().c_str(),"code") ||
				0==strcasecmp(it->tagName().c_str(),"style")))
			{// delete tag which likes script code style
				dom.erase_children(it);
				continue;
			}	
			else if( img && 0==strcasecmp(it->tagName().c_str(),"img"))
			{
				it->parseAttributes();
				if(it->attribute("src").first)
				{
					string src_url=it->attribute("src").second;
					string alt=it->attribute("alt").first?it->attribute("alt").second:"";
					if(!src_url.empty())
					{
						pair<string,string> item;
						item.first=src_url;
						item.second=alt;
						result.imglist.push_back(item);

						if(flg)
						{
							string imgpos="{IMG:"+Utils::Int2String(result.imglist.size())+"}";
							if(result.content[result.content.length()-1]!='\n')
								result.content.append("\n"+imgpos);
							else
								result.content.append(imgpos);
						}
					}
				}
			}
			else if(0==strcasecmp(it->tagName().c_str(),"br") || 0==strcasecmp(it->tagName().c_str(),"p"))
			{
				if(result.content[result.content.length()-1]!='\n')
                                	result.content.append("\n");
			}
		}
		else
		{
			string text=it->text();
			Utils::TrimString(text);			
			result.content.append(text);
		}
	}

	return true;
}
