#!/usr/bin/env python

import zmq, sys
from natip import natip
import zlib, struct
import msgpack

context = zmq.Context()

client = context.socket(zmq.REP)
client.connect("tcp://%s:10011" % natip)

class Dealer(object):

    ucpacker = struct.Struct('B')     # ny
    actionList = [
        '',     # hold the index of `0`.
        # Ask server wheather page exists or not, and tell server where it's from.
        # Should carry enough data for targeting ad.
        'pageExists',
        # Send page content to crawler.
        'crawl',
        # receive responsed ads.
        'showAds',
    ]

    def __init__(self, _data):
        self.header, self.payload = _data

    def isCompressed(self):
        return self.ucpacker.unpack(self.payload[0])[0] == ord('y')

    def resolveAction(self):
        index = self.ucpacker.unpack(self.header[0])[0]
        assert 0 < index < len(self.actionList)
        return self.actionList[index]

    def dump(self):
        return msgpack.unpackb(zlib.decompress(self.payload[1:]) if self.isCompressed() else self.payload[1:])

if __name__ == '__main__':
    while True:
        data = client.recv_multipart()
        assert len(data) == 2
        dealer = Dealer(data)
        print dealer.resolveAction()
        print dealer.dump()
        client.send(r'ok, received!')
