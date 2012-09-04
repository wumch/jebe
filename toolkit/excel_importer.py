#!/usr/bin/env python
#coding:utf-8

import os, sys
src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'target', 'service')
if src_path not in sys.path:
   sys.path.append(src_path)
import codecs
import xlrd
from drivers.tokenizer import Tokenizer
from utils.misc import export
from config import config

class ExcelImporter(object):

    def __init__(self, xls, sheet=0, outfile=None, offset=0):
        self.xls = xlrd.open_workbook(filename=xls)
        self.tab = self.xls.sheet_by_index(sheet)
        self.outfile = outfile
        self.offset = offset
        self.aid = self.offset
        self.suffix = '.txt'
        self.marver = Tokenizer()
        self.ads = {}

    def imports(self):
        for i in xrange(1, self.tab.nrows):
            row = self.tab.row(i)
            self.handleRow(row)
        self.writeFile()

    def handleRow(self, row):
        text = row[1].value
        content = text + ''.join(filter(None, [c.value for c in row[3:]]))
        self.ads[self.aid] = {
            'id': self.aid,
            'link': row[2].value,
            'text' : text,
            'words': self.marver.marve(content=content),
        }
        print self.ads[self.aid]
        self.aid += 1

    def writeFile(self):
        def formatAd(aid):
            adstr = config.jsoner.encode(self.ads[aid])
            assert "'" not in adstr
            adstr = adstr.replace("'", "\'")
            return str(self.ads[aid]['id']) + ":'" + adstr + "',"
        content = 'ads = {' + os.linesep
        content += os.linesep.join(map(formatAd, self.ads))
        content += os.linesep + '}'
        out = codecs.open(filename=self.outfile, mode='w', encoding='utf-8')
        out.truncate()
        out.write(content)
        out.flush()

if __name__ == '__main__':
    argc = len(sys.argv)
    if len(sys.argv) < 2:
        print 'usage: %s <xlsx-file> [output-file] [ad-id-offset] [sheet-index]' % sys.argv[0]
        sys.exit(0)
    xls = sys.argv[1]
    outfile = '/tmp/ads' if argc < 3 else sys.argv[2]
    offset = 0 if argc < 4 else int(sys.argv[3])
    sheet = 0 if argc < 5 else int(sys.argv[4])
    importer = ExcelImporter(xls=xls, outfile=outfile, offset=offset, sheet=sheet)
    importer.imports()
