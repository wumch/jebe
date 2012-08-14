#coding:utf-8

import os, sys
sys.path.append(os.path.dirname(os.path.dirname(__file__)))
from urllib2 import urlopen
from config import config, DEBUG
from utils.UrlParser import UrlParser
from riakstorer import RiakStorer

class PageStorer(RiakStorer):

    buck = 'loc'        # page {url:..., words:...}
    backend = 'hdd2'
    if DEBUG:
        backend = 'leveldb'

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
        self.bucket.new(self._genKey(meta['url']), data).store()

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
        except Exception: pass
