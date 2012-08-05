#!/usr/bin/env python
#coding:utf-8

import os, sys
from hashlib import md5
from urllib2 import urlopen
from json import JSONDecoder
from config import config

def urlComplete(pageUrl):
    url = pageUrl if pageUrl.startswith('http') else ('http://' + pageUrl)
    return url if url.find('/', 10) != -1 else (url + '/')

def genKey(url):
    m = md5()
    m.update(url)
    return m.hexdigest()

def genRequestUrl(pageUrl):
    return ('http://%(host)s:%(port)s/riak/loc/' % config.getRiak()) +genKey(pageUrl)

def look(url):
    try:
        return JSONDecoder().decode(urlopen(genRequestUrl(url), timeout=3).read())['words']
    except Exception:
        return None


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print """usage: %s <url_1> [url_2] ...""" % sys.argv[0]
        sys.exit(1)
    for pageUrl in sys.argv[1:]:
        url = urlComplete(pageUrl)
        print 'url: ', url
        print 'words:'
        print look(url), os.linesep
