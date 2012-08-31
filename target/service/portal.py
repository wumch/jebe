#coding:utf-8
#def application(environ, start_response):
#    start_response('200 OK', [('Content-Type', 'text/plain')])
#    yield 'Hello World\n'
#    yield 'This is uWsgi application.\n'

try:
    import psyco
    psyco.jit()
    from psyco.classes import *
except ImportError:
    pass

import tornado.ioloop
import tornado.web
from tads.handler import HAdsByLoc, logger
from utils.natip import natip

class Portal(tornado.web.RequestHandler):
    def get(self, *args, **kw):
        try:
            handler = HAdsByLoc(self.application, self.request)
            handler.handle()
        except Exception, e:
            print e.args

app = tornado.web.Application([
    (r"/target", HAdsByLoc)
])

if __name__ == '__main__':
    app.listen(port=10020, address=natip)
    tornado.ioloop.IOLoop.instance().start()
