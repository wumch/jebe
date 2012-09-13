#coding:utf-8

import zlib
import struct
from config import config, logger
from model.pagestorer import PageStorer
from model.filestorer import FileStorer
from handler import Handler

class HCrawl(Handler):

    ucpacker = struct.Struct('B')     # for resolve action
    def __init__(self, sock):
        super(HCrawl, self).__init__(sock)
        self.fileStorer = FileStorer()
        self.pageStorer = PageStorer.instance()

    def handle(self, data):
        try:
            meta = config.jsoner.decode(data[0])
            content = zlib.decompress(data[1]) if meta['compressed'] else data[1]
            self.store(meta, content)
        except Exception, e:
            self.replyError()
            logger.error("crawl failed: " + str(e.args))

    def _onWords(self, words):
        self.mrads(words=words)

    def store(self, meta, content):
        self.fileStorer.store(content)
        return self.pageStorer.store(meta, content, callback=self._onWords)
