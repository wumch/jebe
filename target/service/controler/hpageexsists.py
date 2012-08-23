#coding:utf-8

from config import config, logger
from model import *
from handler import Handler

class HPageExists(Handler):

    moveStorer = MoveStorer.instance()
    pageExists = PageStorer.instance()

    def __init__(self, sock):
        super(HPageExists, self).__init__(sock)

    def handle(self, data):
        try:
            info = config.jsoner.decode(data[0])
            self.moveStorer.store(info)
            if self.pageExists.exists(info['url']) is True:
                self.replyOk()
#                self.mrads(loc=info['url'])      # should also carry some ads.
            else:
                self.replyError()
        except Exception, e:
            self.replyOk()      # to make error-occured client no longer upload.
            logger.error("pageExists failed: " + str(e.args))
