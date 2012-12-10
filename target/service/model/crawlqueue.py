#coding:utf-8

import pymongo, datetime
from config import config
from utils.misc import md5

class CrawlQueue(object):

    _instance = None

    crawl_status = {
        'initial' : 1,
        'crawling' : 2,
        'successed' : 3,
        'failed' : 4,
    }

    @classmethod
    def instance(cls):
        if not isinstance(cls._instance, cls):
            cls._instance = cls()
        return cls._instance

    def __init__(self):
        self.prepare()

    def detach(self):
        # NOTE: for better performance, should not find_and_modify()
        entry = self.collections['crawlqueue'].find_one()
        if entry:
            self.collections['crawlqueue'].remove({'_id':entry['_id']})
            return entry['url']
        return None

    def attach(self, url):
        md5ed_url = md5(url)
        if not self.exists(md5ed_url=md5ed_url):
            self._attach(url=url, md5ed_url=md5ed_url)
        return True

    def _attach(self, url, md5ed_url=None):
        self.collections['crawlqueue'].insert(self.genDoc(url=url, md5ed_url=md5ed_url))

    def genDoc(self, url, md5ed_url=None):
        return {
            '_id' : md5ed_url,
            'url' : url,
            'status' : datetime.datetime.now(),
        }

    def prepare(self):
        self.connections = {}
        self.dbs = {}
        self.collections = {}
        for dbtype in ('loc', 'crawlqueue'):
            server = config.getMongoDB(type=dbtype)
            # dbname is same as the collection name
            self.connections[dbtype] = pymongo.Connection(**server['param'])
            self.dbs[dbtype] = self.connections[dbtype][server['db']]
            self.collections[dbtype] = self.dbs[dbtype][server['collection']]

    def exists(self, md5ed_url):
        return self.doc_exists(md5ed_url=md5ed_url) or self.task_exists(md5ed_url=md5ed_url)

    def doc_exists(self, md5ed_url):
        return not not self.collections['loc'].find_one({'_id':md5ed_url})

    def task_exists(self, md5ed_url):
        return not not self.collections['loc'].find_one({'_id':md5ed_url})
