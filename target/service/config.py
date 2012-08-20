#encoding:utf-8

import os
DEBUG = not not os.getenv('JEBE_DEBUG', False)
from random import randint
from utils.natip import natip
from json import JSONDecoder, JSONEncoder
from utils.log import mklogger

_CHARSET = 'utf-8'

class JSONEr(object):

    __CHARSET = _CHARSET
    # some handy objects.
    jsonDecoder = JSONDecoder(encoding=__CHARSET)
    jsonEncoder = JSONEncoder(encoding=__CHARSET)

    def encode(self, obj):
        return self.jsonEncoder.encode(obj)

    def decode(self, string):
        return self.jsonDecoder.decode(string)

class Config(object):

    CHARSET = _CHARSET
    LOG_FILE = r'/var/log/crawler-server.log'
    TIME_ZONE = 'Asia/Shanghai'     # currently useless.

    router_port = 10010
    dealer_port = 10011
    routers = ({'host':'192.168.88.1', 'port':dealer_port},)

    riak_port = 8098
    riaks = ({'host':'192.168.88.1', 'port':riak_port, 'transport_options': {'timeout':3}},
        {'host':'192.168.88.2', 'port':riak_port, 'transport_options': {'timeout':3}},
        {'host':'192.168.88.3', 'port':riak_port, 'transport_options': {'timeout':3}},
        {'host':'192.168.88.4', 'port':riak_port, 'transport_options': {'timeout':3}},)
    bucks = {
        'loc' : {'buck':'ads', 'backend':'hdd2'},
        'mov' : {'buck':'mov', 'backend':'hdd3'},
        'usr' : {'buck':'usr', 'backend':'hdd4'},
        'ads' : {'buck':'ads', 'backend':'hdd4'},
    }

    tokenizers = ('http://192.168.88.2:10086/',
        'http://192.168.88.4:10086/',)

    iothreads = 4

    if DEBUG:       # for test on frank's asus laptop box.
        routers = ({'host':natip, 'port':dealer_port}, )
        riaks = ({'host':natip, 'port':riak_port}, )
        for k in bucks:
            bucks[k]['backend'] = 'leveldb'
        tokenizers = ('http://127.0.0.1:10086/', )
        iothreads = 1

    __instance = None

    @classmethod
    def instance(cls):
        if cls.__instance is None:
            cls.__instance = Config()
        return cls.__instance

    def __init__(self): pass

    def getRouter(self):
        return self.routers[0]

    def getRiak(self):
        return self.riaks[randint(0, len(self.riaks) - 1)]

    def getTokenizer(self, action):
        return self.tokenizers[randint(0, len(self.tokenizers) - 1)] + action

    jsoner = JSONEr()

class SysConfig(object):

    ERR_CODE_KEY_NAME = 'code'
    ERR_CODE_OK = 'ok'
    ERR_CODE_ERR = 'err'

    MAX_ADS = 3

config = Config.instance()
sysconfig = SysConfig()
logger = mklogger(config.LOG_FILE)
