#coding:utf-8

if __name__ == '__main__':
    import os, sys
    src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
    if src_path not in sys.path:
        sys.path.append(src_path)
import struct
import zmq, msgpack
from config import config, sysconfig

class FTEngine(object):

    actions = {
        'match' : 10
    }

    def __init__(self):
        self.actionPacker = struct.Struct('B')
        self.packer = msgpack.Packer(encoding=config.CHARSET)
        self.unpacker = msgpack.Unpacker(encoding=config.CHARSET)
        self.sock = sysconfig.zmq_context.socket(zmq.REQ)
        self.sock.connect(config.getFTEngine())

    def match(self, words):
        return self.request(words=self.decorateWords(words), action='match')

    def matchDecorated(self, decoratedWords):
        return self.request(words=decoratedWords, action='match')

    def decorateWords(self, words):
        if words is None:
            return []
        if not all(map(lambda wm: isinstance(wm[0], str) and isinstance(wm[1], float), words)):
            return [[w.encode(config.CHARSET) if isinstance(w, unicode) else str(w), float(m)] for w,m in words]
        return words

    def request(self, words, action='match'):
        data = self.actionPacker.pack(self.actions[action]) + self.packWordWeightList(words=words)
        self.sock.send(data)
        msg = self.sock.recv()
        self.unpacker.feed(msg)
        return [doc for doc in self.unpacker]

    def packWordWeightList(self, words):
        return self.packer.pack(words)

    def packWordWeight(self, word, weight):
        return self.packer.pack(word) + self.packer.pack(weight)

    def __del__(self):
        self.sock.close()

if __name__ == '__main__':
    from utils.misc import *
    from time import clock
    times = int(sys.argv[1]) if len(sys.argv) > 1 else 1000
    ftengine = FTEngine()
    words = [['游戏d', 10.0], [u'充值d', 100.001], ['软件d',1009.0], ['手机', 10089]]
    words = ftengine.decorateWords(words)
    correct = ftengine.matchDecorated(words)
    begin = clock()
    for i in xrange(0, times):
        res = ftengine.matchDecorated(words)
        assert res == correct, "got different results by same arguments."
    consumed = clock() - begin
    print "time: ", consumed
    print "QPS:  ", (times / consumed) if consumed > 0 else 'infinite'
#    if res is None:
#        print 'return None'
#    else:
#        export(res)
