#coding:utf-8

import os, sys
import codecs
import re

class HeaderConv(object):

    def __init__(self, root='.', h2hpp=False, rmold=False, from_coding='gbk', to_coding='utf-8'):
        self.h2hpp = h2hpp
        self.root = os.path.abspath(root)
        self.from_coding = from_coding
        self.to_coding = to_coding
        self.rmold = rmold
        self.include_regexp = re.compile(r'^\s*#\s*include\s*"(\s*\w*[a-z]*[A-Z]+[\w\.]*)\s*"')
        self.hregexp = re.compile(r'\.h(!pp)')

    def _prepare(self):
        pass

    def process(self):
        self._prepare()
        self._process()

    def _process(self):
        os.path.walk(self.root, self._handle, None)

    def _handle(self, useless, dir, flist):
        for item in flist:
            upath = os.path.join(dir, item)
            path = upath.lower()
            if os.path.isdir(upath):
                if upath != path:
                    os.rename(upath, path)
            elif os.path.isfile(upath):
                self._convContent(upath)

    def _convContent(self, infile):
        outfile = infile.lower()
        if self.h2hpp:
            if outfile.endswith('.h'):
                outfile += 'pp'
        if outfile[outfile.rfind('.'):] not in ('.hpp', '.cpp'):
            return
        try:
            ofp = codecs.open(outfile, 'w', encoding=self.to_coding)
            ifp = codecs.open(infile, 'r', encoding=self.from_coding)
            for line in ifp:
                matched = self.include_regexp.match(line)
                if matched:
                    header_file = matched.groups()
                    if header_file:
                        header_file = header_file[0]
                        if header_file.endswith('.h'):
                            line = line.replace(header_file, header_file.lower() + 'pp', 1)
                if line.endswith('.h'):
                    line += 'pp'
                ofp.write(line)
            ifp.close()
            ofp.close()
            if self.rmold:
                os.remove(infile)
        except UnicodeDecodeError, e:
            print "kid, error occured while converting ", infile, e.args
            sys.exit(1)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print "usage:", sys.argv[0], "<root-path> [h2hpp=0] [rmold=0]"
        sys.exit(1)
    def argconv(argidx, defaults=False):
        if len(sys.argv) <= argidx:
            return defaults
        return True if sys.argv[argidx].lower() in ('yes', 'true', '1', 'on') else False
    h2hpp = argconv(2)
    rmold = argconv(3)
    from_coding = 'gbk'
    to_coding = 'gbk'
    conver = HeaderConv(root=sys.argv[1], h2hpp=h2hpp, rmold=rmold, from_coding=from_coding, to_coding=to_coding)
    conver.process()
