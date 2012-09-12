#coding:utf-8

import gevent, gevent.queue
from config import NotImplementedException

def bind(maybe_self, maybe_bounded, maybe_callback=None):
    if maybe_callback is None:
        return maybe_bounded, maybe_self
    else:
        def func(*args, **kwargs):
            return maybe_bounded(*args, **kwargs)
        return maybe_callback, func

def recurveCallbackBounded(excute_func):
    def __caller(self_, return_address):
        def __work(*args, **kwargs):
            return return_address(excute_func(self_, *args, **kwargs))
        return __work
    return __caller

def recurveCallback(func):
    def __caller(maybe_self, maybe_back=None):
        back, call = bind(maybe_self, func, maybe_back)
        def __work(*args, **kwargs):
            return back(call(*args, **kwargs))
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

    def __init__(self, sock):
        self.queue = gevent.queue.Queue(1024)
        self.sock = sock

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
