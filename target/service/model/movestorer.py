#coding:utf-8

from config import logger, sysconfig
#from riakstorer import RiakStorer
from leveldbstorer import LevelDBStorer

class MoveStorer(LevelDBStorer):

    _dbId = 'mov'      # web-moves

    _instance = None

    @classmethod
    def instance(cls):
        if not isinstance(cls._instance, cls):
            cls._instance = cls()
        return cls._instance

    def __init__(self):
        super(MoveStorer, self).__init__()

    def store(self, info):
        if 'url' not in info or info['url'] == ''       \
            'ref' not in info or info['ref'] == '':
            return None     # `None` means param is wrong.
        key = self._encodeUrl(info['url']) + sysconfig.MOVE_KEY_HYPHEN + self._encodeUrl(info['ref'])
        count = self.getInt(key)
        exists = count is not None
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
