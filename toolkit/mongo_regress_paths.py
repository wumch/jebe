#coding:utf-8

import os, sys
src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'target', 'service')
if src_path not in sys.path:
    sys.path.append(src_path)
import time
import pymongo
from config import config, logger
from utils.urlparser import UrlParser

class Regressor(object):

    _instance = None
    urlParser = UrlParser()

    @classmethod
    def instance(cls):
        if not isinstance(cls._instance, cls):
            cls._instance = cls()
        return cls._instance

    def __init__(self):
        self.curfinished = 0
        self.nextStop = 10000
        self.prepare()

    def prepare(self):
        self.connections = {}
        self.dbs = {}
        self.collections = {}
        for dbtype in ('paths', 'paths_old', ):
            server = config.getMongoDB(type=dbtype)
            # dbname is same as the collection name
            self.connections[dbtype] = pymongo.Connection(**server['param'])
            self.dbs[dbtype] = self.connections[dbtype][server['db']]
            self.collections[dbtype] = self.dbs[dbtype][server['collection']]
        self.pathCursor = self.collections['paths_old'].find().sort("$natural", -1)

    def run(self, skip=0):
        curscaned = skip
        try:
            if skip:
                self.pathCursor.skip(skip)
            for doc in self.pathCursor:
                self._store(doc)
                curscaned += 1
                if self.curfinished >= self.nextStop:
                    logger.info("finished %d, total scaned: %d" % (self.curfinished, curscaned))
                    self.nextStop += 10000
                    time.sleep(0.1)
            return curscaned
        except Exception, e:
            logger.logException(e)
            return curscaned
        except:
            logger.error("unknown-exception")
            return curscaned

    def _store(self, doc):
        if '/' not in doc['_id']:
            return
        self.curfinished += 1

        paths = {
            '_id': self.urlParser.toPath(doc['url']) + '_' + doc['_id'],
            'url': doc['url'],
            'title':doc['title'],
        }
        self.collections['paths'].insert(paths)

if __name__ == '__main__':
    nextSkip = int(sys.argv[1]) if len(sys.argv) > 1 else 0
    while nextSkip is not None:
        nextSkip = Regressor().run(skip=nextSkip)
