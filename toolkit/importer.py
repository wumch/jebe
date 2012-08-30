#!/usr/bin/env python
#coding:utf-8

import os, sys
src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'target', 'service')
if src_path not in sys.path:
    sys.path.append(src_path)
from model.leveldbstorer import LevelDBStorer
from model.ftindex import FTIndex
from drivers.tokenizer import Tokenizer

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
        self.ftindex.store(docId=id, words=data['words'], sync=True)
        self.adsDB.put(id, data, sync=True)

    def check(self, ad):
        assert ad['id'] > 0
        assert ad['link'].startswith('http://')
        assert len(ad['text']) > 0
        assert len(ad['words'][0]) == 2, 'maybe no words for ad[%d]' % ad['id']

def adsImport(files):
    marver = Tokenizer()
    for f in files:
        ad = {}
        fp = open(f, 'r')
        ad['id'] = int(fp.readline().strip())
        ad['link'] = fp.readline().strip()
        ad['text'] = fp.readline().strip()
        ad['words'] = marver.marve(''.join(map(lambda ln: ln.strip(), fp.readlines())))
        importSingle(**ad)

_ads_importer = None
def getImporter():
    global _ads_importer
    if _ads_importer is None:
        _ads_importer = AdsImpor()
    return _ads_importer

def importSingle(id=0, link='', text='', words=None):
    ad = {
        'id' : id,
        'link' : link,
        'text' : text,
        'words' : words
    }
    db = getImporter()
    db.check(ad)
    db.put(id=ad['id'], data=ad)

if __name__ == '__main__':
    adsImport(sys.argv[1:])
