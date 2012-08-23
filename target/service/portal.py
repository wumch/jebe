#coding:utf-8
#def application(environ, start_response):
#    start_response('200 OK', [('Content-Type', 'text/plain')])
#    yield 'Hello World\n'
#    yield 'This is uWsgi application.\n'

from werkzeug.wrappers import Request
from tads.handler import HAdsByLoc, config

@Request.application
def application(request):
    handler = HAdsByLoc(request)
    handler.handle()
    print 'handler.response.data: [%s]' % handler.response.data
#    ad = {'text':u'晕倒', 'link':'http://baidu.com/','id':10087}
#    handler.response.data = 'i8vars.showAds(%s);' % config.jsoner.encode([ad])
    return handler.response

if __name__ == '__main__':
    from werkzeug.serving import run_simple
    run_simple('localhost', 10020, application)
