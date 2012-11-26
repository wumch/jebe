#!/usr/bin/env python
#coding:utf-8

import os, sys
src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'target', 'service')
if src_path not in sys.path:
    sys.path.append(src_path)
import zmq, struct
import pymongo
from config import config, sysconfig

class ClusterInput(object):

    calculater = "tcp://10.10.11.37:10024"
    actionPacker = struct.Struct('B')
    actions = {
        'tell_config' : actionPacker.pack(1),
        'tell_total' : actionPacker.pack(2),
        'send_doc' : actionPacker.pack(10),
        'collected' : actionPacker.pack(11),
        'calculated' : actionPacker.pack(12),
        'thats_all' : actionPacker.pack(3),
    }

    def __init__(self):
        self._prepare()
        self.step = self._calculateTotal() // 100

    def run(self):
        cur = 0
        sent = 0
        total = 0
        for docid, words in self._getDoc(self._calculateTotal()):
            self._sendDoc(docid=docid, words=words)
            cur += 1
            total += 1
            if cur >= self.step:
                sent += 1
#                print "sent %d%% (%d)" % (sent, total)
                cur = 0
        print "all are sent: %d" % total

    def _prepare(self):
        self.sock = sysconfig.zmq_context.socket(zmq.REQ)
        self.sock.connect(self.calculater)
#        mongoServer = config.getMongoDB("text")
#        self.mongoCon = pymongo.Connection(**mongoServer["param"])
#        self.mongo = self.mongoCon[mongoServer['db']][mongoServer['collection']]
#        self.tokenizer = Tokenizer("tcp://192.168.1.8:10024")

    def _getDoc(self, max):
        min_df = self._calculateTotal() * 0.1
        max_df = self._calculateTotal() * 0.7
        cluster = [
            [[1, 0.3], [4, 0.5]],
            [[3, 0.1], [2, 0.6]],
            [[1, 0.1], [4, 0.6]],
        ]
        for i in xrange(0, max):
            cls = i % len(cluster)
            print i, '=>', cls + 1
            yield [i, cluster[cls]]

    def _getDocFromMongo(self, max):
        cursor = self.mongo.find().limit(max).sort("$natural", pymongo.DESCENDING)
        for doc in cursor:
            words = self.tokenizer.count(doc["text"])
            if words:
                yield words

    def _calculateTotal(self):
        return 10
        return min(self.mongo.count(), 1)

    def tellConfig(self):
        return self._request('tell_config', data=[1000, self._calculateTotal(), 2, 5])

    def tellTotal(self):
        return self._request('tell_total', data=self._calculateTotal())

    def _sendDoc(self, docid, words):
        return self._request('send_doc', docid=docid, words=words)

    def thatSAll(self):
        return self._request('thats_all')

    def _request(self, action, data=None, docid=None, words=None):
        self._send(action=action, data=data, docid=docid, words=words)
        return config.msgpack.decode(self.sock.recv())

    def _send(self, action, data=None, docid=None, words=None):
        if docid is None or words is None:
            if data is None:
                self.sock.send(self.actions[action])
            else:
                self.sock.send(self.actions[action] + config.msgpack.encode(data))
        else:
            data = config.msgpack.encode([docid, words]) # + config.msgpack.encode(words)
            self.sock.send(self.actions[action]  + data)

def test():
    clusterinput = ClusterInput()
    print clusterinput.tellConfig()
    clusterinput.run()
    print clusterinput.thatSAll()

if __name__ == '__main__':
    test()
