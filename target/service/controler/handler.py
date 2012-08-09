#coding:utf-8

import zlib
import struct
from urllib2 import urlopen
from config import config, logger, DEBUG
from utils.MarveWords import MarveWords
from model.model import *

class Handler(object):

    ERR_CODE_OK     = 'ok'
    ERR_CODE_ERR    = 'err'

    def __init__(self):
        self.out = {'code':self.ERR_CODE_ERR}

    def handle(self, data):
        raise NotImplementedError("<%s>.%s" % (self.__class__.__name__, sys._getframe().f_code.co_name))

    def replyOk(self):
        self.out['code'] = self.ERR_CODE_OK
        self.response()

    @classmethod    # to make global callable.
    def replyErr(cls):
        global sock
        sock.send(config.jsonEncoder.encode({'code':cls.ERR_CODE_ERR}))

    def response(self, data = None):
        global sock
        d = data or self.out
        sock.send(d if isinstance(d, basestring) else config.jsonEncoder.encode(d))

class HMarve(Handler):

    def __init__(self):
        super(Handler, self).__init__()

    def handle(self, data):
        try:
            meta = config.jsonDecoder.decode(data[0])
            content = zlib.decompress(data[1]) if meta['compressed'] else data[1]
            self.response(self.marve(content))
        except Exception:
            self.replyErr()
            logger.error("marve failed")

    def marve(self, content):
        try:
            json = urlopen(config.getTokenizer('marve'), data=content, timeout=3).read()
            res = MarveWords(config.jsonDecoder.decode(json)).top()
            if DEBUG: print res
            return res
        except Exception:
            return None

class HPageExists(Handler):

    moveStorer = MoveStorer.instance()

    def __init__(self):
        super(HPageExists, self).__init__()

    def handle(self, data):
        try:
            info = config.jsonDecoder.decode(data[0])
            self.replyOk() if self.moveStorer.exists(info) else self.replyErr()
        except Exception:
            self.replyOk()      # to make error-occured client no longer upload.
            logger.error("pageExists failed")

class HCrawl(Handler):

    ucpacker = struct.Struct('B')     # ny
    pageStorer = PageStorer.instance()

    def __init__(self):
        super(HCrawl, self).__init__()
        self.fileStorer = FileStorer()

    def handle(self, data):
        try:
            meta = config.jsonDecoder.decode(data[0])
            content = zlib.decompress(data[1]) if meta['compressed'] else data[1]
            self.store(meta, content)
            self.replyOk()
        except Exception:
            self.replyErr()
            logger.error("crawl failed")

    def store(self, meta, content):
        self.fileStorer.store(content)
        self.pageStorer.store(meta, content)

class HShowAds(Handler):

    def __init__(self):
        super(HShowAds, self).__init__()

    def handle(self, data):
        global sock
        try:
            info = config.jsonDecoder.decode(data)
            self.replyOk() if info else self.replyErr()
        except Exception:
            self.replyErr()
            logger.error("showAds failed")
