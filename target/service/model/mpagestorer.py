#coding:utf-8

import pymongo
from config import config
from driversync.tokenizer import Tokenizer
from utils.misc import md5
import time

class PageStorer(object):

    _instance = None

    @classmethod
    def instance(cls):
        if not isinstance(cls._instance, cls):
            cls._instance = cls()
        return cls._instance

    def __init__(self):
        self.prepare()

    def prepare(self):
        self.tokenizer = Tokenizer.instance()
        self.connections = {}
        self.dbs = {}
        self.collections = {}
        for dbname in ('loc', 'text'):
            # dbname is same as the collection name
            self.connections[dbname] = pymongo.Connection(**config.getMongoDB(type=dbname))
            self.dbs[dbname] = self.connections[dbname][dbname]
            self.collections[dbname] = self.dbs[dbname][dbname]

    def store(self, meta, content):
        return self._store(url=meta['url'], content=content)

    def _store(self, url, content):
        md5_res = md5(url)
        time_stamp = time.time()
        links, content = self._parseContent(content=content)
        text = {'_id':md5_res, 'url':url, 'text':content, 'links':links, 'ts':time_stamp}
        self.collections['text'].insert(text)
        loc = self._getData(url, content, md5_res=md5_res, time_stamp=time_stamp)
        self.collections['loc'].insert(loc)

    def _parseContent(self, content):
        info = content.split("\t", 1)
        return [config.jsoner.decode(info[0]), info[1]] if len(info) == 2 else [[], content]

    def exists(self, url):
        return not not self.collections['loc'].find_one({'_id':md5(url)})

    def _getData(self, url, content, md5_res=None, time_stamp=None):
        wordsWeight = self._marve(content)
        return {
            '_id' : md5_res or md5(url),
            'ts' : time.time() or time_stamp,
            'url' : url,
            'words' : [ww[0] for ww in wordsWeight],
            'weight' : [ww[1] for ww in wordsWeight],
        }

    def _marve(self, content):
        return self.tokenizer.marve(content=content)
