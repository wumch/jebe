#coding:utf-8

import sys
import zmq, struct
from config import config
from controler.handler import Handler as BaseHandler

class Handler(BaseHandler):

    actionPacker = struct.Struct('B')
    actions = {
        11 : 'store',
    }

    def __init__(self, sock):
        super(Handler, self).__init__(sock=sock)
        self.res = self.OK

    def serve(self):
        while True:
            data = self.sock.recv()
            print data
            self.handle(data)

    def handle(self, data):
        print "data:", data
        self._handle(data=data)
        self._finish()

    def _handle(self, data):
        self._pickHandler(data[0])(config.msgpack.decode(data[1:]))

    def _finish(self):
        print "self.res:", self.res
        print "encode(self.res):", config.msgpack.encode(self.res)
        self.sock.send(config.msgpack.encode(self.res))
        print 'sent'

    def _store(self, ad):
        print 'storing ad:'
        print ad
        self.res = self.OK

    def _error(self, *args):
        print >>sys.stderr, "error occured"
        self.res = self.ERR

    def _pickHandler(self, byte):
        action = self.actionPacker.unpack(byte)[0]
        print 'action:[%d]' % action
        return getattr(self, '_' + self.actions[action]) if action in self.actions else self._error

def run():
    sock = zmq.Context(1).socket(zmq.REP)
    addr = config.getAdCollector()
    sock.bind(addr)
    print "listen on", addr
    Handler(sock=sock).serve()

if __name__ == '__main__':
    run()

'''
doc_1:{word_1:count_1, word_2:count_2, word_3:count_3...}
'''