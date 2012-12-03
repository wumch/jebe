#coding:utf-8

from analyzer import *
from supplier import Supplier
from recorder import Recorder

class Judger(object):

    def __init__(self):
        self.supplier = Supplier()
        self.analyzer = Analyzer()
        self.recorder = Recorder()

    def run(self):
        for doc in self.supplier:
            cid = self.judge(doc=doc)
            if cid is not None:
                self.recorder.record(doc=doc, cate=cid)

    def judge(self, doc):
        return self.analyzer.analysis(doc)

if __name__ == '__main__':
    Judger().run()
