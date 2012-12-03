#coding:utf-8

import os
from materials import cates

class Recorder(object):

    def __init__(self):
        self.counter = [0 for i in range(0, max(cates.keys()) + 2)]
        self.outfile = r'/hdd1/classify/materials.txt'
#        self.outfile = r'/tmp/materials.txt'
        self.fp = open(self.outfile, 'w')
        self.format = "%d\t%s" + os.linesep

    def record(self, doc, cate):
        self.counter[cate] += 1
        self.fp.write(self.format % (cate, doc['url']))
