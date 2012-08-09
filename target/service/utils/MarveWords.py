#!/usr/bin/env python
#coding:utf-8

_default_confidence = 0.8    # currently useless
_default_topN = 20

class MarveWords(object):

    def __init__(self, words):
        self.words = words

    def top(self, n = _default_topN):
        return [self.words[i][0] for i in xrange(0, min(n, len(self.words)))]

    def confidence(self, confidence = _default_confidence):
        return None
