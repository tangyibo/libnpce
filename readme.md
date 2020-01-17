# libnpce


介绍
-------

 新闻文章正文抽取News Passage Content Extractor (NPCE)，是为抽取HTML中的文章正文而设计的。
 
 可用libnpce组件抽取新闻页面的如下字段信息：
- 标题
- 发布时间
- 来源及其URL链接地址
- 正文文本内容
- 正文图片信息
- 其他

 该组件提供给予so动态链接库的调用接口和给予RESTful服务调用的接口形式。并支持python调用接口。

编译
------

 1、程序结构
 
```
  \
   -include    头文件所在目录
   -src        源程序所在目录
   -test       测试和使用示例
   -consor     计算所内郭岩老师的consor组件(不含源码)，用于做比较性测试
   -cgifile    用于进行CGI部署的程序
   -parseapp   对APP正文抽取的组件封装
   -python     为python语言调用封装的通用接口
   -server     提供基于HTTP服务访问的RESTful接口
   -Makefile  
   -readme.txt 说明文件
   -bin        二进制文件生成目录
   -run.sh     运行测试程序脚本
```

 2、编译程序
 
```
  make clean   清除历史编译产生的obj和bin文件
  make all     编译生成so库文件
  make test    编译生成测试示例程序(需有libcurl支持)
```

  其他编译选项详见Makefile文件
  
说明
-----
博客宣传地址：https://blog.csdn.net/inrgihc/article/details/103739874

项目文档地址：https://tangyibo.github.io/libariry/2020/01/17/a-news-passage-content-extractor-library/

项目gitee地址：https://gitee.com/inrgihc/libnpce

