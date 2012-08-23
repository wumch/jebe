#coding:utf-8

from config import config
from model import *
from handler import Handler

class HPageAccesser(Handler):

    pageStorer = PageStorer.instance()

    def __init__(self, sock):
        super(HPageAccesser, self).__init__(sock)

    def handle(self, data):
        print data
        url = config.packer.decode(data[0])
        print 'page access: [%s]' % url
        words = config.packer.encode(self.pageStorer.fetchSplitedContent(url=url))
        print words
        self.response(words)

class HPageExistsIPC(Handler):

    pageStorer = PageStorer.instance()

    def __init__(self, sock):
        super(HPageExistsIPC, self).__init__(sock)

    def handle(self, data):
        url = config.packer.decode(data[0])
        res = self.pageStorer.exists(url=url)
        print 'page exists: %s [%s]' % (res, url)
        self.response(config.packer.encode(res))
