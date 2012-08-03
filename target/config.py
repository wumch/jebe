#encoding:utf-8

DEBUG = True
from utils.natip import natip

class Config(object):

    router_port = 10010
    dealer_port = 10011
    routers = ({'host':'192.168.88.1', 'port':dealer_port},)

    riak_port = 8098
    riaks = ({'host':'192.168.88.1', 'port':riak_port},
        {'host':'192.168.88.2', 'port':riak_port},
        {'host':'192.168.88.3', 'port':riak_port},
        {'host':'192.168.88.4', 'port':riak_port},)

    tokenizers = ('http://192.168.88.2:10086/split',
         'http://192.168.88.4:10086/split',)

    iothreads = 4

    if DEBUG:
        routers = ({'host':natip, 'port':dealer_port},)
        riaks = ({'host':natip, 'port':riak_port}, )
        tokenizer = ('http://127.0.0.1:10086/split', )
        iothreads = 1

    __instance = None

    @classmethod
    def instance(cls):
        if cls.__instance is None:
            cls.__instance = Config()
        return cls.__instance

    def __init__(self):
        self.riak_cur = 0
        self.tokenizer_cur = 0

    def getRouter(self):
        return self.routers[0]

    def getRiak(self):
        self.riak_cur += 1
        if self.riak_cur >= len(self.riaks):
            self.riak_cur = 0
        return self.riaks[self.riak_cur]

    def getTokenizer(self):
        self.tokenizer_cur += 1
        if self.tokenizer_cur >= len(self.tokenizers):
            self.tokenizer_cur = 0
        return self.tokenizers[self.tokenizer_cur]

config = Config.instance()
