#!/usr/bin/env python
#coding:utf-8

"""
$    int    MAT_FILE_CLASSID
$    int    number of rows
$    int    number of columns
$    int    total number of nonzeros
$    int    *number nonzeros in each row
$    int    *column indices of all nonzeros (starting index is zero)
$    PetscScalar *values of all nonzeros
"""

import struct

class MatPets(object):

    MAT_FILE_CLASSID = 1211216  # NOTE: should keep consistent with PETSc.

    def __init__(self, outfile, cols, transepose=False, input_col_begins=1):
        self.cols = cols
        self.m = self.n = self.nnz = 0
        self.outfile = outfile
        self._preapre()

    def _preapre(self):
        self.packer_int32 = struct.Struct('i')
        self.packer_double = struct.Struct('d')
        self.out = open(self.outfile, "wb")
        self.out.write(self.packer_int32.pack(self.MAT_FILE_CLASSID))
        self.out.write(self.packer_int32.pack(0))
        self.out.write(self.packer_int32.pack(self.cols))
        self.out.write(self.packer_int32.pack(0))

    def attach(self, row, length=None):
        length = len(row) if length is None else length
        if length == 0:
            return

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
        if self.transpose:
            mn = self.packer_uint64.pack(self.cols) + self.packer_uint64.pack(self.curm)
        else:
            mn = self.packer_uint64.pack(self.curm) + self.packer_uint64.pack(self.cols)
        self.out.write(mn + self.packer_uint64.pack(self.nnz))
        self.out.close()

if __name__ == '__main__':
    import sys
    outfile = "data/matstream-test.mtx" if len(sys.argv) < 2 else sys.argv[1]
    stream = MatStream(outfile=outfile, cols=12, transepose=False, input_col_begins=1)
    docs = [
        [[1, 9.56], [3, 0.01], [9, 0.04]],
        [[2, 9.56], [5, 0.01], [7, 0.04]],
        [[2, 9.56], [8, 0.01], [11, 100.04]],
    ]
    for vec in docs:
        stream.attach(row=vec)
    print "matrix output to %s" % outfile
