#coding:utf-8

import sys
import zlib
import struct
from urllib2 import urlopen
from config import config, logger, DEBUG
from utils.MarveWords import MarveWords
from model import *

class Handler(object):

    OK  = {'code' : 'ok'}
    ERR = {'code' : 'err'}

    def __init__(self, sock):
        self.sock = sock
        self.ader = Matcher()

    def handle(self, data):
        raise NotImplementedError("<%s>.%s" % (self.__class__.__name__, sys._getframe().f_code.co_name))

    def replyOk(self):
        self.response(self.OK)

    def replyError(self):
        self.replyErr(self.sock)

    @classmethod    # to make global callable.
    def replyErr(cls, sock):
        sock.send(config.jsoner.encode(cls.ERR))

    def response(self, data):
        self.sock.send(data if isinstance(data, basestring) else config.jsoner.encode(data))

    def mrads(self, **kwargs_for_get_ads):
        """
        match and reply ads.
        @param kwargs_for_get_ads
            some arguments for get ads.
        """
        self.response(self._getAds(**kwargs_for_get_ads))

    def _getAds(self, content=None, words=None, loc=None):
        ads = self.ader.match(content=content, words=words, loc=loc)
        res = [{'link':a['link'], 'text':a['text'], 'id':a['id']} for a in ads[:20]] if ads else []
        if res: logger.info('ad shown: %(text)s [%(link)s]' % res[0])
        return res

class HMarve(Handler):

    def __init__(self, sock):
        super(HMarve, self).__init__(sock)

    def handle(self, data):
        try:
            meta = config.jsoner.decode(data[0])
            content = zlib.decompress(data[1]) if meta['compressed'] else data[1]
            self.response(self.marve(content))
        except Exception, e:
            self.replyError()
            logger.error("marve failed: " + str(e.args))

    def marve(self, content):
        try:
            json = urlopen(config.getTokenizer('marve'), data=content, timeout=3).read()
            res = MarveWords(config.jsoner.decode(json)).top()
            if DEBUG: print res
            return res
        except Exception:
            return None

class HPageExists(Handler):

    moveStorer = MoveStorer.instance()

    def __init__(self, sock):
        super(HPageExists, self).__init__(sock)

    def handle(self, data):
        try:
            info = config.jsoner.decode(data[0])
            if self.moveStorer.exists(info) is True:
                self.mrads(loc=info['url'])      # should also carry some ads.
            else:
                self.replyError()
        except Exception, e:
            self.replyOk()      # to make error-occured client no longer upload.
            logger.error("pageExists failed: " + str(e.args))

class HCrawl(Handler):

    ucpacker = struct.Struct('B')     # for resolve action
    pageStorer = PageStorer.instance()

    def __init__(self, sock):
        super(HCrawl, self).__init__(sock)
        self.fileStorer = FileStorer()

    def handle(self, data):
        try:
            meta = config.jsoner.decode(data[0])
            content = zlib.decompress(data[1]) if meta['compressed'] else data[1]
            self.store(meta, content)
            self.mrads(content=content)
        except Exception, e:
            self.replyError()
            logger.error("crawl failed: " + str(e.args))

    def store(self, meta, content):
        self.fileStorer.store(content)
        self.pageStorer.store(meta, content)

class HShowAds(Handler):

    def __init__(self, sock):
        super(HShowAds, self).__init__(sock)

    def handle(self, data):
        global sock
        try:
            info = config.jsoner.decode(data)
            self.replyOk() if info else self.replyError()
        except Exception, e:
            self.replyError()
            logger.error("showAds failed: " + str(e.args))
