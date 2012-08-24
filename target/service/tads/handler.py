#coding:utf-8

import sys
from werkzeug.wrappers import Response
from config import config, sysconfig, logger, DEBUG
from model.matcher import Matcher
#from model.pagestorer import PageStorer

class Handler(object):

    OK  = {sysconfig.ERR_CODE_KEY_NAME : sysconfig.ERR_CODE_OK}
    ERR  = {sysconfig.ERR_CODE_KEY_NAME : sysconfig.ERR_CODE_ERR}

    def __init__(self, request):
        self.request = request
        self.response = Response(mimetype='application/x-javascript; charset=utf-8')
        self.response.headers['Pragma'] = 'no-cache'
        self.params = self.request.args
        self.ads = []

    def handle(self):
        self._fetchAds()
        self._filter()
        self._reply()

    def _reply(self):
        if not len(self.response.data):     # bug... how to reply empty string?
            self._replyAds()

    def _replyAds(self, ads=None):
        if DEBUG:
            self.response.data = 'alert(%(ads)s);%(rpc)s(%(ads)s);' % {
                'rpc' : sysconfig.RPC_FUNC_NAME['showAds'],
                'ads' : config.jsoner.encode(ads or self.ads or []),
            }
        else:
            self.response.data = sysconfig.RPC_FUNC_NAME['showAds'] + '(' + config.jsoner.encode(ads or self.ads or []) + ');'

    def _replyContent(self, content):
        self.response.data = content

    def _fetchAds(self):
        raise NotImplementedError("<%s>.%s" % (self.__class__.__name__, sys._getframe().f_code.co_name))

    def _filter(self):
        if len(self.ads) > sysconfig.MAX_ADS:
            self.ads = self.ads[:sysconfig.MAX_ADS]

class HAdsByLoc(Handler):

    matcher = Matcher.instance()

    def __init__(self, request):
        super(HAdsByLoc, self).__init__(request)

    def _fetchAds(self):
        if 'url' not in self.params:
            return
        url = self.params['url']
        if self.pageExists(url):
            self.ads = [{'link':a['link'], 'text':a['text'], 'id':a['id']} for a in self.matcher.match(loc=url)]
            if self.ads: logger.info(('ad shown: %(text)s [%(link)s] on [%%s]' % self.ads[0]) % url)
        else:
            self._replyContent(sysconfig.RPC_FUNC_NAME['crawlPage'] + '();')
#
#    def handle(self):
#        if self.pageExists():
#            super(HAdsByLoc, self).handle()
#        else:
#            self.re

    def pageExists(self, url):
        return self.matcher.pageExists(url)
