#!/usr/bin/env python

try:
    import psyco
    psyco.jit()
    from psyco.classes import *
except ImportError:
    pass

import zmq, struct
from config import config, DEBUG, logger
from drivers.adsupplier import Adsupplier
from controler.handler import Handler
Handler.adsupplier = Adsupplier.instance()
from controler.hcrawl import HCrawl

context = zmq.Context(1)
sock = context.socket(zmq.XREP)
sock.connect("tcp://%(host)s:%(port)d" % config.getRouter())

class Dealer(object):

    ucpacker = struct.Struct('B')
#    handlerList = [None, HPageExists(sock), HCrawl(sock), HShowAds(sock), HPageAccesser(sock)]
    handlerList = [None, None, HCrawl(sock), None, None, None]

    def __init__(self): pass

    def _getHandler(self, header):
        index = self.ucpacker.unpack(header[0])[0]
        return self.handlerList[index] if 0 < index < len(self.handlerList) else None

    def handle(self, header, payload):
        handler = self._getHandler(header)
        if handler is not None:
            handler.handle(payload)

if __name__ == '__main__':
    if DEBUG: print 'running in DEBUG mode'
    dealer = Dealer()
    while True:
        data = sock.recv_multipart()
        if DEBUG: print "received", len(data)
        if len(data) > 1:
            try:
                dealer.handle(data[0], data[1:])
            except Exception, e:
                logger.logException(e)
        else:
            logger.error("kid, some bad guys sent len(data)<=1")
