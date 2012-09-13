#coding:utf-8

import sys
import zmq, struct
from config import config, sysconfig
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
            self.handle(self.sock.recv())

    def handle(self, data):
        self._handle(data=data)
        self._finish()

    def _handle(self, data):
        self._pickHandler(data[0])(config.msgpack.decode(data[1:]))

    def _finish(self):
        self.sock.send(config.msgpack.encode(self.res))

    def _store(self, ad):
        print 'storing ad:'
        print ad
        self.res = self.OK

    def _error(self, *args):
        print >>sys.stderr, "error occured"
        self.res = self.ERR

    def _pickHandler(self, byte):
        action = self.actionPacker.unpack(byte)
        return getattr(self, '_' + self.actions[action]) if action in self.actions else self._error

def run():
    sock = sysconfig.zmq_context.socket(zmq.REP)
    addr = config.getAdCollector()
    sock.connect(addr)
    print "listen on", addr
    Handler(sock=sock).serve()

if __name__ == '__main__':
    run()
