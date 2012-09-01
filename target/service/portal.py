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
from tads.handler import HAdsByLoc
from utils.natip import natip

app = tornado.web.Application([
    (r"/target/", HAdsByLoc),
])

if __name__ == '__main__':
    app.listen(port=10020, address=natip)
    tornado.ioloop.IOLoop.instance().start()
