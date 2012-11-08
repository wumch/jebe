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
        return self.store() if self.env['REQUEST_METHOD'] == 'POST' else False

    def store(self):
        if not self.postData:
            return False
        if self.postData[-1] != 'V' or self.postData[0] != 'A':
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

