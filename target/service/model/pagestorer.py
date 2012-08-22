#coding:utf-8

from urllib2 import urlopen
from config import config, DEBUG, logger
from utils.UrlParser import UrlParser
#from riakstorer import RiakStorer
from model.leveldbstorer import LevelDBStorer

class PageStorer(LevelDBStorer):

    dbId = 'loc'
    urlparser = UrlParser()
    _instance = None

    @classmethod
    def instance(cls):
        if not isinstance(cls._instance, cls):
            cls._instance = cls()
        return cls._instance

    def __init__(self):
        super(PageStorer, self).__init__()

    def store(self, meta, content):
        if DEBUG: return
        data = self._getData(meta, content)
        if data is None: return
        self.put(self._encodeUrl(meta['url']), data)

    def _getData(self, meta, content):
        if 'url' not in meta:
            return None
        urlinfo = self.urlparser.parse(meta['url'])
        if urlinfo is None:
            return None
        words = self._split(content)
        if words is None:
            return None
        return {
            'loc' : urlinfo,
            'ref' : meta['ref'] if 'ref' in meta else '',
            'words' : words,
        }

    def _split(self, content):
        try:
            return urlopen(config.getTokenizer('split'), content, timeout=3).read()
        except Exception, e:
            logger.error(('kid, request to tokenizer/split with len(content)=%d failed: ' % len(content)) + str(e.args))

    def exists(self, url):
        return self.keyExists(self._encodeUrl(url))

    def fetchSplitedContent(self, url=None, encodedUrl=None):
        key = encodedUrl or self._encodeUrl(url)
        res = self.getAuto(key)
        return res['words'] if res else None
