#ifndef _PARSE_APP_NEWS_H
#define _PARSE_APP_NEWS_H
#include <string>
#include "parseapp.h"
using std::string;

/*
 *Sohu 搜狐APP解析
 */
int parse_appnews_sohu(const string &src,const string newsid,const bool img,const bool flg,NpceResult &result);

/*
 *Netease 网易APP解析
 */
int parse_appnews_netease(const string &src,const string newsid,const bool img,const bool flg,NpceResult &result);


extern string g_strLastErrorMsg;

#endif
