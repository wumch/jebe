
#def application(environ, start_response):
#    start_response('200 OK', [('Content-Type', 'text/plain')])
#    yield 'Hello World\n'
#    yield 'This is uWsgi application.\n'

from werkzeug.wrappers import Request
from tads.handler import HAdsByLoc

@Request.application
def application(request):
    handler = HAdsByLoc(request)
    handler.handle()
    return handler.response

if __name__ == '__main__':
    from werkzeug.serving import run_simple
    run_simple('localhost', 10020, application)
