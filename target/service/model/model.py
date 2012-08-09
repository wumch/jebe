#coding:utf-8

import os, sys
sys.path.append(os.path.dirname(os.path.dirname(__file__)))
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
    R_VALUE = 1
    R_VALUE_UP = 2

    def __init__(self):
        self.riakClient = riak.RiakClient(**config.getRiak())
        self.bucket = self.riakClient.bucket(self.buck)

    def _genKey(self, url):
        return md516(url)

class PageStorer(RiakStorer):

    urlparser = UrlParser()

    buck = 'loc'

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

class MovesStorer(RiakStorer):

    buck = 'mov'      # web-moves

    _instance = None

    @classmethod
    def instance(cls):
        if not isinstance(cls._instance, cls):
            cls._instance = cls()
        return cls._instance

    def __init__(self):
        super(MovesStorer, self).__init__()

    def exists(self, url):
        if DEBUG: return False
        return self.bucket.get(key=self._genKey(url), r=self.R_VALUE).exists()

    def store(self, url, ref):
        key = self._genKey(url)
        obj = self.bucket.get(key=key, r=self.R_VALUE)
        if obj.exists():
            try:
                obj.set_data(int(obj.get_data()) + 1).store()
            except Exception:
                logger.critical("failed no sotre web-moves")
        else:
            self.bucket.new_binary(key=key, data=1).store()
