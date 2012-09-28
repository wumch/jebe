#coding:utf-8

import pymongo
from config import config
from driversync.tokenizer import Tokenizer
from utils.misc import md5

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
        links, content = self._parseContent(content=content)
        links = config.jsoner.decode(links)
        text = {'url':url, 'text':content, 'links':links}
        self.collections['text'].insert(text)
        loc = self._getData(url, content)
        self.collections['loc'].insert(loc)

    def _parseContent(self, content):
        info = content.split("\t", 1)
        return info

    def exists(self, url):
        return not not self.collections['loc'].find_one({'url':url})

    def _getData(self, url, content):
        wordsWeight = self._marve(content)
        return {
            '_id' : md5(url),
            'url' : url,
            'words' : [ww[0] for ww in wordsWeight],
            'weight' : [ww[1] for ww in wordsWeight],
        }

    def _marve(self, content):
        return self.tokenizer.marve(content=content)
