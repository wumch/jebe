#!/usr/bin/env python
#coding:utf-8

import os, sys
src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'target', 'service')
if src_path not in sys.path:
    sys.path.append(src_path)
from model.riakstorer import RiakStorer
from config import DEBUG, config
from urllib2 import urlopen

class DataSource(object):

    def __int__(self):
        pass

    def __iter__(self):
        pass

class FileSource(DataSource):

    def __init__(self, fname):
        super(FileSource, self).__int__()
        self.fp = open(fname, 'r')

    def __iter__(self):
        return self

    def next(self):
        line = self.fp.readline()
        if line:
            return line.strip()
        else:
            self.fp.close()
            raise StopIteration

class AdsImpor(RiakStorer):

    buckId = 'ads'
    if DEBUG:
        buckId = 'asus'

    def __init__(self):
        super(AdsImpor, self).__init__()

    def put(self, key, data, isBinary=False):
        print key, data['text']
        if isBinary:
            self.bucket.new_binary(key=key, data=data).store()
        else:
            self.bucket.new(key=key, data=data).store()

def processWords(content):
    return urlopen(config.getTokenizer('split'), data=content, timeout=10).read()

def adsImport(files):
    riak = AdsImpor()
    for f in files:
        ad = {}
        fp = open(f, 'r')
        ad['id'] = str(int(fp.readline().strip()))
        ad['link'] = fp.readline().strip()
        ad['text'] = fp.readline().strip()
        ad['words'] = processWords(''.join(map(lambda ln: ln.strip(), fp.readlines())))
        riak.put(key=ad['id'], data=ad)

if __name__ == '__main__':
    adsImport(sys.argv[1:])
