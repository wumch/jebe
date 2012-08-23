#!/usr/bin/env python
#coding:utf-8

import os, sys
src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'target', 'service')
if src_path not in sys.path:
    sys.path.append(src_path)
import zmq, struct
from urllib2 import urlopen
from json import JSONDecoder
from config import config
from utils.misc import strenc, export

def urlComplete(pageUrl):
    url = pageUrl if pageUrl.startswith('http') else ('http://' + pageUrl)
    return url if url.find('/', 10) != -1 else (url + '/')

def genRequestUrl(pageUrl):
    return ('http://%(host)s:%(port)s/riak/loc/' % config.getRiak()) + strenc(pageUrl)

def look(url):
    try:
        return JSONDecoder().decode(urlopen(genRequestUrl(url), timeout=3).read())
    except Exception:
        return None

def look(url):
    sock = zmq.Context(1).socket(zmq.REQ)
    uri = "tcp://%s:%d" % (config.getRouter()['host'], config.router_port)
    sock.connect(uri)
    sock.send_multipart([struct.pack('B', 4), config.packer.encode(url)])
    resp = sock.recv_multipart()
    print 'resp:[%s]' % resp
    return config.packer.decode(resp[0])


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print """usage: %s <url_1> [url_2] ...""" % sys.argv[0]
        sys.exit(1)
    for pageUrl in sys.argv[1:]:
        url = urlComplete(pageUrl)
        print 'url: ', url
        print 'words:'
        export(look(url))
