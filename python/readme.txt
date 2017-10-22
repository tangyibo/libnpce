将C/C++的库封装为Python的接口，需要的条件是安装Python-devel,安装的步骤如下：
wget http://www.python.org/ftp/python/2.7.6/Python-2.7.6.tar.xz
tar xJf Python-2.7.6.tar.xz
cd Python-2.7.6
./configure --enable-shared --prefix=/usr/local/python2.7
make
make install 
