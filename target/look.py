#coding:utf-8

import os, sys
from hashlib import md5
from urllib2 import urlopen
from json import JSONDecoder
from utils.natip import natip

def urlComplete(url):
    return url if url.startswith('http') else ('http://' + url)

def genKey(url):
    m = md5()
    m.update(url)
    return m.hexdigest()

def genRequestUrl(pageUrl):
    return 'http://%s:8098/riak/loc/%s' % (natip, genKey(pageUrl))

def look(url):
    return JSONDecoder().decode(urlopen(genRequestUrl(url), timeout=3).read())['words']


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print """usage: %s <url_1> [url_2] ...""" % sys.argv[0]
        sys.exit(1)
    for pageUrl in sys.argv[1:]:
        url = urlComplete(pageUrl)
        print 'url: ', url
        print 'words:'
        print look(url), os.linesep