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
from tads.hadsbyloc import HAdsByLoc
from tads.hkwbyloc import HKWByLoc
from utils.natip import natip
from config import DEBUG

app = tornado.web.Application([
    (r"/target/", HAdsByLoc),
    (r"/kwofloc/", HKWByLoc),
], debug=DEBUG)

if __name__ == '__main__':
    app.listen(port=10020, address=natip)
    tornado.ioloop.IOLoop.instance().start()
