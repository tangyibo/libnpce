/*
 * examples.cpp
 *
 *  Created: 2016年8月17日
 *   Author: tang
 */
#include "npce.h"
#include "UrlDownload.h"
#include <iostream>
#include <time.h>
#include <stdio.h>
using namespace std;

string UTC2String(const time_t &tmr)
{
	struct tm *t=localtime(&tmr);
	char szFormat[128];
	sprintf(szFormat,"%d-%d-%d %d:%d:%d",
			t->tm_year+1900 ,
			t->tm_mon+1,
			t->tm_mday,
			t->tm_hour,
			t->tm_min,
			t->tm_sec);

	return string(szFormat);
}

int main(int argc,char *argv[])
{
	if(argc<2)
	{
		cout<<"Usage:"<<argv[0]<<" [url]"<<endl;
		return 1;
	}

	string url=argv[1];
	CUrlDownload loader;
	string html;
	vector< pair<string,string> >::iterator it;

	if(loader.Download2Mem(url,html,true,50))
	{
		int index=0;
		vector< pair<string,string> > links;
		int ret=(int)npce_html_links_wich_anchor(html.c_str(),url.c_str(),links);
		if(0==ret && links.size()>0)
		{
			vector< pair<string,string> >::iterator it;
			for(it=links.begin();it!=links.end();++it)
			{
				string url=it->first;
				if(url[url.length()-1]!='/')
					cout<<"【"<<++index<<"】"<<"["<<it->second<<"]\n"<<it->first<<endl;
			}
		}

	        vector<string> subpagelinks;
        	ret=npce_search_iframe_links(html.c_str(),url.c_str(),subpagelinks);
        	if(0==ret && subpagelinks.size()>0)
        	{
                	for(vector<string>::iterator it=subpagelinks.begin();it!=subpagelinks.end();++it)
                	{
                        	string suburl=*it;
                        	if(suburl.empty())
                                	continue;
				
				cout<<"Load page :"<<suburl<<endl;
				string page;
                        	if(!loader.Download2Mem(suburl,page,true))
                                	continue;

                        	vector< pair<string,string> > sub_anchor_links;
                        	vector< pair<string,string> >::iterator iter;
                        	ret=npce_html_links_wich_anchor(page.c_str(),suburl.c_str(),sub_anchor_links);
                       	 	if(0==ret && sub_anchor_links.size()>0)
                        	{
                                	for(iter=sub_anchor_links.begin();iter!=sub_anchor_links.end();++iter)
                                	{
                    		            	string url=iter->first;
                                		if(url[url.length()-1]!='/')
                                        		cout<<"【"<<++index<<"】"<<"["<<iter->second<<"]\n"<<iter->first<<endl;

					}
				}
			}
		}
	}
	else
		cout<<"Download error!"<<endl;

	return 0;
}



