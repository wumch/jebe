#coding:utf-8

import os
from config import DEBUG

class FileStorer(object):

    prefix = '/ssd-data/crawler/'
    if DEBUG:
        prefix = '/tmp/crawler/'
    suffix = '.txt'

    def __init__(self):
        self.fp = open(self.genFileName(), 'a+')

    def store(self, content):
        self.fp.write(content)
        self.fp.write(os.linesep)

    def genFileName(self):
        if not os.path.exists(self.prefix):
            os.mkdir(self.prefix)
        basename = 1
        filename = self.prefix + str(basename) + self.suffix
        return filename
