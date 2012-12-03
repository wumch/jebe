#coding:utf-8

import os, sys
src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))), 'target', 'service')
if src_path not in sys.path:
    sys.path.append(src_path)
import time
import pymongo
from config import config, logger

class Supplier(object):

    def __init__(self):
        self.maxts = 1352304000     # 2012-11-08 00:00:00
        self.mints = self.maxts - (86400 * 7)    # one week ago
        self.curts = 1352736000     # 2012-11-13 00:00:00
        self.curfinished = 0
        self.nextStop = 1000
        self.curscaned = 0
        self.skip = 0
        self.prepare()

    def __iter__(self):
        self.curscaned = self.skip
        if self.skip:
            self.textCursor.skip(self.skip)
        for doc in self.textCursor:
            if self.check(doc=doc):
                yield doc
            self.curscaned += 1
            if self.curfinished >= self.nextStop:
                logger.info("finished %d, total scaned: %d" % (self.curfinished, self.curscaned))
                self.nextStop += 1000
                time.sleep(0.1)

    def check(self, doc):
        return 'url' in doc and 'title' in doc

    def prepare(self):
        self.connections = {}
        self.dbs = {}
        self.collections = {}
        for dbtype in ('loc', 'text', 'paths'):
            server = config.getMongoDB(type=dbtype)
            # dbname is same as the collection name
            self.connections[dbtype] = pymongo.Connection(**server['param'])
            self.dbs[dbtype] = self.connections[dbtype][server['db']]
            self.collections[dbtype] = self.dbs[dbtype][server['collection']]
        self.textCursor = self.collections['text'].find({}, ['url', 'title']).sort("$natural", -1)
