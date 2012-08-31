#coding:utf-8

import sys
import tornado.web
from config import config, sysconfig, logger, DEBUG
from model.leveldbstorer import LevelDBStorer
from drivers.locdb import LocDB
from drivers.ftengine import FTEngine

class Handler(tornado.web.RequestHandler):

    OK  = {sysconfig.ERR_CODE_KEY_NAME : sysconfig.ERR_CODE_OK}
    ERR  = {sysconfig.ERR_CODE_KEY_NAME : sysconfig.ERR_CODE_ERR}

    def __init__(self, application, request, **kw):
        super(Handler, self).__init__(application, request, **kw)
        self.add_header('Pragma', 'no-cache')
        self.params = {'url':self.get_argument('url', None)}
        self.ads = []
        self.out = ""

    def get(self, *args, **kw):
        return self.handle()

    def handle(self):
        self._fetchAds()
        self._filter()
        self._reply()

    def _reply(self):
        if not len(self.out) and self.ads:     # bug... how to reply empty string?
            self._replyAds()
        self._replyContent(self.out)

    def _replyAds(self, ads=None):
        if DEBUG:
            self.out = 'alert(%(ads)s);%(rpc)s(%(ads)s);' % {
                'rpc' : sysconfig.RPC_FUNC_NAME['showAds'],
                'ads' : config.jsoner.encode(ads or self.ads or []),
            }
        else:
            self.out = sysconfig.RPC_FUNC_NAME['showAds'] + '(' + config.jsoner.encode(ads or self.ads or []) + ');'

    def _replyContent(self, content):
        self.write(content)

    def _fetchAds(self):
        raise NotImplementedError("<%s>.%s" % (self.__class__.__name__, sys._getframe().f_code.co_name))

    def _filter(self):
        if len(self.ads) > sysconfig.MAX_ADS:
            self.ads = self.ads[:sysconfig.MAX_ADS]

class HAdsByLoc(Handler):

    locdb = LocDB()
    ftengine = FTEngine()
    adsdb = LevelDBStorer(dbId='ads')
    cachedAds = {}

    def __init__(self, application, request, **kw):
        super(HAdsByLoc, self).__init__(application, request, **kw)

    def _fetchAds(self):
        url = self.params['url']
        if url is None:
            return self._reply()
        words = self.locdb.marve(url)
        if words is False:
            return self._replyContent(sysconfig.RPC_FUNC_NAME['crawlPage'] + '();')
        adids = self.ftengine.match(words=words)
        self.ads = [self.getAd(adid[0]) for adid in adids]
        if self.ads: logger.info(('ad shown: %(text)s [%(link)s] on [%%s]' % self.ads[0]) % url)

    def getAd(self, adid):
        if adid not in self.cachedAds:
            self.cachedAds[adid] = self.adsdb.getAuto(adid)
        return self.cachedAds.get(adid)
#
#    def handle(self):
#        if self.pageExists():
#            super(HAdsByLoc, self).handle()
#        else:
#            self.re

    def pageExists(self, url):
        return self.locdb.marve(url)
