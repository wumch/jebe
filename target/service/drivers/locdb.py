#coding:utf-8

if __name__ == '__main__':
    import os, sys
    src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
    if src_path not in sys.path:
        sys.path.append(src_path)

import struct
import zmq, msgpack
from config import config, sysconfig, logger
from utils.misc import crc32

class LocDB(object):

    actions = {
        'marve' : 11,
        'store' : 101,
    }

    def __init__(self):
        self.actionPacker = struct.Struct('B')
        self.packer = msgpack.Packer(encoding=config.CHARSET)
        self.unpacker = msgpack.Unpacker(encoding=config.CHARSET, use_list=True)
        self.createSocks()

    def createSocks(self):
        self.socks = []
        for uri in config.locdbs:
            sock = sysconfig.zmq_context.socket(zmq.REQ)
            sock.connect(uri)
            self.socks.append(sock)

    def marve(self, url):
        res = self.request(self.getSock(url), self.packer.pack(url), action='marve')
        if (not isinstance(res, basestring)) or (len(res) == 0):
            detect = ("len(%s)=%d" % (type(res), len(res))) if isinstance(res, basestring) else str(type(res))
            logger.error("kid, qdb response " + detect)
            return None
        self.unpacker.feed(res)
        return self.unpacker.unpack()

    def store(self, url, words):
        words_binary = str(self.packer.pack(self.decorateWords(words)))
        data = self.packer.pack(url) + self.packer.pack(words_binary)
        res = self.request(self.getSock(url), data, action='store')
        self.unpacker.feed(res)
        return self.unpacker.next()

    def decorateWords(self, words):
        if not all(map(lambda wm: isinstance(wm[0], str) and isinstance(wm[1], float), words)):
            words = [[w.encode(config.CHARSET) if isinstance(w, unicode) else str(w), float(m)] for w,m in words]
        words.sort(lambda a, b: cmp(a[1], b[1]), reverse=True)
        return words

    def request(self, sock, data, action):
        sock.send(self.actionPacker.pack(self.actions[action]) + data)
        return sock.recv()

    def getSock(self, url):
        return self.socks[self.urlHash(url)]

    def urlHash(self, url):
        return crc32(url) % len(self.socks)

    def __del__(self):
        for sock in self.socks:
            sock.close()

if __name__ == '__main__':
    from utils.misc import *
    from time import time
    url = 'http://www.jebe.com/page.utf8.html' if len(sys.argv) > 1 else sys.argv[1]
    times = int(sys.argv[2]) if len(sys.argv) > 2 else 1000
    locdb = LocDB()

    words = [['游戏d', 10.0], [u'充值d', 100.001], ['软件d',1009.0], ['手机', 10089]]
    words = locdb.decorateWords(words)

    def testWrite():
        begin = time()
        for i in xrange(0, times):
            locdb.store(url + str(i), words=words)
        consumed = time() - begin
        print "write %d entries in %d seconds" % (times, consumed)
        print "QPS:  ", (times / consumed) if consumed > 0 else 'infinite'

    print locdb.store(url, words)
    correct = locdb.marve(url)
    export(correct)

    def testRead():
        begin = time()
        for i in xrange(0, times):
            res = locdb.marve(url)
            assert res == correct, "got different results by same arguments."
        consumed = time() - begin
        print "read %d entries in %f seconds" % (times, consumed)
        print "QPS:  ", (times / consumed) if consumed > 0 else 'infinite'

    testRead()
