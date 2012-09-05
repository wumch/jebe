#coding:utf-8

import sys
import zlib
from urllib2 import urlopen
from config import config, sysconfig, logger, DEBUG
from utils.MarveWords import MarveWords
from drivers.adsupplier import Adsupplier

class Handler(object):

    OK  = {sysconfig.ERR_CODE_KEY_NAME : sysconfig.ERR_CODE_OK}
    ERR  = {sysconfig.ERR_CODE_KEY_NAME : sysconfig.ERR_CODE_ERR}
    adsupplier = Adsupplier.instance()

    def __init__(self, sock):
        self.sock = sock

    def handle(self, data):
        raise NotImplementedError("<%s>.%s" % (self.__class__.__name__, sys._getframe().f_code.co_name))

    def replyOk(self):
        self.response(self.OK)

    def replyError(self):
        self.replyErr(self.sock)

    @classmethod    # to make global callable.
    def replyErr(cls, sock):
        sock.send(config.jsoner.encode(cls.ERR))

    def response(self, data):
        self.sock.send(data if isinstance(data, basestring) else config.jsoner.encode(data))

    def mrads(self, **kwargs_for_get_ads):
        """
        match and reply ads.
        @param kwargs_for_get_ads
            some arguments for get ads.
        """
        self.response(self._getAds(**kwargs_for_get_ads))

    def _getAds(self, content=None, words=None, loc=None):
        if words:
            ads = self.adsupplier.byMarvedWords(words=words)
            if ads:
                res = '[' + ','.join(ads) + ']'
                logger.info('ad shown by dealer: ' + res)
                return res
        return []

class HMarve(Handler):

    def __init__(self, sock):
        super(HMarve, self).__init__(sock)

    def handle(self, data):
        try:
            meta = config.jsoner.decode(data[0])
            content = zlib.decompress(data[1]) if meta['compressed'] else data[1]
            self.response(self.marve(content))
        except Exception, e:
            self.replyError()
            logger.error("marve failed: " + str(e.args))

    def marve(self, content):
        try:
            json = urlopen(config.getTokenizer('marve'), data=content, timeout=3).read()
            res = MarveWords(config.jsoner.decode(json)).top()
            if DEBUG: print res
            return res
        except Exception:
            return None
