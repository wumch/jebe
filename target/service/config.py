#encoding:utf-8

import os, sys
DEBUG = not not os.getenv('JEBE_DEBUG', False)
from random import randint
from json import JSONDecoder, JSONEncoder
from utils.natip import natip
from utils.log import Logger
from utils.misc import MOVE_KEY_HYPHEN as _MK_HYPHEN
import zmq, msgpack
from utils.debug import *

_DEFAULT_CHARSET = 'utf-8'

class Packer(object):

    _CHARSET = _DEFAULT_CHARSET

    def encode(self, obj):
        raise NotImplementedError("<%s>.%s" % (self.__class__.__name__, sys._getframe().f_code.co_name))

    def decode(self, string, **kw):
        raise NotImplementedError("<%s>.%s" % (self.__class__.__name__, sys._getframe().f_code.co_name))

class JSONEr(Packer):

    # some handy objects.
    jsonDecoder = JSONDecoder(encoding=Packer._CHARSET)
    jsonEncoder = JSONEncoder(encoding=Packer._CHARSET)

    def encode(self, obj):
        return self.jsonEncoder.encode(obj)

    def decode(self, string):
        return self.jsonDecoder.decode(string)

class MsgPacker(Packer):

    packer = msgpack.Packer(default=None, encoding=Packer._CHARSET)
    unpacker = msgpack.Unpacker(encoding=Packer._CHARSET)

    def encode(self, obj):
        return self.packer.pack(obj)

    def decode(self, string, encoding=Packer._CHARSET, use_list=True, **kw):
        return msgpack.unpackb(string, encoding=encoding, use_list=use_list, **kw)

class Config(object):

    CHARSET = _DEFAULT_CHARSET
    LOG_FILE = r'/var/log/adsys.log'
    TIME_ZONE = 'Asia/Shanghai'     # currently useless.

    interal_api_host = '192.168.88.2'
    if DEBUG:
        interal_api_host = natip

    router_port = 10010
    dealer_port = 10011
    routers = ({'host':'192.168.88.1', 'port':dealer_port},)

    riak_port = 8098
    riaks = (
        {'host':'192.168.88.1', 'port':riak_port, 'transport_options': {'timeout':3}},
        {'host':'192.168.88.2', 'port':riak_port, 'transport_options': {'timeout':3}},
        {'host':'192.168.88.3', 'port':riak_port, 'transport_options': {'timeout':3}},
        {'host':'192.168.88.4', 'port':riak_port, 'transport_options': {'timeout':3}},)
    bucks = {
        'loc' : {'buck':'ads', 'backend':'hdd2'},
        'mov' : {'buck':'mov', 'backend':'hdd3'},
        'usr' : {'buck':'usr', 'backend':'hdd4'},
        'ads' : {'buck':'ads', 'backend':'hdd4'},
    }
    __dbpath_prefix = os.path.join('/', 'riak')
    _total_mem = (32 << 30)
    _2GB = (2 << 30) - 1
    dbs = {
        'mov' : {'path' : os.path.join(__dbpath_prefix, 'hdd1'), 'options': {'block_cache_size':_2GB, 'write_buffer_size':int(_total_mem * 0.025)}, },
        'loc' : {'path' : os.path.join(__dbpath_prefix, 'hdd2'), 'options': {'block_cache_size':_2GB, 'write_buffer_size':int(_total_mem * 0.025)}, },
        'ads' : {'path' : os.path.join(__dbpath_prefix, 'hdd3'), 'options': {'block_cache_size':(50 << 20),  'write_buffer_size':int(_total_mem * 0.01)}, },
        'idx' : {'path' : os.path.join(__dbpath_prefix, 'hdd4'), 'options': {'block_cache_size':_2GB, 'write_buffer_size':int(_total_mem * 0.025)}, },
    }
    for k in dbs:
        dbs[k]['path'] = os.path.join(dbs[k]['path'], k)
    dbs['fti'] = dbs['idx']

    ftengines = ('tcp://192.168.88.2:10016', 'tcp://192.168.88.3:10016', 'tcp://192.168.88.4:10016', )
    locdbs = (
        #'tcp://192.168.88.1:10051',
        'tcp://192.168.88.2:10018',
        'tcp://192.168.88.3:10018',
        'tcp://192.168.88.4:10018',
    )

    tokenizers = (
        'tcp://192.168.88.2:10015',
#        'tcp://192.168.88.3:10015',
        'tcp://192.168.88.4:10015',
    )

    mongodbs = {
        'loc' : {'host':'192.168.88.2', 'port':27017, 'max_pool_size':50, 'network_timeout':5, 'socketTimeoutMS':1000},
        'text' : {'host':'192.168.88.4', 'port':27017, 'max_pool_size':50, 'network_timeout':5, 'socketTimeoutMS':1000},
    }

    adcollectors = ('tcp://%s:10016' % interal_api_host, )

    iothreads = 1

    if DEBUG:       # for test on frank's asus laptop box.
        routers = ({'host':natip, 'port':dealer_port}, )
        riaks = ({'host':natip, 'port':riak_port}, )
        for k in dbs:
            dbs[k]['path'] = os.path.join('/', 'server', 'leveldb', k)
        tokenizers = ('tcp://%s:10015' % natip, )
        ftengines = ('tcp://%s:10016' % natip, )
        locdbs = ('tcp://%s:10018' % natip, )
        adcollectors = ('tcp://%s:10017' % natip, )
        mongodbs = {
           'text' : {'host':'127.0.0.1', 'port':27017, 'max_pool_size':50, 'network_timeout':1, 'socketTimeoutMS':200},
           'loc' : {'host':'127.0.0.1', 'port':27017, 'max_pool_size':50, 'network_timeout':1, 'socketTimeoutMS':200},
        }
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

    # NOTE: random is not fast enough (about 2000,000 times per second)
    def getTokenizer(self):
        if isinstance(self.ftengines, basestring):
            return self.tokenizers
        elif len(self.ftengines) == 1:
            return self.tokenizers[0]
        else:
            return self.tokenizers[randint(0, len(self.tokenizers) - 1)]

    def getFTEngine(self):
        if isinstance(self.ftengines, basestring):
            return self.ftengines
        elif len(self.ftengines) == 1:
            return self.ftengines[0]
        else:
            return self.ftengines[randint(0, len(self.ftengines) - 1)]

    def getLocDB(self):
        if isinstance(self.locdbs, basestring):
            return self.locdbs
        elif len(self.locdbs) == 1:
            return self.locdbs[0]
        else:
            return self.locdbs[randint(0, len(self.locdbs) - 1)]

    def getAdCollector(self):
        if isinstance(self.adcollectors, basestring):
            return self.adcollectors
        elif len(self.adcollectors) == 1:
            return self.adcollectors[0]
        else:
            return self.adcollectors[randint(0, len(self.adcollectors) - 1)]

    def getMongoDB(self, type='loc'):
        assert type in self.mongodbs, "kid, what do you what by `%s`" % type
        return self.mongodbs[type]

    jsoner = JSONEr()
    msgpack = MsgPacker()
    packer = msgpack

class SysConfig(object):

    INT_BIT = 64

    ERR_CODE_KEY_NAME = 'code'
    ERR_CODE_OK = 'ok'
    ERR_CODE_ERR = 'err'
    RPC_KEY_NAME = 'callback'

    MOVE_KEY_HYPHEN = _MK_HYPHEN

    RPC_FUNC_NAME = {
        'showAds'      : 'i8vars.showAds',
        'crawlPage'    : 'i8vars.crawlPage',
        'kwOfLoc'      : 'i8vars.baidu_words',
    }

    MAX_ADS = 1
    MAX_KW_OF_LOC = 20

    _zmq_context = None
    def __getattr__(self, item):
        if item == 'zmq_context':
            if self.__class__._zmq_context is None and self._zmq_context is None:
                self.__class__._zmq_context = zmq.Context(Config.iothreads)
            return self._zmq_context
        raise KeyError("<%s> doesn't has attribute '%s'" % (self.__class__.__name__, item))

config = Config.instance()
sysconfig = SysConfig()
logger = Logger.instance(logfile=config.LOG_FILE, also_print=DEBUG)
backtracer = BackTracer(max_depth=10)
