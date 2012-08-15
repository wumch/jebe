#coding:utf-8

import os, sys
sys.path.append(os.path.dirname(os.path.dirname(__file__)))
from config import logger, DEBUG
from riakstorer import RiakStorer

class MoveStorer(RiakStorer):

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
        key = self._genKey(info['url'])
        obj = self.bucket.get(key=key, r=self.R_VALUE)
        if not obj.exists():
            obj = self.bucket.get(key=key, r=self.R_VALUE_UP)
        exists = obj.exists()
        if 'ref' not in info:   # donot store in this case.
            return exists
        if exists:
            try:
                obj.set_data(str(int(obj.get_data()) + 1)).store()
            except Exception, e:
                logger.critical("failed no sotre web-moves: " + str(e.args))
            finally:
                return exists
        else:
            self.bucket.new_binary(key=key, data=str(1)).store()
            return exists
