#coding:utf-8

import os, sys

class BackTracer(object):

    class FrameInfo(object):
        def __init__(self, filepath=None, lineno=None, funcname=None):
            self.filepath, self.lineno, self.funcname = filepath, lineno, funcname
        def __repr__(self):
            return str([self.filepath, self.lineno, self.funcname])

    def __init__(self, max_depth=1000, funcname=True, filepath=True, lineno=True, abspath=False, singleline=True, skip=1, extra_skip=0):
        self.max_depth = max_depth
        self.funcname = funcname
        self.filepath = filepath
        self.lineno = lineno
        self.abspath = abspath
        self.singleline = singleline
        self.skip = skip
        self.extra_skip = extra_skip

    def trace(self, depth=None):
        return self._trace(depth=depth, sp=self.skip+self.extra_skip+1)

    def prety(self, depth=None):
        return ('<=' if self.singleline else os.linesep).join(map(self.pretySingle, self._trace(depth=depth, sp=self.skip+self.extra_skip+1)))

    def pretySingle(self, frame_info):
        return frame_info.filepath + ':' + str(frame_info.lineno) + ':' + frame_info.funcname + ("()" if frame_info.funcname != "<module>" else '')

    def _trace(self, depth=None, sp=None):
        sp = sp or self.skip + self.extra_skip
        depth = self.max_depth if depth is None else depth
        frame = sys._getframe(sp)
        cur = 0
        path = []
        while frame is not None and cur < depth:
            path.append(self._getInfo(frame))
            frame = frame.f_back
            cur += 1
        return path

    def _getInfo(self, frame):
        fcode = frame.f_code
        finfo = self.FrameInfo()
        if self.filepath:
            finfo.filepath = os.path.abspath(fcode.co_filename) if self.abspath else fcode.co_filename
        if self.lineno:
            finfo.lineno = frame.f_lineno
        if self.funcname:
            finfo.funcname = fcode.co_name
        return finfo

class NotImplementedException(NotImplementedError):
    def __init__(self, this=None):
        caller_frame = sys._getframe(1)
        if this is None and 'self' in caller_frame.f_locals:
            this = caller_frame.f_locals['self']
        super(NotImplementedException, self).__init__("<%s>.%s" % (this.__class__.__name__, caller_frame.f_code.co_name))

if __name__ == '__main__':
    class Test():
        def caller(self):
            self.callee()
        def callee(self):
            print BackTracer(max_depth=2).prety()
            raise NotImplementedException()
    Test().callee()
