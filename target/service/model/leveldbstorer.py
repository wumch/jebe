#coding:utf-8

import leveldb
from config import config, sysconfig
from utils.misc import strenc
from struct import Struct

class LevelDBStorer(object):

    _dbId = None
    _charset = config.CHARSET
    int32_packer = Struct('i')
    uint32_packer = Struct('I')
    int64_packer = Struct('l')
    uint64_packer = Struct('L')
    double_packer = Struct('d')

    def __init__(self, dbId=None, dbpath=None):
        self._dbpath = dbpath
        self._dbId = dbId or self.__class__._dbId
        if self._dbpath is None:
            if self._dbId is not None:
                info = config.dbs[self._dbId]
                self._dbpath = info['path']
        if self._dbpath is None:
            raise ValueError('kid, <%s>.dbpath is None' % self.__class__.__name__)
        self.db = leveldb.LevelDB(self._dbpath, create_if_missing=True)

    def get(self, key, raw_key=False, **kw):
        return self.getRaw(key, raw_key=raw_key, **kw)

    def put(self, key, value, raw_key=False, **kw):
#        assert value or isinstance(value, int), 'kid, do NOT put a false value into storage'
        self.db.Put(self._packKey(key, raw_key), self._pack(value, **kw), **kw)

    # TODO: should more strict/explicit
    def keyExists(self, key, raw_key=False, **kw):
        try:
            self.db.Get(self._packKey(key, raw_key=raw_key), **kw)
            return True
        except KeyError:
            return False

    def getRaw(self, key, raw_key=False, default=None, **kw):
        try:
            return self.db.Get(self._packKey(key, raw_key), **kw)
        except KeyError:
            return default

    def getInt(self, key, raw_key=False, bits=sysconfig.INT_BIT, unsigned=True, **kw):
        res = self.getRaw(key, raw_key=raw_key, **kw)
        return self._getIntPacker(bits=bits, unsigned=unsigned).unpack(res)[0] if res is not None else None

    def getStr(self, key, raw_key=False, **kw):
        return self.getRaw(key, raw_key=raw_key, **kw)

    def getUnicode(self, key, raw_key=False, encoding=_charset, **kw):
        return unicode(self.getRaw(key, raw_key=raw_key, **kw), encoding=encoding)

    def getAuto(self, key, raw_key=False, type='msgpacked', bits=None, unsigned=None, **kw):
        return self._unpack(self.getRaw(key, raw_key=raw_key, **kw), type=type, bits=bits, unsigned=unsigned)

    # NOTE: single process and single thread only.
    def incr(self, key, raw_key=False, step=1, bits=sysconfig.INT_BIT, unsigned=True, **kw):
        res = self.getInt(key, raw_key=raw_key, bits=bits, unsigned=unsigned) + step
        self.put(key, res, **kw)
        return res

    def _encodeUrl(self, url):
        return strenc(url)

    def _genUrlKey(self, url, raw_key=False):
        return self._packKey(self._encodeUrl(url), raw_key=raw_key)

    def _packKey(self, key, raw_key=False):
        return key if raw_key and isinstance(key, str) else self._pack(key)

    def _pack(self, var, bits=sysconfig.INT_BIT, unsigned=True, encoding=_charset, **useless):
        if isinstance(var, unicode):
            return var.encode(encoding)
        elif isinstance(var, basestring):
            return var
        elif isinstance(var, int):
            return self._getIntPacker(bits=bits, unsigned=unsigned).pack(var)
        elif isinstance(var, float):
            return self.double_packer.pack(var)
        else:
            return config.msgpack.encode(var)

    # TODO: what will happen while data is None?
    def _unpack(self, data, type=None, bits=None, unsigned=None, **useless):
        """
        @param type can be 'packed'
        """
        if data is None:
            return data
        if type == basestring:
            return data
        elif type == int:
            return self._getIntPacker(bits=bits, unsigned=unsigned).unpack(data)[0]
        elif type == float:
            return self.double_packer.unpack(data)[0]
        elif type == 'msgpacked':
            return config.msgpack.decode(data)
        elif type == 'packed':
            return config.packer.decode(data)
        elif type == 'json':
            return config.jsoner.decode(data)
        else:
            return data

    def _getIntPacker(self, bits=sysconfig.INT_BIT, unsigned=True):
        if bits == 32:
            return self.uint32_packer if unsigned else self.int32_packer
        elif bits == 64:
            return self.uint64_packer if unsigned else self.int64_packer
        else:
            raise TypeError('kid, int type <' + ' u'[unsigned] + 'int%d> is not allowed')
