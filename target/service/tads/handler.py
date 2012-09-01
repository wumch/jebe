#coding:utf-8

import sys
import tornado.web
from config import config, sysconfig, logger, DEBUG, NotImplementedException
from model.leveldbstorer import LevelDBStorer
from drivers.locdb import LocDB
from drivers.ftengine import FTEngine

class Handler(tornado.web.RequestHandler):

    OK  = {sysconfig.ERR_CODE_KEY_NAME : sysconfig.ERR_CODE_OK}
    ERR  = {sysconfig.ERR_CODE_KEY_NAME : sysconfig.ERR_CODE_ERR}

    def __init__(self, application, request, **kw):
        super(Handler, self).__init__(application, request, **kw)
        self.params = {'url':self.get_argument('url', None)}
        self.ads = []
        self.out = ""

    def initialize(self, **kw):
        pass

    def get(self, *args, **kw):
        try:
            return self.handle()
        except Exception, e:
            logger.logException(e)

    def handle(self):
        self._prepare()
        self._fetchAds()
        self._filter()
        self._reply()

    def _prepare(self):
        self._clear_headers_for_304()
        self.set_header('Pragma', 'no-cache')
        self.set_header('Content-Type', 'application/x-javascript; charset=utf-8')
        self.clear_header('Server')

    def _reply(self):
        if not len(self.out) and self.ads:     # bug... how to reply empty string?
            self._genOut()
        self._replyContent(self.out)

    def _genOut(self, ads=None):
        self.out = sysconfig.RPC_FUNC_NAME['showAds'] + '(' + config.jsoner.encode(ads or self.ads or []) + ');'

    def _replyContent(self, content):
        self.write(content)

    def _fetchAds(self):
        raise NotImplementedException()

    def _filter(self):
        if len(self.ads) > sysconfig.MAX_ADS:
            self.ads = self.ads[:sysconfig.MAX_ADS]

    def compute_etag(self):pass

class HAdsByLoc(Handler):

    locdb = LocDB()
    ftengine = FTEngine()
    adsdb = LevelDBStorer(dbId='ads')
    cachedAds = {}
    jsCrawlPage = sysconfig.RPC_FUNC_NAME['crawlPage'] + '();'

    def __init__(self, application, request, **kw):
        super(HAdsByLoc, self).__init__(application, request, **kw)

    def _fetchAds(self):
        url = self.params['url']
        if not isinstance(url, basestring):
            return self._reply()
        words = self.locdb.marve(url)
        if words is False:      # page non-exists
            return self._replyContent(self.jsCrawlPage)
        if words is None:       # error occured (from locdb server)
            return
        adids = self.ftengine.match(words=words)
        self.ads = [self.getAd(adid[0]) for adid in adids]
        self._logShownAds(url)

    def _logShownAds(self, pageUrl):
        if self.ads:
            shown = ','.join([('%(text)s [%(link)s]' % ad) for ad in self.ads])
            logger.info('ad shown: ' + shown + ' on ' + pageUrl)

    def getAd(self, adid):
        if adid not in self.cachedAds:
            ad = self.adsdb.getAuto(adid)
            if ad:
                self.cachedAds[adid] = {
                    'id' : ad['id'],
                    'link' : ad['link'],
                    'text' : ad['text']
                }
        return self.cachedAds.get(adid)

    def pageExists(self, url):
        return self.locdb.marve(url)

    @classmethod
    def _initAds(cls):
        for aid in xrange(1, 150):
            ad = cls.adsdb.getAuto(aid)
            if ad is not None:
                cls.cachedAds[aid] = {
                    'id' : ad['id'],
                    'link' : ad['link'],
                    'text' : ad['text']
                }
        del cls.adsdb.db

if not HAdsByLoc.cachedAds:
    HAdsByLoc._initAds()
