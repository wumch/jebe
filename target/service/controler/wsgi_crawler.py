#coding:utf-8

import os
import zlib
from config import config, logger
from wsgi_controler import WsgiControler
from model.mpagestorer import PageStorer

class WsgiCrawler(WsgiControler):

    def __init__(self, environ, start_response):
        super(WsgiCrawler, self).__init__(environ, start_response)
        if 'CONTENT_LENGTH' in self.env and 'wsgi.input' in self.env:
            self.postData = self.env.get('wsgi.input').read(int(self.env['CONTENT_LENGTH']))
        else:
            logger.error("environ: " + str(self.env))
            self.postData = None
        self.pageStorer = PageStorer.instance()

    def handle(self):
        return self.store() if self.env['REQUEST_METHOD'] == 'POST' else False

    def store(self):
        if not self.postData:
            return False
        if self.postData[-1] != 'V' or self.postData[0] != 'A':
            logger.error("<%s(%d/%d)>[%s][%s]" % (type(self.postData), len(self.postData), int(self.env['CONTENT_LENGTH']), self.postData[:5], self.postData[-5:]))
            return 405
        try:
            info = config.jsoner.decode(zlib.decompress(self.postData[2:-1]))
            self._store(url=info['url'], title=info['title'], content=info['text'], links=info['links'])
            return True
        except Exception, e:
            logger.logException(e)
            return False

    def _store(self, url, title, content, links):
        return self.pageStorer.storeInfo(url=url, title=title, content=content, links=links)

