#ifndef _URL_CODER_H_
#define _URL_CODER_H_
#include <string>

namespace UrlCoder
{
    std::string UrlEncode(const std::string &str);
    std::string UrlDecode(const std::string &str);
};

#endif
