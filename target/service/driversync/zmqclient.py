#coding:utf-8

import gevent, gevent.queue
from gevent_zeromq import zmq
from config import sysconfig, NotImplementedException

def bind(func, *args, **kwargs):
    def __callee(*a, **kw):
        kw.update(kwargs)
        func(*(args + a), **kw)
    return __callee

def recurveCallbackBounded(excute_func):
    def __caller(self_, return_address):
        def __work(*args, **kwargs):
            return return_address(excute_func(self_, *args, **kwargs))
        return __work
    return __caller

class QueueFailFlag(object): pass

QUEUE_FAILED = QueueFailFlag()

class QueueItem(object):
    def __init__(self, send_data, callback):
        self.send_data = send_data
        self.callback = callback

class QueuedSock(object):

    failflag = QUEUE_FAILED
    MAX_SERVE = 50000
    QUEUE_SIZE = 2048

    def __init__(self, uri, context=sysconfig.zmq_context, socktype=zmq.REQ):
        self.uri = uri
        self.context = context
        self.socktype = socktype
        self.served = 0
        self._createSock()

    def _createSock(self):
        if hasattr(self, 'sock'):
            self.sock.close()
        self.sock = self.context.socket(self.socktype)
        self.sock.connect(self.uri)
        self.queue = gevent.queue.Queue(self.QUEUE_SIZE)
        self.serve()

    def emptyCallback(self, *args, **kwargs):
        pass

    def send(self, data, callback):
        if self.queue.full():
            return callback(self.failflag) if callback else None
        else:
            return self.queue.put_nowait(QueueItem(data, callback or self.emptyCallback))

    def recv(self):
        raise NotImplementedException(this=self)

    def serve(self):
        gevent.spawn(self._run)

    def _run(self):
        while True:
            item = self.queue.get()
            self.sock.send(item.send_data)
            data = self.sock.recv()
            item.callback(data)

    def __del__(self):
        self.sock.close()
