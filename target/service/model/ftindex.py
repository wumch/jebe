#coding:utf-8

from config import config, logger, DEBUG
from leveldbstorer import LevelDBStorer
from tokenizer import Tokenizer
from leveldb import WriteBatch

class FTIndex(LevelDBStorer):

    _dbId = 'fti'
    tokenizer = Tokenizer()
    _minMarve = 50          # otherwise will NOT store
    if DEBUG:
        _minMarve = 1

    def __init__(self, **kw):
        super(FTIndex, self).__init__(**kw)

    def store(self, url=None, docId=None, content=None, words=None):
        docId = docId or self._encodeUrl(url)
        words = words or self._marveWords(content)
        if docId is None or words is None:
            return
        self._store(docId=docId, words=self._filterWords(words))

    def _store(self, docId, words):
        batch = WriteBatch()
        for w, c in words:
            key = w.encode(config.CHARSET)
            docs = self.getAuto(key, raw_key=True)
            pad = [docId, c]
            if docs is None:
                docs = [pad]
            else:
                docs.append(pad)
            batch.Put(key, config.msgpack.encode(docs))
        self.db.Write(batch)

    # TODO: currently it's a fucking crazy implementation!
    def match(self, words, rate=None):
        unions = {}
        print 'minMarve', self._minMarve
        print words
        for w in words:
            print type(w), w
            docs = self.getAuto(w, raw_key=True)
            print 'docs:', docs
            if docs is not None:
                for docId, m in docs:
                    if docId in unions:
                        unions[docId] += m
                    else:
                        unions[docId] = m
        minMarve = rate if rate else min(len(words) * 0.2, 1) * self._minMarve
        return self._filterWords([[w, m] for w,m in unions.iteritems()], marve=minMarve)

    def _filterWords(self, words, marve=_minMarve):
        return filter(lambda wm: len(wm) == 2 and wm[1] >= marve, words)

    def _marveWords(self, content):
        if content is None:
            return None
        try:
            return self.tokenizer.marve(content=content)
        except Exception, e:
            logger.error(('kid, request to tokenizer/split with len(content)=%d failed: ' % len(content)) + str(e.args))
