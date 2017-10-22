/*
 * JaccardSimilarity.hpp
 *
 *  Created: 2016年10月2日
 *   Author: tang
 */

#ifndef SRC_JACCARD_SIMILARITY_HPP_
#define SRC_JACCARD_SIMILARITY_HPP_
#include <algorithm>
#include <iostream>
#include <vector>
#include <set>
#include "StringUtil.hpp"

using namespace std;

class JaccardSimilarity
{
public:
	
	JaccardSimilarity()
	{
	}

	double CalculateTextSimilarity(string &str1,string &str2)
	{
		vector<uint16_t> words_for_str1;
		vector<uint16_t> words_for_str2;
		vector<uint16_t>::iterator it;

		if(!utf8ToUnicode< vector<uint16_t> >(str1,words_for_str1) || 
			!utf8ToUnicode< vector<uint16_t> >(str2,words_for_str2 ) )
		{
			return 0.;
		}

		for(it=words_for_str1.begin();it!=words_for_str1.end();)
		{
			if(codeFilter(*it))
			{
				++it;
			}
			else
			{
				it=words_for_str1.erase(it);
			}
		}

		for(it=words_for_str2.begin();it!=words_for_str2.end();)
                {
                        if(codeFilter(*it))
                        {
				++it;
                        }
			else
			{
				it=words_for_str2.erase(it);
			}
                }

		if(words_for_str1.size()+words_for_str2.size()<1)
			return 1.;

		vector<uint16_t> words_intersection;
		vector<uint16_t> words_union;
		std::sort(words_for_str1.begin(),words_for_str1.end());
		std::sort(words_for_str2.begin(),words_for_str2.end());
		std::set_intersection(words_for_str1.begin(),words_for_str1.end(),
					words_for_str2.begin(),words_for_str2.end(),
					std::inserter(words_intersection,words_intersection.begin()));
		
		std::set_union(words_for_str1.begin(),words_for_str1.end(),
                                        words_for_str2.begin(),words_for_str2.end(),
					std::inserter(words_union,words_union.begin()));

		double inter=words_intersection.size();
		double wunion=words_union.size();
		return inter/wunion;
	}

	bool codeFilter(int code) 
	{
        	if ((code < 0x4e00 || code > 0x9fa5) && 
			!(code >= '0' && code <= '9') && 
			!(code >= 'a' && code <= 'z') && 
			!(code >= 'A' && code <= 'Z'))
           		 return false;
        
        	return true;
	}

};

#endif /* SRC_JACCARD_SIMILARITY_HPP_ */
