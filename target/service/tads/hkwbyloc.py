#coding:utf-8

from config import config, sysconfig, logger, NotImplementedException
from model.leveldbstorer import LevelDBStorer
from drivers.locdb import LocDB
from handler import Handler

class HKWByLoc(Handler):

    locdb = LocDB()
    jsNoKw = sysconfig.RPC_FUNC_NAME['kwOfLoc'] + '();'

    def __init__(self, application, request, **kw):
        super(HKWByLoc, self).__init__(application, request, **kw)
        self.words = []

    def _handle(self):
        self._fetchWords()
        self._filter()

    def _genOut(self):
        self.out = sysconfig.RPC_FUNC_NAME['kwOfLoc'] + '(' + config.jsoner.encode(self.words or []) + ');'

    def _fetchWords(self):
        url = self.get_argument('url', None)
        if not isinstance(url, basestring):
            return
        words = self.locdb.marve(url)
        print words
        self.words = [w[0] for w in (words or [])]

    def _filter(self):
        if len(self.words) > sysconfig.MAX_KW_OF_LOC:
            self.words = self.words[:sysconfig.MAX_KW_OF_LOC]
