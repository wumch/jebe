#coding:utf-8

import zlib
import struct
from config import config, logger
from model.mpagestorer import PageStorer
from handler import Handler

class HCrawl(Handler):

    ucpacker = struct.Struct('B')     # for resolve action
    def __init__(self, sock):
        super(HCrawl, self).__init__(sock)
        self.pageStorer = PageStorer.instance()

    def store(self, data):
        try:
            meta = config.jsoner.decode(data[0])
            content = zlib.decompress(data[1]) if meta['compressed'] else data[1]
            self._store(meta, content)
            self.replyOk()
        except Exception, e:
            self.replyError()
            logger.error("crawl failed: " + str(e.args))

    def pageExists(self, data):
        try:
            meta = config.jsoner.decode(data[0])
            if self._pageExists(meta['url']):
                self.replyError()
            else:
                self.replyOk()
        except Exception, e:
            self.replyError()
            logger.logException(e)

    def _pageExists(self, url):
        return self.pageStorer.exists(url=url)

    def _store(self, meta, content):
        return self.pageStorer.store(meta, content)
