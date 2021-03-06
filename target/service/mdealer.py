#!/usr/bin/env python

try:
    import psyco
    psyco.jit()
    from psyco.classes import *
except ImportError:
    pass

import sys
import zmq, struct
from config import config, DEBUG, natip
from controler.handler import Handler
from controler.mhcrawl import HCrawl

context = zmq.Context(1)
sock = context.socket(zmq.REP)
host = sys.argv[1] if len(sys.argv) > 1 else natip
sock.connect("tcp://%s:%d" % (host, config.dealer_port))

class Dealer(object):

    ucpacker = struct.Struct('B')
#    handlerList = [None, HPageExists(sock), HCrawl(sock), HShowAds(sock), HPageAccesser(sock)]
#    handlerList = [None, None, HCrawl(sock), None, None, None]
    handlerList = [None, 'pageExists', 'store', None, None, None]

    def __init__(self):
        self.handler = HCrawl(sock)

    def _getHandler(self, header):
        index = self.ucpacker.unpack(header[0])[0]
        if 0 < index < len(self.handlerList):
            return getattr(self.handler, self.handlerList[index])

    def handle(self, header, payload):
        handle = self._getHandler(header)
        if handle is None:
            global sock
            Handler.replyErr(sock)
        else:
            handle(payload)

if __name__ == '__main__':
    if DEBUG: print 'running in DEBUG mode'
    dealer = Dealer()
    while True:
        try:
            data = sock.recv_multipart()
        except zmq.core.error.ZMQError:
            Handler.replyErr(sock)
            data = sock.recv_multipart()
        if DEBUG: print "received", len(data)
        if len(data) > 1:
            try:
                dealer.handle(data[0], data[1:])
            except Exception:
                Handler.replyErr(sock)
        else:
            Handler.replyErr(sock)
