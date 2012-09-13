#coding:utf-8

import tornado.web
from config import sysconfig, logger, NotImplementedException

class Handler(object):

    OK  = {sysconfig.ERR_CODE_KEY_NAME : sysconfig.ERR_CODE_OK}
    ERR  = {sysconfig.ERR_CODE_KEY_NAME : sysconfig.ERR_CODE_ERR}

    statusMap = {
        200 : 'OK',
        404 : 'Not Found',
        500 : 'Internal Error',
    }
    headers = (
        ('Pragma', 'no-cache'),
        ('Content-Type', 'application/x-javascript; charset=utf-8'),
    )

    def __init__(self, request, params):
        self.request = request
        self.params = params
        self.status = 200
        self.out = ''

    def handle(self):
        self._handle()

    def _handleResult(self, data):
        self._processResult(data)

    def _processResult(self, data):
        pass

    def initialize(self, **kw):
        pass

    def _prepare(self):
        pass

    def _handle(self):
        raise NotImplementedException(this=self)

    def _reply(self, content=None):
        for name, value in self.headers:
            self.request.add_output_header(name, value)
        self.request.send_reply(self.status, self.statusMap[self.status], content or self.out)
        self.request.send_reply_end()
