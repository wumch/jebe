#coding:utf-8

from config import config, logger
from handler import Handler

class HShowAds(Handler):

    def __init__(self, sock):
        super(HShowAds, self).__init__(sock)

    def handle(self, data):
        global sock
        try:
            info = config.jsoner.decode(data)
            self.replyOk() if info else self.replyError()
        except Exception, e:
            self.replyError()
            logger.error("showAds failed: " + str(e.args))
