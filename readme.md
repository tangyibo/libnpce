## libnpce

介绍
======

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
=======

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
 3、使用示例
 
```
  #./run.sh  http://finance.qq.com/a/20160819/023677.htm
```
  
  执行上述命令即可查看到抽取的正文等信息。


存在问题
========
- 问题一：http://www.huaxia.com/xw/zhxw/2017/04/5283628.html【使用tidy库整理后即可抽取正确】
- 问题二：http://news.workercn.cn/607/201708/09/170809020943659.shtml[未知的抽取错误]
- 问题三：对于JS渲染的页面问题,需要借助第三方工具获取到渲染后的html文本页面,例如：

```
./run.sh
"http://10.20.18.8:8050/render.html?url=
http%3a%2f%2fwww.hsnewsn.com%2fInfo.aspx%3fModelId%3d1%26Id%3d34575&timeout=60&wait=10"
```
