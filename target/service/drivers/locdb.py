#coding:utf-8

if __name__ == '__main__':
    import os, sys
    src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
    if src_path not in sys.path:
        sys.path.append(src_path)

import struct
from gevent_zeromq import zmq
import msgpack
from config import config, sysconfig, logger
from utils.misc import crc32
from zmqclient import *

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
            qsock = QueuedSock(sock)
            qsock.serve()
            self.socks.append(qsock)

    def marve(self, url, callback):
        return self.request(self.getSock(url), self.packer.pack(url), action='marve', callback=self._processMarve(callback))

    @recurveCallbackBounded
    def _processMarve(self, res):
        if (not isinstance(res, basestring)) or (len(res) == 0):
            detect = ("len(%s)=%d" % (type(res), len(res))) if isinstance(res, basestring) else str(type(res))
            logger.error("kid, qdb response " + detect)
            return None
        self.unpacker.feed(res)
        return self.unpacker.unpack()

    def store(self, url, words, callback):
        words_binary = str(self.packer.pack(self.decorateWords(words)))
        data = self.packer.pack(url) + self.packer.pack(words_binary)
        return self.request(self.getSock(url), data, action='store', callback=self._processStore(callback))

    @recurveCallbackBounded
    def _processStore(self, res):
        self.unpacker.feed(res)
        return self.unpacker.unpack()

    def decorateWords(self, words):
        if not all(map(lambda wm: isinstance(wm[0], str) and isinstance(wm[1], float), words)):
            words = [[w.encode(config.CHARSET) if isinstance(w, unicode) else str(w), float(m)] for w,m in words]
        words.sort(lambda a, b: cmp(a[1], b[1]), reverse=True)
        return words

    def request(self, sock, data, action, callback):
        return sock.send(self.actionPacker.pack(self.actions[action]) + data, callback=callback)
#        return sock.recv_multipart()

    def getSock(self, url):
        return self.socks[self.urlHash(url)]

    def urlHash(self, url):
        return crc32(url) % len(self.socks)

if __name__ == '__main__':
    from utils.misc import *
    from time import time
    url = sys.argv[1] if len(sys.argv) > 1 else 'http://www.jebe.com/page.utf8.html'
    times = int(sys.argv[2]) if len(sys.argv) > 2 else 1000
    locdb = LocDB()

    words = [['游戏d', 10.0], [u'充值d', 100.001], ['软件d',1009.0], ['手机', 10089]]
    words = locdb.decorateWords(words)

    def testWrite():
        begin = time()
        for i in xrange(0, times):
            locdb.store(url + str(i), words=words, callback=None)
        consumed = time() - begin
        print "write %d entries in %d seconds" % (times, consumed)
        print "QPS:  ", (times / consumed) if consumed > 0 else 'infinite'

#    testWrite()
#    sys.exit(0)

    def testRead():
        begin = time()
        for i in xrange(0, times):
            res = locdb.marve(url)
            assert res == correct, "got different results by same arguments."
        consumed = time() - begin
        print "read %d entries in %f seconds" % (times, consumed)
        print "QPS:  ", (times / consumed) if consumed > 0 else 'infinite'

    class Test(object):
        def callbaxk(self, res):
            print 'callbaxk(%s)' % res
            return res

        def domarve(self):
            correct = locdb.marve(url, callback=self.callbaxk)
            export(correct)

    jobs = [gevent.spawn(Test().domarve)]
    gevent.joinall(jobs)
