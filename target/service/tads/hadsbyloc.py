#coding:utf-8

from config import config, sysconfig, logger, NotImplementedException
from model.leveldbstorer import LevelDBStorer
from drivers.locdb import LocDB
from drivers.ftengine import FTEngine
from handler import Handler

class HAdsByLoc(Handler):

    locdb = LocDB()
    ftengine = FTEngine()
    adsdb = LevelDBStorer(dbId='ads')
    cachedAds = {}
    jsCrawlPage = sysconfig.RPC_FUNC_NAME['crawlPage'] + '();'

    def __init__(self, application, request, **kw):
        super(HAdsByLoc, self).__init__(application, request, **kw)
        self.ads = []

    def _handle(self):
        self._fetchAds()
        self._filter()

    def _genOut(self):
        self.out = sysconfig.RPC_FUNC_NAME['showAds'] + '(' + config.jsoner.encode(self.ads or []) + ');'

    def _fetchAds(self):
        url = self.get_argument('url', None)
        if not isinstance(url, basestring):
            return
        words = self.locdb.marve(url)
        if words is False:      # page non-exists
            self.out = self.jsCrawlPage
            return
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

    def _filter(self):
        if len(self.ads) > sysconfig.MAX_ADS:
            self.ads = self.ads[:sysconfig.MAX_ADS]

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
