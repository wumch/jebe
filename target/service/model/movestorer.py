#coding:utf-8

from config import logger, DEBUG
#from riakstorer import RiakStorer
from model.leveldbstorer import LevelDBStorer

class MoveStorer(LevelDBStorer):

    buck    = 'mov'      # web-moves
    backend = 'hdd3'
    if DEBUG:
        backend = 'leveldb'

    _instance = None

    @classmethod
    def instance(cls):
        if not isinstance(cls._instance, cls):
            cls._instance = cls()
        return cls._instance

    def __init__(self):
        super(MoveStorer, self).__init__()

    def exists(self, info):
        if DEBUG: return False
        return self._store(info)

    def _store(self, info):
        if 'url' not in info:
            return None     # `None` means param is wrong.
        key = self._encodeUrl(info['url'])
        count = self.getInt(key)
        exists = count > 0
        if 'ref' not in info:   # donot store in this case.
            return exists
        if exists:
            try:
                self.put(key, count + 1)
            except Exception, e:
                logger.critical("failed no sotre web-moves: " + str(e.args))
            finally:
                return exists
        else:
            self.put(key, 1)
            return exists
