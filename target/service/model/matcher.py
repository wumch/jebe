#coding:utf-8

import zmq, struct
from config import config, logger
from utils.MarveWords import MarveWords
from pagestorer import PageStorer
from leveldbstorer import LevelDBStorer
from ftindex import FTIndex

# match by words
class Matcher(object):

    field = 'words'
    field = u'words'
    max_ads = 1

    _instance = None

    @classmethod
    def instance(cls):
        if cls._instance is None:
            cls._instance = cls()
        return cls._instance

    def __init__(self):
        self.finder = FTIndex.instance()
        self.ads = LevelDBStorer(dbId='ads')
        self.pageAccesser = zmq.Context(1).socket(zmq.REQ)
        self.pageAccesser.connect("tcp://%s:%d" % (config.getRouter()['host'], config.router_port))

    def match(self, words=None, content=None, loc=None):
        if words is None and content is None and loc is not None:
            splited_content = self._fetchSplitedContent(loc)
            if splited_content is None:
                return []
            ws = MarveWords(content=splited_content).top()
        else:
             ws = MarveWords(words=words, content=content).top()
        return self.search(ws)

    def search(self, words):
#        return [record.get().get_data() for record in self._search(words=words, buck=buck, field=field).run()]
        print 'pattern: [%s]' % words
        res = set()
        count = 0
        for docid, marve in self.finder.match(words=words):
            count += 1
            if count > self.max_ads:
                break
            res.add(docid)
        return [self.ads.getAuto(doc) for doc in res]

    def old_fetchSplitedContent(self, url):
        try:
            return PageStorer.instance().fetchSplitedContent(url=url)
        except Exception, e:
            logger.error(('%s retrieve content by url:[%s] failed: ' % (type(e).__name__, url)) + str(e.args))

    def _fetchSplitedContent(self, url):
        self.pageAccesser.send_multipart([struct.pack('B', 4), config.packer.encode(url)])
        resp = self.pageAccesser.recv_multipart()
        return config.packer.decode(resp[0])

    def pageExists(self, url):
        self.pageAccesser.send_multipart([struct.pack('B', 5), config.packer.encode(url)])
        resp = self.pageAccesser.recv_multipart()
        return not not config.packer.decode(resp[0])
