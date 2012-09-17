#coding:utf-8

from config import config, sysconfig
from drivers.locdb import LocDB
from handler import Handler

class HKWByLoc(Handler):

    locdb = LocDB()
    jsNoKw = sysconfig.RPC_FUNC_NAME['kwOfLoc'] + '=[];'

    def __init__(self, request, params):
        super(HKWByLoc, self).__init__(request, params)
        self.words = []

    def _handle(self):
        self._fetchWords()

    def _genOut(self):
        content = config.jsoner.encode(self.words)
        if sysconfig.RPC_KEY_NAME in self.params:
            self.out = self.params[sysconfig.RPC_KEY_NAME] + '(' + content  + ');'
        else:
            self.out = content

    def _fetchWords(self):
        url = self._getPageUrl()
        if not isinstance(url, basestring):
            return self._processResult(self.words)
        self.locdb.marve(url, callback=self._processResult)

    def _getPageUrl(self):
        if 'url' in self.params:
           return self.params['url']
        else:
            headers = self.request.get_input_headers()
            for name, value in headers:
                if name.lower() == 'referer':
                    return value

    def _processResult(self, words):
        self.words = [w[0] for w in (words or [])]
        self._filter()
        self._genOut()
        self._reply()

    def _filter(self):
        if len(self.words) > sysconfig.MAX_KW_OF_LOC:
            self.words = self.words[:sysconfig.MAX_KW_OF_LOC]
