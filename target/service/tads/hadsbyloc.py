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
        self.ads = []
        self.pageUrl = self._getPageUrl()

    def _handle(self):
        self._fetchAds()

    def _genOut(self):
        content = '[' + ','.join(self.ads) + ']'
        if sysconfig.RPC_KEY_NAME in self.params:
            rpcname = self.params[sysconfig.RPC_KEY_NAME]
            self.out = content if rpcname == '0' else (rpcname + '(' + content  + ');')
        else:
#            self.out = content
            self.out = sysconfig.RPC_FUNC_NAME['showAds'] + '(' + content + ');'

    def _fetchAds(self):
        if not isinstance(self.pageUrl, basestring):
            return self._finish()
        self.locdb.marve(self.pageUrl, callback=self._handleResult)

    def _getPageUrl(self):
        if 'url' in self.params:
           return self.params['url']
        else:
            headers = self.request.get_input_headers()
            for name, value in headers:
                if name.lower() == 'referer':
                    return value.strip()

    def _processResult(self, words):
        if words is False:      # page non-exists
            return self._reply(content=self.jsCrawlPage)
        self.adsupplier.byMarvedWords(words=words or [], callback=self._onAds)

    def _onAds(self, ads):
        self.ads = ads
        self._logShownAds()
        self._filter()
        self._finish()

    def _finish(self):
        self._genOut()
        self._reply()

    def _logShownAds(self):
        if len(self.ads):
            shown = ','.join(self.ads)
            logger.info('ad shown: [' + shown + '] on ' + self.pageUrl)

    def _filter(self):
        if len(self.ads) > sysconfig.MAX_ADS:
            self.ads = self.ads[:sysconfig.MAX_ADS]
