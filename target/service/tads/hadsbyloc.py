#coding:utf-8

from config import sysconfig, logger
from drivers.locdb import LocDB
from handler import Handler
from drivers.adsupplier import Adsupplier

class HAdsByLoc(Handler):

    locdb = LocDB()
    jsCrawlPage = sysconfig.RPC_FUNC_NAME['crawlPage'] + '();'
    adsupplier = Adsupplier.instance()

    def __init__(self, application, request, **kw):
        super(HAdsByLoc, self).__init__(application, request, **kw)
        self.ads = []

    def _handle(self):
        self._fetchAds()
        self._filter()

    def _genOut(self):
        self.out = sysconfig.RPC_FUNC_NAME['showAds'] + '([' + ','.join(self.ads or []) + ']);'

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
        self.ads = self.adsupplier.byMarvedWords(words=words)
        self._logShownAds(url)

    def _logShownAds(self, pageUrl):
        if len(self.ads):
            shown = ','.join(self.ads)
            logger.info('ad shown: [' + shown + '] on ' + pageUrl)

    def _filter(self):
        if len(self.ads) > sysconfig.MAX_ADS:
            self.ads = self.ads[:sysconfig.MAX_ADS]
