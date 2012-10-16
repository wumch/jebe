#!/usr/bin/env python
#coding:utf-8

import os, sys

class WordsDistribution(object):

    def __init__(self, infile, outfile, delimiter, headers, disperse_step=5):
        self.infile = infile
        self.outfile = outfile
        self.delimiter = delimiter
        self.headers = headers
        self.distep = float(disperse_step)

    def preprocess(self):
        ifp = open(self.infile, 'r')
        res = {}
        skiped = 0
        for line in ifp:
            if skiped < self.headers:
                skiped += 1
                continue
            useless, atime = self._parseLine(line=line)
            key = self.disperse(atime)
            if key not in res:
                res[key] = 1
            else:
                res[key] += 1
        ifp.close()
        ofp = open(self.outfile, 'w')
        for dispersed_atime, count in res.iteritems():
            ofp.write(('%d\t%d' + os.linesep) % (dispersed_atime, count))
        ofp.close()

    def _parseLine(self, line):
        info = line.split(self.delimiter)
        return info[0], int(info[1].replace(',', '') if ',' in info[1] else info[1])

    def disperse(self, val):
        return round(val / self.distep) * 5

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print "usage: %s <input-file> <output-file>" % sys.argv[0]
        sys.exit(1)
    ifile, ofile = sys.argv[1:3]
    WordsDistribution(
        infile=ifile,
        outfile=ofile,
        delimiter='\t',
        headers=0,
        disperse_step=5
    ).preprocess()
