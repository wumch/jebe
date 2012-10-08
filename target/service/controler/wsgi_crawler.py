#coding:utf-8

import zlib
from config import config, logger
from wsgi_controler import WsgiControler
from model.mpagestorer import PageStorer

class WsgiCrawler(WsgiControler):

    def __init__(self, environ, start_response):
        super(WsgiCrawler, self).__init__(environ, start_response)
        self.postData = self.env.get('wsgi.input').read() if 'wsgi.input' in self.env else None
        self.pageStorer = PageStorer.instance()

    def handle(self):
        return self.store()

    def store(self):
        if not self.postData:
            return False
        try:
            info = config.jsoner.decode(zlib.decompress(self.postData))
            self._store(url=info['url'], content=info['text'], links=info['links'])
            return True
        except Exception, e:
            logger.error("crawl failed: " + str(e.args))
            return False

    def _store(self, url, content, links):
        return self.pageStorer.storeInfo(url=url, content=content, links=links)

