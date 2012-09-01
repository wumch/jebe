#coding:utf-8

import sys
import logging
from datetimes import now
from debug import BackTracer

# log level settings
RiakWriteError  = 'RiakWriteError'
RiakReadError   = 'RiakReadError'

class LogHandler(logging.FileHandler):

    def format(self, record):
        return now() + ' %(levelname)s %(filename)s:%(lineno)d %(funcName)s%(args)s %(msg)s' % record.__dict__

class Logger(logging.RootLogger):

    _instance = None

    @classmethod
    def instance(cls, logfile=None):
        if cls._instance is None and logfile is not None:
            cls._instance = cls(logfile=logfile)
        return cls._instance

    def __init__(self, logfile, backtracer=None):
        logging.RootLogger.__init__(self, logging.NOTSET)
        self.setLevel(logging.NOTSET)
        self.removeHandler(self.handlers)
        self.addHandler(LogHandler(filename=logfile))
        self.backtracer = backtracer or BackTracer(max_depth=10, extra_skip=2)

    def logException(self, e):
        self.error("kid, error occured: " + str(e.args))

    def error(self, msg, *args, **kwargs):
        logging.RootLogger.error(self, msg + ", call-chain: " + self.backtracer.prety())

def mklogger(logfile):
    logger = logging.getLogger()
    logger.setLevel(logging.NOTSET)
    logger.removeHandler(logger.handlers)
    logger.addHandler(LogHandler(filename=logfile))
    return logger

if __name__ == '__main__':
    import os, sys
    src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))))
    if src_path not in sys.path:
        sys.path.append(src_path)

    logfile='/tmp/test_logger.log'
    def test_logger():
        try:
            raise Exception("exception raised for test logger")
        except Exception, e:
            Logger.instance(logfile=logfile).logExceprion(e)
    test_logger()
    os.system("cat %s" % logfile)
