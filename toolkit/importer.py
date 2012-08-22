#!/usr/bin/env python
#coding:utf-8

import os, sys
src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'target', 'service')
if src_path not in sys.path:
    sys.path.append(src_path)
from model.leveldbstorer import LevelDBStorer
from model.ftindex import FTIndex
from model.tokenizer import Tokenizer

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

class AdsImpor(object):

    def __init__(self):
        self.ftindex = FTIndex()
        self.adsDB = LevelDBStorer(dbId='ads')

    def put(self, id, data):
        self.ftindex.store(docId=id, words=data['words'])
        self.adsDB.put(id, data)

def adsImport(files):
    db = AdsImpor()
    marver = Tokenizer()
    for f in files:
        ad = {}
        fp = open(f, 'r')
        ad['id'] = int(fp.readline().strip())
        ad['link'] = fp.readline().strip()
        ad['text'] = fp.readline().strip()
        ad['words'] = marver.marve(''.join(map(lambda ln: ln.strip(), fp.readlines())))
        db.put(id=ad['id'], data=ad)

if __name__ == '__main__':
    adsImport(sys.argv[1:])
