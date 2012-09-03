#coding:utf-8

import tornado.web
from config import sysconfig, logger, NotImplementedException

class Handler(tornado.web.RequestHandler):

    OK  = {sysconfig.ERR_CODE_KEY_NAME : sysconfig.ERR_CODE_OK}
    ERR  = {sysconfig.ERR_CODE_KEY_NAME : sysconfig.ERR_CODE_ERR}

    def __init__(self, application, request, **kw):
        super(Handler, self).__init__(application, request, **kw)
        self.out = ""

    def initialize(self, **kw):
        pass

    def get(self, *args, **kw):
        try:
            return self.handle()
        except Exception, e:
            logger.logException(e)

    def handle(self):
        self._prepare()
        self._handle()
        self._reply()

    def _prepare(self):
        self._clear_headers_for_304()
        self.set_header('Pragma', 'no-cache')
        self.set_header('Content-Type', 'application/x-javascript; charset=utf-8')
        self.clear_header('Server')

    def _reply(self):
        if not len(self.out):
            self._genOut()
        self._replyContent(self.out)

    def _replyContent(self, content):
        self.write(content)

    def compute_etag(self):
        return None

    def _genOut(self):
        raise NotImplementedException(this=self)

    def _handle(self):
        raise NotImplementedException(this=self)
