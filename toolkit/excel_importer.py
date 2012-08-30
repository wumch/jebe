#!/usr/bin/env python
#coding:utf-8

import os, sys
src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'target', 'service')
if src_path not in sys.path:
   sys.path.append(src_path)
import xlrd
from drivers.tokenizer import Tokenizer
from utils.misc import export
from importer import *

class ExcelImporter(object):

    def __init__(self, xls, sheet=0, outpath=None, offset=0):
        self.xls = xlrd.open_workbook(filename=xls)
        self.tab = self.xls.sheet_by_index(sheet)
        self.outpath = outpath
        self.offset = offset
        self.aid = self.offset
        self.suffix = '.txt'
        self.marver = Tokenizer()

    def imports(self):
        for i in xrange(1, self.tab.nrows):
            row = self.tab.row(i)
            self.handleRow(row)

    def handleRow(self, row):
        text = row[1].value
        content = text + ''.join(filter(None, [c.value for c in row[3:]]))
        print content
        ad = {
            'id': self.aid,
            'link': row[2].value,
            'text' : text,
            'words': self.marver.marve(content=content),
        }
#        outfile = os.path.join(self.outpath, "%d%s" % (self.aid, self.suffix))
#        out = codecs.open(filename=outfile, mode='w', encoding='utf-8')
        export(ad)
        importSingle(**ad)
        self.aid += 1

if __name__ == '__main__':
    argc = len(sys.argv)
    if len(sys.argv) < 2:
        print 'usage: %s <xlsx-file> [output-path] [offset] [sheet-index]' % sys.argv[0]
        sys.exit(0)
    xls = sys.argv[1]
    outpath = '/tmp/ads' if argc < 3 else sys.argv[2]
    offset = 0 if argc < 4 else int(sys.argv[3])
    sheet = 0 if argc < 5 else int(sys.argv[4])
    importer = ExcelImporter(xls=xls, outpath=outpath, offset=offset, sheet=sheet)
    importer.imports()
