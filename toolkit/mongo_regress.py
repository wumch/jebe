#coding:utf-8

import os, sys
src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'target', 'service')
if src_path not in sys.path:
    sys.path.append(src_path)
import time
import pymongo
from config import config, logger
from driversync.tokenizer import Tokenizer

class PageStorer(object):

    _instance = None

    @classmethod
    def instance(cls):
        if not isinstance(cls._instance, cls):
            cls._instance = cls()
        return cls._instance

    def __init__(self):
        self.mints = 1352304000     # 2012-11-08 00:00:00
        self.curts = 1352736000     # 2012-11-13 00:00:00
        self.curfinished = 0
        self.nextStop = 1000
        self.prepare()

    def prepare(self):
        self.tokenizer = Tokenizer.instance()
        self.connections = {}
        self.dbs = {}
        self.collections = {}
        for dbtype in ('loc', 'text'):
            server = config.getMongoDB(type=dbtype)
            # dbname is same as the collection name
            self.connections[dbtype] = pymongo.Connection(**server['param'])
            self.dbs[dbtype] = self.connections[dbtype][server['db']]
            self.collections[dbtype] = self.dbs[dbtype][server['collection']]
        self.textCursor = self.collections['text'].find().sort({"$natural":-1})

    def run(self):
        try:
            for doc in self.textCursor:
                self._store(doc)
                self.curfinished += 1
                if self.curfinished > self.nextStop:
                    logger.info("finished %d" % self.curfinished)
                    self.nextStop += 1000
                    time.sleep(0.1)
        except Exception, e:
            logger.logException(e)
        except:
            logger.error("unknown-exception")

    def _store(self, doc):
        if not ('_id' in doc and 'url' in doc and 'loc' in doc and 'links' in doc and 'title' in doc and 'text' in doc and 'ts' in doc
            and doc['ts'] >= self.mints):
            logger.error("doc format wrong: [%s]" % (doc['_id'] if '_id' in doc else "no-doc-id"))
            return
        wordsWeight = self._marve(doc['text'])
        del doc['text'], doc['links']
        doc['words'] = [ww[0] for ww in wordsWeight]
        doc['ts'] = self.curts
        self.collections['loc'].insert(doc)

    def _marve(self, content):
        return self.tokenizer.marve(content=content)

if __name__ == '__main__':
    PageStorer.instance().run()
