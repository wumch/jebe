#!/usr/bin/env python

import zmq, struct
from config import config, DEBUG
from controler.handler import *

context = zmq.Context(1)
sock = context.socket(zmq.REP)
sock.connect("tcp://%(host)s:%(port)d" % config.getRouter())

class Dealer(object):

    ucpacker = struct.Struct('B')     # ny
    handlerList = [None, HPageExists(), HCrawl(), HShowAds()]
    if DEBUG:   # for test marve
        handlerList[2] = HMarve()

    def __init__(self): pass

    def _getHandler(self, header):
        index = self.ucpacker.unpack(header[0])[0]
        if not 0 < index < len(self.handlerList):
            global sock
            Handler.replyErr(sock)
        return self.handlerList[index]

    def handle(self, header, payload):
        return self._getHandler(header).handle(payload)

if __name__ == '__main__':
    dealer = Dealer()
    while True:
        data = sock.recv_multipart()
        if DEBUG: print "received some"
        if len(data) > 1:
            dealer.handle(data[0], data[1:])
        else:
            Handler.replyErr(sock)
