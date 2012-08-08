#!/usr/bin/env python
#coding:utf-8

import os, sys
from chardet import detect

class FileConverter(object):

    encoding = 'utf-8'
    wrong = ('gbk', 'gb2312', 'gb18030', 'cp936', )

    def __init__(self, ifname, ofname):
        self.ifp = open(ifname, 'r')
        self.ofp = open(ofname, 'a')
        self.ofp.truncate(0)

    def convert(self):
        for line in self.ifp:
            encoding = self.isWrong(line)
#            print encoding, line[:50]
            if encoding is False:
                continue
            elif encoding is True:
                self.ofp.write(line)
            else:
                self.ofp.write(self.convertLine(line, encoding))
            self.ofp.write(os.linesep)

    def isWrong(self, line):
        try:
            encoding = detect(line[:100])['encoding'].lower()
        except Exception:
            return False
        if encoding == self.encoding:
            return True
        elif encoding in self.wrong:
            return encoding
        else:
            return False

    def convertLine(self, line, encoding):
        try:
            return line.decode(encoding).encode(self.encoding)
        except Exception:
            return 'still wrong'

class DirConverter(object):

    def __init__(self, indir, outdir):
        self.outdir = outdir
        self.ifiles = []
        self.ofiles = []
        if not os.path.exists(outdir):
            os.mkdir(outdir)
        for root, dirs, files in os.walk(indir):
            for name in files:
                ifile = os.path.join(root, name)
                self.ifiles.append(ifile)
                ofile = ifile.replace(indir, outdir)
                self.mkdir(os.path.dirname(ofile))
                self.ofiles.append(ofile)

    def mkdir(self, path):
        info = path[1:].split(os.sep)
        info[0] = path[0] + info[0]
        p = ''
        for i in range(0, len(info)):
            p = os.path.join(p, info[i])
            if not os.path.exists(path):
                os.mkdir(p)

    def convert(self):
        for i in range(0, len(self.ifiles)):
            FileConverter(self.ifiles[i], self.ofiles[i]).convert()
            print self.ifiles[i], 'done'


if __name__ == '__main__':
    converter = DirConverter(sys.argv[1], sys.argv[2])
    converter.convert()
    print 'all done'
