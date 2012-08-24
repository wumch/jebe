#!/usr/bin/env python

try:
    import psyco
    psyco.jit()
    from psyco.classes import *
except ImportError:
    pass

import zmq, struct
from config import config, DEBUG
from controler.handler import Handler
from controler.hcrawl import HCrawl
from controler.hpageaccesser import HPageAccesser, HPageExistsIPC

context = zmq.Context(1)
sock = context.socket(zmq.REP)
sock.connect("tcp://%(host)s:%(port)d" % config.getRouter())

class Dealer(object):

    ucpacker = struct.Struct('B')
#    handlerList = [None, HPageExists(sock), HCrawl(sock), HShowAds(sock), HPageAccesser(sock)]
    handlerList = [None, None, HCrawl(sock), None, HPageAccesser(sock), HPageExistsIPC(sock)]

    def __init__(self): pass

    def _getHandler(self, header):
        index = self.ucpacker.unpack(header[0])[0]
        if not 0 < index < len(self.handlerList):
            global sock
            Handler.replyErr(sock)
        return self.handlerList[index]

    def handle(self, header, payload):
        handler = self._getHandler(header)
        if handler is None:
            global sock
            Handler.replyErr(sock)
        else:
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
            except Exception:
                pass
        else:
            Handler.replyErr(sock)
