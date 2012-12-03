#coding:utf-8

from materials import cates

class Recorder(object):

    def __init__(self):
        self.counter = [0 for i in range(0, max(cates.keys()) + 2)]

    def record(self, doc, cate):
        self.counter[cate] += 1
        print "%d\t%s" % (cate, doc['url'])
