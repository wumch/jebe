#coding:utf-8

from urllib2 import urlopen
from config import config, DEBUG, logger
from utils.urlparser import UrlParser
from drivers.locdb import LocDB

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
        if 'url' not in meta:
            return None
        urlinfo = self.urlparser.parse(meta['url'])
        if urlinfo is None:
            return None
        words = self.marve(content)
        if words is None:
            return None
        return words

    def marve(self, content):
        try:
            return config.jsoner.decode(urlopen(config.getTokenizer('marve'), content, timeout=3).read())
        except Exception, e:
            logger.error(('kid, request to tokenizer/marve with len(content)=%d failed: ' % len(content)) + str(e.args))
