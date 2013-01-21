#!/usr/bin/env python
#coding:utf-8

import os, sys
import numpy
from scipy import io
from scipy.sparse import lil_matrix as Matrix, csc_matrix
from sparsesvd import sparsesvd
import codecs

class Builder(object):

    def __init__(self, char_file, phrase_file, out_file, max_rows=None):
        self.char_file = char_file
        self.phrase_file = phrase_file
        self.out_file = out_file
        self.charid_map = {}
        self.idchar_map = []
        self.matrix = None
        self.max_rows = max_rows

    def build(self):
        self.build_map()
        self.build_matrix()

    def dump(self):
        self.dump_matrix()

    def build_map(self):
        fp = codecs.open(self.char_file, mode="rb", encoding="utf-8")
        chars = fp.read().strip()
        assert isinstance(chars, unicode)
        for i in xrange(0, len(chars)):
            self.charid_map[chars[i]] = i
            self.idchar_map.append(chars[i])
        print "char-map contains %d characters, max-col-index: %d" % (len(self.idchar_map), len(chars) - 1)

    def build_matrix(self):
        scan_rows = self.count_line(self.phrase_file) if self.max_rows is None else self.max_rows
        shape = (scan_rows, len(self.idchar_map))
        print "dim of matrix: %s" % str(shape)
        self.matrix = Matrix(shape, dtype=int)
        row = 0
        fp = codecs.open(self.phrase_file, mode='rb', encoding='utf-8')
        for line in fp:
            if row >= scan_rows:
                break
            counted = {}
            for c in line.strip():
                if c in self.charid_map:
                    if c not in counted:
                        counted[c] = 1
                    else:
                        counted[c] += 1
            for c, atimes in counted.iteritems():
                self.matrix[row, self.charid_map[c]] = atimes
            row += 1

    def dump_matrix(self):
        print "dumping to %s" % self.out_file
        io.mmwrite(self.out_file, self.matrix, comment="characters appear times in phrases", field='real')

    def count_line(self, fname):
        lines = 0
        for line in open(fname, 'rb'):
            lines += 1
        return lines

    def decompose(self):
        sigular_vals = 100
        print "decomposing, with %d singular-value requested." % sigular_vals
        ut, s, vt = sparsesvd(csc_matrix(self.matrix), sigular_vals)
        print "s*vt:", os.linesep, numpy.dot(s, vt)
        print "ut:", os.linesep, ut
        print "s:", os.linesep, s
        print "vt", os.linesep, vt

if __name__ == '__main__':
    if len(sys.argv) < 4:
        print "usage: %s <characters-file> <phrases-file> <output-file>" % sys.argv[0]
        sys.exit(1)
    out_file = sys.argv[3]
    if len(os.path.basename(out_file).split('.')) == 1:
        out_file += '.mtx'
    if os.path.exists(out_file):
        print "kid, output-file already exists: %s" % out_file
        sys.exit(1)
    max_rows = int(sys.argv[4]) if len(sys.argv) > 4 else None
    builder = Builder(char_file=sys.argv[1], phrase_file=sys.argv[2], out_file=out_file, max_rows=max_rows)
    builder.build()
    builder.dump()
    #builder.decompose()
