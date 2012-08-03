#!/usr/bin/env python

DEBUG = False

import os
import zmq
from utils.natip import natip
import zlib, struct
import riak
from random import randint
from hashlib import md5
from urllib2 import *
from json import JSONDecoder, JSONEncoder
from utils.UrlParser import UrlParser

context = zmq.Context()
sock = context.socket(zmq.REP)
sock.connect("tcp://%s:10011" % natip)

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

class PageStorer(object):

    urlparser = UrlParser()
    tokenizer = ('http://192.168.88.2:10086/split',
                 'http://192.168.88.4:10086/split',)
    riaks = ({'host':'192.168.88.1', 'port':8098},
             {'host':'192.168.88.2', 'port':8098},
             {'host':'192.168.88.3', 'port':8098},
             {'host':'192.168.88.4', 'port':8098},)
    if DEBUG:
        riaks = ({'host':natip, 'port':8098}, )
        tokenizer = ('http://127.0.0.1:10086/split', )

    buck = 'loc'
    W_VALUE = 1
    DW_VALUE = 0
    R_VALUE = 1

    _instance = None

    @classmethod
    def instance(cls):
        if cls._instance is None:
            cls._instance = cls()
        return cls._instance

    def __init__(self):
        self.riakClient = riak.RiakClient(**self.riaks[randint(0, len(self.riaks) - 1)])
        self.bucket = self.riakClient.bucket(self.buck)

    def store(self, meta, content):
        if DEBUG: return
        data = self.getData(meta, content)
        if data is None: return
        self.bucket.new(self.genKey(meta['url']), data).store(w=self.W_VALUE, dw=self.DW_VALUE)

    def getData(self, meta, content):
        if 'url' not in meta:
            return None
        urlinfo = self.urlparser.parse(meta['url'])
        if urlinfo is None:
            return None
        words = self.split(content)
        if words is None:
            return None
        return {
            'loc' : urlinfo,
            'ref' : meta['ref'] if 'ref' in meta else '',
            'words' : words,
        }

    def split(self, content):
        for server in self.tokenizer:
            try:
                return urlopen(server, content, timeout=3).read()
            except Exception: pass

    def genKey(self, url):
        m = md5()
        m.update(url)
        return m.hexdigest()

    def exists(self, url):
        if DEBUG: return False
        return self.bucket.get(key=self.genKey(url), r=self.R_VALUE).exists()

class Handler(object):

    ERR_CODE_OK     = 'ok'
    ERR_CODE_ERR    = 'err'
    jsonDecoder = JSONDecoder(encoding='utf-8')
    jsonEncoder = JSONEncoder()

    def __init__(self):
        self.out = {'code':self.ERR_CODE_ERR}

    def handle(self, data): pass

    def replyOk(self):
        self.out['code'] = self.ERR_CODE_OK
        self.response()

    @classmethod    # to make global callable.
    def replyErr(cls):
        global sock
        sock.send(cls.jsonEncoder.encode({'code':cls.ERR_CODE_ERR}))

    def response(self, data = None):
        global sock
        d = data or self.out
        sock.send(d if isinstance(d, basestring) else self.jsonEncoder.encode(d))

    def __del__(self):
        self.response(self.out)

class HPageExists(Handler):

    pageStorer = PageStorer.instance()

    def __init__(self):
        super(HPageExists, self).__init__()

    def handle(self, data):
        """
        case:
            non-exists: ask client for details of the page.
            exists: response "all right" with some ads.
        """
        try:
            info = self.jsonDecoder.decode(data[0])
            self.replyOk() if self.pageStorer.exists(info['url']) else self.replyErr()
        except Exception:
            self.replyOk()      # to make error-occured client no longer upload.
            print "pageExists failed: "

class HCrawl(Handler):

    ucpacker = struct.Struct('B')     # ny
    pageStorer = PageStorer.instance()

    def __init__(self):
        super(HCrawl, self).__init__()
        self.fileStorer = FileStorer()

    def handle(self, data):
        try:
            meta = self.jsonDecoder.decode(data[0])
            content = zlib.decompress(data[1]) if meta['compressed'] else data[1]
            self.store(meta, content)
            self.replyOk()
        except Exception:
            self.replyErr()
            print "crawl failed: "

    def store(self, meta, content):
        self.fileStorer.store(content)
        self.pageStorer.store(meta, content)

class HShowAds(Handler):

    def __init__(self):
        super(HShowAds, self).__init__()

    def handle(self, data):
        global sock
        try:
            info = self.jsonDecoder.decode(data)
            self.replyOk() if info else self.replyErr()
        except Exception:
            self.replyErr()
            print "showAds failed"

class Dealer(object):

    ucpacker = struct.Struct('B')     # ny
    handlerList = [None, HPageExists(), HCrawl(), HShowAds()]

    def __init__(self): pass

    def _getHandler(self, header):
        index = self.ucpacker.unpack(header[0])[0]
        if not 0 < index < len(self.handlerList):
            Handler.replyErr()
        return self.handlerList[index]

    def handle(self, header, payload):
        return self._getHandler(header).handle(payload)

if __name__ == '__main__':
    dealer = Dealer()
    while True:
        data = sock.recv_multipart()
        print "received some"
        if len(data) > 1:
            dealer.handle(data[0], data[1:])
        else:
            Handler.replyErr()
