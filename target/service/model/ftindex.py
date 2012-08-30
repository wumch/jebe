#coding:utf-8

from config import config, logger, DEBUG
from leveldbstorer import LevelDBStorer
from drivers.tokenizer import Tokenizer
from leveldb import WriteBatch

class FTIndex(LevelDBStorer):

    _dbId = 'fti'
    tokenizer = Tokenizer()
    _minMarve = 50          # otherwise will NOT store
    if DEBUG:
        _minMarve = 1

    _instance = None

    @classmethod
    def instance(cls):
        if cls._instance is None:
            cls._instance = cls()
        return cls._instance

    def __init__(self, **kw):
        super(FTIndex, self).__init__(**kw)

    def store(self, url=None, docId=None, content=None, words=None, **kw):
        docId = docId or self._encodeUrl(url)
        words = words or self._marveWords(content)
        if docId is None or words is None:
            return
        self._store(docId=docId, words=self._filterWords(words), **kw)

    def _store(self, docId, words, **kw):
        batch = WriteBatch()
        for w, c in words:
            key = w.encode(config.CHARSET)
            docs = self.getAuto(key, raw_key=True)
            pad = [docId, c]
            if docs is None:
                docs = [pad]
            else:
                docs.append(pad)
            from utils.misc import export
            export(docs)
            batch.Put(key, config.msgpack.encode(docs))
        self.db.Write(batch, **kw)

    # TODO: currently it's a fucking crazy implementation!
    def match(self, words, rate=None):
        unions = {}
        for w in words:
            docs = self.getAuto(w, raw_key=True)
            if docs is not None:
                for docId, m in docs:
                    if docId in unions:
                        unions[docId] += m
                    else:
                        unions[docId] = m
        minMarve = rate if rate else min(len(words) * 0.2, 1) * self._minMarve
        res = self._filterWords([[w, m] for w,m in unions.iteritems()], marve=minMarve)
        res.sort(lambda a,b: -cmp(a[1], b[1]))
        return res

    def _filterWords(self, words, marve=_minMarve):
        return filter(lambda wm: len(wm) == 2 and wm[1] >= marve, words)

    def _marveWords(self, content):
        if content is None:
            return None
        try:
            return self.tokenizer.marve(content=content)
        except Exception, e:
            logger.error(('kid, request to tokenizer/split with len(content)=%d failed: ' % len(content)) + str(e.args))
