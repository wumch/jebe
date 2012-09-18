#coding:utf-8

from gevent.wsgi import WSGIHandler
from config import logger
from tads.hadsbyloc import HAdsByLoc
from tads.hkwbyloc import HKWByLoc
from urlparse import urlparse, parse_qs

class WsgiApp(object):

    handlerMap = {
        'target' : HAdsByLoc,
        'kwofloc': HKWByLoc,
    }
    status = {
        200 : 'OK',
        404 : 'Not Found',
        500 : 'Internal Error',
    }
    headers = (
        ('Pragma', 'no-cache'),
        ('Content-Type', 'application/x-javascript; charset=utf-8'),
    )
    default_action = 'target'

    def __init__(self):
        pass

    def __call__(self, request):
        try:
            self._process(request)
        except Exception, e:
            logger.logException(e)
            request.send_error()

    def _process(self, request):
        handlerClass, params = self.parseUri(request.uri)
        if handlerClass:
            try:
                handler = handlerClass(request, params)
                return handler.handle()
            except Exception, e:
                status = 500
                print str(e.args)
                logger.logException(e)
        else:
            status = 404
        self.reply(request, status, '')

    def reply(self, request, status, content):
        for name, value in self.headers:
            request.add_output_header(name, value)
        request.send_reply(status, self.status[status], content)
        request.send_reply_end()

    def parseUri(self, uri):
        urlinfo = urlparse(uri)
        action = urlinfo.path.strip('/') if hasattr(urlinfo, 'path') else self.default_action
        qs = getattr(urlinfo, 'query')
        if qs:
            params = dict(parse_qs(qs=qs))
            for k, v in params.iteritems():
                if isinstance(v, basestring):
                    params[k] = v.strip()
                elif isinstance(v, list):
                    params[k] = v[-1].strip() if len(v) else ''
        else:
            params = {}
        if 'callback' not in params:
            if '?callback=' in qs or '&callback=' in qs:
                params['callback'] = '0'
        return self._pickHandler(action), params

    def _pickHandler(self, action):
        return self.handlerMap[action] if action in self.handlerMap else None

class WsgiWrap(WSGIHandler):

    def __init__(self, request, server):
        super(WsgiWrap, self).__init__(request, server)
        print request

