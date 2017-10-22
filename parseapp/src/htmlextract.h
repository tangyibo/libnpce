#ifndef SRC_HTMLCLEANER_H_
#define SRC_HTMLCLEANER_H_
#include "parseapp.h"
#include <string>
using std::string;

bool html_content_context_extract(const string &src,bool img,bool flg,NpceResult &result);

#endif /* SRC_HTMLCLEANER_H_ */
