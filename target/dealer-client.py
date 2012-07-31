#!/usr/bin/env python

import zmq
from natip import natip
import zlib, struct
from json import JSONDecoder, JSONEncoder

context = zmq.Context(io_threads=1)
sock = context.socket(zmq.REP)
sock.connect("tcp://%s:10011" % natip)

class Handler(object):

    ERR_CODE_OK     = 'y'
    ERR_CODE_ERR    = 'n'
    jsonDecoder = JSONDecoder(encoding='utf-8')
    jsonEncoder = JSONEncoder()

    def __init__(self):
        self.out = self.ERR_CODE_ERR

    def handle(self, data): pass

    def replyOk(self):
        global sock
        sock.send(self.ERR_CODE_OK)

    @classmethod    # to make global callable.
    def replyErr(cls):
        global sock
        sock.send(cls.ERR_CODE_ERR)

    def response(self, data = None):
        global sock
        d = data or self.out
        sock.send(d if isinstance(d, basestring) else self.jsonEncoder.encode(d))

    def __del__(self):
        self.response(self.out)

class HPageExists(Handler):

    def __init__(self):
        super(HPageExists, self).__init__()

    def handle(self, data):
        """
        case:
            non-exists: tell client, ask for details of the page.
            exists: response "all right" with some ads.
        """
        global sock
        try:
            info = self.jsonDecoder.decode(data)
        except Exception:
            sock.send(self.ERR_CODE_ERR)
            print "pageExists failed"
            return
        print info
        sock.send(self.ERR_CODE_OK)

class HCrawl(Handler):

    ucpacker = struct.Struct('B')     # ny

    def __init__(self):
        super(HCrawl, self).__init__()

    def handle(self, data):
        global sock
        try:
            meta = self.jsonDecoder.decode(data[0])
            content = zlib.decompress(data[1]) if meta['compressed'] else data[1]
        except Exception:
            sock.send(self.ERR_CODE_ERR)
            print "crawl failed"
            return
        print meta
        print content
        sock.send(self.ERR_CODE_OK)

class HShowAds(Handler):

    def __init__(self):
        super(HShowAds, self).__init__()

    def handle(self, data):
        global sock
        try:
            info = self.jsonDecoder.decode(data)
        except Exception:
            sock.send(self.ERR_CODE_ERR)
            print "showAds failed"
            return
        print info
        sock.send(self.ERR_CODE_OK)

class Dealer(object):

    ucpacker = struct.Struct('B')     # ny
    handlerList = [None, HPageExists(), HCrawl(), HShowAds()]

    def __init__(self): pass

    def _getHandler(self, header):
        index = self.ucpacker.unpack(header[0])[0]
        if not 0 < index < len(self.handlerList):
            Handler.replyErr()
        return self.handlerList[index]

    def handle(self, header, payload):
        return self._getHandler(header).handle(payload)

if __name__ == '__main__':
    dealer = Dealer()
    while True:
        data = sock.recv_multipart()
        if len(data) > 1:
            dealer.handle(data[0], data[1:])
        else:
            Handler.replyErr()
