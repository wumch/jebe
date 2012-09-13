#coding:utf-8

if __name__ == '__main__':
    import os, sys
    src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
    if src_path not in sys.path:
        sys.path.append(src_path)

import struct
from config import config, logger
from zmqclient import QueuedSock, recurveCallbackBounded

class Tokenizer(object):

    _SERVER_CHARSET = 'utf-8'

    actions = {
        'marve' : 11,
        'split' : 12,
        'count' : 13,
        'compare' : 14,
    }

    _instance = None

    @classmethod
    def instance(cls):
        """
        @return Tokenizer
        """
        if cls._instance is None:
            cls._instance = cls()
        return cls._instance

    def __init__(self):
        self.sock = QueuedSock(config.getTokenizer())
        self.actionPacker = struct.Struct('B')

    def marve(self, content, callback=None):
        return self.request(content=content, action='marve', callback=callback)

    def count(self, content, callback=None):
        return config.jsoner.decode(self.request(content=content, action='count', callback=callback))

    def split(self, content, callback=None):
        return self.request(content=content, action='split', callback=callback)

    def request(self, content, action, callback=None):
        try:
            data = self.actionPacker.pack(self.actions[action]) + (content.encode(self._SERVER_CHARSET) if isinstance(content, unicode) else content)
            self._request(data=data, callback=self._onResponse(callback))
#            self._request(data=data, callback=self._onResponse)
        except Exception, e:
            logger.error(('kid, request to tokenizer/split with len(content)=%d failed: ' % len(content)) + str(e.args))

    @recurveCallbackBounded
    def _onResponse(self, response):
        print response
        return config.msgpack.decode(response)

    def _request(self, data, callback):
        self.sock.send(data, callback=callback)

if __name__ == '__main__':
    from time import time
    from utils.misc import export
    tokenizer = Tokenizer.instance()
    testContent = (u'三个代表重要思想电影武侠客人' * 10000).encode(Tokenizer._SERVER_CHARSET)
    if len(sys.argv) == 1:
        print "usage: %s <content> [request-times=1]" % sys.argv[0]
        sys.exit(1)
    content = sys.argv[1] if sys.argv[1] != 'test' else testContent
    times = int(sys.argv[2]) if len(sys.argv) > 2 else 1
    res = tokenizer.marve(content=content)
    begin = time()
    for i in xrange(0, times):
        assert tokenizer.marve(content=content) == res, "requests with same parameter got different results"
    consumed = time() - begin
    print "finished %d requests in %f seconds" % (times, consumed)
    print "QPS: " + (str(times / consumed) if consumed > 0 else 'infinite')
    print "BPS: " + (str(times * len(testContent) / consumed) if consumed > 0 else 'infinite')
    print
    export(res)
