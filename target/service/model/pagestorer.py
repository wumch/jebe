#coding:utf-8

from utils.urlparser import UrlParser
from drivers.locdb import LocDB
from drivers.tokenizer import Tokenizer

class PageStorer(object):

    urlparser = UrlParser()
    _instance = None

    @classmethod
    def instance(cls):
        if not isinstance(cls._instance, cls):
            cls._instance = cls()
        return cls._instance

    def __init__(self):
        self.locdb = LocDB()

    def store(self, meta, content):
        data = self._getData(meta, content)
        if data is None: return
        self.locdb.store(meta['url'], data)
        return data

    def _getData(self, meta, content):
        # TODO: url unique...
        if 'url' not in meta:
            return None
        urlinfo = self.urlparser.parse(meta['url'])
        if urlinfo is None:
            return None
        return self.marve(content)

    def marve(self, content):
        return Tokenizer.instance().marve(content=content)
