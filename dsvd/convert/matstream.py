#coding:utf-8

import struct

class MatStream(object):

    def __init__(self, outfile, cols, transepose=False, input_col_begins=1):
        self.cols = cols
        self.m = self.n = self.nnz = 0
        self.outfile = outfile
        self.transpose = transepose
        self.input_col_begins = input_col_begins
        self.attached = 0
        self.curm = 0
        self._preapre()

    def _preapre(self):
        self.packer_uint64 = struct.Struct('L')
        self.packer_uint32 = struct.Struct('I')
        self.packer_double = struct.Struct('d')
        self.out = open(self.outfile, "wb")
        self.out.write(self.packer_uint64.pack(0) * 3)

    def attach(self, row, length=None):
        length = len(row) if length is None else length
        if length == 0:
            return
        self.nnz += length
        self.attached += 1
        for n, val in row:
            if self.transpose:
                self._appendCell(n - self.input_col_begins, self.curm, val)
            else:
                self._appendCell(self.curm, n - self.input_col_begins, val)
        self.curm += 1

    def _appendCell(self, m, n, val):
        self.out.write(self.packer_uint32.pack(m) +
            self.packer_uint32.pack(n) +
            self.packer_double.pack(val)
        )

    def __del__(self):
        self.out.seek(0, 0)
        self.out.write(self.packer_uint64.pack(self.cols) +
            self.packer_uint64.pack(self.curm) +
            self.packer_uint64.pack(self.nnz)
        )
        self.out.close()

if __name__ == '__main__':
    import sys
    outfile = "/tmp/matstream-test.mtx" if len(sys.argv) < 2 else sys.argv[1]
    stream = MatStream(outfile=outfile, cols=100, transepose=True, input_col_begins=1)
    docs = [
        [[1, 0.56], [32, 0.01], [13, 0.04]],
        [[1, 0.56], [32, 0.01], [13, 0.04]],
    ]
    for vec in docs:
        stream.attach(row=vec)
    print "matrix output to %s" % outfile
