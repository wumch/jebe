#coding:utf-8

import sys
from werkzeug.wrappers import Response
from config import config, sysconfig, DEBUG
from model.matcher import Matcher

class Handler(object):

    OK  = {sysconfig.ERR_CODE_KEY_NAME : sysconfig.ERR_CODE_OK}
    ERR  = {sysconfig.ERR_CODE_KEY_NAME : sysconfig.ERR_CODE_ERR}

    def __init__(self, request):
        self.request = request
        self.response = Response(mimetype='application/json')
        self.response.headers['Pragma: no-cache'] = 'no-cache'
        self.params = self.request.args
        self.ads = []

    def handle(self):
        self._fetchAds()
        self._filter()
        self._reply()

    def _reply(self, data=None):
        if DEBUG:
            self.response.data = 'alert(%(ads)s);%(rpc)s(%(ads)s);' % {
                'rpc' : sysconfig.RPC_FUNC_NAME,
                'ads' : config.jsoner.encode(data or self.ads or []),
            }
        else:
            self.response.data = sysconfig.RPC_FUNC_NAME + '(' + config.jsoner.encode(data or self.ads or []) + ');'

    def _fetchAds(self):
        raise NotImplementedError("<%s>.%s" % (self.__class__.__name__, sys._getframe().f_code.co_name))

    def _filter(self):
        if len(self.ads) > sysconfig.MAX_ADS:
            self.ads = self.ads[:sysconfig.MAX_ADS]

class HAdsByLoc(Handler):

    matcher = Matcher()

    def __init__(self, request):
        super(HAdsByLoc, self).__init__(request)

    def _fetchAds(self):
        if DEBUG: return
        if 'url' in self.params:
            self.ads = self.matcher.match(loc=self.params['url'])
