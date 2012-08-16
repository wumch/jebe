#coding:utf-8

import os
from urllib2 import urlopen
import riak
from config import config, logger, DEBUG
from utils.UrlParser import UrlParser
from utils.misc import *

class FileStorer(object):

    prefix = '/ssd-data/crawler/'
    if DEBUG:
        prefix = '/tmp/crawler/'
    suffix = '.txt'

    def __init__(self):
        self.fp = open(self.genFileName(), 'a')

    def store(self, content):
        self.fp.write(content)
        self.fp.write(os.linesep)

    def genFileName(self):
        if not os.path.exists(self.prefix):
            os.mkdir(self.prefix)
        basename = 1
        filename = self.prefix + str(basename) + self.suffix
        while os.path.exists(filename):
            basename += 1
            filename = self.prefix + str(basename) + self.suffix
        return filename

class RiakStorer(object):

    buck = None      # make riak raise an error.
    backend = None
    R_VALUE = 1
    R_VALUE_UP = 2

    def __init__(self):
        self.riakClient = riak.RiakClient(**config.getRiak())
        self.bucket = self.riakClient.bucket(self.buck)
        self.bucket.set_property('backend', self.backend)

    def _genKey(self, url):
        return strenc(url)

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

class MoveStorer(RiakStorer):

    buck    = 'mov'      # web-moves
    backend = 'hdd3'
    if DEBUG:
        backend = 'leveldb'

    _instance = None

    @classmethod
    def instance(cls):
        if not isinstance(cls._instance, cls):
            cls._instance = cls()
        return cls._instance

    def __init__(self):
        super(MoveStorer, self).__init__()

    def exists(self, info):
        if DEBUG: return False
        return self._store(info)

    def _store(self, info):
        if 'url' not in info:
            return False
        key = self._genKey(info['url'])
        obj = self.bucket.get(key=key, r=self.R_VALUE)
        if not obj.exists():
            obj = self.bucket.get(key=key, r=self.R_VALUE_UP)
        exists = obj.exists()
        if 'ref' not in info:   # donot store in this case.
            return exists
        if exists:
            try:
                print int(obj.get_data())
                obj.set_data(str(int(obj.get_data()) + 1)).store()
            except Exception:
                logger.critical("failed no sotre web-moves")
        else:
            self.bucket.new_binary(key=key, data=str(1)).store()
