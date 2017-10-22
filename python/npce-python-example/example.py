import urllib
import urllib2
import sys
import libgolaxynpce

url=sys.argv[1]
req=urllib2.Request(url)
file = urllib2.urlopen(req)
html=file.read()
ret=libgolaxynpce.extract(url,html)
print ret
