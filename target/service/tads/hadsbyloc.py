#coding:utf-8

from config import config, sysconfig, logger, NotImplementedException
from model.leveldbstorer import LevelDBStorer
from drivers.locdb import LocDB
from drivers.ftengine import FTEngine
from handler import Handler
from tads.fallbackads import FallbackAd

class HAdsByLoc(Handler):

    locdb = LocDB()
    ftengine = FTEngine()
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
        self.ads = filter(None, [self.getAd(adid) for adid in adids])
        if not self.ads:
            self._fallback()
        self._logShownAds(adids, url)

    def _logShownAds(self, adids, pageUrl):
        if self.ads:
            shown = ','.join(map(str, adids))
            logger.info('ad shown: [' + shown + '] on ' + pageUrl)

    def getAd(self, adid):
#        if adid not in self.cachedAds:
#            adsdb = self._getAdsDB()
#            ad = adsdb.getAuto(adid)
#            del adsdb
#            if ad:
#                self.cachedAds[adid] = {
#                    'id' : ad['id'],
#                    'link' : ad['link'],
#                    'text' : ad['text']
#                }
        return self.cachedAds.get(adid)

    def _filter(self):
        if len(self.ads) > sysconfig.MAX_ADS:
            self.ads = self.ads[:sysconfig.MAX_ADS]
        if len(self.ads) == 1 and '119,' in self.ads[0]:
            self.ads[0] = FallbackAd().getAd()

    def _fallback(self):
        if not self.ads:
            self.ads = [FallbackAd().getAd()]

    @classmethod
    def _getAdsDB(cls):
        return LevelDBStorer(dbId='ads')

    @classmethod
    def _initAds(cls):
        from ads import ads
        cls.cachedAds = ads
        return
        adsdb = cls._getAdsDB()
        for aid in xrange(1, 150):
            ad = adsdb.getAuto(aid)
            if ad is not None:
                cls.cachedAds[aid] = {
                    'id' : ad['id'],
                    'link' : ad['link'],
                    'text' : ad['text']
                }
        del adsdb

if not HAdsByLoc.cachedAds:
    HAdsByLoc._initAds()
