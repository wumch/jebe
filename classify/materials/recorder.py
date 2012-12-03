#coding:utf-8

import os
from materials import cates

class Recorder(object):

    def __init__(self, outfile):
        self.counter = [0 for i in range(0, max(cates.keys()) + 2)]
        self.outfile = outfile
#        self.outfile = r'/tmp/materials.txt'
        self.fp = open(self.outfile, 'w')
        self.format = "%d\t%s" + os.linesep

    def record(self, doc, cate):
        self.counter[cate] += 1
        self.fp.write(self.format % (cate, doc['url'].encode('utf-8')))
