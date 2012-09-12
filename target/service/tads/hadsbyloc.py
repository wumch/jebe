#coding:utf-8

from config import sysconfig, logger
from drivers.locdb import LocDB
from handler import Handler
from drivers.adsupplier import Adsupplier

class HAdsByLoc(Handler):

    locdb = LocDB()
    jsCrawlPage = sysconfig.RPC_FUNC_NAME['crawlPage'] + '();'
    adsupplier = Adsupplier.instance()

    def __init__(self, request, params):
        super(HAdsByLoc, self).__init__(request=request, params=params)
        self.pageUrl = self.params['url'] if 'url' in self.params else None

    def _handle(self):
        self._fetchAds()

    def _genOut(self):
        self.out = sysconfig.RPC_FUNC_NAME['showAds'] + '([' + ','.join(self.ads) + ']);'

    def _fetchAds(self):
        if not isinstance(self.pageUrl, basestring):
            return []
        self.locdb.marve(self.pageUrl, callback=self._handleResult)

    def _processResult(self, words):
        if words is False:      # page non-exists
            return self.jsCrawlPage
        if words is None:       # error occured (from locdb server)
            return []
        self.adsupplier.byMarvedWords(words=words, callback=self._onAds)

    def _onAds(self, ads):
        self.ads = ads
        self._logShownAds()
        self._filter()
        self._genOut()
        self._reply()

    def _logShownAds(self):
        if len(self.ads):
            shown = ','.join(self.ads)
            logger.info('ad shown: [' + shown + '] on ' + self.pageUrl)

    def _filter(self):
        if len(self.ads) > sysconfig.MAX_ADS:
            self.ads = self.ads[:sysconfig.MAX_ADS]
