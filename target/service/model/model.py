#coding:utf-8

import os, sys
sys.path.append(os.path.dirname(os.path.dirname(__file__)))
from urllib2 import urlopen
import riak
from config import config, DEBUG
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

    buck = ':'      # make riak raise an error.
    W_VALUE = 1
    DW_VALUE = 0
    R_VALUE = 1

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
        if cls._instance is None:
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

    buck = 'm'      # web-moves

    def __init__(self):
        super(MovesStorer, self).__init__()

    def exists(self, url):
        if DEBUG: return False
        # TODO: from bitcask
        return self.bucket.get(key=self._genKey(url), r=self.R_VALUE).exists()
