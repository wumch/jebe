#!/usr/bin/env python
#coding:utf-8

import sys
from config import config
from urllib2 import urlopen

# the degree of how explicit the res represents the original content.
# currently useless
_default_confidence = 0.8

_default_topN = 20

class MarveWords(object):

    def __init__(self, words=None, content=None):
        self.words = words
        self.content = content

    def top(self, n=_default_topN):
        self._prepare()
        return [self.words[i][0] for i in xrange(0, min(n, len(self.words) if self.words else 0))]

    def confidence(self, confidence = _default_confidence):
        self._prepare()
        raise NotImplementedError("<%s>.%s" % (self.__class__.__name__, sys._getframe().f_code.co_name))

    def _prepare(self):
        if self.words is None:
            if self.content is not None:
                self._genWordsFromContent()
            else:
                raise ValueError('kid, both words and content of <%s> are None.')

    def _genWordsFromContent(self):
        return config.jsonDecoder.decode(urlopen(config.getTokenizer('marve'), self.content, timeout=3).read())
