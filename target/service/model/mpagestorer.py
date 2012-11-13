#coding:utf-8

import time
import pymongo
from config import config, sysconfig
from driversync.tokenizer import Tokenizer
from utils.misc import md5
from utils.urlparser import UrlParser

class PageStorer(object):

    _instance = None
    urlParser = UrlParser()

    @classmethod
    def instance(cls):
        if not isinstance(cls._instance, cls):
            cls._instance = cls()
        return cls._instance

    def __init__(self):
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

    def store(self, meta, content):
        links, content = self._parseContent(content=content)
        return self._store(url=meta['url'], content=content, links=links)

    def storeInfo(self, url, title, content, links=None):
        return self._store(url=url, title=title, content=content, links=links or [])

    def _store(self, url, title, content, links=None):
        md5_res = md5(url)
        time_stamp = int(time.time())
        # it's better to examine in "loc", but to scatter pressure...
        entry = self.collections['loc'].find_one(md5_res, ['ts'])

        willUpdate = False
        if isinstance(entry, dict):
            willUpdate = True if 'ts' not in entry else (entry['ts'] + sysconfig.CRAWL_UPDATE_INTERVAL < time_stamp)
            if not willUpdate:
                return

        path = self.urlParser.toPath(url)
        text = self._genTextData(url=url, title=title, content=content, path=path, links=links, md5_res=md5_res, time_stamp=time_stamp)
        if willUpdate:
            del text['_id']
            self.collections['text'].update(spec={'_id':entry['_id']}, document=text, upsert=False)
        else:
            self.collections['text'].insert(text)

        loc = self._genLocData(content=content, md5_res=md5_res, time_stamp=time_stamp)
        if willUpdate:
            del loc['_id']
            self.collections['loc'].update(spec={'_id':entry['_id']}, document=loc, upsert=False)
        else:
            self.collections['loc'].insert(loc)

    def _parseContent(self, content):
        info = content.split("\t", 1)
        return [config.jsoner.decode(info[0]), info[1]] if len(info) == 2 else [[], content]

    def exists(self, url):
        return self._exists(md5ed_url=md5(url))

    def _exists(self, md5ed_url):
        return not not self.collections['loc'].find_one({'_id':md5ed_url})

    def _genTextData(self, url, content, path, links, title, md5_res=None, time_stamp=None):
        linksDict = {}
        for href, text in links:
            key = md5(href)
            linksDict[key] = {'url':href, 'text': text}
        return {
            '_id' : md5_res or md5(url),
            'ts' : time_stamp or int(time.time()),
            'url' : url,
            'loc' : path,
            'title' : title,
            'text' : content,
            'links' : linksDict,
        }

    def _genLocData(self, content, md5_res=None, time_stamp=None):
        wordsWeight = self._marve(content)
        return {
            '_id' : md5_res,
            'ts' : int(time.time()) or time_stamp,
            'words' : [ww[0] for ww in wordsWeight],
#            'weight' : [ww[1] for ww in wordsWeight],
        }

    def _marve(self, content):
        return self.tokenizer.marve(content=content)
