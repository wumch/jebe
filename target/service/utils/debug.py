#coding:utf-8

import os, sys
import traceback

class BackTracer(object):

    class FrameInfo(object):
        def __init__(self, filepath=None, lineno=None, funcname=None, code=None):
            self.filepath, self.lineno, self.funcname, self.code = filepath, lineno, funcname, code
        def __repr__(self):
            return str([self.filepath, self.lineno, self.funcname])

    def __init__(self, max_depth=1000, funcname=True, filepath=True, lineno=True,
                 abspath=False, singleline=True, code=True, skip=1, extra_skip=0):
        self.max_depth = max_depth
        self.funcname = funcname
        self.filepath = filepath
        self.lineno = lineno
        self.abspath = abspath
        self.singleline = singleline
        self.code = code
        self.skip = skip
        self.extra_skip = extra_skip

    def trace(self, depth=None):
        return self._trace(depth=depth, sp=self.skip+self.extra_skip+1)

    def prety(self, depth=None):
        return (' <= ' if self.singleline else os.linesep).join(map(self.pretySingle, self._trace(depth=depth, sp=self.skip+self.extra_skip+1)))

    def pretySingle(self, frame_info):
        return frame_info.filepath + ':' + str(frame_info.lineno) + ':' + frame_info.funcname \
               + ("()" if frame_info.funcname != "<module>" else '') \
               + ('' if frame_info.code is None else ('{' + frame_info.code + '}'))

    def _trace(self, depth=None, sp=None):
        info = sys.exc_info()
        return self._traceNormal(depth=depth, sp=sp) if info[2] is None else self._traceException(depth=depth, tb=info[2])

    def _traceException(self, depth=None, tb=None):
        depth = self._getDepth(depth=depth)
        cur = 0
        path = []
        for filepath, lineno, funcname, code in traceback.extract_tb(tb=tb):
            path.append(self._genInfo(filepath=filepath, lineno=lineno, funcname=funcname, code=code))
            cur += 1
            if cur > depth:
                break
        return path

    def _traceNormal(self, depth=None, sp=None):
        sp = sp or self.skip + self.extra_skip
        depth = self._getDepth(depth=depth)
        frame = sys._getframe(sp)
        cur = 0
        path = []
        while frame is not None and cur < depth:
            path.append(self._genInfoFromFrame(frame))
            frame = frame.f_back
            cur += 1
        return path

    def _genInfoFromFrame(self, frame):
        fcode = frame.f_code
        return self._genInfo(filepath=fcode.co_filename, lineno=frame.f_lineno, funcname=fcode.co_name)

    def _genInfo(self, filepath, lineno, funcname, code=None):
        finfo = self.FrameInfo()
        if self.filepath:
            finfo.filepath = os.path.abspath(filepath) if self.abspath else filepath
        if self.lineno:
            finfo.lineno = lineno
        if self.funcname:
            finfo.funcname = funcname
        if self.code:
            finfo.code = code
        return finfo

    def _getDepth(self, depth=None):
        if depth is None:
            return self.max_depth
        elif self.max_depth is None:
            return 10
        else:
            return min(self.max_depth, depth)

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
            raise NotImplementedException()
    try:
        Test().caller()
    except Exception, e:
        print BackTracer(max_depth=2).prety()
        sys.exc_clear()
        print BackTracer(max_depth=10).prety()
