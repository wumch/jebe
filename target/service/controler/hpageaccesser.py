#coding:utf-8

from config import config
from model import *
from handler import Handler
from utils.MarveWords import MarveWords

class HPageAccesser(Handler):

    pageStorer = PageStorer.instance()

    def __init__(self, sock):
        super(HPageAccesser, self).__init__(sock)

    def handle(self, data):
        url = config.packer.decode(data[0])
        splited_content = self.pageStorer.fetchSplitedContent(url=url)
        words = MarveWords(content=splited_content).top()
        self.response(config.packer.encode(words))

class HPageExistsIPC(Handler):

    pageStorer = PageStorer.instance()

    def __init__(self, sock):
        super(HPageExistsIPC, self).__init__(sock)

    def handle(self, data):
        url = config.packer.decode(data[0])
        res = self.pageStorer.exists(url=url)
        self.response(config.packer.encode(res))
