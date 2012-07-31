#!/usr/bin/env python

import zmq
from natip import natip
import zlib, struct
from json import JSONDecoder

context = zmq.Context(2)
client = context.socket(zmq.REP)
client.connect("tcp://%s:10011" % natip)

class Handler(object):

    ERR_CODE_OK     = 'y'
    ERR_CODE_ERR    = 'n'
    jsonDecoder = JSONDecoder(encoding='utf-8')

    def __init__(self): pass
    def handle(self, data): pass

    def replyOk(self):
        global client
        client.send(self.ERR_CODE_OK)

    def replyErr(self):
        global client
        client.send(self.ERR_CODE_ERR)

class HPageExists(Handler):

    def __init__(self):
        super(HPageExists, self).__init__()

    def handle(self, data):
        global client
        try:
            info = self.jsonDecoder.decode(data)
        except Exception:
            client.send(self.ERR_CODE_ERR)
            print "pageExists failed"
            return
        print info
        client.send(self.ERR_CODE_OK)

class HCrawl(Handler):

    ucpacker = struct.Struct('B')     # ny

    def __init__(self):
        super(HCrawl, self).__init__()

    def _isCompressed(self, data):
        return self.ucpacker.unpack(data[0])[0] == ord(self.ERR_CODE_OK)

    def handle(self, data):
        global client
        try:
            meta = self.jsonDecoder.decode(data[0])
            content = zlib.decompress(data[1]) if meta['compressed'] else data[1]
        except Exception:
            client.send(self.ERR_CODE_ERR)
            print "crawl failed"
            return
        print meta
        print content
        client.send(self.ERR_CODE_OK)

class HShowAds(Handler):

    def __init__(self):
        super(HShowAds, self).__init__()

    def handle(self, data):
        global client
        try:
            info = self.jsonDecoder.decode(data)
        except Exception:
            client.send(self.ERR_CODE_ERR)
            print "showAds failed"
            return
        print info
        client.send(self.ERR_CODE_OK)

class Dealer(object):

    ucpacker = struct.Struct('B')     # ny
    handlerList = [None, HPageExists(), HCrawl(), HShowAds()]

    def __init__(self, _data):
        self.header = _data[0]
        self.payload = _data[1:]

    def _getHandler(self):
        index = self.ucpacker.unpack(self.header[0])[0]
        assert 0 < index < len(self.handlerList)
        return self.handlerList[index]

    def handle(self):
        return self._getHandler().handle(self.payload)

if __name__ == '__main__':
    while True:
        data = client.recv_multipart()
        assert len(data) > 1
        Dealer(data).handle()
