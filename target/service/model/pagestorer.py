#coding:utf-8

from utils.urlparser import UrlParser
from drivers.locdb import LocDB
from drivers.tokenizer import Tokenizer
from drivers.zmqclient import bind, recurveCallbackBounded

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

    def store(self, meta, content, callback=None):
        self._getData(meta, content, callback=bind(self._onMarve(callback), meta=meta))

    @recurveCallbackBounded
    def _onMarve(self, meta, words):
        if not words: return []
        self.locdb.store(words['url'], words=words, callback=None)
        return words

    def _getData(self, meta, content, callback=None):
        # TODO: url unique...
        if 'url' not in meta:
            return None
        urlinfo = self.urlparser.parse(meta['url'])
        if urlinfo is None:
            return None
        self.marve(content, callback=callback)

    def marve(self, content, callback=None):
        return Tokenizer.instance().marve(content=content, callback=callback)
