#coding:utf-8

import zlib
import urlparse
from config import config, sysconfig, logger
from wsgi_controler import WsgiControler
from model.mpagestorer import PageStorer
from model.crawlqueue import CrawlQueue

detach_jsonp = '''
window.i8_next_task = function(){document.location.href = '%%s';}
var script = document.createElement('script');
script.charset = 'utf-8';
script.type = 'text/javascript';
script.src = 'http://%s/crawler.js';
document.body.insertBefore(script, document.body.firstChild);
''' % sysconfig.CRAWLER_DOMAIN

class WsgiCrawler(WsgiControler):

    def __init__(self, environ, start_response):
        super(WsgiCrawler, self).__init__(environ, start_response)

    def handle(self):
        try:
            return self._handle()
        except Exception, e:
            logger.logException(e)

    def _handle(self):
        if self.env['REQUEST_METHOD'] == 'POST':
            return self._prepare_store() and self.store()
        else:
            if 'PATH_INFO' in self.env:
                if self.env['PATH_INFO'].startswith('/crawler/task/attach'):
                    return self._prepare_attach() and self.attach_task()
                elif self.env['PATH_INFO'].startswith('/crawler/task/detach'):
                    return self._prepare_detach() and self.detach_task()

    def attach_task(self):
        CrawlQueue.instance().attach(self.url)
        return True

    def detach_task(self):
        url = CrawlQueue.instance().detach()
        return (detach_jsonp % url) if url else ''

    def _prepare_attach(self):
        if 'QUERY_STRING' in self.env:
            res = urlparse.parse_qsl(self.env['QUERY_STRING'], strict_parsing=False)
            for name, value in res:
                if name == 'url' and "'" not in value:
                    self.url = value
                    return True
        elif 'HTTP_REFERER' in self.env:
            self.url = self.env['HTTP_REFERER']
            return True
        return False

    def _prepare_detach(self):
        return True

    def _prepare_store(self):
        if 'CONTENT_LENGTH' in self.env and 'wsgi.input' in self.env:
            self.postData = self.env.get('wsgi.input').read(int(self.env['CONTENT_LENGTH']))
            self.pageStorer = PageStorer.instance()
            return True
        return False

    def store(self):
        if not self.postData:
            return False
        try:
            info = config.jsoner.decode(zlib.decompress(self.postData))
            self._store(url=info['url'], title=info['title'], content=info['text'], links=info['links'])
            return True
        except Exception, e:
            logger.logException(e)
            return False

    def _store(self, url, title, content, links):
        return self.pageStorer.storeInfo(url=url, title=title, content=content, links=links)

