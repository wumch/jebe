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
        for dbtype in ('loc', 'text', 'paths'):
            server = config.getMongoDB(type=dbtype)
            # dbname is same as the collection name
            self.connections[dbtype] = pymongo.Connection(**server['param'])
            self.dbs[dbtype] = self.connections[dbtype][server['db']]
            self.collections[dbtype] = self.dbs[dbtype][server['collection']]
        self.textCursor = self.collections['text'].find().sort("$natural", -1)

    def run(self, skip=0):
        try:
            nextSkip = skip
            if skip:
                self.textCursor.skip(skip)
            for doc in self.textCursor:
                self._store(doc)
                if self.curfinished >= self.nextStop:
                    logger.info("finished %d, nextskip:%d" % (self.curfinished, nextSkip + self.curfinished))
                    self.nextStop += 1000
#                    time.sleep(0.1)
        except Exception, e:
            logger.logException(e)
        except:
            logger.error("unknown-exception")

    def _store(self, doc):
        if not ('_id' in doc and 'url' in doc and 'loc' in doc and 'links' in doc and 'title' in doc and 'text' in doc and 'ts' in doc
            and doc['ts'] >= self.mints):
            logger.error("doc format wrong: [%s]" % (doc['_id'] if '_id' in doc else "no-doc-id"))
            return
        self.curfinished += 1

        paths = {
            '_id': doc['_id'],
            'url': doc['url'],
            'title':doc['title'],
        }
        del doc['text'], doc['links'], doc['title'], doc['loc'], doc['url']
        doc['words'] = [ww[0] for ww in self._marve(doc['text'])]
        doc['ts'] = self.curts
        self.collections['loc'].insert(doc)

        paths['words'] = [ww[0] for ww in self._marve(paths['title'])]
        self.collections['paths'].inert(paths)

    def _marve(self, content):
        return self.tokenizer.marve(content=content)

if __name__ == '__main__':
    if len(sys.argv) > 1:
        PageStorer.instance().run(skip=int(sys.argv[1]))