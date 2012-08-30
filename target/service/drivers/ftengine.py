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
        return self.request(words=words, action='match')

    def request(self, words, action='match'):
        data = self.actionPacker.pack(self.actions[action]) + self.packWordWeightList(words=words)
        print data[1]
        self.sock.send(data)
        msg = self.sock.recv()
        self.unpacker.feed(msg)
        return [doc for doc in self.unpacker]

    def packWordWeightList(self, words):
        return self.packer.pack([self.packWordWeight(*w) for w in words])

    def packWordWeight(self, word, weight):
        return self.packer.pack(word) + self.packer.pack(weight)

if __name__ == '__main__':
    from utils.misc import *
    ftengine = FTEngine()
    words = [['a', 10.0], ['晕倒', 100.0000000000000000000000001]]
    res = ftengine.match(words)
    export(res)
