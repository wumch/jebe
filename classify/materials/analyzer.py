#coding:utf-8

from materials import *
from parser import Parser

class Analyzer(object):

    def __init__(self):
        self.parser = Parser()

    def analysis(self, doc):
        subject = self.parser.parse(doc)
        for cid, info in cates.iteritems():
            if 'cond' in info:
                if info['cond'](subject):
                    return cid
        return None
