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

from werkzeug.wrappers import Request
from tads.handler import HAdsByLoc, config

@Request.application
def application(request):
    handler = HAdsByLoc(request)
    handler.handle()
    return handler.response

if __name__ == '__main__':
    from werkzeug.serving import run_simple
    run_simple('localhost', 10020, application)
