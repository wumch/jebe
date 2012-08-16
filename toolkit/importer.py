#coding:utf-8

import os, sys
src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'target', 'service')
if src_path not in sys.path:
    sys.path.append(src_path)
from riakstorer import RiakStorer
from config import config, DEBUG

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

    buck = 'ads'
    backend = 'hdd1'
    if DEBUG:
        backend = 'leveldb'

    def __init__(self):
        super(AdsImpor, self).__init__()

    def put(self, key, data, isBinary=False):
        if isBinary:
            self.bucket.new_binary(key=key, data=data).store()
        else:
            self.bucket.new(key=key, data=data).store()

def adsImport(files):
    riak = AdsImpor()
    for f in files:
        ad = {}
        fp = open(f, 'r')
        ad['text'] = fp.readline().strip()
        ad['link'] = fp.readline().strip()
        ad['words'] = ' '.join(map(lambda ln: ln.strip(), fp.readlines()))
        riak.put(ad['text'], ad)

if __name__ == '_main__':
    adsImport(sys.argv[1:])
