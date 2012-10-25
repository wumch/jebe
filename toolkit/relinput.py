#!/usr/bin/env python
#coding:utf-8

import os, sys
src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'target', 'service')
if src_path not in sys.path:
    sys.path.append(src_path)
import zmq, struct
import pymongo
from config import config, sysconfig
from utils.natip import natip
from driversync.tokenizer import Tokenizer

class RelInput(object):

    calculater = "tcp://%s:10021" % natip
    actionPacker = struct.Struct('B')
    actions = {
        'tellTotal' : actionPacker.pack(1),
        'sendDoc' : actionPacker.pack(2),
        'thatSAll' : actionPacker.pack(99),
    }

    def __init__(self):
        self._prepare()

    def run(self):
        for doc in self._getDocFromMongo(self._calculateTotal()):
            print "send doc: ", doc
            print "res: ", self._sendDoc(doc)

    def _prepare(self):
        self.sock = sysconfig.zmq_context.socket(zmq.REQ)
        self.sock.connect(self.calculater)
        mongoServer = config.getMongoDB("text")
        self.mongoCon = pymongo.Connection(**mongoServer["param"])
        self.mongo = self.mongoCon[mongoServer['db']][mongoServer['collection']]
        self.tokenizer = Tokenizer.instance()

    def _getDoc(self):
        min_df = self._calculateTotal() * 0.1
        max_df = self._calculateTotal() * 0.9
        doc_useful = [["设计大赛", 3], ["朋克耳钉女", 1]]
        doc_useless = [["宝宝哈衣外贸原单", 100], ["男短袖衬衫特价", 1]]
        for i in xrange(0, self._calculateTotal()):
            if min_df < i < max_df:
                yield doc_useful
            else:
                yield doc_useless

    def _getDocFromMongo(self, max):
        cursor = self.mongo.find().limit(max).sort("$natural", pymongo.DESCENDING)
        for doc in cursor:
            words = self.tokenizer.count(doc["text"])
            if words:
                yield words


    def _calculateTotal(self):
        return min(self.mongo.count(), 10000)

    def tellTotal(self):
        return self._request('tellTotal', self._calculateTotal())

    def _sendDoc(self, words):
        return self._request('sendDoc', words)

    def thatSAll(self):
        return self._request('thatSAll')

    def _request(self, action, data=None):
        self._send(action=action, data=data)
        return config.msgpack.decode(self.sock.recv())

    def _send(self, action, data=None):
        self.sock.send(self.actions[action] if data is None else (self.actions[action] + config.msgpack.encode(data)))

def test():
    relinput = RelInput()
    print relinput.tellTotal()
    relinput.run()
    print relinput.thatSAll()

if __name__ == '__main__':
    test()